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
	shotgun = false;
	weapon_dmg = 0;
	max_vel = 50;
	fire_rate = 0.2f;
	for (int i = 0; i < 5; ++i)
	{
		hitpoints[i] = 4;
	}
	heart_count = 2;
	empty_heart = 0;
	dmg_delay = 0;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Initialize Game state
	state = STATE_MENU;

	m_player = FetchGO();

	// Set m_player stats
	m_player->active = true;
	m_player->type = GameObject::GO_CUBE;
	m_player->hp = 12;
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
	else if (go2->type == GameObject::GO_WALL)
	{
		Vector3 N = go2->normal; // go2 normal
		Vector3 NP(N.y, -N.x, 0); //  go2 right vector
		Vector3 w0_b1 = go2->pos - go1->pos; // point from go1 to go2
		float r = go1->scale.x; // go1 radius
		float h_2 = go2->scale.x; // go2 length
		float l_2 = go2->scale.y; // go2 width

		if (w0_b1.Dot(N) < 0)
			N = -N; //making sure N is outward normal
		if (w0_b1.Dot(NP) < 0)
			NP = -NP; //making sure NP is outward normal

		float dist_N = w0_b1.Dot(N); // dist along N axis
		float dist_NP = w0_b1.Dot(NP); // dist along NP axis

		// Scenario 1: object is in the length side of the Box
		// ball is at least touching and the center of ball must be within the length and it cannot travel opposite to outward normal
		if (dist_N <= r + h_2 && dist_NP <= l_2 && go1->vel.Dot(N) >= 0)
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -N.Normalize();
			collision.dist = r + h_2 - (w0_b1).Dot(N);
			collision.normal = N;
			return collision;
		}
		// Scenario 2: object is in the width side of the Box
		else if (dist_NP <= r + l_2 && dist_N <= h_2 && go1->vel.Dot(NP) >= 0)
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -NP.Normalize();
			collision.dist = r + l_2 - (w0_b1).Dot(NP);
			collision.normal = NP;
			return collision;
		}
		// Scenario 3: object is in the corner of the box
		// center of ball is not within the length and width but is touching the box and velocity cannot be going away from any of the normal
		else if (dist_NP > l_2 && dist_NP <= r + l_2 && dist_N > h_2 && dist_N <= r + h_2 && (go1->vel.Dot(NP) >= 0 || go1->vel.Dot(N) >= 0))
		{
			Collision collision;
			collision.go = go2;
			collision.axis = go1->pos - (go2->pos - N * h_2 - NP * l_2);
			collision.dist = r - collision.axis.Length();
			collision.normal = collision.axis.Normalize();
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

void SceneArchangel::PhysicsResponse(GameObject* go1, Collision collision)
{
	if (collision.go->type == GameObject::GO_BALL)
	{
		u1 = go1->vel;
		m1 = go1->mass;
		u2 = collision.go->vel;
		m2 = collision.go->mass;
		pi = go1->mass * u1 + collision.go->mass * u2;
		go1->vel = u1 * ((m1 - m2) / (m1 + m2)) + u2 * ((2 * m2) / (m1 + m2));
		collision.go->vel = u1 * ((2 * m1) / (m1 + m2)) + u2 * ((m2 - m1) / (m1 + m2));
		v1 = go1->vel;
		v2 = collision.go->vel;
		go1->vel = u1 - (2 * m2 / (m1 + m2)) * (((u1 - u2).Dot(go1->pos - collision.go->pos)) / (go1->pos - collision.go->pos).LengthSquared()) * (go1->pos - collision.go->pos);
		collision.go->vel = u2 - (2 * m1 / (m1 + m2)) * (((u2 - u1).Dot(collision.go->pos - go1->pos)) / (collision.go->pos - go1->pos).LengthSquared()) * (collision.go->pos - go1->pos);
		go1->vel.y *= 0.4;
	}
	else if (collision.go->type == GameObject::GO_WALL)
	{
		if (go1->type == GameObject::GO_BULLET)
		{
			go1->active = false;
		}
		if (go1 == m_player)
		{
			Vector3 N = collision.normal;
			Vector3 u = go1->vel;
			go1->vel = u - (2 * u.Dot(N)) * N;
			go1->vel.y *= 0;
			if (go1->vel.y == 0)
				jump = false;
		}
	}
	else if (collision.go->type == GameObject::GO_PILLAR || collision.go->type == GameObject::GO_CIRCLE)
	{
		Vector3 N = (collision.go->pos - go1->pos).Normalized();
		Vector3 u = go1->vel;
		go1->vel = u - (2 * u).Dot(N) * N;
		go1->vel.y *= 0.4;
	}
	if (!portal_shot)
	{
		if (collision.go->type == GameObject::GO_PORTAL_IN)
		{
			if (go1 == m_player || go1->type == GameObject::GO_BULLET)
			{
				for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject* go = (GameObject*)*it;
					if (go->active)
					{
						if (go->type == GameObject::GO_PORTAL_OUT)
						{
							go1->pos = go->pos;
						}
					}
				}
			}
		}
		else if (collision.go->type == GameObject::GO_PORTAL_OUT)
		{
			if (go1 == m_player || go1->type == GameObject::GO_BULLET)
			{
				for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject* go = (GameObject*)*it;
					if (go->active)
					{
						if (go->type == GameObject::GO_PORTAL_IN)
						{
							go1->pos = go->pos;
						}
					}
				}
			}
		}
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
		if (m_player->bullet_delay > fire_rate)
		{
			if (!shotgun)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_BULLET;
				newGO->scale.Set(1, 0.5f, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3((x / w * m_worldWidth) - newGO->pos.x, ((h - y) / h * m_worldHeight) - newGO->pos.y, 0);
				newGO->vel.Normalize() * 100;
			}
			else
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_BULLET;
				newGO->scale.Set(1, 0.5f, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3((x / w * m_worldWidth) - newGO->pos.x, ((h - y) / h * m_worldHeight) - newGO->pos.y + 1.f, 0);
				newGO->vel.Normalize() * 100;

				GameObject* newGO2 = FetchGO();
				newGO2->active = true;
				newGO2->type = GameObject::GO_BULLET;
				newGO2->scale.Set(1, 0.5f, 0);
				newGO2->pos = m_player->pos;
				newGO2->vel = Vector3((x / w * m_worldWidth) - newGO2->pos.x, ((h - y) / h * m_worldHeight) - newGO2->pos.y - 1.f, 0);
				newGO2->vel.Normalize() * 100;

				GameObject* newGO3 = FetchGO();
				newGO3->active = true;
				newGO3->type = GameObject::GO_BULLET;
				newGO3->scale.Set(1, 0.5f, 0);
				newGO3->pos = m_player->pos;
				newGO3->vel = Vector3((x / w * m_worldWidth) - newGO3->pos.x, ((h - y) / h * m_worldHeight) - newGO3->pos.y + 3.f, 0).Normalize() * 100;
				newGO3->vel.Normalize() * 100;

				GameObject* newGO4 = FetchGO();
				newGO4->active = true;
				newGO4->type = GameObject::GO_BULLET;
				newGO4->scale.Set(1, 0.5f, 0);
				newGO4->pos = m_player->pos;
				newGO4->vel = Vector3((x / w * m_worldWidth) - newGO4->pos.x, ((h - y) / h * m_worldHeight) - newGO4->pos.y - 3.f, 0).Normalize() * 100;
				newGO4->vel.Normalize() * 100;
			}
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
				Boundary(go, 2);
			}
		}
	}
	enableCollision(dt, GameObject::GO_BULLET);
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
				if (go->vel.y <= -200)
				{
					go->vel.y = -200;
				}

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
				Boundary(go, 1);
			}
		}
	}
	enableCollision(dt, GameObject::GO_CUBE);
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
		portal_shot = true;
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
							Collision collision = CheckCollision(go, go2, dt);
							if (collision.dist > 0)
							{
								CollisionBound(go, collision);
								activatePortal(go);
							}
						}
					}
				}
				Boundary(go, 1);
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

