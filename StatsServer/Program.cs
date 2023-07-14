using System.Net;
using System.Net.Http;
using System.Reflection.Metadata;
using System.Text;

using System.Text.Json;
using Microsoft.Graph;
using Microsoft.Graph.Models;

var builder = WebApplication.CreateBuilder(args);
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();
var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.UseDeveloperExceptionPage();
}

app.MapGet("/api/person", () => new { Name = "John Doe", Age = 30 });

app.Run();

//class StatsServer
//{
//    static void Main()
//    {
//        Console.WriteLine("Starting Stats Server");

//        StatsServer Server = new StatsServer();
//        Server.RunServer(); 
//    }

//    void RunServer()
//    {
//        Task HandleTask = HandleRequest();
//        HandleTask.GetAwaiter().GetResult();
//    }

//    async Task HandleRequest()
//    {
//        Telegraph telegraph = new Telegraph();

//        HttpListener Listener = new HttpListener();
//        Listener.Prefixes.Add("http://*:8888/");
//        Listener.Start();

//        while (true)
//        {
//            HttpListenerContext ListenerContext = await Listener.GetContextAsync();

//            HttpListenerRequest Request = ListenerContext.Request;
//            HttpListenerResponse Response = ListenerContext.Response;

//            Console.WriteLine(Request.Url.ToString());
//            Console.WriteLine(Request.HttpMethod);
//            Console.WriteLine(Request.UserHostName);
//            Console.WriteLine(Request.UserAgent);
//            Console.WriteLine();

//            string ResponseString = "{";

//            ResponseString += "\"session_id\": \"what is this?\",";
//            ResponseString += "\"session_name\": \"More what??\",";
//            ResponseString += "\"session_data\": {";

//            ResponseString += "\"players\": [";

//            ResponseString += "{";
//            ResponseString += "\"player_id\": \"aaa\",";
//            ResponseString += "\"player_name\": \"You\",";

//            ResponseString += "\"worlds\": [";

//            List<string> Worlds = new List<string>();

//            List<uint> Areas;
//            telegraph.GetAllAreas(out Areas);

//            foreach (uint Area in Areas)
//            {
//                string WorldEntry = "";

//                string AreaName;
//                telegraph.DebugGetNameFromGuid(Area, out AreaName);

//                WorldEntry += "{";
//                WorldEntry += String.Format("\"world_id\": {0},", Area);
//                WorldEntry += String.Format("\"world_name\": \"{0}\",", AreaName);
//                WorldEntry += "\"islands\": [";

//                List<string> IslandEntries = new List<string>();

//                List<IslandInfo> Islands;
//                telegraph.GetWorldIslands(Area, true, out Islands);

//                foreach (IslandInfo Island in Islands)
//                {
//                    string IslandEntry = "";

//                    IslandEntry += "{";
//                    IslandEntry += String.Format("\"island_id\": {0},", Island.island_id);
//                    IslandEntry += String.Format("\"island_name\": \"{0}\"", Island.name);

//                    IslandEntry += "\"island_inventory\": [],";
//                    IslandEntry += "\"island_production\": [],";
//                    IslandEntry += "\"island_consumption\": [],";
//                    IslandEntry += "\"island_buildings_summary\": [],";
//                    IslandEntry += "\"island_production_buildings\": []";

//                    IslandEntry += "}";

//                    IslandEntries.Add(IslandEntry);
//                }

//                WorldEntry += String.Join(",", IslandEntries);

//                WorldEntry += "]";
//                WorldEntry += "}";

//                Worlds.Add(WorldEntry);
//            }

//            ResponseString += String.Join(",", Worlds);

//            ResponseString += "]";

//            ResponseString += "}";

//            ResponseString += "]";

//            ResponseString += "}";

//            ResponseString += "}";

//            byte[] data = Encoding.UTF8.GetBytes(ResponseString);
//            Response.ContentType = "application/json";
//            Response.ContentEncoding = Encoding.UTF8;
//            Response.ContentLength64 = data.LongLength;

//            await Response.OutputStream.WriteAsync(data, 0, data.Length);
//            Response.Close();
//        }
//    }
//}

