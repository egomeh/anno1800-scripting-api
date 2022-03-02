
TestRunner.Run();

class TestRunner
{
    public static void Run()
    {
        var object1 = new Ship();
        object1.name = "ooooookay lets go";
        var object2 = new SomeListyBoi();
        object2.ids.Add(2);
        object2.ids.Add(1);
        object2.ids.Add(3);

        List<byte> buffer = new List<byte>();

        Serializer.Serialize(object1, buffer);
        Serializer.Serialize(object2, buffer);

        object1 = new Ship();
        object2 = new SomeListyBoi();

        int offsetAfter;
        Serializer.Deserialize(out object1, buffer.ToArray(), 0, out offsetAfter);
        Serializer.Deserialize(out object2, buffer.ToArray(), offsetAfter, out offsetAfter);

        Telegraph telegraph = new Telegraph();
        List<Coordinate> waypoints;
        telegraph.GetShipWaypoints("my message", 0x0000000200000FF8, out waypoints);

        Console.WriteLine(String.Format("{0}, {1}", waypoints.First().x, waypoints.First().y));
    }
}
