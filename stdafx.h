// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "targetver.h"

template <typename T1, typename T2>
T1 min(T1 a, T2 b) {
  return a < b ? a : b;
}

template <typename T1, typename T2>
T1 max(T1 a, T2 b) {
  return a > b ? a : b;
}

#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#ifndef PSAPI_VERSION
#define PSAPI_VERSION 1
#endif
#include <Psapi.h>
#include <Tlhelp32.h>
#pragma comment(lib, "Psapi.lib")

#include <shellapi.h>

// C RunTime Header Files
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be
                                            // explicit
#include <type_traits>

namespace std {
  template <class...>
  using __void_t = void;
}

#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlwin.h>
#include <wtl_10/atlapp.h>
#include <wtl_10/atlcrack.h>
#include <wtl_10/atlctrls.h>
#include <wtl_10/atlctrlx.h>
#include <wtl_10/atlframe.h>
#include <wtl_10/atlgdi.h>
#include <wtl_10/atlmisc.h>
#include <wtl_10/atlsplit.h>
#include <map>

#include "xml/tinyxml.h"

#include "bkwin/bkres/bkres.h"
#include "bkwin/bkwin/CBkDialogViewImplEx.h"