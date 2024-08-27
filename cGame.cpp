#include "cGame.h"

cGame::cGame(void) {}
cGame::~cGame(void){}

bool cGame::Init(int lvl)
{
	bool res = true;
	noclip = false;
	
	time = ang = 0.0f;
	noclipSpeedF = 1.0f; 
	level = lvl;
	state = STATE_RUN;
	

	//Graphics initialization
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,CAMERA_ZNEAR,CAMERA_ZFAR);
	glMatrixMode(GL_MODELVIEW);

	const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_position[] = { 50.0f, 50.0f, 50.0f, 0.0f };
	const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat mat_specular[]   = { 0.3f, 0.3f, 0.3f, 1.0f };
	const GLfloat mat_shininess[]  = { 100.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Texture initialization
	Data.Load();
	Scene.LoadLevel(level,&Terrain,CAMERA_ZFAR);

	//Sound initialization
	Sound.Load();

	//Shader initialization
	Shader.Load();

	//Player initialization
	Player.SetPos(TERRAIN_SIZE/2,Terrain.GetHeight(TERRAIN_SIZE/2,TERRAIN_SIZE/2)+RADIUS,TERRAIN_SIZE/2);

	Sound.Play(SOUND_AMBIENT);
	
	return res;
}

bool cGame::Loop()
{
	bool res=true;
	int t1,t2;
	t1 = glutGet(GLUT_ELAPSED_TIME);

	if(state == STATE_RUN)
	{
		res = Process();
		if(res) Render();
	}

	do { t2 = glutGet(GLUT_ELAPSED_TIME);
	} while(t2-t1 < 1000/FRAMERATE);
	return res;
}

void cGame::ReadKeyboard(unsigned char key, int x, int y, bool press)
{
	if(key >= 'A' && key <= 'Z') key += 32;
	keys[key] = press;
}

void cGame::ReadMouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) {
        if(button == GLUT_LEFT_BUTTON) {
            mouse_left_down = true;
        }
        else if(button == GLUT_RIGHT_BUTTON) {
            mouse_right_down = true;
        }
    }
    else if(state == GLUT_UP) {
        if(button == GLUT_LEFT_BUTTON) {
            mouse_left_down = false;
        }
        else if(button == GLUT_RIGHT_BUTTON) {
            mouse_right_down = false;
        }
    }
}

