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
  DWORD dwBytesTransferred, dwFrameCounter = 0;

  
  while(true)
  {
	  /*
    ReadFile (g_hSerial,            // Port handle
              &Byte,                // Pointer to data to read
              1,                    // Number of bytes to read
              &dwBytesTransferred,  // Pointer to number of bytes
                                    // read
              NULL                  // Must be NULL for Windows CE
             );
    
	*/
 
    Sleep (1);

	if(TRUE == g_bSendSerial){
		BYTE ByteArrToSend[50] = {0};
		//sprintf((char*)ByteArrToSend, "*:P200H%03dR150:#", g_SliderPos);
		sprintf_s((char*)ByteArrToSend, 50, "*:P200H%03dR150:#", g_SliderPos);
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
