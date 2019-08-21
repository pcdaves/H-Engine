#pragma once
//#include <mmsystem.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include "helper.h"

#define MESH_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CImportX
{
public:
	CImportX(LPDIRECT3DDEVICE9 pD3DDevice, LPSTR pFilename);
	~CImportX(void);
	void Render(LPDIRECT3DDEVICE9 d_D3dDevice);
	void Render(LPDIRECT3DDEVICE9 d_D3dDevice, D3DVECTOR inVec);  //over load for collisions
	void UpdateAABB(D3DXMATRIX matTotal);
	D3DXVECTOR3 ReturnMin();
	D3DXVECTOR3 ReturnMax();
	D3DVECTOR ReturnLNG();
private:
	bool LoadModel(LPSTR pFilename, LPDIRECT3DDEVICE9 d_D3dDevice);
	bool LoadImgs(LPDIRECT3DDEVICE9 d_D3dDevice);
	void CreateOBB(void);

	//vars to hold mesh info
	DWORD dNumOfMats;
	LPD3DXMESH dMesh;
	LPD3DXBUFFER dMatsBuffer;

	D3DMATERIAL9* d_Materials;
    LPDIRECT3DTEXTURE9* d_Textures;	

	D3DXVECTOR3 *boxVerts[8]; //model space
	D3DXVECTOR3 *boxWorld[8]; //world space
	
	D3DXVECTOR3 minAABB;	//min vector for aabb
	D3DXVECTOR3 maxAABB;	//max vector for aabb

	//last known good
	D3DXVECTOR3 boxLNG;

};
