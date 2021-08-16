#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
using namespace std;

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	
	// Initialize variables
	m_objectCount = 0;
	m_ballCount = 0;
	interval = 0;
	m_ballSpawned = 1;
	m_ballMax = 1;
	m_ballLabel = 1;
	ball_spawnrate = 0;
	oldx = 0;
	oldy = 0;
	points = 0;
	points_ceiling = 10;

	// Booleans
	initPos = false;
	spawnGhost = true;
	spawnShape = false;
	moveShape = false;
	addball = false;
	start = true;
	init_game = true;


	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	m_squareCount = 0;

	// Initialize Game state
	state = STATE_MENU;

	// Spawn walls
	GameObject* newGO = FetchGO();
	newGO->active = true;
	newGO->type = GameObject::GO_CUBE;
	newGO->scale.Set(2, 50, 1);
	newGO->normal.Set(1, 0, 0);
	newGO->pos = Vector3(m_worldWidth * 0.5 + 25, m_worldHeight * 0.5, 0);

	GameObject* newGO2 = FetchGO();
	newGO2->active = true;
	newGO2->type = GameObject::GO_CUBE;
	newGO2->scale.Set(2, 50, 1);
	newGO2->normal.Set(1, 0, 0);
	newGO2->pos = Vector3(m_worldWidth * 0.5 - 25, m_worldHeight * 0.5, 0);
}

GameObject* SceneCollision::FetchGO()
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

void SceneCollision::ReturnGO(GameObject *go)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		go = (GameObject*)*it;
		if (go->active)
		{
			go->active = false;
			m_objectCount--;
		}
	}
}

bool SceneCollision::CheckCollision(GameObject* go1, GameObject* go2, float dt)
{
	// Handle collision between GO_BALL and GO_BALL using velocity swap
	if (go2->type == GameObject::GO_BALL)
	{
		float combinedRadii = go1->scale.x + go2->scale.x;
		Vector3 displacement = go2->pos - go1->pos;
		if ((displacement.LengthSquared() < combinedRadii * combinedRadii) && (go2->vel - go1->vel).Dot(go2->pos - go1->pos) < 0)
		{
			
			return true;
		}
	}
	else if (go2->type == GameObject::GO_WALL || go2->type == GameObject::GO_CUBE)
	{
		Vector3 N = go2->normal;
		Vector3 NP(N.y, -N.x, 0);	
		Vector3 w0_b1 = go2->pos - go1->pos;
		float r = go1->scale.x;
		float h_2 = go2->scale.x;
		float l_2 = go2->scale.y;
		if (w0_b1.Dot(N) < 0)
			N = -N; //making sure N is outward normal
		if ((w0_b1).Dot(N) < r + h_2 && abs((w0_b1).Dot(NP)) < l_2 && go1->vel.Dot(N) > 0)
		{
			go2->hp--;
			go2->animation_delay = 0;
			
			// Check 2nd wall
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->active)
				{
					if (go->type == GameObject::GO_WALL)
					{
						if (go2 != go && go2->pos == go->pos)
						{
							go->hp--;
							go->animation_delay = 0;
							go->color = go2->color;
						}
					}
				}
			}
			return true;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR || go2->type == GameObject::GO_CIRCLE || go2->type == GameObject::GO_POWERUP)
	{
		Vector3 u = go1->vel;
		Vector3 p2_p1 = go2->pos - go1->pos;
		float r1 = go1->scale.x;
		float r2 = go2->scale.x;

		if ((abs(p2_p1.Length()) < r1 + r2) && ((p2_p1.Dot(u)) > 0)) //prevent internal collision
		{
			go2->hp--;
			// Check pillar
			if (go2->type == GameObject::GO_PILLAR)
			{
				for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject* go = (GameObject*)*it;
					if (go->active)
					{
						if (go->type == GameObject::GO_WALL)
						{
							if (go2->pos == go->pillar1->pos || go2->pos == go->pillar2->pos || go2->pos == go->pillar3->pos || go2->pos == go->pillar4->pos)
							{
								go->animation_delay = 0;
								go->hp--;
							}
						}
					}
				}
			}

			// Play hit animation
			if (go2->type == GameObject::GO_CIRCLE)
			{
				go2->animation_delay = 0;
			}

			// Check if powerup
			if (go2->type == GameObject::GO_POWERUP)
			{
				addball = true;
				m_ballLabel++;
				go2->active = false;
			}
			return true;
		}
	}
	return false;

}

void SceneCollision::CollisionResponse(GameObject* go1, GameObject* go2)
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
		Vector3 N = go2->normal;
		Vector3 u = go1->vel;
		go1->vel = u - (2 * u.Dot(N)) * N;
		go1->vel.y *= 0.4;
	}
	else if (go2->type == GameObject::GO_PILLAR || go2->type == GameObject::GO_CIRCLE)
	{
		Vector3 N = (go2->pos - go1->pos).Normalized();
		Vector3 u = go1->vel;
		go1->vel = u - (2 * u).Dot(N) * N;
		go1->vel.y *= 0.4;
	}
}

