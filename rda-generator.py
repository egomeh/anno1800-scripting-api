import xml.etree.ElementTree as ET
from collections import defaultdict
import re
from string import Template


def get_assets(filepath, text_by_id):
    assets = []
    xml = ET.parse(filepath).getroot()
    for asset in xml.iter('Asset'):
        asset_id = asset.get('ID')
        text_id = asset.find('T').get('ID')
        assets.append({
            'id': asset_id,
            'name': text_by_id[text_id],
            'identifier_name': re.sub('[^a-zA-Z0-9]', '', text_by_id[text_id]),
            'is_production_building': asset.find('FB') is not None
        })
    return assets


# TODO: find a way to distinguish between same type of resources? (e.g., double coal/electricity/fuel)
def to_identifier_to_asset_map(assets_by_id):
    return dict([(v['identifier_name'], v) for (k,v) in assets_by_id.items()])

def make_enum_values_string(assets_by_id):
    assets_enum = ['Invalid = 0']
    identifier_to_asset_map = to_identifier_to_asset_map(assets_by_id)
    for short_name, asset in identifier_to_asset_map.items():
        print(f"{short_name} = {asset['id']}")
        assets_enum.append(f"{short_name} = {asset['id']}")

    return ",\n    ".join(assets_enum)

def make_id_to_building_cases_string(buildings_by_id):
    id_to_building_cases = []
    for id, building in sorted(buildings_by_id.items()):
        id_to_building_cases.append(f"case {id}: return AutoComms::Building::{building['identifier_name']};")
    id_to_building_cases.append('default: return AutoComms::Building::Invalid;')
    return "\n    ".join(id_to_building_cases)


def make_id_to_resource_cases_string(resources_by_id):
    id_to_resource_cases = []
    for id, resource in resources_by_id.items():
        id_to_resource_cases.append(f"case {id}: return AutoComms::Resource::{resource['identifier_name']};")
    id_to_resource_cases.append('default: return AutoComms::Resource::Invalid;')
    return "\n    ".join(id_to_resource_cases)


def make_resource_to_id_cases_string(resources_by_id):
    id_to_resource_cases = []
    resource_map = to_identifier_to_asset_map(resources_by_id)
    for resource, asset in resource_map.items():
        id_to_resource_cases.append(f"case AutoComms::Resource::{resource}: return {asset['id']};")
    id_to_resource_cases.append('default: return -1;')
    return "\n    ".join(id_to_resource_cases)


def make_csharp_production_buildings_list_values_string(production_buildings):
    buildings = []

    for building in production_buildings:
        buildings.append(f"Building.{building['identifier_name']}")

    return ", ".join(buildings)

def main():
    print('Loading xmls...')
    texts_xml = ET.parse('rda-modified/Texts_English.xml').getroot()
    text_by_id = { node.get('ID'): node.text for node in texts_xml.iter('Text') }

    buildings_file_to_load = ['Assets_FactoryBuilding7', 'Assets_FactoryBuilding7_Arctic', 'Assets_FarmBuilding', 'Assets_FarmBuilding_Arctic', 'Assets_Farmfield', 'Assets_FreeAreaBuilding', 'Assets_FreeAreaBuilding_Arctic', 'Assets_HarborWarehouse7', 'Assets_HarborWarehouse7', 'Assets_Slot', 'Assets_SlotFactoryBuilding7', 'Assets_SlotFactoryBuilding7_Arctic', 'Assets_ResidenceBuilding7', 'Assets_ResidenceBuilding7_Arctic', 'Assets_Market', 'Assets_PublicServiceBuilding', 'Assets_CityInstitutionBuilding']

    production_buildings = []
    buildings_by_id = {}
    for file in buildings_file_to_load:
        print(f'Loading {file}...')
        for asset in get_assets(f"rda-modified/{file}.xml", text_by_id):
            asset_id = asset['id']
            buildings_by_id[asset_id] = asset
            if asset['is_production_building']:
                production_buildings.append(asset)

    resources_by_id = {}
    for asset in get_assets(f"rda-modified/Assets_Product.xml", text_by_id):
        resources_by_id[asset['id']] = asset

    buildings_enum_string = make_enum_values_string(buildings_by_id)
    id_to_building_cases_string = make_id_to_building_cases_string(buildings_by_id)
    resources_enum_string = make_enum_values_string(resources_by_id)
    id_to_resource_cases_string = make_id_to_resource_cases_string(resources_by_id)
    resource_to_id_cases_string = make_resource_to_id_cases_string(resources_by_id)
    production_buildings_list_values_string = make_csharp_production_buildings_list_values_string(production_buildings)

    with open('Q/rda.h', 'w+') as f:
        f.write(f'''// AUTO-GENERATED FILE
#pragma once
#include <inttypes.h>

namespace AutoComms {{
enum class Building : uint64_t {{
    {buildings_enum_string}
}};

enum class Resource : uint64_t {{
    {resources_enum_string}
}};
}}

uint64_t ResourceToID(AutoComms::Resource resource);

AutoComms::Building IDToBuildingType(uint64_t buildingID);
AutoComms::Resource IDToResourceType(uint64_t resourceId);

''')

    with open('Q/rda.cpp', 'w+') as f:
        f.write(f'''// AUTO-GENERATED FILE
#include "pch.h"
#include "rda.h"

AutoComms::Building IDToBuildingType(uint64_t buildingID)
{{
    switch(buildingID)
    {{
    {id_to_building_cases_string}
    }}
}}

AutoComms::Resource IDToResourceType(uint64_t resourceId)
{{
    switch (resourceId)
    {{
    {id_to_resource_cases_string}
    }}
}}

uint64_t ResourceToID(AutoComms::Resource resource)
{{
    switch (resource)
    {{
    {resource_to_id_cases_string}
    }}
}}
''')

    with open('monocle/monocle/rda.cs', 'w+') as f:
        f.write(f'''// AUTO-GENERATED FILE
using System;
using System.Collections.Generic;

namespace monocle {{

public enum Building : ulong {{
    {buildings_enum_string}
}}

public enum Resource : ulong {{
    {resources_enum_string}
}}

public static class RDAHelper {{
    public static ICollection<Building> GetProductionBuildings() {{
        return new[] {{ {production_buildings_list_values_string} }};
    }}
}}

}}

''')

if __name__ == "__main__":
    main()
