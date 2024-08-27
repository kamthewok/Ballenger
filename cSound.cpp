#include "cSound.h"

cSound::cSound(void)
{
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, 0);
}

cSound::~cSound(void)
{
	for(int i=0; i<NUM_SOUNDS; i++) sounds[i]->release();
    system->release();
}

bool cSound::Load()
{
	system->createStream("Sounds/ambient.mp3",       FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sounds[SOUND_AMBIENT]);
	system-> createSound("Sounds/bounce.wav",        FMOD_HARDWARE, 0, &sounds[SOUND_BOUNCE]);
	return true;
}

void cSound::Play(int sound_id)
{
	if (sound_id == SOUND_AMBIENT) {
		system->playSound(FMOD_CHANNEL_FREE, sounds[SOUND_AMBIENT], false, &ambient1Channel);
		ambient1Channel->setVolume( 0.125f ); 
	}
	else system->playSound(FMOD_CHANNEL_FREE,sounds[sound_id], false, 0);
}

void cSound::PlayBounce(float vol)
{
	system->playSound(FMOD_CHANNEL_FREE,sounds[SOUND_BOUNCE], true, &bounceChannel);
	bounceChannel->setVolume(vol);
	bounceChannel->setPaused(false);
}

void cSound::StopAll()
{
	ambient1Channel->stop();
}

void cSound::Update()
{
	system->update();
}