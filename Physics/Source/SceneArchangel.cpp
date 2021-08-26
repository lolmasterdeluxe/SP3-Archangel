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
	time_change = false;
	phase = false;
	weapon_dmg = 6;
	max_vel = 50;
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

	m_AttemptLeft = m_AttemptRight = false;

	m_toggleDebugScreen = false;

	//Calculating aspect ratio
	m_screenHeight = 60;
	m_screenWidth = m_screenHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	m_worldHeight = 100.f;
	m_worldWidth = 180;

	// Initialize Game state
	state = STATE_MENU;


	for (int i = 0; i < 150; i++)
	{
		GameObject* asteroid = new GameObject(GameObject::GO_ASTEROID);
		m_goList.push_back(asteroid);
	}

	m_emptyGO = new GameObject(); // do not push this to vector list
	m_emptyGO->type = GameObject::GO_GHOSTBALL;

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
	m_player->scale = Vector3(2, 3.f, 2);
	m_player->bullet_delay = 0;
	m_player->fire_rate = 0.2f;
	m_player->attack = false;

	m_player->under_box = FetchGO();
	m_player->under_box->active = true;
	m_player->under_box->type = GameObject::GO_GHOSTBALL;
	m_player->under_box->pos = m_player->pos;
	m_player->under_box->normal.Set(1, 0, 0);
	m_player->under_box->scale = Vector3(1, 1, 1);
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

void SceneArchangel::ReturnGO(GameObject* go)
{
	if (go->active)
	{
		m_objectCount--;
		go->active = false;
		go->pos.SetZero();
		go->vel.SetZero();
		go->scale.Set(1, 1, 1);
		go->normal.Set(1, 0);
	}
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
				ReturnGO(go);
			}
		}
	}
}

GameObject* SceneArchangel::FindGameObjectWithType(GameObject::GAMEOBJECT_TYPE type)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == type)
			{
				return go;
			}
		}
	}
	return nullptr;
}

