// tools.h : Defines the auxiliary tools
// autor:  Tomasz Jokiel

#ifndef TOOLS_H
#define TOOLS_H

// ************** Includes *************************************************************************

#include <string>
#include <windows.h>

// ************** Definitions **********************************************************************
// ************** Function prototypes **************************************************************

using namespace std;

string  vPrintAsHex             (BYTE* str, unsigned int length);
VOID    vToggleMessageIndidator (UINT uiRadioButtonId);
VOID    vSetMessageIndidator    (UINT uiRadioButtonId);
VOID    vClearMessageIndidator  (UINT uiRadioButtonId);
VOID    vIgnoreClick            (VOID);

#endif // TOOLS_H

// EOF
