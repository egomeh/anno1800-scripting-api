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
        if (!GenerateCSCode())
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

            code += string.Format("    {0} {1};\n", CSTypeString, fieldEntry.Key);
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

    bool GenerateCSCode()
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

}

