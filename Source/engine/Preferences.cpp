#include "Preferences.h"

#include <math.h>

using namespace std;

Preferences::Preferences(string prefsFile)
{
    YAML::Node file = YAML::LoadFile(prefsFile);

    YAML::Node windowNode = file["window"];
    window.resolutionX = windowNode["resolution"][0].as<int>();
    window.resolutionY = windowNode["resolution"][1].as<int>();
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
