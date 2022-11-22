
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        string islandName = "lillypaddy";
        string resourceName = "bricks";

        Telegraph telegraph = new Telegraph();

        ulong gametime;
        ulong frame;
        telegraph.GetGameTime(out gametime, out frame);

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        uint OldWorldId = 0xBF37;

        ulong OldWorldAddress;
        telegraph.DebugGetAreaAddress(OldWorldId, out OldWorldAddress);

        List<IslandInfo> islands;
        telegraph.GetWorldIslands(OldWorldId, true, out islands);

        var lillypaddy = (from island in islands where island.name.Equals(islandName) select island).FirstOrDefault();

        List<IslandResource> resources;
        telegraph.GetIslandResources(OldWorldId, lillypaddy.island_id, out resources);

        var value = (from resource in resources where resource.name.Equals(resourceName) select resource).FirstOrDefault();

        List<ulong> vehicleLists;
        telegraph.DebugGetVehicleLists(out vehicleLists);

        //List<ResourceConsumption> consumption;
        //telegraph.GetIslandResidentialConsumption(oldWorld, lillypaddy.island_id, out consumption);

        //var resourceConsumption = (from c in consumption where c.name.Equals(resourceName) select c.rate).FirstOrDefault();

        //// List<ulong> buildings;
        //// telegraph.DebugGetIslandBuildingAddresses(oldWorld, lillypaddy.island_id, out buildings);

        //Console.WriteLine($"{islandName} has {value.amount} of {resourceName} consumed at a rate of {resourceConsumption} tonnes per minute");
    }
}
