#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include <sstream>

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CIRCLE,
		GO_GHOSTBALL,
		GO_CUBE,
		GO_ASTEROID, //asteroid
		GO_SHIP, //player ship
		GO_GRENADE, //grenade
		GO_ALLY_BULLET,
		GO_ENEMY_SHIP, //enemy ship
		GO_YWING,
		GO_AWING,
		GO_ALLY_SHIP,
		GO_INTERCEPTOR, //ally ship 2
		GO_TIE_BOMBER,
		GO_ALLY_MISSILE,
		GO_ENEMY_MISSILE, //player missile
		GO_POWERUP, //powerup item
		GO_DEATHSTAR,
		GO_SCRAPMETAL,
		GO_TANTIVE,
		GO_IMPERIAL,
		GO_TURRETSINGLE,
		GO_TURRETDOUBLE,
		GO_SINGLEBARREL,
		GO_DOUBLEBARREL,
		GO_MINITURRET,
		GO_FALCON,
		GO_FALCONTURRET,
		GO_CANNON,
		GO_RIFLE,
		GO_LASERGUN,
		GO_BLUELASER,
		GO_LASERBEAM,

		// Enemies GO
		GO_DEMON,
		GO_FALLENANGEL,
		GO_TERMINATOR,
		GO_SOLDIER,

		// Bullet GO
		GO_BULLET, //player bullet
		GO_ENEMY_BULLET, //enemy bullet

		// Wall / Interactable Objects GO
		GO_WALL,
		GO_PILLAR,
		GO_PORTAL_IN,
		GO_PORTAL_OUT,
		GO_POTION,
		GO_MAXPOTION,
		GO_MANAPOTION,
		GO_GOLD,
		GO_CHEST,
		GO_BARREL,
		GO_FIREBALL,
		GO_PLATFORM,
		GO_BOUNCEPLATFORM,

		GO_TOTAL, //must be last
	};
	enum STATE
	{
		STATE_IDLE = 0,
		STATE_PATROL,
		STATE_CLOSE_ATTACK,
		STATE_FAR_ATTACK,
		STATE_RAGE,
		STATE_RELOAD,
		STATE_TOTAL
	};
	GAMEOBJECT_TYPE type;
	STATE state;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 force;
	Vector3 torque;
	Vector3 normal;
	GameObject* pillar1;
	GameObject* pillar2;
	GameObject* pillar3;
	GameObject* pillar4;
	GameObject* under_box;
	GameObject* left_box;
	GameObject* right_box;

	// Conditions and variables for GO
	bool active, turn, shoot, aim, idle, missile, reset, cannon, laser, rifle, bounce, initPos, left, right, attack, rage;
	float mass, pause, shoot_period, prevpos, mana, speed;
	int hp, max_hp, shield, movement_phase, bullet_count, fire_style, color, grenade_count, gold_count, item_count;
	double bullet_delay, grenade_delay, portal_delay, movement_delay, fire_rate, shoot_delay, shield_delay, shield_recharge, animation_delay, FSMCounter, MaxFSMCounter;
	float movement_change;
	std::ostringstream hp_display;

	Vector3 dir; //direction/orientation
	float momentOfInertia;
	float angularVelocity; //in radians

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif