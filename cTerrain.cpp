#include "cTerrain.h"

cTerrain::cTerrain(){}
cTerrain::~cTerrain(){}

Vector cross(Vector a, Vector b)
{
	Vector vec;
	vec.x = a.y*b.z - a.z*b.y;
	vec.y = a.z*b.x - a.x*b.z;
	vec.z = a.x*b.y - a.y*b.x;

	return vec;
}

bool fcompare(Vector a, Vector b) { return (abs(a.y) > abs(b.y)); }

void cTerrain::ComputeTriangle(std::vector<Coord> &triangle)
{
	Triangle tri;

	
	tri.vertexs[0] = triangle[0];
	tri.vertexs[1] = triangle[1];
	tri.vertexs[2] = triangle[2];

	
	tri.barycenter.x = (triangle[0].x + triangle[1].x + triangle[2].x)/3;
	tri.barycenter.y = (triangle[0].y + triangle[1].y + triangle[2].y)/3;
	tri.barycenter.z = (triangle[0].z + triangle[1].z + triangle[2].z)/3;

	
	Vector a,b;
	if(triangles.size()%2 != 0) std::swap(triangle[2],triangle[0]);
	a.x = triangle[1].x - triangle[0].x;
	a.y = triangle[1].y - triangle[0].y;
	a.z = triangle[1].z - triangle[0].z;

	b.x = triangle[2].x - triangle[1].x;
	b.y = triangle[2].y - triangle[1].y;
	b.z = triangle[2].z - triangle[1].z;
	if(triangles.size()%2 != 0) std::swap(triangle[2],triangle[0]);

	Vector aux = cross(a,b);
	float factor = sqrt( 1.0f/(aux.x*aux.x + aux.y*aux.y + aux.z*aux.z) );
	aux.x *= factor;
	aux.y *= factor;
	aux.z *= factor;
	tri.N = aux;

	triangles.push_back(tri);
	std::swap(triangle[1],triangle[0]);
	std::swap(triangle[2],triangle[1]);
	triangle.pop_back();
}

void cTerrain::SetNormalPerVertex(int x,float y,int z)
{
	Vector X,Z,N; X.x = 2.0f; X.z = 0.0f; Z.x = 0.0f; Z.z = 2.0f;
	float left_y, right_y, up_y, down_y;
	
	if(x == 0)
	{
		left_y  = y;
		right_y = heightmap[z * TERRAIN_SIZE + (x+1)] / SCALE_FACTOR;
	}
	else if(x == TERRAIN_SIZE-1)
	{
		left_y  = heightmap[z * TERRAIN_SIZE + (x-1)] / SCALE_FACTOR;
		right_y = y;
	}
	else
	{
		left_y  = heightmap[z * TERRAIN_SIZE + (x-1)] / SCALE_FACTOR;
		right_y = heightmap[z * TERRAIN_SIZE + (x+1)] / SCALE_FACTOR;
	}

	if(z == 0)
	{
		up_y   = y;
		down_y = heightmap[(z+1) * TERRAIN_SIZE + x] / SCALE_FACTOR;
	}
	else if(z == TERRAIN_SIZE-1)
	{
		up_y   = heightmap[(z-1) * TERRAIN_SIZE + x] / SCALE_FACTOR;
		down_y = y;
	}
	else
	{
		up_y   = heightmap[(z-1) * TERRAIN_SIZE + x] / SCALE_FACTOR;
		down_y = heightmap[(z+1) * TERRAIN_SIZE + x] / SCALE_FACTOR;
	}

	X.y = right_y - left_y;
	Z.y = down_y - up_y;

	N = cross(Z,X);
	float factor = sqrt( 1.0f/(N.x*N.x + N.y*N.y + N.z*N.z) );
	N.x *= factor; N.y *= factor; N.z *= factor;

	glNormal3f(N.x,N.y,N.z);
}

