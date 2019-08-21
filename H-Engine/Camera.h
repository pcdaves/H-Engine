#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

#include "helper.h"

class CCamera
{
public:
	CCamera(LPDIRECT3DDEVICE9 in_D3dDevice);
	~CCamera(void);
	void UpdateViewMatrix( LPDIRECT3DDEVICE9 m_pD3DDevice, CAMERA_INFO direction );	
	D3DXVECTOR3 ReturnEye(void);

private:
	void UpdateEye(CAMERA_INFO direction);

	bool   turnHead;
	float  dMoveSpeed;

	D3DXVECTOR3	*vEye;    // Eye Position
	D3DXVECTOR3	*vLook;  // Look Vector
	D3DXVECTOR3	*vUp;      // Up Vector
	D3DXVECTOR3	*vRight;   // Right Vector
};
