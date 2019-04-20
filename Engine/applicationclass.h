////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "terrainshaderclass.h"
#include "lightclass.h"
#include "cylinderclass.h"
#include "leafclass.h"
#include "ColorShaderClass.h"
#include "LSystemStringGenerator.h"
#include <vector>

//GUI INCLUDES

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imconfig.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <stb_rect_pack.h>
#include <stb_textedit.h>
#include <stb_truetype.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////

class ApplicationClass
{
public:

	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void gui();
	int faultValue = 1;
	string LSystemString;

private:

	bool HandleInput(float);
	bool RenderGraphics();
	void ParseLSystem();

private:

	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;
	TerrainShaderClass* m_TerrainShader;
	LightClass* m_Light;
	cylinderclass* m_cylinder;
	std::vector<cylinderclass*> m_Cylinders;
	leafclass* m_leaf;
	std::vector<leafclass*> m_Leaves;
	ColorShaderClass* m_ColorShader;
	LSystemStringGenerator* m_LSystem;
};

#endif