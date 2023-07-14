

Telegraph telegraph = new Telegraph();

var app = WebApplication.Create();

// app.UseHttpsRedirection();

app.MapGet("/anno-stats", () =>
{
    return CreateAnnoStats();
});

app.Run();

AnnoStats CreateAnnoStats()
{
    List<uint> Areas;
    telegraph.GetAllAreas(out Areas);

    return new AnnoStats
    (
        "what",
        "what",
        new SessionData
        (
            new PlayerData[]{
            new PlayerData(
                "whut",
                "what??",
                Areas.ConvertAll<WorldData>((uint Area) =>
                {
                    string AreaName;
                    telegraph.DebugGetNameFromGuid(Area, out AreaName);

                    List<IslandInfo> AnnoIslands;
                    telegraph.GetWorldIslands(Area, true, out AnnoIslands);

                    return new WorldData
                    (
                        string.Format("{0}", Area),
                        AreaName,
                        AnnoIslands.ConvertAll<IslandData>((IslandInfo island_info) =>
                        {
                            List<IslandResource> resources;
                            telegraph.GetIslandResources(Area, island_info.island_id, out resources);

                            return new IslandData
                            (
                                String.Format("{0}", island_info.island_id),
                                island_info.name,
                                resources.ConvertAll<InventoryItem>((IslandResource resource) =>
                                {
                                    return new InventoryItem
                                    (
                                        resource.name,
                                        resource.amount
                                    );
                                }).ToArray(),
                                new string[] { },
                                new string[] { },
                                new string[] { },
                                new string[] { }
                            );
                        }).ToArray()
                    );
                }).ToArray()
            )}
        )
    );
}

record InventoryItem
(
    string name,
    int value
);

record IslandData
(
    string island_id,
    string island_name,
    InventoryItem[] island_inventory,
    string[] island_production,
    string[] island_consumption,
    string[] island_buildings_summary,
    string[] island_production_buildings
);

record WorldData
(
    string world_id,
    string world_name,
    IslandData[] islands
);

record PlayerData
(
    string player_id,
    string player_name,
    WorldData[] worlds
);

record SessionData
(
    PlayerData[] players
);

record AnnoStats
(
    string session_id,
    string session_name,
    SessionData session_data
);


