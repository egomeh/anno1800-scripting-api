using System.Net;
using System.Net.Http;
using System.Reflection.Metadata;
using System.Text;

class StatsServer
{
    static void Main()
    {
        Console.WriteLine("Starting Stats Server");

        StatsServer Server = new StatsServer();
        Server.RunServer();
    }

    void RunServer()
    {
        Task HandleTask = HandleRequest();
        HandleTask.GetAwaiter().GetResult();
    }

    async Task HandleRequest()
    {
        Telegraph telegraph = new Telegraph();

        HttpListener Listener = new HttpListener();
        Listener.Prefixes.Add("http://localhost:8018/");
        Listener.Start();

        while (true)
        {
            HttpListenerContext ListenerContext = await Listener.GetContextAsync();

            HttpListenerRequest Request = ListenerContext.Request;
            HttpListenerResponse Response = ListenerContext.Response;

            Console.WriteLine(Request.Url.ToString());
            Console.WriteLine(Request.HttpMethod);
            Console.WriteLine(Request.UserHostName);
            Console.WriteLine(Request.UserAgent);
            Console.WriteLine();

            List<uint> Areas;
            telegraph.GetAllAreas(out Areas);

            string AreaList = "";

            foreach (uint Area in Areas)
            {
                string name;
                telegraph.DebugGetNameFromGuid(Area, out name);

                AreaList += name + "\n";
            }

            byte[] data = Encoding.UTF8.GetBytes(AreaList);
            Response.ContentType = "text/html";
            Response.ContentEncoding = Encoding.UTF8;
            Response.ContentLength64 = data.LongLength;

            await Response.OutputStream.WriteAsync(data, 0, data.Length);
            Response.Close();
        }
    }
}

