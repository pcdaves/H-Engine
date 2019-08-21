#include "Camera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 in_D3dDevice)
{
	turnHead = false; //is the mouse supposed to move the screen?
	dMoveSpeed = 1.0f; //move spd for cam

	//The camera has three settings: "Camera Position", "Look at Position" and "Up Direction"
	//We have set the following:
	//Camera Position:	(0, 15, -50)
	//Look at Position: (0, 0, 0)
	//Up direction:		Y-Axis.
	vEye = new D3DXVECTOR3( -200.0f, 100.0f,200.0f );
    vLook = new D3DXVECTOR3( 0.5f, -0.2f, -0.4f );
    vUp = new D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	vRight = new D3DXVECTOR3( 1.0f, 0.0f, 0.0f);
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, vEye, vLook, vUp );
    in_D3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//Here we specify the field of view, aspect ration and near and far clipping planes.
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 2000.0f );
    in_D3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

CCamera::~CCamera(void)
{
	SafeDelete(vEye);
	SafeDelete(vLook);
	SafeDelete(vRight);
	SafeDelete(vUp);
}

void CCamera::UpdateViewMatrix( LPDIRECT3DDEVICE9 m_pD3DDevice, CAMERA_INFO direction)	{
	//update the eye from struct
	UpdateEye(direction);

	D3DXMATRIX view;
	D3DXMatrixIdentity( &view );

	D3DXVec3Normalize( vLook, vLook );
	D3DXVec3Cross( vRight, vUp, vLook );
	D3DXVec3Normalize( vRight, vRight );
	D3DXVec3Cross( vUp, vLook, vRight );
	D3DXVec3Normalize( vUp, vUp );

	//setup view matrix
	view._11 = vRight->x;
    view._12 = vUp->x;
    view._13 = vLook->x;
	view._14 = 0.0f;

	view._21 = vRight->y;
    view._22 = vUp->y;
    view._23 = vLook->y;
	view._24 = 0.0f;

	view._31 = vRight->z;
    view._32 = vUp->z;
    view._33 = vLook->z;
	view._34 = 0.0f;

	view._41 = -D3DXVec3Dot( vEye, vRight );
	view._42 = -D3DXVec3Dot( vEye, vUp );
	view._43 = -D3DXVec3Dot( vEye, vLook );
	view._44 =  1.0f;

	//set view matrix
	m_pD3DDevice->SetTransform( D3DTS_VIEW, &view ); 
}

void CCamera::UpdateEye(CAMERA_INFO struc_Info)	{

	//set if mouse should move the cam, if left mose down - move cam!
	turnHead = struc_Info.d_MouseLeft;

	//create temp vectors which represent the look and right vectors, we dont want these to change!!
	D3DXVECTOR3 tempLook, tempRight;
	tempLook.x = vLook->x; tempLook.y = vLook->y; tempLook.z = vLook->z;
	tempRight.x = vRight->x; tempRight.y = vRight->y; tempRight.z = vRight->z; 

	//translate the directions given 
	//x axis
	if(struc_Info.eyeOffSet.x == -1.0f)	{
		vEye->operator +=(tempRight*-dMoveSpeed);
	}

	if(struc_Info.eyeOffSet.x == 1.0f)	{
		vEye->operator -=(tempRight*-dMoveSpeed);
	}

	//y axis
	if(struc_Info.eyeOffSet.y == 1.0f)	{
		vEye->y += dMoveSpeed;
	}
	if(struc_Info.eyeOffSet.y == -1.0f)	{
		vEye->y -= dMoveSpeed;
	}

	//z axis
	if(struc_Info.eyeOffSet.z == -1.0f)	{
		vEye->operator +=(tempLook*-dMoveSpeed);
	}
	if(struc_Info.eyeOffSet.z == 1.0f)	{
		vEye->operator -=(tempLook*-dMoveSpeed);
	}
	

	//rotate camera
	if( struc_Info.xDiff != 0 && turnHead == true)		{
			//use &D3DXVECTOR3(0,1,0) to make sure we always rotate round Y axis, can be subbed with g_vUp for a 0G kinda effect
			D3DXMATRIX matRotation;
			D3DXMatrixRotationAxis( &matRotation, &D3DXVECTOR3(0,1,0), D3DXToRadian((float)struc_Info.xDiff / 3.0f));
			D3DXVec3TransformCoord( vLook, vLook, &matRotation );
			D3DXVec3TransformCoord( vUp, vUp, &matRotation );
		}

	if( struc_Info.yDiff != 0 && turnHead == true)		{
			D3DXMATRIX matRotation;
			D3DXMatrixRotationAxis( &matRotation, vRight, D3DXToRadian((float)struc_Info.yDiff / 3.0f));
			D3DXVec3TransformCoord( vLook, vLook, &matRotation );
			D3DXVec3TransformCoord( vUp, vUp, &matRotation );
		}
}

D3DXVECTOR3 CCamera::ReturnEye(void)	{
	return *vEye;
}