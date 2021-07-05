#pragma once

struct config_t {
    int monkemode = 1;
    bool enabled = true;
    bool alwayson = false;
};

extern config_t config;

bool LoadConfig();
void SaveConfig();
