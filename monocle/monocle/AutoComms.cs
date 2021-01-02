using System;
using System.Collections.Generic;

namespace monocle
{
    public enum Area : ulong
    {
        Invalid = 0,
        OldWorld = 1,
        NewWorld = 2,
        Arctic = 3,
    };

    public enum ShipType : ulong
    {
        Invalid = 0,
        Schooner = 1,
        Gunboat = 2,
        Frigate = 3,
        Clipper = 4,
        ShipOfTheLine = 5,
        OilTanker = 6,
        CargoShip = 7,
        Monitor = 8,
        Battlecruiser = 9,
        CommandShip = 10,
    };

    public struct GameTime
    {
        public UInt64 time;
        public UInt64 frame;
    }

    public struct Coordinate
    {
        public float x;
        public float y;
    }

    public struct ShipMoveData
    {
        public bool moving;
        public Coordinate position;
        public List<Coordinate> waypoints;
    }

    public struct ShipData
    {
        public UInt64 shipId;
        public ShipType shipType;
        public string name;
    }

    public struct ShipCargoSlot
    {
        public bool hasCargo;
        public UInt64 amount;
        public Resource resourceType;
    }

    public struct IslandResourceRecord
    {
        public Resource resourceType;
        public UInt64 capacity;
        public UInt64 amount;
    }

    public struct IslandData
    {
        public UInt64 id;
        public string name;
    }

    public struct BuildingData
    {
        public UInt64 id;
        public Building buidlingType;
        public UInt64 rawBuildingTypeID;
        public Coordinate position;
        public UInt64 debug_address;
    }

    public struct ProductionNode
    {
        public List<Resource> input;
        public Resource output;
        public double rate;
    }

    public struct ConsumptionNode
    {
        public Resource resourceType;
        public double rate;
    }

    public struct TradeNode
    {
        public IslandData island;
    }

    public struct TradeRoute
    {
        public string name;
        public List<TradeNode> nodes;
    }

    public partial class Serializer
    {
        public static bool Serialize(bool data, List<byte> buffer)
        {
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }

        public static bool Deserialize(out bool data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = false;
            offsetAfter = offset;

            if (buffer.Length + 1 <= offset)
                return false;

            data = BitConverter.ToBoolean(buffer, offset);
            offsetAfter = offset + 1;
            return true;
        }

