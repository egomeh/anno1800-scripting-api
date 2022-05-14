
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new();
        // Get all areas (Old World, New World, Cape Trelawney, Enbessa, Arctic)
        telegraph.GetAllAreas(out List<uint> areas);

        // Read all areas
        for (int i = 0; i < areas.Count; i++)
        {
            // Print area
            Console.WriteLine(areas[i]);

            // Get islands of player into this area
            telegraph.GetPlayerIslandsInWorld(areas[i], out List<IslandInfo> islands);

            // Read all islands
            for (int i0 = 0; i0 < islands.Count; i0++)
            {
                // Print island
                Console.WriteLine("Island of player : " + islands[i0].name + " : " + islands[i0].debug_address);
                telegraph.GetIslandResources(areas[i], islands[i0].island_id, out List<IslandResource> resources);

                for (int i1 = 0; i1 < resources.Count; i1++)
                {
                    if (resources[i1].amount <= 0)
                        continue;
                    Console.WriteLine
                        ("Amount : " + resources[i1].amount);
                    Console.WriteLine
                        ("Amount pointer : " + resources[i1].amount_ptr);
                    Console.WriteLine
                        ("Name : " + resources[i1].name);
                    Console.WriteLine
                        ("TypeId : " + resources[i1].type_id);
                    Console.WriteLine
                        ("Capacity : " + resources[i1].capacity);
                }
            }

            // Get all islands of this area
            telegraph.GetAllIslandsOfWorld(areas[i], out List<IslandInfo> islands0);

            // Read all islands
            for (int i0 = 0; i0 < islands0.Count; i0++)
            {
                // Print island
                Console.WriteLine("Island : " + islands0[i0].name + " : " + islands0[i0].debug_address);
            }
        }

        // telegraph.DebugTryEnqueueShipForTrade(areas[0], 0x2482, 0x0000015C18E578A8, 0x0000015C18E578A8);

        //List<ulong> buildings;
        //telegraph.DebugGetIslandBuildingAddresses(0xBF37, 0x2482, out buildings);

        //List<ResourceConsumption> consumption;
        //telegraph.GetIslandIndustrialConversion(0xBF37, 0x2242, out consumption);

        //for (ulong i = 0; i < 2048; ++i)
        //{
        //    ulong address;
        //    telegraph.DebugVirtualGetComponentFromAddress(0x1da13b55278, i, out address);

        //    if (address == 0)
        //        continue;

        //    Console.WriteLine(String.Format("{0} {1:X}", i, address));
        //}
    }
}
