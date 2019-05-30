#include "Sound.h"

using namespace std;

Sound::Sound()
{
    sfxEngine = irrklang::createIrrKlangDevice();

    sfxSources.impactSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_impact.wav");
    sfxSources.resetSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_reset.wav");
    sfxSources.winSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_win.wav");

    sfxSounds.impactSound = 0;
    sfxSounds.resetSound = 0;
    sfxSounds.winSound = 0;

    // loops over the number of tracks, initializing them and adding them to a vector
    // double completion = 0.0;
    // for (int i = 1; i < NUMBER_OF_MUSIC_TRACKS + 1; i++)
    // {
    //     string file = "../Resources/sounds/music/" + to_string(i) + ".wav";

    //     irrklang::ISoundSource *musicSource = sfxEngine->addSoundSourceFromFile(file.c_str());
    //     musicSources.push_back(musicSource);

    //     irrklang::ISound *musicSound = sfxEngine->play2D(musicSources[i - 1], GL_FALSE, true);
    //     musicSounds.push_back(musicSound);

    //     completion = ((float)i * 100 / (float)NUMBER_OF_MUSIC_TRACKS);
    //     cout << setprecision(3) << "Loading Music: " << completion << "% complete." << endl;
    // }
}

Sound::~Sound()
{
    sfxEngine->drop();
}

void Sound::impact()
{
    if (!sfxSounds.impactSound)
    {
        sfxSounds.impactSound = sfxEngine->play2D(sfxSources.impactSoundSource, false);
    }
    if (sfxSounds.impactSound)
    {
        sfxSounds.impactSound->drop(); // don't forget to release the pointer once it is no longer needed by you
        sfxSounds.impactSound = 0;
    }
}

void Sound::reset()
{
    if (!sfxSounds.resetSound)
    {
        sfxSounds.resetSound = sfxEngine->play2D(sfxSources.resetSoundSource, false);
    }
    if (sfxSounds.resetSound)
    {
        sfxSounds.resetSound->drop(); // don't forget to release the pointer once it is no longer needed by you
        sfxSounds.resetSound = 0;
    }
}

void Sound::win()
{
    if (!sfxSounds.winSound)
    {
        sfxSounds.winSound = sfxEngine->play2D(sfxSources.winSoundSource, false);
    }
    if (sfxSounds.winSound)
    {
        sfxSounds.winSound->drop(); // don't forget to release the pointer once it is no longer needed by you
        sfxSounds.winSound = 0;
    }
}