void SceneCollision::SpawnBall()
{
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	double x, y;
	Application::GetCursorPos(&x, &y);
	//Mouse Section
	static bool bLButtonState = false;

	// Spawn ball upon mouse click
	if (m_ballCount == 0)
	{
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;
			initPos = true;
			m_ghost->active = false;
			spawnGhost = true;
			start = false;
			m_ballSpawned = 0;
			if (initPos)
			{
				oldx = x;
				oldy = y;
				initPos = false;
			}
			if (addball)
			{
				m_ballMax++;
				addball = false;
			}
		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			bLButtonState = false;
		}
	}

	// Delay between balls
	if (ball_spawnrate > 0.5f && m_ballSpawned < m_ballMax)
	{
		GameObject* newGO = FetchGO();
		newGO->active = true;
		newGO->type = GameObject::GO_BALL;
		newGO->scale.Set(2, 2, 2);
		newGO->pos = Vector3(m_worldWidth * 0.5, m_worldHeight - 8, 0);
		newGO->vel = Vector3((oldx / w * m_worldWidth) - newGO->pos.x, ((h - oldy) / h * m_worldHeight) - newGO->pos.y, 0).Normalize() * 50;
		newGO->mass = 8;
		ball_spawnrate = 0;
		m_ballSpawned++;
		m_ballCount++;
	}
	// Re-enable ball displayed at top when finished
	else if (m_ballSpawned == m_ballMax && m_ballCount <= 0 && spawnGhost)
	{
		m_ghost = FetchGO();
		m_ghost->type = GameObject::GO_GHOSTBALL;
		m_ghost->active = true;
		m_ghost->scale.Set(2, 2, 1);
		m_ghost->normal.Set(1, 0, 0);
		m_ghost->pos = Vector3(m_worldWidth * 0.5, m_worldHeight - 8, 0);
		spawnGhost = false;

		// Initialize starting pos for moving shapes up
		if (!start)
		{
			moveShape = true;
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->active)
				{
					if (go->type == GameObject::GO_CIRCLE)
					{
						go->initPos = true;
					}
					if (go->type == GameObject::GO_WALL)
					{
						go->initPos = true;
					}
					if (go->type == GameObject::GO_POWERUP)
					{
						go->initPos = true;
					}
				}
			}
		}
	}


}

void SceneCollision::createCube()
{
	// Cube creation
	GameObject* newGO = FetchGO();
	newGO->type = GameObject::GO_WALL;
	newGO->active = true;
	newGO->scale.Set(3, 3, 1);
	newGO->normal.Set(1, 0, 0);
	int hpMultiplier = Math::RandIntMinMax(1, m_ballMax);
	int randHp = Math::RandIntMinMax(0 + m_ballMax, 2 + m_ballMax * hpMultiplier);
	newGO->hp = randHp;

	GameObject* newGO2 = FetchGO();
	newGO2->type = GameObject::GO_WALL;
	newGO2->active = true;
	newGO2->scale.Set(3, 3, 1);
	newGO2->normal.Set(0, 1, 0);
	newGO2->hp = newGO2->hp;

	newGO->pillar1 = FetchGO();
	newGO->pillar1->type = GameObject::GO_PILLAR;
	newGO->pillar1->active = true;
	newGO->pillar1->scale.Set(1.f, 1.f, 1.f);
	newGO->pillar1->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f + 10, 1);

	newGO->pillar2 = FetchGO();
	newGO->pillar2->type = GameObject::GO_PILLAR;
	newGO->pillar2->active = true;
	newGO->pillar2->scale.Set(1.f, 1.f, 1.f);
	newGO->pillar2->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f + 10, 1);

	newGO->pillar3 = FetchGO();
	newGO->pillar3->type = GameObject::GO_PILLAR;
	newGO->pillar3->active = true;
	newGO->pillar3->scale.Set(1.f, 1.f, 1.f);
	newGO->pillar3->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f - 10, 1);

	newGO->pillar4 = FetchGO();
	newGO->pillar4->type = GameObject::GO_PILLAR;
	newGO->pillar4->active = true;
	newGO->pillar4->scale.Set(1.f, 1.f, 1.f);
	newGO->pillar4->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f - 5, 1);

	newGO2->pillar1 = FetchGO();
	newGO2->pillar1->type = GameObject::GO_PILLAR;
	newGO2->pillar1->active = true;
	newGO2->pillar1->scale.Set(1.f, 1.f, 1.f);
	newGO2->pillar1->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f + 10, 1);

	newGO2->pillar2 = FetchGO();
	newGO2->pillar2->type = GameObject::GO_PILLAR;
	newGO2->pillar2->active = true;
	newGO2->pillar2->scale.Set(1.f, 1.f, 1.f);
	newGO2->pillar2->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f + 10, 1);

	newGO2->pillar3 = FetchGO();
	newGO2->pillar3->type = GameObject::GO_PILLAR;
	newGO2->pillar3->active = true;
	newGO2->pillar3->scale.Set(1.f, 1.f, 1.f);
	newGO2->pillar3->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f - 10, 1);

	newGO2->pillar4 = FetchGO();
	newGO2->pillar4->type = GameObject::GO_PILLAR;
	newGO2->pillar4->active = true;
	newGO2->pillar4->scale.Set(1.f, 1.f, 1.f);
	newGO2->pillar4->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f - 5, 1);
}

