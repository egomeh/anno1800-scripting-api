
ResourceExporter.Run();

class ResourceExporter
{
    string AreaToName(uint id)
    {
        if (id == 0xb604)
            return "Enbesa";

        if (id == 0xbf37)
            return "Old World";

        if (id == 0xbf39)
            return "New World";

        if (id == 0xbf47)
            return "Arctic";

        return "Don't konw ?_?";
    }

    public static void Run()
    {
        ResourceExporter exporter = new ResourceExporter();
        exporter.RunInternal();
    }

    public void RunInternal()
    {
        Telegraph telegraph = new Telegraph();

        List<string> issues = new List<string>();

        string output = "{\n";

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        output += "    areas:\n    [\n";

        foreach (uint area in areas)
        {
            List<IslandInfo> islands;
            telegraph.GetPlayerIslandsInWorld(area, out islands);

            output += "        {\n";

            string areaName = AreaToName(area);

            output += string.Format("            area: \"{0}\",\n", areaName);

            output += string.Format("            islands:\n");
            output += string.Format("            [\n");

            foreach (var island in islands)
            {
                output += string.Format("                {{\n");
                output += string.Format("                    name: \"{0}\",\n", island.name);

                output += string.Format("                    resources:\n", island.name);
                output += string.Format("                    [\n", island.name);

                List<IslandResource> resources;
                telegraph.GetIslandResources(area, island.island_id, out resources);

                List<ResourceConsumption> consumption;
                telegraph.GetIslandResidentialConsumption(area, island.island_id, out consumption);

                List<ResourceConsumption> conversion;
                telegraph.GetIslandIndustrialConversion(area, island.island_id, out conversion);

                foreach (IslandResource resource in resources)
                {
                    float residential_rate = 0.0f;
                    float industrial_rate = 0.0f;

                    foreach (ResourceConsumption rc in consumption)
                    {
                        if (rc.type_id != resource.type_id)
                            continue;

                        if (rc.rate == 0.0f)
                            continue;

                        residential_rate = rc.rate;
                    }

                    foreach (ResourceConsumption rc in conversion)
                    {
                        if (rc.type_id != resource.type_id)
                            continue;

                        if (rc.rate == 0)
                            continue;

                        industrial_rate = rc.rate;
                    }

                    bool shouldShow = residential_rate != 0.0f;
                    shouldShow |= residential_rate != 0.0f;
                    shouldShow |= resource.amount > 0;

                    if (!shouldShow)
                        continue;

                    output += string.Format("                        {{\n", resource.name);
                    output += string.Format("                            name: \"{0}\",\n", resource.name);
                    output += string.Format("                            amount: {0},\n", resource.amount);
                    output += string.Format("                            capacity: {0},\n", resource.capacity);

                    if (residential_rate != 0.0f)
                        output += string.Format("                            residential_conversion: {0:0.00},\n", residential_rate);

                    if (industrial_rate != 0)
                        output += string.Format("                            industrial_conversion: {0:0.00},\n", industrial_rate);

                    if (residential_rate + industrial_rate > 0.0f)
                        issues.Add(String.Format("{0} under-produces {1}", island.name, resource.name));

                    output += string.Format("                        }},\n", resource.name);
                }

                output += string.Format("                    ]\n", island.name);

                output += string.Format("                }},\n");
            }

            output += string.Format("            ]\n");

            output += "        },\n";
        }

        output += "    ]\n";

        output += "}\n";

        Console.Write(output);

        foreach (string issue in issues)
        {
            Console.WriteLine(issue);
        }
    }
}
