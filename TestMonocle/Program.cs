
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        List<IslandInfo> islands;
        // ulong address;
        telegraph.GetPlayerIslandsInWorld(0xBF37, out islands);
    }
}