Collision SceneArchangel::CheckCollision(GameObject* go1, GameObject* go2)
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
	else if (go2->type == GameObject::GO_BOUNCEPLATFORM)
	{
		if (go1->vel.y > 0) return Collision();
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

		if (dist_N <= r + h_2 && dist_NP <= r + l_2 && go1->vel.Dot(N) >= 0)
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -N.Normalize();
			collision.dist = r + h_2 - (w0_b1).Dot(N);
			collision.normal = N;
			return collision;
		}
	}
	else if (go2->type == GameObject::GO_PLATFORM)
	{
		if (go1 == m_player && (Application::IsKeyPressed('S') || go1->vel.y > 0))
			return Collision();
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

		if (dist_N <= r + h_2 && dist_NP <= r + l_2 && go1->vel.Dot(N) >= 0)
		{
			Collision collision;
			collision.go = go2;
			collision.axis = -N.Normalize();
			collision.dist = r + h_2 - (w0_b1).Dot(N);
			collision.normal = N;
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
	else
	{ //static ball and non static ball collision
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
	else if (collision.go->type == GameObject::GO_WALL || collision.go->type == GameObject::GO_PLATFORM)
	{
		if (go1 == m_player && !phase)
		{
			Vector3 N = collision.normal;
			Vector3 u = go1->vel;
			go1->vel = u - (2 * u.Dot(N)) * N;
			go1->vel.y *= 0;
		}
		else if (collision.go->type == GameObject::GO_WALL && (go1->type == GameObject::GO_BULLET || go1->type == GameObject::GO_FIREBALL || go1->type == GameObject::GO_ENEMY_BULLET))
		{
			ReturnGO(go1);
		}
		else if (go1->type != GameObject::GO_BULLET && go1->type != GameObject::GO_FIREBALL && go1->type != GameObject::GO_ENEMY_BULLET || go1->type == GameObject::GO_DEMON || go1->type == GameObject::GO_FALLENANGEL || go1->type == GameObject::GO_TERMINATOR || go1->type == GameObject::GO_SOLDIER)
		{
			Vector3 N = collision.normal;
			Vector3 u = go1->vel;
			go1->vel = u - (2 * u.Dot(N)) * N;
			go1->vel.y *= 0.4f;
			go1->vel.x *= 0.7f;
		}
	}
	else if (collision.go->type == GameObject::GO_BOUNCEPLATFORM)
	{
		if ((go1 == m_player && !phase) || go1->type == GameObject::GO_DEMON || go1->type == GameObject::GO_TERMINATOR || go1->type == GameObject::GO_SOLDIER)
		{
			if (go1->vel.y <= 0)
				go1->vel.y = 130;
		}
	}
	else if (collision.go->type == GameObject::GO_PILLAR || collision.go->type == GameObject::GO_CIRCLE)
	{
		Vector3 N = (collision.go->pos - go1->pos).Normalized();
		Vector3 u = go1->vel;
		go1->vel = u - (2 * u).Dot(N) * N;
		go1->vel.y *= 0.4;
	}
	if (go1 == m_player && !phase)
	{
		if (collision.go->type == GameObject::GO_POTION && m_player->hp < m_player->max_hp)
		{
			heal(false);
			ReturnGO(collision.go);
		}
		if (collision.go->type == GameObject::GO_MAXPOTION)
		{
			heal(true);
			ReturnGO(collision.go);
		}
		if (collision.go->type == GameObject::GO_MANAPOTION && go1->mana > 1 && go1->mana < 50)
		{
			mana(0, 10, true);
			ReturnGO(collision.go);
		}
		if (collision.go->type == GameObject::GO_GOLD)
		{
			if (collision.go->vel.x >= -0.1f && collision.go->vel.x <= 0.1f)
			{
				m_player->gold_count++;
				ReturnGO(collision.go);
			}
		}
		if (collision.go->type == GameObject::GO_CHEST)
		{
			openChest(collision.go);
		}
		if (collision.go->type == GameObject::GO_DEMON || collision.go->type == GameObject::GO_FALLENANGEL || collision.go->type == GameObject::GO_TERMINATOR || go1->type == GameObject::GO_SOLDIER)
		{
			takeDMG();
		}
		if (collision.go->type == GameObject::GO_FIREBALL || collision.go->type == GameObject::GO_ENEMY_BULLET)
		{
			takeDMG();
			ReturnGO(collision.go);
		}
	}
	if (go1->type == GameObject::GO_BULLET)
	{
		if (collision.go->type == GameObject::GO_BARREL || collision.go->type == GameObject::GO_DEMON || collision.go->type == GameObject::GO_FALLENANGEL || collision.go->type == GameObject::GO_TERMINATOR || collision.go->type == GameObject::GO_SOLDIER)
		{
			collision.go->hp -= weapon_dmg;
			ReturnGO(go1);
			if (collision.go->hp <= 0)
			{
				if (collision.go->type == GameObject::GO_BARREL)
					openChest(collision.go);
				if (collision.go->item_count >= 3)
					ReturnGO(collision.go);
			}
		}

	}
	if (!portal_shot)
	{
		if (go1->type != GameObject::GO_WALL)
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
				if (!phase)
				{
					go->pos += go->vel * dt * m_speed * time_manip;
					go->vel.y -= dt * elasticity;
				}
				else if (go == m_player)
				{
					go->pos += go->vel * dt * m_speed;
				}
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
	double x, y;
	Application::GetCursorPos(&x, &y);
	ScreenSpaceToWorldSpace(x, y);
	//Mouse Section
	static bool bLButtonState = false;
	float angle;
	if (Application::IsMousePressed(0))
	{
		if (m_player->bullet_delay > m_player->fire_rate / time_manip)
		{
			if (!shotgun)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_BULLET;
				newGO->scale.Set(1, 0.5f, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3(x - newGO->pos.x, y - newGO->pos.y, 0);
				newGO->vel.Normalize() * 100;
			}
			else
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				newGO->type = GameObject::GO_BULLET;
				newGO->scale.Set(1, 0.5f, 0);
				newGO->pos = m_player->pos;
				newGO->vel = Vector3(x - newGO->pos.x, y - newGO->pos.y, 0);
				angle = atan2f(newGO->vel.y, newGO->vel.x) + Math::DegreeToRadian(1);
				newGO->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO->vel.Normalize() * 100;

				GameObject* newGO2 = FetchGO();
				newGO2->active = true;
				newGO2->type = GameObject::GO_BULLET;
				newGO2->scale.Set(1, 0.5f, 0);
				newGO2->pos = m_player->pos;
				newGO2->vel = Vector3(x - newGO->pos.x, y - newGO->pos.y, 0);
				angle = atan2f(newGO2->vel.y, newGO2->vel.x) - Math::DegreeToRadian(1);
				newGO2->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO2->vel.Normalize() * 100;

				GameObject* newGO3 = FetchGO();
				newGO3->active = true;
				newGO3->type = GameObject::GO_BULLET;
				newGO3->scale.Set(1, 0.5f, 0);
				newGO3->pos = m_player->pos;
				newGO3->vel = Vector3(x - newGO->pos.x, y - newGO->pos.y, 0);
				angle = atan2f(newGO3->vel.y, newGO3->vel.x) + Math::DegreeToRadian(3);
				newGO3->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO3->vel.Normalize() * 100;

				GameObject* newGO4 = FetchGO();
				newGO4->active = true;
				newGO4->type = GameObject::GO_BULLET;
				newGO4->scale.Set(1, 0.5f, 0);
				newGO4->pos = m_player->pos;
				newGO4->vel = Vector3(x - newGO->pos.x, y - newGO->pos.y, 0);
				angle = atan2f(newGO4->vel.y, newGO4->vel.x) - Math::DegreeToRadian(3);
				newGO4->vel = Vector3(cosf(angle), sin(angle), 0);
				newGO4->vel.Normalize() * 100;
			}
			m_player->bullet_delay = 0;
		}
	}
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
		if (!m_player->left)
		{
			if (!time_change)
				newGO->vel = Vector3(40, 60, 0);
			else
				newGO->vel = Vector3(60, 80, 0);
			cout << "right true" << endl;
		}
		else
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
					ReturnGO(go);
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
		if (!time_change)
			m_player->vel.y += 100;
		else
			m_player->vel.y += 140;
		jump = true;
	}

	// Movement
	if (phase)
	{
		if (Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
		{
			m_player->vel.y += 3;
		}
		if (Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
		{
			m_player->vel.y -= 3;
		}
	}
	if (Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
	{
		if (!jump)
			m_player->vel.x -= 3;
		else
			m_player->vel.x -= 1;

		m_player->left = true;
	}
	else if (Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
	{
		if (!jump)
			m_player->vel.x += 3;
		else
			m_player->vel.x += 1;

		m_player->left = false;
	}
	else
	{
		m_player->vel.x *= 0.7;
	}
	Boundary(m_player, 1);
	
	
	m_player->under_box->pos.x = m_player->pos.x;
	m_player->under_box->pos.y = (m_player->pos.y - 1);
	if (m_player->vel.y <= 0)
	{
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_WALL || go->type == GameObject::GO_PLATFORM)
				{
					Collision collision = CheckCollision(m_player->under_box, go);
					if (collision.dist > 0)
					{
						jump = false;
					}
				}
			}
		}
	}
	else jump = true;
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
							Collision collision = CheckCollision(go, go2);
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
	Gravity(GameObject::GO_DEMON, 300, dt);
	Gravity(GameObject::GO_TERMINATOR, 300, dt);
	Gravity(GameObject::GO_SOLDIER, 300, dt);
	enableCollision(dt, GameObject::GO_CUBE);
	enableCollision(dt, GameObject::GO_BULLET);
	enableCollision(dt, GameObject::GO_POTION);
	enableCollision(dt, GameObject::GO_MAXPOTION);
	enableCollision(dt, GameObject::GO_MANAPOTION);
	enableCollision(dt, GameObject::GO_GRENADE);
	enableCollision(dt, GameObject::GO_GOLD);
	enableCollision(dt, GameObject::GO_CHEST);
	enableCollision(dt, GameObject::GO_BARREL);
	enableCollision(dt, GameObject::GO_DEMON);
	enableCollision(dt, GameObject::GO_FIREBALL);
	enableCollision(dt, GameObject::GO_FALLENANGEL);
	enableCollision(dt, GameObject::GO_TERMINATOR);
	enableCollision(dt, GameObject::GO_SOLDIER);
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
					Collision collision = CheckCollision(first, other);
					if (collision.dist > 0)
					{
						if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_PLATFORM)
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
			go->pos.x - go->scale.x < 0 && go->vel.x < 0) 
		{
			if (go == m_player)
			{
				if (go->pos.x + go->scale.x > m_worldWidth) // if player outside on the right side
					m_AttemptRight = true;
				else m_AttemptLeft = true;
			}
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
			ReturnGO(go);
		}
	}
}

void SceneArchangel::setGun(float fire, int dmg)
{
	m_player->fire_rate = fire;
	weapon_dmg = dmg;
}

