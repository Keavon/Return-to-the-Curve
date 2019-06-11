#pragma once

#include <string>
#include <cmath>
#include <yaml-cpp/yaml.h>
#include <math.h>

using namespace std;

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

    struct {
        int startup;
        vector<string> list;
    } scenes;

    Preferences(string prefsFile);
};