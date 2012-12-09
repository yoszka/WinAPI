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
#define SEND_BUFFER_SIZE			(50)
#define RECEIVE_BUFFER_SIZE			(50)
#define MINIMUM_DATA_RECEIVED_CNT	(5)
// *********** Function prototypes *****************************************************************
// *********** Global variables definition *********************************************************
BYTE oneByte, lastByte;
BYTE dataReceived[RECEIVE_BUFFER_SIZE];
UCHAR receivedByteCount = 0;
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
  DWORD dwBytesTransferred, dwFrameCounter = 0;
  BYTE Byte;

  
  while(true)
  {
	  
    ReadFile (g_hSerial,            // Port handle
              &Byte,                // Pointer to data to read
              1,                    // Number of bytes to read
              &dwBytesTransferred,  // Pointer to number of bytes
                                    // read
              NULL                  // Must be NULL for Windows CE
             );

	oneByte = Byte;

    if((lastByte == '*') && (oneByte == ':') && (receivedByteCount == 0))          //Detect Start frame
    {
      dataReceived[receivedByteCount++] = lastByte;
    }

    if(receivedByteCount > 0)
    {
    	dataReceived[receivedByteCount++] = oneByte;
    	if(receivedByteCount == RECEIVE_BUFFER_SIZE) receivedByteCount--;
    }


    if((lastByte == ':') && (oneByte == '#') && (receivedByteCount > MINIMUM_DATA_RECEIVED_CNT)){	// Collected all message
    	dataReceived[receivedByteCount++] = 0x00;
    	receivedByteCount = 0;

    	if(0 == strcmp("*:SERVO_READY:#", (char*)dataReceived)){									// Check message
			SendMessage(g_hMainWindow, WM_SERVO_STATE, 0, (LPARAM)0);
    	}
	}
	lastByte = oneByte;
    
	
 
    Sleep (1);

	if((TRUE == g_stSerialSendCmd.bSerialDataPending) && (COM_DISCONNECTED != g_eComPortState)){
		BYTE ByteArrToSend[SEND_BUFFER_SIZE] = {0};
		WCHAR wcToSend[SEND_BUFFER_SIZE] = {0};

		switch(g_stSerialSendCmd.eCommandType){

		case SERVO_CONNECT:
			wcscpy_s(wcToSend, SEND_BUFFER_SIZE, L"*:START:#");
			strncpy((char*)ByteArrToSend, "*:START:#", SEND_BUFFER_SIZE);
			break;

		case SERVO_MOVE:		
			sprintf_s((char*)ByteArrToSend, SEND_BUFFER_SIZE, "*:P200H%03dR150:#", g_SliderPos);
			wsprintf(wcToSend, L"*:P200H%03dR150:#", g_SliderPos);
			break;

		default:
			break;
			
		}


		WriteFile (g_hSerial,           // Port handle
					&ByteArrToSend,       // Pointer to data to write
					16,                   // Number of bytes to write
					&dwBytesTransferred,  // Pointer to number of bytes sent
					NULL                  // Must be NULL for Windows CE
					);

		SetWindowText (g_hWndMessageWindow, wcToSend);
		

	g_stSerialSendCmd.bSerialDataPending = FALSE;	 
	}

	Sleep (1);

  }
      

  _endthread();
}

// EOF
