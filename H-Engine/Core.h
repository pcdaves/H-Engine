#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

#include "helper.h"
#include "ImportX.h"
#include "Camera.h"
#include "Input.h"
#include "DebugToScreen.h"
#include "LightManager.h"

class CCore
{
public:
	CCore(LPDIRECT3DDEVICE9 in_D3dDevice);
	~CCore(void);
	void MainLoop(void);		//main loop of the game
	void InitCore(HWND hWnd, HINSTANCE hInst);		//inits the core befor entering the game, also check incase the constructor fails!
	bool BboxCollision(CImportX * box1, CImportX * box2);

private:
	void Render(CAMERA_INFO info);
	void SkyBox(void);			//the sky box code isnt yet big enuf to justify its own class, so its a function of the core

	LPDIRECT3DDEVICE9 d3dDevice;

	CCamera*  d_Camera;
	CImportX* d_MeshObj1;
	CImportX* d_MeshObj2;
	CImportX* d_MeshObj3;
	CImportX* d_MeshObj4;
	CImportX* d_MeshObj5;
	CImportX* d_MeshObj6;
	CImportX* d_MeshObj7;
	CImportX* d_MeshObj8;

	CInput*	  d_Input;

	CDebugToScreen* d_Debug;

	CLightManager* d_LightManager;
};
