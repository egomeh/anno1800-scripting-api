using System;
using System.Collections.Generic;

namespace monocle
{{
{structures}
    public partial class Serializer
    {{
        public static bool Serialize(bool data, List<byte> buffer)
        {{
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }}

        public static bool Deserialize(out bool data, byte[] buffer, int offset, out int offsetAfter)
        {{
            data = false;
            offsetAfter = offset;

            if (buffer.Length + 1 <= offset)
                return false;

            data = BitConverter.ToBoolean(buffer, offset);
            offsetAfter = offset + 1;
            return true;
        }}

        public static bool Serialize(List<bool> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                if (!Serialize(data[i], buffer))
                    return false;
            }}

            return true;
        }}

        public static bool Deserialize(out List<bool> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<bool>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                bool element = false;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}

        public static bool Serialize(UInt64 data, List<byte> buffer)
        {{
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }}

        public static bool Deserialize(out UInt64 data, byte[] buffer, int offset, out int offsetAfter)
        {{
            data = 0;
            offsetAfter = offset;

            if (buffer.Length + 8 <= offset)
                return false;

            data = BitConverter.ToUInt64(buffer, offset);
            offsetAfter = offset + 8;
            return true;
        }}

        public static bool Serialize(List<UInt64> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                if (!Serialize(data[i], buffer))
                    return false;
            }}

            return true;
        }}

        public static bool Deserialize(out List<UInt64> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<UInt64>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                UInt64 element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}

        public static bool Serialize(float data, List<byte> buffer)
        {{
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }}

        public static bool Deserialize(out float data, byte[] buffer, int offset, out int offsetAfter)
        {{
            data = 0;
            offsetAfter = offset;

            if (buffer.Length + 4 <= offset)
                return false;

            data = BitConverter.ToSingle(buffer, offset);
            offsetAfter = offset + 4;
            return true;
        }}

        public static bool Serialize(List<float> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                if (!Serialize(data[i], buffer))
                    return false;
            }}

            return true;
        }}

        public static bool Deserialize(out List<float> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<float>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                float element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}

        public static bool Serialize(double data, List<byte> buffer)
        {{
            var datum = BitConverter.GetBytes(data);

            buffer.AddRange(datum);

            return true;
        }}

        public static bool Deserialize(out double data, byte[] buffer, int offset, out int offsetAfter)
        {{
            data = 0;
            offsetAfter = offset;

            if (buffer.Length + 8 <= offset)
                return false;

            data = BitConverter.ToDouble(buffer, offset);
            offsetAfter = offset + 8;
            return true;
        }}

        public static bool Serialize(List<double> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                if (!Serialize(data[i], buffer))
                    return false;
            }}

            return true;
        }}

        public static bool Deserialize(out List<double> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<double>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                double element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}

        public static bool Serialize(string data, List<byte> buffer)
        {{
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(data);

            UInt64 size = (ulong)bytes.Length;

            var datum = BitConverter.GetBytes(size);

            buffer.AddRange(datum);
            buffer.AddRange(bytes);

            return true;
        }}

        public static bool Deserialize(out string data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = "";
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            data = System.Text.Encoding.UTF8.GetString(buffer, offsetAfter, (int)size);

            offsetAfter += (int)size;

            return true;
        }}

        public static bool Serialize(List<string> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                if (!Serialize(data[i], buffer))
                    return false;
            }}

            return true;
        }}

        public static bool Deserialize(out List<string> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<string>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                string element = "";
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}

{serialization}
    }}

    public partial class Telegraph
    {{
{functions}
    }}
}}
