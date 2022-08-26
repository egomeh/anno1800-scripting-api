
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        string islandName = "lillypaddy";
        string resourceName = "beer";

        Telegraph telegraph = new Telegraph();

        ulong time;
        ulong frame;
        telegraph.GetGameTime(out time, out frame);

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        var oldWorld = (from area in areas where area == 0xBF37 select area).FirstOrDefault();

        List<IslandInfo> islands;
        telegraph.GetWorldIslands(oldWorld, true, out islands);

        var lillypaddy = (from island in islands where island.name.Equals(islandName) select island).FirstOrDefault();

        List<IslandResource> resources;
        telegraph.GetIslandResources(oldWorld, lillypaddy.island_id, out resources);

        var value = (from resource in resources where resource.name.Equals(resourceName) select resource).FirstOrDefault();

        List<ResourceConsumption> consumption;
        telegraph.GetIslandResidentialConsumption(oldWorld, lillypaddy.island_id, out consumption);

        var resourceConsumption = (from c in consumption where c.name.Equals(resourceName) select c.rate).FirstOrDefault();

        Console.WriteLine($"{islandName} has {value.amount} of {resourceName} consumed at a rate of {resourceConsumption} tonnes per minute");
    }
}
