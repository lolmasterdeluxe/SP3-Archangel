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

	for (int i = 0; i < 100; i++)
	{
		GameObject* ball = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(ball);
	}

	// Initialize variables
	m_objectCount = 0;
	jump = false;
	portal_in = false;
	move_portal_in = false;
	move_portal_out = false;
	portal_shot = false;
	shotgun = false;
	left = false;
	right = true;
	time_change = false;
	weapon_dmg = 0;
	max_vel = 50;
	fire_rate = 0.2f;
	for (int i = 0; i < 5; ++i)
	{
		hitpoints[i] = 4;
	}
	heart_count = 2;
	empty_heart = 0;
	weapon_choice = 1;
	dmg_delay = 0;
	mana_delay = 0;
	time_manip = 1;

	//Calculating aspect ratio
	m_screenHeight = 60.f;
	m_screenWidth = m_screenHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	m_worldHeight = 100.f;
	m_worldWidth = 130;

	// Initialize Game state
	state = STATE_MENU;

	m_player = FetchGO();

	// Set m_player stats
	m_player->active = true;
	m_player->type = GameObject::GO_CUBE;
	m_player->hp = 12;
	m_player->mana = 50;
	m_player->gold_count = 50;
	m_player->grenade_count = 50;
	m_player->max_hp = 12;
	m_player->pos = Vector3(m_worldWidth * 0.5, 30, 0);
	m_player->normal.Set(1, 0, 0);
	m_player->scale = Vector3(2, 2, 2);
	m_player->bullet_delay = 0;

	m_ghost = FetchGO();

	// Set m_ghost stats
	m_ghost->active = true;
	m_ghost->type = GameObject::GO_GHOSTBALL;
	m_ghost->pos = m_player->pos;
	m_ghost->normal.Set(1, 0, 0);
	m_ghost->scale = Vector3(2, 2, 2);

	GameObject* newGO = FetchGO();
	newGO->active = true;
	newGO->type = GameObject::GO_POTION;
	newGO->scale.Set(1, 1, 0);
	newGO->pos = Vector3(m_worldWidth * 0.5 + 20, 10, 0);

	GameObject* newGO2 = FetchGO();
	newGO2->active = true;
	newGO2->type = GameObject::GO_MAXPOTION;
	newGO2->scale.Set(1, 1, 0);
	newGO2->pos = Vector3(m_worldWidth * 0.5 + 40, 10, 0);

	GameObject* newGO3 = FetchGO();
	newGO3->active = true;
	newGO3->type = GameObject::GO_MANAPOTION;
	newGO3->scale.Set(1, 1, 0);
	newGO3->pos = Vector3(m_worldWidth * 0.5 + 60, 10, 0);

	GameObject* newGO4 = FetchGO();
	newGO4->active = true;
	newGO4->type = GameObject::GO_GOLD;
	newGO4->scale.Set(1, 1, 0);
	newGO4->pos = Vector3(m_worldWidth * 0.5 - 10, 10, 0);

	GameObject* newGO5 = FetchGO();
	newGO5->active = true;
	newGO5->type = GameObject::GO_BARREL;
	newGO5->bullet_count = 0;
	newGO5->item_count = 0;
	newGO5->scale.Set(1.5f, 1.5f, 0);
	newGO5->pos = Vector3(m_worldWidth * 0.5 - 20, 10, 0);
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
	else if (go2->type == GameObject::GO_PILLAR || go2->type == GameObject::GO_CIRCLE || go2->type == GameObject::GO_POWERUP || go2->type == GameObject::GO_PORTAL_IN || go2->type == GameObject::GO_PORTAL_OUT || go2->type == GameObject::GO_POTION || go2->type == GameObject::GO_MAXPOTION || go2->type == GameObject::GO_MANAPOTION || go2->type == GameObject::GO_GOLD || go2->type == GameObject::GO_GRENADE || go2->type == GameObject::GO_CHEST || go2->type == GameObject::GO_BARREL)
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
		else if (go1 == m_player)
		{
			Vector3 N = collision.normal;
			Vector3 u = go1->vel;
			go1->vel = u - (2 * u.Dot(N)) * N;
			go1->vel.y *= 0;
		}
		else
		{
			Vector3 N = collision.normal;
			Vector3 u = go1->vel;
			go1->vel = u - (2 * u.Dot(N)) * N;
			go1->vel.y *= 0.4f;
			go1->vel.x *= 0.7f;
		}
	}
	else if (collision.go->type == GameObject::GO_PILLAR || collision.go->type == GameObject::GO_CIRCLE)
	{
		Vector3 N = (collision.go->pos - go1->pos).Normalized();
		Vector3 u = go1->vel;
		go1->vel = u - (2 * u).Dot(N) * N;
		go1->vel.y *= 0.4;
	}
	if (go1 == m_player)
	{
		if (collision.go->type == GameObject::GO_POTION && m_player->hp < m_player->max_hp)
		{
			heal(false);
			collision.go->active = false;
		}
		if (collision.go->type == GameObject::GO_MAXPOTION)
		{
			heal(true);
			collision.go->active = false;
		}
		if (collision.go->type == GameObject::GO_MANAPOTION && go1->mana > 1 && go1->mana < 50)
		{
			mana(0, 10, true);
			collision.go->active = false;
		}
		if (collision.go->type == GameObject::GO_GOLD)
		{
			if (collision.go->vel.x >= -0.1f && collision.go->vel.x <= 0.1f)
			{
				m_player->gold_count++;
				collision.go->active = false;
			}
		}
		if (collision.go->type == GameObject::GO_CHEST)
		{
			openChest(collision.go);
		}
	}
	if (go1->type == GameObject::GO_BULLET)
	{
		if (collision.go->type == GameObject::GO_BARREL)
		{
			collision.go->bullet_count++;
			go1->active = false;
			if (collision.go->bullet_count >= 3)
			{
				openChest(collision.go);
				if (collision.go->item_count >= 3)
					collision.go->active = false;
			}
		}

	}
	if (!portal_shot)
	{
		if (go1 == m_player || go1->type == GameObject::GO_BULLET || go1->type == GameObject::GO_GRENADE)
		{
			if (collision.go->type == GameObject::GO_PORTAL_IN)
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
			else if (collision.go->type == GameObject::GO_PORTAL_OUT)
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

void SceneArchangel::Gravity(GameObject::GAMEOBJECT_TYPE GO, float elasticity, double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				go->pos += go->vel * dt * m_speed * time_manip;
				go->vel.y -= dt * elasticity;
			}
			// terminal velocity
			if (go->vel.y <= -200)
			{
				go->vel.y = -200;
			}
		}

	}
}

