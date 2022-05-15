using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

// A lot of this is written with reference to https://codingvision.net/c-inject-a-dll-into-a-process-w-createremotethread

class Injection
{
    [DllImport("kernel32.dll")]
    public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

    [DllImport("kernel32.dll", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
    static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
    public static extern IntPtr GetModuleHandle(string lpModuleName);

    [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
    static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out UIntPtr lpNumberOfBytesWritten);

    [DllImport("kernel32.dll")]
    static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);

    /**
     * This method allows to inject the source code of Injected.cpp and .h, into Anno1800.exe process
     * 
     * Author : egomeh (https://github.com/egomeh)
     **/
    public static bool InjectDLL(string processName, string DLLPath)
    {
        Process[] processes = Process.GetProcessesByName(processName);

        /**
         * Checks every 15 seconds (15,000 ms), if a process has opened with the name in parameter "processName".
         * After 40 tries (40 * 15 seconds = 10 minutes), the loop and finally the program exits interrupt.
         * 
         * Author : Seynax (https://github.com/seynax)
        **/
        {
            int attempt = 0;
            while (true)
            {
                if (attempt++ >= 40)
                {
                    break;
                }

                processes = Process.GetProcessesByName(processName);

                if (processes.Length > 1)
                {
                    Console.WriteLine(String.Format("More than one process found for {0}", processName));
                    Console.WriteLine(String.Format("Using process with id {0}"), processes[0].Id);

                    return false;
                }
                else if (processes != null && processes.Length > 0)
                {
                    break;
                }

                Console.WriteLine(String.Format("No processes with name {0} found", processName));
                Thread.Sleep(15000);
            }

            if (processes == null || processes.Length == 0)
            {
                Console.WriteLine(String.Format("No processes with name {0} found", processName));

                return false;
            }
        }

        // Get process
        Process targetProcess = processes[0];

        // Open process with proper access rights
        int accessRights = 0x0002 | 0x0400 | 0x0008 | 0x0020 | 0x0010;
        IntPtr handle = OpenProcess(accessRights, false, targetProcess.Id);

        // Get the handle to kernel32 module
        IntPtr kernel32Handle = GetModuleHandle("kernel32.dll");

        // Get the address of LoadLibraryA
        IntPtr loadLibraryAddr = GetProcAddress(kernel32Handle, "LoadLibraryA");

        uint pathLength = (uint)((DLLPath.Length + 1) * Marshal.SizeOf(typeof(char)));

        IntPtr allocMemAddress = VirtualAllocEx(handle, IntPtr.Zero, pathLength, 0x3000, 4);

        // Writing the name of the dll there
        UIntPtr bytesWritten;
        WriteProcessMemory(handle, allocMemAddress, Encoding.Default.GetBytes(DLLPath), pathLength, out bytesWritten);

        // Creating a thread that will call LoadLibraryA with allocMemAddress as argument
        IntPtr remoteThreadHandle = CreateRemoteThread(handle, IntPtr.Zero, 0, loadLibraryAddr, allocMemAddress, 0, IntPtr.Zero);

        return true;
    }
}
