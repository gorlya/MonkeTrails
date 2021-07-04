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
    
    getConfig().config.AddMember("alwayson", config.alwayson, allocator);

    getConfig().Write();
    getLogger().info("Saved Configuration!");
}

bool LoadConfig()
{
    getLogger().info("Loading Config...");
    bool foundEverything = true;
    rapidjson::Document& doc = getConfig().config;
    
    if (doc.HasMember("enabled")) {
        config.enabled = doc["enabled"].GetBool();
    } else {
        foundEverything = false;
    }

    if (foundEverything) getLogger().info("Config Loaded Successfully!");
    return foundEverything;
}
