import sys
import yaml


class DataType():

    def __init__(self, name):

        self.name = name

    def get_name(self):

        return self.name

    def get_cpp_name(self):

        return None

    def get_cpp_def(self):

        return None

    def get_cs_name(self):

        return None

    def get_cs_def(self):

        return None

    def get_cs_serialization(self):

        return None

    def get_cpp_serialization(self):

        return None

    def get_cpp_deserialization(self):

        return None

    def get_cpp_serialization_impl(self):

        return None

    def get_cpp_deserialization_impl(self):

        return None

    def get_python_code(self):

        return None


class BasicType(DataType):

    def __inti__(self):

        pass

    def get_name(self):

        return self.name

    def get_cpp_name(self):

        return self.cpp_name

    def get_cpp_def(self):

        return None

    def get_cs_name(self):

        return self.cs_name


class bool(BasicType):

    def __init__(self):

        self.name = "bool"
        self.cpp_name = "bool"
        self.cs_name = "bool"


class uint64_t(BasicType):

    def __init__(self):

        self.name = "uint64_t"
        self.cpp_name = "uint64_t"
        self.cs_name = "UInt64"


class float(BasicType):

    def __init__(self):

        self.name = "float"
        self.cpp_name = "float"
        self.cs_name = "float"


class double(BasicType):

    def __init__(self):

        self.name = "double"
        self.cpp_name = "double"
        self.cs_name = "double"


class string(BasicType):

    def __init__(self):

        self.name = "string"
        self.cpp_name = "std::string"
        self.cs_name = "string"


class CompoundType(DataType):

    def __init__(self, name, cpp_def, cpp_name, cpp_serial, cpp_deserial, cpp_serial_impl, cpp_deserial_impl, py_code, cs_def, cs_serialization):

        self.name = name
        self.cpp_def = cpp_def
        self.cpp_name = cpp_name
        self.cpp_serial = cpp_serial
        self.cpp_deserial = cpp_deserial
        self.cpp_serial_impl = cpp_serial_impl
        self.cpp_deserial_impl = cpp_deserial_impl
        self.py_code = py_code
        self.cs_name = cpp_name
        self.cs_def = cs_def
        self.cs_serialization = cs_serialization

    def get_cpp_def(self):

        return self.cpp_def

    def get_cpp_name(self):

        return self.cpp_name

    def get_cs_name(self):

        return self.cs_name

    def get_cs_def(self):

        return self.cs_def

    def get_cs_serialization(self):

        return self.cs_serialization

    def get_cpp_serialization(self):

        return self.cpp_serial

    def get_cpp_deserialization(self):

        return self.cpp_deserial

    def get_cpp_serialization_impl(self):

        return self.cpp_serial_impl

    def get_cpp_deserialization_impl(self):

        return self.cpp_deserial_impl

    def get_python_code(self):

        return self.py_code

class EnumType():

    def __init__(self, cpp_def, cs_def, cpp_serialize_header, cpp_serialize_impl, cs_serialize_code):

        self.cpp_def = cpp_def
        self.cs_def = cs_def
        self.cpp_serialize_header = cpp_serialize_header
        self.cpp_serialize_impl = cpp_serialize_impl
        self.cs_serialize_code = cs_serialize_code


def load_comms_data():

    file = open("comms.yaml")
    return yaml.load(file, Loader = yaml.FullLoader)


def load_basic_types():

    basic_types = {}

    basic_types[bool().name] = bool()
    basic_types[uint64_t().name] = uint64_t()
    basic_types[float().name] = float()
    basic_types[double().name] = double()
    basic_types[string().name] = string()

    return basic_types

def parse_list_type(typestring):

    typestring = typestring.strip()

    if not typestring.startswith("list"):

        return None

    typestring = typestring[len("list"):]
    typestring.strip()

    if not typestring[0] == "(":

        return None

    typestring = typestring[1:]

    typestring.strip()

    resultingType = ""

    while not typestring[0].isspace() and not typestring[0] == ")":

        resultingType += typestring[0]
        typestring = typestring[1:]

    typestring = typestring.strip()

    if not typestring == ")":

        return None

    return resultingType


