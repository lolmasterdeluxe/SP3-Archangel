#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
using namespace std;

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	// Set screenspace and worldspace
	m_screenHeight = 100.f;
	m_screenWidth = m_screenHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	m_worldHeight = 200.f;
	m_worldWidth = 200.f;
	state = STATE_MENU;
	
	Math::InitRNG();

	//Exercise 2a: Construct 150 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i < 150; i++)
	{
		GameObject* asteroid = new GameObject(GameObject::GO_ASTEROID);
		m_goList.push_back(asteroid);
	}

	menu_delay = 0;
	m_ship = new GameObject(GameObject::GO_SHIP);

	Reset();

}

GameObject* SceneAsteroid::FetchGO()
{

	// Fetch a game object from m_goList and return it 
	for (auto go : m_goList)
	{
		if (!go->active)
		{
			go->active = true;
			return go;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_ASTEROID);
		m_goList.push_back(newGO);
	}
	return FetchGO();
}

void SceneAsteroid::Reset(void)
{
	// Reset all variables and delete all objects
	DestroyObject(GameObject::GO_SHIP);
	DestroyObject(GameObject::GO_ASTEROID);
	DestroyObject(GameObject::GO_ENEMY_SHIP);
	DestroyObject(GameObject::GO_ENEMY_BULLET);
	DestroyObject(GameObject::GO_SCRAPMETAL);
	DestroyObject(GameObject::GO_BULLET);
	DestroyObject(GameObject::GO_ENEMY_MISSILE);
	DestroyObject(GameObject::GO_ALLY_MISSILE);
	DestroyObject(GameObject::GO_ALLY_BULLET);
	DestroyObject(GameObject::GO_TIE_BOMBER);
	DestroyObject(GameObject::GO_TANTIVE);
	DestroyObject(GameObject::GO_DEATHSTAR);
	DestroyObject(GameObject::GO_IMPERIAL);
	DestroyObject(GameObject::GO_TURRETSINGLE);
	DestroyObject(GameObject::GO_TURRETDOUBLE);
	DestroyObject(GameObject::GO_SINGLEBARREL);
	DestroyObject(GameObject::GO_DOUBLEBARREL);
	DestroyObject(GameObject::GO_MINITURRET);
	DestroyObject(GameObject::GO_ALLY_SHIP);
	DestroyObject(GameObject::GO_AWING);
	DestroyObject(GameObject::GO_YWING);
	DestroyObject(GameObject::GO_INTERCEPTOR);
	DestroyObject(GameObject::GO_FALCON);
	DestroyObject(GameObject::GO_FALCONTURRET);
	DestroyObject(GameObject::GO_BLUELASER);
	DestroyObject(GameObject::GO_LASERBEAM);
	DestroyObject(GameObject::GO_CANNON);
	DestroyObject(GameObject::GO_RIFLE);
	DestroyObject(GameObject::GO_LASERGUN);

	// Set to level one at start of game
	level = LVL_ONE;

	//Physics code here
	m_speed = 1.f;

	m_charge = 0;			   // exhaust counter for standard shooting

	// Counters and spawntimes
	asteroid_spawntime = 10;   // Asteroid spawntime
	asteroid_destroyed = 0;    // Number of asteroids destroyed (For Lvl 2)

	asteroid_count = 0;        // Max number of asteroids on screen
	scrapmetal_count = 0;	   // Number of scrapmetal spawned
	powerup_count = 0;         // Number of powerups spawned

	asteroid_spawned = 0;      // Number of asteroids spawned
	turrets_left = 6;          // Turrets left

	// Booleans
	heavy = false;             // Ship heavy mode toggle
	charged = false;           // Check exhaust bar
	death_star = false;        // Death star spawn 
	tantive = false;           // Tantive spawn
	imperial = false;          // Imperial spawn
	cooldown = false;          // Cooldown for ship shooting
	next_left = false;         // Next left arrow
	next_right = false;        // Next right arrow
	init = true;               // Init for levels
	init2 = true;              // Second init when conditions in a level met
	emergency_repair = false;  // Emergency repair active / inactive
	reinforcements = false;    // Set if show number of ally ships
	laser = false;             // If lasergun equipped

	// Timers
	asteroid_delay = 0;
	activation_delay = 0;
	scrapmetal_delay = 0;
	dmg_delay = 1;
	shoot_delay = 0;
	arrow_delay = 0;
	repair_delay = 0;
	state_delay = 0;
	repair_timer = 0;
	survival_timer = 300;
	powerup_timer = 0;
	powerup_delay = 0;
	laser_delay = 0;

	// Laser width
	laser_width = 0;

	// Ally / Enemy spawning conditions / variables
	for (int i = 0; i < 4; ++i)
	{
		ally_delay[i] = 0;          // Spawn timer
		ally_count[i] = 0;          // Number of allies total
		ally_spawned[i] = 0;        // Number of allies spawned
		ally_max[i] = 0;            // Max number of allies on screen
		ally_spawntime[i] = 10;     // Ally spawnrate
		spawn_ally[i] = false;      // Set to spawn ally

		// Same as ally but for enemy
		enemy_delay[i] = 0;
		enemy_count[i] = 0;
		enemy_spawned[i] = 0;
		enemy_max[i] = 0;
		enemy_spawntime[i] = 10;
		spawn_enemy[i] = false;
	}

	// Flashing colors for health and warning indicator
	health_color = Color(0, 1, 0);
	state_color = Color(0, 1, 0);

	// Set m_ship stats
	m_ship->hp = 100;
	m_ship->shield = 20;
	m_ship->shield_delay = 5;
	m_ship->shield_recharge = 0;
	m_ship->active = true;
	m_ship->mass = 2;
	m_ship->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
	m_ship->scale = Vector3(5, 5, 5);
	m_ship->momentOfInertia = m_ship->mass * ((m_ship->scale.x * .5f) * (m_ship->scale.x * .5f));
	m_ship->angularVelocity = 1;
	m_ship->bullet_delay = 0;

}

void SceneAsteroid::ResetLvl()
{
	// Reset level by deleting all objects and resetting some variables
	DestroyObject(GameObject::GO_ASTEROID);
	DestroyObject(GameObject::GO_SCRAPMETAL);
	DestroyObject(GameObject::GO_CANNON);
	DestroyObject(GameObject::GO_RIFLE);
	DestroyObject(GameObject::GO_LASERGUN);
	DestroyObject(GameObject::GO_ALLY_SHIP);
	DestroyObject(GameObject::GO_INTERCEPTOR);
	DestroyObject(GameObject::GO_TIE_BOMBER);
	DestroyObject(GameObject::GO_ENEMY_SHIP);
	DestroyObject(GameObject::GO_AWING);
	DestroyObject(GameObject::GO_YWING);
	DestroyObject(GameObject::GO_TANTIVE);
	DestroyObject(GameObject::GO_IMPERIAL);
	DestroyObject(GameObject::GO_TURRETSINGLE);
	DestroyObject(GameObject::GO_TURRETDOUBLE);
	DestroyObject(GameObject::GO_SINGLEBARREL);
	DestroyObject(GameObject::GO_DOUBLEBARREL);
	DestroyObject(GameObject::GO_MINITURRET);
	DestroyObject(GameObject::GO_ALLY_BULLET);
	DestroyObject(GameObject::GO_ENEMY_BULLET);
	DestroyObject(GameObject::GO_BLUELASER);
	DestroyObject(GameObject::GO_LASERBEAM);
	DestroyObject(GameObject::GO_BULLET);
	DestroyObject(GameObject::GO_ALLY_MISSILE);
	DestroyObject(GameObject::GO_ENEMY_MISSILE);
	DestroyObject(GameObject::GO_DEATHSTAR);
	DestroyObject(GameObject::GO_FALCON);
	DestroyObject(GameObject::GO_FALCONTURRET);
	scrapmetal_count = 0;
	powerup_count = 0;
	asteroid_spawned = 0;
	init2 = true;
	init = true;
}

void SceneAsteroid::Nextlevel(GAME_LEVEL lvl, int direction)
{
	// set level ENUM
	if (direction == 1) // If direction is left
	{
		if (m_ship->pos.x < 0 - m_ship->scale.x * 0.5f && next_left)
		{
			m_ship->pos.x = m_worldWidth + m_ship->scale.x * 0.5f;
			level = lvl;
			ResetLvl();
			next_left = false;

		}
		else if (m_ship->pos.x < 0 - m_ship->scale.x * 0.5f)
		{
			m_ship->pos.x = m_worldWidth + m_ship->scale.x * 0.5f;
		}
	}
	else if (direction == 2) // If direction is right
	{
		if (m_ship->pos.x > m_worldWidth + m_ship->scale.x * 0.5f && next_right)
		{
			m_ship->pos.x = 0 - m_ship->scale.x * 0.5f;
			level = lvl;
			ResetLvl();
			next_right = false;
		}
		else if (m_ship->pos.x > m_worldWidth + m_ship->scale.x * 0.5f)
		{
			m_ship->pos.x = 0 - m_ship->scale.x * 0.5f;
		}
	}
}

void SceneAsteroid::DestroyObject(GameObject::GAMEOBJECT_TYPE obj)
{
	// Set overloaded object type to inactive
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == obj)
			{
				go->active = false;
			}
		}
	}
}

