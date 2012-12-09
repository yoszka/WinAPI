// tools.cpp : Defines the auxiliary tools
// autor:  Tomasz Jokiel

// *********** Includes ****************************************************************************

#include "stdafx.h"
#include <windows.h>
#include <string>
#include "tools.h"
#include "main.h"

// *********** Definitions and Enums ***************************************************************
// *********** Function prototypes *****************************************************************
// *********** Global variables definition *********************************************************
// *********** Declaration of external global variables ********************************************
// *********** Typedefs ****************************************************************************
// *********** Implememntation *********************************************************************

using namespace std;

/***************************************************************************************************
* brief      : Print given tekst as hex values
* side effect:
***************************************************************************************************/
string vPrintAsHex(BYTE* str, unsigned int length)
{
  char acCharacterBuffer[10] = {0};
  string strOutHexString;

  for(int i = 0; i < length; i++)
  {
    sprintf_s(acCharacterBuffer, 10, "%02X ", str[i]);
    strOutHexString += acCharacterBuffer;
  }
  
  return strOutHexString;
}


/***************************************************************************************************
* brief      : Set checked sytate for choosen radio button - receive message indicator
***************************************************************************************************/
VOID vSetServoStateIndidator(UINT uiRadioButtonId)
{
  CheckRadioButton (g_hMainWindow, uiRadioButtonId, uiRadioButtonId, uiRadioButtonId);
}


/***************************************************************************************************
* brief      : Set unchecked sytate for choosen radio button - receive message indicator
***************************************************************************************************/
VOID vClearServoStateIndidator(UINT uiRadioButtonId)
{
  CheckRadioButton (g_hMainWindow, uiRadioButtonId, uiRadioButtonId, uiRadioButtonId+100);
}


// EOF
