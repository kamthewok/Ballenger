#pragma once

#include "cPlayer.h"
#include "cSound.h"


#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

#define FRAMERATE 60
#define GRAVITY 0.015f

enum{
	STATE_LIVELOSS,
	STATE_RUN,
};

#define TOTAL_LEVELS	  1

//klawisze
#define P_UP		'w'
#define P_DOWN		's'
#define P_LEFT		'a'
#define P_RIGHT		'd'
#define P_JUMP		' '
#define P_PLUS		'+'
#define P_MINUS		'-'

class cGame
{
public:
	cGame(void);
	virtual ~cGame(void);

	bool Init(int lvl);
	bool Loop();
	

	//Input
	void ReadKeyboard(unsigned char key, int x, int y, bool press);
	void ReadMouse(int button, int state, int x, int y);
	void ReadMouseMotion(int x, int y);
	//Process
	bool Process();
	//Output
	void Reshape(int w, int h);
	void Render();

private:
	unsigned char keys[256];
	bool mouse_left_down,mouse_right_down;
	int level,state,respawn_id,pickedkey_id;
	bool noclip,portal_activated;
	float time,ang, noclipSpeedF;

	cScene Scene;
	cData Data;
	cPlayer Player;
	cCamera Camera;
	cSound Sound;
	cShader Shader;

	cTerrain Terrain;

	void Physics(cCords &object);
};