#pragma once

#include "cCords.h"
#include "cCamera.h"

#define PLAYER_SPEED  0.008f
#define PLAYER_JUMP_SPEED  0.4f
#define FRICTION   0.05f
#define ELASTICITY   0.5f 
#define MAX_MOVEMENT 0.4f
#define RADIUS   0.5f

class cPlayer: public cCords
{
public:
	cPlayer();
	~cPlayer();

	void Draw(cData *Data,cCamera *Camera,cShader *Shader);
	void SetFade(bool b);

private:
	bool fade;
};