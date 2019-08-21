//CLightManager - whats this? a joke?
//this class will be responsible for the creation and eventual 
//deletion of all lights, and can be used to controll all aspects
//of any given light in the scene.
//
//this will also make the addition of any special light functions
// easyer to impliment as a "test function".
//
// one of these special functions i hope to have active is distance
// switching, turning lights on and off depending on distance from 
// viewer. this is usefull as graphics cards have a limit on how 
// many active lights there are.
//
/////////////////////////////////////////////////////////////////////
//I am going to try using a linked list structure for the light list,
//there are 2 main reasons for this, 1 this is a test to see if it 
//would be worth making a class to create linked list, and 2 becuse 
//i think that there could be multiple linked lists used in this 
//project, the core render function is a good example.
//
//so make the class abit more friendly, i have decided that the 
//constructor should setup the first node of the list, i have decided
//to make this first item a directional light.

#include "LightManager.h"

CLightManager::CLightManager(CDebugToScreen *p_Debug, CInput *p_Input)	{
	d_Debug = p_Debug;		//set debug
	d_Input = p_Input;		//setup input, this is just to turn on lights in real time.
	p_StartNode = NULL;

	d_Debug->AddItem("Init lighting...", D3DXCOLOR(0.5f, 1.0f, 0.0f, 1.0f));

	dMaxLights = 5;
}

CLightManager::~CLightManager(void)	{
	LIGHT_NODE *next;		//temp to store next pointer
	while(p_StartNode)	{
		//save next node
		next = p_StartNode->p_NextNode;
		//delete start
		SafeDelete(p_StartNode);
		//set start to the new begining
		p_StartNode = next;
	}
}
//******************************************************
//Adds a directional light to the list
//******************************************************
void CLightManager::AddDirectLight(int lightID, LPDIRECT3DDEVICE9 in_D3dDevice, 
									D3DXVECTOR3 inDirection, float inRange, D3DXCOLOR inColour, int inPrio)	{
    //directional lights require the vector to be normalized
	D3DXVec3Normalize(&inDirection, &inDirection);
	//create and populate light to add
       D3DLIGHT9 temp;
       ZeroMemory( &temp, sizeof(D3DLIGHT9) );
       temp.Type    = D3DLIGHT_DIRECTIONAL;
       temp.Diffuse = inColour;
       temp.Range   = inRange;
	   temp.Direction = inDirection;

       // Create a direction for our light - it must be normalized 
       D3DXVECTOR3 tempVec;
       tempVec = D3DXVECTOR3(0.2f,-0.5f,0.5);
       D3DXVec3Normalize( (D3DXVECTOR3*)&temp.Direction, &tempVec );

	   //setup rest of struc
	   LIGHT_NODE *LightTemp = new LIGHT_NODE();	//holds the new light
	   LIGHT_NODE *next = p_StartNode;				//used to get end of list

	   LightTemp->active = true;
	   LightTemp->ID = lightID;
	   LightTemp->light = temp;
	   LightTemp->p_NextNode = NULL;
	   LightTemp->priority = inPrio;

	   d_Debug->AddItem("Adding light...", D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));	

	   //if we havnt setup our start node yet...
	   if(p_StartNode == NULL)	{
		   p_StartNode = LightTemp;	
			return;	}

	   //find end of the list
	   while(next->p_NextNode != NULL)	{
		   next = next->p_NextNode;
	   } 
		
	   //else setup the nodes after

		next->p_NextNode = LightTemp;	   
}


//******************************************************
//turns a light off, my first attempt at a usefull
//recursive function
//use: LightManager->TurnOffLight(ID, LightManager->d_StartNode());
//******************************************************
void CLightManager::TurnOffLight(int lightID, LIGHT_NODE *inNode)	{
	//exit if node == null
	if( inNode->p_NextNode == NULL )	{
		return;	}
	if(lightID != inNode->ID)	{
		//if this is not the right light, go to next node
		TurnOffLight(lightID, inNode->p_NextNode);
	}

	d_Debug->AddItem("Light removed, ID: "+lightID, D3DXCOLOR(0.5f, 1.0f, 0.0f, 1.0f));
	//if we get to here, this is the light we are after!
	inNode->active = false;
	return;
}
//******************************************************
//This should decided on which lights should be on,
// and i say this roughly as this is the main part
//to be changed in further revision.
//then set them on to be rendered.
//******************************************************
void CLightManager::UpdateLights(LPDIRECT3DDEVICE9 in_D3dDevice)	{
	if(d_Input->ReturnKey(DIK_W))	{
		TurnOnClosestLights(in_D3dDevice);
	}
}

//******************************************************
//work out distance from light -> eye
//
//and create a list of the closest
//******************************************************
void CLightManager::TurnOnClosestLights(LPDIRECT3DDEVICE9 in_D3dDevice)	{
	LIGHT_NODE *current = p_StartNode;

	//for the moment just run on fifo
	for(int i=0; i < dMaxLights; i++)	{
		if(current != NULL)	{
			in_D3dDevice->SetLight( i, &current->light );
			in_D3dDevice->LightEnable( i, TRUE );
			current = current->p_NextNode;
		}
	}
}



void CLightManager::TurnOffAllLighting(LPDIRECT3DDEVICE9 in_D3dDevice)		{//for sky box
	LIGHT_NODE *current = p_StartNode;

	//for the moment just run on fifo
	for(int i=0; i < dMaxLights; i++)	{
		if(current != NULL)	{
			in_D3dDevice->LightEnable( i, false );
			current = current->p_NextNode;
		}
	}
}
