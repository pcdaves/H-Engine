#include "ImportX.h"
/*******************************************
This class is used to import an X file

this class also created the OBB and AABB
for a given model.

NOTE: to gather correct textures an edit
must be made to the x files, the file name
should be pre fixed with the address from
execution point (the folder in which the exe
runs) to the directory with the texutes in.

I.E. the program exe is in ./Game/
the mesh and textures reside in ./Game/Mesh/

the textures would need the prefix:
./Mesh/TexuteName.BMP
*******************************************/
CImportX::CImportX(LPDIRECT3DDEVICE9 pD3DDevice, LPSTR pFilename)
{
	dNumOfMats = NULL;
	dMatsBuffer = NULL;

	//prep AABB array
	for(int i=0; i < 8; i++)	{
		boxWorld[i] = new D3DXVECTOR3(0.0f,0.0f,0.0f);
	}
	

	minAABB = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	//min vector for aabb
	maxAABB = D3DXVECTOR3(1.0f, 1.0f, 1.0f);;	//max vector for aabb

	//load the mesh 
	bool returned; //used for error check
	//load model
	returned = LoadModel(pFilename, pD3DDevice);
	//load the models textures
	if(returned==true)	{
		LoadImgs(pD3DDevice);	
	}

	//calculate OBB
	CreateOBB();

}
/*******************************************
Game over, lets pack up
*******************************************/
CImportX::~CImportX(void)
{
	if(d_Textures != NULL)
	{
		for(DWORD i = 0; i < dNumOfMats; i++)
		{
			if(d_Textures[i])
			{
				SafeRelease(d_Textures[i]);
			}
		}
	}

	for(int x=0; x < 8; x++)	{
	SafeDelete(boxVerts[x]); //model space
	SafeDelete(boxWorld[x]); //world space
	}

	delete[] *boxVerts;
	delete[] *boxWorld;
	
	SafeDelete(d_Textures);
	SafeDelete(d_Materials);
	
	SafeRelease(dMatsBuffer);
	SafeRelease(dMesh);
}
/*******************************************
render..
*******************************************/
void CImportX::Render(LPDIRECT3DDEVICE9 d_D3dDevice){
	if(dMesh != NULL)	{
		for(DWORD i = 0; i < dNumOfMats; i++)		{
			d_D3dDevice->SetMaterial(&d_Materials[i]);
			d_D3dDevice->SetTexture(0, d_Textures[i]);        
			dMesh->DrawSubset(i);
		}
	}
}

