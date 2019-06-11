#pragma once

#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <random>
#include <time.h> 
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
        irrklang::ISoundSource *jumpSoundSource;
        irrklang::ISoundSource *winSoundSource;
        irrklang::ISoundSource *superBounceSoundSource;
        irrklang::ISoundSource *fanSoundSource;
    } sfxSources;

    struct
    {
        irrklang::ISound *resetSound;
        irrklang::ISound *impactSound;
        irrklang::ISound *jumpSound;
        irrklang::ISound *winSound;
        irrklang::ISound *superBounceSound;
        irrklang::ISound *fanSound;
    } sfxSounds;

    vector<irrklang::ISoundSource *> musicSources;
    vector<irrklang::ISound *> musicSounds;

    Sound();
    ~Sound();

    void impact(float impactVal);
    void jump();
    void reset();
    void win();
    void fan(float x, float y, float z);
    void superBounce();
    void playPauseMusic();
    void nextTrackMusic();
    void updateMusic();
    void update3dAudio(irrklang::vec3df pos, irrklang::vec3df look, irrklang::vec3df up);
};

extern shared_ptr<Sound> soundEngine;