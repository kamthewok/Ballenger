#pragma once

#include "cScene.h"

class cCords
{
public:
	cCords(void);
	~cCords(void);

	void  SetPos(float posx, float posy, float posz);
	void  SetX(float posx);
	void  SetY(float posy);
	void  SetZ(float posz);
    float GetX();
    float GetY();
    float GetZ();

	void  SetVel(float velx, float vely, float velz);
	void  SetVX(float velx);
	void  SetVY(float vely);
	void  SetVZ(float velz);
    float GetVX();
    float GetVY();
    float GetVZ();

	void  SetYaw(float ang);
	void  SetPitch(float ang);
    float GetYaw();
    float GetPitch();

	void SetState(int s);
	int  GetState();

private:
	float x, y, z;   // Position
	float vx, vy, vz;   // Velocity
	float yaw, pitch;   // rotation angles (by movement) in degrees
	int state;
};
