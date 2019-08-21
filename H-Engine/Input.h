#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <dinput.h>
#include "helper.h"
#include "DebugToScreen.h"

class CInput
{
public:
	CInput(HWND hWnd, HINSTANCE hInst, CDebugToScreen* Debug);
	~CInput(void);	
	CAMERA_INFO  Update(void);
	bool CheckQuit(void);
	(struct)CAMERA_INFO ReturnMouseStruc();
	D3DXVECTOR3 ReturnOffSet(void);
	bool ReturnKey(WORD inKey);
	void UpdateBoxOffSet(D3DVECTOR inVec);

private:

	bool SetupDirectInput(HINSTANCE hInst);
	bool SetupKeyboard(HWND hWnd);
	bool SetupMouse(HWND hWnd);
	void UpdateKeyboard(void);
	void UpdateMouse(void);

	CDebugToScreen* d_Debug;

	LPDIRECTINPUT8		pDirectInput;	//holds our direct input object
	LPDIRECTINPUTDEVICE8 pKeyboard;		//holds our keyboard
	LPDIRECTINPUTDEVICE8 pMouse;		//hold our mouse

	bool dQuit;						//exit if true!
	bool dShowBox;

	int d_MouseX, d_MouseY;				//current mouse position
	int d_PrevMouseX, d_PrevMouseY;		//last mouse position
	bool d_MouseLeft, d_MouseRight;		//button values
	D3DXVECTOR3 offSet;				//used to send bk the offset of the tiger.
	D3DXVECTOR3 offSet2;	

};