void SceneAsteroid::CreateGOs(double& dt)
{
	// Spawn death star
	if (death_star)
	{
		GameObject* newGO = FetchGO();
		newGO->active = true;
		newGO->type = GameObject::GO_DEATHSTAR;
		newGO->scale.Set(50, 50, 50);
		newGO->pos = Vector3(150, 100, 0);
		newGO->vel = Vector3(0, 0, 0);
		newGO->hp = 1000;
		death_star = false;
	}

	// Spawn Tantive IV / Rebellion Corvette and turrets
	if (tantive)
	{
		GameObject* newGO = FetchGO();
		newGO->active = true;
		newGO->type = GameObject::GO_TANTIVE;
		newGO->scale.Set(80, 26.66667f, 26.66667f);
		newGO->pos = Vector3(100, 100, 0);
		newGO->vel = Vector3(0, 0, 0);
		newGO->hp = 1000;

		GameObject* newGO2 = FetchGO();
		newGO2->active = true;
		newGO2->type = GameObject::GO_TURRETSINGLE;
		newGO2->scale.Set(4.8, 4.8, 4.8);
		newGO2->pos = Vector3(90.f, 133.f, 0);
		newGO2->vel = Vector3(0, 0, 0);
		newGO2->hp = 200;
		newGO2->pause = 1;
		newGO2->shoot_period = 4;
		newGO2->fire_style = 2;
		newGO2->aim = true;
		newGO2->mass = 10;
		newGO2->momentOfInertia = newGO2->mass * ((newGO2->scale.x * .5f) * (newGO2->scale.x * .5f));

		GameObject* newGO3 = FetchGO();
		newGO3->active = true;
		newGO3->type = GameObject::GO_TURRETSINGLE;
		newGO3->scale.Set(4.8, 4.8, 4.8);
		newGO3->pos = Vector3(110.f, 133.f, 0);
		newGO3->vel = Vector3(0, 0, 0);
		newGO3->hp = 200;
		newGO3->pause = 1;
		newGO3->shoot_period = 4;
		newGO3->fire_style = 2;
		newGO3->aim = true;
		newGO3->mass = 10;
		newGO3->momentOfInertia = newGO3->mass * ((newGO3->scale.x * .5f) * (newGO3->scale.x * .5f));

		GameObject* newGO4 = FetchGO();
		newGO4->active = true;
		newGO4->type = GameObject::GO_TURRETSINGLE;
		newGO4->scale.Set(4.8, 4.8, 4.8);
		newGO4->pos = Vector3(90.f, 121.f, 0);
		newGO4->vel = Vector3(0, 0, 0);
		newGO4->hp = 200;
		newGO4->pause = 1;
		newGO4->shoot_period = 4;
		newGO4->fire_style = 2;
		newGO4->aim = true;
		newGO4->mass = 10;
		newGO4->momentOfInertia = newGO4->mass * ((newGO4->scale.x * .5f) * (newGO4->scale.x * .5f));

		GameObject* newGO5 = FetchGO();
		newGO5->active = true;
		newGO5->type = GameObject::GO_TURRETSINGLE;
		newGO5->scale.Set(4.8, 4.8, 4.8);
		newGO5->pos = Vector3(110.f, 121.f, 0);
		newGO5->vel = Vector3(0, 0, 0);
		newGO5->hp = 200;
		newGO5->pause = 1;
		newGO5->shoot_period = 4;
		newGO5->fire_style = 2;
		newGO5->aim = true;
		newGO5->mass = 10;
		newGO5->momentOfInertia = newGO5->mass * ((newGO5->scale.x * .5f) * (newGO5->scale.x * .5f));

		GameObject* newGO6 = FetchGO();
		newGO6->active = true;
		newGO6->type = GameObject::GO_TURRETDOUBLE;
		newGO6->scale.Set(5.33333f, 5.33333f, 5.33333f);
		newGO6->pos = Vector3(100.f, 81.5f, 0);
		newGO6->vel = Vector3(0, 0, 0);
		newGO6->hp = 400;
		newGO6->pause = 0.5f;
		newGO6->shoot_period = 5;
		newGO6->fire_style = 2;
		newGO6->aim = true;
		newGO6->mass = 10;
		newGO6->momentOfInertia = newGO6->mass * ((newGO6->scale.x * .5f) * (newGO6->scale.x * .5f));

		GameObject* newGO7 = FetchGO();
		newGO7->active = true;
		newGO7->type = GameObject::GO_TURRETDOUBLE;
		newGO7->scale.Set(5.33333f, 5.33333f, 5.33333f);
		newGO7->pos = Vector3(100, 145.f, 0);
		newGO7->vel = Vector3(0, 0, 0);
		newGO7->hp = 400;
		newGO7->pause = 0.5f;
		newGO7->shoot_period = 5;
		newGO7->fire_style = 2;
		newGO7->aim = true;
		newGO7->mass = 10;
		newGO7->momentOfInertia = newGO7->mass * ((newGO7->scale.x * .5f) * (newGO7->scale.x * .5f));

		tantive = false;
	}

	// Spawn Imperial Star Destroyer and turrets
	if (imperial)
	{
		GameObject* newGO = FetchGO();
		newGO->active = true;
		newGO->type = GameObject::GO_IMPERIAL;
		newGO->scale.Set(90, 45.f, 45.f);
		newGO->pos = Vector3(100, 100, 0);
		newGO->vel = Vector3(0, 0, 0);
		newGO->hp = 1000;

		GameObject* newGO2 = FetchGO();
		newGO2->active = true;
		newGO2->type = GameObject::GO_MINITURRET;
		newGO2->scale.Set(4, 3, 3);
		newGO2->pos = Vector3(75, 71, 0);
		newGO2->vel = Vector3(0, 0, 0);
		newGO2->hp = 250;
		newGO2->pause = 1;
		newGO2->shoot_period = 4;
		newGO2->fire_style = 2;
		newGO2->aim = true;
		newGO2->mass = 10;
		newGO2->momentOfInertia = newGO2->mass * ((newGO2->scale.x * .5f) * (newGO2->scale.x * .5f));

		GameObject* newGO3 = FetchGO();
		newGO3->active = true;
		newGO3->type = GameObject::GO_MINITURRET;
		newGO3->scale.Set(4, 3, 3);
		newGO3->pos = Vector3(75, 65, 0);
		newGO3->vel = Vector3(0, 0, 0);
		newGO3->hp = 250;
		newGO3->pause = 1;
		newGO3->shoot_period = 4;
		newGO3->fire_style = 2;
		newGO3->aim = true;
		newGO3->mass = 10;
		newGO3->momentOfInertia = newGO3->mass * ((newGO3->scale.x * .5f) * (newGO3->scale.x * .5f));

		GameObject* newGO4 = FetchGO();
		newGO4->active = true;
		newGO4->type = GameObject::GO_MINITURRET;
		newGO4->scale.Set(4, 3, 3);
		newGO4->pos = Vector3(75, 59, 0);
		newGO4->vel = Vector3(0, 0, 0);
		newGO4->hp = 250;
		newGO4->pause = 1;
		newGO4->shoot_period = 4;
		newGO4->fire_style = 2;
		newGO4->aim = true;
		newGO4->mass = 10;
		newGO4->momentOfInertia = newGO4->mass * ((newGO4->scale.x * .5f) * (newGO4->scale.x * .5f));

		GameObject* newGO5 = FetchGO();
		newGO5->active = true;
		newGO5->type = GameObject::GO_MINITURRET;
		newGO5->scale.Set(4, 3, 3);
		newGO5->pos = Vector3(75, 53, 0);
		newGO5->vel = Vector3(0, 0, 0);
		newGO5->hp = 250;
		newGO5->pause = 1;
		newGO5->shoot_period = 4;
		newGO5->fire_style = 2;
		newGO5->aim = true;
		newGO5->mass = 10;
		newGO5->momentOfInertia = newGO5->mass * ((newGO5->scale.x * .5f) * (newGO5->scale.x * .5f));

		GameObject* newGO6 = FetchGO();
		newGO6->active = true;
		newGO6->type = GameObject::GO_MINITURRET;
		newGO6->scale.Set(4, 3, 3);
		newGO6->pos = Vector3(125, 71, 0);
		newGO6->vel = Vector3(0, 0, 0);
		newGO6->hp = 250;
		newGO6->pause = 1;
		newGO6->shoot_period = 4;
		newGO6->fire_style = 2;
		newGO6->aim = true;
		newGO6->mass = 10;
		newGO6->momentOfInertia = newGO6->mass * ((newGO6->scale.x * .5f) * (newGO6->scale.x * .5f));

		GameObject* newGO7 = FetchGO();
		newGO7->active = true;
		newGO7->type = GameObject::GO_MINITURRET;
		newGO7->scale.Set(4, 3, 3);
		newGO7->pos = Vector3(125, 65, 0);
		newGO7->vel = Vector3(0, 0, 0);
		newGO7->hp = 250;
		newGO7->pause = 1;
		newGO7->shoot_period = 4;
		newGO7->fire_style = 2;
		newGO7->aim = true;
		newGO7->mass = 10;
		newGO7->momentOfInertia = newGO7->mass * ((newGO7->scale.x * .5f) * (newGO7->scale.x * .5f));

		GameObject* newGO8 = FetchGO();
		newGO8->active = true;
		newGO8->type = GameObject::GO_MINITURRET;
		newGO8->scale.Set(4, 3, 3);
		newGO8->pos = Vector3(125, 59, 0);
		newGO8->vel = Vector3(0, 0, 0);
		newGO8->hp = 250;
		newGO8->pause = 1;
		newGO8->shoot_period = 4;
		newGO8->fire_style = 2;
		newGO8->aim = true;
		newGO8->mass = 10;
		newGO8->momentOfInertia = newGO8->mass * ((newGO8->scale.x * .5f) * (newGO8->scale.x * .5f));

		GameObject* newGO9 = FetchGO();
		newGO9->active = true;
		newGO9->type = GameObject::GO_MINITURRET;
		newGO9->scale.Set(4, 3, 3);
		newGO9->pos = Vector3(125, 53, 0);
		newGO9->vel = Vector3(0, 0, 0);
		newGO9->hp = 250;
		newGO9->pause = 1;
		newGO9->shoot_period = 4;
		newGO9->fire_style = 2;
		newGO9->aim = true;
		newGO9->mass = 10;
		newGO9->momentOfInertia = newGO9->mass * ((newGO9->scale.x * .5f) * (newGO9->scale.x * .5f));

		GameObject* newGO10 = FetchGO();
		newGO10->active = true;
		newGO10->type = GameObject::GO_SINGLEBARREL;
		newGO10->scale.Set(6, 5, 5);
		newGO10->pos = Vector3(100, 55, 0);
		newGO10->vel = Vector3(0, 0, 0);
		newGO10->hp = 500;
		newGO10->pause = 1;
		newGO10->shoot_period = 6;
		newGO10->fire_style = 2;
		newGO10->aim = true;
		newGO10->mass = 10;
		newGO10->momentOfInertia = newGO10->mass * ((newGO10->scale.x * .5f) * (newGO10->scale.x * .5f));

		GameObject* newGO11 = FetchGO();
		newGO11->active = true;
		newGO11->type = GameObject::GO_DOUBLEBARREL;
		newGO11->scale.Set(6, 5, 5);
		newGO11->pos = Vector3(100, 135, 0);
		newGO11->vel = Vector3(0, 0, 0);
		newGO11->hp = 500;
		newGO11->pause = 1;
		newGO11->shoot_period = 6;
		newGO11->fire_style = 2;
		newGO11->aim = true;
		newGO11->mass = 10;
		newGO11->momentOfInertia = newGO11->mass * ((newGO11->scale.x * .5f) * (newGO11->scale.x * .5f));
		imperial = false;
	}

	// Spawn Asteroid over time
	{
		if (asteroid_delay > asteroid_spawntime && asteroid_spawned < asteroid_count)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_ASTEROID;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
			newGO->hp = 7;
			asteroid_delay = 0;
			asteroid_spawned++;
		}
		// Limit hardcoded to 5
		else if (asteroid_spawned < 5) 
		{
			asteroid_delay += dt;
		}
	}

	// Spawn scrapmetal overtime, limit to 3
	if (scrapmetal_delay > 30 && scrapmetal_count < 3)
	{
		GameObject* newGO = FetchGO();
		newGO->active = true;
		newGO->type = GameObject::GO_SCRAPMETAL;
		newGO->scale.Set(4, 4, 4);
		newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
		newGO->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		scrapmetal_delay = 0;
		scrapmetal_count++;
	}

	// Spawn powerups, limit to 3
	if (powerup_delay > 60 && powerup_count < 3)
	{
		// Choose random powerup to spawn
		int powerup_rng = Math::RandIntMinMax(1, 10);

		// Cannon 2/5 chance
		if (powerup_rng >= 1 && powerup_rng <= 4)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_CANNON;
			newGO->scale.Set(3.5f, 1.5f, 1.5f);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		}
		// Rifle 2/5 chance
		else if (powerup_rng >= 5 && powerup_rng <= 8)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_RIFLE;
			newGO->scale.Set(3, 2, 2);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		}
		// Laser 1/5 chance
		else if (powerup_rng >= 9 && powerup_rng <= 10)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_LASERGUN;
			newGO->scale.Set(4, 2, 2);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f), 0);
		}

		// Reset timer and increase counter
		powerup_delay = 0;
		powerup_count++;
	}

	// Spawn Enemy ship
	{
		// X wing 
		if (enemy_spawned[0] < enemy_count[0] && spawn_enemy[0] && enemy_delay[0] > enemy_spawntime[0] && enemy_spawned[0] < enemy_max[0])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_ENEMY_SHIP;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->hp = 15;
			newGO->pause = 1;
			newGO->shoot_period = 4;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));
			enemy_spawned[0]++;
			enemy_delay[0] = 0;
		}
		else if (enemy_spawntime[0] == 0 && enemy_spawned[0] == enemy_count[0])
		{
			spawn_enemy[0] = false;
		}

		// A wing 
		if (enemy_spawned[1] < enemy_count[1] && spawn_enemy[1] && enemy_delay[1] > enemy_spawntime[1] && enemy_spawned[1] < enemy_max[1])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_AWING;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->hp = 45;
			newGO->pause = 1;
			newGO->shoot_period = 6;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));
			enemy_spawned[1]++;
			enemy_delay[1] = 0;

		}
		else if (enemy_spawntime[1] == 0 && enemy_spawned[1] == enemy_count[1])
		{
			spawn_enemy[1] = false;
		}

		// y wing
		if (enemy_spawned[2] < enemy_count[2] && spawn_enemy[2] && enemy_delay[2] > enemy_spawntime[2] && enemy_spawned[2] < enemy_max[2])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_YWING;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->hp = 30;
			newGO->pause = 1;
			newGO->shoot_period = 6;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));
			enemy_spawned[2]++;
			enemy_delay[2] = 0;
		}
		else if (enemy_spawntime[2] == 0 && enemy_spawned[2] == enemy_count[2])
		{
			spawn_enemy[2] = false;
		}

		// Millenium falcon
		if (enemy_spawned[3] < enemy_count[3] && spawn_enemy[3] && enemy_delay[3] > enemy_spawntime[3] && enemy_spawned[3] < enemy_max[3])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_FALCON;
			newGO->scale.Set(27.5f, 20, 20);
			newGO->pos = Vector3(100, 100, 0);
			newGO->hp = 600;
			newGO->pause = 1;
			newGO->shoot_period = 6;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));

			GameObject* newGO2 = FetchGO();
			newGO2->active = true;
			newGO2->type = GameObject::GO_FALCONTURRET;
			newGO2->scale.Set(6.75f, 2.25f, 2.25f);
			newGO2->pos = Vector3(95, 99, 0);
			newGO2->vel = Vector3(0, 0, 0);
			newGO2->hp = 300;
			newGO2->pause = 0.5f;
			newGO2->shoot_period = 3;
			newGO2->fire_style = 2;
			newGO2->aim = true;
			newGO2->mass = 10;
			newGO2->momentOfInertia = newGO2->mass * ((newGO2->scale.x * .5f) * (newGO2->scale.x * .5f));
			enemy_spawned[3]++;
			enemy_delay[3] = 0;
		}
		else if (enemy_spawntime[3] == 0 && enemy_spawned[3] == enemy_count[3])
		{
			spawn_enemy[3] = false;
		}
	}


	// Spawn Ally ship
	{ 
		// TIE Fighter
		if (ally_spawned[0] < ally_count[0] && spawn_ally[0] && ally_delay[0] > ally_spawntime[0] && ally_spawned[0] < ally_max[0])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_ALLY_SHIP;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->hp = 30;
			newGO->pause = 1;
			newGO->shoot_period = 4;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));
			ally_spawned[0]++;
			ally_delay[0] = 0;
		}
		else if (ally_spawntime[0] == 0 && ally_spawned[0] == ally_count[0])
		{
			spawn_ally[0] = false;
		}

		// Interceptor
		if (ally_spawned[1] < ally_count[1] && spawn_ally[1] && ally_delay[1] > ally_spawntime[1] && ally_spawned[1] < ally_max[1])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_INTERCEPTOR;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->hp = 60;
			newGO->pause = 1;
			newGO->shoot_period = 6;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));
			ally_spawned[1]++;
			ally_delay[1] = 0;
		}
		else if (ally_spawntime[1] == 0 && ally_spawned[1] == ally_count[1])
		{
			spawn_ally[1] = false;
		}

		// Bomber
		if (ally_spawned[2] < ally_count[2] && spawn_ally[2] && ally_delay[2] > ally_spawntime[2] && ally_spawned[2] < ally_max[2])
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_TIE_BOMBER;
			newGO->scale.Set(5, 5, 5);
			newGO->pos = Vector3(Math::RandFloatMinMax(m_worldWidth - 1, m_worldWidth + 1), Math::RandFloatMinMax(m_worldHeight - 1, m_worldHeight + 1), 0);
			newGO->hp = 45;
			newGO->pause = 1;
			newGO->shoot_period = 6;
			newGO->fire_style = 1;
			newGO->mass = 2;
			newGO->momentOfInertia = newGO->mass * ((newGO->scale.x * .5f) * (newGO->scale.x * .5f));
			ally_spawned[2]++;
			ally_delay[2] = 0;
		}
		else if (ally_spawntime[2] == 0 && ally_spawned[2] == ally_count[2])
		{
			spawn_ally[2] = false;
		}
	}

	// Enemy Bullet
	if (enemy_spawned[0] > 0 || enemy_spawned[1] > 0 || enemy_spawned[2] > 0 || enemy_spawned[3] > 0 || turrets_left > 0)
	{
		GameObject* newGO;
		GameObject* newGO2;
		GameObject* newGO3;
		GameObject* newGO4;

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_ENEMY_SHIP)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.5)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ENEMY_BULLET;
							newGO->scale.Set(1.5, 0.5, 0);
							newGO2->type = GameObject::GO_ENEMY_BULLET;
							newGO2->scale.Set(1.5, 0.5, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir + right * 4.4;
							newGO2->pos = go->pos + go->dir - right * 4.4;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}
				}

				if (go->type == GameObject::GO_AWING)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.25f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ENEMY_BULLET;
							newGO->scale.Set(1.5, 0.5, 0);
							newGO2->type = GameObject::GO_ENEMY_BULLET;
							newGO2->scale.Set(1.5, 0.5, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir + right * 4.6;
							newGO2->pos = go->pos + go->dir - right * 4.3;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}

				}

				if (go->type == GameObject::GO_FALCON)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.25f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ENEMY_BULLET;
							newGO->scale.Set(2.5f, 0.83333f, 0);
							newGO2->type = GameObject::GO_ENEMY_BULLET;
							newGO2->scale.Set(2.5f, 0.83333f, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir * go->scale.x + right * 4;
							newGO2->pos = go->pos + go->dir * go->scale.x - right * 5;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}

				}

				if (go->type == GameObject::GO_FALCONTURRET)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.15f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ENEMY_BULLET;
							newGO->scale.Set(1, 0.33333f, 0);
							newGO2->type = GameObject::GO_ENEMY_BULLET;
							newGO2->scale.Set(1, 0.33333f, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir * go->scale.x + right * 1;
							newGO2->pos = go->pos + go->dir * go->scale.x - right * 1;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}

				}

				if (go->type == GameObject::GO_YWING)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.5)
						{
							if (go->missile && go->bullet_delay > 2)
							{
								newGO3 = FetchGO();
								newGO4 = FetchGO();
								newGO3->type = GameObject::GO_ENEMY_MISSILE;
								newGO3->scale.Set(2, 1, 0);
								newGO4->type = GameObject::GO_ENEMY_MISSILE;
								newGO4->scale.Set(2, 1, 0);
								newGO3->active = true;
								newGO4->active = true;

								Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
								newGO3->pos = go->pos + go->dir + right * 4.f;
								newGO4->pos = go->pos + go->dir - right * 3.5f;

								newGO3->vel = Vector3(go->dir.x, go->dir.y, 0);
								newGO4->vel = Vector3(go->dir.x, go->dir.y, 0);
								if (newGO3->vel == (Vector3(0, 0, 0)) && newGO4->vel == (Vector3(0, 0, 0)))
								{
									newGO3->vel = Vector3(1, 0, 0);
									newGO4->vel = Vector3(1, 0, 0);
								}
								newGO3->vel.Normalize()* BULLET_SPEED;
								newGO4->vel.Normalize()* BULLET_SPEED;
								go->bullet_delay = 0;
							}
							else if (!go->missile)
							{
								newGO = FetchGO();
								newGO2 = FetchGO();
								newGO->type = GameObject::GO_ENEMY_BULLET;
								newGO->scale.Set(1.5, 0.5, 0);
								newGO2->type = GameObject::GO_ENEMY_BULLET;
								newGO2->scale.Set(1.5, 0.5, 0);
								newGO->active = true;
								newGO2->active = true;

								Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
								newGO->pos = go->pos + go->dir + right * 0.9f;
								newGO2->pos = go->pos + go->dir - right * 0.5f;

								newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
								newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

								if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
								{
									newGO->vel = Vector3(1, 0, 0);
									newGO2->vel = Vector3(1, 0, 0);
								}

								newGO->vel.Normalize()* BULLET_SPEED;
								newGO2->vel.Normalize()* BULLET_SPEED;
								go->bullet_delay = 0;
							}
						}
					}

				}

				if (go->type == GameObject::GO_TURRETSINGLE)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.5f)
						{
							newGO = FetchGO();
							newGO->type = GameObject::GO_ENEMY_BULLET;
							newGO->scale.Set(2, 0.666667f, 0);
							newGO->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir * go->scale.x;
							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}
				}

				if (go->type == GameObject::GO_TURRETDOUBLE)
				{
					if (go->shoot && go->shoot_delay > go->pause)
					{
						if (go->bullet_delay > 0.25f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ENEMY_BULLET;
							newGO->scale.Set(1.5, 0.5f, 0);
							newGO2->scale.Set(1.5, 0.5f, 0);
							newGO2->type = GameObject::GO_ENEMY_BULLET;
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
						
							newGO->pos = go->pos + go->dir * go->scale.x + right * 4;
							newGO2->pos = go->pos + go->dir * go->scale.x - right * 4;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) || newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}
				}

			}
		}
	}

	// Ally Bullet
	if (ally_spawned[0] > 0 || ally_spawned[1] > 0 || ally_spawned[2] > 0 || turrets_left > 0)
	{
		GameObject* newGO;
		GameObject* newGO2;
		GameObject* newGO3;

		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				if (go->type == GameObject::GO_ALLY_SHIP)
				{
					if (go->shoot && go->shoot_delay > 1)
					{
						if (go->bullet_delay > 0.5f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ALLY_BULLET;
							newGO->scale.Set(1.5, 0.5, 0);
							newGO2->type = GameObject::GO_ALLY_BULLET;
							newGO2->scale.Set(1.5, 0.5, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir + right * 0.5f;
							newGO2->pos = go->pos + go->dir - right * 1;
							
							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}

				}

				if (go->type == GameObject::GO_INTERCEPTOR)
				{
					if (go->shoot && go->shoot_delay > 1)
					{
						if (go->bullet_delay > 0.25f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ALLY_BULLET;
							newGO->scale.Set(1.5, 0.5, 0);
							newGO2->type = GameObject::GO_ALLY_BULLET;
							newGO2->scale.Set(1.5, 0.5, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir + right * 4.3;
							newGO2->pos = go->pos + go->dir - right * 4.3;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}
				}

				if (go->type == GameObject::GO_TIE_BOMBER)
				{
					if (go->shoot && go->shoot_delay > 1)
					{
						if (go->bullet_delay > 0.5f)
						{
							if (go->missile && go->bullet_delay > 1)
							{
								newGO3 = FetchGO();
								newGO3->type = GameObject::GO_ALLY_MISSILE;
								newGO3->scale.Set(2, 1, 0);
								newGO3->active = true;
								Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
								newGO3->pos = go->pos + go->dir + right * 2;
								newGO3->vel = Vector3(go->dir.x, go->dir.y, 0);

								if (newGO3->vel == (Vector3(0, 0, 0)))
								{
									newGO3->vel = Vector3(1, 0, 0);
								}
								newGO3->vel.Normalize()* BULLET_SPEED;

								go->bullet_delay = 0;
							}
							else if (!go->missile)
							{
								newGO = FetchGO();
								newGO2 = FetchGO();
								newGO->type = GameObject::GO_ALLY_BULLET;
								newGO->scale.Set(1.5f, 0.5f, 0);
								newGO2->type = GameObject::GO_ALLY_BULLET;
								newGO2->scale.Set(1.5f, 0.5f, 0);
								newGO->active = true;
								newGO2->active = true;

								Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
								newGO->pos = go->pos + go->dir - right * 1;
								newGO2->pos = go->pos + go->dir - right * 2;

								newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
								newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

								if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
								{
									newGO->vel = Vector3(1, 0, 0);
									newGO2->vel = Vector3(1, 0, 0);
								}
								newGO->vel.Normalize()* BULLET_SPEED;
								newGO2->vel.Normalize()* BULLET_SPEED;

								go->bullet_delay = 0;
							}
						}
					}
				}
				if (go->type == GameObject::GO_MINITURRET)
				{
					if (go->shoot && go->shoot_delay > 0.5f)
					{
						if (go->bullet_delay > 0.5f)
						{
							newGO = FetchGO();
							newGO->type = GameObject::GO_ALLY_BULLET;
							newGO->scale.Set(2, 0.6666667f, 0);
							newGO->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir * 1;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}
				}

				if (go->type == GameObject::GO_SINGLEBARREL)
				{
					if (go->shoot && go->shoot_delay > 0.5f)
					{
						if (go->bullet_delay > 1.f)
						{
							newGO = FetchGO();
							newGO->type = GameObject::GO_ALLY_MISSILE;
							newGO->scale.Set(2, 1, 0);
							newGO->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir * 1;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}
				}

				if (go->type == GameObject::GO_DOUBLEBARREL)
				{
					if (go->shoot && go->shoot_delay > 0.25f)
					{
						if (go->bullet_delay > 0.25f)
						{
							newGO = FetchGO();
							newGO2 = FetchGO();
							newGO->type = GameObject::GO_ALLY_BULLET;
							newGO->scale.Set(1.5, 0.5, 0);
							newGO2->type = GameObject::GO_ALLY_BULLET;
							newGO2->scale.Set(1.5, 0.5, 0);
							newGO->active = true;
							newGO2->active = true;

							Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
							newGO->pos = go->pos + go->dir + right * 1.5f;
							newGO2->pos = go->pos + go->dir - right * 1.5f;

							newGO->vel = Vector3(go->dir.x, go->dir.y, 0);
							newGO2->vel = Vector3(go->dir.x, go->dir.y, 0);

							if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
							{
								newGO->vel = Vector3(1, 0, 0);
								newGO2->vel = Vector3(1, 0, 0);
							}
							newGO->vel.Normalize()* BULLET_SPEED;
							newGO2->vel.Normalize()* BULLET_SPEED;
							go->bullet_delay = 0;
						}
					}

				}
			}
		}
	}

	// m_ship bullets and powerups
	// Shoot standard bullet
	if (!heavy) 
	{
		if (!cooldown && !m_ship->laser && !m_ship->cannon && !m_ship->rifle)
		{
			if (Application::IsKeyPressed(VK_SPACE) && m_charge < 3)
			{
				m_charge += dt;
				if (m_ship->bullet_delay > 0.2f)
				{
					GameObject* newGO = FetchGO();
					GameObject* newGO2 = FetchGO();
					newGO->active = true;
					newGO2->active = true;
					newGO->type = GameObject::GO_BULLET;
					newGO2->type = GameObject::GO_BULLET;
					newGO->scale.Set(1.5, 0.5, 0);
					newGO2->scale.Set(1.5, 0.5, 0);

					Vector3 right = m_ship->dir.Cross(Vector3(0, 0, -1));
					newGO->pos = m_ship->pos + m_ship->dir + right * 1;
					newGO2->pos = m_ship->pos + m_ship->dir - right * 1;
				
					newGO->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					newGO2->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
					{
						newGO->vel = Vector3(1, 0, 0);
						newGO2->vel = Vector3(1, 0, 0);
					}
					newGO->vel.Normalize()* BULLET_SPEED;
					newGO2->vel.Normalize()* BULLET_SPEED;
					m_ship->bullet_delay = 0;
				}
			}
			else if (m_charge >= 0 && m_charge < 3)
			{
				m_charge -= dt * 2;
			}
			else if (m_charge >= 3)
			{
				cooldown = true;
			}
			if (Application::IsKeyPressed(VK_LCONTROL) && activation_delay > 1)
			{
				heavy = true;
				activation_delay = 0;
				m_charge = 0;
			}
		}
		else if (cooldown)
		{
			m_charge -= dt;
			if (Application::IsKeyPressed(VK_SPACE))
			{
				std::cout << "<-------------- cooldown -------------->" << "\n";
			}
			if (m_charge <= 0)
			{
				cooldown = false;
			}
		}

		// Cannon powerup changes bullet to missile
		if (m_ship->cannon)
		{
			if (Application::IsKeyPressed(VK_SPACE) && powerup_timer > 0)
			{
				if (m_ship->bullet_delay > 0.5f)
				{
					GameObject* newGO = FetchGO();
					GameObject* newGO2 = FetchGO();
					newGO->active = true;
					newGO2->active = true;
					newGO->type = GameObject::GO_ALLY_MISSILE;
					newGO2->type = GameObject::GO_ALLY_MISSILE;
					newGO->scale.Set(1.5, 0.5, 0);
					newGO2->scale.Set(1.5, 0.5, 0);

					Vector3 right = m_ship->dir.Cross(Vector3(0, 0, -1));
					newGO->pos = m_ship->pos + m_ship->dir + right * 1;
					newGO2->pos = m_ship->pos + m_ship->dir - right * 1;

					newGO->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					newGO2->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
					{
						newGO->vel = Vector3(1, 0, 0);
						newGO2->vel = Vector3(1, 0, 0);
					}
					newGO->vel.Normalize()* BULLET_SPEED;
					newGO2->vel.Normalize()* BULLET_SPEED;
					m_ship->bullet_delay = 0;
					powerup_timer -= 1;
				}
			}
			else if (powerup_timer <= 0)
			{
				m_ship->cannon = false;
			}
		}

		// Rifle powerup changes normal green laser to stronger blue laser
		if (m_ship->rifle)
		{
			if (Application::IsKeyPressed(VK_SPACE) && powerup_timer > 0)
			{
				if (m_ship->bullet_delay > 0.25f)
				{
					GameObject* newGO = FetchGO();
					GameObject* newGO2 = FetchGO();
					newGO->active = true;
					newGO2->active = true;
					newGO->type = GameObject::GO_BLUELASER;
					newGO2->type = GameObject::GO_BLUELASER;
					newGO->scale.Set(1.5, 0.5, 0);
					newGO2->scale.Set(1.5, 0.5, 0);

					Vector3 right = m_ship->dir.Cross(Vector3(0, 0, -1));
					newGO->pos = m_ship->pos + m_ship->dir + right * 1;
					newGO2->pos = m_ship->pos + m_ship->dir - right * 1;

					newGO->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					newGO2->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					if (newGO->vel == (Vector3(0, 0, 0)) && newGO2->vel == (Vector3(0, 0, 0)))
					{
						newGO->vel = Vector3(1, 0, 0);
						newGO2->vel = Vector3(1, 0, 0);
					}
					newGO->vel.Normalize()* BULLET_SPEED;
					newGO2->vel.Normalize()* BULLET_SPEED;
					m_ship->bullet_delay = 0;
				}
				powerup_timer -= dt * 2;
			}
			else if (powerup_timer <= 0)
			{
				m_ship->rifle = false;
			}
		}

		// Laser powerup shoots a laser
		if (m_ship->laser)
		{
			if (Application::IsKeyPressed(VK_SPACE) && powerup_timer > 0)
			{
				if (!laser)
				{
					GameObject* newGO = FetchGO();
					newGO->active = true;
					newGO->type = GameObject::GO_LASERBEAM;
					newGO->scale.Set(100, laser_width, 0);

					Vector3 right = m_ship->dir.Cross(Vector3(0, 0, -1));
					newGO->pos = m_ship->pos + m_ship->dir;

					newGO->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
					if (newGO->vel == (Vector3(0, 0, 0)))
					{
						newGO->vel = Vector3(1, 0, 0);
					}
					newGO->vel.Normalize()* BULLET_SPEED;
					laser = true;
				}
				for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject* go = (GameObject*)*it;
					if (go->active)
					{
						if (go->type == GameObject::GO_LASERBEAM)
						{
							Vector3 right = m_ship->dir.Cross(Vector3(0, 0, -1));
							go->pos = m_ship->pos + m_ship->dir * 100;
							go->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
							go->scale.Set(100, laser_width, 0);
						}
					}
				}
				if (m_ship->bullet_delay > 0.01f && laser_width < 2)
				{
					laser_width += 0.025f;
					m_ship->bullet_delay = 0;
				}
				else
					powerup_timer -= dt * 4;
				
			}
			else if (powerup_timer <= 0)
			{
				m_ship->laser = false;
				laser_width = 0;
				laser = false;
				for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject* go = (GameObject*)*it;
					if (go->active)
					{
						if (go->type == GameObject::GO_LASERBEAM)
						{
							go->active = false;
						}
					}
				}
			}
			else
			{
				laser = false;
				laser_width = 0;
				for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
				{
					GameObject* go = (GameObject*)*it;
					if (go->active)
					{
						if (go->type == GameObject::GO_LASERBEAM)
						{
							go->active = false;
						}
					}
				}
			}
		}
	}

	// If heavy mode, shoot larger bullet
	if (heavy) 
	{
		if (Application::IsKeyPressed(VK_LCONTROL) && activation_delay > 1)
		{
			heavy = false;
			activation_delay = 0;
			m_charge = 0;
		}
		if (!charged)
		{
			if (Application::IsKeyPressed(VK_SPACE))
			{
				m_charge += dt;
				if (m_charge > 1)
				{
					charged = true;
				}
			}
			else if (m_charge > 0)
			{
				m_charge -= dt;
			}
		}
		else if (charged)
		{
			GameObject* newGO = FetchGO();
			newGO->active = true;
			newGO->type = GameObject::GO_BULLET;
			newGO->scale.Set(2, 0.66667f, 0);
			newGO->pos = Vector3(m_ship->pos.x, m_ship->pos.y, 0);
			newGO->vel = Vector3(m_ship->dir.x, m_ship->dir.y, 0);
			if (newGO->vel == (Vector3(0, 0, 0)))
			{
				newGO->vel = Vector3(1, 0, 0);
			}
			newGO->vel.Normalize()* BULLET_SPEED * 2;
			charged = false;
			m_charge = 0;
		}
	}

}

void SceneAsteroid::CreateLvl()
{
	// Check for each level ENUM
	// Respective level contents
	if (level == LVL_ONE)
	{
		if (init)
		{
			indicator = INDICATOR_FIVE;
			objective = OBJECTIVE_ZERO;
			asteroid_count = 0;
			next_left = true;
			death_star = true;
			init = false;
		}
		// If testing, set next level here
		Nextlevel(LVL_EIGHT, 1);
	}
	// asteroid shooting
	if (level == LVL_TWO)
	{
		if (init)
		{
			indicator = INDICATOR_ONE;
			objective = OBJECTIVE_ONE;
			SpawnAsteroid(10, 3);
			init = false;
		}
		
		if (asteroid_destroyed >= 10)
		{
			objective = OBJECTIVE_ZERO;
			next_left = true;
			asteroid_destroyed = 10;
		}
		Nextlevel(LVL_THREE, 1);
	}
	// First x wings enemies
	// TIE fighter allies
	if (level == LVL_THREE)
	{
		cout << enemy_spawned[0] << endl;
		cout << spawn_enemy[0] << endl;
		if (init)
		{
			indicator = INDICATOR_TWO;
			objective = OBJECTIVE_TWO;
			SpawnAsteroid(10, 10);
			SpawnAlly(2, 2, 0, 0);
			SpawnEnemy(5, 5, 0, 0);
			init = false;
		}
		if (enemy_count[0] <= 0)
		{
			objective = OBJECTIVE_ZERO;
			next_left = true;
		}
		Nextlevel(LVL_FOUR, 1);
	}
	// Second x wing enemies
	if (level == LVL_FOUR)
	{
		if (init)
		{
			indicator = INDICATOR_TWO;
			objective = OBJECTIVE_TWO;
			SpawnAsteroid(10, 10);
			SpawnAlly(5, 5, 0, 0);
			SpawnEnemy(10, 10, 0, 5);
			init = false;
		}
		if (enemy_count[0] <= 0)
		{
			objective = OBJECTIVE_ZERO;
			next_left = true;
		}
		Nextlevel(LVL_FIVE, 1);
	}
	// tantive with turrets
	// bomber allies
	if (level == LVL_FIVE)
	{
		if (init)
		{
			indicator = INDICATOR_THREE;
			objective = OBJECTIVE_THREE;
			reinforcements = true;
			tantive = true;
			SpawnAsteroid(5, 10);
			SpawnAlly(15, 5, 0, 5);
			SpawnAlly(15, 5, 2, 5);
			SpawnEnemy(500, 15, 0, 5);
			init = false;
		}
		if (turrets_left <= 0)
		{
			SpawnEnemy(0, 0, 0, 0);
			objective = OBJECTIVE_FOUR;
			next_right = true;
			reinforcements = false;
		}
		Nextlevel(LVL_SIX, 2);
	}
	// First a wing enemies
	// interceptor allies
	if (level == LVL_SIX)
	{
		if (init)
		{
			indicator = INDICATOR_TWO;
			objective = OBJECTIVE_TWO;

			SpawnAsteroid(5, 10);
			SpawnAlly(500, 7, 0, 5);
			SpawnAlly(500, 3, 1, 5);
			SpawnEnemy(10, 5, 0, 5);
			SpawnEnemy(5, 5, 1, 5);
			init = false;
		}
		if (enemy_count[0] + enemy_count[1] <= 0)
		{
			objective = OBJECTIVE_FOUR;
			next_right = true;
		}
		Nextlevel(LVL_SEVEN, 2);
	}
	// Defend imperial star destroyer
	// y wing enemies
	if (level == LVL_SEVEN)
	{
		if (init)
		{
			survival_timer = 180;
			indicator = INDICATOR_FOUR;
			objective = OBJECTIVE_FIVE;
			turrets_left = 10;
			imperial = true;
			SpawnAsteroid(5, 10);
			SpawnAlly(500, 5, 0, 5);
			SpawnEnemy(500, 5, 0, 5);
			SpawnEnemy(500, 5, 1, 5);
			SpawnEnemy(500, 5, 2, 5);
			init = false;
		}
		if (turrets_left <= 0)
		{
			state = STATE_LOSE2;
		}
		if (survival_timer <= 0)
		{
			survival_timer = 0;
			if (init2)
			{
				indicator = INDICATOR_TWO;
				SpawnEnemy(10, 5, 0, 5);
				SpawnEnemy(10, 5, 1, 5);
				SpawnEnemy(10, 5, 2, 5);
				SpawnAlly(500, 3, 0, 5);
				SpawnAlly(500, 7, 1, 3);
				init2 = false;
			}

		}
		if (enemy_count[0] + enemy_count[1] + enemy_count[2] <= 0)
		{
			DestroyObject(GameObject::GO_ENEMY_SHIP);
			DestroyObject(GameObject::GO_AWING);
			DestroyObject(GameObject::GO_YWING);
			enemy_count[0] = 0;
			enemy_count[1] = 0;
			enemy_count[2] = 0;
			objective = OBJECTIVE_FOUR;
			next_right = true;
		}
		Nextlevel(LVL_EIGHT, 2);
	}
	// Millenium falcon boss fight
	if (level == LVL_EIGHT)
	{
		if (init)
		{
			indicator = INDICATOR_FIVE;
			objective = OBJECTIVE_SIX;
			SpawnAsteroid(5, 10);
			SpawnAlly(500, 5, 0, 5);
			SpawnAlly(500, 5, 2, 5);
			SpawnEnemy(1, 1, 3, 0);
			SpawnEnemy(500, 5, 0, 5);
			SpawnEnemy(500, 5, 1, 5);
			init = false;
		}
		if (enemy_count[3] <= 0)
		{
			if (init2)
			{
				objective = OBJECTIVE_TWO;
				indicator = INDICATOR_TWO;
				SpawnEnemy(5, 5, 0, 5);
				SpawnEnemy(5, 5, 1, 5);
				init2 = false;
			}
			if (enemy_count[0] + enemy_count[1] <= 0)
			{
				DestroyObject(GameObject::GO_ENEMY_SHIP);
				DestroyObject(GameObject::GO_AWING);
				DestroyObject(GameObject::GO_YWING);
				objective = OBJECTIVE_FOUR;
				next_right = true;
			}
		}
		Nextlevel(LVL_NINE, 2);
	}
	// Win by reaching level nine
	if (level == LVL_NINE)
	{
		state = STATE_WIN;
	}
}

void SceneAsteroid::Gameplay(double &dt)
{
	// Call Gameplay functions for each entity
	linearVel(GameObject::GO_ENEMY_SHIP, dt);
	angularVel(GameObject::GO_ENEMY_SHIP, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_ENEMY_SHIP, 'e', 0);
	AllyBulletCollision(GameObject::GO_ENEMY_SHIP, 0, 5);
	Idle(GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_ENEMY_SHIP, GameObject::GO_ALLY_SHIP);
	Aim(GameObject::GO_ENEMY_SHIP, GameObject::GO_INTERCEPTOR);
	Aim(GameObject::GO_ENEMY_SHIP, GameObject::GO_SINGLEBARREL);
	Aim(GameObject::GO_ENEMY_SHIP, GameObject::GO_DOUBLEBARREL);
	Aim(GameObject::GO_ENEMY_SHIP, GameObject::GO_MINITURRET);
	Aim(GameObject::GO_ENEMY_SHIP, GameObject::GO_TIE_BOMBER);
	AimPlayer(GameObject::GO_ENEMY_SHIP);

	linearVel(GameObject::GO_AWING, dt);
	angularVel(GameObject::GO_AWING, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_AWING, 'e', 1);
	AllyBulletCollision(GameObject::GO_AWING, 1, 5);
	Idle(GameObject::GO_AWING);
	Aim(GameObject::GO_AWING, GameObject::GO_ALLY_SHIP);
	Aim(GameObject::GO_AWING, GameObject::GO_INTERCEPTOR);
	Aim(GameObject::GO_AWING, GameObject::GO_SINGLEBARREL);
	Aim(GameObject::GO_AWING, GameObject::GO_DOUBLEBARREL);
	Aim(GameObject::GO_AWING, GameObject::GO_MINITURRET);
	Aim(GameObject::GO_AWING, GameObject::GO_TIE_BOMBER);
	AimPlayer(GameObject::GO_AWING);

	linearVel(GameObject::GO_YWING, dt);
	angularVel(GameObject::GO_YWING, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_YWING, 'e', 2);
	AllyBulletCollision(GameObject::GO_YWING, 2, 5);
	MissileIdle(GameObject::GO_YWING);
	Aim(GameObject::GO_YWING, GameObject::GO_ALLY_SHIP);
	Aim(GameObject::GO_YWING, GameObject::GO_INTERCEPTOR);
	Aim(GameObject::GO_YWING, GameObject::GO_SINGLEBARREL);
	Aim(GameObject::GO_YWING, GameObject::GO_DOUBLEBARREL);
	Aim(GameObject::GO_YWING, GameObject::GO_MINITURRET);
	Aim(GameObject::GO_YWING, GameObject::GO_TIE_BOMBER);
	AimPlayer(GameObject::GO_YWING);

	linearVel(GameObject::GO_FALCON, dt);
	angularVel(GameObject::GO_FALCON, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_FALCON, 'e', 3);
	AllyBulletCollision(GameObject::GO_FALCON, 3, 15);
	Idle(GameObject::GO_FALCON);
	Aim(GameObject::GO_FALCON, GameObject::GO_ALLY_SHIP);
	Aim(GameObject::GO_FALCON, GameObject::GO_INTERCEPTOR);
	Aim(GameObject::GO_FALCON, GameObject::GO_SINGLEBARREL);
	Aim(GameObject::GO_FALCON, GameObject::GO_DOUBLEBARREL);
	Aim(GameObject::GO_FALCON, GameObject::GO_MINITURRET);
	Aim(GameObject::GO_FALCON, GameObject::GO_TIE_BOMBER);
	AimPlayer(GameObject::GO_FALCON);

	angularVel(GameObject::GO_FALCONTURRET, -5.0f, 5.0f, dt);
	Aim(GameObject::GO_FALCONTURRET, GameObject::GO_ALLY_SHIP);
	Aim(GameObject::GO_FALCONTURRET, GameObject::GO_INTERCEPTOR);
	Aim(GameObject::GO_FALCONTURRET, GameObject::GO_TIE_BOMBER);
	AimPlayer(GameObject::GO_FALCONTURRET);

	linearVel(GameObject::GO_ALLY_SHIP, dt);
	angularVel(GameObject::GO_ALLY_SHIP, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_ALLY_SHIP, 'a', 0);
	EnemyBulletCollision(GameObject::GO_ALLY_SHIP, 0, 5);
	Idle(GameObject::GO_ALLY_SHIP);
	Aim(GameObject::GO_ALLY_SHIP, GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_ALLY_SHIP, GameObject::GO_TURRETSINGLE);
	Aim(GameObject::GO_ALLY_SHIP, GameObject::GO_TURRETDOUBLE);
	Aim(GameObject::GO_ALLY_SHIP, GameObject::GO_AWING);
	Aim(GameObject::GO_ALLY_SHIP, GameObject::GO_YWING);
	Aim(GameObject::GO_ALLY_SHIP, GameObject::GO_FALCON);

	linearVel(GameObject::GO_INTERCEPTOR, dt);
	angularVel(GameObject::GO_INTERCEPTOR, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_INTERCEPTOR, 'a', 1);
	EnemyBulletCollision(GameObject::GO_INTERCEPTOR, 1, 5);
	Idle(GameObject::GO_INTERCEPTOR);
	Aim(GameObject::GO_INTERCEPTOR, GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_INTERCEPTOR, GameObject::GO_AWING);
	Aim(GameObject::GO_INTERCEPTOR, GameObject::GO_YWING);
	Aim(GameObject::GO_INTERCEPTOR, GameObject::GO_FALCON);

	linearVel(GameObject::GO_TIE_BOMBER, dt);
	angularVel(GameObject::GO_TIE_BOMBER, -5.0f, 5.0f, dt);
	AsteroidCollision(GameObject::GO_TIE_BOMBER, 'a', 2);
	EnemyBulletCollision(GameObject::GO_TIE_BOMBER, 2, 5);
	MissileIdle(GameObject::GO_TIE_BOMBER);
	Aim(GameObject::GO_TIE_BOMBER, GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_TIE_BOMBER, GameObject::GO_TURRETSINGLE);
	Aim(GameObject::GO_TIE_BOMBER, GameObject::GO_TURRETDOUBLE);
	Aim(GameObject::GO_TIE_BOMBER, GameObject::GO_AWING);
	Aim(GameObject::GO_TIE_BOMBER, GameObject::GO_YWING);
	Aim(GameObject::GO_TIE_BOMBER, GameObject::GO_FALCON);

	angularVel(GameObject::GO_TURRETSINGLE, -0.5f, 0.5f, dt);
	AllyBulletCollision(GameObject::GO_TURRETSINGLE, 0, 2);
	Aim(GameObject::GO_TURRETSINGLE, GameObject::GO_ALLY_SHIP);
	AimPlayer(GameObject::GO_TURRETSINGLE);

	angularVel(GameObject::GO_TURRETDOUBLE, -0.5f, 0.5f, dt);
	AllyBulletCollision(GameObject::GO_TURRETDOUBLE, 0, 2);
	Aim(GameObject::GO_TURRETDOUBLE, GameObject::GO_ALLY_SHIP);
	AimPlayer(GameObject::GO_TURRETDOUBLE);

	angularVel(GameObject::GO_MINITURRET, -0.5f, 0.5f, dt);
	EnemyBulletCollision(GameObject::GO_MINITURRET, 0, 2);
	Aim(GameObject::GO_MINITURRET, GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_MINITURRET, GameObject::GO_AWING);
	Aim(GameObject::GO_MINITURRET, GameObject::GO_YWING);

	angularVel(GameObject::GO_SINGLEBARREL, -0.5f, 0.5f, dt);
	EnemyBulletCollision(GameObject::GO_SINGLEBARREL, 0, 2);
	Aim(GameObject::GO_SINGLEBARREL, GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_SINGLEBARREL, GameObject::GO_AWING);
	Aim(GameObject::GO_SINGLEBARREL, GameObject::GO_YWING);

	angularVel(GameObject::GO_DOUBLEBARREL, -0.5f, 0.5f, dt);
	EnemyBulletCollision(GameObject::GO_DOUBLEBARREL, 0, 2);
	Aim(GameObject::GO_DOUBLEBARREL, GameObject::GO_ENEMY_SHIP);
	Aim(GameObject::GO_DOUBLEBARREL, GameObject::GO_AWING);
	Aim(GameObject::GO_DOUBLEBARREL, GameObject::GO_YWING);

	// Wrap around (1 = wrap, 2 = set inactive)
	WrapAround(GameObject::GO_BULLET, 2);
	WrapAround(GameObject::GO_ALLY_BULLET, 2);
	WrapAround(GameObject::GO_ALLY_MISSILE, 2);
	WrapAround(GameObject::GO_ENEMY_BULLET, 2);
	WrapAround(GameObject::GO_ENEMY_MISSILE, 2);
	WrapAround(GameObject::GO_BLUELASER, 2);
	WrapAround(GameObject::GO_ENEMY_SHIP, 1);
	WrapAround(GameObject::GO_AWING, 1);
	WrapAround(GameObject::GO_YWING, 1);
	WrapAround(GameObject::GO_FALCON, 1);
	WrapAround(GameObject::GO_ALLY_SHIP, 1);
	WrapAround(GameObject::GO_INTERCEPTOR, 1);
	WrapAround(GameObject::GO_TIE_BOMBER, 1);
	WrapAround(GameObject::GO_ASTEROID, 1);
	WrapAround(GameObject::GO_SCRAPMETAL, 1);
	WrapAround(GameObject::GO_CANNON, 1);
	WrapAround(GameObject::GO_RIFLE, 1);
	WrapAround(GameObject::GO_LASERGUN, 1);

	// Special case iterator for entities not applicable with above functions
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			// Handling collision between GO_SHIP and GO_ASTEROID
			if (go->type == GameObject::GO_ASTEROID)
			{
				go->pos += go->vel * dt * m_speed;

				if ((go->pos - m_ship->pos).Length() <= (go->scale.Length() + m_ship->scale.Length()))
				{
					go->active = false;
					asteroid_spawned--;
					dmg_delay = 0;
					TakeDMG(10);
					asteroid_destroyed++;
				}
			}

			// Attaching falcon turret
			if (go->type == GameObject::GO_FALCON)
			{
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->type == GameObject::GO_FALCONTURRET)
					{
						Vector3 right = go->dir.Cross(Vector3(0, 0, -1));
						go2->pos = go->pos + go->dir * (-4.5f) - right * (0.5f);
					}
				}
			}

			// Moving scrapmetal and collision with m_ship
			if (go->type == GameObject::GO_SCRAPMETAL)
			{
				go->pos += go->vel * dt * m_speed;

				if ((go->pos - m_ship->pos).Length() <= (go->scale.Length() + m_ship->scale.Length()) && m_ship->hp < 100)
				{
					go->active = false;
					m_ship->hp += 20;
				}
				if (m_ship->hp > 100)
				{
					m_ship->hp = 100;
				}
			}

			// Powerup collisions and boolean activations
			if (go->type == GameObject::GO_CANNON)
			{
				go->pos += go->vel * dt * m_speed;

				if ((go->pos - m_ship->pos).Length() <= (go->scale.Length() + m_ship->scale.Length()))
				{
					go->active = false;
					m_ship->cannon = true;
					m_ship->rifle = false;
					m_ship->laser = false;
					heavy = false;
					powerup_timer = 30;
					powerup_count--;
				}
			}

			if (go->type == GameObject::GO_RIFLE)
			{
				go->pos += go->vel * dt * m_speed;

				if ((go->pos - m_ship->pos).Length() <= (go->scale.Length() + m_ship->scale.Length()))
				{
					go->active = false;
					m_ship->rifle = true;
					m_ship->cannon = false;
					m_ship->laser = false;
					heavy = false;
					powerup_timer = 30;
					powerup_count--;
				}
			}

			if (go->type == GameObject::GO_LASERGUN)
			{
				go->pos += go->vel * dt * m_speed;

				if ((go->pos - m_ship->pos).Length() <= (go->scale.Length() + m_ship->scale.Length()))
				{
					go->active = false;
					m_ship->laser = true;
					m_ship->rifle = false;
					m_ship->cannon = false;
					heavy = false;
					powerup_timer = 30;
					powerup_count--;
				}
			}

			// Special case if laser is on when another powerup picked up
			if (!m_ship->laser)
			{
				DestroyObject(GameObject::GO_LASERBEAM);
			}
			
			// Ally bullet collisions with asteroid and bullet movements
			if (go->type == GameObject::GO_BULLET || go->type == GameObject::GO_ALLY_BULLET || go->type == GameObject::GO_BLUELASER || go->type == GameObject::GO_ALLY_MISSILE)
			{
				if (go->type == GameObject::GO_ALLY_MISSILE)
					go->pos += go->vel * dt * BULLET_SPEED * 0.75f;
				else if (go->type == GameObject::GO_BLUELASER)
					go->pos += go->vel * dt * BULLET_SPEED * 1.25f;
				else
					go->pos += go->vel * dt * BULLET_SPEED;

				// Aseteroid splitting
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->type == GameObject::GO_ASTEROID)
					{
						if (go2->active)
						{
							if ((go->pos - go2->pos).Length() <= (go->scale.Length() + go2->scale.Length()))
							{
								go->active = false;
								if (!heavy)
								{
									go2->hp--;
								}
								else if (go->type == GameObject::GO_BULLET)
								{
									go2->hp -= 4;
								}
								else
								{
									go2->hp--;
								}
								// If asteroid reach a certain HP, split randomly
								if (go2->hp == 3)
								{
									Vector3 totalMomentum = -go2->vel * go2->hp; //stores the resulting vector of all the splits + current vel
									float totalHP = go2->hp - 1;
									int numSplits = Math::RandIntMinMax(2, 4);
									GameObject* newAsteroid;
									for (int i = 0; i < numSplits && totalHP >= 1; i++)
									{
										Vector3 newVel = Vector3(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
										float newHP = Math::RandFloatMinMax(go->hp / (numSplits * 2), (go2->hp + 1) / (numSplits));
										newHP = Math::Max(1.f, newHP);
										if (newHP > totalHP)
										{
											newHP = totalHP;
										}
										newAsteroid = FetchGO();
										newAsteroid->active = true;
										newAsteroid->type = GameObject::GO_ASTEROID;
										newAsteroid->hp = newHP;
										newAsteroid->scale.Set(newAsteroid->hp * 3, newAsteroid->hp * 3, 1);
										newAsteroid->vel = newVel;
										newAsteroid->pos = go2->pos;
										totalMomentum += newVel * newHP;
										totalHP -= newHP;
									}
									newAsteroid = FetchGO();
									newAsteroid->active = true;
									newAsteroid->type = GameObject::GO_ASTEROID;
									newAsteroid->hp = totalHP + 1;
									if (newAsteroid->hp >= 1)
									{
										newAsteroid->scale.Set(newAsteroid->hp * 3, newAsteroid->hp * 3, 1);
										newAsteroid->vel = -totalMomentum * (1 / (newAsteroid->hp));
										newAsteroid->pos = go->pos;
									}
									go2->active = false;
									asteroid_spawned--;
									asteroid_destroyed++;
								}
								else if (go2->hp < 0)
								{
									go2->active = false;
									asteroid_spawned--;
									asteroid_destroyed++;
								}
							}
						}
					}
				}
			}

			// Enemy bullet movements and special case collision with m_ship
			if (go->type == GameObject::GO_ENEMY_BULLET || go->type == GameObject::GO_ENEMY_MISSILE)
			{
				if (go->type == GameObject::GO_ENEMY_MISSILE)
					go->pos += go->vel * dt * BULLET_SPEED * 0.75f;
				else
					go->pos += go->vel * dt * BULLET_SPEED;

				if ((go->pos - m_ship->pos).Length() <= (go->scale.Length() + m_ship->scale.Length()) - 5)
				{
					if (go->type == GameObject::GO_ENEMY_MISSILE)
					{
						TakeDMG(5);
					}
					else
					{
						TakeDMG(1);
					}
					dmg_delay = 0;
					go->active = false;
				}

			}

		}
	}
}