void SceneCollision::createPillars(GameObject* go1, GameObject* go2)
{
	// Pillar creation
	go1->pillar1 = FetchGO();
	go1->pillar1->type = GameObject::GO_PILLAR;
	go1->pillar1->active = true;
	go1->pillar1->scale.Set(0.01f, 0.01f, 0.01f);
	go1->pillar1->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f + 10, 1);

	go1->pillar2 = FetchGO();
	go1->pillar2->type = GameObject::GO_PILLAR;
	go1->pillar2->active = true;
	go1->pillar2->scale.Set(0.01f, 0.01f, 0.01f);
	go1->pillar2->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f + 10, 1);

	go1->pillar3 = FetchGO();
	go1->pillar3->type = GameObject::GO_PILLAR;
	go1->pillar3->active = true;
	go1->pillar3->scale.Set(0.01f, 0.01f, 0.01f);
	go1->pillar3->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f - 10, 1);

	go1->pillar4 = FetchGO();
	go1->pillar4->type = GameObject::GO_PILLAR;
	go1->pillar4->active = true;
	go1->pillar4->scale.Set(0.01f, 0.01f, 0.01f);
	go1->pillar4->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f - 5, 1);

	go2->pillar1 = FetchGO();
	go2->pillar1->type = GameObject::GO_PILLAR;
	go2->pillar1->active = true;
	go2->pillar1->scale.Set(0.01f, 0.01f, 0.01f);
	go2->pillar1->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f + 10, 1);

	go2->pillar2 = FetchGO();
	go2->pillar2->type = GameObject::GO_PILLAR;
	go2->pillar2->active = true;
	go2->pillar2->scale.Set(0.01f, 0.01f, 0.01f);
	go2->pillar2->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f + 10, 1);

	go2->pillar3 = FetchGO();
	go2->pillar3->type = GameObject::GO_PILLAR;
	go2->pillar3->active = true;
	go2->pillar3->scale.Set(0.01f, 0.01f, 0.01f);
	go2->pillar3->pos = Vector3(m_worldWidth * 0.5f + 10, m_worldHeight * 0.5f - 10, 1);

	go2->pillar4 = FetchGO();
	go2->pillar4->type = GameObject::GO_PILLAR;
	go2->pillar4->active = true;
	go2->pillar4->scale.Set(0.01f, 0.01f, 0.01f);
	go2->pillar4->pos = Vector3(m_worldWidth * 0.5f - 10, m_worldHeight * 0.5f - 5, 1);
}