def get_data_structures(data_section, data_types, enums):

    print("Current data types: {}".format(", ".join([t for t in data_types])))

    struct_template = """struct {name}
{{
{fields}
}};
"""

    serialization_header_template = "bool Serialize(const {typename}& data, std::vector<uint8_t>& stream);"
    deserialization_header_template = "bool Deserialize({typename} *data, const std::vector<uint8_t>& stream, size_t* offset);"

    serialize_impl_template = """bool Serialize(const {typename}& data, std::vector<uint8_t>& stream)
{{
{code}
    return true;
}}"""

    serialize_field_template = "    if (!Serialize(data.{field}, stream))\n        return false;\n"

    deserialize_impl_template = """bool Deserialize({typename}* data, const std::vector<uint8_t>& stream, size_t* offset)
{{
{code}
    return true;
}}"""

    deserialize_field_template = "    if (!Deserialize(&data->{field}, stream, offset))\n        return false;\n"

    field_template = "{type} {name};"

    py_code_template = """
class {name}:

    def __init__(self{init_params}):

{fields}

def serialize_{name}(value):

    data = b''
{serialization}
    return data


def deserialize_{name}(data, offset):

    value = {name}()
    offset = 0
{deserialization}
    return value, offset
"""

    cs_struct_template = """    public struct {name}
    {{
{fields}
    }}
"""

    cs_field_template = "        public {typename} {name};"
    cs_list_field_template = """        {typename} m_{name};
        public {typename} {name}
        {{
            get
            {{
                if ( m_{name} == null )
                    m_{name} = new {typename}();

                return m_{name};
            }}
            set {{ m_{name} = value; }}
        }}"""

    cs_serialization_template = """        public static bool Serialize({typename} data, List<byte> buffer)
        {{{serialize_fields}
            return true;
        }}

        public static bool Deserialize(out {typename} data, byte[] buffer, int offset, out int offsetAfter)
        {{
            data = default;
            offsetAfter = offset;
{deserialize_fields}
            return true;
        }}

        public static bool Serialize(List<{typename}> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                if (!Serialize(data[i], buffer))
                    return false;
            }}

            return true;
        }}

        public static bool Deserialize(out List<{typename}> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<{typename}>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                {typename} element = default;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}
"""

    for structure in data_section:

        cpp_fields = []

        key = list(structure.keys())[0]

        fields = structure[key]

        python_fields = []
        python_init_params = []
        python_serialization = []
        python_deserialization = []

        cs_fields = []

        cs_serialization = []

        for field in fields:

            name = list(field.keys())[0]
            field_type_name = field[name][0]["type"]

            is_list = False

            list_type = parse_list_type(field_type_name)

            if not list_type is None:

                field_type_name = list_type
                is_list = True



            if field_type_name in data_types:

                field_type = data_types[field_type_name]
                final_field_typename = field_type.get_cpp_name()
                cs_field_typename = field_type.get_cs_name()

            elif field_type_name in enums:

                field_type = field_type_name
                final_field_typename = field_type_name
                cs_field_typename = field_type_name

            else:

                print("'{}' does not exist".format(field_type_name))

                sys.exit(0)

            python_serial = (" " * 4) + "data += serialize_{typename}(value.{name})".format(typename = field_type_name, name = name)
            python_deserial = (" " * 4) + "value.{name}, offset = deserialize_{typename}(data, offset)".format(typename = field_type_name, name = name)

            if is_list:

                final_field_typename = "std::vector<{}>".format(final_field_typename)
                cs_field_typename = "List<{}>".format(cs_field_typename)

                python_serial = (" " * 4) + "data += serialize_list(value.{name}, serialize_{name})".format(typename = field_type_name, name = name)
                python_deserial = (" " * 4) + "value.{name}, offset = deserialize_list(value.{name}, serialize_{typename})".format(typename = field_type_name, name = name)

            python_serialization.append(python_serial)
            python_deserialization.append(python_deserial)

            cpp_field = field_template.format(type = final_field_typename, name = name)
            cpp_fields.append(cpp_field)
            python_fields.append((" " * 8) + "self.{name} = {name}".format(name = name))
            python_init_params.append("{name} = None".format(name = name))

            cs_field = cs_field_template.format(typename = cs_field_typename, name = name)

            # if is_list:

                # cs_field = cs_list_field_template.format(typename = cs_field_typename, name = name)

            cs_fields.append(cs_field)

        cpp_def = struct_template.format(name = key,fields = "\n".join(["    {}".format(field) for field in cpp_fields]))
        cpp_name = key
        cpp_serial = serialization_header_template.format(typename = cpp_name)
        cpp_deserial = deserialization_header_template.format(typename = cpp_name)

        cs_def = cs_struct_template.format(name = cpp_name, fields = "\n".join(cs_fields))

        cs_serialization_field_template = """
            if (!Serialize(data.{fieldname}, buffer))
                return false;
"""

        cs_deserialization_field_template = """
            if (!Deserialize(out data.{fieldname}, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;
"""

        serialize_fields = []
        deserialize_fields = []

        cs_serialize_fields = []
        cs_deserialize_fields = []

        for field in fields:

            serialize_fields.append(serialize_field_template.format(field = list(field.keys())[0]))
            deserialize_fields.append(deserialize_field_template.format(field = list(field.keys())[0]))

            cs_serialize_fields.append(cs_serialization_field_template.format( fieldname = list(field.keys())[0]))
            cs_deserialize_fields.append(cs_deserialization_field_template.format( fieldname = list(field.keys())[0]))

        cpp_serial_impl = serialize_impl_template.format(typename = cpp_name, code = "\n".join(serialize_fields))
        cpp_deserial_impl = deserialize_impl_template.format(typename = cpp_name, code = "\n".join(deserialize_fields))

        serialize_fields_code = "".join(cs_serialize_fields)
        deserialize_fields_code = "".join(cs_deserialize_fields)
        cs_serialize_code = cs_serialization_template.format(typename = cpp_name, serialize_fields = serialize_fields_code, deserialize_fields = deserialize_fields_code)

        python_field_string = (" " * 8) + "pass"
        python_init_string = ""

        if len(python_fields) > 0:

            python_field_string = "\n".join(python_fields)

            python_init_string = ", " + ", ".join(python_init_params)

        python_serialization_string = "\n".join(python_serialization)
        python_deserialization_string = "\n".join(python_deserialization)

        py_code = py_code_template.format(name = key, fields = python_field_string, init_params = python_init_string, serialization = python_serialization_string, deserialization = python_deserialization_string)

        data_types[key] = CompoundType(key, cpp_def, cpp_name, cpp_serial, cpp_deserial, cpp_serial_impl, cpp_deserial_impl, py_code, cs_def, cs_serialize_code)