void SceneAsteroid::linearVel(GameObject::GAMEOBJECT_TYPE GO, double &dt)
{
	// linear motion for GO
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				Vector3 m_accel = go->force * (1.f / go->mass);

				go->vel += m_accel * dt * m_speed;
				go->pos.y += go->vel.y * dt * m_speed;
				go->pos.x += go->vel.x * dt * m_speed;

				if (go->vel.Length() > ENEMY_MAX_SPEED)
				{
					go->vel = go->vel.Normalize() * ENEMY_MAX_SPEED;
				}

			}
		}
	}
}

void SceneAsteroid::angularVel(GameObject::GAMEOBJECT_TYPE GO, float min, float max, double &dt)
{
	// Angular motion for GO
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				///turn a vector by angle
				float alpha = go->torque.z / go->momentOfInertia;

				go->angularVelocity += alpha * dt;

				go->angularVelocity = Math::Clamp(go->angularVelocity, min, max);
				float dTheta = go->angularVelocity * dt;

				go->dir.Set(go->dir.x * cosf(dTheta) - go->dir.y * sinf(dTheta), go->dir.x * sinf(dTheta) + go->dir.y * cosf(dTheta)); //turn a vector by angle

				go->force.SetZero();
				go->torque.SetZero();
				go->movement_delay += dt;
				go->shoot_delay += dt;
				go->bullet_delay += dt;
			}

		}

	}
}