void cTerrain::Load(int level)
{
	triangles.clear();

	char file[32];
	if(level<10) sprintf(file,"Levels/terrain0%d.raw",level);
	else		 sprintf(file,"Levels/terrain%d.raw",level);
	FILE *pFile = fopen(file, "rb");
	fread(&heightmap,TERRAIN_SIZE * TERRAIN_SIZE,1,pFile); 
	fclose(pFile);

	id_Terrain=glGenLists(2);
	glNewList(id_Terrain,GL_COMPILE);
	for (int z = 0; z <TERRAIN_SIZE-1 ; z++)
	{
		std::vector<Coord> triangle(0);
		Coord vertex;

		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < TERRAIN_SIZE; x++)
		{
			
			float scaledHeight = heightmap[z * TERRAIN_SIZE + x] / SCALE_FACTOR;
			float nextScaledHeight = heightmap[(z + 1) * TERRAIN_SIZE + x] / SCALE_FACTOR;

			glTexCoord2f((float)x/TERRAIN_SIZE*64, (float)z/TERRAIN_SIZE*64);
			SetNormalPerVertex(x, scaledHeight, z);
			glVertex3f(x, scaledHeight, z);
			vertex.x=x; vertex.y=scaledHeight; vertex.z=z;
			triangle.push_back(vertex);
			if(triangle.size() == 3) ComputeTriangle(triangle);

			glTexCoord2f((float)x/TERRAIN_SIZE*64, (float)(z + 1)/TERRAIN_SIZE*64);
			SetNormalPerVertex(x, scaledHeight, (z + 1));
			glVertex3f(x, nextScaledHeight, (z + 1));
			vertex.x=x; vertex.y=nextScaledHeight; vertex.z=z+1;
			triangle.push_back(vertex);
			if(triangle.size() == 3) ComputeTriangle(triangle);
		}
		glEnd();
	}
	glEndList();

	id_Normals=glGenLists(1);
	glNewList(id_Normals,GL_COMPILE);
	glLineWidth(1.0);
	glColor3f(0.5, 0.5, 1.0);
	for(unsigned int i=0; i<triangles.size(); i++)
	{
		glBegin(GL_LINES);
		glVertex3f(triangles[i].barycenter.x, triangles[i].barycenter.y, triangles[i].barycenter.z);
		glVertex3f(triangles[i].barycenter.x + triangles[i].N.x, triangles[i].barycenter.y + triangles[i].N.y, triangles[i].barycenter.z + triangles[i].N.z);
		glEnd();
	}
	glColor3f(1.0,1.0,1.0);
	glEndList();
}

void cTerrain::Draw()
{
	glCallList(id_Terrain);
	
}

float cTerrain::GetVertexHeigh(int x, int z)
{
	return heightmap[(int)z * TERRAIN_SIZE + (int)x] / SCALE_FACTOR;
}

float cTerrain::GetHeight(float x,float z)
{
	if(x < 0 || x > TERRAIN_SIZE-1 || z < 0 || z > TERRAIN_SIZE-1) return 0.0f;
	else
	{
		int intx, intz;
		float fracx, fracz, height;

		intx = (int)x;
		intz = (int)z;

		fracx = fmod(x,1);
		fracz = fmod(z,1);
		
		if(x == (float)intx && z == (float)intz) return GetVertexHeigh(intx, intz);
		if(x == (float)intx) return GetVertexHeigh(intx, intz)*(1-fracz) + GetVertexHeigh(intx, intz+1)*fracz;
		if(z == (float)intz) return GetVertexHeigh(intx, intz)*(1-fracx) + GetVertexHeigh(intx+1, intz)*fracx;

		if(fracz == 1.0f - fracx) 
		{
			return GetVertexHeigh(intx+1, intz)*(fracz/sin(45.0f)) + GetVertexHeigh(intx, intz+1)*(1-fracz/sin(45.0f));
		}
		if(fracz < 1.0f - fracx) 
		{
			height = GetVertexHeigh(intx, intz)*(1-fracx) + GetVertexHeigh(intx+1, intz)*fracx;
			return height*(1-fracz) + GetVertexHeigh(intx, intz+1)*fracz;
		}
		if(fracz > 1.0f - fracx) 
		{
			height = GetVertexHeigh(intx, intz+1)*(1-fracx) + GetVertexHeigh(intx+1, intz+1)*fracx;
			return height*fracz + GetVertexHeigh(intx+1, intz)*(1-fracz);
		}
		return 0.0f;
	}
}

bool cTerrain::IsColliding(Coord P, float radius, Triangle tri,Coord &center)
{
	Coord Q;  Q.x = P.x + radius*(-tri.N.x);  Q.y = P.y + radius*(-tri.N.y);  Q.z = P.z + radius*(-tri.N.z);
	Vector V;  V.x = Q.x - P.x;  V.y = Q.y - P.y;  V.z = Q.z - P.z;
	float D = -(tri.N.x*tri.vertexs[0].x + tri.N.y*tri.vertexs[0].y + tri.N.z*tri.vertexs[0].z);
	float lambda = -(tri.N.x*P.x + tri.N.y*P.y + tri.N.z*P.z + D) / (tri.N.x*V.x + tri.N.y*V.y + tri.N.z*V.z);

	if(lambda > 1.0f) return false; 
	Coord I;  I.x = P.x + lambda*V.x;  I.y = P.y + lambda*V.y;  I.z = P.z + lambda*V.z; 

	float tri_orientation = (tri.vertexs[0].x - tri.vertexs[2].x) * (tri.vertexs[1].z - tri.vertexs[2].z) - (tri.vertexs[0].z - tri.vertexs[2].z) * (tri.vertexs[1].x - tri.vertexs[2].x);
	float ABIor = (tri.vertexs[0].x - I.x) * (tri.vertexs[1].z - I.z) - (tri.vertexs[0].z - I.z) * (tri.vertexs[1].x - I.x);
	float BCIor = (tri.vertexs[1].x - I.x) * (tri.vertexs[2].z - I.z) - (tri.vertexs[1].z - I.z) * (tri.vertexs[2].x - I.x);
	float CAIor = (tri.vertexs[2].x - I.x) * (tri.vertexs[0].z - I.z) - (tri.vertexs[2].z - I.z) * (tri.vertexs[0].x - I.x);
	if( tri_orientation >= 0.0f && (ABIor < 0.0f  || BCIor < 0.0f || CAIor < 0.0f) ) return false;
	if( tri_orientation < 0.0f && (ABIor >= 0.0f  || BCIor >= 0.0f || CAIor >= 0.0f) ) return false;
	
	float factor = sqrt( (radius*radius)/(tri.N.x*tri.N.x + tri.N.y*tri.N.y + tri.N.z*tri.N.z) );
	center.x = I.x + tri.N.x*factor;
	center.y = I.y + tri.N.y*factor;
	center.z = I.z + tri.N.z*factor;

	return true;
}

