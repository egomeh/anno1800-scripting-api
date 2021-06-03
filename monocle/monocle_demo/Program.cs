using System;
using System.Collections.Generic;
using System.Linq;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices.WindowsRuntime;
using monocle;
using System.Collections;
using System.Net;
using System.Security.Principal;
using System.Xml.Schema;
using SharpDX.DirectInput;

namespace monocle_demo
{
    class Program
    {
        static void Main(string[] args)
        {
            Program program = new Program();
            // program.Run(args);

            
            // program.ShipComponentExperiment();

            program.Demo();
            // program.DemoAllIslands();
        }

        void DemoAllIslands()
        {
            Telegraph telegraph = new Telegraph();

            if (!telegraph.GetAllIslands(out var allIslands))
            {
                Console.WriteLine("Couldn't find any islands");
                return;
            }


            Console.WriteLine(allIslands.Count);
            allIslands = allIslands.Where(island => island.name.Length > 0 && island.name.Length < 20).ToList();

            foreach (var island in allIslands)
            {
                Console.WriteLine($"\n\nFound island {island.name} - {island.id}");

                if (!telegraph.GetIslandConsumption(island.id, out var consumption))
                    continue;

                foreach(var con in consumption)
                {
                    Console.WriteLine($"{con.resourceType.ToString()} - {con.rate}");
                }

                if (!telegraph.GetIslandBuildings(island.id, out var buildings))
                    continue;

                // foreach (var building in buildings)
                // {
                //     Console.WriteLine($"{building.id} - {building.buidlingType} - {building.rawBuildingTypeID}");
                // }

                var validBuildings = buildings.Where(b => b.buidlingType != Building.Invalid).ToList();
                var productionBuildings = validBuildings.Where(b => RDAHelper.GetProductionBuildings().Contains(b.buidlingType)).ToList();
                Console.WriteLine($"{buildings.Count} buildings ({validBuildings.Count} valid, {productionBuildings.Count} production)");

                foreach (var building in productionBuildings)
                {
                    if (!telegraph.GetBuildingProduction(island.id, building.id, out var productionNode))
                         continue;
                    Console.WriteLine($"Building {building.id} - {building.buidlingType.ToString()} - {productionNode.rate} {productionNode.output}");
                }
            }
        }

