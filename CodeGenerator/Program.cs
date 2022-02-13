
using System.Collections;
using YamlDotNet.Serialization;

Console.WriteLine("Hello world");

List<Object> functions;
List<Object> types;

YamlDataReader.ReadYamlData("data", out functions, out types);

List<TypeInfo> typeNames = YamlDataReader.GetTypeInfo(types);

return 0;

//List<string> GetAllYamlFiles(string directory)
//{
//    var files = new List<string>();

//    string[] fileEntries = Directory.GetFiles(directory);

//    foreach (string fileName in fileEntries)
//    {
//        if (fileName.EndsWith(".yaml"))
//            files.Add(fileName);
//    }

//    string[] subdirectoryEntries = Directory.GetDirectories(directory);
//    foreach (string subdirectory in subdirectoryEntries)
//    {
//        List<string> subDirectoryFiles = GetAllYamlFiles(subdirectory);
//        files.AddRange(subDirectoryFiles);
//    }

//    return files;
//}

//List<string> commandlineArgs = Environment.GetCommandLineArgs().ToList();
//string dataFolder = commandlineArgs[1];
//string outputFolder = commandlineArgs[2];

//List<string> yamlFiles = GetAllYamlFiles(dataFolder);

//foreach (string yamlFile in yamlFiles)
//{
//    var deserializer = new Deserializer();
//    string yamlContent = File.ReadAllText(yamlFile);
//    var result = deserializer.Deserialize<List<Hashtable>>(new StringReader(yamlContent));

//    foreach (var item in result)
//    {
//        Console.WriteLine("Item:");
//        foreach (DictionaryEntry entry in item)
//        {
//            Console.WriteLine("- {0} = {1}", entry.Key, entry.Value);
//        }
//    }
//}

//File.WriteAllText(String.Format(@"{0}/Types.gen.cs", outputFolder), "class Coordinate {}");

//return 0;