        public static bool Serialize(List<bool> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<bool> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<bool>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                bool element = false;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(UInt64 data, List<byte> buffer)
        {
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }

        public static bool Deserialize(out UInt64 data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = 0;
            offsetAfter = offset;

            if (buffer.Length + 8 <= offset)
                return false;

            data = BitConverter.ToUInt64(buffer, offset);
            offsetAfter = offset + 8;
            return true;
        }

        public static bool Serialize(List<UInt64> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<UInt64> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<UInt64>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                UInt64 element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(float data, List<byte> buffer)
        {
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }

        public static bool Deserialize(out float data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = 0;
            offsetAfter = offset;

            if (buffer.Length + 4 <= offset)
                return false;

            data = BitConverter.ToSingle(buffer, offset);
            offsetAfter = offset + 4;
            return true;
        }

        public static bool Serialize(List<float> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<float> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<float>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                float element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(double data, List<byte> buffer)
        {
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }

        public static bool Deserialize(out double data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = 0;
            offsetAfter = offset;

            if (buffer.Length + 8 <= offset)
                return false;

            data = BitConverter.ToDouble(buffer, offset);
            offsetAfter = offset + 8;
            return true;
        }

        public static bool Serialize(List<double> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<double> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<double>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                double element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(string data, List<byte> buffer)
        {
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(data);

            UInt64 size = (ulong)bytes.Length;

            var datum = BitConverter.GetBytes(size);

            buffer.AddRange(datum);
            buffer.AddRange(bytes);

            return true;
        }

        public static bool Deserialize(out string data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = "";
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            data = System.Text.Encoding.UTF8.GetString(buffer, offsetAfter, (int)size);

            offsetAfter += (int)size;

            return true;
        }

        public static bool Serialize(List<string> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<string> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<string>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                string element = "";
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(Area data, List<byte> buffer)
        {
            return Serialize((ulong)data, buffer);
        }

        public static bool Deserialize(out Area data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = Area.Invalid;
            ulong temp = 0;
            offsetAfter = offset;

            if (!Deserialize(out temp, buffer, offset, out offsetAfter))
                return false;

            data = (Area)temp;

            return true;
        }

        public static bool Serialize(List<Area> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                ulong d = 0;
                if (!Serialize(d, buffer))
                    return false;
                data[i] = (Area)d;
            }

            return true;
        }

        public static bool Deserialize(out List<Area> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<Area>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                Area element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }
        public static bool Serialize(ShipType data, List<byte> buffer)
        {
            return Serialize((ulong)data, buffer);
        }

        public static bool Deserialize(out ShipType data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = ShipType.Invalid;
            ulong temp = 0;
            offsetAfter = offset;

            if (!Deserialize(out temp, buffer, offset, out offsetAfter))
                return false;

            data = (ShipType)temp;

            return true;
        }

        public static bool Serialize(List<ShipType> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                ulong d = 0;
                if (!Serialize(d, buffer))
                    return false;
                data[i] = (ShipType)d;
            }

            return true;
        }

        public static bool Deserialize(out List<ShipType> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<ShipType>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                ShipType element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }
        public static bool Serialize(Resource data, List<byte> buffer)
        {
            return Serialize((ulong)data, buffer);
        }

        public static bool Deserialize(out Resource data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = Resource.Invalid;
            ulong temp = 0;
            offsetAfter = offset;

            if (!Deserialize(out temp, buffer, offset, out offsetAfter))
                return false;

            data = (Resource)temp;

            return true;
        }

        public static bool Serialize(List<Resource> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                ulong d = 0;
                if (!Serialize(d, buffer))
                    return false;
                data[i] = (Resource)d;
            }

            return true;
        }

        public static bool Deserialize(out List<Resource> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<Resource>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                Resource element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }
        public static bool Serialize(Building data, List<byte> buffer)
        {
            return Serialize((ulong)data, buffer);
        }

        public static bool Deserialize(out Building data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = Building.Invalid;
            ulong temp = 0;
            offsetAfter = offset;

            if (!Deserialize(out temp, buffer, offset, out offsetAfter))
                return false;

            data = (Building)temp;

            return true;
        }

        public static bool Serialize(List<Building> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                ulong d = 0;
                if (!Serialize(d, buffer))
                    return false;
                data[i] = (Building)d;
            }

            return true;
        }

        public static bool Deserialize(out List<Building> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<Building>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                Building element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }
        public static bool Serialize(GameTime data, List<byte> buffer)
        {
            if (!Serialize(data.time, buffer))
                return false;

            if (!Serialize(data.frame, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out GameTime data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.time, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.frame, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<GameTime> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<GameTime> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<GameTime>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                GameTime element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(Coordinate data, List<byte> buffer)
        {
            if (!Serialize(data.x, buffer))
                return false;

            if (!Serialize(data.y, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out Coordinate data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.x, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.y, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<Coordinate> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<Coordinate> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<Coordinate>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                Coordinate element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(ShipMoveData data, List<byte> buffer)
        {
            if (!Serialize(data.moving, buffer))
                return false;

            if (!Serialize(data.position, buffer))
                return false;

            if (!Serialize(data.waypoints, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out ShipMoveData data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.moving, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.position, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.waypoints, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<ShipMoveData> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<ShipMoveData> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<ShipMoveData>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                ShipMoveData element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(ShipData data, List<byte> buffer)
        {
            if (!Serialize(data.shipId, buffer))
                return false;

            if (!Serialize(data.shipType, buffer))
                return false;

            if (!Serialize(data.name, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out ShipData data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.shipId, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.shipType, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<ShipData> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<ShipData> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<ShipData>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                ShipData element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(ShipCargoSlot data, List<byte> buffer)
        {
            if (!Serialize(data.hasCargo, buffer))
                return false;

            if (!Serialize(data.amount, buffer))
                return false;

            if (!Serialize(data.resourceType, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out ShipCargoSlot data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.hasCargo, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.amount, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.resourceType, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<ShipCargoSlot> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<ShipCargoSlot> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<ShipCargoSlot>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                ShipCargoSlot element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(IslandResourceRecord data, List<byte> buffer)
        {
            if (!Serialize(data.resourceType, buffer))
                return false;

            if (!Serialize(data.capacity, buffer))
                return false;

            if (!Serialize(data.amount, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out IslandResourceRecord data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.resourceType, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.capacity, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.amount, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<IslandResourceRecord> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<IslandResourceRecord> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<IslandResourceRecord>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                IslandResourceRecord element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(IslandData data, List<byte> buffer)
        {
            if (!Serialize(data.id, buffer))
                return false;

            if (!Serialize(data.name, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out IslandData data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.id, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<IslandData> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<IslandData> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<IslandData>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                IslandData element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(BuildingData data, List<byte> buffer)
        {
            if (!Serialize(data.id, buffer))
                return false;

            if (!Serialize(data.buidlingType, buffer))
                return false;

            if (!Serialize(data.rawBuildingTypeID, buffer))
                return false;

            if (!Serialize(data.position, buffer))
                return false;

            if (!Serialize(data.debug_address, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out BuildingData data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.id, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.buidlingType, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.rawBuildingTypeID, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.position, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.debug_address, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<BuildingData> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<BuildingData> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<BuildingData>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                BuildingData element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(ProductionNode data, List<byte> buffer)
        {
            if (!Serialize(data.input, buffer))
                return false;

            if (!Serialize(data.output, buffer))
                return false;

            if (!Serialize(data.rate, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out ProductionNode data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.input, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.output, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.rate, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<ProductionNode> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<ProductionNode> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<ProductionNode>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                ProductionNode element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(ConsumptionNode data, List<byte> buffer)
        {
            if (!Serialize(data.resourceType, buffer))
                return false;

            if (!Serialize(data.rate, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out ConsumptionNode data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.resourceType, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.rate, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<ConsumptionNode> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<ConsumptionNode> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<ConsumptionNode>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                ConsumptionNode element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(TradeNode data, List<byte> buffer)
        {
            if (!Serialize(data.island, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out TradeNode data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.island, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<TradeNode> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<TradeNode> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<TradeNode>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                TradeNode element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

        public static bool Serialize(TradeRoute data, List<byte> buffer)
        {
            if (!Serialize(data.name, buffer))
                return false;

            if (!Serialize(data.nodes, buffer))
                return false;

            return true;
        }

        public static bool Deserialize(out TradeRoute data, byte[] buffer, int offset, out int offsetAfter)
        {
            data = default;
            offsetAfter = offset;

            if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            if (!Deserialize(out data.nodes, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }

        public static bool Serialize(List<TradeRoute> data, List<byte> buffer)
        {
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {
                if (!Serialize(data[i], buffer))
                    return false;
            }

            return true;
        }

        public static bool Deserialize(out List<TradeRoute> data, byte[] buffer, int offset, out int offsetAfter)
        {
            ulong size = 0;
            data = new List<TradeRoute>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {
                TradeRoute element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }

            return true;
        }

    }

    public partial class Telegraph
    {
        public bool GetGameTime(out GameTime gameTime)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            gameTime = default;
            ulong function_id = 1;

            if (!Serializer.Serialize(function_id, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out gameTime, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool WriteLineToLog(string message)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 2;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(message, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool WriteLinesToLog(List<string> messages)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 3;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(messages, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool GetShipById(UInt64 id, out ShipData shipData)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            shipData = default;
            ulong function_id = 4;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(id, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out shipData, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetShipComponentAddress(UInt64 shipID, UInt64 componentID, out UInt64 address)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            address = default;
            ulong function_id = 5;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Serializer.Serialize(componentID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out address, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetShipMoveData(UInt64 id, out ShipMoveData moveData)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            moveData = default;
            ulong function_id = 6;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(id, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out moveData, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetBuildingComponentAddress(UInt64 islandID, UInt64 buildingID, UInt64 componentID, out UInt64 address)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            address = default;
            ulong function_id = 7;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Serializer.Serialize(buildingID, payload))
                return false;

            if (!Serializer.Serialize(componentID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out address, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetAllShips(Area area, out List<ShipData> ships_ShipData_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            ships_ShipData_list = default;
            ulong function_id = 8;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(area, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out ships_ShipData_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetShipAddress(UInt64 shipID, out UInt64 address)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            address = default;
            ulong function_id = 9;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out address, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetShipCargo(UInt64 shipID, out List<ShipCargoSlot> cargo_ShipCargoSlot_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            cargo_ShipCargoSlot_list = default;
            ulong function_id = 10;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out cargo_ShipCargoSlot_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool AddWaypoint(List<UInt64> IDs, Coordinate waypoint)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 11;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(IDs, payload))
                return false;

            if (!Serializer.Serialize(waypoint, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool GetIslandsByName(string name, out List<IslandData> islands_IslandData_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            islands_IslandData_list = default;
            ulong function_id = 12;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(name, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out islands_IslandData_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetIslandResources(UInt64 islandID, out List<IslandResourceRecord> resources_IslandResourceRecord_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            resources_IslandResourceRecord_list = default;
            ulong function_id = 13;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out resources_IslandResourceRecord_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetIslandBuildings(UInt64 islandID, out List<BuildingData> buildings_BuildingData_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            buildings_BuildingData_list = default;
            ulong function_id = 14;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out buildings_BuildingData_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool ShipDumpCargo(UInt64 shipID, UInt64 slotIndex)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 15;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Serializer.Serialize(slotIndex, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool ShipDumpCargo(UInt64 shipID, UInt64 slotIndex, Coordinate position)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 16;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Serializer.Serialize(slotIndex, payload))
                return false;

            if (!Serializer.Serialize(position, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool LoadCargoToShip(UInt64 islandID, UInt64 shipID, Resource resource, UInt64 amount, UInt64 slotIndex)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 17;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Serializer.Serialize(resource, payload))
                return false;

            if (!Serializer.Serialize(amount, payload))
                return false;

            if (!Serializer.Serialize(slotIndex, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool OffloadCargoFromShip(UInt64 islandID, UInt64 shipID, UInt64 amount, UInt64 slotIndex)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;

            ulong function_id = 18;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Serializer.Serialize(shipID, payload))
                return false;

            if (!Serializer.Serialize(amount, payload))
                return false;

            if (!Serializer.Serialize(slotIndex, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();


            return true;
        }
    
        public bool GetBuildingProduction(UInt64 islandID, UInt64 buildingID, out ProductionNode production)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            production = default;
            ulong function_id = 19;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Serializer.Serialize(buildingID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out production, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetIslandConsumption(UInt64 islandID, out List<ConsumptionNode> consumption_ConsumptionNode_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            consumption_ConsumptionNode_list = default;
            ulong function_id = 20;

            if (!Serializer.Serialize(function_id, payload))
                return false;
            if (!Serializer.Serialize(islandID, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out consumption_ConsumptionNode_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetAllIslands(out List<IslandData> islands_IslandData_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            islands_IslandData_list = default;
            ulong function_id = 21;

            if (!Serializer.Serialize(function_id, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out islands_IslandData_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
        public bool GetAllTradeRoutes(out List<TradeRoute> routes_TradeRoute_list)
        {
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
            routes_TradeRoute_list = default;
            ulong function_id = 22;

            if (!Serializer.Serialize(function_id, payload))
                return false;

            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

            if (!Serializer.Deserialize(out routes_TradeRoute_list, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;

            return true;
        }
    
    }
}