        void Demo()
        {

            {
                Telegraph telegraph = new Telegraph();

                monocle.GameTime gameTime;
                telegraph.GetGameTime(out gameTime);

                ShipMoveData shipMoveData;
                telegraph.GetShipMoveData(0x0000000200000FF8, out shipMoveData);

                //List<monocle.ShipData> ships;
                //List<monocle.IslandData> islands;

                //telegraph.GetAllShips(Area.OldWorld, out ships);
                //telegraph.GetIslandsByName("Shortisland", out islands);

                //List<monocle.ConsumptionNode> musliConsumption = null;
                //telegraph.GetIslandConsumption(islands.First().id, out musliConsumption);

                //// List with Orpheus' ID
                //List<ulong> Orpheus = new List<ulong>() { 0x0000000200000F88 };

                //// The target destination
                //Coordinate destination = new Coordinate() { x = 1111f, y = 1616f };

                //// Call user-facing part of function
                //telegraph.AddWaypoint(Orpheus, destination);

                return;

                //List<monocle.ConsumptionNode> musliConsumption;
                //telegraph.GetIslandConsumption(islands.FirstOrDefault().id, out musliConsumption);

                //List<monocle.BuildingData> buildings = new List<monocle.BuildingData>();
                //telegraph.GetIslandBuildings(islands.FirstOrDefault().id, out buildings);

                //monocle.BuildingData bakery;
                //bakery.id = 0;

                //foreach (var building in buildings)
                //{
                //    if (building.buidlingType == Building.Bakery)
                //    {
                //        bakery = building;
                //        break;
                //    }
                //}

                //monocle.ProductionNode production;
                //telegraph.GetBuildingProduction(islands.FirstOrDefault().id, bakery.id, out production);

                //for (ulong i = 0; i < 2048; ++i)
                //{
                //    ulong compAddress = 0;

                //    // telegraph.GetShipComponentAddress(0x0000000200000FF8, i, out compAddress);

                //    telegraph.GetBuildingComponentAddress(islands.FirstOrDefault().id, bakery.id, i, out compAddress);

                //    if (compAddress != 0)
                //    {
                //        string sAddress = compAddress.ToString("X");
                //        string sI = i.ToString();

                //        Console.WriteLine(string.Format("{0} {1}", sI, sAddress));
                //    }
                //}

                //List<ulong> ids = new List<ulong>();
                //ids.Add(ships[0].shipId);

                //telegraph.AddWaypoint(ids, new monocle.Coordinate { x = 1111, y = 1616});

                //List<TradeRoute> tradeRoutes;

                //telegraph.GetAllTradeRoutes(out tradeRoutes);

                //List<ShipData> oldWorldShips;
                //List<ShipData> newWorldShips;

                //telegraph.GetAllShips(Area.OldWorld, out oldWorldShips);
                //telegraph.GetAllShips(Area.NewWorld, out newWorldShips);

                //List<IslandData> islands = null;
                //telegraph.GetIslandsByName("musli", out islands);

                //var musli = islands.First();

                //List<ShipData> ships = null;
                //telegraph.GetAllShips(Area.OldWorld, out ships);

                //var goldenBehind = from ship in ships where ship.name.Equals("Golden Behind") select ship;

                //List < ConsumptionNode > musliconsumption;
                //telegraph.GetIslandConsumption(musli.id, out musliconsumption);
            }

            return;

           //// Initialize DirectInput
           //// var directInput = new DirectInput();

           //// Find a Joystick Guid
           //// var joystickGuid = Guid.Empty;

           //// foreach (var deviceInstance in directInput.GetDevices(DeviceType.Gamepad,
           ////             DeviceEnumerationFlags.AllDevices))
           ////     joystickGuid = deviceInstance.InstanceGuid;

           //// If Gamepad not found, look for a Joystick
           //// if (joystickGuid == Guid.Empty)
           ////         foreach (var deviceInstance in directInput.GetDevices(DeviceType.Joystick,
           ////                 DeviceEnumerationFlags.AllDevices))
           ////             joystickGuid = deviceInstance.InstanceGuid;

           //// If Joystick not found, throws an error
           //// if (joystickGuid == Guid.Empty)
           //// {
           ////     Console.WriteLine("No joystick/Gamepad found.");
           ////     Console.ReadKey();
           ////     Environment.Exit(1);
           //// }

           //// Instantiate the joystick
           ////var joystick = new Joystick(directInput, joystickGuid);

           //// Console.WriteLine("Found Joystick/Gamepad with GUID: {0}", joystickGuid);

           //// Query all suported ForceFeedback effects
           ////var allEffects = joystick.GetEffects();
           //// foreach (var effectInfo in allEffects)
           ////     Console.WriteLine("Effect available {0}", effectInfo.Name);

           //// Set BufferSize in order to use buffered data.
           ////joystick.Properties.BufferSize = 128;

           //// Acquire the joystick
           //// joystick.Acquire();

           //// monocle.Telegraph telegraph = new monocle.Telegraph();

           //// List<ShipData> oldWorldShips = new List<ShipData>();

           //// telegraph.GetAllShips(Area.OldWorld, out oldWorldShips);

           //// List<ulong> ships = new List<ulong>();
           //// ships.Add(oldWorldShips[5].shipId);

           //// ShipMoveData moveData;

           //// double x = 0;
           //// double y = 0;

           //// float dx = 0;
           //// float dy = 0;

           //// Poll events from joystick
           //// while (true)
           //// {
           ////     joystick.Poll();
           ////     var datas = joystick.GetBufferedData();
           ////     foreach (var state in datas)
           ////     {
           ////         if (state.Offset == JoystickOffset.Y)
           ////         {
           ////             y = ((double)state.Value / 65535.0 - .5) * 2.0;
           ////         }
           ////         else if (state.Offset == JoystickOffset.X)
           ////         {
           ////             x = ((double)state.Value / 65535.0 - .5) * 2.0;
           ////         }

           ////         Console.WriteLine(String.Format("{0} - {1}", x, y));
           ////     }

           ////     dx = (float)x * 20.0f;
           ////     dy = (float)y * 20.0f;

           ////     if (-.1 < x && x < .1)
           ////         dx = 0;

           ////     if (-.1 < y && y < .1)
           ////         dy = 0;

           ////     if (dx == 0.0f && dy == 0.0f)
           ////         continue;

           ////     telegraph.GetShipMoveData(ships.First(), out moveData);
           ////     telegraph.AddWaypoint(ships, new Coordinate() { x = moveData.position.x - dx, y = moveData.position.y + dy });
           //// }

           //// while (true)
           //// {
           ////     telegraph.GetShipMoveData(ships.First(), out moveData);
           ////     telegraph.AddWaypoint(ships, new Coordinate() { x = moveData.position.x + 10, y = moveData.position.y });
           //// }


           //// return;

           //// List<monocle.IslandData> islandData = new List<monocle.IslandData>();

           //// if (!telegraph.GetIslandsByName("Shortisland", out islandData))
           ////     return;

           //// monocle.IslandData island = islandData.First();

           //// List<monocle.BuildingData> buildings = new List<monocle.BuildingData>();

           //// if (!telegraph.GetIslandBuildings(islandData.First().id, out buildings))
           ////     return;

           //// foreach (var building in buildings)
           //// {
           ////     if (building.buidlingType == monocle.Building.SmallTradingPost)
           ////     {
           ////         for (ulong i = 0; i < 2048; ++i)
           ////         {
           ////             ulong compAddress = 0;

           ////             telegraph.GetBuildingComponentAddress(islandData.First().id, building.id, i, out compAddress);

           ////             if (compAddress != 0)
           ////             {
           ////                 string sAddress = compAddress.ToString("X");
           ////                 string sI = i.ToString();

           ////                 Console.WriteLine(string.Format("{0} {1}", sI, sAddress));
           ////             }
           ////         }
           ////     }


           ////     monocle.ProductionNode productionNode;

           ////     if (!telegraph.GetBuildingProduction(island.id, building.id, out productionNode))
           ////         continue;
           //// }

           //// List<monocle.ConsumptionNode> consumption = new List<monocle.ConsumptionNode>();

           //// if (!telegraph.GetIslandConsumption(island.id, out consumption))
           ////     return;

           //// List<monocle.ShipData> ships = new List<monocle.ShipData>();

           //// if (!telegraph.GetAllShips(monocle.Area.OldWorld, out ships))
           ////     return;

           //// ships = (from s in ships where s.shipType == monocle.ShipType.Clipper select s).ToList();

           //// monocle.ShipMoveData moveData;

           //// if (!telegraph.GetShipMoveData(ships.First().shipId, out moveData))
           ////     return;

           //// monocle.Coordinate target = moveData.position;
           //// target.x = target.x + 100;

           //// List<ulong> ids = (from s in ships select s.shipId).ToList();

           //// if (!telegraph.AddWaypoint(ids, target))
           ////     return;

           //// List<monocle.ShipCargoSlot> cargo = new List<monocle.ShipCargoSlot>();

           //// if (!telegraph.GetShipCargo(ids.First(), out cargo))
           ////     return;

           //// if (!telegraph.ShipDumpCargo(ids.First(), 1))
           ////     return;

           //// ulong address = 0;

           //// telegraph.GetShipComponentAddress(ids.First(), 253, out address);
        }

