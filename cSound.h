#pragma once

#pragma comment(lib, "fmodex_vc.lib" ) // fmod library

#include "fmod.hpp" //fmod c++ header

//Sound array size
#define NUM_SOUNDS 2

//Sound identifiers
enum {
	SOUND_AMBIENT,
	SOUND_BOUNCE,
};

class cSound
{
public:
	cSound(void);
	virtual ~cSound(void);

	bool Load();
	void Play(int sound_id);
	void PlayBounce(float vol);
	void StopAll();
	void Update();

	FMOD::System*     system; 
    FMOD::Sound*      sounds[NUM_SOUNDS]; 
	FMOD::Channel*    ambient1Channel;
	FMOD::Channel*    bounceChannel;
	FMOD::DSP*        dspSmoothStop;
};