void SceneArchangel::SpawnBullet(double dt)
{
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	double x, y;
	Application::GetCursorPos(&x, &y);
	//Mouse Section
	static bool bLButtonState = false;
	float angle;
	if (Application::IsMousePressed(0))
	{
		if (m_player->bullet_delay > fire_rate / time_manip)
		{
			if (!shotgun)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_BULLET;
				newGO->scale.Set(1, 0.5f, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO->pos.y, 0);
				newGO->vel.Normalize() * 100;
			}
			else
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_BULLET;
				newGO->scale.Set(1, 0.5f, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO->pos.y, 0);
				angle = atan2f(newGO->vel.y, newGO->vel.x) + Math::DegreeToRadian(1);
				newGO->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO->vel.Normalize() * 100;

				GameObject* newGO2 = FetchGO();
				newGO2->active = true;
				newGO2->type = GameObject::GO_BULLET;
				newGO2->scale.Set(1, 0.5f, 0);
				newGO2->pos = m_player->pos;
				newGO2->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO2->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO2->pos.y, 0);
				angle = atan2f(newGO2->vel.y, newGO2->vel.x) - Math::DegreeToRadian(1);
				newGO2->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO2->vel.Normalize() * 100;

				GameObject* newGO3 = FetchGO();
				newGO3->active = true;
				newGO3->type = GameObject::GO_BULLET;
				newGO3->scale.Set(1, 0.5f, 0);
				newGO3->pos = m_player->pos;
				newGO3->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO3->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO3->pos.y, 0);
				angle = atan2f(newGO3->vel.y, newGO3->vel.x) + Math::DegreeToRadian(3);
				newGO3->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO3->vel.Normalize() * 100;

				GameObject* newGO4 = FetchGO();
				newGO4->active = true;
				newGO4->type = GameObject::GO_BULLET;
				newGO4->scale.Set(1, 0.5f, 0);
				newGO4->pos = m_player->pos;
				newGO4->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO4->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO4->pos.y, 0);
				angle = atan2f(newGO4->vel.y, newGO4->vel.x) - Math::DegreeToRadian(3);
				newGO4->vel = Vector3(cosf(angle), sin(angle), 0);
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
				go->pos += go->vel * dt * 100 * time_manip;
				Boundary(go, 2);
			}
		}
	}

	enableCollision(dt, GameObject::GO_BULLET);
}

