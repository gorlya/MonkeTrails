#include "config.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

static ModInfo modInfo = {ID, VERSION};
extern Logger& getLogger();

config_t config;
Configuration& getConfig()
{
    static Configuration config(modInfo);
    config.Load();
    return config;
} 

void SaveConfig()
{
    getLogger().info("Saving Configuration...");
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    rapidjson::Document::AllocatorType& allocator = getConfig().config.GetAllocator();
    
    getConfig().config.AddMember("enabled", config.enabled, allocator);
    getConfig().config.AddMember("trailmode", config.trailmode, allocator);
    getConfig().config.AddMember("trailsize", config.trailsize, allocator);
    getConfig().config.AddMember("trailwidth", config.trailwidth, allocator);

    getConfig().Write();
    getLogger().info("Saved Configuration!");
}

bool LoadConfig()
{
    getLogger().info("Loading Config...");
    bool foundEverything = true;
    rapidjson::Document& doc = getConfig().config;
    
    if (doc.HasMember("enabled")) { config.enabled = doc["enabled"].GetBool(); }
    if (doc.HasMember("trailmode")) { config.trailmode = doc["trailmode"].GetInt(); } 
    if (doc.HasMember("trailsize")) { config.trailsize = doc["trailsize"].GetInt(); } 
    if (doc.HasMember("trailwidth")) { config.trailwidth = doc["trailwidth"].GetInt(); } 

    if (foundEverything) getLogger().info("Config Loaded Successfully!");
    return foundEverything;
}
