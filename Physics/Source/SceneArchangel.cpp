#include "SceneArchangel.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
using namespace std;

SceneArchangel::SceneArchangel()
{
}

SceneArchangel::~SceneArchangel()
{
}

void SceneArchangel::Init()
{
	SceneBase::Init();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	// Initialize variables
	m_objectCount = 0;
	jump = false;
	portal_in = false;
	move_portal_in = false;
	move_portal_out = false;
	portal_shot = false;
	max_vel = 50;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Initialize Game state
	state = STATE_MENU;

	m_player = FetchGO();

	// Set m_player stats
	m_player->active = true;
	m_player->type = GameObject::GO_CUBE;
	m_player->pos = Vector3(m_worldWidth * 0.5, 30, 0);
	m_player->normal.Set(1, 0, 0);
	m_player->scale = Vector3(2, 2, 2);
	m_player->bullet_delay = 0;
}

GameObject* SceneArchangel::FetchGO()
{
	// Fetch a game object from m_goList and return it 
	for (auto go : m_goList)
	{
		if (!go->active)
		{
			// increase object count every time an object is set to active
			go->active = true;
			m_objectCount++;
			return go;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(newGO);
	}

	return FetchGO();

	return NULL;
}

void SceneArchangel::ReturnGO(GameObject::GAMEOBJECT_TYPE GO)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				go->active = false;
				m_objectCount--;
			}
		}
	}
}

Collision SceneArchangel::CheckCollision(GameObject* go1, GameObject* go2, float dt)
{
	// Handle collision between GO_BALL and GO_BALL using velocity swap
	if (go2->type == GameObject::GO_BALL)
	{
		float combinedRadii = go1->scale.x + go2->scale.x;
		Vector3 displacement = go2->pos - go1->pos;
		if ((displacement.LengthSquared() < combinedRadii * combinedRadii) && (go2->vel - go1->vel).Dot(go2->pos - go1->pos) < 0)
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -displacement.Normalize();
			collision.dist = combinedRadii - displacement.Length();
			return collision;
		}
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_CUBE)
	{
		Vector3 N = go2->normal; // go2 normal
		Vector3 NP(N.y, -N.x, 0); //  go2 right vector
		Vector3 w0_b1 = go2->pos - go1->pos; // point from go1 to go2
		float r = go1->scale.x; // go1 radius
		float h_2 = go2->scale.x; // go2 width
		float l_2 = go2->scale.y; // go2 height
		if (w0_b1.Dot(N) < 0)
			N = -N; //making sure N is outward normal
		if ((w0_b1).Dot(N) < r + h_2 && abs((w0_b1).Dot(NP)) < l_2 && go1->vel.Dot(N) > 0)
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -N.Normalize();
			collision.dist = r + h_2 - (w0_b1).Dot(N);
			return collision;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR || go2->type == GameObject::GO_CIRCLE || go2->type == GameObject::GO_POWERUP || go2->type == GameObject::GO_PORTAL_IN || go2->type == GameObject::GO_PORTAL_OUT)
	{
		Vector3 u = go1->vel;
		Vector3 p2_p1 = go2->pos - go1->pos;
		float r1 = go1->scale.x;
		float r2 = go2->scale.x;

		if ((abs(p2_p1.Length()) < r1 + r2) && ((p2_p1.Dot(u)) > 0)) //prevent internal collision
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -p2_p1.Normalize();
			collision.dist = r1 + r2 - p2_p1.Length();
			return collision;
		}
	}
	return Collision();
}

