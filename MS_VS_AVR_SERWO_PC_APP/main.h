#pragma once

// ************** Includes *************************************************************************

#include <windows.h>
#include "resource.h"

// ************** Definitions **********************************************************************
// *********** Typedefs ****************************************************************************

// Define serial port state
typedef enum
{
  COM_DISCONNECTED,
  COM_CONNECTED 

}EN_COM_PORT_STATE;

// *********** Declaration of external global variables ********************************************
extern BOOL				   g_bSendSerial;															// Flag to indicate send request to serial port
extern UINT                g_SliderPos;																// Global slider position
extern HWND                g_hComboPort;
extern HANDLE              g_hSerial;                                                               // serial port handle
extern HWND                g_hWndMessageWindow;
extern HWND                g_hEditSliderValue;
extern EN_COM_PORT_STATE   g_eComPortState;

// EOF
