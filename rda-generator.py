import xml.etree.ElementTree as ET
from collections import defaultdict
import re
from string import Template


def main():
    print('Loading xmls...')
    texts_xml = ET.parse('rda-modified/Texts_English.xml').getroot()
    text_by_id = { node.get('ID'): node.text for node in texts_xml.iter('Text') }

    file_to_load = ['Assets_FactoryBuilding7', 'Assets_FactoryBuilding7_Arctic', 'Assets_FarmBuilding', 'Assets_FarmBuilding_Arctic', 'Assets_Farmfield', 'Assets_FreeAreaBuilding', 'Assets_FreeAreaBuilding_Arctic', 'Assets_HarborWarehouse7', 'Assets_HarborWarehouse7', 'Assets_SlotFactoryBuilding7', 'Assets_SlotFactoryBuilding7_Arctic', 'Assets_ResidenceBuilding7', 'Assets_ResidenceBuilding7_Arctic', 'Assets_Market', 'Assets_PublicServiceBuilding']

    buildings_by_id = {}
    for file in file_to_load:
        print(f'Loading {file}...')
        xml = ET.parse(f"rda-modified/{file}.xml").getroot()
        for asset in xml.iter('Asset'):
            asset_id = asset.get('ID')
            text_id = asset.find('T').get('ID')
            buildings_by_id[asset_id] = {
                'name': text_by_id[text_id],
                'identifier_name': re.sub('[^a-zA-Z0-9]', '', text_by_id[text_id])
            }

    buildings_enum = ['Invalid = 0']
    unique_buildings = set(map(lambda x: x['identifier_name'], buildings_by_id.values()))
    for idx, short_name in enumerate(unique_buildings):
        print(f'{short_name} = {idx}')
        buildings_enum.append(f'{short_name} = {idx + 1}')

    buildings_enum_string = ",\n    ".join(buildings_enum)

    id_to_building_cases = []
    for id, building in buildings_by_id.items():
        id_to_building_cases.append(f"case {id}: return AutoComms::Building::{building['identifier_name']}")
    id_to_building_cases.append('default: return AutoComms::Building::Invalid;')
    id_to_building_cases_string = ";\n    ".join(id_to_building_cases)

    with open('Q/rda.h', 'w+') as f:
        f.write(f'''// AUTO-GENERATED FILE
#pragma once
#include <inttypes.h>

namespace AutoComms {{
enum class Building : uint64_t {{
    {buildings_enum_string}
}};
}}

AutoComms::Building IDToBuildingType(uint64_t buildingID);
''')

    with open('Q/rda.cpp', 'w+') as f:
        f.write(f'''// AUTO-GENERATED FILE
#include "pch.h"
#include "rda.h"

AutoComms::Building IDToBuildingType(uint64_t buildingID) {{
    switch(buildingID) {{
    {id_to_building_cases_string}
    }}
}}
''')

if __name__ == "__main__":
    main()
