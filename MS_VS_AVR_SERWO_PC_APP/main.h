#pragma once

// ************** Includes *************************************************************************

#include <windows.h>
#include "resource.h"

// ************** Definitions **********************************************************************

#define WM_SERVO_STATE (WM_USER + 0x0001)

// *********** Typedefs ****************************************************************************

// Define serial port state
typedef enum
{
  COM_DISCONNECTED,
  COM_CONNECTED 

}EN_COM_PORT_STATE;

// Define servo state
typedef enum
{
  SERVO_DISCONNECTED,
  SERVO_CONNECTED 

}EN_SERVO_STATE;

// Define send serial port data type
typedef enum
{
	SERVO_CONNECT,
	SERVO_MOVE

}EN_COM_PORT_SEND_DATA_TYPE;

// Define send serial port control command
typedef struct
{
	BOOL						bSerialDataPending;		// Flag to indicate send request to serial port
	EN_COM_PORT_SEND_DATA_TYPE	eCommandType;			// To define what to send

}ST_SERIAL_SEND_COMMAND;

// *********** Declaration of external global variables ********************************************
//extern BOOL				   g_bSendSerial;															// Flag to indicate send request to serial port
extern ST_SERIAL_SEND_COMMAND	g_stSerialSendCmd;
extern UINT						g_SliderPos;															// Global slider position
extern HWND						g_hComboPort;
extern HANDLE					g_hSerial;                                                              // serial port handle
extern HWND						g_hWndMessageWindow;
extern HWND						g_hEditSliderValue;
extern EN_COM_PORT_STATE		g_eComPortState;
extern EN_SERVO_STATE			g_eServoState;
extern HWND						g_hMainWindow;

// EOF
