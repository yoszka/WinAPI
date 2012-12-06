#ifndef CONTENT_H
#define CONTENT_H

// ************** Definitions **********************************************************************
#define ID_BUTTON_2        (0x02)                                                                   // Exemplary ID of button for Exit
#define ID_EDIT_1          (0x03)                                                                   // ID of EDIT text
#define ID_RADIOBUTTON_1   (0x04)   // Odbior
#define ID_RADIOBUTTON_2   (0x05)   // Polaczono

// ************** Function prototypes **************************************************************
BOOL bInitializeContent(HINSTANCE hMainProgramInstance, HWND hWnd);                                 // Declare Initialization function

#endif // CONTENT_H