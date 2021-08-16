#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneCollision : public SceneBase
{
public:
	SceneCollision();
	~SceneCollision();

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
	void createCube();
	void createPillars(GameObject* go1, GameObject* go2);
	void spawnShapes();
	void initShapes();
	void moveShapes(double dt);

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
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject* m_ghost;
	GameObject* m_square[50];
	int m_objectCount, m_squareCount, m_ballCount, m_ballSpawned, m_ballMax, m_ballLabel, points, points_ceiling;
	bool initPos, start, spawnGhost, spawnShape, moveShape, addball, init_game;
	double interval, ball_spawnrate, oldx, oldy;
	
	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2, pi;
};

#endif