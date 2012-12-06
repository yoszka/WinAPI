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

//#define WINDOW_WIDTH                    (550)
//#define WINDOW_HEIGTH                   (200)
#define CHECCK_PORT_ON_START            FALSE
#define DEFAULT_REFERENCE_VOLTAGE_AC1   (3)
#define DEFAULT_REFERENCE_VOLTAGE_AC2   (3)


// *********** Typedefs ****************************************************************************




// Define serial port state
typedef enum
{
  KONFIGURUJ_AC1,
  KONFIGURUJ_AC2

}EN_WINDOW_TYPE;

// *********** Function prototypes *****************************************************************

LRESULT CALLBACK WindowProcedure   (HWND, UINT, WPARAM, LPARAM);                                    // Declare Windows procedure
LRESULT CALLBACK EditProc          (HWND, UINT, WPARAM, LPARAM);                                    // For editbox "Konfiguruj" reference voltage
LRESULT CALLBACK EditProcMulti     (HWND, UINT, WPARAM, LPARAM);                                    // For editbox "Konfiguruj" multiplier
BOOL    CALLBACK DlgEditProc       (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL    CALLBACK DlgConfigureProc  (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static BOOL bRunEditWindow         (EN_WINDOW_TYPE eWindowType);

// *********** Global variables definition *********************************************************
CHAR                szClassName[ ] = "WindowsApp";                                                  // Make the class name into a global variable
FLOAT               g_fAC1ReffVoltage    = DEFAULT_REFERENCE_VOLTAGE_AC1;
FLOAT               g_fAC2ReffVoltage    = DEFAULT_REFERENCE_VOLTAGE_AC2;
FLOAT               g_fACReffVoltageTemp = 1;
BOOL                g_bMultiplyAC2 = FALSE;
BOOL                g_bSendSerial = FALSE;

CHAR                g_buffer [1024];                                                                // global purpose buffer for string conversions
UINT                g_Counter     = 0;                                                              // global purpose counter
UINT                g_SliderPos   = 0;                                                              // slider position
HWND                g_hButtonExit;                                                                  // should be global becouse we could need acces to it from other spaces
HWND                g_hTextWnd;
HWND                g_hReceiveIndicator;
HWND                g_hConnectIndicator;
HWND                g_hComboPort;
HWND                g_hProgressBar;
HWND                g_hProgressBar2;
HWND                g_hwnd;                                                                         // main window handle
HWND                g_hwndConfig;                                                                   // config window handle - Edit
HANDLE              g_hSerial;                                                                      // serial port handle
HWND                g_hProgressBarAC1;
HWND                g_hProgressBarAC2;
HWND                g_hWndMessageWindow;
HWND                g_hReceiveIndDlg;
HWND                g_hConnectedIndDlg;
HWND                g_hEditAC1Dlg;
HWND                g_hEditAC2Dlg;
HWND                g_hEditTempDlg;
HWND				g_hSliderTempDlg;
WNDPROC             g_wndProcEdit;
WNDPROC             g_wndProcChbox;

ST_CONNECTION_STATE g_stConnectionState = {FALSE, FALSE};
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
  //g_hwnd = hDialog;

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
  //static EN_COM_PORT_STATE g_eComPortState = COM_DISCONNECTED;
  CHAR    cBuffer[7];                                                                                  // local text buffer
  HWND    hWndComboBox        = GetDlgItem(hWndDlg, IDC_COMBO1);                                       // Get dialog item - combo box
  HWND    hWndConnectButton   = GetDlgItem(hWndDlg, IDC_BUTTON4);                                      // Get dialog item - button "Po³¹cz"
  HWND    hWndProgressBarAC1  = GetDlgItem(hWndDlg, IDC_PROGRESS1);                                    // Get dialog item - progress bar AC1
  HWND    hWndProgressBarAC2  = GetDlgItem(hWndDlg, IDC_PROGRESS2);                                    // Get dialog item - progress bar AC2
  HWND    hWndMessageWindow   = GetDlgItem(hWndDlg, IDC_EDIT4);                                        // Get dialog item - message window
  HWND    hReceiveIndDlg      = GetDlgItem(hWndDlg, IDC_RADIO2);                                       // Get dialog item - receive indicator radio button
  HWND    hConnectedIndDlg    = GetDlgItem(hWndDlg, IDC_RADIO1);                                       // Get dialog item - PC connected indicator radio button
  HWND    hEditAC1Dlg         = GetDlgItem(hWndDlg, IDC_EDIT1);                                        // Get dialog item - AC 1 value 
  HWND    hEditAC2Dlg         = GetDlgItem(hWndDlg, IDC_EDIT2);                                        // Get dialog item - AC 2 value
  HWND    hEditTempDlg        = GetDlgItem(hWndDlg, IDC_EDIT3);                                        // Get dialog item - temp value
  HWND    hSliderTempDlg      = GetDlgItem(hWndDlg, IDC_SLIDER1);                                      // Get dialog item - slider

  

 switch (Msg)
 {
  case WM_INITDIALOG:
    g_hProgressBarAC1   = hWndProgressBarAC1;
    g_hProgressBarAC2   = hWndProgressBarAC2;
    g_hWndMessageWindow = hWndMessageWindow;
    g_hReceiveIndDlg    = hReceiveIndDlg;
    g_hConnectedIndDlg  = hConnectedIndDlg;
    g_hEditAC1Dlg       = hEditAC1Dlg;
    g_hEditAC2Dlg       = hEditAC2Dlg;
    g_hEditTempDlg      = hEditTempDlg;
	g_hSliderTempDlg    = hSliderTempDlg;

    for(int i = 1; i < 26; i++)
    {      
      sprintf(cBuffer, "COM %i", i);
      SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)cBuffer);
    }
    
    // Choose default value
    SendMessage(hWndComboBox, CB_SETCURSEL, 0, (LPARAM)0);

    // Configure progressBars
    // AC1
    SendMessage(hWndProgressBarAC1, PBM_SETPOS,     (WPARAM)0,  0);                                 // set start position for progres bar
    SendMessage(hWndProgressBarAC1, PBM_SETSTEP,    (WPARAM)1,  0);                                 // define step for progress bar
    SendMessage(hWndProgressBarAC1, PBM_SETRANGE,   0,         (LPARAM)MAKELONG(0,MAX_ADC_VALUE));
    SendMessage(hWndProgressBarAC1, PBM_SETBARCOLOR,0,         (LPARAM)RGB(0, 0, 255));             // Set bar color for progress bar
    SendMessage(hWndProgressBarAC1, PBM_SETBKCOLOR, 0,         (LPARAM)RGB(255, 255, 255));         // Set background color for progress bar

    // AC2
    SendMessage(hWndProgressBarAC2, PBM_SETPOS,     (WPARAM)0,  0);                                 // set start position for progres bar
    SendMessage(hWndProgressBarAC2, PBM_SETSTEP,    (WPARAM)1,  0);                                 // define step for progress bar
    SendMessage(hWndProgressBarAC2, PBM_SETRANGE,   0,         (LPARAM)MAKELONG(0,MAX_ADC_VALUE));
    SendMessage(hWndProgressBarAC2, PBM_SETBARCOLOR,0,         (LPARAM)RGB(0, 0, 255));             // Set bar color for progress bar
    SendMessage(hWndProgressBarAC2, PBM_SETBKCOLOR, 0,         (LPARAM)RGB(255, 255, 255));         // Set background color for progress bar

	// Application icon
    SendMessage(hWndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_EXCLAMATION)));
    SendMessage(hWndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_EXCLAMATION)));

	// Slidebar
	SendMessage(hSliderTempDlg, TBM_SETRANGE, true, MAKELONG(1,20));
	//SendMessage(hSliderTempDlg, TBM_SETPOS,true,(LPARAM)10);
	//int pozycja = (int)SendMessage(hSliderTempDlg,TBM_GETPOS, 0, 0);
	sprintf(cBuffer, "%d", (int)SendMessage(hSliderTempDlg,TBM_GETPOS, 0, 0));
	SetWindowText (g_hEditAC1Dlg, cBuffer);

    break;
  
  // ---------------------------------------------------------
  case WM_COMMAND:
  // ---------------------------------------------------------
    switch (LOWORD(wParam))
    {
      // ---------------------------*
      case IDC_BUTTON1:                                                                             // "Konfiguruj" AC 1
      // ---------------------------*
         bRunEditWindow (KONFIGURUJ_AC1);
         break;

      // ---------------------------*
      case IDC_BUTTON2:                                                                             // "Konfiguruj" AC 2
      // ---------------------------*
        bRunEditWindow (KONFIGURUJ_AC2);
        break;

      // ---------------------------*
      case IDC_BUTTON4:                                                                             // "Po³¹cz"
      // ---------------------------*
        {
          UINT uiPortNbr = ComboBox_GetCurSel(hWndComboBox);
          uiPortNbr++;                                                                              // values are counting from "0"
          sprintf(cBuffer, "COM%i", uiPortNbr);

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
            g_hSerial     = NULL;
            g_eComPortState = COM_DISCONNECTED;
            SendMessage(g_hConnectedIndDlg,  BM_SETCHECK, BST_UNCHECKED, 0);                        // Turn off connected indicator
            SetWindowText( g_hConnectedIndDlg, "Roz³¹czono");
            SendMessage(hWndProgressBarAC1, PBM_SETPOS, (WPARAM)0, 0);
            SendMessage(hWndProgressBarAC2, PBM_SETPOS, (WPARAM)0, 0);
            SetWindowText (hWndMessageWindow, "");
            KillTimer(g_hwnd, ID_TIMER_2);
            SetWindowText( hWndConnectButton, "Po³¹cz");
            EnableWindow(hWndComboBox, TRUE);
          }
        }
        break;

      // ---------------------------*
      case IDC_RADIO1:
      // ---------------------------*
        vIgnoreClick();
        break;

      // ---------------------------*
      case IDC_RADIO2:
      // ---------------------------*
        vIgnoreClick();
        break;

      // ---------------------------*
      default:
      // ---------------------------*
        return FALSE;
    }
    break;

