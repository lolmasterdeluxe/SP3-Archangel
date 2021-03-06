#ifndef SCENE_ARCHANGEL_H
#define SCENE_ARCHANGEL_H
#define SCREEN_HEIGHT 60

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "CMapMaker.h"
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
	void ReturnGO(GameObject* go);
	void ReturnGO(GameObject::GAMEOBJECT_TYPE GO);
	GameObject* FindGameObjectWithType(GameObject::GAMEOBJECT_TYPE type);
	Collision CheckCollision(GameObject* go1, GameObject* go2);
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
	void Melee(double dt);
	void heal(bool max_potion);
	void mana(float interval, float amount, bool restore);
	void openChest(GameObject* go);
	void manipTime(double dt);
	void setCollisionBox(GameObject* go);
	void demonAI(double dt);
	void fallenAngelAI(double dt);
	void terminatorAI(double dt);
	void soldierAI(double dt);
	void demonBossAI(double dt);
	void metalGearAI(double dt);
	void ramboAI(double dt);
	void linearVel(double dt, GameObject::GAMEOBJECT_TYPE GO, float speed);
	void runAnimation(double dt, GameObject::GAMEOBJECT_TYPE GO, int i, double animation_max, int frame_max);

	// mouse stuff
	void ScreenSpaceToWorldSpace(double& x, double& y);
	GameObject* ObjectOnCursor();

	// Map Stuff
	void InitMap();
	void SaveMap();
	void ClearMap();

	void SpawnEnemies();

protected:
	// Game states
	enum GAME_STATE
	{
		STATE_NONE = 0,
		STATE_INITMENU,
		STATE_INTRO,
		STATE_MENU,
		STATE_INITPLAY,
		STATE_PLAY,
		STATE_PAUSE,
		STATE_LOSE_ANIM,
		STATE_WIN_ANIM,
		STATE_LOSE,
		STATE_WIN,
	};
	enum PLAY_STATE
	{
		PLAY_PREBATTLE,
		PLAY_BATTLE,
		PLAY_POSTBATTLE,
	};
	enum PLAY_REALM
	{
		REALM_HELL,
		REALM_FUTURE,
		REALM_MODERN,
	};

	GAME_STATE state;

	PLAY_STATE playState; // state of the gameplay

	PLAY_REALM realm;

	// maps and levels
	CMapMaker mapMaker;

	// When player walk into the boundary, attempt to switch level
	bool m_AttemptLeft, m_AttemptRight;

	// Camera
	Vector3 cameraPos;

	// button toggling
	bool escapeButtonState, rButtonState;

	// Gameplay variables
	std::vector<GameObject *> m_goList;
	float m_speed, max_vel, fire_rate, time_manip;
	float m_worldWidth, m_worldHeight, m_screenWidth, m_screenHeight;
	GameObject * m_ghost, * m_player, * m_emptyGO;
	int m_objectCount, weapon_dmg, hitpoints[20], heart_count, empty_heart, weapon_choice;
	bool initPos, jump, portal_in, move_portal_in, move_portal_out, portal_shot, shotgun, time_change, phase;
	double dmg_delay, mana_delay;

	//Debuging
	bool m_toggleDebugScreen;
	
	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2, pi;
};

#endif