void SceneCollision::spawnShapes()
{
	// Spawning shapes
	if (spawnShape)
	{
		// Number of objects to spawn rng (1 - 3)
		int numberOfObjects = Math::RandIntMinMax(1, 3);
		if (numberOfObjects == 1)
		{
			// choice of shape (circle or square)
			int choiceOfShape = Math::RandIntMinMax(1, 2);
			if (choiceOfShape == 1)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				// check if criteria met for spawning powerup (intervals of 10)
				if (points < points_ceiling || points >= 50 && m_ballMax == 5)
				{
					newGO->type = GameObject::GO_CIRCLE;
					newGO->scale.Set(3, 3, 1);
				}
				else
				{
					newGO->type = GameObject::GO_POWERUP;
					newGO->scale.Set(5.f, 5.f, 1);
					points_ceiling += 10;
				}
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				// Position spawning rng
				int positioning = Math::RandIntMinMax(1, 2);
				if (positioning == 1)
				{
					int randomPos = Math::RandIntMinMax(0, 4);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 8.f, -5, 0);
					else if (randomPos == 2)
						newGO->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
					else if (randomPos == 3)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 8.f, -5, 0);
					else if (randomPos == 4)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

					cout << "position 1 single circle triggered" << endl;
				}
				else if (positioning == 2)
				{
					int randomPos = Math::RandIntMinMax(0, 4);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 13.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 4.f, -5, 0);
					else if (randomPos == 2)
						newGO->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
					else if (randomPos == 3)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 4.f, -5, 0);
					else if (randomPos == 4)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 13.f, -5, 0);

					cout << "position 2 single circle triggered" << endl;
				}

			}
			else if (choiceOfShape == 2)
			{
				GameObject* newGO = FetchGO();
				newGO->type = GameObject::GO_WALL;
				newGO->active = true;
				newGO->scale.Set(3, 3, 1);
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->type = GameObject::GO_WALL;
				newGO2->active = true;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(0, 1, 0);
				newGO2->hp = newGO->hp;

				createPillars(newGO, newGO2);

				int positioning = Math::RandIntMinMax(1, 2);
				if (positioning == 1)
				{
					int randomPos = Math::RandIntMinMax(0, 4);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 8.f, -5, 0);
					else if (randomPos == 2)
						newGO->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
					else if (randomPos == 3)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 8.f, -5, 0);
					else if (randomPos == 4)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

					cout << "position 1 single square triggered" << endl;
				}
				else if (positioning == 2)
				{
					int randomPos = Math::RandIntMinMax(0, 4);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 13.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 4.f, -5, 0);
					else if (randomPos == 2)
						newGO->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
					else if (randomPos == 3)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 4.f, -5, 0);
					else if (randomPos == 4)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 13.f, -5, 0);

					cout << "position 2 single square triggered" << endl;
				}
				newGO2->pos = newGO->pos;
			}

		}
		else if (numberOfObjects == 2)
		{

			int choiceOfShape = Math::RandIntMinMax(1, 3);

			if (choiceOfShape == 1)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				if (points < points_ceiling || points >= 50 && m_ballMax >= 5)
				{
					newGO->type = GameObject::GO_CIRCLE;
					newGO->scale.Set(3, 3, 1);
				}
				else
				{
					newGO->type = GameObject::GO_POWERUP;
					newGO->scale.Set(5.f, 5.f, 1);
					points_ceiling += 10;
				}
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->active = true;
				newGO2->type = GameObject::GO_CIRCLE;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(1, 0, 0);
				int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
				newGO2->hp = randHp2;

				int positioning = Math::RandIntMinMax(1, 2);
				if (positioning == 1)
				{
					int randomPos = Math::RandIntMinMax(0, 1);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 8.f, -5, 0);

					int randomPos2 = Math::RandIntMinMax(0, 1);
					if (randomPos2 == 0)
						newGO2->pos = Vector3(m_worldWidth * 0.5f - 8.f, -5, 0);
					else if (randomPos2 == 1)
						newGO2->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

					cout << "position 1 double triggered" << endl;
				}
				else if (positioning == 2)
				{
					int randomPos = Math::RandIntMinMax(0, 1);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 13.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 4.f, -5, 0);

					int randomPos2 = Math::RandIntMinMax(0, 1);
					if (randomPos2 == 0)
						newGO2->pos = Vector3(m_worldWidth * 0.5f - 4.f, -5, 0);
					else if (randomPos2 == 1)
						newGO2->pos = Vector3(m_worldWidth * 0.5f + 13.f, -5, 0);

					cout << "position 2 double triggered" << endl;
				}
			}
			else if (choiceOfShape == 2)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				if (points < points_ceiling || points >= 50 && m_ballMax == 5)
				{
					newGO->type = GameObject::GO_CIRCLE;
					newGO->scale.Set(3, 3, 1);
				}
				else
				{
					newGO->type = GameObject::GO_POWERUP;
					newGO->scale.Set(5.f, 5.f, 1);
					points_ceiling += 10;
				}
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->type = GameObject::GO_WALL;
				newGO2->active = true;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(1, 0, 0);
				int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
				newGO2->hp = randHp2;

				GameObject* newGO3 = FetchGO();
				newGO3->type = GameObject::GO_WALL;
				newGO3->active = true;
				newGO3->scale.Set(3, 3, 1);
				newGO3->normal.Set(0, 1, 0);
				newGO3->hp = newGO2->hp;

				createPillars(newGO2, newGO3);

				int positioning = Math::RandIntMinMax(1, 2);
				if (positioning == 1)
				{
					int randomPos = Math::RandIntMinMax(0, 2);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 8.f, -5, 0);

					int randomPos2 = Math::RandIntMinMax(0, 1);
					if (randomPos2 == 0)
						newGO2->pos = Vector3(m_worldWidth * 0.5f - 8.f, -5, 0);
					else if (randomPos2 == 1)
						newGO2->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

					cout << "position 1 double triggered" << endl;
				}
				else if (positioning == 2)
				{
					int randomPos = Math::RandIntMinMax(0, 1);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 13.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 4.f, -5, 0);

					int randomPos2 = Math::RandIntMinMax(0, 1);
					if (randomPos2 == 0)
						newGO2->pos = Vector3(m_worldWidth * 0.5f - 4.f, -5, 0);
					else if (randomPos2 == 1)
						newGO2->pos = Vector3(m_worldWidth * 0.5f + 13.f, -5, 0);

					cout << "position 2 double triggered" << endl;
				}
				newGO3->pos = newGO2->pos;
			}
			else if (choiceOfShape == 3)
			{
				GameObject* newGO = FetchGO();
				newGO->type = GameObject::GO_WALL;
				newGO->active = true;
				newGO->scale.Set(3, 3, 1);
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->type = GameObject::GO_WALL;
				newGO2->active = true;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(0, 1, 0);
				newGO2->hp = newGO->hp;

				createPillars(newGO, newGO2);

				int positioning = Math::RandIntMinMax(1, 2);
				if (positioning == 1)
				{
					int randomPos = Math::RandIntMinMax(0, 1);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 8.f, -5, 0);

					newGO2->pos = newGO->pos;

					GameObject* newGO3 = FetchGO();
					newGO3->type = GameObject::GO_WALL;
					newGO3->active = true;
					newGO3->scale.Set(3, 3, 1);
					newGO3->normal.Set(1, 0, 0);
					int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
					int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
					newGO3->hp = randHp2;

					GameObject* newGO4 = FetchGO();
					newGO4->type = GameObject::GO_WALL;
					newGO4->active = true;
					newGO4->scale.Set(3, 3, 1);
					newGO4->normal.Set(0, 1, 0);
					newGO4->hp = newGO3->hp;

					createPillars(newGO3, newGO4);

					int randomPos2 = Math::RandIntMinMax(0, 1);
					if (randomPos2 == 0)
						newGO3->pos = Vector3(m_worldWidth * 0.5f - 8.f, -5, 0);
					else if (randomPos2 == 1)
						newGO3->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

					newGO4->pos = newGO3->pos;
					cout << "position 1 double triggered" << endl;
				}
				else if (positioning == 2)
				{
					int randomPos = Math::RandIntMinMax(0, 1);
					if (randomPos == 0)
						newGO->pos = Vector3(m_worldWidth * 0.5f - 13.f, -5, 0);
					else if (randomPos == 1)
						newGO->pos = Vector3(m_worldWidth * 0.5f + 4.f, -5, 0);

					newGO2->pos = newGO->pos;

					GameObject* newGO3 = FetchGO();
					newGO3->type = GameObject::GO_WALL;
					newGO3->active = true;
					newGO3->scale.Set(3, 3, 1);
					newGO3->normal.Set(1, 0, 0);
					int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
					int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
					newGO3->hp = randHp2;

					GameObject* newGO4 = FetchGO();
					newGO4->type = GameObject::GO_WALL;
					newGO4->active = true;
					newGO4->scale.Set(3, 3, 1);
					newGO4->normal.Set(0, 1, 0);
					newGO4->hp = newGO3->hp;

					createPillars(newGO3, newGO4);

					int randomPos2 = Math::RandIntMinMax(0, 1);
					if (randomPos2 == 0)
						newGO3->pos = Vector3(m_worldWidth * 0.5f - 4.f, -5, 0);
					else if (randomPos2 == 1)
						newGO3->pos = Vector3(m_worldWidth * 0.5f + 13.f, -5, 0);

					newGO4->pos = newGO3->pos;
					cout << "position 2 double triggered" << endl;
				}

			}

		}
		else if (numberOfObjects == 3)
		{
			int choiceOfShape = Math::RandIntMinMax(1, 4);

			if (choiceOfShape == 1)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				if (points < points_ceiling || points >= 50 && m_ballMax == 5)
				{
					newGO->type = GameObject::GO_CIRCLE;
					newGO->scale.Set(3, 3, 1);
				}
				else
				{
					newGO->type = GameObject::GO_POWERUP;
					newGO->scale.Set(5.f, 5.f, 1);
					points_ceiling += 10;
				}
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->active = true;
				newGO2->type = GameObject::GO_CIRCLE;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(1, 0, 0);
				int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
				newGO2->hp = randHp2;

				GameObject* newGO3 = FetchGO();
				newGO3->active = true;
				newGO3->type = GameObject::GO_CIRCLE;
				newGO3->scale.Set(3, 3, 1);
				newGO3->normal.Set(1, 0, 0);
				int hpMultiplier3 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp3 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier3, 3 + m_ballMax * hpMultiplier3);
				newGO3->hp = randHp3;

				int randomPos = Math::RandIntMinMax(0, 1);
				if (randomPos == 0)
					newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
				else if (randomPos == 1)
					newGO->pos = Vector3(m_worldWidth * 0.5f + 9.f, -5, 0);

				int randomPos2 = Math::RandIntMinMax(0, 1);
				if (randomPos2 == 0)
					newGO2->pos = Vector3(m_worldWidth * 0.5f - 9.f, -5, 0);
				else if (randomPos2 == 1)
					newGO2->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

				newGO3->pos = Vector3(m_worldWidth * 0.5f, -5, 0);

				cout << "position triple triggered" << endl;
			}
			else if (choiceOfShape == 2)
			{
				GameObject* newGO = FetchGO();
				newGO->active = true;
				if (points < points_ceiling || points >= 50 && m_ballMax == 5)
				{
					newGO->type = GameObject::GO_CIRCLE;
					newGO->scale.Set(3, 3, 1);
				}
				else
				{
					newGO->type = GameObject::GO_POWERUP;
					newGO->scale.Set(5.f, 5.f, 1);
					points_ceiling += 10;
				}
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->type = GameObject::GO_WALL;
				newGO2->active = true;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(1, 0, 0);
				int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
				newGO2->hp = randHp2;

				GameObject* newGO3 = FetchGO();
				newGO3->type = GameObject::GO_WALL;
				newGO3->active = true;
				newGO3->scale.Set(3, 3, 1);
				newGO3->normal.Set(0, 1, 0);
				newGO3->hp = newGO2->hp;

				createPillars(newGO2, newGO3);

				GameObject* newGO4 = FetchGO();
				newGO4->active = true;
				newGO4->type = GameObject::GO_CIRCLE;
				newGO4->scale.Set(3, 3, 1);
				newGO4->normal.Set(1, 0, 0);
				int hpMultiplier3 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp3 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier3, 3 + m_ballMax * hpMultiplier3);
				newGO4->hp = randHp3;

				int randomPos = Math::RandIntMinMax(0, 1);
				if (randomPos == 0)
					newGO2->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
				else if (randomPos == 1)
					newGO2->pos = Vector3(m_worldWidth * 0.5f + 9.f, -5, 0);

				int randomPos2 = Math::RandIntMinMax(0, 1);
				if (randomPos2 == 0)
					newGO->pos = Vector3(m_worldWidth * 0.5f - 9.f, -5, 0);
				else if (randomPos2 == 1)
					newGO->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

				newGO4->pos = Vector3(m_worldWidth * 0.5f, -5, 0);

				newGO3->pos = newGO2->pos;
				cout << "position triple triggered" << endl;
			}
			else if (choiceOfShape == 3)
			{
				GameObject* newGO = FetchGO();
				newGO->type = GameObject::GO_WALL;
				newGO->active = true;
				newGO->scale.Set(3, 3, 1);
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->type = GameObject::GO_WALL;
				newGO2->active = true;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(0, 1, 0);
				newGO2->hp = newGO->hp;

				createPillars(newGO, newGO2);

				GameObject* newGO3 = FetchGO();
				newGO3->active = true;
				if (points < points_ceiling || points >= 50 && m_ballMax == 5)
				{
					newGO3->type = GameObject::GO_CIRCLE;
					newGO3->scale.Set(3, 3, 1);
				}
				else
				{
					newGO3->type = GameObject::GO_POWERUP;
					newGO3->scale.Set(5.f, 5.f, 1);
					points_ceiling += 10;
				}
				newGO3->normal.Set(1, 0, 0);
				int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
				newGO3->hp = randHp2;

				int randomPos = Math::RandIntMinMax(0, 1);
				if (randomPos == 0)
					newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
				else if (randomPos == 1)
					newGO->pos = Vector3(m_worldWidth * 0.5f + 9.f, -5, 0);

				newGO2->pos = newGO->pos;

				GameObject* newGO4 = FetchGO();
				newGO4->type = GameObject::GO_WALL;
				newGO4->active = true;
				newGO4->scale.Set(3, 3, 1);
				newGO4->normal.Set(1, 0, 0);
				int hpMultiplier3 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp3 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier3, 3 + m_ballMax * hpMultiplier3);
				newGO4->hp = randHp3;

				GameObject* newGO5 = FetchGO();
				newGO5->type = GameObject::GO_WALL;
				newGO5->active = true;
				newGO5->scale.Set(3, 3, 1);
				newGO5->normal.Set(0, 1, 0);
				newGO5->hp = newGO4->hp;

				createPillars(newGO4, newGO5);

				int randomPos2 = Math::RandIntMinMax(0, 1);
				if (randomPos2 == 0)
					newGO4->pos = Vector3(m_worldWidth * 0.5f - 9.f, -5, 0);
				else if (randomPos2 == 1)
					newGO4->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

				newGO3->pos = Vector3(m_worldWidth * 0.5f, -5, 0);

				newGO5->pos = newGO4->pos;
				cout << "position triple triggered" << endl;
			}
			else if (choiceOfShape == 4)
			{
				GameObject* newGO = FetchGO();
				newGO->type = GameObject::GO_WALL;
				newGO->active = true;
				newGO->scale.Set(3, 3, 1);
				newGO->normal.Set(1, 0, 0);
				int hpMultiplier = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier, 3 + m_ballMax * hpMultiplier);
				newGO->hp = randHp;

				GameObject* newGO2 = FetchGO();
				newGO2->type = GameObject::GO_WALL;
				newGO2->active = true;
				newGO2->scale.Set(3, 3, 1);
				newGO2->normal.Set(0, 1, 0);
				newGO2->hp = newGO->hp;

				createPillars(newGO, newGO2);

				int randomPos = Math::RandIntMinMax(0, 1);
				if (randomPos == 0)
					newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
				else if (randomPos == 1)
					newGO->pos = Vector3(m_worldWidth * 0.5f + 9.f, -5, 0);

				newGO2->pos = newGO->pos;

				GameObject* newGO3 = FetchGO();
				newGO3->type = GameObject::GO_WALL;
				newGO3->active = true;
				newGO3->scale.Set(3, 3, 1);
				newGO3->normal.Set(1, 0, 0);
				int hpMultiplier2 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp2 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier2, 3 + m_ballMax * hpMultiplier2);
				newGO3->hp = randHp2;

				GameObject* newGO4 = FetchGO();
				newGO4->type = GameObject::GO_WALL;
				newGO4->active = true;
				newGO4->scale.Set(3, 3, 1);
				newGO4->normal.Set(0, 1, 0);
				newGO4->hp = newGO3->hp;

				createPillars(newGO3, newGO4);

				int randomPos2 = Math::RandIntMinMax(0, 1);
				if (randomPos2 == 0)
					newGO3->pos = Vector3(m_worldWidth * 0.5f - 9.f, -5, 0);
				else if (randomPos2 == 1)
					newGO3->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

				newGO4->pos = newGO3->pos;

				GameObject* newGO5 = FetchGO();
				newGO5->type = GameObject::GO_WALL;
				newGO5->active = true;
				newGO5->scale.Set(3, 3, 1);
				newGO5->normal.Set(1, 0, 0);
				int hpMultiplier3 = Math::RandIntMinMax(1, m_ballMax + 5);
				int randHp3 = Math::RandIntMinMax(0 + m_ballMax * hpMultiplier3, 3 + m_ballMax * hpMultiplier3);
				newGO5->hp = randHp3;

				GameObject* newGO6 = FetchGO();
				newGO6->type = GameObject::GO_WALL;
				newGO6->active = true;
				newGO6->scale.Set(3, 3, 1);
				newGO6->normal.Set(0, 1, 0);
				newGO6->hp = newGO5->hp;

				createPillars(newGO5, newGO6);

				newGO5->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
				newGO6->pos = newGO5->pos;
				cout << "position triple triggered" << endl;
			}
		}
		spawnShape = false;
	}

}

