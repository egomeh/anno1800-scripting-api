
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        string GrainName;
        telegraph.DebugGetNameFromGuid(0xF6A10, out GrainName);

        List<uint> Areas;
        telegraph.GetAllAreas(out Areas);

        string AreaName;
        telegraph.DebugGetNameFromGuid(Areas[1], out AreaName);

        List<IslandInfo> Islands;
        telegraph.GetWorldIslands(Areas[1], true, out Islands);

        List<IslandResource> IslandsResource;
        telegraph.GetIslandResources(Areas[1], Islands[1].island_id, out IslandsResource);

        foreach (IslandResource res in IslandsResource)
        {
            if (res.name.Equals("Potatoes"))
            {
                Console.WriteLine(String.Format("musli has {0} potatoes", res.amount));
            }
        }

        List<ResourceConsumption> resco;
        telegraph.GetIslandResidentialConsumption(Areas[1], Islands[1].island_id, out resco);

        List<ResourceConsumption> convertion;
        telegraph.GetIslandIndustrialConversion(Areas[1], Islands[1].island_id, out convertion);

        return;
    }
}

