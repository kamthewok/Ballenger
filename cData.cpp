#include "cData.h"

cData::cData(void) {}
cData::~cData(void){}

int cData::GetID(int img)
{
	return textures[img].GetID();
}

void cData::GetSize(int img, int *w, int *h)
{
	textures[img].GetSize(w,h);
}

bool cData::LoadImage(int img, char *filename, int type)
{
	int res;

	res = textures[img].Load(filename,type);
	if(!res) return false;

	return true;
}
bool cData::Load()
{
	int res;

	res = LoadImage(IMG_SPACE,"Textures/space.png",GL_RGBA);
	if(!res) return false;
	res = LoadImage(IMG_ROCK,"Textures/rock.png",GL_RGBA);
	if(!res) return false;
	res = LoadImage(IMG_PLAYER,"Textures/player.png",GL_RGBA);
	if(!res) return false;
	res = LoadImage(IMG_PLAYER_NMAP,"Textures/playerNmap.png",GL_RGBA);
	if(!res) return false;

	return true;
}