void SceneArchangel::PhysicsResponse(GameObject* go1, GameObject* go2)
{
	if (go2->type == GameObject::GO_BALL)
	{
		u1 = go1->vel;
		m1 = go1->mass;
		u2 = go2->vel;
		m2 = go2->mass;
		pi = go1->mass * u1 + go2->mass * u2;
		go1->vel = u1 * ((m1 - m2) / (m1 + m2)) + u2 * ((2 * m2) / (m1 + m2));
		go2->vel = u1 * ((2 * m1) / (m1 + m2)) + u2 * ((m2 - m1) / (m1 + m2));
		v1 = go1->vel;
		v2 = go2->vel;
		go1->vel = u1 - (2 * m2 / (m1 + m2)) * (((u1 - u2).Dot(go1->pos - go2->pos)) / (go1->pos - go2->pos).LengthSquared()) * (go1->pos - go2->pos);
		go2->vel = u2 - (2 * m1 / (m1 + m2)) * (((u2 - u1).Dot(go2->pos - go1->pos)) / (go2->pos - go1->pos).LengthSquared()) * (go2->pos - go1->pos);
		go1->vel.y *= 0.4;
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_CUBE)
	{
		if (go1->type == GameObject::GO_PORTAL_IN)
		{
			go1->vel = 0;
			portal_in = true;
			move_portal_in = true;
			go1->scale.Set(2, 2.5f, 1);
			cout << "portal in" << endl;
		}
		else if (go1->type == GameObject::GO_PORTAL_OUT)
		{
			go1->vel = 0;
			portal_in = false;
			move_portal_out = true;
			go1->scale.Set(2, 2.5f, 1);
			cout << "portal out" << endl;
		}
		else
		{
			Vector3 N = go2->normal;
			Vector3 u = go1->vel;
			go1->vel = u - (2 * u.Dot(N)) * N;
			go1->vel.y *= 0;
			jump = false;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR || go2->type == GameObject::GO_CIRCLE)
	{
		Vector3 N = (go2->pos - go1->pos).Normalized();
		Vector3 u = go1->vel;
		go1->vel = u - (2 * u).Dot(N) * N;
		go1->vel.y *= 0.4;
	}
}

void SceneArchangel::CollisionBound(GameObject* go1, Collision collision)
{
	go1->pos += collision.axis * collision.dist;
}

void SceneArchangel::SpawnBullet(double dt)
{
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	double x, y;
	Application::GetCursorPos(&x, &y);
	//Mouse Section
	static bool bLButtonState = false;
	if (Application::IsMousePressed(0))
	{
		if (m_player->bullet_delay > 0.2f)
		{
			cout << "shooting" << endl;
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_BULLET;
			newGO->scale.Set(1, 1, 0);
			newGO->pos = m_player->pos;
			newGO->vel = Vector3((x / w * m_worldWidth) - newGO->pos.x, ((h - y) / h * m_worldHeight) - newGO->pos.y, 0).Normalize() * 100;
			newGO->vel.Normalize()* 100;
			m_player->bullet_delay = 0;
		}
	}
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_BULLET)
			{
				go->pos += go->vel * dt * 100;
			}
		}
	}
}