void SceneArchangel::activatePortal(GameObject* go)
{
	if (go->active)
	{
		if (go->type == GameObject::GO_PORTAL_IN)
		{
			go->vel = 0;
			portal_in = true;
			move_portal_in = true;
			go->scale.Set(2, 2.5f, 1);
		}
		else if (go->type == GameObject::GO_PORTAL_OUT)
		{
			go->vel = 0;
			portal_in = false;
			move_portal_out = true;
			go->scale.Set(2, 2.5f, 1);
		}
		portal_shot = false;
	}
}

void SceneArchangel::enableCollision(double dt, GameObject::GAMEOBJECT_TYPE GO)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;
				if (go2->active)
				{
					GameObject* first = go;
					GameObject* other = go2;
					if (first->type != GO)
					{
						if (other->type != GO)
							continue;
						first = go2;
						other = go;
					}
					Collision collision = CheckCollision(first, other, dt);
					if (collision.dist > 0)
					{
						if (go2->type != GameObject::GO_PORTAL_IN && go2->type != GameObject::GO_PORTAL_OUT && go == m_player)
						{
							CollisionBound(first, collision);
						}
						PhysicsResponse(first, collision);
						continue;
					}
				}
			}
		}
	}
}


void SceneArchangel::Boundary(GameObject* go, int choice)
{
	// Wrap around for GO
	if (choice == 1)
	{
		// Out of bounds checking
		if (go->pos.x + go->scale.x > m_worldWidth && go->vel.x > 0 ||
			go->pos.x - go->scale.x < 0 && go->vel.x < 0) {
			go->vel.x = 0;
			activatePortal(go);
		}
		if (go->pos.y + go->scale.y > m_worldHeight && go->vel.y > 0 ||
			go->pos.y - go->scale.y < 0 && go->vel.y < 0) {
			go->vel.y = 0;
			activatePortal(go);
		}
					
	}
	if (choice == 2)
	{
		if ((go->pos.x > m_worldWidth + go->scale.x || go->pos.x < 0 - go->scale.x) ||
			(go->pos.y > m_worldHeight + go->scale.y || go->pos.y < 0 - go->scale.y))
		{
			go->active = false;
		}
	}
}