def get_enums(enums):

    file = open("enums.yaml")
    enum_data = yaml.load(file, Loader = yaml.FullLoader)["enums"]

    cpp_enum_defs = []
    cs_enum_defs = []

    for enum_type in enum_data:

        value_counter = 0

        value_strings = ["Invalid = 0"]

        for enum_type_name in enum_type:

            values = enum_type[enum_type_name]

            for value in values:

                for value_name in value:

                    value_counter = value_counter + 1

                    value_string = value[value_name]

                    value_stings_entry = "{entry_name} = {value_counter}".format(entry_name = value_name, value_counter = value_counter)

                    value_strings.append(value_stings_entry)

        cpp_enum_def = """enum class {name} : uint64_t
{{
{values}
}};
""".format(name = enum_type_name, values = "\n".join([" " * 4 + s + "," for s in value_strings]))

        cs_enum_def = """    public enum {name} : ulong
    {{
{values}
    }};
""".format(name = enum_type_name, values = "\n".join([" " * 8 + s + "," for s in value_strings]))

        cs_serialization_code = """        public static bool Serialize({enum_type} data, List<byte> buffer)
        {{
            return Serialize((ulong)data, buffer);
        }}

        public static bool Deserialize(out {enum_type} data, byte[] buffer, int offset, out int offsetAfter)
        {{
            data = {enum_type}.Invalid;
            ulong temp = 0;
            offsetAfter = offset;

            if (!Deserialize(out temp, buffer, offset, out offsetAfter))
                return false;

            data = ({enum_type})temp;

            return true;
        }}

        public static bool Serialize(List<{enum_type}> data, List<byte> buffer)
        {{
            UInt64 size = (UInt64)data.Count;

            if (!Serialize(size, buffer))
                return false;

            for (int i = 0; i < data.Count; ++i)
            {{
                ulong d = 0;
                if (!Serialize(d, buffer))
                    return false;
                data[i] = ({enum_type})d;
            }}

            return true;
        }}

        public static bool Deserialize(out List<{enum_type}> data, byte[] buffer, int offset, out int offsetAfter)
        {{
            ulong size = 0;
            data = new List<{enum_type}>();
            offsetAfter = offset;

            if (!Deserialize(out size, buffer, offsetAfter, out offsetAfter))
                return false;

            for (ulong i = 0; i < size; ++i)
            {{
                {enum_type} element = 0;
                if (!Deserialize(out element, buffer, offsetAfter, out offsetAfter))
                    return false;
                data.Add(element);
            }}

            return true;
        }}""".format(enum_type = enum_type_name)

        cpp_serialize_header = """bool Serialize(const {enum_type}& data, std::vector<uint8_t>& stream);
bool Deserialize({enum_type} *data, const std::vector<uint8_t>& stream, size_t* offset);""".format(enum_type = enum_type_name)

        cpp_serialize_impl = """bool Serialize(const {enum_type}& data, std::vector<uint8_t>& stream)
{{
    return Serialize((const uint64_t)data, stream);
}}

bool Deserialize({enum_type} *data, const std::vector<uint8_t>& stream, size_t* offset)
{{
    return Deserialize((uint64_t*)data, stream, offset);
}}""".format(enum_type = enum_type_name)

        enum_data_type = EnumType(cpp_enum_def, cs_enum_def, cpp_serialize_header, cpp_serialize_impl, cs_serialization_code)
        enums[enum_type_name] = enum_data_type