void CImportX::Render(LPDIRECT3DDEVICE9 d_D3dDevice, D3DVECTOR inVec){
	boxLNG = inVec;
	if(dMesh != NULL)	{
		for(DWORD i = 0; i < dNumOfMats; i++)		{
			d_D3dDevice->SetMaterial(&d_Materials[i]);
			d_D3dDevice->SetTexture(0, d_Textures[i]);        
			dMesh->DrawSubset(i);
		}
	}
}
/*******************************************
Load the model 
return a bool error
*******************************************/
bool CImportX::LoadModel(LPSTR pFilename, LPDIRECT3DDEVICE9 d_D3dDevice)	{
	//load mesh
	if(D3D_OK==(D3DXLoadMeshFromX(pFilename,				//file address
								D3DXMESH_SYSTEMMEM,		//options
								d_D3dDevice,			//d3ddevice
								NULL,					//The adjacency data contains an array of three DWORDs per face that specify the three neighbors for each face in the mesh
								&dMatsBuffer,			//buffer for matterials
								NULL,					//Pointer to a buffer containing an array of effect instances
								&dNumOfMats,			//number of materials
								&dMesh	)))				//pointer to the interface of the mesh

	/*if(loaded mesh ok)*/	{	
		return true;		}

	//failed..
	M_BoxOk("void CImportModel::LoadModel( LPSTR pFilename )\n Failed to load given pFilename."); exit(8);
	return false;
}
/*******************************************
Load matterials and textures, 
return bool error
*******************************************/
bool CImportX::LoadImgs(LPDIRECT3DDEVICE9 d_D3dDevice)	{

	//get pointer to the first mat
	D3DXMATERIAL* matMaterials = (D3DXMATERIAL*)dMatsBuffer->GetBufferPointer();
    
	//create arrays that have enuf space for the mats..
	d_Materials = new D3DMATERIAL9[dNumOfMats];
    d_Textures  = new LPDIRECT3DTEXTURE9[dNumOfMats];

    //go thought the buffer and copy the materials to the array
	for(DWORD i = 0; i < dNumOfMats; i++)
    {
        // copy mat to array
        d_Materials[i] = matMaterials[i].MatD3D;

        //set lighting
		d_Materials[i].Ambient = d_Materials[i].Diffuse;

		// create the texture
		if(D3D_OK != D3DXCreateTextureFromFile(d_D3dDevice, matMaterials[i].pTextureFilename, &d_Textures[i]))	{
			return false;	}

    }
    
	//Make sure that the normals are setup for our mesh
	//dMesh->CloneMeshFVF(D3DXMESH_MANAGED, MESH_D3DFVF_CUSTOMVERTEX, d_D3dDevice, &dMesh);	
	D3DXComputeNormals(dMesh, NULL);
	return true;
}
/*******************************************
Created the OBB which is a box made of 2 
vectors, this box will be alligned in model 
space.

this only has to be called 1 time as we save
the box verts in private members
*******************************************/
void CImportX::CreateOBB(void)	{

	//locals
	D3DXVECTOR3 min;		//holds min box size
	D3DXVECTOR3 max;		//holds max box size
	DWORD numVerts;			//used to calc box
	DWORD bytesPerVert;		//used to calc box
	D3DXVECTOR3 *pVert;		//used to calc box


	//get num verts and bytes per vert for the mesh
	numVerts = dMesh->GetNumVertices();
	bytesPerVert = dMesh->GetNumBytesPerVertex();


	//get a pointer for the first vert 
	if(SUCCEEDED(dMesh->LockVertexBuffer(D3DLOCK_DISCARD,(LPVOID*)&pVert))) {
		dMesh->UnlockVertexBuffer();
				}

	//compute bounding box?
	if(D3DXComputeBoundingBox(pVert, numVerts, bytesPerVert, &min, &max) == D3DERR_INVALIDCALL)
		{	M_BoxOk("BOX NOT MADE!!! this means error!!");	}

	//now stick that into the 8 cords needed for aabb...
	boxVerts[0] = new D3DXVECTOR3(min.x, min.y, min.z);
	boxVerts[1] = new D3DXVECTOR3(max.x, max.y, max.z);
	boxVerts[2] = new D3DXVECTOR3(max.x, min.y, min.z);
	boxVerts[3] = new D3DXVECTOR3(min.x, max.y, min.z);
	boxVerts[4] = new D3DXVECTOR3(min.x, min.y, max.z);
	boxVerts[5] = new D3DXVECTOR3(max.x, max.y, min.z);
	boxVerts[6] = new D3DXVECTOR3(max.x, min.y, max.z);
	boxVerts[7] = new D3DXVECTOR3(min.x, max.y, max.z);
	//these are stored in this class for use later by UpdateAABB, this will set a aabb for this model in world space.

}
/*******************************************
this transforms the OBB into world space
and works out the min and max vectors, these 
are stored as private members to be used 
by the collision detection
*******************************************/
void CImportX::UpdateAABB(D3DXMATRIX matTotal)	{
	//holds our temp vector
		D3DXVECTOR3 temp;
	//open loop, objective is to transform each box vert to world space so they match up with the model
		for( int i = 0; i < 8; i++ ){
		//transform
		D3DXVec3TransformCoord(&temp, boxVerts[i], &matTotal );
		//save in our array
		boxWorld[i]->x = temp.x;
		boxWorld[i]->y = temp.y;
		boxWorld[i]->z = temp.z;
		}

	//Now pick the 2 verts that make the min and max for collisions...
		//prepare for loop: set our min and max to the first entrys
		minAABB.x = boxWorld[0]->x;
		minAABB.y = boxWorld[0]->y;
		minAABB.z = boxWorld[0]->z;

		maxAABB.x = boxWorld[1]->x;
		maxAABB.y = boxWorld[1]->y;
		maxAABB.z = boxWorld[1]->z;

		//go through and find the min and max vectors
		for(int i = 0; i < 8 ; i++)	{
			if(minAABB.x > boxWorld[i]->x)	{	minAABB.x = boxWorld[i]->x;	}
			if(maxAABB.x < boxWorld[i]->x)	{	maxAABB.x = boxWorld[i]->x;	}

			if(minAABB.y > boxWorld[i]->y)	{	minAABB.y = boxWorld[i]->y;	}
			if(maxAABB.y < boxWorld[i]->y)	{	maxAABB.y = boxWorld[i]->y;	}

			if(minAABB.z > boxWorld[i]->z)	{	minAABB.z = boxWorld[i]->z;	}
			if(maxAABB.z < boxWorld[i]->z)	{	maxAABB.z = boxWorld[i]->z;	}			
		}

}
/*******************************************
returns the minAABB as a vector3
*******************************************/
D3DXVECTOR3 CImportX::ReturnMin()	{
	return minAABB;
}
/*******************************************
returns the maxAABB as a vector3
*******************************************/
D3DXVECTOR3 CImportX::ReturnMax()	{
	return maxAABB;
}

D3DVECTOR CImportX::ReturnLNG()		{
	return boxLNG;
}