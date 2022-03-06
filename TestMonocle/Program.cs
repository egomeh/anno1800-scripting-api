
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        ulong time;
        ulong frame;
        telegraph.GetGameTime(out time, out frame);

        Console.WriteLine(String.Format("{0} {1}", time, frame));
    }
}
