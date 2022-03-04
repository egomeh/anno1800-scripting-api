
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph(TelegramMode.Testing);
        telegraph.WriteMessageBox("Hello from telegraph!");

        ulong time;
        ulong frame;
        telegraph.GetGameTime(out time, out frame);
    }
}
