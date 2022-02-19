// Auto generated code
using System.Collections.Generic;

public struct Coordinate
{
    public Coordinate()
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
        itemID = default;
        volume = default;
    }

    public ulong itemID;
    public uint volume;
}


public struct SomeListyBoi
{
    public SomeListyBoi()
    {
        ids = new List<ulong>();
    }

    public List<ulong> ids;
}


public struct Ship
{
    public Ship()
    {
        name = "";
    }

    public string name;
}


public struct ShipCargo
{
    public ShipCargo()
    {
        cargo = new List<CargoSlot>();
    }

    public List<CargoSlot> cargo;
}

