/* This is a usefull header, this has alot of usefull macros and definitions,
it would be better to have a few different headers with more specialised
function, but as the code is small and does not conflict in anyway, i think 
it is fine to leave it alone for now.*/


#if !defined(AFX_HELPER_H)
#define AFX_HELPER_H

#define SafeRelease(pInterface) if(pInterface != NULL) {pInterface->Release(); pInterface=NULL;}
#define SafeDelete(pObject) if(pObject != NULL) {delete pObject; pObject=NULL;}

//define key states
#define KEYDOWN(name, key) (name[key] & 0x80) 
#define MOUSEBUTTONDOWN(key) (key & 0x80)

//mouse buttons, used at CInput::UpdateMouse()
#define MOUSEBUTTON_LEFT 0
#define MOUSEBUTTON_RIGHT 1
#define MOUSEBUTTON_MIDDLE 2

/*********************
Mine!
**********************/
#define M_BoxOk(inString)	MessageBox(NULL,inString, "ERROR",MB_OK|MB_ICONEXCLAMATION);
#define M_BoxYesNo(inString)	MessageBox(NULL,inString, "ERROR",MB_YESNO|MB_ICONEXCLAMATION);

#include <d3dx9.h>
struct CAMERA_INFO	{					//used to return mouse information to the camera
		bool d_MouseLeft, d_MouseRight;		//true = button down
		int xDiff, yDiff;				//how much difference between frames	
		D3DXVECTOR3 eyeOffSet;				//used to translate x/y/z movement of eye vec
		bool dQuit;						//quit
		D3DXVECTOR3 boxOffSet;			//box movement
		bool dShowBox;
	};
/*********************
/End Mine!
**********************/
#endif