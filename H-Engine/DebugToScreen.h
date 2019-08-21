#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <vector>
#include <string>
#include <sstream> //this allows int to string convertion, and easy construction of strings.
#include "helper.h"

struct DISPLAY_ITEM	{				//represents a single items to be printed to screen								
		std::string text;			//text to be printed							
		D3DXCOLOR textColour;		//colour of the text
		bool essential;				//is this item essential? and should it be kicked off the list?		
	};

//define array positions for the essential details
#define FRAMENUM 0
#define AVGFPS 1
#define RUNTIME 2

class CDebugToScreen
{
public:
	CDebugToScreen(LPDIRECT3DDEVICE9 in_D3dDevice);
	~CDebugToScreen(void);
	void Render(bool inBool);					//render the text objects
	void AddItem( std::string inString, D3DXCOLOR inColour);	//adds an item to output list

private:
	void SetupEssentialInfo(void);  //sets up the initial info in the struc
	void UpdateEssentialInfo(void);	//update essential info, this is frame#, avg fps, run time.

	LPD3DXFONT dD3dxFont;			//our text interface
	DWORD dStartTime;				//start time so we can work out the fps
	int dFrameNum;					//current frame number
	int dFps;						//avg fps
	static const int dListMax = 10; //max size of the list, static cus i dont want this changin at run time

	std::vector<DISPLAY_ITEM> vItemList;//the list of items that need to be printed to screen.
};