void SceneAsteroid::WrapAround(GameObject::GAMEOBJECT_TYPE GO, int choice)
{
	// Wrap around for GO
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				// GO wrap around
				if (choice == 1)
				{
					if (go->pos.x > m_worldWidth + go->scale.x * 0.5f)
					{
						go->pos.x = 0 - go->scale.x * 0.5f;
					}
					else if (go->pos.x < 0 - go->scale.x * 0.5f)
					{
						go->pos.x = m_worldWidth + go->scale.x * 0.5f;
					}
					if (go->pos.y > m_worldHeight + go->scale.y * 0.5f)
					{
						go->pos.y = 0 - go->scale.y * 0.5f;
					}
					else if (go->pos.y < 0 - go->scale.y * 0.5f)
					{
						go->pos.y = m_worldHeight + go->scale.y * 0.5f;
					}
				}
				// Destroy GO
				else if (choice == 2)
				{
					if (go->pos.x > m_worldWidth + go->scale.x * 0.5f)
					{
						go->active = false;
					}
					else if (go->pos.x < 0 - go->scale.x * 0.5f)
					{
						go->active = false;
					}
					if (go->pos.y > m_worldHeight + go->scale.y * 0.5f)
					{
						go->active = false;
					}
					else if (go->pos.y < 0 - go->scale.y * 0.5f)
					{
						go->active = false;
					}
				}
			}
		}
	}
	// Special case for m_ship
	if (choice == 3)
	{
		if (m_ship->pos.x > m_worldWidth + m_ship->scale.x * 0.5f && !next_right)
		{
			m_ship->pos.x = 0 - m_ship->scale.x * 0.5f;
		}
		else if (m_ship->pos.x < 0 - m_ship->scale.x * 0.5f && !next_left)
		{
			m_ship->pos.x = m_worldWidth + m_ship->scale.x * 0.5f;
		}
		if (m_ship->pos.y > m_worldHeight + m_ship->scale.y * 0.5f)
		{
			m_ship->pos.y = 0 - m_ship->scale.y * 0.5f;
		}
		else if (m_ship->pos.y < 0 - m_ship->scale.y * 0.5f)
		{
			m_ship->pos.y = m_worldHeight + m_ship->scale.y * 0.5f;
		}
	}
}

