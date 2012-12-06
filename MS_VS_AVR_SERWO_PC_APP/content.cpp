// content.cpp : Defines the content ingredients
// autor:  Tomasz Jokiel

// *********** Includes ****************************************************************************

#include "stdafx.h"
#include "main.h"
#include "content.h"
#include <windows.h>
#include <commctrl.h>                                                                               // for progress bar
#include <string>

// *********** Definitions and Enums ***************************************************************
// *********** Function prototypes *****************************************************************
// *********** Global variables definition *********************************************************
// *********** Declaration of external global variables ********************************************
// *********** Typedefs ****************************************************************************
// *********** Implememntation *********************************************************************



/***************************************************************************************************
* brief      : This function is called by the Windows function DispatchMessage()
* side effect:
***************************************************************************************************/
BOOL bInitializeContent(HINSTANCE hMainProgramInstance, HWND hWnd)
{

    g_hButtonExit = CreateWindowEx (
          0,
          "BUTTON",
          "Exit",                                                                                   // Text on button
           WS_CHILD | WS_VISIBLE,                                                                   // Properties
          5,                                                                                        // Position x
          70,                                                                                       // Position y
          100,                                                                                      // Size x
          20,                                                                                       // Size y
          hWnd,                                                                                     // Handle of main window
          (HMENU) ID_BUTTON_2,                                                                      // ID of window (button)
          hMainProgramInstance,                                                                     // Main program instance
          NULL);

    g_hTextWnd = CreateWindowEx (
          WS_EX_CLIENTEDGE,
          "EDIT",
          NULL,                                                                                     // Text on button
           WS_CHILD | WS_VISIBLE | ES_MULTILINE,                                                                   // Properties
          5,                                                                                      // Position x
          100,                                                                                      // Position y
          685,                                                                                      // Size x
          100,                                                                                      // Size y
          hWnd,                                                                                     // Handle of main window
          (HMENU) ID_EDIT_1,                                                                        // ID of window (edit text)
          hMainProgramInstance,                                                                     // Main program instance
          NULL);
          
  // Initialize buttons
  g_hReceiveIndicator = CreateWindowEx (
          0,
          "BUTTON",
          "Odbior",
          WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
          11,
          25,
          90,
          40,
          hWnd,
          (HMENU)ID_RADIOBUTTON_1,
          hMainProgramInstance,
          NULL);


  // Initialize buttons
  g_hConnectIndicator = CreateWindowEx (
          0,
          "BUTTON",
          "Po³¹czono",
          WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
          11,
          19,
          90,
          10,
          hWnd,
          (HMENU)ID_RADIOBUTTON_2,
          hMainProgramInstance,
          NULL);
          
  // Combo box - wybór portu
  g_hComboPort = CreateWindowEx (
          WS_EX_CLIENTEDGE,
          "COMBOBOX",
          NULL,
          WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
          600,
          20,
          80,
          450,
          hWnd,
          (HMENU)NULL,
          hMainProgramInstance,
          NULL);

  for(int i = 1; i < 26; i++)
  {
    CHAR cBuffer[7];
    sprintf(cBuffer, "COM %i", i);

    SendMessage(g_hComboPort, CB_ADDSTRING, 0, (LPARAM)cBuffer);
  }
      
          
// ---------------------------------------------------------------------------------------
  // Progress bar
  INITCOMMONCONTROLSEX cc;                                                                            //ini struct
  cc.dwSize =sizeof(INITCOMMONCONTROLSEX);
  cc.dwICC  =ICC_BAR_CLASSES;
  InitCommonControlsEx(&cc);                                                                          // initialize

  g_hProgressBar = CreateWindowEx(
            0,
            PROGRESS_CLASS,
            0,
            WS_VISIBLE|WS_CHILD,
            100, 10,
            400,10,
            hWnd,
            0,
            hMainProgramInstance,
            0);

  SendMessage(g_hProgressBar, PBM_SETPOS,     (WPARAM)0,  0);                                              // set start position for progres bar
  SendMessage(g_hProgressBar, PBM_SETSTEP,    (WPARAM)1,  0);                                            // define step for progress bar
  SendMessage(g_hProgressBar, PBM_SETRANGE,   0,         (LPARAM)MAKELONG(0,255));
  SendMessage(g_hProgressBar, PBM_SETBARCOLOR,0,         (LPARAM)RGB(0, 255, 0));                            // Set bar color for progress bar
  SendMessage(g_hProgressBar, PBM_SETBKCOLOR, 0,         (LPARAM)RGB(255, 255, 255));                        // Set background color for progress bar
  
// ---------------------------------------------------------------------------------------
  // Progress bar 2
  g_hProgressBar2 = CreateWindowEx(
            0,
            PROGRESS_CLASS,
            0,
            WS_VISIBLE|WS_CHILD,
            100, 30,
            400,10,
            hWnd,
            0,
            hMainProgramInstance,
            0);

  SendMessage(g_hProgressBar2, PBM_SETPOS,     (WPARAM)0,  0);                                              // set start position for progres bar
  SendMessage(g_hProgressBar2, PBM_SETSTEP,    (WPARAM)1,  0);                                            // define step for progress bar
  SendMessage(g_hProgressBar2, PBM_SETRANGE,   0,         (LPARAM)MAKELONG(0,255));
  SendMessage(g_hProgressBar2, PBM_SETBARCOLOR,0,         (LPARAM)RGB(0, 255, 0));                            // Set bar color for progress bar
  SendMessage(g_hProgressBar2, PBM_SETBKCOLOR, 0,         (LPARAM)RGB(255, 255, 255));                        // Set background color for progress bar
  
  return TRUE;
}

// EOF
