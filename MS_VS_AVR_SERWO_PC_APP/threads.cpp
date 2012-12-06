// threads.cpp : Defines the additional threads used in application
// autor:  Tomasz Jokiel

// *********** Includes ****************************************************************************

#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>                                                                               // for progress bar
#include <process.h>                                                                                // wymagane ze wzglêdu na _beginthread
#include <string>
#include "tools.h"
#include "threads.h"
#include "main.h"

// *********** Definitions and Enums ***************************************************************

#define STOP_AT_INTER_CONN_ERR    FALSE                                                             // stop application when internal error occur

// *********** Function prototypes *****************************************************************
// *********** Global variables definition *********************************************************
// *********** Declaration of external global variables ********************************************
// *********** Typedefs ****************************************************************************
// *********** Implememntation *********************************************************************

using namespace std;

/***************************************************************************************************
* brief      : Procedura watku portu szeregowego
* side effect:
***************************************************************************************************/
void __cdecl ThreadProc (void* Args)
{
  BYTE Byte = 0, LastByte = 0, preLastByte = 0;
  BYTE ByteArr[50] = {0};
  DWORD dwBytesTransferred, dwFrameCounter = 0;

  
  while(true)
  {
    ReadFile (g_hSerial,            // Port handle
              &Byte,                // Pointer to data to read
              1,                    // Number of bytes to read
              &dwBytesTransferred,  // Pointer to number of bytes
                                    // read
              NULL                  // Must be NULL for Windows CE
             );
    

    if((LastByte == 0x10) && (Byte == 0x02) && (dwFrameCounter == 0))                               //Detect Start frame
    {
      memset(ByteArr, 0, sizeof(ByteArr));
      ByteArr[dwFrameCounter++] = LastByte;
    }

    if(dwFrameCounter > 0)                                                                          // collect frame bytes
    {
      ByteArr[dwFrameCounter++] = Byte;      
    }

    if((preLastByte == 0x10) && (LastByte == 0x03) && (dwFrameCounter > 5))                         // Detect End frame
    {
      if(ByteArr[2] == 0x02)
      {
        string  strframeContent;
        INT     iTemp           = 0;
        CHAR    acBuffer[10]    = {0};
        UCHAR   ucAC2value      = 0;
        
        // Set state connection indicator and receiving indicator
        SendMessage(g_hReceiveIndDlg,   BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(g_hConnectedIndDlg, BM_SETCHECK, BST_CHECKED, 0);

        // Change label on state connection indicator
        SetWindowText( g_hConnectedIndDlg, "Po³¹czono");

        // Set flag state connection indicator and receiving indicator
        g_stConnectionState.bZigBeeConnected = TRUE;
        g_stConnectionState.bZigBeeReceiving = TRUE;

        // Start timers
        SetTimer(g_hwnd, ID_TIMER_1,  1000, 0);                                                      // Receiving frame timer   (1s)
        SetTimer(g_hwnd, ID_TIMER_2, 25000, 0);                                                      // Disconnect expire timer (25s)
        
        // Create string for "Message window"
        strframeContent = vPrintAsHex(ByteArr, dwFrameCounter);

        // Show "Message window" content
        SetWindowText (g_hWndMessageWindow, strframeContent.c_str());

#if STOP_AT_INTER_CONN_ERR
        // Detect error
        if(0x00 == (unsigned char)ByteArr[10])
        {
          Beep( 750, 300 );                                                                          // Found Error transmission between AT1284P & AT3290P
          MessageBox (NULL, "Connection error", "Error", MB_ICONERROR | MB_OK);
        }
         //Beep( 750, 10 );
#endif // STOP_AT_INTER_CONN_ERR

        // Prepare AC 2 value
        ucAC2value = ByteArr[10];

        if(g_bMultiplyAC2)
        {
          if(ByteArr[10] > 51)                                                                      // Exceed range of UINT 255/5=51
          {
            ucAC2value = 0xFF;
          }
          else
          {
            ucAC2value = ByteArr[10] * RESISTOR_DIVIDER_RATIO;
          }
        }
        
        // Set progressbar for AC 1 and AC 2
        SendMessage(g_hProgressBarAC1, PBM_SETPOS, (WPARAM)((unsigned char)ByteArr[ 9]), 0);
        SendMessage(g_hProgressBarAC2, PBM_SETPOS, (WPARAM)((unsigned char)ucAC2value), 0);

        // Calculate and display AC 1 value in Volts
        sprintf(acBuffer, "%.2f V", ((float)(unsigned char)ByteArr[9])/((float)MAX_ADC_VALUE)*g_fAC1ReffVoltage);
        SetWindowText (g_hEditAC1Dlg, acBuffer);

        // Calculate and display AC 2 value in Volts
        sprintf(acBuffer, "%.2f V", ((float)(unsigned char)ucAC2value)/((float)MAX_ADC_VALUE)*g_fAC2ReffVoltage);
        SetWindowText (g_hEditAC2Dlg, acBuffer);

        // Combine temperature value
        iTemp |= (INT)ByteArr[8] << 24;
        iTemp |= (INT)ByteArr[7] << 16;
        iTemp |= (INT)ByteArr[6] <<  8;
        iTemp |= (INT)ByteArr[5] <<  0;

        // Create and display temperature value
        sprintf(acBuffer, "%i *C", iTemp);
        SetWindowText (g_hEditTempDlg, acBuffer);

        Sleep (5000);
      }
      
      memset(ByteArr, 0, sizeof(ByteArr));
      dwFrameCounter = 0;
    }

    preLastByte = LastByte;
    LastByte    = Byte;




    Sleep (1);
	if(TRUE == g_bSendSerial){
		BYTE ByteArrToSend[50] = {0};
		sprintf((char*)ByteArrToSend, "*:P200H%03dR150:#", g_SliderPos);
		//SetWindowText (g_hEditAC2Dlg, (char*)ByteArrToSend);
		SetWindowText (g_hWndMessageWindow, (char*)ByteArrToSend);
		g_bSendSerial = FALSE;
		
		if(COM_DISCONNECTED != g_eComPortState){
			WriteFile (g_hSerial,           // Port handle
					  &ByteArrToSend,       // Pointer to data to write
					  16,                   // Number of bytes to write
					  &dwBytesTransferred,  // Pointer to number of bytes sent
					  NULL                  // Must be NULL for Windows CE
					 );
		}
	 
	}

  }
      

  _endthread();
}

// EOF
