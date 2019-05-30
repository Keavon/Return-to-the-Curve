#pragma once

#include <cstdlib>
#include <string>
#include <memory>
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
        irrklang::ISoundSource *winSoundSource;
    } sfxSources;

    struct
    {
        irrklang::ISound *resetSound;
        irrklang::ISound *impactSound;
        irrklang::ISound *winSound;
    } sfxSounds;

    // vector<irrklang::ISoundSource *> musicSources;
    // vector<irrklang::ISound *> musicSounds;

    irrklang::ISoundSource *musicSource;
    irrklang::ISound *musicSound;

    Sound();
    ~Sound();

    void impact();
    void reset();
    void win();
    void music();
};

extern std::shared_ptr<Sound> soundEngine;
