
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

        return;
    }
}