case WM_HSCROLL :
	

        // Calculate and display AC 1 value in Volts


		switch(LOWORD(wParam)){
		case SB_THUMBPOSITION:
			g_bSendSerial = TRUE;
		case SB_THUMBTRACK:
			g_SliderPos = 21 - HIWORD(wParam);
			sprintf(cBuffer, "%d", g_SliderPos);
			SetWindowText (g_hEditAC1Dlg, cBuffer);
			//g_bSendSerial = TRUE;
			break;
		}

      //switch(wParam)
      //{
      //case TB_THUMBTRACK :
         //dwPos = SendMessage(hWndSlider, TBM_GETPOS, 0, 0);
         //break;
     // }

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
    //return DefWindowProc (hWndDlg, Msg, wParam, lParam);
	  return FALSE;
 }
 return TRUE;
}



/***************************************************************************************************
 * \brief Run config window with parameter
**************************************************************************************************/
static BOOL bRunEditWindow (EN_WINDOW_TYPE eWindowType)
{

  switch(eWindowType)
  {
    // ---------------------------*
    case KONFIGURUJ_AC1:
    // ---------------------------*
      g_fACReffVoltageTemp = g_fAC1ReffVoltage;
      DialogBox (GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPPAGE_LARGE2), g_hwnd, DlgConfigureProc);
      g_fAC1ReffVoltage = g_fACReffVoltageTemp;
      break;

    // ---------------------------*
    case KONFIGURUJ_AC2:
    // ---------------------------*
      g_fACReffVoltageTemp = g_fAC2ReffVoltage;
      DialogBox (GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPPAGE_LARGE), g_hwnd, DlgConfigureProc);
      g_fAC2ReffVoltage = g_fACReffVoltageTemp;
      break;

    // ---------------------------*
    default:
    // ---------------------------*
      return FALSE;
  }

  return TRUE;
}



