
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph(TelegramMode.Testing);

        while (true)
        {
            ulong address;
            telegraph.DebugGetFirstAreaStructAddress(out address);
        }
    }
}
