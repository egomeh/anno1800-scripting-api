
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        Telegraph telegraph = new Telegraph();

        List<uint> areas;
        telegraph.GetAllAreas(out areas);

        foreach (uint area in areas)
        {
            List<IslandInfo> islands;
            telegraph.GetWorldIslands(area, true, out islands);

            foreach (var island in islands)
            {
                Console.WriteLine(String.Format("{0} {1}", island.island_id, island.name));
            }
        }
    }
}
