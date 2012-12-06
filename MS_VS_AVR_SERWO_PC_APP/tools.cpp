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
    sprintf(acCharacterBuffer, "%02X ", str[i]);
    strOutHexString += acCharacterBuffer;
  }
  
  return strOutHexString;
}



/***************************************************************************************************
* brief      : Toggle state of choosen radio button - receive message indicator
***************************************************************************************************/
VOID vToggleMessageIndidator(UINT uiRadioButtonId)
{
  if(BST_CHECKED == IsDlgButtonChecked(g_hwnd, uiRadioButtonId))
    {
      CheckRadioButton (g_hwnd, uiRadioButtonId, uiRadioButtonId, uiRadioButtonId+100);
    }
    else
    {
      CheckRadioButton (g_hwnd, uiRadioButtonId, uiRadioButtonId, uiRadioButtonId);
    }
}



/***************************************************************************************************
* brief      : Set checked sytate for choosen radio button - receive message indicator
***************************************************************************************************/
VOID vSetMessageIndidator(UINT uiRadioButtonId)
{
  CheckRadioButton (g_hwnd, uiRadioButtonId, uiRadioButtonId, uiRadioButtonId);
}


/***************************************************************************************************
* brief      : Set unchecked sytate for choosen radio button - receive message indicator
***************************************************************************************************/
VOID vClearMessageIndidator(UINT uiRadioButtonId)
{
  CheckRadioButton (g_hwnd, uiRadioButtonId, uiRadioButtonId, uiRadioButtonId+100);
}


/***************************************************************************************************
* brief      : Ignore click from radiobuttons
***************************************************************************************************/
VOID vIgnoreClick()
{
  if(FALSE == g_stConnectionState.bZigBeeConnected)
  {
    SendMessage(g_hConnectedIndDlg, BM_SETCHECK, BST_UNCHECKED, 0);
  }
  else
  {
    SendMessage(g_hConnectedIndDlg, BM_SETCHECK, BST_CHECKED, 0);
  }

  if(FALSE == g_stConnectionState.bZigBeeReceiving)
  {
    SendMessage(g_hReceiveIndDlg,   BM_SETCHECK, BST_UNCHECKED, 0);
  }
  else
  {
    SendMessage(g_hReceiveIndDlg,   BM_SETCHECK, BST_CHECKED, 0);
  }
}

// EOF
