#include "testing.h"
#include "serialization.gen.h"
#include "structs.gen.h"
#include "remote_call_handler_test.h"

#include <vector>

void testing()
{
	Ship ship;
	ship.name = "Hi I'm a little ship, how's it going?";

	CargoSlot slot1;
	slot1.itemID = 1010223;
	slot1.volume = 4;

	CargoSlot slot2;
	slot2.itemID = 1010224;
	slot2.volume = 34;

	ship.cargo.cargo.push_back(slot1);
	ship.cargo.cargo.push_back(slot2);

	Ship deserializedShip;

	std::vector<uint8_t> data;

	Serialize(ship, data);

	size_t cursor = 0;
	Deserialize(&deserializedShip, data, &cursor);

	RemoteCallHandlerTest handler;
}

