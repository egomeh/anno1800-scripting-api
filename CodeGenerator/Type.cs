

using System.Text.RegularExpressions;

abstract class Type
{
    protected string m_Name = "";

    public string name { get { return m_Name; } }
}

abstract class BaseType : Type
{ }

class Bool : BaseType
{
    public Bool()
    {
        m_Name = "bool";
    }
}

class Int32 : BaseType
{
    public Int32()
    {
        m_Name = "int32";
    }
}

class UInt32 : BaseType
{
    public UInt32()
    {
        m_Name = "uint32";
    }
}

class Int64 : BaseType
{
    public Int64()
    {
        m_Name = "int64";
    }
}

class UInt64 : BaseType
{
    public UInt64()
    {
        m_Name = "uint64";
    }
}

class Float : BaseType
{
    public Float()
    {
        m_Name = "float";
    }
}

class Double : BaseType
{
    public Double()
    {
        m_Name = "double";
    }
}

class StringType : Type
{
    public StringType()
    {
        m_Name = "string";
    }
}

class ListType : Type
{
    public ListType(string name, Type elementType)
    {
        m_Name = name;
        m_ElementType = elementType;
    }

    public Type GetElementType()
    {
        return m_ElementType;
    }

    Type m_ElementType;
}

class CompoundType : Type
{
    public CompoundType(string name)
    {
        m_Name = name;
        m_Fields = new Dictionary<string, Type>();
    }

    public void AddField(string name, Type type)
    {
        m_Fields[name] = type;
    }

    public Dictionary<string, Type> GetFields()
    {
        return m_Fields;
    }

    Dictionary<string, Type> m_Fields;
}

class TypeTable
{
    public TypeTable()
    {
        m_Types = new Dictionary<string, Type>();

        m_Types["bool"] = new Bool();
        m_Types["int32"] = new Int32();
        m_Types["uint32"] = new UInt32();
        m_Types["int64"] = new Int64();
        m_Types["uint64"] = new UInt64();
        m_Types["float"] = new Float();
        m_Types["double"] = new Double();
        m_Types["string"] = new StringType();
    }

    public Type GetType(string typename)
    {
        return m_Types[typename];
    }

    public bool TypeNameInTable(string typeName)
    {
        return m_Types.ContainsKey(typeName);
    }

    public List<string> GetRegisteredNames()
    {
        return m_Types.Keys.ToList();
    }

    public bool AddTypes(List<TypeInfo> validatedTypes)
    {
        while (validatedTypes.Count > 0)
        {
            TypeInfo TypeToAdd = validatedTypes.First();
            validatedTypes.RemoveAt(0);
             
            if (m_Types.Keys.Contains(TypeToAdd.name))
                continue;

            CompoundType newType = new CompoundType(TypeToAdd.name);

            bool allTypesAreReady = true;
            foreach (var field in TypeToAdd.fields)
            {
                Regex listTypeRegex = new Regex(@"list\((.*)\)");

                Match m = listTypeRegex.Match(field.type);

                bool isList = m.Success;

                if (isList)
                {
                    string innerType = m.Groups[1].Value;

                    // If we don't have the inner type, skip until we do
                    if (!m_Types.Keys.Contains(innerType))
                    {
                        allTypesAreReady = false;
                        break;
                    }

                    // If we have the inner type, but not the list type then make the list type
                    if (!m_Types.Keys.Contains(field.type))
                    {
                        ListType newListType = new ListType(TypeToAdd.name, m_Types[innerType]);
                        m_Types.Add(field.type, newListType);
                    }
                }
                else if (!m_Types.Keys.Contains(field.type))
                {
                    allTypesAreReady = false;
                    break;
                }

                newType.AddField(field.name, m_Types[field.type]);
            }
            
            // This type depends on types that are not added yet,
            // so it goes back in the list.
            if (!allTypesAreReady)
            {
                validatedTypes.Add(TypeToAdd);
                continue;
            }

            m_Types.Add(TypeToAdd.name, newType);
        }

        return true;
    }
    
    // Make sure we have these types or make them if we can
    public bool EnsureTypes(List<string> types)
    {
        foreach (string type in types)
        {
            if (m_Types.Keys.Contains(type))
                continue;

            Regex listTypeRegex = new Regex(@"list\((.*)\)");

            Match m = listTypeRegex.Match(type);

            if (m.Success)
            {
                string innerType = m.Groups[1].Value;

                if (!m_Types.Keys.Contains(innerType))
                    return false;

                ListType newType = new ListType(type, m_Types[innerType]);
                m_Types.Add(type, newType);
            }
        }

        return true;
    }

    // name from yaml to types
    Dictionary<string, Type> m_Types;
}
