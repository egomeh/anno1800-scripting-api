class Program
{
    static void Main(string[] args)
    {

        TestRunner.Run(0);
        //if (args.Length == 0)
        //{
        //    Console.WriteLine("Please provide an integer in hex as a command line argument.");
        //    return;
        //}

        //foreach (string hexString in args)
        //{
        //    try
        //    {
        //        uint number = Convert.ToUInt32(hexString, 16);
        //        TestRunner.Run(number);
        //    }
        //    catch (FormatException)
        //    {
        //        Console.WriteLine($"Invalid hex string: {hexString}");
        //    }
        //    catch (OverflowException)
        //    {
        //        Console.WriteLine($"The hex string is too large to convert to an integer: {hexString}");
        //    }
        //}
    }
}

class TestRunner
{
    public static void Run(uint id)
    {
        Telegraph telegraph = new Telegraph();

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        List<IslandInfo> islands;
        telegraph.GetWorldIslands(areas[1], true, out islands);

        List<ResourceConsumption> industrial_consumption;
        telegraph.GetIslandIndustrialConversion(areas[1], islands[1].island_id, out industrial_consumption);

        //string GrainName;
        //telegraph.DebugGetNameFromGuid(id, out GrainName);

        //Console.WriteLine("{0}", GrainName);

        return;
    }
}