        bool SendShipToPosition(monocle.Telegraph telegraph)
        {
            List<monocle.ShipData> shipsData = null;

            if (!telegraph.GetAllShips(monocle.Area.OldWorld, out shipsData))
                return false;

            List<ulong> shipsID = (from d in shipsData select d.shipId).ToList();

            if (!telegraph.AddWaypoint(shipsID, new monocle.Coordinate() { x = 1111f, y = 1616 }))
                return false;

            return true;
        }

        bool ShipComponentExperiment()
        {
            Telegraph telegraph = new Telegraph();

            //List<monocle.ShipData> ships = null;

            //if (!telegraph.GetAllShips(monocle.Area.OldWorld, out ships))
            // return false;

            for (ulong i = 0; i < 2048; ++i)
            {
                ulong address = 0;

                if (!telegraph.GetShipComponentAddress(0x200000FF8, i, out address))
                    continue;

                if (address == 0)
                    continue;

                string idString = i.ToString();
                string addressString = address.ToString("X");

                Console.WriteLine(string.Format("{0} {1}", idString, addressString));
            }

            return true;
        }

        bool BuildingExperiment(monocle.Telegraph telegraph)
        {
            List<monocle.IslandData> islands = null;

            if (!telegraph.GetIslandsByName("musli", out islands))
                return false;

            if (islands == null || islands.Count != 1)
                return false;

            monocle.IslandData island = islands.First();

            List<monocle.BuildingData> buildings = null;

            Stopwatch sw = new Stopwatch();

            sw.Start();

            if (!telegraph.GetIslandBuildings(island.id, out buildings))
                return false;

            sw.Stop();

            Console.WriteLine("Elapsed={0}", sw.Elapsed.TotalMilliseconds);

            if (buildings == null)
                return false;

            var silo = (buildings.Where((x) => x.buidlingType == monocle.Building.FarmerResidence)).ToList().First();

            var farmersHouses  = (buildings.Where((x) => x.buidlingType == monocle.Building.FarmerResidence)).ToList();
            var workersHouses  = (buildings.Where((x) => x.buidlingType == monocle.Building.WorkerResidence)).ToList();
            var artisansHouses = (buildings.Where((x) => x.buidlingType == monocle.Building.ArtisanResidence)).ToList();

            for (ulong i = 0; i < 2048; ++i)
            {
                ulong address = 0;

                if (!telegraph.GetBuildingComponentAddress(island.id, silo.id, i, out address))
                    continue;

                if (address == 0)
                    continue;

                string idString = i.ToString();
                string addressString = address.ToString("X");

                Console.WriteLine(string.Format("{0} {1}", idString, addressString));
            }

            monocle.ProductionNode productionNode;

            telegraph.GetBuildingProduction(island.id, silo.id, out productionNode);

            return true;
        }

