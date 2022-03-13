// Auto generated code
using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Net;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;


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

public class Telegraph
{
    Socket m_Socket;

    public Telegraph(TelegramMode mode = TelegramMode.Inject)
    {
        IPHostEntry ipHostInfo = Dns.GetHostEntry("localhost");
        IPAddress ipAddress = ipHostInfo.AddressList[0];
        IPEndPoint localEndPoint = new IPEndPoint(ipAddress, 4050);
        Socket listener = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

        listener.Bind(localEndPoint);
        listener.Listen(1);

        Task<Socket> acceptedSocket = listener.AcceptAsync();

        if (mode == TelegramMode.Testing)
            Windows.LoadLibrary("../x64/Debug/Injected.dll");
        else
            Injection.InjectDLL("anno1800", Path.GetFullPath(@"../x64/Debug/Injected.dll"));

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

    public bool DebugGetResourceInfoFromAddress(ulong address, out IslandResource resource)
    {
        resource = default;
        List<byte> outgoingData = new List<byte>();

        ulong functionIndex = 3;

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

        ulong functionIndex = 4;

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

        ulong functionIndex = 5;

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

        ulong functionIndex = 6;

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

        ulong functionIndex = 7;

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

        ulong functionIndex = 8;

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

        ulong functionIndex = 9;

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

}