void SceneArchangel::throwGrenade(double dt)
{
	static bool bLButtonState = false;
	if (Application::IsKeyPressed('L') && !bLButtonState && m_player->grenade_count > 0)
	{
		bLButtonState = true;
		GameObject* newGO = FetchGO();
		newGO->active = true;
		newGO->type = GameObject::GO_GRENADE;
		newGO->scale.Set(1.f, 1.f, 0);
		newGO->pos = m_player->pos;
		if (right)
		{
			if (!time_change)
				newGO->vel = Vector3(40, 60, 0);
			else
				newGO->vel = Vector3(60, 80, 0);
			cout << "right true" << endl;
		}
		else if (left)
		{
			if (!time_change)
				newGO->vel = Vector3(-40, 60, 0);
			else
				newGO->vel = Vector3(-60, 80, 0);
			cout << "left true" << endl;
		}
		cout << newGO->vel.x << endl;
		newGO->grenade_delay = 0;
		m_player->grenade_count--;
	}
	else if (!Application::IsKeyPressed('L'))
	{
		bLButtonState = false;
	}
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_GRENADE)
			{
				Boundary(go, 2);
				go->grenade_delay += dt;
				if (go->grenade_delay > 2)
				{
					go->active = false;
				}
			}
		}
	}
}

void SceneArchangel::playerLogic(double dt)
{
	Gravity(GameObject::GO_CUBE, 300, dt);

	// Setting speed limiters
	if (m_player->vel.x >= max_vel)
	{
		m_player->vel.x = max_vel;
	}
	if (m_player->vel.x <= -max_vel)
	{
		m_player->vel.x = -max_vel;
	}

	// Jump
	if (Application::IsKeyPressed(VK_SPACE) && !jump)
	{
		m_player->vel.y += 30;
		jump = true;
	}

	// Movement
	if (Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
	{
		m_player->vel.y += 3;
	}
	if (Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
	{
		m_player->vel.y -= 3;
	}
	if (Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
	{
		if (!jump)
			m_player->vel.x -= 3;
		else
			m_player->vel.x -= 1;
		left = true;
		right = false;
	}
	else if (Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
	{
		if (!jump)
			m_player->vel.x += 3;
		else
			m_player->vel.x += 1;
		right = true;
		left = false;
	}
	else
	{
		m_player->vel.x *= 0.7;
	}
	Boundary(m_player, 1);
		
	m_ghost->pos.x = m_player->pos.x;
	m_ghost->pos.y = (m_player->pos.y - 2);
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_WALL)
			{
				Collision collision = CheckCollision(m_ghost, go, dt);
				if (collision.dist > 0)
				{
					jump = false;
				}
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
				newGO->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO->pos.y, 0);
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
				newGO->vel = Vector3((x / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f) - newGO->pos.x, ((h - y) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f) - newGO->pos.y, 0);
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
				go->pos += go->vel * dt * 50 * time_manip;
			}
			else if (go->type == GameObject::GO_PORTAL_OUT && !move_portal_out)
			{
				go->pos += go->vel * dt * 50 * time_manip;
			}
		}
	}
}

