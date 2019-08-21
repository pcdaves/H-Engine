#include "Core.h"
/*******************************************
init global variables
*******************************************/
CCore::CCore(LPDIRECT3DDEVICE9 in_D3dDevice)
{
	d3dDevice = in_D3dDevice;
	d_MeshObj1 = NULL;
	d_MeshObj2 = NULL;
	d_MeshObj3 = NULL;
	d_MeshObj4 = NULL;
	d_MeshObj5 = NULL;
	d_MeshObj6 = NULL;
	d_MeshObj7 = NULL;
	d_MeshObj8 = NULL;
	d_Camera   = NULL;
	/*d_Input	   = NULL;*/
}
/*******************************************
Game over, lets pack up
*******************************************/
CCore::~CCore(void)
{
	SafeDelete(d_MeshObj1);
	SafeDelete(d_MeshObj2);
	SafeDelete(d_MeshObj3);
	SafeDelete(d_MeshObj4);
	SafeDelete(d_MeshObj5);
	SafeDelete(d_MeshObj6);
	SafeDelete(d_MeshObj7);
	SafeDelete(d_MeshObj8);
	SafeDelete(d_Camera);
	SafeDelete(d_Input);
	SafeDelete(d_Debug);
	SafeDelete(d_LightManager);
}
/*******************************************
there are a 2 reasons for the initcore()
first is to finish off initialising the class.

second is to report any errors during the 
creation of this class, as the constructor 
has no return, and can fail.
*******************************************/
void CCore::InitCore(HWND hWnd, HINSTANCE hInst)	{
	d_Debug = new CDebugToScreen(d3dDevice);
	d_Camera = new CCamera(d3dDevice);
	d_MeshObj1 = new CImportX(d3dDevice, "../Mesh/buildings/ball.x" );
	d_MeshObj2 = new CImportX(d3dDevice, "../Mesh/buildings/rock.x" );
	d_MeshObj3 = new CImportX(d3dDevice,"../Mesh/buildings/complex.x");
	d_MeshObj4 = new CImportX(d3dDevice,"../Mesh/buildings/tallbuilding.x");
	d_MeshObj5 = new CImportX(d3dDevice,"../Mesh/Racer/Tower/tower.x");
	d_MeshObj6 = new CImportX(d3dDevice,"../Mesh/buildings/Singlebuilding.x");
	d_MeshObj7 = new CImportX(d3dDevice,"../Mesh/cube.x");
	d_MeshObj8 = new CImportX(d3dDevice,"../Mesh/buildings/complex.x");
	d_Input = new CInput(hWnd, hInst,d_Debug);

	d_LightManager = new CLightManager(d_Debug, d_Input);	//pass debugg pointer 

	d_LightManager->AddDirectLight(	1,								//light ID - int
						d3dDevice,					//d3d device
						D3DXVECTOR3(0.2f,-0.5f,0.5f),	//direction for light d3dxvec3
						1000.0f,						//range float
						D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f),//d3dxcolor
						1);								//priority

	d_LightManager->AddDirectLight(	2,								//light ID - int
						d3dDevice,					//d3d device
						D3DXVECTOR3(0.0f,-0.5f,0.1f),	//direction for light d3dxvec3
						1000.0f,						//range float
						D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f),//d3dxcolor
						1);								//priority

	d_LightManager->AddDirectLight(	44,				//light ID - int
						d3dDevice,					//d3d device
						D3DXVECTOR3(0.5f,-0.5f,0.2f),	//direction for light d3dxvec3
						1000.0f,						//range float
						D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f),//d3dxcolor
						1);								//priority
}
/*******************************************
main loop.
runs none stop till game over
*******************************************/
void CCore::MainLoop()	{
	int count = 0;
	//Enter the game loop
    MSG msg; 
    BOOL fMessage;

    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	//Game started, so record time
	//m_dwStartTime = timeGetTime();

    while(msg.message != WM_QUIT)
    {
		count++;
        fMessage = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

		if(fMessage)
        {
            //Process message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			int dStartTime = timeGetTime();
			CAMERA_INFO temp; //temp to hold our struc
			//update input, returning a CAMERA_INFO struc
			temp = d_Input->Update();

			//check if we are quitting
			if(temp.dQuit == true)	{	PostQuitMessage( 0 ); /*exit*/	}
			
			//render 3d objects
            Render(temp);
			int dEndTime = timeGetTime();

			if(count%400 == 0)	{

			//work out time taken and put it into a string
			int temp2 = dEndTime-dStartTime;
			char timeTaken[255];
			std::string tmpString;
			tmpString = "Single frame: ";			
			itoa(temp2, timeTaken, 10);
			tmpString += timeTaken;
			tmpString += "ms";

			//print it to console
			d_Debug->AddItem(tmpString.c_str(), D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));
			}

        }
    }
}
/*******************************************
Render...
*******************************************/
void CCore::Render(CAMERA_INFO info)	{

	//update camera eye position
	d_Camera->UpdateViewMatrix(d3dDevice, info);
	d_LightManager->UpdateLights(d3dDevice);

	// Clear the backbuffer and the zbuffer
    d3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( d3dDevice->BeginScene() ) )    {

		D3DXMATRIXA16 test;
		D3DXVECTOR3 carLook;
		D3DXMATRIXA16 matBuilding, matBuildScale;	//building scale / movement matrix
		

		//setup and render sky dome	
		SkyBox();

		//render floating rock	
		D3DXMatrixTranslation(&matBuilding, 0.0f, 0.0f, 0.0f);
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );		
		d_MeshObj2->UpdateAABB(matBuilding);
		d_MeshObj2->Render(d3dDevice);

		//render big building 1		
		D3DXMatrixTranslation(&matBuilding, -70.0f, 70.0f, -73.0f);
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
		d_MeshObj3->UpdateAABB(matBuilding);
		d_MeshObj3->Render(d3dDevice);

		//render the big building
		D3DXMatrixTranslation(&matBuilding, 150.0f, 70.0f, 15.0f);		
		D3DXMatrixScaling(&matBuildScale, 0.5f, 0.5f, 1.0f);
		//multi
		D3DXMatrixMultiply(&matBuilding, &matBuilding, &matBuildScale);
		//world.. aabb.. render..
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
		d_MeshObj8->UpdateAABB(matBuilding);
		d_MeshObj8->Render(d3dDevice);

		//tall building
		//transform
		D3DXMatrixScaling(&matBuildScale, 0.1f, 0.1f, 0.1f);
		D3DXMatrixTranslation(&matBuilding, 960.0f, 0.0f, -430.0f);
		//multi
		D3DXMatrixMultiply(&matBuilding, &matBuilding, &matBuildScale);
		//world...update box.. render..
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
		d_MeshObj4->UpdateAABB(matBuilding);
		d_MeshObj4->Render(d3dDevice);


		//round tower
		//transform
		D3DXMatrixTranslation(&matBuilding, 110.0f, 15.0f, 100.0f);
		D3DXMatrixScaling(&matBuildScale, 0.3f, 0.3f, 0.3f);
		//multiply our 2 transforms
		D3DXMatrixMultiply(&matBuilding, &matBuilding, &matBuildScale);
		//set world, update box, render...
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
		d_MeshObj5->UpdateAABB(matBuilding);
		d_MeshObj5->Render(d3dDevice);


		//small building
		//transform
		D3DXMatrixTranslation(&matBuilding, 550.0f, 50.0f, -600.0f);
		D3DXMatrixScaling(&matBuildScale, 0.07f, 0.1f, 0.07f);
		//multiply our 2 transforms
		D3DXMatrixMultiply(&matBuilding, &matBuilding, &matBuildScale);
		//set world, update box, render...
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
		d_MeshObj6->UpdateAABB(matBuilding);
		d_MeshObj6->Render(d3dDevice);


		//render box
		if(info.dShowBox)	{
		D3DXMatrixTranslation(&matBuilding, info.boxOffSet.x, info.boxOffSet.y, info.boxOffSet.z);
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
		d_MeshObj7->UpdateAABB(matBuilding);

		//check for collision
			if(BboxCollision(d_MeshObj7, d_MeshObj2) || BboxCollision(d_MeshObj7, d_MeshObj3) || BboxCollision(d_MeshObj7, d_MeshObj4) ||
								BboxCollision(d_MeshObj7, d_MeshObj5) || BboxCollision(d_MeshObj7, d_MeshObj6) || BboxCollision(d_MeshObj7, d_MeshObj8))	{
				d_Debug->AddItem("Collision detected", D3DXCOLOR(0.0f, 0.5f, 0.5f, 1.0f));
				//find out the last known good position
				info.boxOffSet = d_MeshObj7->ReturnLNG();
				//update box cords in input
				d_Input->UpdateBoxOffSet(info.boxOffSet);

				//reset the position in 3d world
				D3DXMatrixTranslation(&matBuilding, info.boxOffSet.x, info.boxOffSet.y, info.boxOffSet.z);
				d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );
				d_MeshObj7->UpdateAABB(matBuilding);
			}
		
		d_MeshObj7->Render(d3dDevice, info.boxOffSet);
		}

	}

	//print the text to screen
	d_Debug->Render( d_Input->ReturnMouseStruc().d_MouseRight);

	// End the scene
    d3dDevice->EndScene();   

    // Present the backbuffer contents to the display
    d3dDevice->Present( NULL, NULL, NULL, NULL );

}
/*******************************************
The sky box is only a small ammount of code
i do not think this should be put in a class

the idea is to keep the sky box centered 
over the camera to create an illusion of
distance
*******************************************/
void CCore::SkyBox()	{
		D3DXMATRIXA16 matBuilding;	//building scale / movement matrix
		
		//render sky dome	
		D3DXVECTOR3 temp = d_Camera->ReturnEye();						//get eye position
		D3DXMatrixTranslation(&matBuilding, temp.x, temp.y, temp.z);	//set center of dome to our position
		d3dDevice->SetTransform( D3DTS_WORLD, &matBuilding );		
		d_MeshObj1->UpdateAABB(matBuilding);							//.... not required...
		
		//the lighting should be turned off to render the sky box
		d_LightManager->TurnOffAllLighting(d3dDevice);
		d_MeshObj1->Render(d3dDevice);
		//enable lighting
		d_LightManager->TurnOnClosestLights(d3dDevice);
}

bool CCore::BboxCollision(CImportX * box1, CImportX * box2)
{
	if( box1->ReturnMax().x > box2->ReturnMin().x && box1->ReturnMin().x < box2->ReturnMax().x)	{
		if( box1->ReturnMax().y > box2->ReturnMin().y && box1->ReturnMin().y < box2->ReturnMax().y)	{
			if( box1->ReturnMax().z > box2->ReturnMin().z && box1->ReturnMin().z < box2->ReturnMax().z)	{
				return true;
			}
		}
	}
	return false;
}
