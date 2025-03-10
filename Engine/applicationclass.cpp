////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
#include <stack>

ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_Terrain = 0;
	m_Timer = 0;
	m_Position = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_FontShader = 0;
	m_Text = 0;
	m_TerrainShader = 0;
	m_Light = 0;
	m_cylinder = 0;
	m_leaf = 0;
	m_ColorShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float cameraX, cameraY, cameraZ;
	D3DXMATRIX baseViewMatrix;
	char videoCard[128];
	int videoMemory;

	ShowCursor(true);

	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the camera.
	cameraX = 50.0f;
	cameraY = 2.0f;
	cameraZ = 50.0f;

	m_Camera->SetPosition(cameraX, cameraY, cameraZ);

	// Create the model object.
	m_cylinder = new cylinderclass(0);
	if (!m_cylinder)
	{
		return false;
	}

	// Initialize the model object.
	result = m_cylinder->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cylinder model object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	m_leaf = new leafclass();
	if (!m_leaf)
	{
		return false;
	}

	// Initialize the model object.
	result = m_cylinder->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the leaf model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the terrain object.
	m_Terrain = new TerrainClass;
	if (!m_Terrain)
	{
		return false;
	}

	// Initialize the terrain object.
	//result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../Engine/data/heightmap01.bmp", L"../Engine/data/grass.dds", L"../Engine/data/slope.dds",
		//L"../Engine/data/rock.dds");
	result = m_Terrain->InitializeTerrain(m_Direct3D->GetDevice(), 128, 128, L"../Engine/data/grass.dds", L"../Engine/data/dirt.dds",
		L"../Engine/data/rock.dds", L"../Engine/data/snow.dds");   //initialise the flat terrain.
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(cameraX, cameraY, cameraZ);

	// Create the fps object.
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	// Create the terrain shader object.
	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader)
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, -1.0f, 0.0f);

	LSystemString = m_LSystem->generateString("FA", 5);

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the terrain shader object.
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the position object.
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the terrain object.
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Release the model object.
	if (m_cylinder)
	{
		m_cylinder->Shutdown();
		delete m_cylinder;
		m_cylinder = 0;
	}

	// Release the model object.
	if (m_leaf)
	{
		m_leaf->Shutdown();
		delete m_leaf;
		m_leaf = 0;
	}

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;

	// Read the user input.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the CPU usage value in the text object.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = RenderGraphics();
	if (!result)
	{
		return false;
	}

	return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.

	//Generate trees
	keyDown = m_Input->IsSpacePressed();
	if (keyDown)
	{
		ParseLSystem(0, 0, 0);
	}

	//Apply noise
	keyDown = m_Input->IsNPressed();
	m_Terrain->GenerateHeightMap(m_Direct3D->GetDevice(), keyDown);
	
	//Smooth terrain
	keyDown = m_Input->IsMPressed();
	m_Terrain->SmoothTerrain(m_Direct3D->GetDevice(), keyDown);

	//Fault terrain
	keyDown = m_Input->IsFPressed();
	m_Terrain->Faulting(m_Direct3D->GetDevice(), keyDown, (float(rand() % 2) + 0.5));

	//Reset system
	keyDown = m_Input->IsRPressed();
	m_Terrain->FlattenTerrain(m_Direct3D->GetDevice(), keyDown);
	if (keyDown)
	{
		m_Cylinders.clear();
		m_Leaves.clear();
	}

	//Turn camera left
	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	//Turn camera right
	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	//Move camera forward
	keyDown = m_Input->IsWPressed();
	m_Position->MoveForward(keyDown);

	//Move camera back
	keyDown = m_Input->IsSPressed();
	m_Position->MoveBackward(keyDown);

	//Move camera left
	keyDown = m_Input->IsAPressed();
	m_Position->MoveLeft(keyDown);

	//Move camera right
	keyDown = m_Input->IsDPressed();
	m_Position->MoveRight(keyDown);

	//Move camera up
	keyDown = m_Input->IsEPressed();
	m_Position->MoveUpward(keyDown);

	//Move camera down
	keyDown = m_Input->IsQPressed();
	m_Position->MoveDownward(keyDown);

	//Turn camera up
	keyDown = m_Input->IsUpPressed();
	m_Position->LookUpward(keyDown);

	//turn camera down
	keyDown = m_Input->IsDownPressed();
	m_Position->LookDownward(keyDown);

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(posX, posY, posZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// Clear the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Render the terrain buffers.
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	// Render the terrain using the terrain shader.
	result = m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetGrassTexture(),
		m_Terrain->GetSlopeTexture(), m_Terrain->GetRockTexture(), m_Terrain->GetSnowTexture());
	if (!result)
	{
		return false;
	}

	//Render cylinders
	for (int i = 0; i < m_Cylinders.size(); i++) 
	{
		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Cylinders[i]->Render(m_Direct3D->GetDeviceContext());

		// Render the model using the color shader.
		result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Cylinders[i]->GetIndexCount(), m_Cylinders[i]->transform, viewMatrix, projectionMatrix);
		if (!result)
		{
			return false;
		}
	}

	//Render leaves
	for (int i = 0; i < m_Leaves.size(); i++)
	{
		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Leaves[i]->Render(m_Direct3D->GetDeviceContext());

		// Render the model using the color shader.
		result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Leaves[i]->GetIndexCount(), m_Leaves[i]->transform, viewMatrix, projectionMatrix);
		if (!result)
		{
			return false;
		}
	}

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_Direct3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	//Implement GUI functionality
	//gui();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void ApplicationClass::ParseLSystem(float xpos, float ypos, float zpos)
{
	//Set coordinates for tree to be generated at
	xpos = (rand() % 128) + 1;
	zpos = (rand() % 128) + 1;
	ypos = m_Terrain->getXZHeight(xpos, zpos);

	//Initialise matrices
	D3DXMATRIX transformMatrix;
	m_Direct3D->GetWorldMatrix(transformMatrix);
	D3DXMatrixTranslation(&transformMatrix, xpos, ypos, zpos);

	D3DXMATRIX move;
	D3DXMATRIX rotation;

	D3DXMatrixTranslation(&move, 0.f, 0.f, 0.f);
	D3DXMatrixRotationYawPitchRoll(&rotation, 0.f, 0.f, 0.0f);

	D3DXMATRIX l, r, f, b;
	D3DXMatrixRotationYawPitchRoll(&l, D3DXToRadian(120.f), 0.f, 0.f);
	D3DXMatrixRotationYawPitchRoll(&r, D3DXToRadian(-120.f), 0.f, 0.f);
	D3DXMatrixRotationYawPitchRoll(&f, 0.f, D3DXToRadian(25.f), 0.f);
	D3DXMatrixRotationYawPitchRoll(&b, 0.f, D3DXToRadian(15.f), 0.f);

	std::stack<D3DXMATRIX> matrixStack;
	std::stack<D3DXMATRIX> moveStack;
	std::stack<D3DXMATRIX> rotationStack;

	// [ = store the state
    // ] = revert the state
    // F = create tree segment
    // A = Terminator (create a leaf / do nothing)
    // & = Pitch out by X degrees
    // / & \\ = rotate around the Y axis by -X or +X degrees

	for (int i = 0; i < LSystemString.length(); i++) 
	{
		int randNum = (rand() % 2) + 1;

		switch (LSystemString[i])
		{
		case 'F':
		{
			float h = 0;

			//Create a cylinder
			transformMatrix = (rotation * move) * transformMatrix;

			//Add and initialise a new cylinder
			m_Cylinders.push_back(new cylinderclass(0));
			m_Cylinders.back()->Initialize(m_Direct3D->GetDevice());

			h = m_Cylinders.back()->cylinderHeight;

			m_Cylinders.back()->transform = transformMatrix;

			//Move to new position and reset rotation
			D3DXMatrixTranslation(&move, 0.f, h, 0.f);
			D3DXMatrixRotationYawPitchRoll(&rotation, 0.f, 0.f, 0.0f);
		}
			break;

		case 'A':

			//Create leaf
			transformMatrix = (rotation * move)*transformMatrix;

			//Add and initialise a new leaf
			m_Leaves.push_back(new leafclass());
			m_Leaves.back()->Initialize(m_Direct3D->GetDevice());

			m_Leaves.back()->transform = transformMatrix;

			//Reset rotation
			D3DXMatrixRotationYawPitchRoll(&rotation, 0.f, 0.f, 0.0f);

			break;

		case '[':

			//Store the state
			matrixStack.push(transformMatrix);
			moveStack.push(move);
			rotationStack.push(rotation);
			
			break;

		case ']':

			//Revert the state
			transformMatrix = matrixStack.top();
			matrixStack.pop();
			move = moveStack.top();
			moveStack.pop();
			rotation = rotationStack.top();
			rotationStack.pop();

			break;

		case '&':

			//Pitch out by X degrees
			if (randNum == 1)
			{
				rotation *= f;
			}
			else
			{
				rotation *= b;
			}

			break;

		case '/':

			//Rotate around Y +
			rotation *= l;

			break;

		case '\\':

			//Rotate around Y -
			rotation *= r;

			break;

		default:
			break;
		}

	}
}