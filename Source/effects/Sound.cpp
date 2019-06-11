#include "Sound.h"

Sound::Sound()
{
    sfxEngine = irrklang::createIrrKlangDevice();

    sfxSources.impactSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_impact.wav", irrklang::ESM_NO_STREAMING, true);
    sfxSources.resetSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_reset.wav", irrklang::ESM_NO_STREAMING, true);
    sfxSources.jumpSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_jump.wav", irrklang::ESM_NO_STREAMING, true);
    sfxSources.winSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_win.wav", irrklang::ESM_NO_STREAMING, true);
    sfxSources.superBounceSoundSource = sfxEngine->addSoundSourceFromFile("../Resources/sounds/marble_super_bounce.wav", irrklang::ESM_NO_STREAMING, true);

    sfxSounds.impactSound = 0;
    sfxSounds.resetSound = 0;
    sfxSounds.jumpSound = 0;
    sfxSounds.winSound = 0;
    sfxSounds.superBounceSound = 0;

    // loops over the number of tracks, initializing them and adding them to a vector
    // double completion = 0.0;
    for (int i = 1; i < NUMBER_OF_MUSIC_TRACKS + 1; i++)
    {
        string file = "../Resources/sounds/music/" + to_string(i) + ".wav";

        irrklang::ISoundSource *musicSource = sfxEngine->addSoundSourceFromFile(file.c_str(), irrklang::ESM_AUTO_DETECT);
        musicSource->setDefaultVolume(0.15f);
        musicSources.push_back(musicSource);

        irrklang::ISound *musicSound = sfxEngine->play2D(musicSources[i - 1], false, true);
        musicSounds.push_back(musicSound);

        // completion = ((float)i * 100 / (float)NUMBER_OF_MUSIC_TRACKS);
        // cout << setprecision(3) << "Loading Music: " << completion << "% complete." << endl;
    }

    srand((int)static_cast<int>(time(0)));
    int i = (rand() % NUMBER_OF_MUSIC_TRACKS);
    currentTrack = i;
}

Sound::~Sound()
{
    sfxEngine->drop();
}

void Sound::impact(float impactVal)
{
    if (!sfxSounds.impactSound)
    {
        float soundScale = min(1.0f, max(0.0f, ((impactVal * 2.0f) / 100.0f)));

        sfxSounds.impactSound = sfxEngine->play2D(sfxSources.impactSoundSource, false, true);
        sfxSounds.impactSound->setVolume(soundScale);
        sfxSounds.impactSound->setIsPaused(false);
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
        sfxSounds.resetSound = sfxEngine->play2D(sfxSources.resetSoundSource, false, true);
        sfxSounds.resetSound->setVolume(0.4f);
        sfxSounds.resetSound->setIsPaused(false);
    }
    if (sfxSounds.resetSound)
    {
        sfxSounds.resetSound->drop(); // don't forget to release the pointer once it is no longer needed by you
        sfxSounds.resetSound = 0;
    }
}

void Sound::jump()
{
    if (!sfxSounds.resetSound)
    {
        sfxSounds.jumpSound = sfxEngine->play2D(sfxSources.jumpSoundSource, false, true);
        sfxSounds.jumpSound->setVolume(0.4f);
        sfxSounds.jumpSound->setIsPaused(false);
    }
    if (sfxSounds.jumpSound)
    {
        sfxSounds.jumpSound->drop(); // don't forget to release the pointer once it is no longer needed by you
        sfxSounds.jumpSound = 0;
    }
}

void Sound::superBounce()
{
    if (!sfxSounds.superBounceSound)
    {
        sfxSounds.superBounceSound = sfxEngine->play2D(sfxSources.superBounceSoundSource, false, true);
        sfxSounds.superBounceSound->setVolume(0.4f);
        sfxSounds.superBounceSound->setIsPaused(false);
    }
    if (sfxSounds.superBounceSound)
    {
        sfxSounds.superBounceSound->drop(); // don't forget to release the pointer once it is no longer needed by you
        sfxSounds.superBounceSound = 0;
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

void Sound::playPauseMusic()
{
    musicSounds[currentTrack]->setIsPaused(!musicSounds[currentTrack]->getIsPaused());
}

void Sound::nextTrackMusic()
{
    musicSounds[currentTrack]->setIsPaused(true);
    currentTrack = (currentTrack + 1) % NUMBER_OF_MUSIC_TRACKS;
    musicSounds[currentTrack]->setIsPaused(false);
}

void Sound::updateMusic()
{
    if (musicSounds[currentTrack]->isFinished())
    {
        musicSounds[currentTrack] = sfxEngine->play2D(musicSources[currentTrack], false, true);
        nextTrackMusic();
    }
}