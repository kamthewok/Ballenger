#pragma once

#include "cTexture.h"


#define NUM_TEX	4


enum {
	IMG_SPACE,
	IMG_ROCK,
	IMG_PLAYER,
	IMG_PLAYER_NMAP,
};

class cData
{
public:
	cData(void);
	~cData(void);

	int  GetID(int img);
	void GetSize(int img, int *w, int *h);
	bool Load();

private:
	cTexture textures[NUM_TEX];
	bool LoadImage(int img,char *filename,int type = GL_RGBA);
};
