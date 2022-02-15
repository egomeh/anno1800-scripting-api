using System.Collections;
using System.Diagnostics;
using System.Text.RegularExpressions;
using YamlDotNet.Serialization;

struct FieldEntry
{
    public string name;
    public string type;
}

class TypeInfo
{
    public TypeInfo()
    {
        fields = new List<FieldEntry>();
        name = "<None>";
    }
    
    public string name;
    public List<FieldEntry> fields;
}

//class FunctionInfo
//{
//    public FunctionInfo()
//    {
//        input = new List<string>();
//        output = new List<string>();
//    }

//    public string name;
//    public List<string> input;
//    public List<string> output;
//}

class YamlDataReader
{
    static List<string> GetAllYamlFiles(string directory)
    {
        var files = new List<string>();

        string[] fileEntries = Directory.GetFiles(directory);

        foreach (string fileName in fileEntries)
        {
            if (fileName.EndsWith(".yaml"))
                files.Add(fileName);
        }

        string[] subdirectoryEntries = Directory.GetDirectories(directory);
        foreach (string subdirectory in subdirectoryEntries)
        {
            List<string> subDirectoryFiles = GetAllYamlFiles(subdirectory);
            files.AddRange(subDirectoryFiles);
        }

        return files;
    }

    public static void ReadYamlData(string directory, out List<Object> functions, out List<Object> types)
    {
        functions = new List<Object>();
        types = new List<Object>();

        List<string> yamlFiles = GetAllYamlFiles(directory);

        foreach (string yamlFile in yamlFiles)
        {
            var deserializer = new Deserializer();
            string yamlContent = File.ReadAllText(yamlFile);
            var result = deserializer.Deserialize<List<Hashtable>>(new StringReader(yamlContent));

            foreach (var item in result)
            {
                foreach (DictionaryEntry entry in item)
                {
                    List<Object>? items = entry.Value as List<Object>;

                    if (items == null)
                        continue;

                    if (entry.Key.Equals("functions"))
                    {
                        functions.AddRange(items);
                    }
                    else if (entry.Key.Equals("types"))
                    {
                        types.AddRange(items);
                    }
                }
            }
        }
    }

    public static List<TypeInfo> GetTypeInfo(List<Object> data)
    {
        List<TypeInfo> types = new List<TypeInfo>();

        foreach (Object item in data)
        {
            var type = item as Dictionary<Object, Object>;
            Debug.Assert(type != null);
            string? name = type.Keys.First() as string;
            Debug.Assert(!string.IsNullOrEmpty(name));

            TypeInfo typeInfo = new TypeInfo();
            typeInfo.name = name;

            var fieldsSection = type[name] as List<Object>;
            Debug.Assert(fieldsSection != null);

            // This checks that there is only one `fields` section.
            Debug.Assert(fieldsSection.Count == 1);

            var fieldsDicionary = fieldsSection[0] as Dictionary<Object, Object>;
            Debug.Assert(fieldsDicionary != null);

            var fields = fieldsDicionary["fields"] as List<Object>;
            Debug.Assert(fields != null);

            foreach (var fieldItem in fields)
            {
                var field = fieldItem as Dictionary<Object, Object>;
                Debug.Assert(field != null);

                string? fieldName = field.Keys.First() as string;
                Debug.Assert(!string.IsNullOrEmpty(fieldName));

                string? fieldType = field[fieldName] as string;
                Debug.Assert(!string.IsNullOrEmpty(fieldType));

                FieldEntry entry = new FieldEntry();
                entry.name = fieldName;
                entry.type = fieldType;

                typeInfo.fields.Add(entry);
            }

            types.Add(typeInfo);
        }

        return types;
    }

    public static bool ValidateTypes(List<TypeInfo> customTypes, List<string> builtinTypes, out string error)
    {
        error = string.Empty;

        var customTypeNames = customTypes.Select(x => x.name).ToList();

        List<string> allTypesNames = customTypeNames;
        allTypesNames.AddRange(builtinTypes);

        foreach (string typeName in allTypesNames)
        {
            if (allTypesNames.Where(x => x.Equals(typeName)).Count() > 1)
            {
                error = String.Format("Type {0} defined multiple times", typeName);
                return false;
            }
        }

        foreach (TypeInfo typeInfo in customTypes)
        {
            foreach (var field in typeInfo.fields)
            {
                string typename = field.type;

                Regex listTypeRegex = new Regex(@"list\((.*)\)");

                // This is a bit hacky but it'll do for now.
                while (true)
                {
                    Match m = listTypeRegex.Match(typename);

                    if (!m.Success)
                        break;

                    typename = m.Groups[1].Value;
                }

                int timesDefined = allTypesNames.Where(x => x.Equals(typename)).Count();

                if (timesDefined == 1)
                    continue;

                if (timesDefined == 0)
                    error = String.Format("Type {0} has field {1} of undefnied type {2}", typeInfo.name, field.name, typename);
                else
                    error = String.Format("Type {0} has field {1} of type {2} defined more than once", typeInfo.name, field.name, typename);

                return false;
            }
        }

        return true;
    }
}