void cGame::ReadMouseMotion(int x, int y)
{

    static bool just_warped = false;

    if(just_warped) {
        just_warped = false;
        return;
    }

    int dx = x - SCREEN_WIDTH/2;
    int dy = y - SCREEN_HEIGHT/2;

    if(dx) {
        Camera.RotateYaw(CAMERA_SPEED*dx);
		if(Camera.GetState() != STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw());
    }

    if(dy) {
        Camera.RotatePitch(-CAMERA_SPEED*dy);
    }

    glutWarpPointer(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

    just_warped = true;
}


bool cGame::Process()
{
	bool res=true;

	if(keys[27])	res=false;

	float vx,vy,vz;
	Camera.GetDirectionVector(vx,vy,vz);
	float factor = sqrt( 1.0f/(vx*vx + vz*vz) );

	if(keys['1']) Camera.SetState(STATE_FPS);
	Player.SetFade(!keys['2']);
	if(keys['3']) Camera.SetState(STATE_TPS);

	if(noclip)
	{
		if(keys[P_UP])
		{
			Player.SetX(Player.GetX() + noclipSpeedF*vx);
			Player.SetY(Player.GetY() + noclipSpeedF*vy);
			Player.SetZ(Player.GetZ() + noclipSpeedF*vz);
			if(Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw());
		}
		else if(keys[P_DOWN])
		{
			Player.SetX(Player.GetX() - noclipSpeedF*vx);
			Player.SetY(Player.GetY() - noclipSpeedF*vy);
			Player.SetZ(Player.GetZ() - noclipSpeedF*vz);
			if(Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw()-PI);
		}
		if(keys[P_LEFT])
		{
			Player.SetX(Player.GetX() + noclipSpeedF*vz*factor);
			Player.SetZ(Player.GetZ() - noclipSpeedF*vx*factor);
			if(Camera.GetState() == STATE_TPS_FREE)
			{
				if(keys['w']) Camera.SetLastYaw(Camera.GetYaw()-PI/4);
				else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw()-(PI*3)/4);
				else Camera.SetLastYaw(Camera.GetYaw()-PI/2);
			}
		}
		else if(keys[P_RIGHT])
		{
			Player.SetX(Player.GetX() - noclipSpeedF*vz*factor);
			Player.SetZ(Player.GetZ() + noclipSpeedF*vx*factor);
			if(Camera.GetState() == STATE_TPS_FREE)
			{
				if(keys['w']) Camera.SetLastYaw(Camera.GetYaw()+PI/4);
				else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw()+(PI*3)/4);
				else Camera.SetLastYaw(Camera.GetYaw()+PI/2);
			}
		}
		if(mouse_left_down) Player.SetY(Player.GetY() + noclipSpeedF);
		else if(mouse_right_down) Player.SetY(Player.GetY() - noclipSpeedF);
		if(keys[P_PLUS])
		{
			noclipSpeedF += 0.01f;
			if(noclipSpeedF > 2.0f) noclipSpeedF = 2.0f;
		}
		else if(keys[P_MINUS])
		{
			noclipSpeedF -= 0.01f;
			if(noclipSpeedF < 0.05f) noclipSpeedF = 0.05f;
		}
	}
	else
	{
		if(keys[P_UP])
		{
			float nextVX = Player.GetVX() + PLAYER_SPEED*vx*factor;
			float nextVZ = Player.GetVZ() + PLAYER_SPEED*vz*factor;
			float limitation_factor;
			if( sqrt(nextVX*nextVX + nextVZ*nextVZ) <= MAX_MOVEMENT ) limitation_factor = 1.0f;
			else limitation_factor = sqrt( (MAX_MOVEMENT*MAX_MOVEMENT)/(nextVX*nextVX + nextVZ*nextVZ) );
			Player.SetVX(nextVX*limitation_factor);
			Player.SetVZ(nextVZ*limitation_factor);

			if(Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw());
		}
		else if(keys[P_DOWN])
		{
			float nextVX = Player.GetVX() - PLAYER_SPEED*vx*factor;
			float nextVZ = Player.GetVZ() - PLAYER_SPEED*vz*factor;
			float limitation_factor;
			if( sqrt(nextVX*nextVX + nextVZ*nextVZ) <= MAX_MOVEMENT ) limitation_factor = 1.0f;
			else limitation_factor = sqrt( (MAX_MOVEMENT*MAX_MOVEMENT)/(nextVX*nextVX + nextVZ*nextVZ) );
			Player.SetVX(nextVX*limitation_factor);
			Player.SetVZ(nextVZ*limitation_factor);

			if(Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw()-PI);
		}
		if(keys[P_LEFT])
		{
			float nextVX = Player.GetVX() + PLAYER_SPEED*vz*factor;
			float nextVZ = Player.GetVZ() - PLAYER_SPEED*vx*factor;
			float limitation_factor;
			if( sqrt(nextVX*nextVX + nextVZ*nextVZ) <= MAX_MOVEMENT ) limitation_factor = 1.0f;
			else limitation_factor = sqrt( (MAX_MOVEMENT*MAX_MOVEMENT)/(nextVX*nextVX + nextVZ*nextVZ) );
			Player.SetVX(nextVX*limitation_factor);
			Player.SetVZ(nextVZ*limitation_factor);

			if(Camera.GetState() == STATE_TPS_FREE)
			{
				if(keys['w']) Camera.SetLastYaw(Camera.GetYaw()-PI/4);
				else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw()-(PI*3)/4);
				else Camera.SetLastYaw(Camera.GetYaw()-PI/2);
			}
		}
		else if(keys[P_RIGHT])
		{
			float nextVX = Player.GetVX() - PLAYER_SPEED*vz*factor;
			float nextVZ = Player.GetVZ() + PLAYER_SPEED*vx*factor;
			float limitation_factor;
			if( sqrt(nextVX*nextVX + nextVZ*nextVZ) <= MAX_MOVEMENT ) limitation_factor = 1.0f;
			else limitation_factor = sqrt( (MAX_MOVEMENT*MAX_MOVEMENT)/(nextVX*nextVX + nextVZ*nextVZ) );
			Player.SetVX(nextVX*limitation_factor);
			Player.SetVZ(nextVZ*limitation_factor);
			
			if(Camera.GetState() == STATE_TPS_FREE)
			{
				if(keys['w']) Camera.SetLastYaw(Camera.GetYaw()+PI/4);
				else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw()+(PI*3)/4);
				else Camera.SetLastYaw(Camera.GetYaw()+PI/2);
			}
		}
		if(keys[P_JUMP])
		{
			if(Player.GetY()-RADIUS < Terrain.GetHeight(Player.GetX(),Player.GetZ())+0.01f)
			{
				Player.SetVY(PLAYER_JUMP_SPEED);
				Sound.PlayBounce(1.0f);
			}
		}

		float initial_z = Player.GetZ();
		Physics(Player);

		Coord P; P.x = Player.GetX(); P.y = Player.GetY(); P.z = Player.GetZ();
		float r = RADIUS;

	}

	Sound.Update();

	return res;
}

