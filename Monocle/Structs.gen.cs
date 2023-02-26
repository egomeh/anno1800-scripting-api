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


public struct Vector
{
    public Vector()
    {
        x = default;
        y = default;
    }

    public float x;
    public float y;
}


public struct CargoSlot
{
    public CargoSlot()
    {
        occupied = default;
        type_name = "";
        type_id = default;
        amount = default;
    }

    public bool occupied;
    public string type_name;
    public uint type_id;
    public uint amount;
}


public struct ShipInfo
{
    public ShipInfo()
    {
        id = default;
        debug_address = default;
        name = "";
        idle = default;
        position = new Vector();
        rotation = default;
        cargo = new List<CargoSlot>();
    }

    public uint id;
    public ulong debug_address;
    public string name;
    public bool idle;
    public Vector position;
    public float rotation;
    public List<CargoSlot> cargo;
}


public struct DebugComponent
{
    public DebugComponent()
    {
        id = default;
        address = default;
    }

    public ulong id;
    public ulong address;
}

