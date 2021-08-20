#ifndef SCENE_ARCHANGEL_H
#define SCENE_ARCHANGEL_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "CMapStorage.h"
#include "Collision.h"

class SceneArchangel : public SceneBase
{
public:
	SceneArchangel();
	~SceneArchangel();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();
	void ReturnGO(GameObject::GAMEOBJECT_TYPE GO);
	Collision CheckCollision(GameObject* go1, GameObject* go2, float dt);
	void PhysicsResponse(GameObject* go1, Collision collision);
	void CollisionBound(GameObject* go1, Collision collision);
	void Gravity(GameObject::GAMEOBJECT_TYPE GO, float elasticity, double dt);
	void SpawnBullet(double dt);
	void throwGrenade(double dt);
	void playerLogic(double dt);
	void portalLogic(double dt);
	void itemLogic(double dt);
	void activatePortal(GameObject* go);
	void enableCollision(double dt, GameObject::GAMEOBJECT_TYPE GO);
	void Boundary(GameObject* go, int choice);
	void setGun(float fire_rate, int dmg);
	void pickWeapon(double dt);
	void takeDMG();
	void heal(bool max_potion);
	void mana(float interval, int amount, bool restore);
	void InitMap(int lvl);
protected:
	// Game states
	enum GAME_STATE
	{
		STATE_NONE = 0,
		STATE_MENU,
		STATE_PLAY,
		STATE_LOSE
	};

	GAME_STATE state;

	// Gameplay variables
	std::vector<GameObject *> m_goList;
	float m_speed, max_vel, fire_rate;
	float m_worldWidth, m_worldHeight, m_screenWidth, m_screenHeight;
	GameObject * m_ghost, * m_player;
	int m_objectCount, weapon_dmg, hitpoints[5], heart_count, empty_heart, weapon_choice;
	bool initPos, jump, portal_in, move_portal_in, move_portal_out, portal_shot, shotgun, left, right;
	double dmg_delay, mana_delay;
	
	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2, pi;
};

#endif