void SceneArchangel::itemLogic(double dt)
{
	Gravity(GameObject::GO_POTION, 300, dt);
	Gravity(GameObject::GO_MAXPOTION, 300, dt);
	Gravity(GameObject::GO_MANAPOTION, 300, dt);
	Gravity(GameObject::GO_GRENADE, 200, dt);
	Gravity(GameObject::GO_GOLD, 300, dt);
	Gravity(GameObject::GO_CHEST, 300, dt);
	Gravity(GameObject::GO_BARREL, 300, dt);
	enableCollision(dt, GameObject::GO_POTION);
	enableCollision(dt, GameObject::GO_MAXPOTION);
	enableCollision(dt, GameObject::GO_MANAPOTION);
	enableCollision(dt, GameObject::GO_GRENADE);
	enableCollision(dt, GameObject::GO_GOLD);
	enableCollision(dt, GameObject::GO_CHEST);
	enableCollision(dt, GameObject::GO_BARREL);
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
						if (go == m_player && go2->type == GameObject::GO_WALL || go != m_player && go2->type == GameObject::GO_WALL)
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
	// AK47
	if (Application::IsKeyPressed('1'))
	{
		setGun(0.2f, 6);
		shotgun = false;
		weapon_choice = 1;
	}
	// SMG
	else if (Application::IsKeyPressed('2'))
	{
		setGun(0.1f, 5);
		shotgun = false;
		weapon_choice = 2;
	}
	// LMG
	else if (Application::IsKeyPressed('3'))
	{
		setGun(0.3f, 7);
		shotgun = false;
		weapon_choice = 3;
	}
	// SHOTGUN
	else if (Application::IsKeyPressed('4'))
	{
		setGun(0.6f, 5);
		shotgun = true;
		weapon_choice = 4;
	}
	// REVOLVER
	else if (Application::IsKeyPressed('5'))
	{
		setGun(0.4f, 8);
		shotgun = false;
		weapon_choice = 5;
	}

}

void SceneArchangel::takeDMG()
{
	if (dmg_delay > 3)
	{
		m_player->hp--;
		if (hitpoints[heart_count - empty_heart] > 1)
			hitpoints[heart_count - empty_heart]--;

		if (hitpoints[heart_count - empty_heart] <= 1)
			empty_heart++;

		dmg_delay = 0;
	/*	cout << "Empty heart count: " << empty_heart << endl;
		cout << "Current hitpoints: " << hitpoints[heart_count - empty_heart] << endl;
		cout << "m_player->hp: " << m_player->hp << endl;*/
	}
}

void SceneArchangel::heal(bool max_potion)
{
	cout << endl;
	if (max_potion)
	{
		heart_count++;
		hitpoints[heart_count - empty_heart] = hitpoints[heart_count - empty_heart - 1];
		hitpoints[heart_count - empty_heart - 1] = 4;
		for (int i = 1; i <= empty_heart; ++i)
		{
			hitpoints[heart_count - empty_heart + i] = 1;
			//cout << "configured health" << endl;
		}
		m_player->max_hp += 4;
		m_player->hp += 4;
		/*cout << "heal max" << endl;
		cout << "Hitpoints before: " << hitpoints[heart_count - empty_heart - 1] << endl;*/
	}
	else
	{
		// Healing if heart is full
		if (hitpoints[heart_count - empty_heart] == 4 && empty_heart > 0)
		{
			empty_heart--;
			m_player->hp++;
			hitpoints[heart_count - empty_heart]++;
			//cout << "heal over" << endl;
		}
		// Increasing max health

		else if (hitpoints[heart_count - empty_heart] < 4)
		{
			hitpoints[heart_count - empty_heart]++;
			m_player->hp++;
			//cout << "heal normal" << endl;
		}
	}
	
	if (m_player->hp >= m_player->max_hp)
		m_player->hp = m_player->max_hp;

	if (empty_heart < 0)
	{
		empty_heart = 0;
	}
	/*cout << "Empty heart count: " << empty_heart << endl;
	cout << "Current hitpoints: " << hitpoints[heart_count - empty_heart] << endl;
	cout << "m_player->hp: " << m_player->hp << endl;*/
}