void SceneArchangel::playerLogic(double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_CUBE)
			{
				go->pos += go->vel * dt * m_speed;

				go->vel.y -= dt * 300;

				// Setting speed limiters
				if (go->vel.x >= max_vel)
				{
					go->vel.x = max_vel;
				}
				if (go->vel.x <= -max_vel)
				{
					go->vel.x = -max_vel;
				}

				// Jump
				if (Application::IsKeyPressed(VK_SPACE) && !jump)
				{
					go->vel.y += 100;
					jump = true;
				}

				// Movement
				if (Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
				{
					go->vel.y += 3;
				}
				if (Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
				{
					go->vel.y -= 3;
				}
				if (Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
				{
					if (!jump)
						go->vel.x -= 3;
					else
						go->vel.x -= 1;
				}
				else if (Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
				{
					if (!jump)
						go->vel.x += 3;
					else
						go->vel.x += 1;
				}
				else
				{
					go->vel.x *= 0.7;
				}

				// Out of bounds checking
				if (go->pos.x + go->scale.x > m_worldWidth && go->vel.x > 0 ||
					go->pos.x - go->scale.x < 0 && go->vel.x < 0) {
					go->vel.x = 0;
				}
				if (go->pos.y + go->scale.y > m_worldHeight && go->vel.y > 0 ||
					go->pos.y - go->scale.y < 0 && go->vel.y < 0) {
					go->vel.y = 0;
				}

				if ((go->pos.x > m_worldWidth + go->scale.x || go->pos.x < 0 - go->scale.x) ||
					(go->pos.y > m_worldHeight + go->scale.y || go->pos.y < 0 - go->scale.y))
				{
					ReturnGO(GameObject::GO_CUBE);
					break;
				}

				
			}
			for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;
				if (go2->active)
				{
					GameObject* cube = go;
					GameObject* other = go2;
					if (cube->type != GameObject::GO_CUBE)
					{
						if (other->type != GameObject::GO_CUBE)
							continue;
						cube = go2;
						other = go;
					}
					Collision collision = CheckCollision(cube, other, dt);
					if (collision.dist > 0)
					{
						PhysicsResponse(cube, other);
						CollisionBound(cube, collision);
						continue;
					}
				}
			}
		}
	}
}

void SceneArchangel::portalLogic(double dt)
{
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	double x, y;
	Application::GetCursorPos(&x, &y);
	//Mouse Section
	static bool bLButtonState = false;
	if (Application::IsMousePressed(1) && !portal_shot)
	{
		if (m_player->portal_delay > 1)
		{
			if (!portal_in)
			{
				ReturnGO(GameObject::GO_PORTAL_IN);
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_PORTAL_IN;
				newGO->scale.Set(2.5f, 2, 1);
				newGO->normal.Set(1, 0, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3((x / w * m_worldWidth) - newGO->pos.x, ((h - y) / h * m_worldHeight) - newGO->pos.y, 0).Normalize() * 50;
				newGO->vel.Normalize() * 50;
				move_portal_in = false;
			}
			else
			{
				ReturnGO(GameObject::GO_PORTAL_OUT);
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_PORTAL_OUT;
				newGO->scale.Set(2.5f, 2, 1);
				newGO->normal.Set(1, 0, 0);	
				newGO->pos = m_player->pos;
				newGO->vel = Vector3((x / w * m_worldWidth) - newGO->pos.x, ((h - y) / h * m_worldHeight) - newGO->pos.y, 0).Normalize() * 50;
				newGO->vel.Normalize() * 50;
				move_portal_out = false;
			}
		}
		portal_shot = true;
	}
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_PORTAL_IN || go->type == GameObject::GO_PORTAL_OUT)
			{
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->active)
					{
						if (go2->type == GameObject::GO_WALL)
						{
							if (CheckCollision(go, go2, dt).dist > 0)
							{
								PhysicsResponse(go, go2);
								portal_shot = false;
							}
						}
					}
				}
			}
		}
	}
	
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_PORTAL_IN && !move_portal_in)
			{
				go->pos += go->vel * dt * 50;
			}
			else if (go->type == GameObject::GO_PORTAL_OUT && !move_portal_out)
			{
				go->pos += go->vel * dt * 50;
			}
		}
	}
}

