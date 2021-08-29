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

		GEO_BACKGROUND,
		GEO_LOSE,
		GEO_WIN,
		GEO_MENU,

		GEO_MISSILE,
		GEO_CHARGEBG,
		GEO_CHARGE,
		GEO_WALL,

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
		GEO_BULLET,

		GEO_BARREL,
		GEO_CHESTCLOSED,
		GEO_CHESTOPENED,

		GEO_HEALTHPOTION,
		GEO_MAXPOTION,
		GEO_MANAPOTION,
		GEO_COIN,

		GEO_DEMON,
		GEO_FALLENANGEL,
		GEO_SOLDIER,
		GEO_TERMINATOR_NEUTRAL,
		GEO_TERMINATOR_RAGE,
		GEO_FIREBALL,
		GEO_SWORD,

		GEO_DEMONLORD,
		GEO_METALGEAR,
		GEO_RAMBO,

		GEO_NETHERBRICK,
		GEO_MODERNBLOCK,
		GEO_FUTUREBLOCK,
		GEO_NETHERPLATFORM,
		GEO_MODERNPLATFORM,
		GEO_FUTUREPLATFORM,
		GEO_JUMPBLOCK,

		GEO_BLUEPORTAL,
		GEO_ORANGEPORTAL,

		GEO_FRAME1,
		GEO_FRAME2,
		GEO_FRAME3,
		GEO_FRAME4,
		GEO_FRAME5,
		GEO_FRAME6,
		GEO_FRAME7,
		GEO_FRAME8,
		GEO_FRAME9,
		GEO_FRAME10,
		GEO_FRAME11,
		GEO_FRAME12,
		GEO_FRAME13,
		GEO_FRAME14,
		GEO_FRAME15,
		GEO_FRAME16,
		GEO_FRAME17,

		GEO_1FRAME1,
		GEO_1FRAME2,
		GEO_1FRAME3,
		GEO_1FRAME4,
		GEO_1FRAME5,
		GEO_1FRAME6,
		GEO_1FRAME7,
		GEO_1FRAME8,
		GEO_1FRAME9,
		GEO_1FRAME10,
		GEO_1FRAME11,
		GEO_1FRAME12,
		GEO_1FRAME13,
		GEO_1FRAME14,
		GEO_1FRAME15,
		GEO_1FRAME16,
		GEO_1FRAME17,

		GEO_SWORD1,
		GEO_SWORD2,
		GEO_SWORD3,
		GEO_SWORD4,
		GEO_SWORD5,
		GEO_SWORD6,
		GEO_SWORD7,

		GEO_AKARM,
		GEO_LMGARM,
		GEO_SMGARM,
		GEO_SHOTGUNARM,
		GEO_REVOLVERARM,

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