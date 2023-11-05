#pragma once

#include "structs.gen.h"
#include "hook.h"
#include "anno_native.h"
#include "ui.h"

#include <unordered_map>
#include <memory>

class AnnoComponent
{
public:
	AnnoComponent(uint32_t id, uint64_t _address);
	virtual ~AnnoComponent();

	uint64_t GetAddress();

	virtual void Render();
	
	uint32_t GetId();
	virtual std::string GetName();

protected:
	uint32_t id;
	uint64_t address;
};

class AnnoComponent_Name : public AnnoComponent
{
public:
	AnnoComponent_Name(uint32_t id, uint64_t _address);
	virtual ~AnnoComponent_Name();

	virtual void Render() override;

	virtual std::string GetName();

private:

	std::string NameOfComponent;
};

class AnnoComponent_InternalStorage : public AnnoComponent
{
public:
	AnnoComponent_InternalStorage(uint32_t id, uint64_t _address);
	virtual ~AnnoComponent_InternalStorage();

	virtual void Render() override;

	virtual std::string GetName();

private:

	struct StorageItem
	{
		std::string TypeName;
		uint32_t Capacity;
		uint32_t Amount;
	};

	std::vector<StorageItem> StorageItems;
};

class AnnoComponent_Production : public AnnoComponent
{
public:
	AnnoComponent_Production(uint32_t id, uint64_t _address);
	virtual ~AnnoComponent_Production();

	virtual void Render() override;

	virtual std::string GetName();

private:

	struct SecondaryProduction
	{
		uint32_t type;
		std::string name;

		// The ratio for rate
		int nthCycle;
	};

	uint32_t ProductionOutputId;
	std::string ProductionOutputName;

	float Efficiency;
	uint32_t cycle_base_time_ms;
	float time_left_in_cycle;
	float cycle_time_multiplier;
	uint32_t cycle_count;

	std::vector<SecondaryProduction> SecondaryProductions;
};

class AnnoComponent_Movement : public AnnoComponent
{
public:
	AnnoComponent_Movement(uint32_t id, uint64_t _address);
	virtual ~AnnoComponent_Movement();

	virtual void Render() override;

	virtual std::string GetName();

private:

	struct WayPoint
	{
		float x;
		float y;
	};

	float x, y, rotation;
	std::vector<WayPoint> Waypoints;
};


class AnnoEntity
{
public:
	AnnoEntity(uint64_t address);
	virtual ~AnnoEntity();

	uint64_t GetAddress();
	std::string GetTypeName();
	uint32_t GetTypeId();

	float GetX();
	float GetY();
	float GetRotation();

	std::vector<std::shared_ptr<AnnoComponent>> GetComponents();

private:

	uint64_t address;
	std::string TypeName;
	uint32_t TypeId;

	float x, y, rotation;

	std::vector<std::shared_ptr<AnnoComponent>> Components;
};

std::shared_ptr<AnnoComponent> GetComponentFromEntity(AnnoEntity& Entity, uint32_t ComponentId);


