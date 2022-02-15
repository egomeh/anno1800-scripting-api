

class Type
{
}

class Bool : Type
{
}

class Int32 : Type
{
}

class UInt32 : Type
{
}

class Int64 : Type
{
}

class UInt64 : Type
{
}

class Float : Type
{
}

class Double : Type
{
}

class List
{
    public List(Type elementType)
    {
        m_ElementType = elementType;
    }

    Type m_ElementType;
}

class CompoundType
{
    public CompoundType()
    {
        m_Fields = new List<Type>();
    }

    List<Type> m_Fields;
}

class TypeTable
{
    public TypeTable()
    {
        m_Types = new Dictionary<string, Type>();

        m_Types["bool"] = new Bool();
        m_Types["int32"] = new Int32();
        m_Types["uint32"] = new UInt32();
        m_Types["int64"] = new UInt32();
        m_Types["uint64"] = new UInt64();
        m_Types["float"] = new Float();
        m_Types["double"] = new Double();
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
        return true;
    }

    // name from yaml to types
    Dictionary<string, Type> m_Types;
}