void SceneArchangel::pickWeapon(double dt)
{
	// Sword
	if (weapon_choice >= 0 && weapon_choice <= 4)
		Melee();
	else
		SpawnBullet(dt);

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
	if (Application::IsKeyPressed('0'))
	{
		setGun(0.2f, 10);
		shotgun = false;
		weapon_choice = 0;
	}
	else if (Application::IsKeyPressed('1'))
	{
		setGun(0.2f, 20);
		shotgun = false;
		weapon_choice = 1;
	}
	// Knife
	else if (Application::IsKeyPressed('2'))
	{
		setGun(0.1f, 15);
		shotgun = false;
		weapon_choice = 2;
	}
	// Spear
	else if (Application::IsKeyPressed('3'))
	{
		setGun(0.25f, 20);
		shotgun = false;
		weapon_choice = 3;
	}
	// Scythe
	else if (Application::IsKeyPressed('4'))
	{
		setGun(0.6f, 30);
		shotgun = true;
		weapon_choice = 4;
	}
	//AK
	else if (Application::IsKeyPressed('5'))
	{
		setGun(0.2f, 3);
		shotgun = false;
		weapon_choice = 5;
	}
	// SMG
	else if (Application::IsKeyPressed('6'))
	{
		setGun(0.1f, 2);
		shotgun = false;
		weapon_choice = 6;
	}
	// LMG
	else if (Application::IsKeyPressed('7'))
	{
		setGun(0.25f, 4);
		shotgun = false;
		weapon_choice = 7;
	}
	// SHOTGUN
	else if (Application::IsKeyPressed('8'))
	{
		setGun(0.6f, 2);
		shotgun = true;
		weapon_choice = 8;
	}
	// REVOLVER
	else if (Application::IsKeyPressed('9'))
	{
		setGun(0.4f, 6);
		shotgun = false;
		weapon_choice = 9;
	}

}

void SceneArchangel::takeDMG()
{
	if (dmg_delay > 1)
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

void SceneArchangel::Melee()
{
	static bool bLButtonState3 = false;
	if (Application::IsMousePressed(0) && !bLButtonState3)
	{
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_DEMON || go->type == GameObject::GO_FALLENANGEL || go->type == GameObject::GO_TERMINATOR || go->type == GameObject::GO_SOLDIER)
				{
					if (go->pos.y + 5 > m_player->pos.y && go->pos.y - 5 < m_player->pos.y)
					{
						if (m_player->left)
						{
							if (m_player->pos.x - 10 < go->pos.x && m_player->pos.x > go->pos.x && !m_player->attack)
							{
								go->hp -= weapon_dmg;
								go->vel.x -= 25;
								m_player->attack = true;
							}
						}
						else if (!m_player->left)
						{
							if (m_player->pos.x + 10 > go->pos.x && m_player->pos.x < go->pos.x && !m_player->attack)
							{
								go->hp -= weapon_dmg;
								go->vel.x += 25;
								m_player->attack = true;
							}
						}
					}
				}
			}
		}
		bLButtonState3 = true;
	}
	else if (!Application::IsMousePressed(0))
	{
		bLButtonState3 = false;
		m_player->attack = false;
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

void SceneArchangel::mana(float interval, float amount,  bool restore)
{
	if (mana_delay > interval && m_player->mana > 0 && m_player->mana <= 50)
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

void SceneArchangel::demonAI(double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_FIREBALL)
			{
				go->pos += go->vel * dt * 100 * time_manip;
				Boundary(go, 2);
			}
			
			if (go->type == GameObject::GO_DEMON)
			{
				Boundary(go, 1);
				go->FSMCounter += dt;
				go->bullet_delay += dt;
				go->speed = 6;

				go->left_box->pos.x = go->pos.x - 4.f;
				go->left_box->pos.y = go->pos.y - 1.5f;
				go->right_box->pos.x = go->pos.x + 4.f;
				go->right_box->pos.y = go->pos.y - 1.5f;

				if (go->hp <= 0)
				{
					ReturnGO(go);
					ReturnGO(go->left_box);
					ReturnGO(go->right_box);
				}


				// Setting speed limiters
				if (go->vel.x >= 25)
				{
					go->vel.x = 25;
				}
				if (go->vel.x <= -25)
				{
					go->vel.x = -25;
				}

				switch (go->state)
				{
				case go->STATE_IDLE:
					if (go->FSMCounter > go->MaxFSMCounter)
					{
						go->state = go->STATE_PATROL;
						go->FSMCounter = 0;
					}
					break;

				case go->STATE_PATROL:
					if (go->left)
						go->vel.x -= go->speed;
					else if (!go->left)
						go->vel.x += go->speed;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{
							if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_PLATFORM)
							{
								Collision collision = CheckCollision(go->left_box, go2);
								Collision collision2 = CheckCollision(go->right_box, go2);
								if (collision.dist > 0 && collision2.dist <= 0)
									go->left = true;
								else if(collision2.dist > 0 && collision.dist <= 0)
									go->left = false;
							}
						}
					}

					// To the right / left of demon
					if (go->pos.x + 10 > m_player->pos.x || go->pos.x - 10 < m_player->pos.x)
					{
						if (go->pos.y + 5 > m_player->pos.y && go->pos.y - 5 < m_player->pos.y)
							go->state = go->STATE_CLOSE_ATTACK;
						else 
							go->state = go->STATE_FAR_ATTACK;
					}
					
					
					if (go->FSMCounter > go->MaxFSMCounter * 3)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}

					break;

				case go->STATE_CLOSE_ATTACK:
					go->speed = 3;
					// To the right / left of demon
					if (go->pos.x > m_player->pos.x)
						go->vel.x -= go->speed;
					else if (go->pos.x < m_player->pos.x)
						go->vel.x += go->speed;
				
					if (go->FSMCounter > go->MaxFSMCounter * 5)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}
					break;

				case go->STATE_FAR_ATTACK:
					go->fire_rate = 2;
					if (go->FSMCounter > go->MaxFSMCounter * 5)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}

					if (go->pos.y + 5 > m_player->pos.y && go->pos.y - 5 < m_player->pos.y)
					{
						go->state = go->STATE_CLOSE_ATTACK;
						go->FSMCounter = 0;
					}

					if (go->bullet_delay > go->fire_rate / time_manip)
					{
						GameObject* newGO = FetchGO();
						newGO->active = true;
						newGO->type = GameObject::GO_FIREBALL;
						newGO->scale.Set(2.f, 1.f, 0);
						newGO->pos = go->pos;
						float angle = atan2f(m_player->pos.y - go->pos.y, m_player->pos.x - go->pos.x);
						newGO->vel = Vector3(cosf(angle), sin(angle), 0);
						newGO->vel.Normalize() * 100;
						go->bullet_delay = 0;
					}
					break;

				default:
					break;

				}
			}
		}
	}
}

