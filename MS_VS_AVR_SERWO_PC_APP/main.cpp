// main.cpp : Defines the entry point for the application and window procedures
// autor:  Tomasz Jokiel

// *********** Includes ****************************************************************************

#include "stdafx.h"
#include "enumser.h"
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
#define BIG_BUFFER_SIZE					(100)


// *********** Typedefs ****************************************************************************
// *********** Function prototypes *****************************************************************

LRESULT CALLBACK WindowProcedure   (HWND, UINT, WPARAM, LPARAM);                                    // Declare Windows procedure
LRESULT CALLBACK EditProc          (HWND, UINT, WPARAM, LPARAM);                                    // For editbox "Konfiguruj" reference voltage
LRESULT CALLBACK EditProcMulti     (HWND, UINT, WPARAM, LPARAM);                                    // For editbox "Konfiguruj" multiplier
BOOL    CALLBACK DlgEditProc       (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL    CALLBACK DlgConfigureProc  (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

// *********** Global variables definition *********************************************************

//BOOL					g_bSendSerial = FALSE;
ST_SERIAL_SEND_COMMAND	g_stSerialSendCmd = {FALSE};
UINT					g_SliderPos   = 0;                                                              // slider position
HWND					g_hComboPort;
HANDLE					g_hSerial;                                                                      // serial port handle
HWND					g_hWndMessageWindow;
HWND					g_hEditSliderValue;
HWND					g_hSliderDlg;
WNDPROC					g_wndProcEdit;
HWND					g_hMainWindow;

EN_COM_PORT_STATE		g_eComPortState = COM_DISCONNECTED;
EN_SERVO_STATE			g_eServoState	= SERVO_DISCONNECTED;


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
  g_hMainWindow = hDialog;

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
  static UINT uiPorts[25];
  WCHAR   wcBuffer[SMALL_BUFFER_SIZE];                                                                  // local text buffer
  HWND    hWndComboBox        = GetDlgItem(hWndDlg, IDC_COMBO1);                                       // Get dialog item - combo box
  HWND    hWndConnectButton   = GetDlgItem(hWndDlg, IDC_BUTTON4);                                      // Get dialog item - button "Po³¹cz"
  HWND    hWndMessageWindow   = GetDlgItem(hWndDlg, IDC_EDIT4);                                        // Get dialog item - message window
  HWND    hEditSliderValue    = GetDlgItem(hWndDlg, IDC_EDIT1);                                        // Get dialog item - Slider value 
  HWND    hSliderTempDlg      = GetDlgItem(hWndDlg, IDC_SLIDER1);                                      // Get dialog item - slider
  HWND    hServoStateTempDlg  = GetDlgItem(hWndDlg, IDC_RADIO1);                                       // Get dialog item - slider

  

 switch (Msg)
 {
  case WM_INITDIALOG:
    g_hWndMessageWindow = hWndMessageWindow;
    g_hEditSliderValue	= hEditSliderValue;
	g_hSliderDlg		= hSliderTempDlg;
	
	/*
    for(int i = 1; i < 26; i++)
    {      
      sprintf_s(wcBuffer, SMALL_BUFFER_SIZE, "COM %i", i);
      SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)wcBuffer);
    }
	*/

	{
		#if defined CENUMERATESERIAL_USE_STL
		  std::vector<UINT> ports;
		#if defined _UNICODE
		  std::vector<std::wstring> friendlyNames;
		  std::vector<std::wstring> sPorts;
		#else
		  std::vector<std::string> friendlyNames;
		  std::vector<std::string> sPorts;
		#endif  
		  size_t i;
		#elif defined _AFX
		  CUIntArray ports;
		  CStringArray friendlyNames;
		  CStringArray sPorts;
		  INT_PTR i;
		  WCHAR wcBigBuffer[BIG_BUFFER_SIZE];
		#else
		  CSimpleArray<UINT> ports;
		  CSimpleArray<CString> friendlyNames;
		  CSimpleArray<CString> sPorts;
		  int i;
		#endif

	  _tprintf(_T("Device Manager (SetupAPI - GUID_DEVINTERFACE_COMPORT) reports\n"));
	  if (CEnumerateSerial::UsingSetupAPI1(ports, friendlyNames))
	  {
	  #ifdef CENUMERATESERIAL_USE_STL
		for (i=0; i<ports.size(); i++)
		  _tprintf(_T("COM%u <%s>\n"), ports[i], friendlyNames[i].c_str());
	  #else
		for (i=0; i<ports.GetSize(); i++){
			wsprintf(wcBigBuffer, _T("COM%u <%s>"), ports[i], friendlyNames[i].operator LPCTSTR());
			SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)wcBigBuffer);
		}
		// STore port number values		
		for (i=0; i<ports.GetSize(); i++){			
			uiPorts[i] = ports[i];
		}
		  //_tprintf(_T("COM%u <%s>\n"), ports[i], friendlyNames[i].operator LPCTSTR());
	  #endif
	  }
	  else
		_tprintf(_T("CEnumerateSerial::UsingSetupAPI1 failed, Error:%u\n"), GetLastError());
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
	wsprintf(wcBuffer, L"%d", (int)SendMessage(hSliderTempDlg,TBM_GETPOS, 0, 0));
	//sprintf_s(wcBuffer, SMALL_BUFFER_SIZE, "%d", (int)SendMessage(hSliderTempDlg,TBM_GETPOS, 0, 0));
	SetWindowText (g_hEditSliderValue, wcBuffer);

	// Get manes of serial ports
        TCHAR targetPath[5000];
 /*
        QueryDosDevice(NULL,targetPath,5000);
 
        for(int i=0; i<5000;i++){
			if (targetPath[i]=='\0'){
				targetPath[i]='\n';
			}
		}

			targetPath[4999] = '\0';
 */
	     //SetWindowText (g_hWndMessageWindow, targetPath);
        //MessageBox(hWndDlg,targetPath,"Info",NULL);

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
			//CHAR cBuffer[SMALL_BUFFER_SIZE] = {0};
			UINT uiSelected = ComboBox_GetCurSel(hWndComboBox);
//			uiSelected++;                                                                              // values are counting from "0"
//			wsprintf(wcBuffer, L"COM%i", uiSelected);
			wsprintf(wcBuffer, L"COM%i", uiPorts[uiSelected]);											// Use values readed from system
			
		  

			if(COM_DISCONNECTED == g_eComPortState)
			{
			if(!bOpenSerialPort(wcBuffer))
			{
				WCHAR wcBigBuffer[BIG_BUFFER_SIZE];
				g_eComPortState = COM_DISCONNECTED;

				wstring strInfoMessage = L"Nie mozna otworzyc portu ";
				strInfoMessage += wcBuffer;

				MessageBox (NULL, strInfoMessage.c_str(), L"B³¹d", MB_ICONERROR | MB_OK);
			}
			else
			{
				SetWindowText( hWndConnectButton, L"Roz³¹cz");
				g_eComPortState = COM_CONNECTED;
				EnableWindow(hWndComboBox, FALSE);

				g_stSerialSendCmd.bSerialDataPending	= TRUE;
				g_stSerialSendCmd.eCommandType			= SERVO_CONNECT;
			}
			}
			else
			{
			CloseHandle(g_hSerial);                                                                 // close COM port
			g_hSerial		= NULL;
			g_eComPortState = COM_DISCONNECTED;

			SetWindowText (hWndMessageWindow, L"");
			SetWindowText( hWndConnectButton, L"Po³¹cz");
			EnableWindow(hWndComboBox, TRUE);

			// Servo indicators
			EnableWindow(hSliderTempDlg, FALSE);
			vClearServoStateIndidator(IDC_RADIO1);
			SetWindowText (hServoStateTempDlg, L"Servo not connected");			
			}
        }
        break;

      // ---------------------------*
      default:
      // ---------------------------*
        return FALSE;
    }
    break;

	case WM_HSCROLL :												// Scrool bar events

		switch(LOWORD(wParam)){
		case SB_THUMBPOSITION:										// On slider push
			g_stSerialSendCmd.bSerialDataPending	= TRUE;
			g_stSerialSendCmd.eCommandType			= SERVO_MOVE;
		case SB_THUMBTRACK:											// On slider change value
			g_SliderPos = HIWORD(wParam);							// Get slider value
			wsprintf(wcBuffer, L"%d", g_SliderPos);
			SetWindowText (g_hEditSliderValue, wcBuffer);
			break;
		}

      break;

	case WM_SERVO_STATE:
		EnableWindow(hSliderTempDlg, TRUE);
		vSetServoStateIndidator(IDC_RADIO1);
		SetWindowText (hServoStateTempDlg, L"Servo Connected");
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
