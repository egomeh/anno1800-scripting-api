using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace ProductionViewer
{
    public struct ResourceInfo
    {
        public int count;
        public int capacity;
        public float residential;
        public float industry;
    }


    public struct GameState
    {
        public List<string> islandNames;
        public List<string> resourceNames;

        public Dictionary<string, ResourceInfo> resources;

        public GameState()
        {
            islandNames = new List<string>();
            resourceNames = new List<string>();
            resources = new Dictionary<string, ResourceInfo>();
        }
    }

    public partial class App : Application
    {
        public Telegraph telegraph = new Telegraph();

        public delegate void GameStateUpdateHandler(object sender, GameState gameState);
        public event GameStateUpdateHandler? GameStateUpdate;

        public App()
        {
            InitializeComponent();

            UpdateTick();

            //System.Windows.Threading.DispatcherTimer dispatcherTimer = new System.Windows.Threading.DispatcherTimer();
            //dispatcherTimer.Tick += new EventHandler(UpdateTick);
            //dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            //dispatcherTimer.Start();
        }
        private async void UpdateTick()
        {
            while (true)
            {
                UpdateGameInfo();
                await Task.Delay(500);
            }
        }

        private void UpdateGameInfo()
        {
            GameState gameState = new GameState();

            gameState.islandNames.Clear();
            gameState.resourceNames.Clear();

            List<uint> areas;
            telegraph.GetAllAreas(out areas);

            foreach (uint area in areas)
            {
                List<IslandInfo> islands;
                telegraph.GetWorldIslands(area, true, out islands);

                foreach (IslandInfo island in islands)
                {
                    gameState.islandNames.Add(island.name);

                    List<IslandResource> resources;
                    telegraph.GetIslandResources(area, island.island_id, out resources);

                    List<ResourceConsumption> residentialConsumption;
                    telegraph.GetIslandResidentialConsumption(area, island.island_id, out residentialConsumption);

                    List<ResourceConsumption> industryConsumption;
                    telegraph.GetIslandIndustrialConversion(area, island.island_id, out industryConsumption);

                    foreach (var resource in resources)
                    {
                        ResourceInfo info;
                        info.count = resource.amount;
                        info.capacity = resource.capacity;
                        info.residential = 0.0f;
                        info.industry = 0.0f;

                        foreach (ResourceConsumption rc in residentialConsumption)
                        {
                            if (rc.type_id == resource.type_id)
                            {
                                info.residential = rc.rate;
                            }
                        }

                        foreach (ResourceConsumption rc in industryConsumption)
                        {
                            if (rc.type_id == resource.type_id)
                            {
                                info.industry = -rc.rate;
                            }
                        }

                        string key = String.Format("{0}.{1}", island.name, resource.name);

                        if (!gameState.resources.ContainsKey(key))
                            gameState.resources.Add(key, info);

                        if (gameState.resourceNames.Contains(resource.name))
                            continue;

                        gameState.resourceNames.Add(resource.name);
                    }
                }
            }

            GameStateUpdate?.Invoke(this, gameState);
        }
    }
}
