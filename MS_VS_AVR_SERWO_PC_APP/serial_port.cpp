// serial_port.cpp : Defines the serial port functions
// autor:  Tomasz Jokiel

// *********** Includes ****************************************************************************

#include "stdafx.h"
#include "main.h"
#include <string>

// *********** Definitions and Enums ***************************************************************
// *********** Function prototypes *****************************************************************
// *********** Global variables definition *********************************************************
// *********** Declaration of external global variables ********************************************
// *********** Typedefs ****************************************************************************
// *********** Implememntation *********************************************************************

using namespace std;

/***************************************************************************************************
* brief      : open serial port with given name (i.e. "COM6")
* side effect:
***************************************************************************************************/
BOOL bOpenSerialPort(char* pcPortName)
{
  // Open the serial port.
  g_hSerial = CreateFile (pcPortName,                 // Pointer to the name of the port
                      GENERIC_READ | GENERIC_WRITE, // Access (read-write) mode
                      0,                            // Share mode
                      NULL,                         // Pointer to the security attribute
                      OPEN_EXISTING,                // How to open the serial port
                      0,                            // Port attributes
                      NULL);                        // Handle to port with attribute




  if(g_hSerial==INVALID_HANDLE_VALUE)
  {
    if(GetLastError()==ERROR_FILE_NOT_FOUND)
      {
        // Serial port does not exist.
        GetLastError ();
        return FALSE;
      }

        // Some other error occurred.
        GetLastError ();
        return FALSE;
  }


  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength=sizeof(DCB);

  // Get the default port setting information.
  if (!GetCommState(g_hSerial, &dcbSerialParams))
  {
    // "Error getting state.
    GetLastError ();
    return FALSE;
  }

  dcbSerialParams.BaudRate      = CBR_9600;
  dcbSerialParams.fBinary       = TRUE;          // Binary mode; no EOF check
  dcbSerialParams.ByteSize      = 8;
  dcbSerialParams.StopBits      = ONESTOPBIT;
  dcbSerialParams.Parity        = NOPARITY;
  dcbSerialParams.fOutxCtsFlow  = FALSE;         // No CTS output flow control
  dcbSerialParams.fOutxDsrFlow  = FALSE;         // No DSR output flow control
  dcbSerialParams.fAbortOnError = FALSE;

  if(!SetCommState(g_hSerial, &dcbSerialParams))
  {
    // Error setting serial port state.
    GetLastError ();
    return FALSE;
  }

  // Retrieve the timeout parameters for all read and write operations
  // on the port.
  COMMTIMEOUTS CommTimeouts;
  GetCommTimeouts (g_hSerial, &CommTimeouts);

  // Change the COMMTIMEOUTS structure settings.
  CommTimeouts.ReadIntervalTimeout          = MAXDWORD;
  CommTimeouts.ReadTotalTimeoutMultiplier   = 10;
  CommTimeouts.ReadTotalTimeoutConstant     = 50;
  CommTimeouts.WriteTotalTimeoutMultiplier  = 10;
  CommTimeouts.WriteTotalTimeoutConstant    = 1000;

  // Set the timeout parameters for all read and write operations
  // on the port.
  if (!SetCommTimeouts (g_hSerial, &CommTimeouts))
  {
    // Unable to set the timeout parameters.
    GetLastError ();
    return FALSE;
  }
  
  return TRUE;
}

// EOF
