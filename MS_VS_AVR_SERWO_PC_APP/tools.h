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

string  vPrintAsHex					(BYTE* str, unsigned int length);
VOID	vSetServoStateIndidator		(UINT uiRadioButtonId);
VOID	vClearServoStateIndidator	(UINT uiRadioButtonId);

#endif // TOOLS_H

// EOF
