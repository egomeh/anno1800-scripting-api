
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        string sourceislandName = "lillypaddy";
        string targetislandName = "Shortisland";
        string resourceName = "bricks";
        uint transfer_amount = 2500;

        Telegraph telegraph = new Telegraph();

        string GrainName;
        telegraph.DebugGetNameFromGuid(0xF6A10, out GrainName);

        List<uint> Areas;
        telegraph.GetAllAreas(out Areas);

        string AreaName;
        telegraph.DebugGetNameFromGuid(Areas[1], out AreaName);

        return;
    }
}

