

Serializer serializer = new Serializer();


Coordinate c = new Coordinate();
c.x = 5.4f;
c.y = 1.23f;

List<byte> buffer = new List<byte>();

serializer.Serialize(c, buffer);

Coordinate c2;

int offsetAfter;
serializer.Deserialize(out c2, buffer.ToArray(), 0, out offsetAfter);

Console.WriteLine("hello world");