void cGame::Physics(cCords &object)
{
	Coord initialPos; initialPos.x = object.GetX(); initialPos.y = object.GetY(); initialPos.z = object.GetZ();
	Coord center; center.x = object.GetX() + object.GetVX(); center.y = object.GetY() + object.GetVY(); center.z = object.GetZ() + object.GetVZ();
	std::vector<Vector> cnormals = Terrain.GetCollisionNormals(center,RADIUS);
	object.SetPos(center.x,center.y,center.z); 

	if(object.GetZ() != initialPos.z || object.GetX() != initialPos.x)
	{
		float yaw,pitch;
		float dx = abs(abs(object.GetX()) - abs(initialPos.x)), dz = abs(abs(object.GetZ()) - abs(initialPos.z));
		if(object.GetZ() > initialPos.z && object.GetX() >= initialPos.x) yaw = atan( dx / dz ); 
		if(object.GetZ() <= initialPos.z && object.GetX() > initialPos.x) yaw = PI/2 + atan( dz / dx ); 
		if(object.GetZ() < initialPos.z && object.GetX() <= initialPos.x) yaw = PI + atan( dx / dz );
		if(object.GetZ() >= initialPos.z && object.GetX() < initialPos.x) yaw = PI*3/2 + atan( dz / dx );
		object.SetYaw(yaw*(180/PI));

		float perimeter = PI*2*RADIUS;
		float dy = abs(abs(object.GetY()) - abs(initialPos.y));
		float travel_dist = sqrt( dx*dx + dy*dy + dz*dz );
		if(cos(yaw) >= 0.0f) pitch = object.GetPitch() + (travel_dist/perimeter) * 360.0f;
		else pitch = object.GetPitch() - (travel_dist/perimeter) * 360.0f;
		if(pitch < 0.0f) pitch = 360.0f - abs(pitch);
		object.SetPitch(fmod(pitch,360.0f));
	}

	if(cnormals.empty()) object.SetVY(object.GetVY() - GRAVITY);
	else
	{
		Vector G,F,G1,F1,cNormal;
		float rz,rx; 
		float factor,N = 0.0f;

		G.x = 0.0f; G.y = -GRAVITY; G.z = 0.0f;
		F.x = object.GetVX(); F.y = object.GetVY(); F.z = object.GetVZ();
		cNormal.x = 0.0f; cNormal.y = 0.0f; cNormal.z = 0.0f;

		for(unsigned int i=0; i<cnormals.size(); i++)
		{
			if(cnormals[i].x == 0.0f) rz = 0.0f;
			else if(cnormals[i].x >  0.0f) rz = -PI/2 + atan(cnormals[i].y/cnormals[i].x);
			else rz = PI/2 + atan(cnormals[i].y/cnormals[i].x);

			if(cnormals[i].z == 0.0f) rx = 0.0f;
			else if(cnormals[i].z >  0.0f) rx = PI/2 - atan(cnormals[i].y/cnormals[i].z);
			else rx = -PI/2 - atan(cnormals[i].y/cnormals[i].z);

			G1.x = cos(-rz)*G.x - sin(-rz)*G.y;
			G1.y = cos(-rx)*sin(-rz)*G.x + cos(-rx)*cos(-rz)*G.y - sin(-rx)*G.z;
			G1.z = sin(-rx)*sin(-rz)*G.x + sin(-rx)*cos(-rz)*G.y + cos(-rx)*G.z;

			F1.x = cos(-rz)*F.x - sin(-rz)*F.y;
			F1.y = cos(-rx)*sin(-rz)*F.x + cos(-rx)*cos(-rz)*F.y - sin(-rx)*F.z;
			F1.z = sin(-rx)*sin(-rz)*F.x + sin(-rx)*cos(-rz)*F.y + cos(-rx)*F.z;
			
			float cN = 0.0f;
			if (G1.y < 0.0f) {cN -= G1.y; G1.y = 0.0f;}
			if (F1.y < 0.0f) {cN -= F1.y; F1.y = 0.0f;}
			N += cN; 
			cNormal.x += cnormals[i].x;
			cNormal.y += cnormals[i].y;
			cNormal.z += cnormals[i].z;

			if(cN > 0.0f && abs(F1.x) + abs(F1.z) > 0.0f)
			{
				factor = sqrt( ((FRICTION*cN)*(FRICTION*cN)) / (F1.x*F1.x + F1.z*F1.z) );

				if(abs(F1.x) < abs(F1.x*factor)) F1.x = 0.0f;
				else F1.x -= F1.x*factor;

				if(abs(F1.z) < abs(F1.z*factor)) F1.z = 0.0f;
				else F1.z -= F1.z*factor;
			}
				G.x = cos(rz)*G1.x - sin(rz)*cos(rx)*G1.y + sin(rz)*sin(rx)*G1.z;
			G.y = sin(rz)*G1.x + cos(rz)*cos(rx)*G1.y - cos(rz)*sin(rx)*G1.z;
			G.z = sin(rx)*G1.y + cos(rx)*G1.z;

			F.x = cos(rz)*F1.x - sin(rz)*cos(rx)*F1.y + sin(rz)*sin(rx)*F1.z;
			F.y = sin(rz)*F1.x + cos(rz)*cos(rx)*F1.y - cos(rz)*sin(rx)*F1.z;
			F.z = sin(rx)*F1.y + cos(rx)*F1.z;
		}

		float nextVX = F.x + G.x;
		float nextVY = F.y + G.y;
		float nextVZ = F.z + G.z;

		float limitation_factor;
		if( sqrt(nextVX*nextVX + nextVY*nextVY + nextVZ*nextVZ) <= MAX_MOVEMENT ) limitation_factor = 1.0f;
		else limitation_factor = sqrt( (MAX_MOVEMENT*MAX_MOVEMENT)/(nextVX*nextVX + nextVY*nextVY + nextVZ*nextVZ) );
		
		nextVX *= limitation_factor;
		nextVY *= limitation_factor;
		nextVZ *= limitation_factor;

		if(N > GRAVITY*4) factor = sqrt( (N*N) / (cNormal.x*cNormal.x + cNormal.y*cNormal.y + cNormal.z*cNormal.z) );
		else factor = 0.0f;

		nextVX += cNormal.x*factor*ELASTICITY;
		nextVY += cNormal.y*factor*ELASTICITY;
		nextVZ += cNormal.z*factor*ELASTICITY;
		
		float bounceForce = sqrt( (cNormal.x*factor*ELASTICITY)*(cNormal.x*factor*ELASTICITY) + (cNormal.y*factor*ELASTICITY)*(cNormal.y*factor*ELASTICITY) + (cNormal.z*factor*ELASTICITY)*(cNormal.z*factor*ELASTICITY) );
		if(bounceForce >= PLAYER_JUMP_SPEED) Sound.PlayBounce(1.0f);
		else if(bounceForce/PLAYER_JUMP_SPEED > 0.2f) Sound.PlayBounce(bounceForce/PLAYER_JUMP_SPEED);

		object.SetVel(nextVX, nextVY, nextVZ);
	}
}


void cGame::Reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei)w, (GLsizei)h); 
    glMatrixMode (GL_PROJECTION); 

    glLoadIdentity ();
    gluPerspective (45.0, (GLfloat)w / (GLfloat)h, CAMERA_ZNEAR , CAMERA_ZFAR); 
    glMatrixMode (GL_MODELVIEW); 
}

void cGame::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	Camera.Update(&Terrain, Player.GetX(), Player.GetY(), Player.GetZ());
	ang = fmod(ang+2,360);

	Coord playerPos; playerPos.x = Player.GetX(); playerPos.y = Player.GetY(); playerPos.z = Player.GetZ();

	Scene.Draw(&Data,&Terrain,&Shader,playerPos);	
	if(abs(Camera.GetZ()) < Camera.GetDistance())
		Player.Draw(&Data,&Camera,&Shader);
	else
		Player.Draw(&Data,&Camera,&Shader);

	glutSwapBuffers();
}