void SceneCollision::initShapes()
{
	// Initialize shapes at beginning of game
	if (init_game)
	{
		int positioning = Math::RandIntMinMax(1, 2);
		if (positioning == 1)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_CIRCLE;
			newGO->scale.Set(3, 3, 1);
			newGO->normal.Set(1, 0, 0);
			newGO->hp = 1;

			int randomPos = Math::RandIntMinMax(0, 4);
			if (randomPos == 0)
				newGO->pos = Vector3(m_worldWidth * 0.5f - 13.f, 5, 0);
			else if (randomPos == 1)
				newGO->pos = Vector3(m_worldWidth * 0.5f - 4.f, 5, 0);
			else if (randomPos == 2)
				newGO->pos = Vector3(m_worldWidth * 0.5f, 5, 0);
			else if (randomPos == 3)
				newGO->pos = Vector3(m_worldWidth * 0.5f + 4.f, 5, 0);
			else if (randomPos == 4)
				newGO->pos = Vector3(m_worldWidth * 0.5f + 13.f, 5, 0);


			GameObject* newGO2 = FetchGO();
			newGO2->active = true;
			newGO2->type = GameObject::GO_CIRCLE;
			newGO2->scale.Set(3, 3, 1);
			newGO2->normal.Set(1, 0, 0);
			newGO2->hp = 2;

			int randomPos2 = Math::RandIntMinMax(0, 4);
			if (randomPos2 == 0)
				newGO2->pos = Vector3(m_worldWidth * 0.5f - 13.f, -5, 0);
			else if (randomPos2 == 1)
				newGO2->pos = Vector3(m_worldWidth * 0.5f - 4.f, -5, 0);
			else if (randomPos2 == 2)
				newGO2->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
			else if (randomPos2 == 3)
				newGO2->pos = Vector3(m_worldWidth * 0.5f + 4.f, -5, 0);
			else if (randomPos2 == 4)
				newGO2->pos = Vector3(m_worldWidth * 0.5f + 13.f, -5, 0);

			cout << "position 1 triggered" << endl;
		}
		else if (positioning == 2)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_CIRCLE;
			newGO->scale.Set(3, 3, 1);
			newGO->normal.Set(1, 0, 0);
			newGO->hp = 1;

			int randomPos = Math::RandIntMinMax(0, 4);
			if (randomPos == 0)
				newGO->pos = Vector3(m_worldWidth * 0.5f - 18.f, 5, 0);
			else if (randomPos == 1)
				newGO->pos = Vector3(m_worldWidth * 0.5f - 8.f, 5, 0);
			else if (randomPos == 2)
				newGO->pos = Vector3(m_worldWidth * 0.5f, 5, 0);
			else if (randomPos == 3)
				newGO->pos = Vector3(m_worldWidth * 0.5f + 8.f, 5, 0);
			else if (randomPos == 4)
				newGO->pos = Vector3(m_worldWidth * 0.5f + 18.f, 5, 0);

			GameObject* newGO2 = FetchGO();
			newGO2->active = true;
			newGO2->type = GameObject::GO_CIRCLE;
			newGO2->scale.Set(3, 3, 1);
			newGO2->normal.Set(1, 0, 0);
			newGO2->hp = 2;

			int randomPos2 = Math::RandIntMinMax(0, 4);
			if (randomPos2 == 0)
				newGO2->pos = Vector3(m_worldWidth * 0.5f - 18.f, -5, 0);
			else if (randomPos2 == 1)
				newGO2->pos = Vector3(m_worldWidth * 0.5f - 8.f, -5, 0);
			else if (randomPos2 == 2)
				newGO2->pos = Vector3(m_worldWidth * 0.5f, -5, 0);
			else if (randomPos2 == 3)
				newGO2->pos = Vector3(m_worldWidth * 0.5f + 8.f, -5, 0);
			else if (randomPos2 == 4)
				newGO2->pos = Vector3(m_worldWidth * 0.5f + 18.f, -5, 0);

			cout << "position 2 triggered" << endl;
		}
		init_game = false;
	}
}

