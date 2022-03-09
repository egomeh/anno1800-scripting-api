
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        ulong time;
        ulong frame;
        telegraph.GetGameTime(out time, out frame);

        IslandResource fishOnMusli;
        telegraph.DebugGetResourceInfoFromAddress(0, out fishOnMusli);

        Console.WriteLine(String.Format("{0} {1}", time, frame));       
    }
}
