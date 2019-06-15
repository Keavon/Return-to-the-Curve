#include "Preferences.h"

Preferences::Preferences(string prefsFile)
{
    YAML::Node file = YAML::LoadFile(prefsFile);
    YAML::Node windowNode = file["window"];

    // Resolution
    if (windowNode["resolution"].IsSequence()) {
        window.resolutionX = windowNode["resolution"][0].as<int>();
        window.resolutionY = windowNode["resolution"][1].as<int>();
    }
    else if (windowNode["resolution"].as<string>() == "native") {
        window.resolutionX = -1;
        window.resolutionY = -1;
    }

    // Maximized and fullscreen
    window.maximized = windowNode["maximized"].as<bool>();
    window.fullscreen = windowNode["fullscreen"].as<bool>();

    YAML::Node shadowsNode = file["shadows"];
    shadows.resolution = 256 * (int)pow(2, shadowsNode["resolution"].as<int>()) - 256;
    shadows.samples = shadowsNode["samples"].as<int>();

    YAML::Node soundNode = file["sound"];
    sound.music = soundNode["music"].as<bool>();

    YAML::Node scenesNode = file["scenes"];
    scenes.startup = scenesNode["startup"].as<int>();
    scenes.list = scenesNode["list"].as<vector<string>>();
}