void SceneCollision::moveShapes(double dt)
{
	// Moving objects up
	if (moveShape)
	{
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_CIRCLE)
				{
					if (go->initPos)
					{
						go->prevpos = go->pos.y + 10;
						go->initPos = false;
					}
					else
					{
						if (go->pos.y < go->prevpos)
						{
							go->pos.y += 10 * dt;
						}
						else
						{
							moveShape = false;
							spawnShape = true;
						}
					}
				}

				if (go->type == GameObject::GO_WALL)
				{
					if (go->initPos)
					{
						go->prevpos = go->pos.y + 10;
						go->initPos = false;
					}
					else
					{
						if (go->pos.y < go->prevpos)
						{
							go->pos.y += 10 * dt;
						}
						else
						{
							moveShape = false;
							spawnShape = true;
						}
					}
				}
				if (go->type == GameObject::GO_POWERUP)
				{
					if (go->initPos)
					{
						go->prevpos = go->pos.y + 10;
						go->initPos = false;
					}
					else
					{
						if (go->pos.y < go->prevpos)
						{
							go->pos.y += 10 * dt;
						}
						else
						{
							moveShape = false;
							spawnShape = true;
						}
					}
				}

			}
		}
	}
}

void SceneCollision::Update(double dt)
{
	// Update timers
	interval += dt;
	ball_spawnrate += dt;

	SceneBase::Update(dt);
	SpawnBall();

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
					if (go->type == GameObject::GO_CIRCLE || go->type == GameObject::GO_WALL || go->type == GameObject::GO_PILLAR || go->type == GameObject::GO_POWERUP)
					{
						go->active = false;
					}
				}
			}

			// Reset variables
			init_game = true;
			moveShape = false;
			spawnShape = false;
			addball = false;
			m_ballMax = 1;
			m_ballLabel = 1;
			points = 0;
			points_ceiling = 10;
			state = STATE_PLAY;
		}
	}
	// Play state
	else if (state == STATE_PLAY)
	{
		initShapes();
		moveShapes(dt);
		spawnShapes();

		if (Application::IsKeyPressed('9'))
			m_speed = Math::Max(0.f, m_speed - 0.1f);

		if (Application::IsKeyPressed('0') && interval > 1)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_POWERUP;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
			interval = 0;
			cout << "powerup spawned" << endl;
		}

		if (m_ballMax > 5)
		{
			m_ballMax = 5;
			m_ballLabel = 5;
		}
		
		//Physics Simulation Section

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				go->animation_delay += dt;

				if (go->type == GameObject::GO_WALL)
				{
					// Despawn cube upon leaving boundary
					if (go->pos.y + go->scale.y > m_worldHeight)
					{
						state = STATE_LOSE;
					}

					// Play popping animation
					if (go->animation_delay <= 0.6f)
					{
						if (go->animation_delay < 0.1f)
						{
							go->scale.Set(3.5f, 3.5f, 1);
						}
						else if (go->animation_delay >= 0.1f && go->animation_delay < 0.5f)
						{
							go->scale.x -= 0.02083333f;
							go->scale.y -= 0.02083333f;
						}
						else
						{
							go->scale.Set(3, 3, 1);
						}
					}

					// Set pillar positions with cube

					Vector3 N = go->normal;
					Vector3 NP(N.y, -N.x, 0);
					Vector3 right = go->normal.Cross(Vector3(0, 0, -1));

					go->normal.Set(go->normal.x * cosf(0.01f) - go->normal.y * sinf(0.01f), go->normal.x * sinf(0.01f) + go->normal.y * cosf(0.01f));
					go->pillar1->pos = go->pos + N * go->scale.x;
					go->pillar1->pos -= NP * go->scale.y;

					go->pillar2->pos = go->pos - N * go->scale.x;
					go->pillar2->pos -= NP * go->scale.y;

					go->pillar3->pos = go->pos + N * go->scale.x;
					go->pillar3->pos -= -NP * go->scale.y;

					go->pillar4->pos = go->pos + -N * go->scale.x;
					go->pillar4->pos -= -NP * go->scale.y;

					// Destroy cube if hp <= 0
					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{
							if (go2->type == GameObject::GO_WALL)
							{
								if (go2->pos == go->pos)
								{
									if (go->hp + go2->hp <= 0)
									{
										go->pillar1->active = false;
										go->pillar2->active = false;
										go->pillar3->active = false;
										go->pillar4->active = false;
										go2->pillar1->active = false;
										go2->pillar2->active = false;
										go2->pillar3->active = false;
										go2->pillar4->active = false;
										go->active = false;
										go2->active = false;
										points++;
									}
								}

							}
						}
					}
				}

				if (go->type == GameObject::GO_BALL)
				{
					go->pos += go->vel * dt * m_speed;

					go->vel.y -= dt * 100;

					// Despawn ball upon leaving boundary
					if (go->pos.x + go->scale.x > m_worldWidth && go->vel.x > 0 ||
						go->pos.x - go->scale.x < 0 && go->vel.x < 0) {
						go->active = false;
						m_ballCount--;
					}
					if (go->pos.y + go->scale.y > m_worldHeight && go->vel.y > 0 ||
						go->pos.y - go->scale.y < 0 && go->vel.y < 0) {
						go->vel.y *= 0.7;
						go->active = false;
						m_ballCount--;
					}

					if ((go->pos.x > m_worldWidth + go->scale.x || go->pos.x < 0 - go->scale.x) ||
						(go->pos.y > m_worldHeight + go->scale.y || go->pos.y < 0 - go->scale.y))
					{
						ReturnGO(go);
						break;
					}
				}
				if (go->type == GameObject::GO_CIRCLE)
				{
					// Lose condition if object exits through the top
					if (go->pos.y + go->scale.y > m_worldHeight)
					{
						state = STATE_LOSE;
					}

					// Play popping animation
					if (go->animation_delay <= 0.6f)
					{
						if (go->animation_delay < 0.1f)
						{
							go->scale.Set(3.5f, 3.5f, 1);
						}
						else if (go->animation_delay >= 0.1f && go->animation_delay < 0.5f)
						{
							go->scale.x -= 0.02083333f;
							go->scale.y -= 0.02083333f;
						}
						else
						{
							go->scale.Set(3, 3, 1);
						}
					}
					if (go->hp <= 0)
					{
						go->active = false;
						points++;
					}
				}

				// Handle collision between GO_BALL and GO_BALL using velocity swap
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
						if (CheckCollision(ball, other, dt))
						{
							CollisionResponse(ball, other);
							continue;
						}
					}
				}
			}
		}
	}
}


