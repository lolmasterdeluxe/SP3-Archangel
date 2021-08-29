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

		//UI MENU
		GO_BUTTON,

		// Enemies GO
		GO_DEMON,
		GO_FALLENANGEL,
		GO_TERMINATOR,
		GO_SOLDIER,
		GO_DEMONLORD,
		GO_METALGEAR,
		GO_RAMBO,

		// Bullet GO
		GO_BULLET,		 //player bullet
		GO_ENEMY_BULLET, //enemy bullet
		GO_MISSILE,      //enemy missile
		GO_MG_MISSILE,   //metal gear's missile

		// Wall / Interactable Objects GO
		GO_WALL,
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
		GO_REALMCHANGER,

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
		STATE_RETREAT,
		STATE_TOTAL
	};
	GAMEOBJECT_TYPE type;
	STATE state;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 force;
	Vector3 normal;
	GameObject* under_box;
	GameObject* left_box;
	GameObject* right_box;

	// Conditions and variables for GO
	bool active, reset, initPos, open, left, right, attack, rage, attacking, retreat;
	float mass, pause, shoot_period, prevpos, mana, speed;
	int hp, max_hp, shield, movement_phase, gold_count, item_count, frame_count[2], weapon_type;
	double bullet_delay, portal_delay, movement_delay, fire_rate, shoot_delay, shield_delay, shield_recharge, animation_delay, FSMCounter, MaxFSMCounter;
	float movement_change;
	std::ostringstream hp_display;

	std::string goTag;

	Vector3 dir; //direction/orientation
	float momentOfInertia;
	float angularVelocity; //in radians

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif