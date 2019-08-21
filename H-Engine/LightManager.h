#pragma once
#include "DebugToScreen.h"
#include <d3dx9.h>
#include "helper.h"
#include "Input.h"
#include <dinput.h>


struct LIGHT_NODE	{
	int ID;
	int priority;
	bool active;
	D3DLIGHT9 light;
	LIGHT_NODE *p_NextNode;
};
	
class CLightManager
{
public:
	CLightManager(CDebugToScreen *p_Debug, CInput *p_Input);
	~CLightManager(void);

	//add lights
	void AddDirectLight(int lightID, LPDIRECT3DDEVICE9 in_D3dDevice, 
						D3DXVECTOR3 inDirection, float inRange, D3DXCOLOR inColour, int inPrio);

	int AddPointLight(int lightID, LPDIRECT3DDEVICE9 in_D3dDevice);
	int AddSpotLight(int lightID, LPDIRECT3DDEVICE9 in_D3dDevice);

	//light opperations
	void TurnOffLight(int lightID, LIGHT_NODE *inNode);
	void TurnOnClosestLights(LPDIRECT3DDEVICE9 in_D3dDevice);
	void UpdateLights(LPDIRECT3DDEVICE9 in_D3dDevice);
	void TurnOffAllLighting(LPDIRECT3DDEVICE9 in_D3dDevice);//for sky box


private:
	void CalculateLightDistance();

	LIGHT_NODE *p_StartNode;

	CDebugToScreen *d_Debug;
	CInput *d_Input;
	int dMaxLights;
};
