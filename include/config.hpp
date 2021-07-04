#pragma once

struct config_t {
    bool enabled = true;
    bool alwayson = false;
};

extern config_t config;

bool LoadConfig();
void SaveConfig();
