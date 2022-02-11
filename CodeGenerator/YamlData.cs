using System.Collections;
using YamlDotNet.Serialization;

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
}
