/***************************************************************\
*This class is for debug help, the idea is to pass a string to	*
*this class it will add a structure to the end of a vector		*
*which represents the info we wanna display. an extra note is 	*
*this class is not usefull for windows or a range of d3d errors	*
*****************************************************************
*I beleive this class will be usefull for AI command debuging	*
*and other "game logic" debug messages, this can be viewed as a *
*primative "console", for later revisions:						*
*	-Filter for different subjects (AI, physics, collisions)	*
*	-way to input a command, and have the input controll apects *
*		of the game												*
*	-intergrate into player hud as a "console"					*
*****************************************************************
*struct DISPLAY_ITEM	{										*
*		std::string text;										*
*		D3DXCOLOR textColour;									*
*		bool essential;											*
*	};															*
*this is the kind of structure i am thinking of, with the		*
*possibility of adding either priority or filter.				*
*****************************************************************
*there should be a log on screen with essential details always	*
*avalible from at the top of the screen, these are things like	*
*current frame, avg fps, run time.								*
*																*
*after these essential details a list of size n lines of output.*
*****************************************************************
*ie. if we include the 3 essentials, and have a list of total	*
*size 10we can then add lines of output till we reach 10, at	*
*this point the screen is full and we will drop the first none	*
*essential item on the list. the point of this? real time		*
\***************************************************************/

#include "DebugToScreen.h"
/*********************************************
constructor... the code blocks are sperated
by these comments for easy location of blocks
*********************************************/
CDebugToScreen::CDebugToScreen(LPDIRECT3DDEVICE9 in_D3dDevice)	{

	//setup our vars
	dD3dxFont = NULL;				//our text interface
	dStartTime = timeGetTime();		//start time so we can work out the fps
	dFrameNum = 0;					//current frame number
	dFps = 0;						//avg fps

	//setup locals
	HRESULT hr;
	HDC hDC;
	int nHeight = 0;
	int nPointSize = 10;

	 //////////////////////////
	//create our text object//
   //////////////////////////

	//getdc: retrieves a handle to a display device context 
	hDC = GetDC( NULL );

	nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) ); //work out font size

	ReleaseDC( NULL, hDC );	//no longer needed, release

	//create our text object
	hr = D3DXCreateFont(	in_D3dDevice, 
							nHeight, 
							0, 
							FW_BOLD, 
							0, 
							FALSE, 
							DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, 
							DEFAULT_QUALITY, 
							DEFAULT_PITCH | FF_DONTCARE, 
							TEXT("Arial"), 
							&dD3dxFont );

	//if we fail to create the text object, show a msg box to tell us.
	if( FAILED( hr ) )
		M_BoxOk("Call to D3DXCreateFont failed!");

	//setup the frame num / fps / run time
	SetupEssentialInfo();
}
/*******************************************
delete..
*******************************************/
CDebugToScreen::~CDebugToScreen(void)	{
	SafeRelease(dD3dxFont);
}
/*******************************************
Render...
*******************************************/
void CDebugToScreen::Render(bool inBool)	{
	//update fps/frame num/ run time
	UpdateEssentialInfo();

	if(inBool == true)	{
		AddItem("w0000t, i r legend!", D3DXCOLOR(0.0f, 0.5f, 1.0f, 1.0f));	
	}

	//define a rectangle and place it in the first position
	RECT destRect1;
    SetRect( &destRect1, 0, 0, 0, 0 );

	//loop through the list and print the list, each line shifted down 20 pixels 
	for(int i = 0; i < (int)vItemList.size(); i++)	{
		//print item
	int temp = dD3dxFont->DrawText(	NULL,							//handle to window
								vItemList.at(i).text.c_str(),		//text to print
								(int)vItemList.at(i).text.size(),	//size of string... if -1, string is assuemed to be null terminated  
								&destRect1,							//rectangle to draw to
								DT_NOCLIP,							//clip options
								vItemList.at(i).textColour );		//colour of the text

	if(temp == 0)	{
		//M_BoxOk("eeeek we broken down");	
	}
	//set the rectangle 20 lower then the priviouse line.
    SetRect( &destRect1, 0, 20+(i*20), 0, 0 );

	}
	//update frame num
	dFrameNum++;
}
/*******************************************
adds an item to the vItemList

if the array is not full, fill it, else
delete first none essential and push the
new entry to the back
*******************************************/
void CDebugToScreen::AddItem( std::string inString, D3DXCOLOR inColour)	{
	DISPLAY_ITEM temp;
	//setup the temp struc
	temp.text = inString;
	temp.textColour = inColour;
	temp.essential = false;	//none important message

	int dSize = (int)vItemList.size(); //get the total size of the array

	if(dSize < 10)	{	//if array hasnt got 10 lines yet..
		//add new item to list
		vItemList.push_back(temp);	
	}

	else	{	//else the array is full...

		//loop through the list, find the first none essential item
		for(int i = 0; i < (int)vItemList.size(); i++)	{
			//is this item essential??
			if(vItemList.at(i).essential == false)	{	
				//if not, delete entry i
				vItemList.erase(vItemList.begin() + i);	
				//push a new item to the back
				vItemList.push_back(temp);	
				return;
			}
		}
	}
}
/*******************************************
initialise the essential outputs

frame number
avg frames this run
run time this run

all text should be the same colour to 
represent they are essential.

as red normally represents a locked state
red would be the ideal choice.
*******************************************/
void CDebugToScreen::SetupEssentialInfo(void)	{
	DISPLAY_ITEM temp;

	//setup frame number...
	temp.essential = true;
	temp.text = "Frame number: " + dFrameNum;
	temp.textColour.r = 1.0f;
	temp.textColour.g = 0.0f;
	temp.textColour.b = 0.0f;
	temp.textColour.a = 1.0f;	//alpha
	//add item to list, this will be index 0
	vItemList.push_back(temp);  

	//setup fps, at this point we cant do a decent avg, so for the first sec of run time, display "claculating..."
	temp.text = "Calculating avg fps... ";
	//add item to list, this will be index 1
	vItemList.push_back(temp);

	//add run time..
	DWORD runTime = timeGetTime() - dStartTime;
	temp.text = ""+runTime;
	//add item to list, this will be index 2
	vItemList.push_back(temp);


}
/*******************************************
Updates the fps/frame numer and run time

this could have more accurate equations,
*******************************************/
void CDebugToScreen::UpdateEssentialInfo(void)	{
	//calc run time...
	DWORD runTime = (timeGetTime() - dStartTime) / 1000;
	//make string stream
	std::stringstream tempStream;
	std::string	tempString; 

	tempStream << "Frame number: " << dFrameNum;
	//update frame number
	vItemList.at(FRAMENUM).text = tempStream.str();

	//update avg fps if time is over 1 sec
	if(runTime > 5)	{		//avoid divide 0 and avoid erratic fps read out at start
		tempStream.str(""); //clear the stream
		dFps = (dFrameNum / runTime); //avg fps = frames rendered / seconds of render time, this could be better! but for what we require in this project i do not beleive it is needed
		tempStream <<  "Avg FPS: " << dFps;
		vItemList.at(AVGFPS).text =	tempStream.str();	}

	//update runtime
		tempStream.str(""); //clear the stream
		tempStream <<"Run time: " << runTime << " seconds";
		vItemList.at(RUNTIME).text = tempStream.str();	
}