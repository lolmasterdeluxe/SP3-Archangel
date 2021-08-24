#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>

SceneBase::SceneBase()
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//comg.vertexshader", "Shader//comg.fragmentshader");

	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// Use our shader
	glUseProgram(m_programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 20, 0);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_NUMLIGHTS], 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	camera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));

	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_BALL] = MeshBuilder::GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
	meshList[GEO_REDBALL] = MeshBuilder::GenerateSphere("red ball", Color(1, 0, 0), 10, 10, 1.f);
	meshList[GEO_ORANGEBALL] = MeshBuilder::GenerateSphere("orange ball", Color(1, 0.647f, 0), 10, 10, 1.f);
	meshList[GEO_YELLOWBALL] = MeshBuilder::GenerateSphere("yellow ball", Color(1, 1, 0), 10, 10, 1.f);
	meshList[GEO_GREENBALL] = MeshBuilder::GenerateSphere("green ball", Color(0, 0.502f, 0), 10, 10, 1.f);
	meshList[GEO_CYANBALL] = MeshBuilder::GenerateSphere("cyan ball", Color(0, 1, 1), 10, 10, 1.f);
	meshList[GEO_BLUEBALL] = MeshBuilder::GenerateSphere("blue ball", Color(0, 0, 1), 10, 10, 1.f);
	meshList[GEO_PURPLEBALL] = MeshBuilder::GenerateSphere("purple ball", Color(0.502f, 0.000, 0.502f), 10, 10, 1.f);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 2.f);
	meshList[GEO_REDCUBE] = MeshBuilder::GenerateCube("red cube", Color(1, 0, 0), 2.f);
	meshList[GEO_ORANGECUBE] = MeshBuilder::GenerateCube("orange cube", Color(1, 0.647f, 0), 2.f);
	meshList[GEO_YELLOWCUBE] = MeshBuilder::GenerateCube("yellow cube", Color(1, 1, 0), 2.f);
	meshList[GEO_GREENCUBE] = MeshBuilder::GenerateCube("green cube", Color(0, 0.502f, 0), 2.f);
	meshList[GEO_CYANCUBE] = MeshBuilder::GenerateCube("cyan cube", Color(0, 1, 1), 2.f);
	meshList[GEO_BLUECUBE] = MeshBuilder::GenerateCube("blue cube", Color(0, 0, 1), 2.f);
	meshList[GEO_PURPLECUBE] = MeshBuilder::GenerateCube("purple cube", Color(0.502f, 0.000, 0.502f), 2.f);
	meshList[GEO_BALLUP] = MeshBuilder::GenerateQuad("ball", Color(1, 1, 1), 1.f);
	meshList[GEO_BALLUP]->textureID = LoadTGA("Image//ballup.tga");
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//Arial_N.tga");
	meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);
	meshList[GEO_SHIP] = MeshBuilder::GenerateQuad("spaceship", Color(1, 1, 1), 2.f);
	meshList[GEO_SHIP]->textureID = LoadTGA("Image//tiefighter.tga");
	meshList[GEO_ASTEROID] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 2.f);
	meshList[GEO_ASTEROID]->textureID = LoadTGA("Image//asteroid.tga");
	meshList[GEO_BULLET] = MeshBuilder::GenerateQuad("greenlaser", Color(1, 1, 1), 2.f);
	meshList[GEO_BULLET]->textureID = LoadTGA("Image//allylaser.tga");
	meshList[GEO_BLUELASER] = MeshBuilder::GenerateQuad("bluelaser", Color(1, 1, 1), 2.f);
	meshList[GEO_BLUELASER]->textureID = LoadTGA("Image//bluelaser.tga");
	meshList[GEO_LASERBEAM] = MeshBuilder::GenerateQuad("laserbeam", Color(1, 1, 1), 2.f);
	meshList[GEO_LASERBEAM]->textureID = LoadTGA("Image//laserbeam.tga");
	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("background", Color(1, 1, 1), 2.f);
	meshList[GEO_BACKGROUND]->textureID = LoadTGA("Image//space.tga");
	meshList[GEO_ENEMY_SHIP] = MeshBuilder::GenerateQuad("xwing", Color(1, 1, 1), 2.f);
	meshList[GEO_ENEMY_SHIP]->textureID = LoadTGA("Image//xwing.tga");
	meshList[GEO_ENEMY_BULLET] = MeshBuilder::GenerateQuad("redlaser", Color(1, 1, 1), 2.f);
	meshList[GEO_ENEMY_BULLET]->textureID = LoadTGA("Image//enemylaser.tga");
	meshList[GEO_CHARGEBG] = MeshBuilder::GenerateQuad("ChargeBG", Color(0, 0, 0), 1);
	meshList[GEO_CHARGE] = MeshBuilder::GenerateQuad("Charge", Color(0, 0, 1), 1);
	meshList[GEO_COOLDOWN] = MeshBuilder::GenerateQuad("Cooldown", Color(1, 0, 0), 1);
	meshList[GEO_DEATHSTAR] = MeshBuilder::GenerateQuad("deathstar", Color(1, 1, 1), 2.f);
	meshList[GEO_DEATHSTAR]->textureID = LoadTGA("Image//deathstar.tga");
	meshList[GEO_SCRAPMETAL] = MeshBuilder::GenerateQuad("scrap metal", Color(1, 1, 1), 2.f);
	meshList[GEO_SCRAPMETAL]->textureID = LoadTGA("Image//scrapmetal.tga");
	meshList[GEO_TANTIVE] = MeshBuilder::GenerateQuad("Tantive IV", Color(1, 1, 1), 2.f);
	meshList[GEO_TANTIVE]->textureID = LoadTGA("Image//tantive.tga");
	meshList[GEO_TURRETSINGLE] = MeshBuilder::GenerateQuad("Turret Single", Color(1, 1, 1), 2.f);
	meshList[GEO_TURRETSINGLE]->textureID = LoadTGA("Image//turretsingle.tga");
	meshList[GEO_TURRETDOUBLE] = MeshBuilder::GenerateQuad("Turret Double", Color(1, 1, 1), 2.f);
	meshList[GEO_TURRETDOUBLE]->textureID = LoadTGA("Image//turretdouble.tga");
	meshList[GEO_MENU] = MeshBuilder::GenerateQuad("Menu", Color(1, 1, 1), 2.f);
	meshList[GEO_MENU]->textureID = LoadTGA("Image//start_menu.tga");
	meshList[GEO_LEFT_ARROW] = MeshBuilder::GenerateQuad("Left Arrow", Color(1, 1, 1), 2.f);
	meshList[GEO_LEFT_ARROW]->textureID = LoadTGA("Image//left_arrow.tga");
	meshList[GEO_RIGHT_ARROW] = MeshBuilder::GenerateQuad("Right Arrow", Color(1, 1, 1), 2.f);
	meshList[GEO_RIGHT_ARROW]->textureID = LoadTGA("Image//right_arrow.tga");
	meshList[GEO_WIN] = MeshBuilder::GenerateQuad("Win", Color(1, 1, 1), 2.f);
	meshList[GEO_WIN]->textureID = LoadTGA("Image//winscreen.tga");
	meshList[GEO_LOSE] = MeshBuilder::GenerateQuad("Lose", Color(1, 1, 1), 2.f);
	meshList[GEO_LOSE]->textureID = LoadTGA("Image//losescreen.tga");
	meshList[GEO_MENU2] = MeshBuilder::GenerateQuad("Win", Color(1, 1, 1), 2.f);
	meshList[GEO_MENU2]->textureID = LoadTGA("Image//MenuScrn.tga");
	meshList[GEO_LOSE2] = MeshBuilder::GenerateQuad("Ball Menu", Color(1, 1, 1), 2.f);
	meshList[GEO_LOSE2]->textureID = LoadTGA("Image//LoseScrn.tga");
	meshList[GEO_DESTROYED] = MeshBuilder::GenerateQuad("Ball Lose", Color(1, 1, 1), 2.f);
	meshList[GEO_DESTROYED]->textureID = LoadTGA("Image//stardestroyerdestroyed.tga");
	meshList[GEO_INTERCEPTOR] = MeshBuilder::GenerateQuad("Interceptor", Color(1, 1, 1), 2.f);
	meshList[GEO_INTERCEPTOR]->textureID = LoadTGA("Image//interceptor.tga");
	meshList[GEO_IMPERIAL] = MeshBuilder::GenerateQuad("Imperial Star Destroyer", Color(1, 1, 1), 2.f);
	meshList[GEO_IMPERIAL]->textureID = LoadTGA("Image//StarDestroyer.tga");
	meshList[GEO_AWING] = MeshBuilder::GenerateQuad("A wing", Color(1, 1, 1), 2.f);
	meshList[GEO_AWING]->textureID = LoadTGA("Image//Awing.tga");
	meshList[GEO_YWING] = MeshBuilder::GenerateQuad("Y wing", Color(1, 1, 1), 2.f);
	meshList[GEO_YWING]->textureID = LoadTGA("Image//ywing.tga");
	meshList[GEO_SINGLEBARREL] = MeshBuilder::GenerateQuad("Single Barrel", Color(1, 1, 1), 2.f);
	meshList[GEO_SINGLEBARREL]->textureID = LoadTGA("Image//singlebarrel.tga");
	meshList[GEO_DOUBLEBARREL] = MeshBuilder::GenerateQuad("Double barrel", Color(1, 1, 1), 2.f);
	meshList[GEO_DOUBLEBARREL]->textureID = LoadTGA("Image//doublebarrel.tga");
	meshList[GEO_MINITURRET] = MeshBuilder::GenerateQuad("Small turret", Color(1, 1, 1), 2.f);
	meshList[GEO_MINITURRET]->textureID = LoadTGA("Image//smallturret.tga");
	meshList[GEO_MISSILE] = MeshBuilder::GenerateQuad("Missile", Color(1, 1, 1), 2.f);
	meshList[GEO_MISSILE]->textureID = LoadTGA("Image//missile.tga");
	meshList[GEO_TIE_BOMBER] = MeshBuilder::GenerateQuad("Tie Bomber", Color(1, 1, 1), 2.f);
	meshList[GEO_TIE_BOMBER]->textureID = LoadTGA("Image//bomber.tga");
	meshList[GEO_TIE_ADVANCED] = MeshBuilder::GenerateQuad("Advanced", Color(1, 1, 1), 2.f);
	meshList[GEO_TIE_ADVANCED]->textureID = LoadTGA("Image//tieadvanced.tga");
	meshList[GEO_FALCON] = MeshBuilder::GenerateQuad("Millenium Falcon", Color(1, 1, 1), 2.f);
	meshList[GEO_FALCON]->textureID = LoadTGA("Image//milleniumfalcon.tga");
	meshList[GEO_FALCONTURRET] = MeshBuilder::GenerateQuad("Millenium Falcon Turret", Color(1, 1, 1), 2.f);
	meshList[GEO_FALCONTURRET]->textureID = LoadTGA("Image//falconturret.tga");
	meshList[GEO_CANNON] = MeshBuilder::GenerateQuad("Cannon", Color(1, 1, 1), 2.f);
	meshList[GEO_CANNON]->textureID = LoadTGA("Image//cannon.tga");
	meshList[GEO_RIFLE] = MeshBuilder::GenerateQuad("Rifle gun", Color(1, 1, 1), 2.f);
	meshList[GEO_RIFLE]->textureID = LoadTGA("Image//rifle.tga");
	meshList[GEO_LASERGUN] = MeshBuilder::GenerateQuad("laser gun", Color(1, 1, 1), 2.f);
	meshList[GEO_LASERGUN]->textureID = LoadTGA("Image//lasergun.tga");
	meshList[GEO_WALL] = MeshBuilder::GenerateQuad("wall", Color(1, 1, 1), 2.f);
	meshList[GEO_HEXBG] = MeshBuilder::GenerateQuad("Hex background", Color(1, 1, 1), 2.f);
	meshList[GEO_HEXBG]->textureID = LoadTGA("Image//hexbg.tga");
	meshList[GEO_GREYBG] = MeshBuilder::GenerateQuad("Grey background", Color(1, 1, 1), 2.f);
	meshList[GEO_GREYBG]->textureID = LoadTGA("Image//greybg.tga");
	meshList[GEO_PORTAL_IN] = MeshBuilder::GenerateSphere("Blue portal", Color(0, 0, 1), 10, 10, 1.f);
	meshList[GEO_PORTAL_OUT] = MeshBuilder::GenerateSphere("Orange portal", Color(1, 0.647f, 0), 10, 10, 1.f);
	meshList[GEO_FULLHEART] = MeshBuilder::GenerateQuad("Full heart", Color(1, 1, 1), 2.f);
	meshList[GEO_FULLHEART]->textureID = LoadTGA("Image//fullheart.tga");
	meshList[GEO_80HEART] = MeshBuilder::GenerateQuad("Full heart", Color(1, 1, 1), 2.f);
	meshList[GEO_80HEART]->textureID = LoadTGA("Image//80heart.tga");
	meshList[GEO_20HEART] = MeshBuilder::GenerateQuad("Full heart", Color(1, 1, 1), 2.f);
	meshList[GEO_20HEART]->textureID = LoadTGA("Image//20heart.tga");
	meshList[GEO_EMPTYHEART] = MeshBuilder::GenerateQuad("Full heart", Color(1, 1, 1), 2.f);
	meshList[GEO_EMPTYHEART]->textureID = LoadTGA("Image//emptyheart.tga");
	meshList[GEO_FIREBALL] = MeshBuilder::GenerateQuad("Fireball", Color(1, 1, 1), 2.f);
	meshList[GEO_FIREBALL]->textureID = LoadTGA("Image//fireball.tga");
	meshList[GEO_AK47] = MeshBuilder::GenerateQuad("AK47", Color(1, 1, 1), 2.f);
	meshList[GEO_AK47]->textureID = LoadTGA("Image//AK.tga");
	meshList[GEO_SMG] = MeshBuilder::GenerateQuad("SMG", Color(1, 1, 1), 2.f);
	meshList[GEO_SMG]->textureID = LoadTGA("Image//SMG.tga");
	meshList[GEO_SHOTGUN] = MeshBuilder::GenerateQuad("Shotgun", Color(1, 1, 1), 2.f);
	meshList[GEO_SHOTGUN]->textureID = LoadTGA("Image//Shotgun.tga");
	meshList[GEO_LMG] = MeshBuilder::GenerateQuad("LMG", Color(1, 1, 1), 2.f);
	meshList[GEO_LMG]->textureID = LoadTGA("Image//Light_Machine_Gun.tga");
	meshList[GEO_REVOLVER] = MeshBuilder::GenerateQuad("Revolver", Color(1, 1, 1), 2.f);
	meshList[GEO_REVOLVER]->textureID = LoadTGA("Image//Revolver.tga");


	bLightEnabled = false;
	DisableMonospace("Image//Arial_N.csv");
}

void SceneBase::Update(double dt)
{
	//Keyboard Section
	/*if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
	
	fps = (float)(1.f / dt);
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	if(!mesh || mesh->textureID <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if(!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	float increment = 0;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		//characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		characterSpacing.SetToTranslation(0.5f + increment, 0.5f, 0);
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
		increment += width[text[i]] / 64.0f;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if(enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
		
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{	
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if(mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if(mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneBase::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	//to do: scale and translate accordingly
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 0);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::DisableMonospace(std::string _csv_filepath)
{
	std::ifstream file;
	std::string tempstr;
	int i = 0;
	file.open(_csv_filepath);
	if (file.is_open() == true) {
		while (i != 256) {
			getline(file, tempstr);
			width[i] = stoi(tempstr);
			++i;
		}
		file.close();
	}
	else
	{
		std::cout << "Unable to open CSV file\n";
	}
}

void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
