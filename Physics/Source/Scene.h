#ifndef SCENE_H
#define SCENE_H

class Scene
{
	bool endGame;
public:
	Scene() :
	endGame(false)
	{}
	~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;
	bool GetEnd() { return endGame; }
	void EndGame() { endGame = true; }
};

#endif