        bool ComputeIslandProductionBalance(monocle.Telegraph telegraph, string islandName)
        {
            List<monocle.IslandData> islands = null;

            if (!telegraph.GetIslandsByName("musli", out islands))
                return false;

            if (islands == null || islands.Count != 1)
                return false;

            monocle.IslandData island = islands.First();

            List<monocle.BuildingData> buildings = null;

            if (!telegraph.GetIslandBuildings(island.id, out buildings))
                return false;

            if (buildings == null)
                return false;

            List<monocle.ProductionNode> production = new List<monocle.ProductionNode>();

            foreach (var building in buildings)
            {
                monocle.ProductionNode node;

                if (!telegraph.GetBuildingProduction(island.id, building.id, out node))
                    continue;

                production.Add(node);
            }

            Dictionary<monocle.Resource, double> balance = new Dictionary<monocle.Resource, double>();

            foreach (monocle.ProductionNode node in production)
            {
                if (!balance.ContainsKey(node.output))
                    balance.Add(node.output, 0.0);

                balance[node.output] += node.rate;

                foreach (monocle.Resource input in node.input)
                {
                    if (!balance.ContainsKey(input))
                        balance.Add(input, 0.0);

                    balance[input] -= node.rate;
                }
            }

            foreach (var res in balance.Keys)
            {
                Console.WriteLine(string.Format("{0} - {1}", res.ToString(), balance[res]));
            }

            return true;
        }

        bool ConsumptionExperiment(monocle.Telegraph telegraph)
        {
            List<monocle.IslandData> islands = null;

            if (!telegraph.GetIslandsByName("lillypaddy", out islands))
                return false;

            if (islands.Count != 1)
                return false;

            monocle.IslandData island = islands.First();

            while (true)
            {
                List<monocle.ConsumptionNode> consumption = null;

                if (!telegraph.GetIslandConsumption(island.id, out consumption))
                    return false;

                double schnappsBalance = 0.0;

                foreach (ConsumptionNode node in consumption)
                {
                    if (node.resourceType == monocle.Resource.Schnapps)
                        schnappsBalance -= node.rate;
                }

                List<monocle.BuildingData> buildings = null;

                if (!telegraph.GetIslandBuildings(island.id, out buildings))
                    return false;

                if (buildings == null)
                    return false;

                foreach (var building in buildings)
                {
                    monocle.ProductionNode node;

                    if (!telegraph.GetBuildingProduction(island.id, building.id, out node))
                        continue;

                    if (node.output == monocle.Resource.Schnapps)
                        schnappsBalance += node.rate;
                }

                Console.WriteLine(schnappsBalance);

                Thread.Sleep(10 * 1000);
            }

            return true;
        }

