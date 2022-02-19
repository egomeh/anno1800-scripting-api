using System.Text.RegularExpressions;

class CodeGenerator
{
    string m_CppPath = "";
    string m_CsPath = "";

    TypeTable m_TypeTable;

    public CodeGenerator(string cppPath, string csPath, TypeTable typeTable)
    {
        m_CppPath = cppPath;
        m_CsPath = csPath;
        m_TypeTable = typeTable;
    }


    public bool GenerateCode()
    {
        if (!GenerateCSTypes())
            return false;

        if (!GenerateCSSerializationCode())
            return false;

        if (!GenerateCPPCode())
            return false;

        return true;
    }

    bool GetCPPTypeString(Type type, out string typeString)
    {
        typeString = "";

        if (type is Bool)
            typeString = "bool";
        else if (type is Int32)
            typeString = "int32_t";
        else if (type is UInt32)
            typeString = "uint32_t";
        else if (type is Int64)
            typeString = "int64_t";
        else if (type is UInt64)
            typeString = "uint64_t";
        else if (type is Float)
            typeString = "float";
        else if (type is Double)
            typeString = "double";
        else if (type is StringType)
            typeString = "std::string";
        else if (type is ListType)
        {
            ListType? lt = type as ListType;

            if (lt == null)
                return false;

            string innerType;
            if (!GetCPPTypeString(lt.GetElementType(), out innerType))
                return false;

            typeString = string.Format("std::vector<{0}>", innerType);
        }
        else if (type is CompoundType)
        {
            CompoundType? ct = type as CompoundType;

            if (ct == null)
                return false;

            typeString = ct.name;
        }

        return !string.IsNullOrEmpty(typeString);
    }


    bool GetCSTypeString(Type type, out string typeString)
    {
        typeString = "";

        if (type is Bool)
            typeString = "bool";
        else if (type is Int32)
            typeString = "int";
        else if (type is UInt32)
            typeString = "uint";
        else if (type is Int64)
            typeString = "long";
        else if (type is UInt64)
            typeString = "ulong";
        else if (type is Float)
            typeString = "float";
        else if (type is Double)
            typeString = "double";
        else if (type is StringType)
            typeString = "string";
        else if (type is ListType)
        {
            ListType? lt = type as ListType;

            if (lt == null)
                return false;

            string innerType;
            if (!GetCSTypeString(lt.GetElementType(), out innerType))
                return false;

            typeString = string.Format("List<{0}>", innerType);
        }
        else if (type is CompoundType)
        {
            CompoundType? ct = type as CompoundType;

            if (ct == null)
                return false;

            typeString = ct.name;
        }

        return !string.IsNullOrEmpty(typeString);
    }

    bool GenerateCSType(TypeTable typeTable, Type type, out string code)
    {
        code = "";

        CompoundType? compoundType = type as CompoundType;

        if (compoundType == null)
            return false;

        code += String.Format("struct {0}\n{{\n", compoundType.name);
        
        foreach (var fieldEntry in compoundType.GetFields())
        {
            string CSTypeString;

            if (!GetCSTypeString(fieldEntry.Value, out CSTypeString))
                return false;

            code += string.Format("    public {0} {1};\n", CSTypeString, fieldEntry.Key);
        }

        code += "}\n";

        return true;
    }

    bool GenerateCPPType(TypeTable typeTable, Type type, out string code)
    {
        code = "";

        CompoundType? compoundType = type as CompoundType;

        if (compoundType == null)
            return false;

        code += String.Format("struct {0}\n{{\n", compoundType.name);

        foreach (var fieldEntry in compoundType.GetFields())
        {
            string CSTypeString;

            if (!GetCPPTypeString(fieldEntry.Value, out CSTypeString))
                return false;

            code += string.Format("    {0} {1};\n", CSTypeString, fieldEntry.Key);
        }

        code += "};\n";

        return true;
    }

    bool GenerateCSTypes()
    {
        string CSTypesFile = Path.Combine(m_CsPath, "Structs.gen.cs");

        string CSTypeCode = "// Auto generated code\n";
        CSTypeCode += "using System.Collections.Generic;\n";

        foreach (var typename in m_TypeTable.GetRegisteredNames())
        {
            Type type = m_TypeTable.GetType(typename);

            if (!(type is CompoundType))
                continue;

            string code;
            if (!GenerateCSType(m_TypeTable, type, out code))
                continue;

            CSTypeCode += "\n" + code + "\n";
        }

        File.WriteAllText(CSTypesFile, CSTypeCode);

        return true;
    }