void SceneArchangel::fallenAngelAI(double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_FIREBALL)
			{
				go->pos += go->vel * dt * 100 * time_manip;
				Boundary(go, 2);
			}

			if (go->type == GameObject::GO_FALLENANGEL)
			{
				Boundary(go, 1);
				go->pos += go->vel * dt * m_speed * time_manip;
				go->FSMCounter += dt;
				go->bullet_delay += dt;
				go->speed = 3;

				go->left_box->pos.x = go->pos.x - 4.f;
				go->left_box->pos.y = go->pos.y;
				go->right_box->pos.x = go->pos.x + 4.f;
				go->right_box->pos.y = go->pos.y;

				if (go->hp <= 0)
				{
					ReturnGO(go);
					ReturnGO(go->left_box);
					ReturnGO(go->right_box);
				}


				// Setting speed limiters
				if (go->vel.x >= 25)
				{
					go->vel.x = 25;
				}
				if (go->vel.x <= -25)
				{
					go->vel.x = -25;
				}

				switch (go->state)
				{
				case go->STATE_IDLE:
					if (go->FSMCounter > go->MaxFSMCounter)
					{
						go->state = go->STATE_PATROL;
						go->FSMCounter = 0;
					}
					break;

				case go->STATE_PATROL:
					if (go->left)
						go->vel.x -= go->speed;
					else if (!go->left)
						go->vel.x += go->speed;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{
							if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_PLATFORM)
							{
								Collision collision = CheckCollision(go->left_box, go2);
								Collision collision2 = CheckCollision(go->right_box, go2);
								if (collision.dist > 0 && collision2.dist <= 0)
									go->left = true;
								else if (collision2.dist > 0 && collision.dist <= 0)
									go->left = false;
							}
						}
					}

					// To the right / left of angel
					if (go->pos.x + 20 > m_player->pos.x || go->pos.x - 20 < m_player->pos.x)
					{
						float rng = Math::RandIntMinMax(1, 2);
						if (rng == 1)
							go->state = go->STATE_CLOSE_ATTACK;
						else if (rng == 2)
							go->state = go->STATE_FAR_ATTACK;
					}


					if (go->FSMCounter > go->MaxFSMCounter * 3)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}

					break;

				case go->STATE_CLOSE_ATTACK:
					go->speed = 3;
					// To the right / left of demon
					if (go->pos.x > m_player->pos.x)
						go->vel.x -= go->speed;
					else if (go->pos.x < m_player->pos.x)
						go->vel.x += go->speed;

					if (!go->attack)
					{
						if (go->pos.y > m_player->pos.y + 3)
							go->vel.y -= go->speed;
						else if (go->pos.y < m_player->pos.y)
							go->vel.y += go->speed;
						else
							go->attack = true;
					}

					if (go->FSMCounter > go->MaxFSMCounter)
					{
						if (go->pos.y <= m_player->pos.y + 20)
						{
							go->vel.y += go->speed;
						}
						else
						{
							go->vel.y *= 0;
							go->state = go->STATE_IDLE;
							go->FSMCounter = 0;
							go->attack = false;
						}
					}
					break;

				case go->STATE_FAR_ATTACK:
					go->fire_rate = 1;
					if (go->FSMCounter > go->MaxFSMCounter * 2)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}

					if (go->pos.y + 5 > m_player->pos.y && go->pos.y - 5 < m_player->pos.y)
					{
						go->state = go->STATE_CLOSE_ATTACK;
						go->FSMCounter = 0;
					}

					if (go->bullet_delay > go->fire_rate / time_manip)
					{
						GameObject* newGO = FetchGO();
						newGO->active = true;
						newGO->type = GameObject::GO_FIREBALL;
						newGO->scale.Set(2.f, 1.f, 0);
						newGO->pos = go->pos;
						float angle = atan2f(m_player->pos.y - go->pos.y, m_player->pos.x - go->pos.x);
						newGO->vel = Vector3(cosf(angle), sin(angle), 0);
						newGO->vel.Normalize() * 100;
						go->bullet_delay = 0;
					}
					break;

				default:
					break;

				}
			}
		}
	}
}

void SceneArchangel::terminatorAI(double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_ENEMY_BULLET)
			{
				go->pos += go->vel * dt * 100 * time_manip;
				Boundary(go, 2);
			}

			if (go->type == GameObject::GO_TERMINATOR)
			{
				Boundary(go, 1);
				go->FSMCounter += dt;
				go->bullet_delay += dt;
				if (!go->rage)
				{
					go->speed = 1;
					go->fire_rate = 1;
				}
				else
				{
					go->speed = 3;
					go->fire_rate = 0.5f;
				}

				go->left_box->pos.x = go->pos.x - 4.f;
				go->left_box->pos.y = go->pos.y - 1.5f;
				go->right_box->pos.x = go->pos.x + 4.f;
				go->right_box->pos.y = go->pos.y - 1.5f;
				
				if (!go->rage && go->hp <= 10)
				{
					go->state = go->STATE_RAGE;
					go->FSMCounter = 0;
				}
				else if (go->hp <= 0)
				{
					ReturnGO(go);
					ReturnGO(go->left_box);
					ReturnGO(go->right_box);
				}


				// Setting speed limiters
				if (go->vel.x >= 25)
				{
					go->vel.x = 25;
				}
				if (go->vel.x <= -25)
				{
					go->vel.x = -25;
				}

				switch (go->state)
				{
				case go->STATE_IDLE:
					if (go->FSMCounter > go->MaxFSMCounter)
					{
						go->state = go->STATE_PATROL;
						go->FSMCounter = 0;
					}
					break;

				case go->STATE_PATROL:
					if (go->left)
						go->vel.x -= go->speed;
					else if (!go->left)
						go->vel.x += go->speed;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{
							if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_PLATFORM)
							{
								Collision collision = CheckCollision(go->left_box, go2);
								Collision collision2 = CheckCollision(go->right_box, go2);
								if (collision.dist > 0 && collision2.dist <= 0)
									go->left = true;
								else if (collision2.dist > 0 && collision.dist <= 0)
									go->left = false;
							}
						}
					}

					// To the right / left of angel
					if (go->pos.x + 20 > m_player->pos.x || go->pos.x - 20 < m_player->pos.x)
					{
						go->state = go->STATE_FAR_ATTACK;
					}


					if (go->FSMCounter > go->MaxFSMCounter * 6)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}

					break;

				case go->STATE_FAR_ATTACK:
					if (go->pos.x > m_player->pos.x)
						go->vel.x -= go->speed;
					else if (go->pos.x < m_player->pos.x)
						go->vel.x += go->speed;

					if (go->FSMCounter > go->MaxFSMCounter * 5 && !go->rage)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}
					if (go->bullet_delay > go->fire_rate / time_manip)
					{
						GameObject* newGO = FetchGO();
						newGO->active = true;
						newGO->type = GameObject::GO_ENEMY_BULLET;
						newGO->scale.Set(1.f, 0.5f, 0);
						newGO->pos = go->pos;
						float angle = atan2f(m_player->pos.y - go->pos.y, m_player->pos.x - go->pos.x);
						newGO->vel = Vector3(cosf(angle), sin(angle), 0);
						newGO->vel.Normalize() * 100;
						go->bullet_delay = 0;
					}
					break;


				case go->STATE_RAGE:
					go->rage = true;
					go->hp = 20;
					if (go->FSMCounter > go->MaxFSMCounter * 3)
					{
						go->hp += 40;
						go->state = go->STATE_FAR_ATTACK;
						go->FSMCounter = 0;
						go->attack = false;
					}
					break;

				
				default:
					break;

				}
			}
		}
	}
}