std::vector<Vector> cTerrain::GetCollisionNormals(Coord &center, float radius)
{
	std::vector<Vector> cnormals(0);

	if(center.x < radius || center.x > (float)TERRAIN_SIZE-1.0f-radius || center.z < radius || center.z > (float)TERRAIN_SIZE-1.0f-radius)
	{
		Vector N;
		N.x = 0.0f; N.y = 1.0f; N.z = 0.0f;
		if(center.y <= radius) { center.y = radius; cnormals.push_back(N); }
		return cnormals;
	}
	else
	{
		int intx, intz;
		float fracx, fracz;
		int trianglex, trianglez;
		Coord P; P.x = center.x; P.y = center.y; P.z = center.z;
		Triangle tri;

		intx = (int)center.x;
		intz = (int)center.z;

		fracx = fmod(center.x,1);
		fracz = fmod(center.z,1);
		trianglex = intx*2;
		trianglez = intz;

		if(fracx == 0.5f && fracz == 0.5f)
		{
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			sort(cnormals.begin(),cnormals.end(),fcompare);
			return cnormals;
		}
		if(fracx == 0.5f)
		{
			if(fracz < 0.5f)
			{
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex+1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				sort(cnormals.begin(),cnormals.end(),fcompare);
			}
			else
			{
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex+1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				sort(cnormals.begin(),cnormals.end(),fcompare);
			}
			return cnormals;
		}
		if(fracz == 0.5f)
		{
			if(fracx < 0.5f)
			{
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex-2];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex-1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				sort(cnormals.begin(),cnormals.end(),fcompare);
			}
			else
			{
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+2];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
				tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+3];
				if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

				sort(cnormals.begin(),cnormals.end(),fcompare);
			}
			return cnormals;
		}
		if(fracx < 0.5 && fracz < 0.5)
		{
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex-2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex-1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);


			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex-2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex-1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			sort(cnormals.begin(),cnormals.end(),fcompare);
			return cnormals;
		}
		if(fracx > 0.5 && fracz < 0.5)
		{
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+3];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);


			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex+2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez-1)*(TERRAIN_SIZE-1)*2 + trianglex+3];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			sort(cnormals.begin(),cnormals.end(),fcompare);
			return cnormals;
		}
		if(fracx < 0.5 && fracz > 0.5)
		{
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex-2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex-1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);


			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex-2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex-1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			sort(cnormals.begin(),cnormals.end(),fcompare);
			return cnormals;
		}
		if(fracx > 0.5 && fracz > 0.5)
		{
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[trianglez*(TERRAIN_SIZE-1)*2 + trianglex+3];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);


			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex+1];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex+2];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);
			
			tri = triangles[(trianglez+1)*(TERRAIN_SIZE-1)*2 + trianglex+3];
			if(IsColliding(P,radius,tri,center)) cnormals.push_back(tri.N);

			sort(cnormals.begin(),cnormals.end(),fcompare);
			return cnormals;
		}
		return cnormals; 
	}
}

