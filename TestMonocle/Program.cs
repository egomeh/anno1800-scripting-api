
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        ulong time;
        ulong frame;
        telegraph.GetGameTime(out time, out frame);

        List<IslandResource> resourceChain;
        telegraph.DebugGetResourceChainInfoFromAddress(0x26DEB5243A0, out resourceChain);

        int amount = resourceChain.Where( x => { return x.type_id == 1010227; } ).First().amount;

        Console.WriteLine(String.Format("Amount: {0}", amount));
    }
}
