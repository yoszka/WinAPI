// main.cpp : Defines the entry point for the application and window procedures
// autor:  Tomasz Jokiel

// *********** Includes ****************************************************************************

#include "stdafx.h"
#include "main.h"
#include <windows.h>
#include <Windowsx.h>                                                                               // for MACRO ComboBox_SetCurSel(hWndCB, index);
#include <iostream>
#include <process.h>                                                                                //  _beginthread
#include <string>
#include <commctrl.h>                                                                               // for progressbar
#include "content.h"
#include "threads.h"
#include "tools.h"
#include "serial_port.h"

// *********** Definitions and Enums ***************************************************************
#define CHECCK_PORT_ON_START            FALSE
#define DEFAULT_REFERENCE_VOLTAGE_AC1   (3)
#define DEFAULT_REFERENCE_VOLTAGE_AC2   (3)
#define SMALL_BUFFER_SIZE				(7)


// *********** Typedefs ****************************************************************************
// *********** Function prototypes *****************************************************************

LRESULT CALLBACK WindowProcedure   (HWND, UINT, WPARAM, LPARAM);                                    // Declare Windows procedure
LRESULT CALLBACK EditProc          (HWND, UINT, WPARAM, LPARAM);                                    // For editbox "Konfiguruj" reference voltage
LRESULT CALLBACK EditProcMulti     (HWND, UINT, WPARAM, LPARAM);                                    // For editbox "Konfiguruj" multiplier
BOOL    CALLBACK DlgEditProc       (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL    CALLBACK DlgConfigureProc  (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

// *********** Global variables definition *********************************************************

BOOL                g_bSendSerial = FALSE;
UINT                g_SliderPos   = 0;                                                              // slider position
HWND                g_hComboPort;
HANDLE              g_hSerial;                                                                      // serial port handle
HWND                g_hWndMessageWindow;
HWND                g_hEditSliderValue;
HWND				g_hSliderDlg;
WNDPROC             g_wndProcEdit;

EN_COM_PORT_STATE g_eComPortState = COM_DISCONNECTED;


// *********** Declaration of external global variables ********************************************
// *********** Implememntation *********************************************************************

using namespace std;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
	MSG         messages;                                                                              // Here messages to the application are saved

         
  // Start thread for serial port communications
  HANDLE hThread = (HANDLE) _beginthread (ThreadProc, 0, NULL);

  // Run main dialog													           NULL - insead hwnd to show taskbar application icon
  HWND hDialog = CreateDialog(hThisInstance, MAKEINTRESOURCE(IDD_PROPPAGE_LARGE1), NULL, DlgEditProc);
  ShowWindow (hDialog, SW_SHOW);

  // Run the message loop. It will run until GetMessage() returns 0
  while (GetMessage (&messages, NULL, 0, 0))
  {
    // Translate virtual-key messages into character messages
    TranslateMessage(&messages);
    // Send message to WindowProcedure
    DispatchMessage(&messages);
  }

  // The program return-value is 0 - The value that PostQuitMessage() gave
  return messages.wParam;
}


/***************************************************************************************************
 * \brief Procedure for dialog window
 **************************************************************************************************/
BOOL CALLBACK DlgEditProc (HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  CHAR    cBuffer[SMALL_BUFFER_SIZE];                                                                  // local text buffer
  HWND    hWndComboBox        = GetDlgItem(hWndDlg, IDC_COMBO1);                                       // Get dialog item - combo box
  HWND    hWndConnectButton   = GetDlgItem(hWndDlg, IDC_BUTTON4);                                      // Get dialog item - button "Po³¹cz"
  HWND    hWndMessageWindow   = GetDlgItem(hWndDlg, IDC_EDIT4);                                        // Get dialog item - message window
  HWND    hEditSliderValue    = GetDlgItem(hWndDlg, IDC_EDIT1);                                        // Get dialog item - Slider value 
  HWND    hSliderTempDlg      = GetDlgItem(hWndDlg, IDC_SLIDER1);                                      // Get dialog item - slider

  

 switch (Msg)
 {
  case WM_INITDIALOG:
    g_hWndMessageWindow = hWndMessageWindow;
    g_hEditSliderValue	= hEditSliderValue;
	g_hSliderDlg		= hSliderTempDlg;

    for(int i = 1; i < 26; i++)
    {      
      sprintf_s(cBuffer, SMALL_BUFFER_SIZE, "COM %i", i);
      SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)cBuffer);
    }
    
    // Choose default value
    SendMessage(hWndComboBox, CB_SETCURSEL, 0, (LPARAM)0);

	// Application icon
    SendMessage(hWndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_EXCLAMATION)));
    SendMessage(hWndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_EXCLAMATION)));

	// Slidebar
	SendMessage(hSliderTempDlg, TBM_SETRANGE, true, MAKELONG(1,20));
	//SendMessage(hSliderTempDlg, TBM_SETPOS,true,(LPARAM)10);
	//int pozycja = (int)SendMessage(hSliderTempDlg,TBM_GETPOS, 0, 0);
	sprintf_s(cBuffer, SMALL_BUFFER_SIZE, "%d", (int)SendMessage(hSliderTempDlg,TBM_GETPOS, 0, 0));
	SetWindowText (g_hEditSliderValue, cBuffer);

    break;
  
  // ---------------------------------------------------------
  case WM_COMMAND:
  // ---------------------------------------------------------
    switch (LOWORD(wParam))
    {

      // ---------------------------*
      case IDC_BUTTON4:                                                                             // "Po³¹cz"
      // ---------------------------*
        {
          UINT uiPortNbr = ComboBox_GetCurSel(hWndComboBox);
          uiPortNbr++;                                                                              // values are counting from "0"
          sprintf_s(cBuffer, SMALL_BUFFER_SIZE, "COM%i", uiPortNbr);

          if(COM_DISCONNECTED == g_eComPortState)
          {
            if(!bOpenSerialPort(cBuffer))
            {
              g_eComPortState = COM_DISCONNECTED;

              string strInfoMessage = "Nie mozna otworzyc portu ";
              strInfoMessage += cBuffer;
              MessageBox (NULL, strInfoMessage.c_str(), "B³¹d", MB_ICONERROR | MB_OK);
            }
            else
            {
              SetWindowText( hWndConnectButton, "Roz³¹cz");
              g_eComPortState = COM_CONNECTED;
              EnableWindow(hWndComboBox, FALSE);
            }
          }
          else
          {
            CloseHandle(g_hSerial);                                                                 // close COM port
            g_hSerial		= NULL;
            g_eComPortState = COM_DISCONNECTED;

            SetWindowText (hWndMessageWindow, "");
            SetWindowText( hWndConnectButton, "Po³¹cz");
            EnableWindow(hWndComboBox, TRUE);
          }
        }
        break;

      // ---------------------------*
      default:
      // ---------------------------*
        return FALSE;
    }
    break;

	case WM_HSCROLL :							// Scrool bar events

		switch(LOWORD(wParam)){
		case SB_THUMBPOSITION:					// On slider push
			g_bSendSerial = TRUE;
		case SB_THUMBTRACK:						// On slider change value
			g_SliderPos = 21 - HIWORD(wParam);
			sprintf_s(cBuffer, SMALL_BUFFER_SIZE, "%d", g_SliderPos);
			SetWindowText (g_hEditSliderValue, cBuffer);
			break;
		}

      break;

  // ---------------------------------------------------------
  case WM_CLOSE:
  // ---------------------------------------------------------
	//ExitProcess(0);
    CloseHandle   (g_hSerial);                                                                    // close COM port
    DestroyWindow (hWndDlg);
	PostQuitMessage (0);
    break;

  // ---------------------------------------------------------
  default: 
  // ---------------------------------------------------------
	  return FALSE;
 }
 return TRUE;
}

// EOF
