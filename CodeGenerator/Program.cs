
using System.Collections;
using YamlDotNet.Serialization;

Console.WriteLine("Generating type and function code...");

List<Object> functions;
List<Object> types;

YamlDataReader.ReadYamlData("data", out functions, out types);

List<TypeInfo> typeNames = YamlDataReader.GetTypeInfo(types);

TypeTable typeTable = new TypeTable();
string validationError;

if (!YamlDataReader.ValidateTypes(typeNames, typeTable.GetRegisteredNames(), out validationError))
{
    Console.WriteLine(validationError);
    return 1;
}

List<string> referencedTypeNames = YamlDataReader.GetReferencedTypes(functions);

if (!typeTable.AddTypes(typeNames))
    return 1;

if (!typeTable.EnsureTypes(referencedTypeNames))
    return 1;

FunctionTable functionTable = new FunctionTable();
functionTable.AddFunctions(typeTable, functions);

string currentDirectory = Directory.GetCurrentDirectory();

string MonocleSourcePath = Path.GetFullPath(Path.Join(currentDirectory, "..", "Monocle"));
string InjectedSourcePath = Path.GetFullPath(Path.Join(currentDirectory, "..", "Injected"));

CodeGenerator codeGenerator = new CodeGenerator(InjectedSourcePath, MonocleSourcePath, typeTable, functionTable);
codeGenerator.GenerateCode();

Console.WriteLine("Done generating type and function code");

return 0;