void SceneAsteroid::Idle(GameObject::GAMEOBJECT_TYPE GO)
{
	// Idle for GO
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				if (go->idle)
				{
					// Prepare to attack
					if (go->movement_phase == 1 && go->movement_delay < go->movement_change)
					{
						go->aim = true;
						go->idle = false;
					}
					// Move forward
					else if (go->movement_phase == 2 && go->movement_delay < go->movement_change)
					{
						go->force += go->dir * 100;
						go->torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));
						go->turn = false;
					}
					// Turn left
					else if (go->movement_phase == 3 && go->movement_delay < go->movement_change)
					{
						go->force += go->dir * 1;
						go->torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));
						go->turn = true;
					}
					// Turn right
					else if (go->movement_phase == 4 && go->movement_delay < go->movement_change)
					{
						go->force += go->dir * 1;
						go->torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));
						go->turn = true;
					}
					// reshuffle movement through RNG
					else
					{
						go->movement_delay = 0;
						if (go->turn == false)
						{
							go->movement_phase = Math::RandIntMinMax(2, 4);
							go->movement_change = Math::RandFloatMinMax(0.5f, 1);
						}
						else
						{
							go->movement_phase = Math::RandIntMinMax(1, 2);
							go->movement_change = Math::RandFloatMinMax(0.5f, 1);
						}

						go->angularVelocity = 0;
					}
				}
			}
		}
	}
}

