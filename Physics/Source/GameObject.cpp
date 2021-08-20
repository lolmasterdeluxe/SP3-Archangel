#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	turn(false),
	shoot(false),
	aim(false),
	idle(true),
	reset(false),
	missile(false),
	bounce(false),
	initPos(false),
	color(Math::RandIntMinMax(0, 7)),
	prevpos(0),
	grenade_count(0),
	gold_count(0),
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
	torque(0, 0, 0),
	fire_rate(0),
	bullet_delay(0),
	portal_delay(0),
	movement_delay(0),
	movement_phase(2),
	movement_change(1),
	bullet_count(0),
	shoot_delay(0),
	grenade_delay(0),
	pause(0),
	shoot_period(0),
	fire_style(0),
	animation_delay(1)
{

}

GameObject::~GameObject()
{
}