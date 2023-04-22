
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        string sourceislandName = "lillypaddy";
        string targetislandName = "Shortisland";
        string resourceName = "bricks";
        uint transfer_amount = 2500;

        Telegraph telegraph = new Telegraph();

        ///ulong gametime;
        //ulong frame;
        //telegraph.GetGameTime(out gametime, out frame);

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        List<IslandInfo> Islands;
        telegraph.GetWorldIslands(areas[1], true, out Islands);

        return;

        //uint OldWorldId = 0xBF37;

        //ulong OldWorldAddress;
        //telegraph.DebugGetAreaAddress(OldWorldId, out OldWorldAddress);

        //List<IslandInfo> islands;
        //telegraph.GetWorldIslands(OldWorldId, true, out islands);

        //var sourceisland = (from island in islands where island.name.Equals(sourceislandName) select island).FirstOrDefault();
        //var targetisland = (from island in islands where island.name.Equals(targetislandName) select island).FirstOrDefault();

        ////List<IslandResource> resources;
        ////telegraph.GetIslandResources(OldWorldId, lillypaddy.island_id, out resources);

        ////var value = (from resource in resources where resource.name.Equals(resourceName) select resource).FirstOrDefault();

        //List<ulong> vehicleLists;
        //telegraph.DebugGetVehicleLists(out vehicleLists);

        //List<ShipInfo> ships;
        //telegraph.GetShipsInRegion(OldWorldId, out ships);

        //List<DebugComponent> components;
        //telegraph.DebugGetEntityComponets(ships[0].debug_address, out components);

        //ulong time_start = 0;
        //ulong frame_start = 0;
        //bool started = false;

        //uint _temp;
        //telegraph.SetIslandResource(OldWorldId, sourceisland.island_id, 0xf6a1d, 250, out _temp);
        //telegraph.SetIslandResource(OldWorldId, targetisland.island_id, 0xf6a1d, 250, out _temp);

        //uint goods_transferred = 0;
        //while (goods_transferred < transfer_amount)
        //{
        //    uint previousBricksSource;
        //    telegraph.SetIslandResource(OldWorldId, sourceisland.island_id, 0xf6a1d, 250, out previousBricksSource);

        //    if (!started && previousBricksSource < 250)
        //    {
        //        started = true;
        //        telegraph.GetGameTime(out time_start, out frame_start);
        //        Console.WriteLine("Starting the clock!");
        //    }

        //    uint previousBricksTarget;
        //    telegraph.SetIslandResource(OldWorldId, targetisland.island_id, 0xf6a1d, 250, out previousBricksTarget);

        //    uint diff = previousBricksTarget - 250;

        //    goods_transferred += diff;

        //    Thread.Sleep(1000);
        //}


        //ulong time_end;
        //ulong frame_end;
        //telegraph.GetGameTime(out time_end, out frame_end);

        //ulong minutesSpent = (time_end - time_start) / (1000 * 60);

        //Console.WriteLine(String.Format("Transferred {0} bricks in {1} min", transfer_amount, minutesSpent));

        // List<ResourceConsumption> consumption;
        // telegraph.GetIslandResidentialConsumption(oldWorld, lillypaddy.island_id, out consumption);

        // var resourceConsumption = (from c in consumption where c.name.Equals(resourceName) select c.rate).FirstOrDefault();

        // List<ulong> buildings;
        // telegraph.DebugGetIslandBuildingAddresses(oldWorld, lillypaddy.island_id, out buildings);

        // Console.WriteLine($"{islandName} has {value.amount} of {resourceName} consumed at a rate of {resourceConsumption} tonnes per minute");
    }
}

