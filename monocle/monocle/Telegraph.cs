using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.Serialization;

namespace monocle
{
    public partial class Telegraph : IDisposable
    {
        public Telegraph()
        {
            m_Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            m_Socket.Connect("localhost", 1800);
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

        public void Dispose()
        {
            Console.WriteLine("Closng socket");

            if (m_Socket.Connected)
                m_Socket.Close();
        }

        private Socket m_Socket;
    }
}