void SceneArchangel::InitMap(int lvl)
{
	vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> mapInfo = CMapStorage::GetInstance()->GetMapInfo(lvl);
	vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> entityInfo = CMapStorage::GetInstance()->GetEntityInfo(lvl);
	for (int i = 0; i < mapInfo.size(); i++)
	{
		cout << "spawned wall, ";
		GameObject* go = FetchGO();
		go->active = true;
		go->type = mapInfo[i].first;
		go->pos = mapInfo[i].second[0];
		cout << go->pos << ", ";
		go->scale = mapInfo[i].second[1];
		cout << go->scale << ", ";
		go->normal = mapInfo[i].second[2];
		cout << go->normal << endl;
		go->hp = 100;
		if (go->type == GameObject::GO_WALL)
		{
			// Pillar creation
			go->pillar1 = FetchGO();
			go->pillar1->type = GameObject::GO_PILLAR;
			go->pillar1->active = true;
			go->pillar1->scale.Set(0.01f, 0.01f, 0.01f);

			go->pillar2 = FetchGO();
			go->pillar2->type = GameObject::GO_PILLAR;
			go->pillar2->active = true;
			go->pillar2->scale.Set(0.01f, 0.01f, 0.01f);

			go->pillar3 = FetchGO();
			go->pillar3->type = GameObject::GO_PILLAR;
			go->pillar3->active = true;
			go->pillar3->scale.Set(0.01f, 0.01f, 0.01f);

			go->pillar4 = FetchGO();
			go->pillar4->type = GameObject::GO_PILLAR;
			go->pillar4->active = true;
			go->pillar4->scale.Set(0.01f, 0.01f, 0.01f);
		}
	}
	for (int i = 0; i < entityInfo.size(); i++)
	{
		if (entityInfo[i].first == GameObject::GO_CUBE)
		{
			m_player->pos = entityInfo[i].second[0];
			cout << "set player pos" << endl;
		}
	}
}

void SceneArchangel::Update(double dt)
{
	// Update timers
	m_player->bullet_delay += dt;
	m_player->portal_delay += dt;

	SceneBase::Update(dt);
	SpawnBullet(dt);

	playerLogic(dt);
	portalLogic(dt);

	// Menu / Lose state
	if (state == STATE_MENU || state == STATE_LOSE)
	{
		// Space to continue
		if (Application::IsKeyPressed(VK_SPACE))
		{
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->active)
				{
					if (go->type == GameObject::GO_CIRCLE)
					{
						go->active = false;
					}
				}
			}

			InitMap(0);

			state = STATE_PLAY;
		}
	}
	// Play state
	else if (state == STATE_PLAY)
	{
		if (Application::IsKeyPressed('9'))
			m_speed = Math::Max(0.f, m_speed - 0.1f);

		//Physics Simulation Section

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_WALL)
				{
					// Set pillar positions with cube

					Vector3 N = go->normal;
					Vector3 NP(N.y, -N.x, 0);
					Vector3 right = go->normal.Cross(Vector3(0, 0, -1));

					go->pillar1->pos = go->pos + N * go->scale.x;
					go->pillar1->pos -= NP * go->scale.y;

					go->pillar2->pos = go->pos - N * go->scale.x;
					go->pillar2->pos -= NP * go->scale.y;

					go->pillar3->pos = go->pos + N * go->scale.x;
					go->pillar3->pos -= -NP * go->scale.y;

					go->pillar4->pos = go->pos + -N * go->scale.x;
					go->pillar4->pos -= -NP * go->scale.y;

				}
				if (go->type == GameObject::GO_BALL)
				{
					go->pos += go->vel * dt * m_speed;

					go->vel.y -= dt * 100;

					// Despawn ball upon leaving boundary
					if (go->pos.x + go->scale.x > m_worldWidth && go->vel.x > 0 ||
						go->pos.x - go->scale.x < 0 && go->vel.x < 0) {
						go->active = false;
					}
					if (go->pos.y + go->scale.y > m_worldHeight && go->vel.y > 0 ||
						go->pos.y - go->scale.y < 0 && go->vel.y < 0) {
						go->vel.y *= 0.7;
						go->active = false;
					}

					if ((go->pos.x > m_worldWidth + go->scale.x || go->pos.x < 0 - go->scale.x) ||
						(go->pos.y > m_worldHeight + go->scale.y || go->pos.y < 0 - go->scale.y))
					{
						ReturnGO(GameObject::GO_BALL);
						break;
					}
				}

				for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->active)
					{
						GameObject* ball = go;
						GameObject* other = go2;
						if (ball->type != GameObject::GO_BALL)
						{
							if (other->type != GameObject::GO_BALL)
								continue;
							ball = go2;
							other = go;
						}
						if (CheckCollision(ball, other, dt).dist > 0)
						{
							PhysicsResponse(ball, other);
							continue;
						}
					}
				}
			}
		}
	}
}


