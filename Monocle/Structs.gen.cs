// Auto generated code
using System.Collections.Generic;

    public struct IslandResource
    {
        public IslandResource()
        {
            type_id = default;
            name = "";
            amount_ptr = default;
            amount = default;
            capacity = default;
        }

    public int type_id;             // (uint32_t*)(address+0x10)
    public string name;             // GetNameFromGUID(resourceInfo->type_id, known)
    public ulong amount_ptr;        // address+0x20
    public int amount;              // *(uint32_t*)(resourceInfo->amount_ptr)
    public int capacity;            // (uint32_t*)(address+0x1C)
    }




    public struct ResourceConsumption
    {
        public ResourceConsumption()
        {
            type_id = default;
            name = "";
            rate = default;
        }

        // island_consumption_ptr = *(uint64_t*)(data.rcx + 0x28)
        // i = island_consumption_ptr
        // resource_type = *(uint32_t*)i; (uint32_t)
        public int type_id; 
        public string name; // GetNameFromGUID(resource_type, known)

        // float rate_modifier = *(float*)(i + 0xC);
        // consumption.rate = 60.0f * rate_modifier;
        public float rate;
    }


    public struct IslandInfo
    {
    // resource_struct_address = *(uint64_t*)(island_address + 0x170); (uint64_t)
    // resource_chain_start = **(uint64_t**)(resource_struct_address + 0x40); (uint64_t)

    public IslandInfo()
        {
            island_id = default;
            name = "";
            debug_address = default;
        }

        public uint island_id;          // = *(uint32_t*)(address + 0x10) & 0xffff
        public string name;             // in address+0x118
        public ulong debug_address;     // address+0x0
    }

// AreaInfo
// Island list pointer :
//  - uint64_t intermediate_struct = *(uint64_t*)(area_address + 0x200);
//  - *list_pointer = *(uint64_t*) (intermediate_struct + 0x78);