void SceneArchangel::soldierAI(double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_ENEMY_BULLET)
			{
				go->pos += go->vel * dt * 100 * time_manip;
				Boundary(go, 2);
			}

			if (go->type == GameObject::GO_SOLDIER)
			{
				Boundary(go, 1);
				go->FSMCounter += dt;
				go->bullet_delay += dt;
				go->speed = 2;
				go->fire_rate = 0.5f;
				
				go->left_box->pos.x = go->pos.x - 4.f;
				go->left_box->pos.y = go->pos.y - 1.5f;
				go->right_box->pos.x = go->pos.x + 4.f;
				go->right_box->pos.y = go->pos.y - 1.5f;

				if (go->hp <= 0)
				{
					ReturnGO(go);
					ReturnGO(go->left_box);
					ReturnGO(go->right_box);
				}

				// Setting speed limiters
				if (go->vel.x >= 25)
				{
					go->vel.x = 25;
				}
				if (go->vel.x <= -25)
				{
					go->vel.x = -25;
				}
				switch (go->state)
				{
				case go->STATE_IDLE:
					if (go->FSMCounter > go->MaxFSMCounter)
					{
						go->state = go->STATE_PATROL;
						go->FSMCounter = 0;
					}
					break;

				case go->STATE_PATROL:
					if (go->left)
						go->vel.x -= go->speed;
					else if (!go->left)
						go->vel.x += go->speed;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{
							if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_PLATFORM)
							{
								Collision collision = CheckCollision(go->left_box, go2);
								Collision collision2 = CheckCollision(go->right_box, go2);
								if (collision.dist > 0 && collision2.dist <= 0)
									go->left = true;
								else if (collision2.dist > 0 && collision.dist <= 0)
									go->left = false;
							}
						}
					}

					// To the right / left of angel
					if (go->pos.x + 30 > m_player->pos.x || go->pos.x - 30 < m_player->pos.x)
					{
						go->state = go->STATE_FAR_ATTACK;
					}

					if (go->FSMCounter > go->MaxFSMCounter * 5)
					{
						go->state = go->STATE_IDLE;
						go->FSMCounter = 0;
					}
					break;

				case go->STATE_FAR_ATTACK:
					if (go->FSMCounter > go->MaxFSMCounter * 3)
					{
						go->state = go->STATE_RELOAD;
						go->FSMCounter = 0;
					}
					if (go->bullet_delay > go->fire_rate / time_manip)
					{
						GameObject* newGO = FetchGO();
						newGO->active = true;
						newGO->type = GameObject::GO_ENEMY_BULLET;
						newGO->scale.Set(1.f, 0.5f, 0);
						newGO->pos = go->pos;
						float angle = atan2f(m_player->pos.y - go->pos.y, m_player->pos.x - go->pos.x);
						newGO->vel = Vector3(cosf(angle), sin(angle), 0);
						newGO->vel.Normalize() * 100;
						go->bullet_delay = 0;
					}
					break;

				case go->STATE_RELOAD:
					if (go->pos.x > m_player->pos.x - 40 && go->pos.x < m_player->pos.x)
						go->vel.x -= go->speed;
					else if (go->pos.x < m_player->pos.x + 40 && go->pos.x > m_player->pos.x)
						go->vel.x += go->speed;
					
					if (go->FSMCounter > go->MaxFSMCounter * 2)
					{
						go->state = go->STATE_PATROL;
						go->FSMCounter = 0;
					}
					break;

				default:
					break;

				}
			}
		}
	}
}

void SceneArchangel::runAnimation(double dt, GameObject::GAMEOBJECT_TYPE GO, double animation_max, int frame_max)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				go->animation_delay += dt;
				if (go->animation_delay > animation_max)
				{
					go->frame_count++;
					go->animation_delay = 0;
				}
				if (go->frame_count >= frame_max)
				{
					go->frame_count = 0;
				}
			}
		}
	}
}


void SceneArchangel::ScreenSpaceToWorldSpace(double& x, double& y)
{
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	double sX = x;
	double sY = y;
	x = (sX / w * m_screenWidth) + (cameraPos.x - m_screenWidth * .5f);
	y = ((h - sY) / h * m_screenHeight) + (cameraPos.y - m_screenHeight * .5f);
}

GameObject* SceneArchangel::ObjectOnCursor()
{
	double x, y;
	Application::GetCursorPos(&x, &y);
	ScreenSpaceToWorldSpace(x, y);
	
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			m_emptyGO->pos.Set(x, y);
			m_emptyGO->scale.Set(.01f, .01f, 1);
			m_emptyGO->type = GameObject::GO_GHOSTBALL;
			if (CheckCollision(m_emptyGO, go).dist > 0)
				return go;
			else if ( // AABB Collision check
				go->pos.x + go->scale.x >= m_emptyGO->pos.x &&
				go->pos.x - go->scale.x <= m_emptyGO->pos.x &&
				go->pos.y + go->scale.y >= m_emptyGO->pos.y &&
				go->pos.y - go->scale.y >= m_emptyGO->pos.y
				)
			{
				return go;
			}
		}
	}
	return nullptr;
}

void SceneArchangel::manipTime(double dt)
{
	static bool bLButtonState3 = false;
	static bool bLButtonState4 = false;
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
		time_change = false;
	}
	
	if (Application::IsKeyPressed('O') && !bLButtonState4)
	{
		bLButtonState4 = true;
		if (!phase)
		{
			phase = true;
			time_manip = 0.0f;
		}
		else
		{
			phase = false;
			time_manip = 1.f;
		}
	}
	else if (!Application::IsKeyPressed('O'))
	{
		bLButtonState4 = false;
	}
	if (time_change)
		mana(0.25f, 0.75f, false);
	else if (phase)
		mana(0.25f, 1.f, false);

	if (m_player->mana <= 0)
	{
		time_change = false;
		phase = false;
		time_manip = 1.f;
	}
}

void SceneArchangel::setCollisionBox(GameObject* go)
{
	go->left_box = FetchGO();
	go->left_box->active = true;
	go->left_box->type = GameObject::GO_GHOSTBALL;
	go->left_box->pos = go->pos;
	go->left_box->scale = go->scale;
	go->left_box->normal = go->normal;

	go->right_box = FetchGO();
	go->right_box->active = true;
	go->right_box->type = GameObject::GO_GHOSTBALL;
	go->right_box->pos = go->pos;
	go->right_box->scale = go->scale;
	go->right_box->normal = go->normal;
}

