
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        string islandName = "Shortisland";
        string resourceName = "fish";

        Telegraph telegraph = new Telegraph();

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        uint oldWorld = (from area in areas where area == 0xBF37 select area).FirstOrDefault();

        List<IslandInfo> islands;
        telegraph.GetWorldIslands(oldWorld, true, out islands);

        IslandInfo ourIsland = (from island in islands where island.name.Equals(islandName) select island).FirstOrDefault();

        List<IslandResource> resources;
        telegraph.GetIslandResources(oldWorld, ourIsland.island_id, out resources);

        var amount = (from resource in resources where resource.name.Equals(resourceName) select resource.amount).FirstOrDefault();

        List<ResourceConsumption> consumptions;
        telegraph.GetIslandResidentialConsumption(oldWorld, ourIsland.island_id, out consumptions);

        var rate = (from consumption in consumptions where consumption.name.Equals(resourceName) select consumption.rate).FirstOrDefault();

        Console.WriteLine("{0} has {1} tonnes of {2} consumed at a rate of {3} tonnes per minute", islandName, amount, resourceName, rate);
    }
}