void SceneAsteroid::MissileIdle(GameObject::GAMEOBJECT_TYPE GO)
{
	// Idle for GOs with missiles
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				if (go->idle)
				{
					// Prepare to attack
					if (go->movement_phase == 1 && go->movement_delay < go->movement_change)
					{
						go->aim = true;
						go->idle = false;
					}
					// Move forward
					else if (go->movement_phase == 2 && go->movement_delay < go->movement_change)
					{
						go->force += go->dir * 100;
						go->torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));
						go->turn = false;
					}
					// Prepare to attack with Missile
					else if (go->movement_phase == 3 && go->movement_delay < go->movement_change)
					{
						go->aim = true;
						go->idle = false;
						go->missile = true;
					}
					// Turn left
					else if (go->movement_phase == 4 && go->movement_delay < go->movement_change)
					{
						go->force += go->dir * 1;
						go->torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));
						go->turn = true;
					}
					// Turn right
					else if (go->movement_phase == 5 && go->movement_delay < go->movement_change)
					{
						go->force += go->dir * 1;
						go->torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));
						go->turn = true;
					}
					// reshuffle movement through RNG
					else
					{
						go->movement_delay = 0;
						if (go->turn == false)
						{
							go->movement_phase = Math::RandIntMinMax(3, 5);
							go->movement_change = Math::RandFloatMinMax(0.5f, 1);
						}
						else
						{
							go->movement_phase = Math::RandIntMinMax(1, 3);
							go->movement_change = Math::RandFloatMinMax(0.5f, 1);
						}

						go->angularVelocity = 0;
					}
				}
			}
		}
	}
}


void SceneAsteroid::AsteroidCollision(GameObject::GAMEOBJECT_TYPE GO, char side, int unit)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			// handling collision between GO and GO_ASTEROID
			if (go->type == GameObject::GO_ASTEROID)
			{
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->type == GO)
					{
						if ((go->pos - go2->pos).Length() <= (go->scale.Length() + go2->scale.Length()))
						{
							go2->hp -= 10;
							go->active = false;
							asteroid_spawned--;
							asteroid_delay = 0;

							// if GO die
							if (go2->hp <= 0)
							{
								go2->active = false;
								// Enemy
								if (side == 'e')
								{
									enemy_spawned[unit]--;
									enemy_count[unit]--;
									if (go2->type == GameObject::GO_FALCON)
										DestroyObject(GameObject::GO_FALCONTURRET);
								}
								// Ally
								else if (side == 'a')
								{
									ally_spawned[unit]--;
									ally_count[unit]--;
								}
							}
						}
					}
				}
			}
		}
	}
}

void SceneAsteroid::AllyBulletCollision(GameObject::GAMEOBJECT_TYPE GO, int unit, float range)
{
	// Ally bullet collision with enemy
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_BULLET || go->type == GameObject::GO_ALLY_BULLET || go->type == GameObject::GO_ALLY_MISSILE || go->type == GameObject::GO_BLUELASER || go->type == GameObject::GO_LASERBEAM)
			{
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;

					// Enemy ship hit by player or ally bullet
					if (go2->type == GO)
					{
						if (go2->active)
						{
							if ((go->pos - go2->pos).Length() <= (go->scale.Length() + (go2->scale.Length() - range)))
							{
								if (go->type != GameObject::GO_LASERBEAM)
									go->active = false;

								if (!heavy && go->type == GameObject::GO_BULLET)
								{
									go2->hp--;
								}
								else if (heavy && go->type == GameObject::GO_BULLET)
								{
									go2->hp -= 5;
								}
								else if (go->type == GameObject::GO_ALLY_MISSILE)
								{
									go2->hp -= 5;
								}
								else if (go->type == GameObject::GO_BLUELASER)
								{
									go2->hp -= 3;
								}
								else if (go->type == GameObject::GO_LASERBEAM && laser_delay > 0.05f && laser_width >= 2)
								{
									go2->hp--;
									laser_delay = 0;
								}

							}
							// if GO die
							if (go2->hp <= 0)
							{
								go2->active = false;
								// Check if turrets, ammend accordingly
								if (go2->type == GameObject::GO_TURRETSINGLE || go2->type == GameObject::GO_TURRETDOUBLE)
									turrets_left--;
								else
								{
									enemy_spawned[unit]--;
									enemy_count[unit]--;
									enemy_delay[unit] = 0;
								}
								if (go2->type == GameObject::GO_FALCON)
									DestroyObject(GameObject::GO_FALCONTURRET);
							}
						}

					}
				}
			}
		}
	}
}

void SceneAsteroid::EnemyBulletCollision(GameObject::GAMEOBJECT_TYPE GO, int unit, float range)
{
	//Collision with enemy bullet
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GameObject::GO_ENEMY_BULLET || go->type == GameObject::GO_ENEMY_MISSILE)
			{
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->active)
					{
						if (go2->type == GO)
						{
							if (go2->active)
							{
								if ((go->pos - go2->pos).Length() <= (go->scale.Length() + go2->scale.Length()) - range)
								{
									go->active = false;
									if (go->type == GameObject::GO_ENEMY_MISSILE)
									{
										go2->hp -= 5;
									}
									else
									{
										go2->hp--;
									}
								}

								// if GO die
								if (go2->hp <= 0)
								{
									go2->active = false;
									// Check if turrets, ammend accordingly
									if (go2->type == GameObject::GO_SINGLEBARREL || go2->type == GameObject::GO_DOUBLEBARREL || go2->type == GameObject::GO_MINITURRET)
										turrets_left--;
									else
									{
										ally_spawned[unit]--;
										ally_count[unit]--;
										ally_delay[unit] = 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void SceneAsteroid::TakeDMG(int dmg)
{
	// Special Case for m_ship due to shield 
	m_ship->shield -= dmg;
	m_ship->shield_delay = 0;

	// If shield broken, substract HP
	if (m_ship->shield <= 0)
	{
		m_ship->hp -= dmg;
	}
}

void SceneAsteroid::Aim(GameObject::GAMEOBJECT_TYPE GO, GameObject::GAMEOBJECT_TYPE GO2)
{
	// Ship targeting using vectors converted to angles
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				if (go->aim)
				{
					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject* go2 = (GameObject*)*it2;
						if (go2->active)
						{
							if (go2->type == GO2)
							{
								float dot = (go2->pos.y - go->pos.y);
								float det = (go2->pos.x - go->pos.x);
								float ObjAngle = Math::RadianToDegree(atan2(go->dir.y, go->dir.x));
								float ObjToObj2Angle = Math::RadianToDegree(atan2(dot, det));

								// If targetted object angle is lower
								if (ObjAngle > ObjToObj2Angle)
								{
									// Turn right
									go->force += go->dir * 5;
									go->torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));
									
									// If angle is reached within a 10 degree range, activate shoot
									if (ObjAngle >= ObjToObj2Angle - 5 && ObjAngle <= ObjToObj2Angle + 5)
									{
										// For ships
										if (go->fire_style == 1)
										{
											go->angularVelocity = 0;
											go->shoot = true;
											if (!go->reset)
											{
												go->shoot_delay = 0;
												go->reset = true;
											}
										}
										// For turrets
										else if (go->fire_style == 2)
										{
											go->angularVelocity = 0;
											go->shoot = true;
											go->shoot_delay = 0;
											go->aim = false;
										}
									}
								}
								// If targetted object angle is higher
								else if (ObjAngle < ObjToObj2Angle)
								{
									// Turn left
									go->force += go->dir * 5;
									go->torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));

									// If angle is reached within a 10 degree range, activate shoot
									if (ObjAngle >= ObjToObj2Angle - 5 && ObjAngle <= ObjToObj2Angle + 5)
									{
										// For ships
										if (go->fire_style == 1)
										{
											go->angularVelocity = 0;
											go->shoot = true;
											if (!go->reset)
											{
												go->shoot_delay = 0;
												go->reset = true;
											}
										}
										// For turrets
										else if (go->fire_style == 2)
										{
											go->angularVelocity = 0;
											go->shoot = true;
											go->shoot_delay = 0;
											go->aim = false;
										}
									}
								}
							}
						}
					}
				}

				// When shooting, move forward
				if (go->shoot)
				{
					if (go->shoot_delay > go->pause)
					{
						go->force += go->dir * 100;
						go->torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));

						// After a certain period of shooting, reset
						if (go->shoot_delay > go->shoot_period)
						{
							if (go->fire_style == 1)
							{
								go->shoot = false;
								go->idle = true;
								go->aim = false;
								go->missile = false;
								go->reset = false;
							}
							else if (go->fire_style == 2)
							{
								go->shoot = false;
								go->idle = true;
								go->aim = true;
							}
						}
					}
				}
			}
		}
	}

}