void SceneArchangel::InitMap()
{
	ClearMap();
	const MapData* mapInfo = mapMaker.GetMapData();
	bool haveEnemies = false;
	for (int i = 0; i < mapInfo->wallDataList.size(); i++)
	{ // spawn walls
		cout << "spawned wall, ";
		GameObject* go = FetchGO();
		go->active = true;
		go->type = mapInfo->wallDataList[i]->type;
		go->pos = mapInfo->wallDataList[i]->pos;
		cout << go->pos << ", ";
		go->scale = mapInfo->wallDataList[i]->scale;
		cout << go->scale << ", ";
		go->normal = mapInfo->wallDataList[i]->rot;
		cout << go->normal << endl;
		go->hp = 100;
	}
	for (int i = 0; i < mapInfo->entityDataList.size(); i++)
	{
		if (mapInfo->entityDataList[i]->type == GameObject::GO_CUBE)
		{ // set player position
			//if (mapMaker.IsFromFront())
			{
				m_player->pos = mapInfo->entityDataList[i]->pos;
				cout << "set player pos enter" << m_player->pos << endl;
			}
		}
		else if (mapInfo->entityDataList[i]->type == GameObject::GO_GHOSTBALL)
		{
			/*if (!mapMaker.IsFromFront())
			{
				m_player->pos = mapInfo->entityDataList[i]->pos;
				cout << "set player pos exit" << m_player->pos << endl;
			}*/
		}
		else if 
			(
				mapInfo->entityDataList[i]->type != GameObject::GO_TERMINATOR &&
				mapInfo->entityDataList[i]->type != GameObject::GO_SOLDIER &&
				mapInfo->entityDataList[i]->type != GameObject::GO_FALLENANGEL &&
				mapInfo->entityDataList[i]->type != GameObject::GO_DEMON
				)
		{ // Spawn loot and enemies
			cout << "spawned entity, ";
			GameObject* go = FetchGO();
			go->active = true;
			go->type = mapInfo->entityDataList[i]->type;
			go->pos = mapInfo->entityDataList[i]->pos;
			cout << go->pos << ", ";
			go->scale = mapInfo->entityDataList[i]->scale;
			cout << go->scale << ", ";
			go->normal = mapInfo->entityDataList[i]->rot;
			cout << go->normal << endl;
			if (mapInfo->entityDataList[i]->type == GameObject::GO_BARREL)
				go->hp = 15;
			/*if (mapInfo->entityDataList[i]->type == GameObject::GO_DEMON)
			{
				go->hp = 30;
				setCollisionBox(go);
			}
			else if (mapInfo->entityDataList[i]->type == GameObject::GO_FALLENANGEL)
			{
				go->hp = 60;
				setCollisionBox(go);
			}
			else if (mapInfo->entityDataList[i]->type == GameObject::GO_TERMINATOR)
			{
				go->hp = 90;
				setCollisionBox(go);
			}
			else if (mapInfo->entityDataList[i]->type == GameObject::GO_SOLDIER)
			{
				go->hp = 75;
				setCollisionBox(go);
			}*/
		}
		else // have enemies in this level
		{
			haveEnemies = true;
		}
	}
	if (haveEnemies)
	{
		playState = PLAY_PREBATTLE; // enemies in this level
		cout << "have enemies\n";
	}
	else
	{
		playState = PLAY_POSTBATTLE; // no enemies in this level
		cout << "no enemies\n";
	}
}

void SceneArchangel::SaveMap()
{
	vector<GOData*> savedEntities;
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active && go != m_player->under_box)
		{
			if (go->type == GameObject::GO_CUBE ||
				go->type == GameObject::GO_BARREL || // Stuff to save
				go->type == GameObject::GO_POTION ||
				go->type == GameObject::GO_MAXPOTION ||
				go->type == GameObject::GO_MANAPOTION ||
				go->type == GameObject::GO_CHEST ||
				go->type == GameObject::GO_GOLD ||
				go->type == GameObject::GO_PORTAL_IN ||
				go->type == GameObject::GO_PORTAL_OUT
				)
			{
				if (go->type == GameObject::GO_CUBE)
				{
					if (go->pos.x > 180 * .5f)
						go->pos.x = 175;
					else go->pos.x = 5;
				}
				GOData* goData = new GOData();
				goData->type = go->type;
				goData->pos = go->pos;
				goData->rot = go->normal;
				goData->scale = go->scale;
				savedEntities.push_back(goData);
			}
		}
	}
	mapMaker.SaveEntityData(savedEntities);
}

void SceneArchangel::ClearMap()
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active && go != m_player && go != m_player->under_box)
		{
			ReturnGO(go);
		}
	}
	cout << "clearmap\n";
}

void SceneArchangel::SpawnEnemies()
{
	const MapData* mapInfo = mapMaker.GetMapData();
	for (int i = 0; i < mapInfo->entityDataList.size(); i++)
	{
		if ( // Put all the enemy types here
			mapInfo->entityDataList[i]->type == GameObject::GO_TERMINATOR ||
			mapInfo->entityDataList[i]->type == GameObject::GO_SOLDIER ||
			mapInfo->entityDataList[i]->type == GameObject::GO_FALLENANGEL ||
			mapInfo->entityDataList[i]->type == GameObject::GO_DEMON
			)
		{ // Spawn loot and enemies
			cout << "spawned entity, ";
			GameObject* go = FetchGO();
			go->active = true;
			go->type = mapInfo->entityDataList[i]->type;
			go->pos = mapInfo->entityDataList[i]->pos;
			cout << go->pos << ", ";
			go->scale = mapInfo->entityDataList[i]->scale;
			cout << go->scale << ", ";
			go->normal = mapInfo->entityDataList[i]->rot;
			cout << go->normal << endl;
			if (mapInfo->entityDataList[i]->type == GameObject::GO_DEMON)
			{
				go->hp = 30;
				setCollisionBox(go);
			}
			else if (mapInfo->entityDataList[i]->type == GameObject::GO_FALLENANGEL)
			{
				go->hp = 60;
				setCollisionBox(go);
			}
			else if (mapInfo->entityDataList[i]->type == GameObject::GO_TERMINATOR)
			{
				go->hp = 90;
				setCollisionBox(go);
			}
			else if (mapInfo->entityDataList[i]->type == GameObject::GO_SOLDIER)
			{
				go->hp = 75;
				setCollisionBox(go);
			}
		}
	}
}

