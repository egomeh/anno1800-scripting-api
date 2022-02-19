// Auto generated code
using System.Collections.Generic;

struct Coordinate
{
    public Coordinate()
    {
        x = default;
        y = default;
    }

    public float x;
    public float y;
}


struct CargoSlot
{
    public CargoSlot()
    {
        itemID = default;
        volume = default;
    }

    public ulong itemID;
    public uint volume;
}


struct SomeListyBoi
{
    public SomeListyBoi()
    {
        ids = new List<ulong>();
    }

    public List<ulong> ids;
}


struct Ship
{
    public Ship()
    {
        name = "";
    }

    public string name;
}


struct ShipCargo
{
    public ShipCargo()
    {
        cargo = new List<CargoSlot>();
    }

    public List<CargoSlot> cargo;
}