void SceneAsteroid::AimPlayer(GameObject::GAMEOBJECT_TYPE GO)
{
	// Same as Aim() but for player
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			if (go->type == GO)
			{
				if (go->aim)
				{
					float dot = (m_ship->pos.y - go->pos.y);
					float det = (m_ship->pos.x - go->pos.x);
					float ObjAngle = Math::RadianToDegree(atan2(go->dir.y, go->dir.x));
					float ObjToObj2Angle = Math::RadianToDegree(atan2(dot, det));

					// If targetted object angle is lower
					if (ObjAngle > ObjToObj2Angle)
					{
						// Turn right
						go->force += go->dir * 5;
						go->torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));

						// If angle is reached within a 10 degree range, activate shoot
						if (ObjAngle >= ObjToObj2Angle - 5 && ObjAngle <= ObjToObj2Angle + 5)
						{
							// For ships
							if (go->fire_style == 1)
							{
								go->angularVelocity = 0;
								go->shoot = true;
								if (!go->reset)
								{
									go->shoot_delay = 0;
									go->reset = true;
								}
							}
							// For turrets
							else if (go->fire_style == 2)
							{
								go->angularVelocity = 0;
								go->shoot = true;
								go->shoot_delay = 0;
								go->aim = false;
							}
						}
					}
					// If targetted object angle is higher
					else if (ObjAngle < ObjToObj2Angle)
					{
						// Turn left
						go->force += go->dir * 5;
						go->torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));

						// If angle is reached within a 10 degree range, activate shoot
						if (ObjAngle >= ObjToObj2Angle - 5 && ObjAngle <= ObjToObj2Angle + 5)
						{
							// For ships
							if (go->fire_style == 1)
							{
								go->angularVelocity = 0;
								go->shoot = true;
								if (!go->reset)
								{
									go->shoot_delay = 0;
									go->reset = true;
								}
							}
							// For turrets
							else if (go->fire_style == 2)
							{
								go->angularVelocity = 0;
								go->shoot = true;
								go->shoot_delay = 0;
								go->aim = false;
							}
						}
					}
				}

				// When shooting, move forward
				if (go->shoot)
				{
					if (go->shoot_delay > go->pause)
					{
						go->force += go->dir * 100;
						go->torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));

						// After a certain period of shooting, reset
						if (go->shoot_delay > go->shoot_period)
						{
							if (go->fire_style == 1)
							{
								go->shoot = false;
								go->idle = true;
								go->aim = false;
								go->missile = false;
								go->reset = false;
							}
							else if (go->fire_style == 2)
							{
								go->shoot = false;
								go->idle = true;
								go->aim = true;
							}
						}
					}
				}
			}
		}
	}

}

void SceneAsteroid::SpawnAlly(int count, int max, int unit, double spawntime)
{
	// Set ally ship values
	ally_count[unit] = count;
	ally_spawntime[unit] = spawntime;
	ally_max[unit] = max;
	ally_spawned[unit] = 0;
	ally_delay[unit] = 0;
	spawn_ally[unit] = true;
}

void SceneAsteroid::SpawnEnemy(int count, int max, int unit, double spawntime)
{
	// Set enemy ship values
	enemy_count[unit] = count;
	enemy_spawntime[unit] = spawntime;
	enemy_max[unit] = max;
	enemy_spawned[unit] = 0;
	enemy_delay[unit] = 0;
	spawn_enemy[unit] = true;
}

void SceneAsteroid::SpawnAsteroid(int count, double spawntime)
{
	// Set asteroid spawnrate
	asteroid_count = count;
	asteroid_spawntime = spawntime;
	asteroid_spawned = 0;
	asteroid_delay = 0;
}

void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);
	// Calculating aspect ratio
	// Update screen space
	m_screenHeight = 100.f;
	m_screenWidth = m_screenHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	menu_delay += dt;

	// Before game state
	if (state == STATE_MENU)
	{
		if (Application::IsKeyPressed(VK_SPACE) && menu_delay > 0.25f)
		{
			state = STATE_CONTROLS;
			menu_delay = 0;
		}
	}
	else if (state == STATE_CONTROLS)
	{
		if (Application::IsKeyPressed(VK_SPACE) && menu_delay > 0.25f)
		{
			state = STATE_ABOUT;
			menu_delay = 0;
		}
	}
	else if (state == STATE_ABOUT)
	{
		if (Application::IsKeyPressed(VK_SPACE) && menu_delay > 1)
		{
			state = STATE_PLAY;
			menu_delay = 0;
		}
	}
	else if (state == STATE_WIN || state == STATE_LOSE || state == STATE_LOSE2)
	{
		if (Application::IsKeyPressed(VK_RETURN) && menu_delay > 1)
		{
			state = STATE_PLAY;
			menu_delay = 0;
		}
	}
	// During game state
	else if (state == STATE_PLAY)
	{
		// Set all timers
		fps = 1 / dt;
		dmg_delay += dt;
		scrapmetal_delay += dt;
		for (int i = 0; i < 4; ++i)
		{
			enemy_delay[i] += dt;
			ally_delay[i] += dt;
		}
		activation_delay += dt;
		state_delay += dt;
		m_ship->bullet_delay += dt;
		repair_delay += dt;
		repair_timer += dt;
		arrow_delay += dt;
		laser_delay += dt;
		survival_timer -= dt;
		powerup_timer -= dt;
		powerup_delay += dt;
		m_ship->shield_delay += dt; 
		m_ship->shield_recharge += dt;

		// Call levels, GOs and Gameplay functions
		CreateLvl();
		CreateGOs(dt);
		Gameplay(dt);

		// Change hp color to red for a second when dmg taken
		if (dmg_delay < 1 && m_ship->shield <= 0)
			health_color = Color(1, 0, 0);
		else
			health_color = Color(0, 1, 0);

		// Flash "integrity in critical condition" text
		if (state_delay < 1)
			state_color = Color(1, 0, 0);
		else if (state_delay > 1 && state_delay < 2)
			state_color = Color(0, 1, 0);
		else
			state_delay = 0;

		// lose condition
		if (m_ship->hp <= 0)
			state = STATE_LOSE;

		// Emergency repair
		if (emergency_repair)
		{
			if (repair_timer > 0.025f && repair_delay < 5 && m_ship->hp < 100)
			{
				m_ship->hp += 1;
				repair_timer = 0;
			}
			else if (repair_delay > 65)
			{
				emergency_repair = false;
			}
		}

		// Ship recharge
		if (m_ship->shield_recharge > 0.1f && m_ship->shield_delay > 5 && m_ship->shield < 20)
		{
			m_ship->shield += 1;
			m_ship->shield_recharge = 0;
		}

		m_force.SetZero();
		m_torque.SetZero();

		// Ship movement controls
		if (Application::IsKeyPressed('W') || Application::IsKeyPressed(VK_UP))
		{
			m_force += m_ship->dir * 100;
			m_torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));
		}
		if (Application::IsKeyPressed('A') || Application::IsKeyPressed(VK_LEFT))
		{
			m_force += m_ship->dir * 5;
			m_torque = Vector3(1, -1, 0).Cross(Vector3(0, 50, 0));
		}
		if (Application::IsKeyPressed('S') || Application::IsKeyPressed(VK_DOWN))
		{
			m_force -= m_ship->dir * 100;
			m_torque = Vector3(0, 1, 0).Cross(Vector3(0, -100, 0));
		}
		if (Application::IsKeyPressed('D') || Application::IsKeyPressed(VK_RIGHT))
		{
			m_force += m_ship->dir * 5;
			m_torque = Vector3(-1, -1, 0).Cross(Vector3(0, 50, 0));
		}
		if (Application::IsKeyPressed('R') && !emergency_repair && m_ship->hp < 100)
		{
			emergency_repair = true;
			repair_delay = 0;
		}

		// Linear motion for m_ship
		Vector3 m_accel = m_force * (1.f / m_ship->mass);

		m_ship->vel += m_accel * dt * m_speed;
		m_ship->pos += m_ship->vel * dt * m_speed;
		if (m_ship->vel.Length() > MAX_SPEED)
		{
			m_ship->vel = m_ship->vel.Normalize() * MAX_SPEED;
		}

		m_force = Vector3(0, 0, 0);

		// Angular motion for m_ship

		float alpha = m_torque.z / m_ship->momentOfInertia;

		m_ship->angularVelocity += alpha * dt;

		m_ship->angularVelocity = Math::Clamp(m_ship->angularVelocity, -5.0f, 5.0f);//Zhi Wei jape paul
		float dTheta = m_ship->angularVelocity * dt;

		m_ship->dir.Set(m_ship->dir.x * cosf(dTheta) - m_ship->dir.y * sinf(dTheta),
		m_ship->dir.x * sinf(dTheta) + m_ship->dir.y * cosf(dTheta)); //turn a vector by angle

		// Wrap ship position if it leaves screen
		WrapAround(GameObject::GO_SHIP, 3);
	}
		
}

void SceneAsteroid::RenderGO(GameObject *go)
{
	glDisable(GL_DEPTH_TEST);
	switch(go->type)
	{
	case GameObject::GO_SHIP:
		modelStack.PushMatrix();
		modelStack.Translate(m_ship->pos.x, m_ship->pos.y, m_ship->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(m_ship->dir.y, m_ship->dir.x)), 0, 0, 1);
		modelStack.Scale(m_ship->scale.x, m_ship->scale.y, m_ship->scale.z);
		RenderMesh(meshList[GEO_TIE_ADVANCED], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ASTEROID:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID], false);
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

	case GameObject::GO_BLUELASER:
		//Excerise 4c: Render a sphere with radius 0.2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLUELASER], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_LASERBEAM:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_LASERBEAM], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ENEMY_SHIP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMY_SHIP], false);
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

	case GameObject::GO_ENEMY_MISSILE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MISSILE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ALLY_MISSILE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MISSILE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_DEATHSTAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_DEATHSTAR], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_SCRAPMETAL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SCRAPMETAL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_TANTIVE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(90, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_TANTIVE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_TURRETSINGLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_TURRETSINGLE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_TURRETDOUBLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_TURRETDOUBLE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ALLY_SHIP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIP], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_TIE_BOMBER:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_TIE_BOMBER], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ALLY_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BULLET], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_INTERCEPTOR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_INTERCEPTOR], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_YWING:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_YWING], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_AWING:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_AWING], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_IMPERIAL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(90, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_IMPERIAL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FALCON:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FALCON], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FALCONTURRET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_FALCONTURRET], false);
		modelStack.PopMatrix();
		break;


	case GameObject::GO_SINGLEBARREL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SINGLEBARREL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_DOUBLEBARREL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_DOUBLEBARREL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_MINITURRET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MINITURRET], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_CANNON:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CANNON], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_RIFLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_RIFLE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_LASERGUN:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_LASERGUN], false);
		modelStack.PopMatrix();
		break;
	}

	glEnable(GL_DEPTH_TEST);
}