void SceneArchangel::Update(double dt)
{
	// Update timers
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
						ReturnGO(go);
					}
				}
			}

			mapMaker.GenerateMap();
			InitMap();

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

		portalLogic(dt);
		pickWeapon(dt);
		itemLogic(dt);
		playerLogic(dt);
		throwGrenade(dt);
		manipTime(dt);
		if (playState == PLAY_BATTLE)
		{
			demonAI(dt);
			fallenAngelAI(dt);
			terminatorAI(dt);
			soldierAI(dt);
			runAnimation(dt, GameObject::GO_CUBE, 0.25f, 16);
			m_AttemptLeft = m_AttemptRight = false;

			if  ( // if no more enemies left
				FindGameObjectWithType(GameObject::GO_SOLDIER) == nullptr &&
				FindGameObjectWithType(GameObject::GO_FALLENANGEL) == nullptr &&
				FindGameObjectWithType(GameObject::GO_TERMINATOR) == nullptr &&
				FindGameObjectWithType(GameObject::GO_DEMON) == nullptr
				)
			{
				playState = PLAY_POSTBATTLE;
			}
		}
		else if (playState == PLAY_PREBATTLE && m_player->pos.x > 20 && m_player->pos.x < 160)
		{
			SpawnEnemies();
			playState = PLAY_BATTLE;
		}
		else
		{
			// Change Level
			if (m_AttemptLeft)
			{
				m_AttemptLeft = false;
				if (playState == PLAY_POSTBATTLE) SaveMap();
				if (mapMaker.GoLeft())
				{
					InitMap();
				}
			}
			if (m_AttemptRight)
			{
				m_AttemptRight = false;
				if (playState == PLAY_POSTBATTLE) SaveMap();
				if (mapMaker.GoRight())
				{
					InitMap();
				}
			}
		}

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

		if (Application::IsKeyPressed(VK_F1))
		{
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->active)
				{
					cout << go->type << endl;
				}
			}
		}
		static bool f3ButtonState = false;
		if (Application::IsKeyPressed(VK_F3) && !f3ButtonState)
		{
			f3ButtonState = true;
			m_toggleDebugScreen = !m_toggleDebugScreen;
		}
		else if (!Application::IsKeyPressed(VK_F3))
		{
			f3ButtonState = false;
		}
	}
}


void SceneArchangel::RenderGO(GameObject *go)
{
	float angle;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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

	//case GameObject::GO_GHOSTBALL:
	//	// Ball displayed at top
	//	modelStack.PushMatrix();
	//	modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	//	modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
	//	modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
	//	RenderMesh(meshList[GEO_BALL], false);
	//	modelStack.PopMatrix();
	//	break;

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

	case GameObject::GO_PLATFORM:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	    angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_GREENCUBE], false);
		modelStack.PopMatrix();
		break;
		
	case GameObject::GO_BOUNCEPLATFORM:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	    angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUECUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CUBE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y + 1.5f, go->pos.z);
		angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		if (m_player->left)
			modelStack.Rotate(0, 0, 1, 0);
		else
			modelStack.Rotate(180, 0, 1, 0);
		

		if (weapon_choice == 0)
		{
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			if (go->frame_count == 0)
				RenderMesh(meshList[GEO_FRAME1], false);
			else if (go->frame_count == 1)
				RenderMesh(meshList[GEO_FRAME2], false);
			else if (go->frame_count == 2)
				RenderMesh(meshList[GEO_FRAME3], false);
			else if (go->frame_count == 3)
				RenderMesh(meshList[GEO_FRAME4], false);
			else if (go->frame_count == 4)
				RenderMesh(meshList[GEO_FRAME5], false);
			else if (go->frame_count == 5)
				RenderMesh(meshList[GEO_FRAME6], false);
			else if (go->frame_count == 6)
				RenderMesh(meshList[GEO_FRAME7], false);
			else if (go->frame_count == 7)
				RenderMesh(meshList[GEO_FRAME8], false);
			else if (go->frame_count == 8)
				RenderMesh(meshList[GEO_FRAME9], false);
			else if (go->frame_count == 9)
				RenderMesh(meshList[GEO_FRAME10], false);
			else if (go->frame_count == 10)
				RenderMesh(meshList[GEO_FRAME11], false);
			else if (go->frame_count == 11)
				RenderMesh(meshList[GEO_FRAME12], false);
			else if (go->frame_count == 12)
				RenderMesh(meshList[GEO_FRAME13], false);
			else if (go->frame_count == 13)
				RenderMesh(meshList[GEO_FRAME14], false);
			else if (go->frame_count == 14)
				RenderMesh(meshList[GEO_FRAME15], false);
			else if (go->frame_count == 15)
				RenderMesh(meshList[GEO_FRAME16], false);
			else if (go->frame_count == 16)
				RenderMesh(meshList[GEO_FRAME17], false);
		}
		else
		{
			modelStack.Scale(go->scale.x - 0.7f, go->scale.y, go->scale.z);
			if (go->frame_count == 0)
				RenderMesh(meshList[GEO_1FRAME1], false);
			else if (go->frame_count == 1)
				RenderMesh(meshList[GEO_1FRAME2], false);
			else if (go->frame_count == 2)
				RenderMesh(meshList[GEO_1FRAME3], false);
			else if (go->frame_count == 3)
				RenderMesh(meshList[GEO_1FRAME4], false);
			else if (go->frame_count == 4)
				RenderMesh(meshList[GEO_1FRAME5], false);
			else if (go->frame_count == 5)
				RenderMesh(meshList[GEO_1FRAME6], false);
			else if (go->frame_count == 6)
				RenderMesh(meshList[GEO_1FRAME7], false);
			else if (go->frame_count == 7)
				RenderMesh(meshList[GEO_1FRAME8], false);
			else if (go->frame_count == 8)
				RenderMesh(meshList[GEO_1FRAME9], false);
			else if (go->frame_count == 9)
				RenderMesh(meshList[GEO_1FRAME10], false);
			else if (go->frame_count == 10)
				RenderMesh(meshList[GEO_1FRAME11], false);
			else if (go->frame_count == 11)
				RenderMesh(meshList[GEO_1FRAME12], false);
			else if (go->frame_count == 12)
				RenderMesh(meshList[GEO_1FRAME13], false);
			else if (go->frame_count == 13)
				RenderMesh(meshList[GEO_1FRAME14], false);
			else if (go->frame_count == 14)
				RenderMesh(meshList[GEO_1FRAME15], false);
			else if (go->frame_count == 15)
				RenderMesh(meshList[GEO_1FRAME16], false);
			else if (go->frame_count == 16)
				RenderMesh(meshList[GEO_1FRAME17], false);
		}
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

	case GameObject::GO_ENEMY_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMY_BULLET], false);
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

	case GameObject::GO_DEMON:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_REDCUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FALLENANGEL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUECUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_TERMINATOR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_SOLDIER:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_GREENCUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FIREBALL :
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FIREBALL], false);
		modelStack.PopMatrix();
		break;
	}
	glEnable(GL_DEPTH_TEST);
}

