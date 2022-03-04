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
        offsetAfter = 0;
        if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
            return false;
        data = System.Text.Encoding.UTF8.GetString(buffer, offsetAfter, (int)size);
        offsetAfter += (int)size;
        return true;
    }

}
