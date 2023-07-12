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

            byte[] data = Encoding.UTF8.GetBytes("I'm a http server with access to Anno >:)");
            Response.ContentType = "text/html";
            Response.ContentEncoding = Encoding.UTF8;
            Response.ContentLength64 = data.LongLength;

            await Response.OutputStream.WriteAsync(data, 0, data.Length);
            Response.Close();
        }
    }
}

