#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneAsteroid : public SceneBase
{
	static const int MAX_SPEED = 50;
	static const int ENEMY_MAX_SPEED = 40;
	static const int BULLET_SPEED = 100;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	

	GameObject* FetchGO();

protected:
	// Game state
	enum GAME_STATE
	{
		STATE_NONE = 0,
		STATE_MENU,
		STATE_CONTROLS,
		STATE_ABOUT,
		STATE_PLAY,
		STATE_WIN,
		STATE_LOSE,
		STATE_LOSE2
	};
	// Levels
	enum GAME_LEVEL
	{
		LVL_ZERO = 0,
		LVL_ONE,
		LVL_TWO,
		LVL_THREE,
		LVL_FOUR,
		LVL_FIVE,
		LVL_SIX,
		LVL_SEVEN,
		LVL_EIGHT,
		LVL_NINE,
		LVL_TEN
	};
	// Current objectives
	enum GAME_OBJECTIVE
	{
		OBJECTIVE_ZERO = 0,
		OBJECTIVE_ONE,
		OBJECTIVE_TWO,
		OBJECTIVE_THREE,
		OBJECTIVE_FOUR,
		OBJECTIVE_FIVE,
		OBJECTIVE_SIX,
	};
	// Left indicators for counters
	enum GAME_INDICATOR
	{
		INDICATOR_ZERO = 0,
		INDICATOR_ONE,
		INDICATOR_TWO,
		INDICATOR_THREE,
		INDICATOR_FOUR,
		INDICATOR_FIVE
	};

	// Declare enum objects
	GAME_LEVEL level;
	GAME_STATE state;
	GAME_OBJECTIVE objective;
	GAME_INDICATOR indicator;

	// Gameplay functions
	void Nextlevel(GAME_LEVEL level, int direction); 
	void DestroyObject(GameObject::GAMEOBJECT_TYPE typeValue);
	void Reset();
	void ResetLvl();
	void CreateGOs(double &dt);
	void CreateLvl();
	void Gameplay(double &dt);
	void linearVel(GameObject::GAMEOBJECT_TYPE GO, double &dt);
	void angularVel(GameObject::GAMEOBJECT_TYPE GO, float min, float max, double &dt);
	void WrapAround(GameObject::GAMEOBJECT_TYPE GO, int choice);
	void Idle(GameObject::GAMEOBJECT_TYPE GO);
	void Aim(GameObject::GAMEOBJECT_TYPE GO, GameObject::GAMEOBJECT_TYPE GO2);
	void AimPlayer(GameObject::GAMEOBJECT_TYPE GO);
	void SpawnAlly(int ally_c, int ally_m, int i, double ally_sp);
	void SpawnEnemy(int count, int max, int unit, double spawntime);
	void SpawnAsteroid(int count, double spawntime);
	void MissileIdle(GameObject::GAMEOBJECT_TYPE GO);
	void AsteroidCollision(GameObject::GAMEOBJECT_TYPE GO, char side, int unit);
	void AllyBulletCollision(GameObject::GAMEOBJECT_TYPE GO, int unit, float range);
	void EnemyBulletCollision(GameObject::GAMEOBJECT_TYPE GO, int unit, float range);
	void TakeDMG(int);

	// Declare Variables
	std::vector<GameObject *> m_goList;
	float m_speed, m_worldWidth, m_worldHeight, m_screenWidth, m_screenHeight, m_charge, laser_width;
	bool heavy, charged, cooldown, death_star, tantive, imperial, next_left, next_right, init, init2, emergency_repair, spawn_ally[4], spawn_enemy[4], laser, reinforcements;
	GameObject *m_ship, *deathstar;
	Vector3 m_force, m_torque;
	Color health_color, state_color;
	int m_objectCount, fps, asteroid_count, asteroid_spawned, enemy_count[4], enemy_spawned[4], ally_spawned[4], ally_count[4], scrapmetal_count, powerup_count, turrets_left, asteroid_destroyed, enemy_max[4], ally_max[4];
	double asteroid_delay, asteroid_spawntime, enemy_delay[4], enemy_spawntime[4], activation_delay, scrapmetal_delay, powerup_delay, dmg_delay, shoot_delay, state_delay, menu_delay, arrow_delay, repair_delay, laser_delay, repair_timer, ally_delay[4], ally_spawntime[4], survival_timer, powerup_timer;
};

#endif