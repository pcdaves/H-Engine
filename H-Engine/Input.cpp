#include "Input.h"

CInput::CInput(HWND hWnd, HINSTANCE hInst, CDebugToScreen* Debug)
{
	pDirectInput = NULL;		//holds our direct input object
	pKeyboard = NULL;			//holds our keyboard
	pMouse = NULL;			//hold our mouse

	dQuit = false;			//exit if true!
	dShowBox = false;

	offSet.x = offSet.y = offSet.z = 0.0;
	offSet2.x = -30.0;
	offSet2.y = 10.0;
	offSet2.z = 30.0;

	d_Debug = Debug;



	//Create the DirectInput object
	SetupDirectInput(hInst);
	//setup keyboard
	SetupKeyboard(hWnd);
	//setup mouse
	SetupMouse(hWnd);
}

CInput::~CInput(void)
{	//released in reverse order to initiation...
	if(pKeyboard)
	{
		pKeyboard->Unacquire(); 
	}

	if(pMouse)
	{
		pMouse->Unacquire();
	}

	SafeRelease(pMouse);
	SafeRelease(pKeyboard);
	SafeRelease(pDirectInput);
}

/*******************************************
create a direct input interface
return true if all goes well
*******************************************/
bool CInput::SetupDirectInput(HINSTANCE hInst)	{
	if(FAILED(DirectInput8Create(	hInst, 
									DIRECTINPUT_VERSION, 
									IID_IDirectInput8, 
									(void**)&pDirectInput, 
									NULL))) 	{ 
		//indicate a failure has happened
		M_BoxOk("Unable to create DirectInput interface.");
	}
	else	{	return true;	} //worked so return true

	//we failed... return false
	return false;
}
/*******************************************
Setup the keyboard  
return true if all goes well
*******************************************/
bool CInput::SetupKeyboard(HWND hWnd)	{
	//create the keyboard
	if(FAILED(pDirectInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL))) 	{ 
		M_BoxOk("failed to create DirectInput keyboard");
		return false; 
	}


	//set data format
	if(FAILED(pKeyboard->SetDataFormat(&c_dfDIKeyboard)))	{ 
		M_BoxOk("failed to set the keyboard data format");
		return false; 
	}


	//set cooperative level 
	if(FAILED(pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))		{ 
		M_BoxOk("failed keyboard cooperative level");
		return false;
	}
	

	//acquire the keyboard
	if(pKeyboard)	{
		pKeyboard->Acquire(); 
	}

	return true;
}
/*******************************************
setup the mouse
return true if all goes well
*******************************************/
bool CInput::SetupMouse(HWND hWnd)	{

	//Create the mouse device object
	if(FAILED(pDirectInput->CreateDevice(GUID_SysMouse, &pMouse, NULL)))	{ 
		M_BoxOk("failed to create mouse");
		return false; 
	}
	

	//Set the data format for the mouse
	if(FAILED(pMouse->SetDataFormat(&c_dfDIMouse)))	{ 
		M_BoxOk("failed to set the mouse");
		return false; 
	}
	

	//Set the cooperative level for the mouse
	if(FAILED(pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))	{ 
		M_BoxOk("failed mouse cooperative level.");
		return false;
	}
	

	//Acquire the mouse
	if(pMouse)	{
		pMouse->Acquire(); 
	}

	return true;

}
/*******************************************
Update the keyboard
*******************************************/
void CInput::UpdateKeyboard(void)	{
	char keys[256]; 

	//reset offset vector, as we want fresh info
	offSet.x = offSet.y = offSet.z = 0.0;
    
	//Acquire the keyboard if we fail
	if(pKeyboard)		{
		pKeyboard->Acquire(); }

	if(FAILED(pKeyboard->GetDeviceState(sizeof(keys),(LPVOID)&keys)))    { 
	//failed, msg box to tell the user then return out of here
	//stupid idea of the year to me =)
		//M_BoxOk("Failed to get device state");
		return; 		 
	}

	//*****************
	//check key states
	//*****************

	if(KEYDOWN(keys, DIK_ESCAPE))	{
        //Escape key pressed. Quit game.
		dQuit = true;
	}

	if(KEYDOWN(keys, DIK_MINUS) && dShowBox == true)	{
        //hide the collision box
		dShowBox = false;
		d_Debug->AddItem("Box hidden.", D3DXCOLOR(1,1,1,1));
	}

	if(KEYDOWN(keys, DIK_EQUALS) && dShowBox == false)	{
        //show the collision box
		dShowBox = true;
		d_Debug->AddItem("Box showing.", D3DXCOLOR(1,1,1,1));
	}

	float offSetAmmount = 1.0f;		
	//move X
    if(KEYDOWN(keys, DIK_LEFT))	{
		offSet.x -= offSetAmmount;
	}
    else if(KEYDOWN(keys, DIK_RIGHT))	{
		offSet.x += offSetAmmount;
	}


	//move Y
    if(KEYDOWN(keys, DIK_NEXT))	{
		offSet.y -= offSetAmmount;
	}
    else if(KEYDOWN(keys, DIK_PRIOR))	{
		offSet.y += offSetAmmount;
	}


	//move Z
    if(KEYDOWN(keys, DIK_DOWN))		{
		offSet.z -= offSetAmmount;
	}
    else if(KEYDOWN(keys, DIK_UP))	{
		offSet.z += offSetAmmount;
	}


	//move box
	//move X
    if(KEYDOWN(keys, DIK_S)&& dShowBox == true)	{
		offSet2.x -= offSetAmmount;
	}
    else if(KEYDOWN(keys, DIK_W)&& dShowBox == true)	{
		offSet2.x += offSetAmmount;
	}


	//move Y
    if(KEYDOWN(keys, DIK_Q)&& dShowBox == true)	{
		offSet2.y -= offSetAmmount;
	}
    else if(KEYDOWN(keys, DIK_E)&& dShowBox == true)	{
		offSet2.y += offSetAmmount;
	}


	//move Z
    if(KEYDOWN(keys, DIK_D)&& dShowBox == true)		{
		offSet2.z -= offSetAmmount;
	}
    else if(KEYDOWN(keys, DIK_A)&& dShowBox == true)	{
		offSet2.z += offSetAmmount;
	}
}
/***********************************************
Update mouse and save its new position+keydowns
***********************************************/
void CInput::UpdateMouse(void)	{
	DIMOUSESTATE MouseState;

	//our current is now out of date, so make it our previouse
	d_PrevMouseX = d_MouseX;
	d_PrevMouseY = d_MouseY;

	//if needed, first aquire the mouse
	if(pMouse)	{
		pMouse->Acquire(); 
	}

	//get state of the mouse
	if(FAILED(pMouse->GetDeviceState(sizeof(MouseState),(LPVOID)&MouseState)))    { 
		//M_BoxOk("Failed to grab mouse");
		return; 
    } 

	//Is the left mouse button down?
	if(MOUSEBUTTONDOWN(MouseState.rgbButtons[MOUSEBUTTON_LEFT]))	{
		d_MouseLeft = true;
	}
	else	{
		d_MouseLeft = false;
	}

	//Is the right mouse button down?
	if(MOUSEBUTTONDOWN(MouseState.rgbButtons[MOUSEBUTTON_RIGHT]))	{
		d_MouseRight = true;
	}
	else	{
		d_MouseRight = false;
	}
    
	//save current mouse pos
	d_MouseX += MouseState.lX;
	d_MouseY += MouseState.lY;
}
/*******************************************
Render...
*******************************************/
CAMERA_INFO CInput::Update(void)	{
	UpdateMouse();
	UpdateKeyboard();

	return ReturnMouseStruc();
}
/*******************************************
return the quit var, bool if true = quit
*******************************************/
bool CInput::CheckQuit(void)	{
	return dQuit;
}
/*******************************************
returns...
*******************************************/
CAMERA_INFO CInput::ReturnMouseStruc()	{
	CAMERA_INFO temp;
	temp.d_MouseLeft = d_MouseLeft;
	temp.d_MouseRight = d_MouseRight;
	temp.xDiff = d_MouseX - d_PrevMouseX;
	temp.yDiff = d_MouseY - d_PrevMouseY;
	temp.eyeOffSet = offSet;
	temp.dQuit	= dQuit;
	temp.boxOffSet = offSet2;
	temp.dShowBox = dShowBox;

	return temp;
}
/*******************************************
returns a struct which represents which way
the camera should move.

each axis is either -1.0, 0.0 or 1.0
this means if offset.x = 1.0 we must 
move along that axis (going up)

if it is -1.0 we move down the axis.
0.0 implys no movement.
*******************************************/
D3DXVECTOR3 CInput::ReturnOffSet()	{
	return offSet;
}

/*******************************************
return the state for a single key
*******************************************/
bool CInput::ReturnKey(WORD inKey)	{
	char keys[256]; 

	//Acquire the keyboard if we fail
	if(pKeyboard)		{
		pKeyboard->Acquire(); }

	if(FAILED(pKeyboard->GetDeviceState(sizeof(keys),(LPVOID)&keys)))    { 
	//failed, msg box to tell the user then return out of here
	//stupid idea of the year to me =)
		//M_BoxOk("Failed to get device state");
		return false; 		 
	}

	//*****************
	//check key state
	//*****************

	if(KEYDOWN(keys, inKey))	{
        //key is down
		return true;
	}

	return false; //it wasnt pressed down
}

void CInput::UpdateBoxOffSet(D3DVECTOR inVec)		{
	offSet2 = inVec;
}