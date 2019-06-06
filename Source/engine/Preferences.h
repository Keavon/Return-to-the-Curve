#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

class Preferences
{
public:
    struct {
        int resolutionX;
        int resolutionY;
        bool maximized;
        bool fullscreen;
    } window;

    struct {
        int resolution;
        int samples;
    } shadows;

    struct {
        bool music;
    } sound;

    Preferences(std::string prefsFile);
};