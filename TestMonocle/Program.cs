
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();
        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        Console.WriteLine("Starting !");
        for (int i = 0; i < areas.Count; i++)
        {
            Console.WriteLine(areas[i]);
            List<IslandInfo> islands;
            telegraph.GetPlayerIslandsInWorld(areas[i], out islands);

            for (int i0 = 0; i0 < islands.Count; i0++)
            {
                Console.WriteLine("Island : " + islands[i0].name);
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
