// Auto generated code
using System.Collections.Generic;

public struct IslandResource
{
    public IslandResource()
    {
        type_id = default;
        name = "";
        amount = default;
        capacity = default;
        amount_ptr = default;
    }

    public int type_id;
    public string name;
    public int amount;
    public int capacity;
    public ulong amount_ptr;
}


public struct ResourceConsumption
{
    public ResourceConsumption()
    {
        type_id = default;
        name = "";
        rate = default;
    }

    public int type_id;
    public string name;
    public float rate;
}


public struct IslandInfo
{
    public IslandInfo()
    {
        island_id = default;
        name = "";
        debug_address = default;
    }

    public uint island_id;
    public string name;
    public ulong debug_address;
}


public struct ShipInfo
{
    public ShipInfo()
    {
        id = default;
        debug_address = default;
    }

    public uint id;
    public ulong debug_address;
}

