// Auto generated code
using System;
using System.Collections.Generic;


public class Serializer
{
    public static bool Serialize(bool data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out bool data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(bool) <= offset)
            return false;

        data = BitConverter.ToBoolean(buffer, offset);
        offsetAfter += sizeof(bool);
        return true;
    }


    public static bool Serialize(int data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out int data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(int) <= offset)
            return false;

        data = BitConverter.ToInt32(buffer, offset);
        offsetAfter += sizeof(int);
        return true;
    }


    public static bool Serialize(uint data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out uint data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(uint) <= offset)
            return false;

        data = BitConverter.ToUInt32(buffer, offset);
        offsetAfter += sizeof(uint);
        return true;
    }


    public static bool Serialize(long data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out long data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(long) <= offset)
            return false;

        data = BitConverter.ToInt64(buffer, offset);
        offsetAfter += sizeof(long);
        return true;
    }


    public static bool Serialize(ulong data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out ulong data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(ulong) <= offset)
            return false;

        data = BitConverter.ToUInt64(buffer, offset);
        offsetAfter += sizeof(ulong);
        return true;
    }


    public static bool Serialize(float data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out float data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(float) <= offset)
            return false;

        data = BitConverter.ToSingle(buffer, offset);
        offsetAfter += sizeof(float);
        return true;
    }


    public static bool Serialize(double data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public static bool Deserialize(out double data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(double) <= offset)
            return false;

        data = BitConverter.ToDouble(buffer, offset);
        offsetAfter += sizeof(double);
        return true;
    }


    public static bool Serialize(string data, List<byte> buffer)
    {
        byte[] bytes = System.Text.Encoding.UTF8.GetBytes(data);
        byte[] size = BitConverter.GetBytes((ulong)bytes.Length);
        buffer.AddRange(size);
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


    public static bool Serialize(IslandResource data, List<byte> buffer)
    {
        if (!Serialize(data.type_id, buffer))
            return false;

        if (!Serialize(data.name, buffer))
            return false;

        if (!Serialize(data.amount, buffer))
            return false;

        if (!Serialize(data.capacity, buffer))
            return false;

        if (!Serialize(data.amount_ptr, buffer))
            return false;

        return true;
    }

    public static bool Deserialize(out IslandResource data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new IslandResource();
        offsetAfter = offset;

        if (!Deserialize(out data.type_id, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.amount, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.capacity, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.amount_ptr, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public static bool Serialize(ResourceConsumption data, List<byte> buffer)
    {
        if (!Serialize(data.type_id, buffer))
            return false;

        if (!Serialize(data.name, buffer))
            return false;

        if (!Serialize(data.rate, buffer))
            return false;

        return true;
    }

    public static bool Deserialize(out ResourceConsumption data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new ResourceConsumption();
        offsetAfter = offset;

        if (!Deserialize(out data.type_id, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.rate, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public static bool Serialize(IslandInfo data, List<byte> buffer)
    {
        if (!Serialize(data.island_id, buffer))
            return false;

        if (!Serialize(data.name, buffer))
            return false;

        if (!Serialize(data.debug_address, buffer))
            return false;

        return true;
    }

    public static bool Deserialize(out IslandInfo data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new IslandInfo();
        offsetAfter = offset;

        if (!Deserialize(out data.island_id, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.debug_address, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public static bool Serialize(ShipInfo data, List<byte> buffer)
    {
        if (!Serialize(data.id, buffer))
            return false;

        if (!Serialize(data.debug_address, buffer))
            return false;

        return true;
    }

    public static bool Deserialize(out ShipInfo data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new ShipInfo();
        offsetAfter = offset;

        if (!Deserialize(out data.id, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.debug_address, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public static bool Serialize(List<IslandInfo> data, List<byte> buffer)
    {
        ulong size = (ulong)data.Count;
        if (!Serialize(size, buffer))
                return false;

        for (int i = 0; i < data.Count; ++i)
        {
            if (!Serialize(data[i], buffer))
                return false;
        }

        return true;
    }

    public static bool Deserialize(out List<IslandInfo> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<IslandInfo>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            IslandInfo element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public static bool Serialize(List<uint> data, List<byte> buffer)
    {
        ulong size = (ulong)data.Count;
        if (!Serialize(size, buffer))
                return false;

        for (int i = 0; i < data.Count; ++i)
        {
            if (!Serialize(data[i], buffer))
                return false;
        }

        return true;
    }

    public static bool Deserialize(out List<uint> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<uint>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            uint element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public static bool Serialize(List<IslandResource> data, List<byte> buffer)
    {
        ulong size = (ulong)data.Count;
        if (!Serialize(size, buffer))
                return false;

        for (int i = 0; i < data.Count; ++i)
        {
            if (!Serialize(data[i], buffer))
                return false;
        }

        return true;
    }

    public static bool Deserialize(out List<IslandResource> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<IslandResource>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            IslandResource element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public static bool Serialize(List<ResourceConsumption> data, List<byte> buffer)
    {
        ulong size = (ulong)data.Count;
        if (!Serialize(size, buffer))
                return false;

        for (int i = 0; i < data.Count; ++i)
        {
            if (!Serialize(data[i], buffer))
                return false;
        }

        return true;
    }

    public static bool Deserialize(out List<ResourceConsumption> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<ResourceConsumption>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            ResourceConsumption element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public static bool Serialize(List<ShipInfo> data, List<byte> buffer)
    {
        ulong size = (ulong)data.Count;
        if (!Serialize(size, buffer))
                return false;

        for (int i = 0; i < data.Count; ++i)
        {
            if (!Serialize(data[i], buffer))
                return false;
        }

        return true;
    }

    public static bool Deserialize(out List<ShipInfo> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<ShipInfo>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            ShipInfo element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public static bool Serialize(List<ulong> data, List<byte> buffer)
    {
        ulong size = (ulong)data.Count;
        if (!Serialize(size, buffer))
                return false;

        for (int i = 0; i < data.Count; ++i)
        {
            if (!Serialize(data[i], buffer))
                return false;
        }

        return true;
    }

    public static bool Deserialize(out List<ulong> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<ulong>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            ulong element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }


}