    bool GenerateCPPCode()
    {
        string CPPTypesFile = Path.Combine(m_CppPath, "structs.gen.h");

        string CPPTypeCode = "#pragma once\n";
        CPPTypeCode += "#include <vector>\n";
        CPPTypeCode += "#include <inttypes.h>\n";
        CPPTypeCode += "#include <string>\n";

        foreach (var typename in m_TypeTable.GetRegisteredNames())
        {
            Type type = m_TypeTable.GetType(typename);

            if (!(type is CompoundType))
                continue;

            string code;
            if (!GenerateCPPType(m_TypeTable, type, out code))
                continue;

            CPPTypeCode += "\n" + code + "\n";
        }

        File.WriteAllText(CPPTypesFile, CPPTypeCode);

        return true;
    }

    bool GetCSBitConverterPostFix(Type type, out string postfix)
    {
        postfix = "";

        if (type is Bool)
            postfix = "Boolean";
        else if (type is Int32)
            postfix = "Int32";
        else if (type is UInt32)
            postfix = "UInt32";
        else if (type is Int64)
            postfix = "Int64";
        else if (type is UInt64)
            postfix = "UInt64";
        else if (type is Float)
            postfix = "Single";
        else if (type is Double)
            postfix = "Double";

        return !String.IsNullOrEmpty(postfix);
    }

    bool GenerateCSSerializationForType(TypeTable typetable, Type type, out string code)
    {
        code = "";

        string csTypeName;

        if (!GetCSTypeString(type, out csTypeName))
            return false;

        if (type is BaseType)
        {
            string bitConverterPostfix;
            if (!GetCSBitConverterPostFix(type, out bitConverterPostfix))
                return false;

            code += string.Format("    public bool Serialize({0} data, List<byte> buffer)\n", csTypeName);
            code += "    {\n";
            code += "        var bytes = BitConverter.GetBytes(data);\n";
            code += "        buffer.AddRange(bytes);\n";
            code += "        return true;\n";
            code += "    }\n\n";

            code += string.Format("    public bool Deserialize(out {0} data, byte[] buffer, int offset, out int offsetAfter)\n", csTypeName);
            code += "    {\n";
            code += "        data = default;\n";
            code += "        offsetAfter = offset;\n\n";
            code += String.Format("        if (buffer.Length + sizeof({0}) <= offset)\n", csTypeName);
            code += "            return false;\n\n";
            code += String.Format("        data = BitConverter.To{0}(buffer, offset);\n", bitConverterPostfix);
            code += String.Format("        offsetAfter = sizeof({0});\n", csTypeName);
            code += "        return true;\n";
            code += "    }\n";


            return true;
        }
        else if (type is StringType)
        {
            code += "    public bool Serialize(string data, List<byte> buffer)\n";
            code += "    {\n";
            code += "        byte[] bytes = System.Text.Encoding.UTF8.GetBytes(data);\n";

            code += "        byte[] size = BitConverter.GetBytes((ulong)bytes.Length);\n";

            code += "        buffer.AddRange(size);\n";
            code += "        buffer.AddRange(bytes);\n";

            code += "        return true;\n";
            code += "    }\n";
        }
        else if (type is CompoundType)
        {
            CompoundType? ct = type as CompoundType;

            if (ct == null)
                return false;

            code += string.Format("    public bool Serialize({0} data, List<byte> buffer)\n", csTypeName);
            code += "    {\n";

            foreach (var fieldEntry in ct.GetFields())
            {
                code += string.Format("        if (!Serialize(data.{0}, buffer))\n", fieldEntry.Key);
                code += string.Format("            return false;\n\n");
            }

            code += "        return true;\n";
            code += "    }\n\n";
        }
        else if (type is ListType)
        {
            ListType? lt = type as ListType;

            if (lt == null)
                return false;

            code += string.Format("    public bool Serialize({0} data, List<byte> buffer)\n", csTypeName);
            code += "    {\n";

            code += "        ulong size = (ulong)data.Count;\n";
            code += "        if (!Serialize(size, buffer))\n";
            code += "                return false;\n\n";


            code += "        for (int i = 0; i < data.Count; ++i)\n";
            code += "        {\n";
            code += "            if (!Serialize(data[i], buffer))\n";
            code += "                return false;\n";
            code += "        }\n\n";

            code += "        return true;\n";
            code += "    }\n\n";
        }


        return true;
    }

    bool GenerateCSSerializationCode()
    {
        string CSSerializationFile = Path.Combine(m_CsPath, "Serialization.gen.cs");

        string CSSerializationCode = "// Auto generated code\n";
        CSSerializationCode += "using System;\n";
        CSSerializationCode += "using System.Collections.Generic;\n";
        CSSerializationCode += "\n\n";

        CSSerializationCode += "class Serializer\n{";

        foreach (var typename in m_TypeTable.GetRegisteredNames())
        {
            Type type = m_TypeTable.GetType(typename);

            string code;
            if (!GenerateCSSerializationForType(m_TypeTable, type, out code))
                continue;

            CSSerializationCode += "\n" + code + "\n";
        }

        CSSerializationCode += "}\n";

        File.WriteAllText(CSSerializationFile, CSSerializationCode);

        return true;
    }

}

