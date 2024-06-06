#include "types.h"
#include <iostream>
#include <vector>
#include <string>

namespace Inquire {

#ifdef _WIN32

#include <windows.h>

// extern HANDLE hConsole;
extern HANDLE hStdin;
extern INPUT_RECORD irInputRecord;
extern DWORD dwEventsRead;
extern KEY_EVENT_RECORD key;

#endif


KeyResult key_catch();

}