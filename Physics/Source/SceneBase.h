#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "Scene.h"
#include "Mtx44.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "GameObject.h"
#include <vector>
#include <fstream>

class SceneBase : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_TEXT,
		GEO_BALL,
		GEO_REDBALL,
		GEO_ORANGEBALL,
		GEO_YELLOWBALL,
		GEO_GREENBALL,
		GEO_CYANBALL,
		GEO_BLUEBALL,
		GEO_PURPLEBALL,
		GEO_CUBE,
		GEO_REDCUBE,
		GEO_ORANGECUBE,
		GEO_YELLOWCUBE,
		GEO_GREENCUBE,
		GEO_CYANCUBE,
		GEO_BLUECUBE,
		GEO_PURPLECUBE,
		GEO_BALLUP,
		GEO_SHIP,
		GEO_TIE_ADVANCED,
		GEO_TIE_BOMBER,
		GEO_ASTEROID,
		GEO_BULLET,
		GEO_BACKGROUND,
		GEO_ENEMY_SHIP,
		GEO_ENEMY_BULLET,
		GEO_CHARGEBG,
		GEO_CHARGE,
		GEO_COOLDOWN,
		GEO_DEATHSTAR,
		GEO_SCRAPMETAL,
		GEO_TANTIVE,
		GEO_FALCON,
		GEO_FALCONTURRET,
		GEO_TURRETSINGLE,
		GEO_TURRETDOUBLE,
		GEO_MENU,
		GEO_LEFT_ARROW,
		GEO_RIGHT_ARROW,
		GEO_WIN,
		GEO_LOSE,
		GEO_INTERCEPTOR,
		GEO_IMPERIAL,
		GEO_AWING,
		GEO_YWING,
		GEO_SINGLEBARREL,
		GEO_DOUBLEBARREL,
		GEO_MINITURRET,
		GEO_MISSILE,
		GEO_CANNON,
		GEO_RIFLE,
		GEO_LASERGUN,
		GEO_BLUELASER,
		GEO_LASERBEAM,
		GEO_DESTROYED,
		GEO_WALL,
		GEO_MENU2,
		GEO_LOSE2,
		GEO_HEXBG,
		GEO_GREYBG,
		GEO_PORTAL_IN,
		GEO_PORTAL_OUT,
		GEO_FULLHEART,
		GEO_80HEART,
		GEO_20HEART,
		GEO_EMPTYHEART,
		GEO_AK47,
		GEO_LMG,
		GEO_SMG,
		GEO_SHOTGUN,
		GEO_REVOLVER,
		GEO_DEMON,
		GEO_FIREBALL,
		NUM_GEOMETRY,
	};
public:
	SceneBase();
	~SceneBase();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMesh(Mesh *mesh, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderGO(GameObject *go);
	void DisableMonospace(std::string _csv_filepath);

	GameObject* FetchGO();
protected:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera camera;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Light lights[1];

	bool bLightEnabled;

	float fps;

	int width[256];
};

#endif