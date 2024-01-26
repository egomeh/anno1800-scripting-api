using System.Reflection;

class Program
{
    static void Main(string[] args)
    {
        var assembly = Assembly.GetExecutingAssembly();
        string temporaryPath = Path.GetTempPath();
        string targetDllPath = Path.Combine(temporaryPath, "Injected.dll");

        using (Stream? inStream = assembly.GetManifestResourceStream("Monocle.Injected.dll"))
        using (FileStream outStream = File.OpenWrite(targetDllPath))
        {
            if (inStream != null)
            {
                BinaryReader reader = new BinaryReader(inStream);
                BinaryWriter writer = new BinaryWriter(outStream);

                byte[] buffer = new Byte[1024];
                int bytesRead;

                while ((bytesRead = inStream.Read(buffer, 0, 1024)) > 0)
                {
                    outStream.Write(buffer, 0, bytesRead);
                }
            }
        }

        Injection.InjectDLL("anno1800", Path.GetFullPath(targetDllPath));
    }
}