        struct IslandResourceData
        {
            public int capacity;
            public int amount;
            public double rate;
        }

        struct ResourcePacket
        {
            public monocle.Resource resourceType;
            public int amount;
        }

        struct Delivery
        {
            public IslandData pickupIsland;
            public IslandData targetIsland;
            public bool finalRun;
            public bool done;
            public ulong shipID;
            public List<ResourcePacket> packets;
        }


        struct Demand
        {
            public IslandData island;
            public monocle.Resource resourceType;
            public double urgency;
            public int amountDemanded;
        }

        struct Supply
        {
            public IslandData island;
            public monocle.Resource resourceType;
            public int amountSupplied;
        }

        readonly Resource[] ConstructionMaterials =
        {
            Resource.Timber,
            Resource.Bricks,
            Resource.SteelBeams,
            Resource.Windows,
            Resource.Sails,
            Resource.Weapons,
            Resource.AdvancedWeapons,
            Resource.ReinforcedConcrete,
        };

        void Run(string[] args)
        {
            monocle.Telegraph telegraph = new monocle.Telegraph();

            List<monocle.IslandData> islands = new List<IslandData>();

            List<Delivery> deliveries = new List<Delivery>();

            Dictionary<ulong, monocle.Coordinate> tradingPosts = new Dictionary<ulong, monocle.Coordinate>();

            foreach (string name in args)
            {
                List<IslandData> tempData = null;
                if (!telegraph.GetIslandsByName(name, out tempData))
                {
                    Console.WriteLine("Could not get island");
                    return;
                }

                islands.AddRange(tempData);
            }

            while (true)
            {
                var islandResources = new Dictionary<monocle.IslandData, Dictionary<monocle.Resource, IslandResourceData>>();

                foreach (var island in islands)
                {
                    Dictionary<monocle.Resource, IslandResourceData> islandResource = new Dictionary<monocle.Resource, IslandResourceData>();

                    List<monocle.IslandResourceRecord> resources = new List<monocle.IslandResourceRecord>();

                    if (!telegraph.GetIslandResources(island.id, out resources))
                        return;

                    foreach (var record in resources)
                    {

                        islandResource.Add(record.resourceType, new IslandResourceData()
                        {
                            capacity = (int)record.capacity,
                            amount = (int)record.amount,
                            rate = 0.0,
                        });
                    }

                    List<monocle.ConsumptionNode> consumption = null;

                    if (!telegraph.GetIslandConsumption(island.id, out consumption))
                        return;

                    foreach (ConsumptionNode node in consumption)
                    {
                        IslandResourceData data = islandResource[node.resourceType];
                        data.rate -= node.rate;
                        islandResource[node.resourceType] = data;
                    }

                    List<monocle.BuildingData> buildings = null;

                    if (!telegraph.GetIslandBuildings(island.id, out buildings))
                        return;

                    foreach (var building in buildings)
                    {
                        if (building.buidlingType == monocle.Building.LargeTradingPost ||
                            building.buidlingType == monocle.Building.MediumTradingPost ||
                            building.buidlingType == monocle.Building.SmallTradingPost)
                            tradingPosts[island.id] = building.position;

                        monocle.ProductionNode productionNode;

                        if (!telegraph.GetBuildingProduction(island.id, building.id, out productionNode))
                            continue;

                        foreach (var input in productionNode.input)
                        {
                            IslandResourceData inputData = islandResource[input];
                            inputData.rate -= Math.Max(0.1, productionNode.rate);
                            islandResource[input] = inputData;
                        }

                        IslandResourceData outputData = islandResource[productionNode.output];
                        outputData.rate += productionNode.rate;
                        islandResource[productionNode.output] = outputData;
                    }

                    islandResources.Add(island, islandResource);
                }

                List<monocle.ShipData> ships = null;

                if (!telegraph.GetAllShips(monocle.Area.OldWorld, out ships))
                    return;

                var schooners = (from ship in ships where ship.shipType == monocle.ShipType.Schooner select ship).ToList();
                List<ShipData> idleSchooners = new List<ShipData>();

                List<int> deliveriesToRemove = new List<int>();
                for (int i = 0; i < deliveries.Count; ++i)
                {
                    var shipIDs = (from s in ships select s.shipId).ToList();

                    if (!shipIDs.Contains(deliveries[i].shipID) || deliveries[i].done)
                        deliveriesToRemove.Add(i);
                }

                foreach (var schooner in schooners)
                {
                    monocle.ShipMoveData moveData;

                    if (!telegraph.GetShipMoveData(schooner.shipId, out moveData))
                        return;

                    bool schoonerInDelivery = false;

                    foreach (var delivery in deliveries)
                    {
                        if (delivery.shipID == schooner.shipId)
                            schoonerInDelivery = true;
                    }

                    if (schoonerInDelivery)
                        continue;

                    if (!moveData.moving)
                        idleSchooners.Add(schooner);
                }

                deliveriesToRemove.Reverse();

                foreach (int idx in deliveriesToRemove)
                {
                    deliveries.RemoveAt(idx);
                }

                List<Supply> supplies = new List<Supply>();
                List<Demand> demands = new List<Demand>();

                foreach (monocle.IslandData island in islandResources.Keys)
                {
                    foreach (var r in islandResources[island].Keys)
                    {
                        int virtualAmount = islandResources[island][r].amount;
                        int capacity = islandResources[island][r].capacity;
                        double ownRate = islandResources[island][r].rate;

                        foreach (var delivery in deliveries)
                        {
                            if (delivery.targetIsland.id == island.id)
                            {
                                foreach (var package in delivery.packets)
                                {
                                    if (package.resourceType == r)
                                        virtualAmount += package.amount;
                                }
                            }

                            if (delivery.pickupIsland.id == island.id && !delivery.finalRun)
                            {
                                foreach (var package in delivery.packets)
                                {
                                    if (package.resourceType == r)
                                        virtualAmount -= package.amount;
                                }
                            }
                        }

                        if (r != Resource.Oil && r != Resource.Coal)
                        {
                            if (ownRate < 0.0 && (capacity - virtualAmount) > 50)
                            {
                                Demand demand = new Demand()
                                {
                                    island = island,
                                    resourceType = r,
                                    urgency = -ownRate * (1.0 - ((double)virtualAmount / capacity)),
                                    amountDemanded = capacity - virtualAmount,
                                };

                                demands.Add(demand);
                                continue;
                            }

                            if (ownRate > 0.0 && (virtualAmount > 70) || virtualAmount == capacity)
                            {
                                Supply supply = new Supply()
                                {
                                    island = island,
                                    resourceType = r,
                                    amountSupplied = islandResources[island][r].amount - 20,
                                };

                                supplies.Add(supply);
                                continue;
                            }

                            if (ConstructionMaterials.ToList().Contains(r) && ownRate == 0.0)
                            {
                                int toFull = capacity - virtualAmount;

                                if (toFull < 50)
                                    continue;

                                Demand demand = new Demand()
                                {
                                    island = island,
                                    resourceType = r,
                                    urgency = 1.0 / (capacity / (double)toFull),
                                    amountDemanded = toFull,
                                };

                                demands.Add(demand);
                                continue;
                            }
                        }
                    }
                }

                demands.Sort((s1, s2) => s2.urgency.CompareTo(s1.urgency));

                Console.Clear();

                foreach (Supply supply in supplies)
                {
                    Console.WriteLine(string.Format("{0} supplies {1} tonnes of {2}", supply.island.name, supply.amountSupplied.ToString(), supply.resourceType));
                }

                foreach (Demand demand in demands)
                {
                    Console.WriteLine(string.Format("{0} demands {1} tonnes of {2} at urgency {3}", demand.island.name, demand.amountDemanded.ToString(), demand.resourceType, demand.urgency));
                }

                if (idleSchooners.Count > 0 && demands.Count > 0)
                {
                    List<ResourcePacket> packets = new List<ResourcePacket>();

                    monocle.IslandData pickupIsland = new monocle.IslandData();
                    monocle.IslandData targetIsland = new monocle.IslandData();

                    bool found = false;

                    for (int i = 0; i < demands.Count; ++i)
                    {
                        var demand = demands[i];

                        if (found)
                            break;

                        for (int j = 0; j < supplies.Count; ++j)
                        {
                            var supply = supplies[j];

                            // Should not happen, but eh...
                            if (supply.island.id == demand.island.id)
                                continue;

                            if (demand.resourceType == supply.resourceType)
                            {
                                int packetSize = Math.Max(0, Math.Min(50, Math.Min(demand.amountDemanded, supply.amountSupplied)));
                                packets.Add(new ResourcePacket() { resourceType = supply.resourceType, amount = packetSize });
                                pickupIsland = supply.island;
                                targetIsland = demand.island;

                                demand.amountDemanded -= packetSize;
                                supply.amountSupplied -= packetSize;

                                demands[i] = demand;
                                supplies[j] = supply;

                                found = true;
                                break;
                            }
                        }
                    }

                    if (packets.Count > 0)
                    {
                        Delivery delivery = new Delivery()
                        {
                            packets = packets,
                            finalRun = false,
                            done = false,
                            pickupIsland = pickupIsland,
                            targetIsland = targetIsland,
                            shipID = idleSchooners.First().shipId
                        };

                        Coordinate pickupCoord = tradingPosts[pickupIsland.id];
                        List<ulong> shipID = new List<ulong>();
                        ulong schooner = idleSchooners.First().shipId;
                        shipID.Add(schooner);

                        if (!telegraph.ShipDumpCargo(schooner, 0))
                            return;

                        if (!telegraph.ShipDumpCargo(schooner, 1))
                            return;

                        if (!telegraph.AddWaypoint(shipID, pickupCoord))
                            return;

                        deliveries.Add(delivery);
                    }
                }

                for (int i = 0; i < deliveries.Count; ++i)
                {
                    var delivery = deliveries[i];

                    Console.WriteLine(string.Format("Delivering {0} tonnes of {1} from {2} to {3}", delivery.packets.First().amount, delivery.packets.First().resourceType, delivery.pickupIsland.name, delivery.targetIsland.name));

                    monocle.ShipMoveData moveData;
                    if (!telegraph.GetShipMoveData(delivery.shipID, out moveData))
                        return;

                    if (moveData.moving)
                        continue;

                    monocle.Coordinate targetLocation = delivery.finalRun ? tradingPosts[delivery.targetIsland.id] : tradingPosts[delivery.pickupIsland.id];

                    double x1 = moveData.position.x;
                    double x2 = targetLocation.x;
                    double y1 = moveData.position.y;
                    double y2 = targetLocation.y;
                    double dx = x2 - x1;
                    double dy = y2 - y1;

                    double distance = Math.Sqrt(dx * dx + dy * dy);

                    if (distance > 25.0)
                    {
                        List<ulong> shipIds = new List<ulong>();
                        shipIds.Add(delivery.shipID);

                        if (!telegraph.AddWaypoint(shipIds, targetLocation))
                            return;

                        continue;
                    }

                    if (delivery.finalRun)
                    {
                        if (!telegraph.OffloadCargoFromShip(delivery.targetIsland.id, delivery.shipID, 50, 0))
                            return;

                        delivery.done = true;
                        deliveries[i] = delivery;
                    }
                    else
                    {
                        if (!telegraph.LoadCargoToShip(delivery.pickupIsland.id, delivery.shipID, delivery.packets.First().resourceType, (ulong)delivery.packets.First().amount, 0))
                            return;

                        delivery.finalRun = true;
                        deliveries[i] = delivery;

                        List<ulong> shipIds = new List<ulong>();
                        shipIds.Add(delivery.shipID);

                        if (!telegraph.AddWaypoint(shipIds, tradingPosts[delivery.targetIsland.id]))
                            return;
                    }
                }

                Thread.Sleep(500);
            }
        }
    }
}
