#include "cScene.h"

cScene::cScene(void){}
cScene::~cScene(void){}


void cScene::LoadLevel(int level, cTerrain *Terrain, float zfar)
{
	Terrain->Load(level);

}

void DrawOGLAxis()
{
	GLUquadricObj *q = gluNewQuadric();


	glPushMatrix();
	glColor3f(0.0,0.0,1.0);
	gluCylinder(q,0.1f,0.0f,3.0f,3,1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 
	glColor3f(1.0,0.0,0.0);
	gluCylinder(q,0.1f,0.0f,3.0f,3,1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
	glColor3f(0.0,1.0,0.0);
	gluCylinder(q,0.1f,0.0f,3.0f,3,1);
	glPopMatrix();

	gluDeleteQuadric(q);
	glColor3f(1.0,1.0,1.0); 
}

void cScene::Draw(cData *Data, cTerrain *Terrain, cShader *Shader, Coord playerPos)
{
	glDisable(GL_LIGHTING);

	glDisable(GL_DEPTH_TEST); 
	
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_SPACE));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_ROCK));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_SIMPLE_TERRAIN);
	Shader->SetUniform("tex_top", 0);
	Shader->SetUniform("tex_side", 1);

	Terrain->Draw();
	Shader->Deactivate();

	glEnable(GL_LIGHTING);
}