def GenerateAutoCommsHeader(data_types, enums):

    template = ""

    with open("autocomms.h", "r") as file:

        template = file.read()

    struct_defs = []
    serialization_defs = []

    for enum in enums:

        enum_data= enums[enum]

        struct_defs.append(enum_data.cpp_def)
        serialization_defs.append(enum_data.cpp_serialize_header)

    for name in data_types:

        data_type = data_types[name]

        cpp_def = data_type.get_cpp_def()

        if not cpp_def is None:

            struct_defs.append(cpp_def)

        cpp_serial_def = data_type.get_cpp_serialization()

        if not cpp_serial_def is None:

            serialization_defs.append(cpp_serial_def)

        cpp_deserial_def = data_type.get_cpp_deserialization()

        if not cpp_deserial_def is None:

            serialization_defs.append(cpp_deserial_def)

    output = template.format(struct_defs = "\n".join(struct_defs), serialization_defs = "\n".join(serialization_defs))

    with open("Q/autocomms.h", "w") as file:

        file.write(output)


def GenerateAutoCommsImplementation(data_types, enums):

    template = ""

    with open("autocomms.cpp", "r") as file:

        template = file.read()

    implementations = []

    for enum in enums:

        enum_data = enums[enum]

        implementations.append(enum_data.cpp_serialize_impl)

    for name in data_types:

        data_type = data_types[name]

        serial_impl = data_type.get_cpp_serialization_impl()

        if not serial_impl is None:

            implementations.append(serial_impl)

        deserial_impl = data_type.get_cpp_deserialization_impl()

        if not deserial_impl is None:

            implementations.append(deserial_impl)

    output = template.format(serialization_impl = "\n\n".join(implementations))

    with open("Q/autocomms.cpp", "w") as file:

        file.write(output)

