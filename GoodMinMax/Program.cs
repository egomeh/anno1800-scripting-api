
using System.Threading;

GoodsMinMax.Run();

class GoodsMinMax
{
    private static bool keepRunning = true;

    public static void Run()
    {
        //Console.CancelKeyPress += delegate (object sender, ConsoleCancelEventArgs e) {
        //    e.Cancel = true;
        //    GoodsMinMax.keepRunning = false;
        //};

        Console.WriteLine("Drawing wand...");
        Console.WriteLine("Yelling \"Resourcius minimalus maximus\"");
        Telegraph telegraph = new Telegraph();
        Console.WriteLine("Hoping things are working :S");

        while (keepRunning)
        {
            List<uint> areas;
            telegraph.GetAllAreas(out areas);

            foreach (uint area in areas)
            {
                List<IslandInfo> islands;
                telegraph.GetWorldIslands(area, true, out islands);

                foreach (IslandInfo island in islands)
                {
                    telegraph.MinMaxResourcesOnIsland(area, island.island_id, 50, 50);
                }
            }

            Thread.Sleep(1000);
        }
    }
}
