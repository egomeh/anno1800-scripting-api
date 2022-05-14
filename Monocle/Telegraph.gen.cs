// Auto generated code
using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Net;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Text.RegularExpressions;

public enum TelegramMode
{
    Inject,
    Testing,
}

class Windows
{
    [DllImport("kernel32.dll")]
    public static extern IntPtr LoadLibrary(string dllToLoad);
};

/**
 * The program sets up a local network and listens in order to accept a future client.
 * Then it injects a source code in the form of .dll in the Anno1800.exe process through the class Injection.cs
 * After injecting, the program sets up a gateway through a local network between itself and the injected code
 * 
 * Author : egomeh (https://github.com/egomeh)
 * 
 **/
public class Telegraph
{
    readonly Socket? m_Socket;

    public Telegraph(TelegramMode mode = TelegramMode.Inject)
    {
        // local ip search
        IPHostEntry ipHostInfo = Dns.GetHostEntry("localhost");
        IPAddress ipAddress = ipHostInfo.AddressList[0];

        /**
         * Searches for the first IP that matches the pattern "x.x.x.x" with x between one and three digits
         * (This is a resolution that prevents the program from using a non-functional ip (:::1) as a user had)
         * 
         * Author : Seynax (https://github.com/seynax)
        **/
        {
            for (int i = 0; i < ipHostInfo.AddressList.Count(); i++)
            {
                IPAddress tempIpAddress = ipHostInfo.AddressList[i];

                if (tempIpAddress == null)
                    continue;

                if (Regex.IsMatch(tempIpAddress.ToString(), "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}"))
                {
                    ipAddress = tempIpAddress;

                    break;
                }
            }
        }

        // Opening the connection
        IPEndPoint localEndPoint = new IPEndPoint(ipAddress, 4050);
        Socket listener = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

        // Bind to the listener and wait for accept a new client (the source code injected.dll)
        listener.Bind(localEndPoint);
        listener.Listen(1);
        Task<Socket> acceptedSocket = listener.AcceptAsync();

        /**
         * Source code injection (injected.dll) or load library
         **/
        {
            var assembly = Assembly.GetExecutingAssembly();
            string temporaryPath = Path.GetTempPath();
            string targetDllPath = Path.Combine(temporaryPath, "Injected.dll");

            using (Stream? inStream = assembly.GetManifestResourceStream("Monocle.Injected.dll"))
            {
                if (inStream == null)
                    return;

                using (FileStream outStream = File.OpenWrite(targetDllPath))
                {
                    BinaryReader reader = new(inStream);
                    BinaryWriter writer = new(outStream);

                    byte[] buffer = new Byte[1024];
                    int bytesRead;

                    while ((bytesRead = inStream.Read(buffer, 0, 1024)) > 0)
                    {
                        outStream.Write(buffer, 0, bytesRead);
                    }
                }
            }

            if (mode == TelegramMode.Testing)
                Windows.LoadLibrary(@"../x64/Release/Injected.dll");
            else
                Injection.InjectDLL("anno1800", Path.GetFullPath(targetDllPath));
        }

        // Waiting for instructions to complete
        acceptedSocket.Wait();
        m_Socket = acceptedSocket.Result;
    }


    public bool Exchange(List<byte> payload, out List<byte> response)
    {
        response = new List<byte>();

        ulong payloadSize = (ulong)payload.Count + 8uL;

        List<byte> finalPayload = new List<byte>();

        if (!Serializer.Serialize(payloadSize, finalPayload))
            return false;

        finalPayload.AddRange(payload);

        if (m_Socket == null)
            return false;

        int sentBytes = m_Socket.Send(finalPayload.ToArray());

        if (sentBytes != finalPayload.Count)
            return false;

        byte[] tempBuffer = new byte[512];

        int readBytes = m_Socket.Receive(tempBuffer);

        for (int i = 0; i < readBytes; ++i)
            response.Add(tempBuffer[i]);

        ulong expectedSize = 0;
        int offset = 0;

        if (!Serializer.Deserialize(out expectedSize, tempBuffer, offset, out offset))
            return false;

        while (response.Count < (int)expectedSize)
        {
            readBytes = m_Socket.Receive(tempBuffer);

            for (int i = 0; i < readBytes; ++i)
                response.Add(tempBuffer[i]);
        }

        bool success = false;
        offset = 8;

        if (!Serializer.Deserialize(out success, response.ToArray(), offset, out offset))
            return false;

        if (!success)
            return false;

        // If we succeeded, cut out the size and succes values form the stream
        response.RemoveRange(0, 9);

        return true;
    }
    public bool WriteMessageBox(string message)
    {
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 1;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(message, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        return true;
    }

    public bool GetGameTime(out ulong time, out ulong frame)
    {
        time = default;
        frame = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 2;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out time, inData, offset, out offset))
            return false;