void SceneAsteroid::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	if (state == STATE_PLAY)
	{
		// Clamp screen space if reached end of world space
		float clamp_screen_x = Math::Clamp((m_ship->pos.x - (m_screenWidth * .5f)), .0f, m_worldWidth - m_screenWidth);
		float clamp_screen_y = Math::Clamp((m_ship->pos.y - (m_screenHeight * .5f)), .0f, m_worldHeight - m_screenHeight);
		projection.SetToOrtho(clamp_screen_x, clamp_screen_x + m_screenWidth, clamp_screen_y, clamp_screen_y + m_screenHeight, -10, 10);
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

	// Background in - game
	if (state == STATE_PLAY)
	{
		modelStack.PushMatrix();
		modelStack.Translate(50, 50, 0);
		modelStack.Scale(50, 50, 1);
		RenderMesh(meshList[GEO_BACKGROUND], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(150, 50, 0);
		modelStack.Scale(50, 50, 1);
		RenderMesh(meshList[GEO_BACKGROUND], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(50, 150, 0);
		modelStack.Scale(50, 50, 1);
		RenderMesh(meshList[GEO_BACKGROUND], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(150, 150, 0);
		modelStack.Scale(50, 50, 1);
		RenderMesh(meshList[GEO_BACKGROUND], false);
		modelStack.PopMatrix();
	}
	// Menu background image
	else if (state == STATE_MENU || state == STATE_CONTROLS || state == STATE_ABOUT)
	{
		modelStack.PushMatrix();
		modelStack.Translate(100, 100, 0);
		modelStack.Scale(100, 100, 1);
		RenderMesh(meshList[GEO_MENU], false);
		modelStack.PopMatrix();
	}
	// Win background image
	else if (state == STATE_WIN)
	{
		modelStack.PushMatrix();
		modelStack.Translate(100, 100, 0);
		modelStack.Scale(100, 100, 1);
		RenderMesh(meshList[GEO_WIN], false);
		modelStack.PopMatrix();
	}
	// Lose condition 1
	else if (state == STATE_LOSE)
	{
		modelStack.PushMatrix();
		modelStack.Translate(100, 100, 0);
		modelStack.Scale(100, 100, 1);
		RenderMesh(meshList[GEO_LOSE], false);
		modelStack.PopMatrix();
	}
	// Lose condition 2
	else if (state == STATE_LOSE2)
	{
		modelStack.PushMatrix();
		modelStack.Translate(100, 100, 0);
		modelStack.Scale(100, 100, 1);
		RenderMesh(meshList[GEO_DESTROYED], false);
		modelStack.PopMatrix();
	}

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}


	RenderGO(m_ship);


	// On screen text
	if (state == STATE_PLAY)
	{
		std::ostringstream ss7;
		ss7.str("");

		// Render UI 
		if (!m_ship->laser && !m_ship->cannon && !m_ship->rifle)
		{
			if (heavy)
				ss7 << "Heavy Mode: On";
			else if (!heavy)
				ss7 << "Heavy Mode: Off";
		}
		else if (m_ship->laser)
			ss7 << "Lasergun Mounted";
		else if (m_ship->cannon)
			ss7 << "Rockets Loaded";
		else if (m_ship->rifle)
			ss7 << "Rifle Turret Mounted";
		RenderTextOnScreen(meshList[GEO_TEXT], ss7.str(), Color(0, 1, 0), 2.5f, 0, 3);

		std::ostringstream ss8;
		ss8.str("");
		ss8 << "FPS: " << fps;
		RenderTextOnScreen(meshList[GEO_TEXT], ss8.str(), Color(0, 1, 0), 2.5f, 73, 57);

		std::ostringstream ss2;
		ss2.str("");
		ss2 << "HP:" << m_ship->hp;
		RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), health_color, 2.5f, 0, 57);

		std::ostringstream ss3;
		ss3.str("");

		std::ostringstream ss4;
		ss4.str("");

		// Top left indicator for counters
		if (indicator == INDICATOR_ONE)
			ss3 << "Asteroids destroyed: " << asteroid_destroyed << "/10";
		else if (indicator == INDICATOR_TWO)
			ss3 << "Enemies left: " << enemy_count[0] + enemy_count[1] + enemy_count[2];
		else if (indicator == INDICATOR_THREE)
			ss3 << "Turrets left: " << turrets_left;
		else if (indicator == INDICATOR_FOUR)
		{
			if (((int)survival_timer % 60) < 10)
				ss3 << "Time until reinforcements arrive: " << ((int)survival_timer / 60) << ":0" << (int)survival_timer % 60;
			else
				ss3 << "Time until reinforcements arrive: " << ((int)survival_timer / 60) << ":" << (int)survival_timer % 60;
		}
		else if (indicator == INDICATOR_FIVE)
		{
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->active)
				{
					if (go->type == GameObject::GO_FALCON)
					{
						ss3 << "Millenium Falcon HP:";
						RenderMeshOnScreen(meshList[GEO_CHARGEBG], 8, 54, 16, 2);
						RenderMeshOnScreen(meshList[GEO_COOLDOWN], -8 + (0.026667f * go->hp), 54, 16, 2);
					}
				}
			}
		}

	    if (reinforcements)
			ss4 << "Reinforcements remaining: " << ally_count[0] + ally_count[1] + ally_count[2];

		RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(0, 1, 0), 2.5f, 0, 55);
		RenderTextOnScreen(meshList[GEO_TEXT], ss4.str(), Color(0, 1, 0), 2.5f, 56, 53);
		
		// Debugging purposes
	    //std::ostringstream ss5;
		//ss5.str("");
		//ss5 << "ship pos: " << "[" << m_ship->pos.x << ", " << m_ship->pos.y << ", " << m_ship->pos.z << "]";
		//RenderTextOnScreen(meshList[GEO_TEXT], ss5.str(), Color(0, 1, 0), 3, 0, 18);*/

		//std::ostringstream ss6;
		//ss6.str("");
		//ss6 << "ship mass: " << m_ship->mass;
		//RenderTextOnScreen(meshList[GEO_TEXT], ss6.str(), Color(0, 1, 0), 3, 0, 21);

		std::ostringstream ss9;
		ss9.str("");

		// Charge / cooldown bar from shooting
		RenderMeshOnScreen(meshList[GEO_CHARGEBG], 10, 2, 20, 1);
		if (!m_ship->laser && !m_ship->cannon && !m_ship->rifle)
		{
			if (heavy)
				RenderMeshOnScreen(meshList[GEO_CHARGE], -10 + m_charge * 20, 2, 20, 1);
			else if (!cooldown)
				RenderMeshOnScreen(meshList[GEO_CHARGE], -10 + m_charge * 6.6667, 2, 20, 1);
			else if (cooldown)
				RenderMeshOnScreen(meshList[GEO_COOLDOWN], -10 + m_charge * 6.6667, 2, 20, 1);
		}
		else
			RenderMeshOnScreen(meshList[GEO_CHARGE], -10 + powerup_timer * 0.666667f, 2, 20, 1);

		// Emergency repair bar
		RenderMeshOnScreen(meshList[GEO_CHARGEBG], 70, 2, 20, 2);
		if (emergency_repair)
		{
			if (repair_delay > 5)
			{
				RenderMeshOnScreen(meshList[GEO_COOLDOWN], 90 - repair_delay * 0.3f, 2, 20, 2);
				RenderTextOnScreen(meshList[GEO_TEXT], "E - REPAIR RECHARGING", Color(0, 1, 0), 2, 60, 4);
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_COOLDOWN], 70 + repair_delay * 4, 2, 20, 2);
			}
		}
		else if (!emergency_repair)
		{
			RenderMeshOnScreen(meshList[GEO_CHARGE], 70, 2, 20, 2);
			RenderTextOnScreen(meshList[GEO_TEXT],"EMERGENCY REPAIR READY", Color(0, 1, 0), 2, 60, 4);
		}

		// Shield integrity bar
		RenderMeshOnScreen(meshList[GEO_CHARGEBG], 70, 8, 20, 2);
		RenderMeshOnScreen(meshList[GEO_CHARGE], 90 - m_ship->shield, 8, 20, 2);
		if (m_ship->shield_delay > 5 && m_ship->shield >= 20)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "SHIELD ACTIVE", Color(0, 1, 0), 2, 60, 10);
		}
		else if (m_ship->shield_delay < 5 && m_ship->shield > 0 && m_ship->shield < 20)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "TAKING DAMAGE", Color(0, 1, 0), 2, 60, 10);
		}
		else if (m_ship->shield_delay > 5 && m_ship->shield < 20)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "RECHARGING", Color(0, 1, 0), 2, 60, 10);
		}
		else if (m_ship->shield <= 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "SHIELD BROKEN", Color(0, 1, 0), 2, 60, 10);
		}
		
		// Warning indicators (Displayed top center of screen)
		if (cooldown)
		{
			ss9 << "- GUN OVERHEATED, COOLING DOWN -";
			RenderTextOnScreen(meshList[GEO_TEXT], ss9.str(), state_color, 3, 23, 57);
		}
		else if (emergency_repair && repair_delay < 5)
		{
			ss9 << "- EMERGENCY REPAIR IN PROGRESS -";
			RenderTextOnScreen(meshList[GEO_TEXT], ss9.str(), state_color, 3, 23, 57);
		}
		else if (m_ship->hp <= 20)
		{
			ss9 << "- INTEGRITY IN CRITICAL STATE -";
			RenderTextOnScreen(meshList[GEO_TEXT], ss9.str(), state_color, 3, 23, 57);
		}
	}
	// Text on screen before game starts
	else if (state == STATE_MENU)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "STAR WARS EPISODE IV", Color(0, 1, 0), 5, 20, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [SPACE] to start", Color(0, 1, 0), 3, 30, 25);
	}
	else if (state == STATE_CONTROLS)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "CONTROLS", Color(0, 1, 0), 5, 30, 55);
		RenderTextOnScreen(meshList[GEO_TEXT], "W/A/S/D or Arrow Keys to move", Color(0, 1, 0), 4, 3, 45);
		RenderTextOnScreen(meshList[GEO_TEXT], "[SPACE] to shoot", Color(0, 1, 0), 4, 3, 40);
		RenderTextOnScreen(meshList[GEO_TEXT], "[LEFT_CTRL] to toggle heavy mode", Color(0, 1, 0), 4, 3, 35);
		RenderTextOnScreen(meshList[GEO_TEXT], "Hold [SPACE] to charge attack while heavy mode", Color(0, 1, 0), 4, 3, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [R] to activate emergency repair when ready", Color(0, 1, 0), 4, 3, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [SPACE] to continue", Color(0, 1, 0), 3, 30, 10);
	}
	else if (state == STATE_ABOUT)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "ABOUT", Color(0, 1, 0), 5, 35, 55);
		RenderTextOnScreen(meshList[GEO_TEXT], "You are an Imperial Officer tasked with using the TIE", Color(0, 1, 0), 4, 3, 50);
		RenderTextOnScreen(meshList[GEO_TEXT], "Advanced x1, an advanced prototype starfighter armed with ", Color(0, 1, 0), 4, 3, 45);
		RenderTextOnScreen(meshList[GEO_TEXT], "many features normal TIE starfighters do not have.", Color(0, 1, 0), 4, 3, 40);
		RenderTextOnScreen(meshList[GEO_TEXT], "Instructions given directly by Supreme Commander Darth", Color(0, 1, 0), 4, 3, 35);
		RenderTextOnScreen(meshList[GEO_TEXT], "Vader state to weaken the Rebellion Corvette (Tantive IV),", Color(0, 1, 0), 4, 3, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "commanded by rebel spy Princess Leia Organa. You are", Color(0, 1, 0), 4, 3, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "then tasked to clear any remaining rebellion forces and", Color(0, 1, 0), 4, 3, 20);
		RenderTextOnScreen(meshList[GEO_TEXT], "return to the Death Star for further instructions.", Color(0, 1, 0), 4, 3, 15);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [SPACE] to continue", Color(0, 1, 0), 3, 30, 5);
	}
	// Win / Lose text
	// Reset game per condition
	else if (state == STATE_WIN)
	{
		Reset();
		m_ship->active = false;
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU HAVE DESTROYED THE REBELS", Color(0, 1, 0), 5, 10, 30);
	    RenderTextOnScreen(meshList[GEO_TEXT], "Press [ENTER] to restart", Color(0, 1, 0), 3, 30, 25);
	}
	else if (state == STATE_LOSE)
	{
		Reset();
		m_ship->active = false;
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU DIED", Color(1, 0, 0), 5, 33, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [ENTER] to restart", Color(0, 1, 0), 3, 30, 25);
	}
	else if (state == STATE_LOSE2)
	{
		Reset();
		m_ship->active = false;
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU FAILED TO DEFEND THE SHIP", Color(1, 0, 0), 5, 10, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [ENTER] to restart", Color(0, 1, 0), 3, 30, 25);
	}

	// Next arrows for left and right
	if (next_left)
	{
		if (arrow_delay < 1)
		{
			RenderMeshOnScreen(meshList[GEO_LEFT_ARROW], 5, 50, 5, 3.5);
			RenderTextOnScreen(meshList[GEO_TEXT], "<<<NEXT>>>", Color(1, 0, 0), 2.5f, 1, 43);
		}
		else if (arrow_delay > 2)
		{
			arrow_delay = 0;
		}
	}
	if (next_right)
	{
		if (arrow_delay < 1)
		{
			RenderMeshOnScreen(meshList[GEO_RIGHT_ARROW], 75, 50, 5, 3.5);
			RenderTextOnScreen(meshList[GEO_TEXT], "<<<NEXT>>>", Color(1, 0, 0), 2.5f, 69, 43);
		}
		else if (arrow_delay > 2)
		{
			arrow_delay = 0;
		}
	}

	// Objectives 
	if (objective == OBJECTIVE_ZERO)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Proceed to next level", Color(0, 1, 0), 2.5f, 55, 55);
	else if (objective == OBJECTIVE_ONE)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Destroy the asteroids", Color(0, 1, 0), 2.5f, 55, 55);
	else if (objective == OBJECTIVE_TWO)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Eliminate the Rebellion Ships", Color(0, 1, 0), 2.5f, 49, 55);
	else if (objective == OBJECTIVE_THREE)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Destroy the turrets", Color(0, 1, 0), 2.5f, 57, 55);
	else if (objective == OBJECTIVE_FOUR)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Return to the Death Star", Color(0, 1, 0), 2.5f, 52.5f, 55);
	else if (objective == OBJECTIVE_FIVE)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Defend the Imperial Star Destroyer", Color(0, 1, 0), 2.5f, 44.5f, 55);
	else if (objective == OBJECTIVE_SIX)
		RenderTextOnScreen(meshList[GEO_TEXT], "Objective: Defeat the Millenium Falcon", Color(0, 1, 0), 2.5f, 50, 55);
}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}