void SceneArchangel::setGun(float fire, int dmg)
{
	fire_rate = fire;
	weapon_dmg = dmg;
}

void SceneArchangel::pickWeapon(double dt)
{
	if (Application::IsKeyPressed('1'))
	{
		setGun(0.2f, 6);
		shotgun = false;
	}
	else if (Application::IsKeyPressed('2'))
	{
		setGun(0.1f, 5);
		shotgun = false;
	}
	else if (Application::IsKeyPressed('3'))
	{
		setGun(0.3f, 7);
		shotgun = false;
	}
	else if (Application::IsKeyPressed('4'))
	{
		setGun(0.6f, 5);
		shotgun = true;
	}
	else if (Application::IsKeyPressed('5'))
	{
		setGun(0.4f, 8);
		shotgun = false;
	}

}

void SceneArchangel::takeDMG()
{
	if (dmg_delay > 3)
	{
		m_player->hp--;
		hitpoints[heart_count - empty_heart]--;
		if (hitpoints[heart_count - empty_heart] <= 1)
		{
			empty_heart++;
		}
		dmg_delay = 0;
	}
}

void SceneArchangel::heal()
{
	m_player->hp++;
	if (hitpoints[heart_count - empty_heart] == 4)
	{
		empty_heart--;
	}
	hitpoints[heart_count - empty_heart]++;
	if (empty_heart < 0)
	{
		heart_count++;
		empty_heart = 0;
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
	dmg_delay += dt;

	SceneBase::Update(dt);


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
		SpawnBullet(dt);
		playerLogic(dt);
		portalLogic(dt);
		pickWeapon(dt);
		takeDMG();
		static bool bLButtonState = false;
		if (Application::IsKeyPressed('F') && !bLButtonState)
		{
			bLButtonState = true;
			heal();
		}
		else if (!Application::IsKeyPressed('F'))
		{
			bLButtonState = false;
		}

		if (Application::IsKeyPressed('9'))
			m_speed = Math::Max(0.f, m_speed - 0.1f);
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
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);

		ss << m_player->pos;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);

		for (int i = 0; i <= heart_count; ++i)
		{
			if (i >= (heart_count - empty_heart))
			{
				modelStack.PushMatrix();
				modelStack.Translate(5 + i * 8, 95, 1);
				modelStack.Scale(3, 3, 1);
				if (hitpoints[i] == 4)
				{
					RenderMesh(meshList[GEO_FULLHEART], false);
				}
				else if (hitpoints[i] == 3)
				{
					RenderMesh(meshList[GEO_80HEART], false);
				}
				else if (hitpoints[i] == 2)
				{
					RenderMesh(meshList[GEO_20HEART], false);
				}
				else if (hitpoints[i] <= 1)
				{
					RenderMesh(meshList[GEO_EMPTYHEART], false);
				}
				modelStack.PopMatrix();
			}
			else
			{
				modelStack.PushMatrix();
				modelStack.Translate(5 + i * 8, 95, 1);
				modelStack.Scale(3, 3, 1);
				RenderMesh(meshList[GEO_FULLHEART], false);
				modelStack.PopMatrix();
			}
		}

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
