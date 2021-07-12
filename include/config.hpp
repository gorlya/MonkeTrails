#pragma once

struct config_t {
    bool enabled = true;
    int trailmode = 1;
    int trailsize = 1;
    int trailwidth = 2;
    int trailpublic = 0;
};

extern config_t config;

bool LoadConfig();
void SaveConfig();
