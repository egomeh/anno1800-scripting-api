
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        while (true)
        {
            ulong address;
            telegraph.DebugGetAreaWithCode(0xBF37, out address);
        }
    }
}
