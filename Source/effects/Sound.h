#pragma once

#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <irrKlang.h>

#define NUMBER_OF_MUSIC_TRACKS 5

using namespace std;

class Sound
{
public:
    // Sound
    int currentTrack;
    irrklang::ISoundEngine *sfxEngine;

    struct
    {
        irrklang::ISoundSource *resetSoundSource;
        irrklang::ISoundSource *impactSoundSource;
        irrklang::ISoundSource *winSoundSource;
    } sfxSources;

    struct
    {
        irrklang::ISound *resetSound;
        irrklang::ISound *impactSound;
        irrklang::ISound *winSound;
    } sfxSounds;

    vector<irrklang::ISoundSource *> musicSources;
    vector<irrklang::ISound *> musicSounds;

    irrklang::ISoundSource *musicSource;
    irrklang::ISound *musicSound;

    Sound();
    ~Sound();

    void impact(float impactVal);
    void reset();
    void win();
    void playPauseMusic();
    void nextTrackMusic();
    void updateMusic();
};

extern std::shared_ptr<Sound> soundEngine;