        if (!Serializer.Deserialize(out frame, inData, offset, out offset))
            return false;

        return true;
    }

    public bool GetPlayerIslandsInWorld(uint area, out List<IslandInfo> islands)
    {
        islands = new List<IslandInfo>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 3;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(area, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out islands, inData, offset, out offset))
            return false;

        return true;
    }

    public bool GetAllIslandsOfWorld(uint area, out List<IslandInfo> islands)
    {
        islands = new List<IslandInfo>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 22;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(area, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out islands, inData, offset, out offset))
            return false;

        return true;
    }

    public bool GetAllAreas(out List<uint> areas)
    {
        areas = new List<uint>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 4;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out areas, inData, offset, out offset))
            return false;

        return true;
    }

    public bool GetIslandResources(uint areaCode, uint islandId, out List<IslandResource> resources)
    {
        resources = new List<IslandResource>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 5;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(areaCode, outgoingData))
            return false;

        if (!Serializer.Serialize(islandId, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out resources, inData, offset, out offset))
            return false;

        return true;
    }

    public bool GetIslandResidentialConsumption(uint areaCode, uint islandId, out List<ResourceConsumption> resources)
    {
        resources = new List<ResourceConsumption>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 6;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(areaCode, outgoingData))
            return false;

        if (!Serializer.Serialize(islandId, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out resources, inData, offset, out offset))
            return false;

        return true;
    }

    public bool GetIslandIndustrialConversion(uint areaId, uint islandId, out List<ResourceConsumption> resources)
    {
        resources = new List<ResourceConsumption>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 7;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(areaId, outgoingData))
            return false;

        if (!Serializer.Serialize(islandId, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out resources, inData, offset, out offset))
            return false;

        return true;
    }

    public bool MinMaxResourcesOnIsland(uint areaId, uint islandId, uint lowerBound, uint upperBound)
    {
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 8;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(areaId, outgoingData))
            return false;

        if (!Serializer.Serialize(islandId, outgoingData))
            return false;

        if (!Serializer.Serialize(lowerBound, outgoingData))
            return false;

        if (!Serializer.Serialize(upperBound, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        return true;
    }

    public bool DebugGetResourceInfoFromAddress(ulong address, out IslandResource resource)
    {
        resource = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 9;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out resource, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetResourceChainInfoFromAddress(ulong address, out List<IslandResource> resource)
    {
        resource = new List<IslandResource>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 10;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out resource, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetIslandNameFromAddress(ulong address, out string name)
    {
        name = "";
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 11;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out name, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugReadStringFromAddress(ulong address, out string name)
    {
        name = "";
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 12;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out name, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetIslandResources(ulong address, out List<IslandResource> resources)
    {
        resources = new List<IslandResource>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 13;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out resources, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetIslandChainFromAddress(ulong address, out List<IslandInfo> islands)
    {
        islands = new List<IslandInfo>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 14;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out islands, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetFirstAreaStructAddress(out ulong address)
    {
        address = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 15;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out address, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetAreaWithCode(uint code, out ulong address)
    {
        address = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 16;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(code, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out address, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetNameFromGuid(uint guid, out string name)
    {
        name = "";
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 17;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(guid, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out name, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetGuidFromName(string name, out uint guid)
    {
        guid = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 18;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(name, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out guid, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugVirtualGetComponentFromAddress(ulong address, ulong componentId, out ulong componentAddress)
    {
        componentAddress = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 19;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(address, outgoingData))
            return false;

        if (!Serializer.Serialize(componentId, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out componentAddress, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugGetIslandBuildingAddresses(uint areaId, uint islandId, out List<ulong> addresses)
    {
        addresses = new List<ulong>();
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 20;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(areaId, outgoingData))
            return false;

        if (!Serializer.Serialize(islandId, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        byte[] inData = response.ToArray();

        int offset = 0;
        if (!Serializer.Deserialize(out addresses, inData, offset, out offset))
            return false;

        return true;
    }

    public bool DebugTryEnqueueShipForTrade(uint areaId, uint islandId, ulong tradeComponent)
    {
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 21;

        if (!Serializer.Serialize(functionIndex, outgoingData))
            return false;

        if (!Serializer.Serialize(areaId, outgoingData))
            return false;

        if (!Serializer.Serialize(islandId, outgoingData))
            return false;

        if (!Serializer.Serialize(tradeComponent, outgoingData))
            return false;

        List<byte> response;

        if (!Exchange(outgoingData, out response))
            return false;

        return true;
    }

}