/***************************************************************************************************
 * \brief Procedure for dialog window
 **************************************************************************************************/
BOOL CALLBACK DlgConfigureProc (HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  INT     iCommaPos          = 0;
  UINT    uiLength           = 0;
  CHAR    acBuffer[7]        = {0};  
  LPSTR   lpstrBuffer        = NULL;
  HWND    hEditRatioACDlg    = GetDlgItem(hWndDlg, IDC_EDIT1);                                      // Get dialog item - refference voltage AC 1
  HWND    hChkboxAC2Dlg      = GetDlgItem(hWndDlg, IDC_CHECKBOX1);                                  // Get dialog item - refference voltage AC 2  
  string  strTmpBuffer;                                                                             // Buffer for temp reference voltage

  static BOOL  bCheckboxState    = FALSE;

 switch (Msg)
 {
  // ---------------------------------------------------------
  case WM_INITDIALOG:
  // ---------------------------------------------------------
    sprintf(g_buffer, "%.2f", g_fACReffVoltageTemp);
    SetWindowText (hEditRatioACDlg, g_buffer);

    // Set checked or unchecked checkbox
    if(g_bMultiplyAC2)
    {
      CheckDlgButton (hWndDlg, IDC_CHECKBOX1, BST_CHECKED); 
    }
    else
    {
      CheckDlgButton (hWndDlg, IDC_CHECKBOX1, BST_UNCHECKED); 
    }

    // Change default window proc to handle key messages
    g_wndProcEdit  = (WNDPROC) SetWindowLong (hEditRatioACDlg, GWL_WNDPROC, (LPARAM) EditProc);      // replace default procedure for hEditRatioACDlg
    g_wndProcChbox = (WNDPROC) SetWindowLong (hChkboxAC2Dlg,   GWL_WNDPROC, (LPARAM) EditProcMulti); // replace default procedure for hChkboxAC2Dlg
                                                                                                     // wndProc will contain old (default Windows) procedure for hEditRatioACDlg
    g_hwndConfig = hWndDlg;
    break;

  // ---------------------------------------------------------
  case WM_COMMAND:
  // ---------------------------------------------------------
    switch (LOWORD(wParam))
    {
      // ---------------------------*
      case IDC_EDIT1: 
      // ---------------------------*
        break; 

      // ---------------------------*
      case IDC_CHECKBOX1:
      // ---------------------------*
        bCheckboxState = (IsDlgButtonChecked (hWndDlg, IDC_CHECKBOX1) == BST_CHECKED);
        break; 

      // ---------------------------*
      case IDC_BUTTON1:                                                                               // OK
      // ---------------------------*
      {
        // read reference voltage
        uiLength = GetWindowTextLength (hEditRatioACDlg);
        lpstrBuffer = (LPSTR) GlobalAlloc (GPTR, uiLength);
        GetWindowText (hEditRatioACDlg, lpstrBuffer, uiLength + 1);
        strTmpBuffer = lpstrBuffer;

        // change comma to dot
        iCommaPos = strTmpBuffer.find(',');

        if(iCommaPos >= 0)
        {
          lpstrBuffer[iCommaPos] = '.';
        }

        g_fACReffVoltageTemp = atof(lpstrBuffer);

        // Set multiplyer or not
        g_bMultiplyAC2 = bCheckboxState;
        EndDialog (hWndDlg, FALSE);
      }
      break; 

      // ---------------------------*
      default:
      // ---------------------------*
        return FALSE;
    }
    break;

  // ---------------------------------------------------------
  case WM_CLOSE:
  // ---------------------------------------------------------
    EndDialog (hWndDlg, FALSE);
    break;

  // ---------------------------------------------------------
  default: 
  // ---------------------------------------------------------
    return FALSE;
 }
 return TRUE;
}



