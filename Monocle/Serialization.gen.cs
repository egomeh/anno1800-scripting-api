// Auto generated code
using System;
using System.Collections.Generic;


public class Serializer
{
    public bool Serialize(bool data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out bool data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(bool) <= offset)
            return false;

        data = BitConverter.ToBoolean(buffer, offset);
        offsetAfter += sizeof(bool);
        return true;
    }


    public bool Serialize(int data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out int data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(int) <= offset)
            return false;

        data = BitConverter.ToInt32(buffer, offset);
        offsetAfter += sizeof(int);
        return true;
    }


    public bool Serialize(uint data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out uint data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(uint) <= offset)
            return false;

        data = BitConverter.ToUInt32(buffer, offset);
        offsetAfter += sizeof(uint);
        return true;
    }


    public bool Serialize(long data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out long data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(long) <= offset)
            return false;

        data = BitConverter.ToInt64(buffer, offset);
        offsetAfter += sizeof(long);
        return true;
    }


    public bool Serialize(ulong data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out ulong data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(ulong) <= offset)
            return false;

        data = BitConverter.ToUInt64(buffer, offset);
        offsetAfter += sizeof(ulong);
        return true;
    }


    public bool Serialize(float data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out float data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(float) <= offset)
            return false;

        data = BitConverter.ToSingle(buffer, offset);
        offsetAfter += sizeof(float);
        return true;
    }


    public bool Serialize(double data, List<byte> buffer)
    {
        var bytes = BitConverter.GetBytes(data);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out double data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = default;
        offsetAfter = offset;

        if (buffer.Length + sizeof(double) <= offset)
            return false;

        data = BitConverter.ToDouble(buffer, offset);
        offsetAfter += sizeof(double);
        return true;
    }


    public bool Serialize(string data, List<byte> buffer)
    {
        byte[] bytes = System.Text.Encoding.UTF8.GetBytes(data);
        byte[] size = BitConverter.GetBytes((ulong)bytes.Length);
        buffer.AddRange(size);
        buffer.AddRange(bytes);
        return true;
    }

    public bool Deserialize(out string data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size = 0;
        data = "";
        offsetAfter = 0;
        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
            return false;
        data = System.Text.Encoding.UTF8.GetString(buffer, offsetAfter, (int)size);
        offsetAfter += (int)size;
        return true;
    }


    public bool Serialize(Coordinate data, List<byte> buffer)
    {
        if (!Serialize(data.x, buffer))
            return false;

        if (!Serialize(data.y, buffer))
            return false;

        return true;
    }

    public bool Deserialize(out Coordinate data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new Coordinate();
        offsetAfter = offset;

        if (!Deserialize(out data.x, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.y, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public bool Serialize(CargoSlot data, List<byte> buffer)
    {
        if (!Serialize(data.itemID, buffer))
            return false;

        if (!Serialize(data.volume, buffer))
            return false;

        return true;
    }

    public bool Deserialize(out CargoSlot data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new CargoSlot();
        offsetAfter = offset;

        if (!Deserialize(out data.itemID, buffer, offset, out offsetAfter))
            return false;
        offset = offsetAfter;

        if (!Deserialize(out data.volume, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public bool Serialize(List<ulong> data, List<byte> buffer)
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

    public bool Deserialize(out List<ulong> data, byte[] buffer, int offset, out int offsetAfter)
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



    public bool Serialize(SomeListyBoi data, List<byte> buffer)
    {
        if (!Serialize(data.ids, buffer))
            return false;

        return true;
    }

    public bool Deserialize(out SomeListyBoi data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new SomeListyBoi();
        offsetAfter = offset;

        if (!Deserialize(out data.ids, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public bool Serialize(Ship data, List<byte> buffer)
    {
        if (!Serialize(data.name, buffer))
            return false;

        return true;
    }

    public bool Deserialize(out Ship data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new Ship();
        offsetAfter = offset;

        if (!Deserialize(out data.name, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public bool Serialize(List<CargoSlot> data, List<byte> buffer)
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

    public bool Deserialize(out List<CargoSlot> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<CargoSlot>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            CargoSlot element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public bool Serialize(ShipCargo data, List<byte> buffer)
    {
        if (!Serialize(data.cargo, buffer))
            return false;

        return true;
    }

    public bool Deserialize(out ShipCargo data, byte[] buffer, int offset, out int offsetAfter)
    {
        data = new ShipCargo();
        offsetAfter = offset;

        if (!Deserialize(out data.cargo, buffer, offset, out offsetAfter))
            return false;

        return true;
    }



    public bool Serialize(List<Coordinate> data, List<byte> buffer)
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

    public bool Deserialize(out List<Coordinate> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<Coordinate>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            Coordinate element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }



    public bool Serialize(List<string> data, List<byte> buffer)
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

    public bool Deserialize(out List<string> data, byte[] buffer, int offset, out int offsetAfter)
    {
        ulong size;
        data = new List<string>();
        offsetAfter = offset;

        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

        for (ulong i = 0; i < size; ++i)
        {
            string element;
            if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                return false;
            data.Add(element);
        }

        return true;
    }


}
