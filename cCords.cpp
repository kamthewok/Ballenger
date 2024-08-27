#include "cCords.h"

cCords::cCords(void)
{
	SetVel(0.0, 0.0, 0.0);
	yaw = 0.0f; pitch = 0.0f;
}
cCords::~cCords(void){}

void cCords::SetPos(float posx, float posy, float posz)
{
	x = posx;
	y = posy;
	z = posz;
}
void cCords::SetX(float posx)
{
	x = posx;
}
void cCords::SetY(float posy)
{
	y = posy;
}
void cCords::SetZ(float posz)
{
	z = posz;
}
float cCords::GetX()
{
    return x;
}
float cCords::GetY()
{
    return y;
}
float cCords::GetZ()
{
    return z;
}

void cCords::SetVel(float velx, float vely, float velz)
{
	vx = velx;
	vy = vely;
	vz = velz;
}
void cCords::SetVX(float velx)
{
	vx = velx;
}
void cCords::SetVY(float vely)
{
	vy = vely;
}
void cCords::SetVZ(float velz)
{
	vz = velz;
}
float cCords::GetVX()
{
    return vx;
}
float cCords::GetVY()
{
    return vy;
}
float cCords::GetVZ()
{
    return vz;
}

void cCords::SetYaw(float ang)
{
	yaw = ang;
}
void cCords::SetPitch(float ang)
{
	pitch = ang;
}
float cCords::GetYaw()
{
	return yaw;
}
float cCords::GetPitch()
{
	return pitch;
}

void cCords::SetState(int s)
{
	state = s;
}
int cCords::GetState()
{
	return state;
}
