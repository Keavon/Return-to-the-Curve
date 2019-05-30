#pragma once

#include <cstdlib>
#include <string>
#include <irrKlang.h>

#define NUMBER_OF_MUSIC_TRACKS 5

class Sound
{
public:
    // Sound
    irrklang::ISoundEngine *sfxEngine;
    
    struct
    {
        irrklang::ISoundSource *resetSoundSource;
        irrklang::ISoundSource *impactSoundSource;
    } sfxSources;
    
    struct
    {
        irrklang::ISound *resetSound;
        irrklang::ISound *impactSound;
    } sfxSounds;

    // vector<irrklang::ISoundSource *> musicSources;
    // vector<irrklang::ISound *> musicSounds;


    Sound();
    ~Sound();

    void impact();
    void reset();
};