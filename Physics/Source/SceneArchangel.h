#ifndef SCENE_ARCHANGEL_H
#define SCENE_ARCHANGEL_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

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
	void ReturnGO(GameObject *go);
	bool CheckCollision(GameObject* go1, GameObject* go2, float dt);
	void CollisionResponse(GameObject* go1, GameObject* go2);
	void SpawnBall();
	void playerLogic(double dt);

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
	float m_speed, max_vel;
	float m_worldWidth;
	float m_worldHeight;
	GameObject* m_ghost;
	GameObject* m_square[50];
	int m_objectCount;
	bool initPos, jump;
	
	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2, pi;
};

#endif