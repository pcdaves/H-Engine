/*************************************************************
This is the entry point for the "H-Engine" application.

the main parts of d3d should not be done here!

this is for setup and init of the game. 

after setup a new game core should be made and init called 
on it, this will be passed to the main game loop till 
termination of the game
**************************************************************/

#include <Windows.h>
#include <mmsystem.h>
#include "helper.h"
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <crtdbg.h>
#include "Core.h"

/*******************************************
First job is to define our global variables.
*******************************************/
LPDIRECT3D9             g_D3D           = NULL; 
LPDIRECT3DDEVICE9       g_D3DDEVICE     = NULL; 
CCore*					d_Core;

WNDCLASSEX				window;

/*******************************************
Prototype our functions.
*******************************************/
HRESULT InitD3d(HWND hWnd, HINSTANCE hInst);
HWND InitWindow(void);
bool OpenCore(HWND hWnd, HINSTANCE hInst);
void ShutDown(void);
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

/*******************************************
Entry point of the app!
*******************************************/
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )	{
	//create window
	HWND hWnd = InitWindow();
	//initialise d3d
	InitD3d(hWnd, hInst);
	//show and update the window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

	//open core
	OpenCore(hWnd, hInst);
	//enter main loop
	d_Core->MainLoop();

	//EXIT must have been called or an error happened if we dropped from main loop
	//shutdown and release mem bk to system, if possible add an error check and output.
	ShutDown();
	//check for memory leaks
	UnregisterClass( "H-Engine", window.hInstance );
	_CrtDumpMemoryLeaks();
	return 0;
}

/*******************************************
Setup our window here
*******************************************/
HWND InitWindow(void)	{
	// Register the window class
	WNDCLASSEX window =  {	sizeof(WNDCLASSEX), 
							CS_CLASSDC,			//style
							MsgProc,			//pointer to our msg proc
							0L,					//extra space
							0L,					//wnd extra space
							GetModuleHandle(NULL),	//where to find the msg proc?
							NULL,				//icon?
							NULL,				//cursor setting
							NULL,				//background not much use
							NULL,				//menu
							"H-Engine",			//class name
							NULL };				//icon?

	RegisterClassEx(&window);

	//create the window!
	HWND hWnd = CreateWindow( "H-Engine",				//class name
							  "AABB Demo",				//window name!
                              WS_OVERLAPPEDWINDOW,		//window style
							  25,						//x offset
							  25,						//y offset
							  1100,						//width
							  700,						//height
                              NULL,						//parent window
							  NULL,						//menu setup
							  window.hInstance,				//Handle to the instance of the module to be associated with the window.
							  NULL );

	return hWnd;
}


/*******************************************
Setup D3D
*******************************************/
HRESULT InitD3d(HWND hWnd, HINSTANCE hInst)	{
	// Create the D3D object.
    if( NULL == ( g_D3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{	return E_FAIL;	}

    // setup the struc used to create the d3ddevice
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED	( g_D3D->CreateDevice(	D3DADAPTER_DEFAULT,						//which adapter
										D3DDEVTYPE_HAL,							//what type of device are we using?
										hWnd,									
										D3DCREATE_SOFTWARE_VERTEXPROCESSING,	//behavior flags
										&d3dpp,									//presentation params specified above
										&g_D3DDEVICE	) ) )					//returned interface
    {	//couldnt make d3ddevice, so fail and return.
        return E_FAIL;
    }

	//d_Input = new CInput(hWnd,hInst);

    // enable zbuffer
    g_D3DDEVICE->SetRenderState( D3DRS_ZENABLE, TRUE );

    // enable a low ambient lighting 
    g_D3DDEVICE->SetRenderState( D3DRS_AMBIENT,RGB(30,30,30));

	//enable lighting
	g_D3DDEVICE->SetRenderState(D3DRS_LIGHTING,true);

	//enable culling on counter clock wise faces!
	g_D3DDEVICE->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

    return S_OK;
}





/*********************************************
this is the msg handeler specified in winmain
*********************************************/
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
			//tell core to shut down
			PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

/*********************************************
2 mothods that init the core
dont require there down function, 
but it looks nice =)
*********************************************/
bool OpenCore(HWND hWnd, HINSTANCE hInst)	{
	//create core
	d_Core = new CCore(g_D3DDEVICE);
	//init core, returned if we are good to go
	d_Core->InitCore( hWnd, hInst);

	return true; //return if we are good to enter the main loop!
}
/*********************************************
time to go! clean up the mem used 
and return it to the system
*********************************************/
void ShutDown(void)	{
	SafeDelete(d_Core);

	if( g_D3D != NULL )
        g_D3D->Release();

	if( g_D3DDEVICE != NULL )
        g_D3DDEVICE->Release();
}