void SceneArchangel::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Calculating aspect ratio
	m_screenWidth = m_screenHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

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
		modelStack.Translate(m_worldWidth * .5f, m_worldHeight * .5f, 1);
		modelStack.Scale(m_worldWidth * .5f, m_worldHeight * .5f, 1);
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

		std::ostringstream ss;
		std::ostringstream ss2;
		std::ostringstream ss3;

		RenderMeshOnScreen(meshList[GEO_CHARGE], -9.f + (m_player->mana) * 0.36f, 53, 18, 2.f);

		RenderMeshOnScreen(meshList[GEO_GREENBALL], 9, 2.5f, 1, 1.2f);
		ss2 << "x" << m_player->grenade_count;
		RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 1, 1), 2.5f, 10.5f, 1);

		RenderMeshOnScreen(meshList[GEO_YELLOWBALL], 15, 2.5f, 1, 1.2f);
		ss3 << "x" << m_player->gold_count;
		RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(1, 1, 1), 2.5f, 16.5f, 1);

		float angle;
		double x, y;
		Application::GetCursorPos(&x, &y);
		screenSpaceToWorldSpace(x, y);

		if (x < m_player->pos.x)
			m_player->left = true;
		else if (x > m_player->pos.x)
			m_player->left = false;
		glDisable(GL_CULL_FACE);

		if (weapon_choice == 1)
		{
			RenderMeshOnScreen(meshList[GEO_SWORD], 4.2f, 5, 4.f, 3);
		}
		else if (weapon_choice == 2)
		{
			RenderMeshOnScreen(meshList[GEO_KNIFE], 4.2f, 5, 2.f, 1);
		}
		else if (weapon_choice == 3)
		{
			RenderMeshOnScreen(meshList[GEO_SPEAR], 4.2f, 5, 4.f, 3);
		}
		else if (weapon_choice == 4)
		{
			RenderMeshOnScreen(meshList[GEO_SCYTHE], 4.2f, 5, 4.f, 3);
		}
		else if (weapon_choice == 5)
		{
			RenderMeshOnScreen(meshList[GEO_AK47], 4.2f, 5, 4.f, 3);
			modelStack.PushMatrix();
			if (m_player->left)
			{
				modelStack.Translate(m_player->pos.x + 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(180, 1, 0, 0);
			}
			else
			{
				modelStack.Translate(m_player->pos.x - 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(0, 1, 0, 0);
			}
			modelStack.Scale(4, 1, 1);
			RenderMesh(meshList[GEO_AKARM], false);
			modelStack.PopMatrix();
		}
		else if (weapon_choice == 6)
		{
			RenderMeshOnScreen(meshList[GEO_SMG], 4.2f, 5, 4.f, 3);
			modelStack.PushMatrix();
			if (m_player->left)
			{
				modelStack.Translate(m_player->pos.x + 0.9f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(180, 1, 0, 0);
			}
			else
			{
				modelStack.Translate(m_player->pos.x - 0.9f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(0, 1, 0, 0);
			}
			modelStack.Scale(3.8f, 0.8f, 1);
			RenderMesh(meshList[GEO_SMGARM], false);
			modelStack.PopMatrix();
		}
		else if (weapon_choice == 7)
		{
			RenderMeshOnScreen(meshList[GEO_LMG], 4.2f, 5, 4.f, 3);
			modelStack.PushMatrix();
			if (m_player->left)
			{
				modelStack.Translate(m_player->pos.x + 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(180, 1, 0, 0);
			}
			else
			{
				modelStack.Translate(m_player->pos.x - 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(0, 1, 0, 0);
			}
			modelStack.Scale(4, 1, 1);
			RenderMesh(meshList[GEO_LMGARM], false);
			modelStack.PopMatrix();
		}
		else if (weapon_choice == 8)
		{
			RenderMeshOnScreen(meshList[GEO_SHOTGUN], 4.2f, 5, 4.f, 3);
			modelStack.PushMatrix();
			if (m_player->left)
			{
				modelStack.Translate(m_player->pos.x + 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(180, 1, 0, 0);
			}
			else
			{
				modelStack.Translate(m_player->pos.x - 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(0, 1, 0, 0);
			}
			modelStack.Scale(4, 1, 1);
			RenderMesh(meshList[GEO_SHOTGUNARM], false);
			modelStack.PopMatrix();
		}
		else if (weapon_choice == 9)
		{
			RenderMeshOnScreen(meshList[GEO_REVOLVER], 4.2f, 5, 4.f, 3);
			modelStack.PushMatrix();
			if (m_player->left)
			{
				modelStack.Translate(m_player->pos.x + 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(180, 1, 0, 0);
			}
			else
			{
				modelStack.Translate(m_player->pos.x - 0.85f, m_player->pos.y + 3.1f, m_player->pos.z);
				angle = atan2f(y - 4 - m_player->pos.y, x - m_player->pos.x);
				modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
				modelStack.Rotate(0, 1, 0, 0);
			}
			modelStack.Scale(3.8f, 0.8f, 1);
			RenderMesh(meshList[GEO_REVOLVERARM], false);
			modelStack.PopMatrix();
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

		if (m_toggleDebugScreen)
		{
			// Display FPS
			int ylvl = 58 / 2;
			std::ostringstream ss;
			ss << "FPS: " << fps;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // fps
			RenderTextOnScreen(meshList[GEO_TEXT], "Object Count: " + std::to_string(m_objectCount), Color(1, 1, 1), 2, 0, 2 * ylvl--); // object Count
			ss.str("");
			ss << "player position: " << m_player->pos;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // player pos
			double x, y;
			Application::GetCursorPos(&x, &y);
			ss.str("");
			ss << "Mouse position (screen space): " << x << ", " << y;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // cursor pos in screen space
			ScreenSpaceToWorldSpace(x, y);
			ss.str("");
			ss << "Mouse position (world space): " << x << ", " << y;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // cursor pos in screen space
			
			GameObject* goOnCursor = ObjectOnCursor();
			if (goOnCursor != nullptr)
			{
				ss.str("");
				ss << "Object On Cursor{ type: " << goOnCursor->type;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // object on cursor's type
				ss.str("");
				ss << "pos: " << goOnCursor->pos;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // object on cursor's pos
				ss.str("");
				ss << "scale: " << goOnCursor->scale;
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // object on cursor's scale
				ss.str("");
				ss << "hp: " << goOnCursor->hp << " }";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // object on cursor's hp
			}

			ss.str("");
			ss << "Gameplay State: " << playState;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 0, 2 * ylvl--); // gameplay state
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
		//delete m_ghost;
		m_ghost = NULL;
	}
}