/***************************************************************************************************
 * \brief Procedure for Edit window 
 **************************************************************************************************/
LRESULT CALLBACK EditProc (HWND hWndEdit, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
  case WM_KEYUP:
    {
     switch ( (int) wParam )
     {
      case VK_RETURN:
      SendMessage(g_hwndConfig, WM_COMMAND,(WPARAM)IDC_BUTTON1,0);                                      // send message for close with confirm data - like "OK"
      break;
 
      case VK_ESCAPE:
      SendMessage(g_hwndConfig, WM_CLOSE,(WPARAM)0,0);                                                  // send message for close
      break;
     }
    }
    break;
  }

  return CallWindowProc (g_wndProcEdit, hWndEdit, msg, wParam, lParam);
}



/***************************************************************************************************
 * \brief Procedure for Checkbox
 **************************************************************************************************/
LRESULT CALLBACK EditProcMulti (HWND hWndEdit, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
  case WM_KEYUP:
    {
     switch ( (int) wParam )
     {
      case VK_RETURN:
      SendMessage(g_hwndConfig, WM_COMMAND,(WPARAM)IDC_BUTTON1,0);                                      // send message for close with confirm data - like "OK"
      break;
 
      case VK_ESCAPE:
      SendMessage(g_hwndConfig, WM_CLOSE,(WPARAM)0,0);                                                  // send message for close
      break;
     }
    }
    break;
  }

  return CallWindowProc (g_wndProcChbox, hWndEdit, msg, wParam, lParam);
}

// EOF
