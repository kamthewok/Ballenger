#pragma once

#include "cData.h"
#include "cTerrain.h"
#include "Globals.h"
#include "cShader.h"

class cScene
{
public:
	cScene(void);
	virtual ~cScene(void);

	void LoadLevel(int level, cTerrain *Terrain, float zfar);
	void Draw(cData *Data, cTerrain *Terrain, cShader *Shader, Coord playerPos);
};
