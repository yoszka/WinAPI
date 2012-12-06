#pragma once

// ************** Includes *************************************************************************

#include <windows.h>
#include "resource.h"

// ************** Definitions **********************************************************************
                                                                                                    
#define ID_TIMER_1                (0x11)                                                            // receiving status
#define ID_TIMER_2                (0x12)                                                            // lost connection timer
#define MAX_ADC_VALUE             (255)
#define RESISTOR_DIVIDER_RATIO    (5)

// *********** Typedefs ****************************************************************************

// Connection state
typedef struct
{
  BOOL bZigBeeConnected;
  BOOL bZigBeeReceiving;
} ST_CONNECTION_STATE;


// Define serial port state
typedef enum
{
  COM_DISCONNECTED,
  COM_CONNECTED 

}EN_COM_PORT_STATE;

// *********** Declaration of external global variables ********************************************

extern FLOAT               g_fAC1ReffVoltage;                                                       // refference voltage for ADC 1
extern FLOAT               g_fAC2ReffVoltage;                                                       // refference voltage for ADC 2
extern FLOAT               g_fACReffVoltageTemp;                                                    // Buffer for temporary value
extern BOOL                g_bMultiplyAC2;                                                          // flag indicate if myltiplyer is used
extern BOOL				   g_bSendSerial;															// Flag to indicate send request to serial port
extern CHAR                g_buffer [1024];                                                         // global purpose buffer for string conversions
extern UINT                g_Counter;                                                               // global purpose counter
extern UINT                g_SliderPos;																// Global slider position
extern HWND                g_hButtonExit;                                                           // 
extern HWND                g_hTextWnd;
extern HWND                g_hReceiveIndicator;
extern HWND                g_hConnectIndicator;
extern HWND                g_hComboPort;
extern HWND                g_hProgressBar;
extern HWND                g_hProgressBar2;
extern HWND                g_hwnd;                                                                  // window handle
extern HANDLE              g_hSerial;                                                               // serial port handle
extern HWND                g_hProgressBarAC1;
extern HWND                g_hProgressBarAC2;
extern HWND                g_hWndMessageWindow;
extern HWND                g_hReceiveIndDlg;
extern HWND                g_hConnectedIndDlg;
extern HWND                g_hEditAC1Dlg;
extern HWND                g_hEditAC2Dlg;
extern HWND                g_hEditTempDlg;
extern ST_CONNECTION_STATE g_stConnectionState;
extern EN_COM_PORT_STATE   g_eComPortState;

// EOF