bool cTerrain::IsColliding(Coord P, Coord Q, Triangle tri,float &lambda)
{
	Vector V;  V.x = Q.x - P.x;  V.y = Q.y - P.y;  V.z = Q.z - P.z;
	float D = -(tri.N.x*tri.vertexs[0].x + tri.N.y*tri.vertexs[0].y + tri.N.z*tri.vertexs[0].z);
	if(tri.N.x*V.x + tri.N.y*V.y + tri.N.z*V.z == 0.0f) return false;
	else lambda = -(tri.N.x*P.x + tri.N.y*P.y + tri.N.z*P.z + D) / (tri.N.x*V.x + tri.N.y*V.y + tri.N.z*V.z);

	if(lambda < 0.0f || lambda > 1.0f) return false;
	Coord I;  I.x = P.x + lambda*V.x;  I.y = P.y + lambda*V.y;  I.z = P.z + lambda*V.z; 

		float tri_orientation = (tri.vertexs[0].x - tri.vertexs[2].x) * (tri.vertexs[1].z - tri.vertexs[2].z) - (tri.vertexs[0].z - tri.vertexs[2].z) * (tri.vertexs[1].x - tri.vertexs[2].x);
	float ABIor = (tri.vertexs[0].x - I.x) * (tri.vertexs[1].z - I.z) - (tri.vertexs[0].z - I.z) * (tri.vertexs[1].x - I.x);
	float BCIor = (tri.vertexs[1].x - I.x) * (tri.vertexs[2].z - I.z) - (tri.vertexs[1].z - I.z) * (tri.vertexs[2].x - I.x);
	float CAIor = (tri.vertexs[2].x - I.x) * (tri.vertexs[0].z - I.z) - (tri.vertexs[2].z - I.z) * (tri.vertexs[0].x - I.x);
	if( tri_orientation >= 0.0f && (ABIor < 0.0f  || BCIor < 0.0f || CAIor < 0.0f) ) return false;
	if( tri_orientation < 0.0f && (ABIor >= 0.0f  || BCIor >= 0.0f || CAIor >= 0.0f) ) return false;

	return true;
}

float cTerrain::GetSegmentIntersectionLambda(float x,float y,float z, float vx,float vy,float vz, float dist)
{
	Coord P; P.x = x; P.y = y;  P.z = z;
	Coord Q; Q.x = x - dist*vx; Q.y = y - dist*vy; Q.z = z - dist*vz;
	Triangle tri;
	float lambda, lowest_lambda = 1.0f;
	int trianglePx = ((int)P.x)*2, trianglePz = (int)P.z;
	int	triangleQx = ((int)Q.x)*2, triangleQz = (int)Q.z;

	if( (trianglePx < 0 && triangleQx < 0) || (trianglePx > (TERRAIN_SIZE-2)*2 && triangleQx > (TERRAIN_SIZE-2)*2) ) return lowest_lambda;
	if( (trianglePz < 0 && triangleQz < 0) || (trianglePz > (TERRAIN_SIZE-2)   && triangleQz > (TERRAIN_SIZE-2)  ) ) return lowest_lambda;
	
	if(trianglePx < 0) trianglePx = 0;
	else if(trianglePx > (TERRAIN_SIZE-2)*2) trianglePx = (TERRAIN_SIZE-2)*2;
	if(trianglePz < 0) trianglePz = 0;
	else if(trianglePz > TERRAIN_SIZE-2) trianglePz = TERRAIN_SIZE-2;

	if(triangleQx < 0) triangleQx = 0;
	else if(triangleQx > (TERRAIN_SIZE-2)*2) triangleQx = (TERRAIN_SIZE-2)*2;
	if(triangleQz < 0) triangleQz = 0;
	else if(triangleQz > TERRAIN_SIZE-2) triangleQz = TERRAIN_SIZE-2;
		if(trianglePx == triangleQx)
	{
		if(trianglePz > triangleQz) std::swap(trianglePz,triangleQz);
		for(int j = trianglePz; j <= triangleQz; j++)
		{
			tri = triangles[j*(TERRAIN_SIZE-1)*2 + trianglePx];
			if(IsColliding(P,Q,tri,lambda) && lambda < lowest_lambda) lowest_lambda = lambda;
			tri = triangles[j*(TERRAIN_SIZE-1)*2 + trianglePx+1];
			if(IsColliding(P,Q,tri,lambda) && lambda < lowest_lambda) lowest_lambda = lambda;
		}
		return lowest_lambda;
	}
	if(trianglePz == triangleQz)
	{
		if(trianglePx > triangleQx) std::swap(trianglePx,triangleQx);
		for(int i = trianglePx; i <= triangleQx+1; i++)
		{
			tri = triangles[trianglePz*(TERRAIN_SIZE-1)*2 + i];
			if(IsColliding(P,Q,tri,lambda) && lambda < lowest_lambda) lowest_lambda = lambda;
		}
		return lowest_lambda;
	}

	if(trianglePx > triangleQx) std::swap(trianglePx,triangleQx);
	if(trianglePz > triangleQz) std::swap(trianglePz,triangleQz);
	for(int i = trianglePx; i <= triangleQx+1; i++)
	{
		for(int j = trianglePz; j <= triangleQz; j++)
		{
			tri = triangles[j*(TERRAIN_SIZE-1)*2 + i];
			if(IsColliding(P,Q,tri,lambda) && lambda < lowest_lambda) lowest_lambda = lambda;
		}
	}
	
	return lowest_lambda;
}