void SceneArchangel::RenderGO(GameObject *go)
{
	float angle;
	glDisable(GL_DEPTH_TEST);
	switch(go->type)
	{
	case GameObject::GO_BALL:
		// Render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CIRCLE:
		// Render the circle obstalce
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		// Change colors depending on health
		if (go->color == 0)
			RenderMesh(meshList[GEO_BALL], false);
		if (go->hp >= 1 && go->hp < 5)
			RenderMesh(meshList[GEO_YELLOWBALL], false);
		if (go->hp >= 5 && go->hp < 10)
			RenderMesh(meshList[GEO_ORANGEBALL], false);
		if (go->hp >= 10 && go->hp < 15)
			RenderMesh(meshList[GEO_REDBALL], false);
		if (go->hp >= 15 && go->hp < 20)
			RenderMesh(meshList[GEO_GREENBALL], false);
		if (go->hp >= 20 && go->hp < 25)
			RenderMesh(meshList[GEO_CYANBALL], false);
		if (go->hp >= 25 && go->hp < 30)
			RenderMesh(meshList[GEO_BLUEBALL], false);
		if (go->hp >= 30)
			RenderMesh(meshList[GEO_PURPLEBALL], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_GHOSTBALL:
		// Ball displayed at top
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_PILLAR:
		// Pillars that attach to cube
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		break;
		
	case GameObject::GO_WALL:
		// Cube obstacle
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	    angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);

		// Change colors depending on health
		if (go->color == 0)
			RenderMesh(meshList[GEO_CUBE], false);
		if (go->hp >= 1 && go->hp < 5)
			RenderMesh(meshList[GEO_YELLOWCUBE], false);
		if (go->hp >= 5 && go->hp < 10)
			RenderMesh(meshList[GEO_ORANGECUBE], false);
		if (go->hp >= 10 && go->hp < 15)
			RenderMesh(meshList[GEO_REDCUBE], false);
		if (go->hp >= 15 && go->hp < 20)
			RenderMesh(meshList[GEO_GREENCUBE], false);
		if (go->hp >= 20 && go->hp < 25)
			RenderMesh(meshList[GEO_CYANCUBE], false);
		if (go->hp >= 25 && go->hp < 30)
			RenderMesh(meshList[GEO_BLUECUBE], false);
		if (go->hp >= 30)
			RenderMesh(meshList[GEO_PURPLECUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CUBE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_POWERUP:
		// Powerup as ball
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALLUP], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BULLET], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_PORTAL_IN:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUEBALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_PORTAL_OUT:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ORANGEBALL], false);
		modelStack.PopMatrix();
		break;
	}
	glEnable(GL_DEPTH_TEST);
}

void SceneArchangel::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	
	RenderMesh(meshList[GEO_AXES], false);

	// Menu state background
	if (state == STATE_MENU)
	{
		modelStack.PushMatrix();
		modelStack.Translate(80, 50, 1);
		modelStack.Scale(80, 50, 1);
		RenderMesh(meshList[GEO_MENU2], false);
		modelStack.PopMatrix();
	}
	// Play state background
	else if (state == STATE_PLAY)
	{
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				RenderGO(go);
			}
		}
		//On screen in - game text

		// Display FPS
		std::ostringstream ss;
		ss << "FPS: " << fps;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);
	}
	// Lose state bg
	else if (state == STATE_LOSE)
	{
		modelStack.PushMatrix();
		modelStack.Translate(80, 50, 1);
		modelStack.Scale(80, 50, 1);
		RenderMesh(meshList[GEO_LOSE2], false);
		modelStack.PopMatrix();
	}
}

void SceneArchangel::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