def GenerateFunctionCalls(data_types, functionData, enums):

    template = ""
    cpp_template = ""
    cs_template = ""

    with open("autocomms.py", "r") as file:

        template = file.read()

    with open("autocomms_native.h", "r") as file:

        cpp_template = file.read()

    with open("AutoComms.cs", "r") as file:

        cs_template = file.read()

    python_classes = []
    cs_struct_defs = []
    cs_struct_serialize = []

    cs_serialize_defs = []

    for enum in enums:

        enum_data = enums[enum]

        cs_struct_defs.append(enum_data.cs_def)
        cs_serialize_defs.append(enum_data.cs_serialize_code)

    for name in data_types:

        data_type = data_types[name]

        python_code = data_type.get_python_code()
        cs_def = data_type.get_cs_def()
        cs_serialize_def = data_type.get_cs_serialization()

        if not python_code is None:

            python_classes.append(python_code)
            cs_struct_defs.append(cs_def)
            cs_serialize_defs.append(cs_serialize_def)

    python_classes = "\n".join(python_classes)

    function_template = """def {name}({parameters}):

    function_id = {function_id}

    payload = serialize_uint64_t(function_id)
{serialize_parameters}

    result = exchange(payload)

    if result is None:

        return None

    offset = 0

{deserialize_result}

{result}

"""

    cpp_signature_template = "bool TargetCall_{name}({parameters})"

    cpp_handle_case_template = """        case {function_id}: // {name}
        {{
{structs}

{call}

{serialize}
            break;
        }}
"""

    cs_function_template = """        public bool {name}({parameters})
        {{
            List<byte> payload = new List<byte>();
            List<byte> response = default;
            int offset = 0;
            int offsetAfter = offset;
{out_default}
            ulong function_id = {function_id};

            if (!Serializer.Serialize(function_id, payload))
                return false;
{serialize}
            if (!Exchange(payload, out response))
                return false;

            byte[] buffer = response.ToArray();

{deserialize}
            return true;
        }}
    """

    cpp_definitions = []
    cpp_handle_cases = []

    functions = []
    cs_functions = []

    function_id = 0

    for function in functionData:

        function_id += 1

        function_name = list(function.keys())[0]

        params = []
        params_serial = []

        result_deserial = []
        result_return = []

        cpp_parameters = []
        cpp_structs = []

        cpp_input_identifiers = []
        cpp_output_identifiers = []

        thisFunctionData = function[function_name]

        cs_params = []
        cs_out_default = []
        cs_serialize = []
        cs_deserialize = []

        input = {}
        output = {}

        if thisFunctionData is None:

            thisFunctionData = {}

        for item in thisFunctionData:

            kind = list(item.keys())[0]

            if kind == "input" and input == {}:

                input = item["input"]

            elif kind == "output" and output == {}:

                output = item["output"]

            else:

                print("Invalid function parameters")
                sys.exit(0)

        for param in input:

            param_name = list(param.keys())[0]
            param_type = param[param_name][0]["type"]
            symtab_name = param_type

            list_type = parse_list_type(symtab_name)

            is_list = not list_type is None

            if is_list:

                symtab_name = list_type

            cs_name =symtab_name

            if symtab_name in data_types:

                symtab_entry = data_types[symtab_name]
                cs_name = symtab_entry.get_cs_name()

            elif symtab_name in enums:

                cs_name = symtab_name

            else:

                print("{} not foundin symtab nor enums".format(symtab_name))
                sys.exit(0)

            param_typename = param_type

            parameter_name = "{name}_{type}".format(name = param_name, type = param_typename)
            cs_parameter_name = "{type} {name}".format(name = param_name, type = cs_name)

            cs_serialize.append("""            if (!Serializer.Serialize({name}, payload))
                return false;
""".format(name = param_name))

            if is_list:

                cs_parameter_name = "List<{type}> {name}".format(name = param_name, type = cs_name)

            serialize_parameter = "serialize_{type}({name})".format(name = parameter_name, type = param_type)

            list_type = parse_list_type(param_type)

            cpp_type = "std::string" if param_type == "string" else param_type
            cpp_parameter = "const {type}& {name}".format(type = cpp_type, name = param_name)

            if not list_type is None:

                symtab_name = list_type
                param_typename = "{type}_list".format(type = list_type)
                parameter_name = "{name}_{type}".format(name = param_name, type = param_typename)

                serialize_parameter = "serialize_list({name}, serialize_{type})".format(name = parameter_name, type = list_type)

                cpp_type = "std::vector<{type}>".format(type = "std::string" if list_type == "string" else list_type)
                cpp_parameter = "const {type}& {name}".format(type = cpp_type, name = param_name)

            cpp_parameters.append(cpp_parameter)
            cpp_structs.append("""            {typename} {name};
            if (!AutoComms::Deserialize(&{name}, buffer, &offset))
                return false;
""".format(typename = cpp_type, name = param_name))

            cpp_input_identifiers.append(param_name)

            if not symtab_name in data_types and symtab_name not in enums:

                print("Typename {} not in type definition table".format(symtab_name))
                sys.exit(0)

            params.append(parameter_name)
            cs_params.append(cs_parameter_name)

            params_serial.append("{indent}payload += {serialize_parameter}".format(indent = " " * 4, serialize_parameter = serialize_parameter, name = param_name))

        for result in output:

            output_name = list(result.keys())[0]
            output_type = result[output_name][0]["type"]
            symtab_name = output_type

            result_name = "{name}_{type}".format(name = output_name, type = output_type)

            deserialize_output = "{indent}{name}, offset = deserialize_{type}(result, offset)".format(indent = " " * 4, name = result_name, type = output_type)

            list_type = parse_list_type(output_type)

            cpp_name = output_name
            cpp_type = "std::string" if output_type == "string" else output_type
            cpp_parameter = "{type}* {name}".format(type = cpp_type, name = cpp_name)


            is_list = False
            if not list_type is None:

                is_list = True

                symtab_name = list_type
                output_typename = "{type}_list".format(type = list_type)
                output_name = "{name}_{type}".format(name = output_name, type = output_typename)

                deserialize_output = "{indent}{name}, offset = deserialize_list(result, offset, deserialize_{type})".format(indent = " " * 4, name = output_name, type = list_type)
                result_name = output_name

                cpp_type = "std::vector<{type}>".format(type = "std::string" if list_type == "string" else list_type)
                cpp_parameter = "{type}* {name}".format(type = cpp_type, name = cpp_name)

                symtab_entry = data_types[list_type]
                cs_typename = "List<{typename}>".format(typename = symtab_entry.get_cs_name())
            else:
                symtab_entry = data_types[symtab_name]
                cs_typename = format(symtab_entry.get_cs_name())


            cs_deserialize.append("""            if (!Serializer.Deserialize(out {name}, buffer, offset, out offsetAfter))
                return false;

            offset = offsetAfter;
""".format(name = output_name))

            cs_parameter_name = "out {type} {name}".format(name = output_name, type = cs_typename)

            cs_params.append(cs_parameter_name)

            cs_out_default.append("            {name} = default;".format(name = output_name))

            cpp_parameters.append(cpp_parameter)
            cpp_structs.append("            {typename} {name};".format(typename = cpp_type, name = cpp_name))

            cpp_output_identifiers.append(cpp_name)

            result_return.append(result_name)
            result_deserial.append(deserialize_output)

        params_string = ", ".join(params)
        serialize_string = "\n".join(params_serial)

        deserialize_string = "\n".join(result_deserial)

        if len(result_return) > 0:
            return_string = "{i}return ".format(i = " " * 4) + ", ".join(result_return)
        else:
            return_string = ""

        # cpp_signature_templateprint(cpp_parameters)

        cpp_function_signature = cpp_signature_template.format(name = function_name, parameters = ", ".join(cpp_parameters))


        cpp_call_parameters = cpp_input_identifiers + ["&{name}".format(name = name) for name in cpp_output_identifiers]

        cpp_native_call_string = "{i}success = TargetCall_{function_name}({parameters});".format(i = " " * 12,
            function_name = function_name, parameters = ", ".join(cpp_call_parameters));

        cpp_serialize_result_template = """{i}if (!AutoComms::Serialize({name}, outputBuffer))
    {i}return false;
"""

        cpp_serialize_result_string = "\n".join([cpp_serialize_result_template.format(i = " " * 12, name = name) for name in cpp_output_identifiers])

        cpp_definitions.append(cpp_function_signature)
        cpp_handle_cases.append(cpp_handle_case_template.format(function_id = function_id, structs = "\n".join(cpp_structs),
            name = function_name, call = cpp_native_call_string, serialize = cpp_serialize_result_string))

        functions.append(function_template.format(name = function_name,
            parameters = params_string, serialize_parameters = serialize_string, function_id = function_id,
            deserialize_result = deserialize_string, result = return_string))

        cs_params_string = ", ".join(cs_params)
        cs_out_default_string = "\n".join(cs_out_default)
        cs_serialize_string = "\n".join(cs_serialize)
        cs_deserialize_string = "\n".join(cs_deserialize)

        cs_functions.append(cs_function_template.format(name = function_name, parameters = cs_params_string,
            function_id = function_id, out_default = cs_out_default_string, serialize = cs_serialize_string,
            deserialize = cs_deserialize_string))

    functions_string = "\n".join(functions)

    cpp_signature_string = "\n".join(s + ";" for s in cpp_definitions)
    cpp_handle_cases_string = "\n".join(cpp_handle_cases)

    cs_struct_def_string = "\n".join(cs_struct_defs)
    cs_serialization_def_string = "\n".join(cs_serialize_defs)

    cs_functions_string = "\n".join(cs_functions)

    with open("annoscripting/annoscripting/autocomms.py", "w") as file:

        file.write(template.format(classes = python_classes, functions = functions_string))

    with open("TomCruise/autocomms_native.h", "w") as file:

        file.write(cpp_template.format(function_signatures = cpp_signature_string, handle_cases = cpp_handle_cases_string))

    with open("monocle/monocle/AutoComms.cs", "w") as file:

        file.write(cs_template.format(structures = cs_struct_def_string, serialization = cs_serialization_def_string, functions = cs_functions_string))


def GeneratePythonFunctions(data_types, functions):

    print(functions)



def main():

    enums = {}
    data_types = load_basic_types()

    commsData = load_comms_data()

    if commsData is None:

        print("Could not parse data")

        return

    if not "data" in commsData:

        print("No data section found")

        return

    if not "functions" in commsData:

        print("No function section found")

        return

    get_enums(enums)
    get_data_structures(commsData["data"], data_types, enums)

    GenerateAutoCommsHeader(data_types, enums)
    GenerateAutoCommsImplementation(data_types, enums)
    GenerateFunctionCalls(data_types, commsData["functions"], enums)


if __name__ == "__main__":

    main()