void SceneArchangel::mana(float interval, int amount,  bool restore)
{
	if (mana_delay > interval && m_player->mana > 0 && m_player->mana < 50)
	{
		if (restore)
			m_player->mana += amount;
		else
			m_player->mana -= amount;

		mana_delay = 0;
	}
}

void SceneArchangel::openChest(GameObject* go)
{
	for (int i = 0; i < 3; ++i)
	{
		if (go->item_count < 3)
		{
			GameObject* newGO = FetchGO();
			int item_type = Math::RandIntMinMax(1, 3);
			int dir = Math::RandIntMinMax(1, 2);
			int positive_vel = Math::RandIntMinMax(10, 40);
			int negative_vel = Math::RandIntMinMax(-40, -10);
			if (item_type == 1)
			{
				newGO->active = true;
				newGO->type = GameObject::GO_POTION;
				newGO->scale.Set(1, 1, 1);
				if (dir == 1)
				{
					newGO->vel = Vector3(positive_vel, positive_vel + 20, 0);
				}
				else if (dir == 2)
				{
					newGO->vel = Vector3(negative_vel, positive_vel + 20, 0);
				}
				newGO->pos = go->pos;
			}
			else if (item_type == 2)
			{
				newGO->active = true;
				newGO->type = GameObject::GO_MANAPOTION;
				newGO->scale.Set(1, 1, 1);
				if (dir == 1)
				{
					newGO->vel = Vector3(positive_vel, positive_vel + 20, 0);
				}
				else if (dir == 2)
				{
					newGO->vel = Vector3(negative_vel, positive_vel + 20, 0);
				}
				newGO->pos = go->pos;
			}
			else if (item_type == 3)
			{
				newGO->active = true;
				newGO->type = GameObject::GO_GOLD;
				newGO->scale.Set(1, 1, 1);
				if (dir == 1)
				{
					newGO->vel = Vector3(positive_vel, positive_vel + 20, 0);
				}
				else if (dir == 2)
				{
					newGO->vel = Vector3(negative_vel, positive_vel + 20, 0);
				}
				newGO->pos = go->pos;
			}
			go->item_count++;
			cout << "count: " << go->item_count << endl;
			cout << "type: " << item_type << endl;
			cout << "dir: " << dir << endl;
		}
	}
}