void SceneCollision::RenderGO(GameObject *go)
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

	
	}
	glEnable(GL_DEPTH_TEST);
}

void SceneCollision::Render()
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
		// Displaying colored balls
		modelStack.PushMatrix();
		modelStack.Translate(8, 67, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_YELLOWBALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, 58, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_ORANGEBALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, 49, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_REDBALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, 40, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_GREENBALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, 31, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_CYANBALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, 22, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_BLUEBALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(8, 13, 0.3f);
		modelStack.Scale(2.3f, 2.3f, 1);
		RenderMesh(meshList[GEO_PURPLEBALL], false);
		modelStack.PopMatrix();

		// In game background
		modelStack.PushMatrix();
		modelStack.Translate(80, 50, 0.1f);
		modelStack.Scale(80, 50, 1);
		RenderMesh(meshList[GEO_HEXBG], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(80, 50, 0.2f);
		modelStack.Scale(25, 50, 1);
		RenderMesh(meshList[GEO_GREYBG], false);
		modelStack.PopMatrix();

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				RenderGO(go);
			}
		}
		//On screen in - game text

		std::ostringstream ss6;
		ss6.precision(0);
		ss6 << points;
		RenderTextOnScreen(meshList[GEO_TEXT], ss6.str(), Color(1, 1, 0), 3.f, 39.5f, 57);

		// Controls etc..
		RenderTextOnScreen(meshList[GEO_TEXT], "Controls:", Color(1, 0, 0), 3.5f, 1.6f, 55);

		RenderTextOnScreen(meshList[GEO_TEXT], "Aim cursor on screen", Color(0, 1, 0), 3.5f, 1.6f, 51);

		RenderTextOnScreen(meshList[GEO_TEXT], "M1 to shoot", Color(0, 1, 0), 3.5f, 1.6f, 47);

		// Display balls player has
		std::ostringstream ss3;
		ss3.precision(0);
		ss3 << "Balls available: " << m_ballLabel;
		RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(0, 1, 0), 3.5f, 1.6f, 43);

		// Display health range for each color
		RenderTextOnScreen(meshList[GEO_TEXT], ": 1 <= HP < 5", Color(1, 1, 0), 2.5f, 5.5f, 39);

		RenderTextOnScreen(meshList[GEO_TEXT], ": 5 <= HP < 10", Color(1, 0.647f, 0), 2.5f, 5.5f, 34);

		RenderTextOnScreen(meshList[GEO_TEXT], ": 10 <= HP < 15", Color(1, 0, 0), 2.5f, 5.5f, 28);

		RenderTextOnScreen(meshList[GEO_TEXT], ": 15 <= HP < 20", Color(0, 1, 0), 2.5f, 5.5f, 23);

		RenderTextOnScreen(meshList[GEO_TEXT], ": 20 <= HP < 25", Color(0, 1, 1), 2.5f, 5.5f, 17.5f);

		RenderTextOnScreen(meshList[GEO_TEXT], ": 25 <= HP < 30", Color(0, 0, 1), 2.5f, 5.5f, 12);

		RenderTextOnScreen(meshList[GEO_TEXT], ": 30 <= HP", Color(1.f, 0.000, 1.f), 2.5f, 5.5f, 7);

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

void SceneCollision::Exit()
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
