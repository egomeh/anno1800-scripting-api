
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        //ulong areaAddress;

        //ulong islandAddress = 0x2135D4A8160;

        //List<IslandResource> resources;
        //telegraph.DebugGetIslandResources(islandAddress, out resources);

        //bstring islandName;
        //telegraph.DebugGetIslandNameFromAddress(islandAddress, out islandName);

        //List<IslandInfo> info;
        //telegraph.DebugGetIslandChainFromAddress(islandAddress, out info);

        //Console.WriteLine(info.First().name);

        while (true)
        {
            ulong areaAddress;
            telegraph.DebugGetFirstAreaStructAddress(out areaAddress);
        }

        // int amount = resources.Where(x => { return x.type_id == 1010200; }).First().amount;
    }
}
