#pragma once

#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <irrKlang.h>
#include <algorithm>

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
        irrklang::ISoundSource *superBounceSoundSource;
    } sfxSources;

    struct
    {
        irrklang::ISound *resetSound;
        irrklang::ISound *impactSound;
        irrklang::ISound *winSound;
        irrklang::ISound *superBounceSound;
    } sfxSounds;

    vector<irrklang::ISoundSource *> musicSources;
    vector<irrklang::ISound *> musicSounds;

    Sound();
    ~Sound();

    void impact(float impactVal);
    void reset();
    void win();
    void superBounce();
    void playPauseMusic();
    void nextTrackMusic();
    void updateMusic();
};

extern std::shared_ptr<Sound> soundEngine;
