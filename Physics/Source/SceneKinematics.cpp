#include "SceneKinematics.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneKinematics::SceneKinematics()
{
}

SceneKinematics::~SceneKinematics()
{
}

void SceneKinematics::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	m_gravity.Set(0, -9.8f, 0); //init gravity as 9.8ms-2 downwards
	Math::InitRNG();

	for (int i = 0; i < 1000; ++i)
	{
		m_ghost = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(m_ghost);
	}
	//Exercise 1: construct 10 GameObject with type GO_BALL and add into m_goList
		
}

void SceneKinematics::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Keyboard Section
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if (Application::IsKeyPressed(VK_OEM_PLUS))
	{
		//Exercise 6: adjust simulation speed
		m_speed += 0.01f;
	}
	if (Application::IsKeyPressed(VK_OEM_MINUS))
	{
		//Exercise 6: adjust simulation speed
		m_speed -= 0.01f;
		if (m_speed <= 0 + Math::EPSILON)
		{
			m_speed = 0;
		}
	}
	if (Application::IsKeyPressed('C')) // Joel Pang
	{
		//Exercise 9: clear screen
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
				go->active = false;
		}
	}

	if (Application::IsKeyPressed('B'))
	{
		//Exercise 9: spawn balls
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (!go->active)
			{
				go->active = true;
				go->type = GameObject::GO_BALL;
				int w = Application::GetWindowWidth();
				int h = Application::GetWindowHeight();
				go->pos.Set(rand() % int(m_worldWidth), rand() % int(m_worldHeight), 0);
				go->vel.Set(rand() % 200 - 100, rand() % 200 - 100, 0); //random range between -100 - 100
			}
		}
	}
	if (Application::IsKeyPressed('V'))
	{
		//Exercise 9: spawn obstacles
		for (int i = 0; i < 10; ++i)
		{
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (!go->active)
				{
					go->active = true;
					go->type = GameObject::GO_CUBE;
					int w = Application::GetWindowWidth();
					int h = Application::GetWindowHeight();
					go->pos.Set(rand() % int(m_worldWidth), rand() % int(m_worldHeight), 0);
					break;
				}
			}
		}
	}

	//Mouse Section
	static bool bLButtonState = false;
	//Exercise 10: ghost code here
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		//Exercise 10: spawn ghost ball
		if (!m_ghost->active)
		{
			m_ghost->active = true;
			m_ghost->pos = Vector3(x / w * m_worldWidth, (h - y) / h * m_worldHeight, 0);
			m_ghost->vel = Vector3(0, 0, 0);
		}
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		//Exercise 4: spawn ball
		//Exercise 10: replace Exercise 4 code and use ghost to determine ball velocity
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = *it;
			if (!go->active)
			{
				go->active = true;
				go->type = GameObject::GO_BALL;
				go->scale = m_ghost->scale;

				double x, y;
				Application::GetCursorPos(&x, &y);
				int w = Application::GetWindowWidth();
				int h = Application::GetWindowHeight();

				go->pos = m_ghost->pos;
				go->vel = go->pos - Vector3(x / w * m_worldWidth, (h - y) / h * m_worldHeight, 0);
				m_ghost->vel = go->vel;
				m_ghost->active = false;
				m_timeGO = go;
				break;
			}
		}
	}

	//Exercise 10: print m_ghost position and velocity information
	//ss.str("");
	//ss.precision(4);
	//ss << "Gpos: " << m_ghost->pos;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	//ss.str("");
	//ss.precision(4);
	//ss << "Gvel: " << m_ghost->vel;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);
						
		//Exercise 10: replace Exercise 4 code and use ghost to determine ball velocity

		//Exercise 11: kinematics equation
		//v = u + a * t
		//t = (v - u ) / a

		//v * v = u * u + 2 * a * s
		//s = - (u * u) / (2 * a)
						
		//s = u * t + 0.5 * a * t * t
		//(0.5 * a) * t * t + (u) * t + (-s) = 0
	
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1)) //Ridwan benar
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
		//Exercise 7: spawn obstacles using GO_CUBE

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = x / w * m_worldWidth;
		float posY = (h - y) / h * m_worldHeight;


		for (int i = 0; i < m_goList.size(); i++)
		{
			if (m_goList.at(i)->active == false)
			{
				m_goList.at(i)->active = true;
				m_goList.at(i)->type = GameObject::GO_CUBE;
				m_goList.at(i)->pos.Set(posX, posY);
				break;
			}
		}
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
	}
	//Physics Simulation Section
	fps = (float)(1.f / dt);

	//Exercise 11: update kinematics information
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			if(go->type == GameObject::GO_BALL)
			{
				//Exercise 2: implement equation 1 & 2
				go->vel += m_gravity * dt * m_speed;
				go->pos += go->vel * dt * m_speed;
				//Exercise 12: replace Exercise 2 code and use average speed instead



				//Exercise 8: check collision with GO_CUBE
				for (std::vector<GameObject*>::iterator it_col = m_goList.begin(); it_col != m_goList.end(); ++it_col)
				{
					GameObject* goCol = (GameObject*)*it_col;
					if (goCol->active && goCol->type == GameObject::GO_CUBE)
					{
						if ((go->pos - goCol->pos).Length() < ((go->scale.x + goCol->scale.x) * 1.0f))
						{ // Collided
							go->active = goCol->active = false;
						}
					}
				}
			}

			

			//Exercise 5: unspawn ball when outside window
			if (!(go->pos.x < m_worldWidth && go->pos.y < m_worldHeight && go->pos.x > 0 && go->pos.y > 0))
			{
				go->active = false;
			}
		}
	}
}

void SceneKinematics::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 3: render a sphere with radius 1
		modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_CUBE: //Ridwan pintu benard yes
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		//Exercise 7: render a cube with length 2
		break;
	}
}

void SceneKinematics::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderMesh(meshList[GEO_AXES], false);

	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}
	if (m_ghost->active)
	{
		RenderGO(m_ghost);
	}

	//On screen text
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	//Exercise 6: print simulation speed
	RenderTextOnScreen(meshList[GEO_TEXT], "m_speed: " + std::to_string(m_speed), Color(0, 1, 0), 3, 0, 6);
	
	//Exercise 10: print m_ghost position and velocity information

	//Exercise 11: print kinematics information
	//v = u + a * t
	//t = (v - u ) / a

	//v * v = u * u + 2 * a * s
	//s = - (u * u) / (2 * a)

	//s = u * t + 0.5 * a * t * t
	//(0.5 * a) * t * t + (u) * t + (-s) = 0

	RenderTextOnScreen(meshList[GEO_TEXT], "Kinematics", Color(0, 1, 0), 3, 0, 0);
}

void SceneKinematics::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
