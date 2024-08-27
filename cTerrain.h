#pragma once

#include "Globals.h"

#define TERRAIN_SIZE    1024
const float MAX_HEIGHT = 64.0f;
const float SCALE_FACTOR = 256.0f / MAX_HEIGHT;
struct Coord {float x,y,z;};
struct Vector {float x,y,z;};
struct Triangle {Vector N; Coord barycenter; Coord vertexs[3];};

class cTerrain
{
public:
	cTerrain();
	~cTerrain();
	void  Load(int level);
	void  Draw();
	float GetHeight(float x,float z);
	std::vector<Vector> GetCollisionNormals(Coord &center, float radius);
	float GetSegmentIntersectionLambda(float x,float y,float z, float vx,float vy,float vz, float dist);

private:
	GLubyte heightmap[TERRAIN_SIZE * TERRAIN_SIZE]; 
	std::vector<Triangle> triangles;
	int id_Terrain,id_Normals;

	void  ComputeTriangle(std::vector<Coord> &triangle);
	void  SetNormalPerVertex(int x,float y,int z);
	float GetVertexHeigh(int x, int z);
	bool  IsColliding(Coord P, float radius, Triangle tri,Coord &center);
	bool  IsColliding(Coord P, Coord Q, Triangle tri,float &lambda);
};
