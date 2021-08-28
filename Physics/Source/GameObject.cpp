#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	reset(false),
	initPos(false),
	open(false),
	left(false),
	right(false),
	attack(false),
	rage(false),
	attacking(false),
	prevpos(0),
	gold_count(0),
	item_count(0),
	frame_count{ 0,0 },
	mass(1.f),
	dir(1, 0, 0),
	normal(1, 0, 0),
	momentOfInertia(1.f),
	angularVelocity(0.f),
	hp(0),
	max_hp(0),
	mana(0),
	shield(0),
	shield_delay(0),
	shield_recharge(0),
	force(0, 0, 0),
	fire_rate(0),
	bullet_delay(0),
	portal_delay(0),
	movement_delay(0),
	movement_phase(2),
	movement_change(1),
	shoot_delay(0),
	pause(0),
	shoot_period(0),
	animation_delay(0),
	FSMCounter(0),
	MaxFSMCounter(1),
	speed(0),
	weapon_type(0),
	state(STATE_IDLE)
{

}

GameObject::~GameObject()
{
}