void SceneArchangel::manipTime(double dt)
{
	static bool bLButtonState3 = false;
	if (Application::IsKeyPressed('P') && !bLButtonState3)
	{
		bLButtonState3 = true;
		if (!time_change)
		{
			time_change = true;
			time_manip = 0.5f;
		}
		else
		{
			time_change = false;
			time_manip = 1.f;
		}
	}
	else if (!Application::IsKeyPressed('P'))
	{
		bLButtonState3 = false;
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
	m_screenHeight = 60.f;
	m_screenWidth = m_screenHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	m_player->bullet_delay += dt;
	m_player->portal_delay += dt;
	dmg_delay += dt;
	mana_delay += dt;
	
	SceneBase::Update(dt);

	cameraPos.Set(m_screenWidth * .5f, m_screenHeight * .5f);

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
		//Camera Position Setting
		// Clamp screen space if reached end of world space
		float clamp_pos_x = Math::Clamp((m_player->pos.x), m_screenWidth * .5f, m_worldWidth - m_screenWidth * .5f);
		float clamp_pos_y = Math::Clamp((m_player->pos.y), m_screenHeight * .5f, m_worldHeight - m_screenHeight * .5f);
		cameraPos.Set(clamp_pos_x, clamp_pos_y);

		SpawnBullet(dt);
		playerLogic(dt);
		portalLogic(dt);
		pickWeapon(dt);
		itemLogic(dt);
		throwGrenade(dt);
		manipTime(dt);

		static bool bLButtonState = false;
		static bool bLButtonState2 = false;
		if (Application::IsKeyPressed('F') && !bLButtonState)
		{
			bLButtonState = true;
			heal(false);
		}
		else if (!Application::IsKeyPressed('F'))
		{
			bLButtonState = false;
		}
		if (Application::IsKeyPressed('G') && !bLButtonState2)
		{
			bLButtonState2 = true;
			heal(true);
		}
		else if (!Application::IsKeyPressed('G'))
		{
			bLButtonState2 = false;
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

	case GameObject::GO_POTION:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_REDBALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_MAXPOTION:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ORANGEBALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_MANAPOTION:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUEBALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_GRENADE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_GREENBALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_GOLD:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_YELLOWBALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CHEST:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_YELLOWCUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BARREL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ORANGECUBE], false);
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
	if (state == STATE_PLAY)
	{
		projection.SetToOrtho(cameraPos.x - m_screenWidth * .5f, cameraPos.x + m_screenWidth * .5f, cameraPos.y - m_screenHeight * .5f, cameraPos.y + m_screenHeight * .5f, -10, 10);
	}
	else
	{
		projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	}
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
		std::ostringstream ss2;
		std::ostringstream ss3;
		//ss << "FPS: " << fps;
		//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);

		/*ss << m_player->pos;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);*/

		RenderMeshOnScreen(meshList[GEO_CHARGE], -6.f + (m_player->mana) * 0.24f, 53, 12, 2.5f);

		RenderMeshOnScreen(meshList[GEO_GREENBALL], 9, 2.5f, 1, 1.2f);
		ss2 << "x" << m_player->grenade_count;
		RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 1), 2.5f, 10.5f, 1);

		RenderMeshOnScreen(meshList[GEO_YELLOWBALL], 15, 2.5f, 1, 1.2f);
		ss3 << "x" << m_player->gold_count;
		RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(1, 1, 1), 2.5f, 16.5f, 1);

		if (weapon_choice == 1)
		{
			RenderMeshOnScreen(meshList[GEO_ORANGEBALL], 4, 5, 2.5f, 3);
		}
		else if (weapon_choice == 2)
		{
			RenderMeshOnScreen(meshList[GEO_YELLOWBALL], 4, 5, 2.5f, 3);
		}
		else if (weapon_choice == 3)
		{
			RenderMeshOnScreen(meshList[GEO_REDBALL], 4, 5, 2.5f, 3);
		}
		else if (weapon_choice == 4)
		{
			RenderMeshOnScreen(meshList[GEO_BLUEBALL], 4, 5, 2.5f, 3);
		}
		else if (weapon_choice == 5)
		{
			RenderMeshOnScreen(meshList[GEO_PURPLEBALL], 4, 5, 2.5f, 3);
		}
		for (int i = 0; i <= heart_count; ++i)
		{
			if (i >= (heart_count - empty_heart))
			{
				if (hitpoints[i] == 4)
				{
					RenderMeshOnScreen(meshList[GEO_FULLHEART], 2 + i * 4, 57, 1.7f, 1.7f);
				}
				else if (hitpoints[i] == 3)
				{
					RenderMeshOnScreen(meshList[GEO_80HEART], 2 + i * 4, 57, 1.7f, 1.7f);
				}
				else if (hitpoints[i] == 2)
				{
					RenderMeshOnScreen(meshList[GEO_20HEART], 2 + i * 4, 57, 1.7f, 1.7f);
				}
				else if (hitpoints[i] <= 1)
				{
					RenderMeshOnScreen(meshList[GEO_EMPTYHEART], 2 + i * 4, 57, 1.7f, 1.7f);
				}
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_FULLHEART], 2 + i * 4, 57, 1.7f, 1.7f);
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
