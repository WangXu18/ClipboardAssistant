#include "stdafx.h"
#include "lbtoast.h"
#include "lbtoast_pop.h"
#include "KBkInitSource.h"
#include "CmdLineParser.h"
#include "publish/kxedump/kxedump.h"
#include <signal.h>

CAppModule _Module;

namespace {
void HandleAbortSignal(int signum) {
  ::RaiseException(STATUS_FATAL_APP_EXIT, 0, 0, nullptr);
  _exit(1);
}

void KSInvalidParameter(const wchar_t* expression,
                        const wchar_t* function,
                        const wchar_t* file,
                        unsigned int line,
                        uintptr_t reserved) {
  ::RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, nullptr);
  _exit(1);
}

void KSPureCall() {
  ::RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, nullptr);
  _exit(1);
}

void RegisterCrashHandlers() {
  _set_invalid_parameter_handler(KSInvalidParameter);
  _set_purecall_handler(KSPureCall);
  signal(SIGABRT, HandleAbortSignal);
}
};

HANDLE InitInstanceMutex() {
  HANDLE hMutex = NULL;
  SECURITY_ATTRIBUTES SecurityAttributes;
  SECURITY_DESCRIPTOR SecurityDesc;

  memset(&SecurityAttributes, 0, sizeof(SECURITY_ATTRIBUTES));

  BOOL bRet = ::InitializeSecurityDescriptor(&SecurityDesc,
                                             SECURITY_DESCRIPTOR_REVISION);
  if (bRet == FALSE) {
    return NULL;
  }

  bRet = ::SetSecurityDescriptorDacl(&SecurityDesc, TRUE, NULL, FALSE);
  if (bRet == FALSE) {
    return NULL;
  }

  SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  SecurityAttributes.bInheritHandle = FALSE;
  SecurityAttributes.lpSecurityDescriptor = &SecurityDesc;

  hMutex = ::CreateMutex(&SecurityAttributes,
                         FALSE,
                         L"Global\\lbtoast_clipboard");
  DWORD dwLastError = GetLastError();
  if (dwLastError == ERROR_ALREADY_EXISTS) {
    ::CloseHandle(hMutex);
    return NULL;
  }

  return hMutex;
}

int Run(LPTSTR lpstrCmdLine, int nCmdShow = SW_SHOWDEFAULT) {
  CCmdLineParser parser;
  parser.Parse(lpstrCmdLine);
  CString typePop = parser.GetVal(L"type");
  if (typePop == L"clipboard") {
    HANDLE mutex = InitInstanceMutex();
    if (mutex == NULL) {
      return 0;
    }

    CLbToast dlg;
    if (!dlg.InitWorkThread()) {
      ::CloseHandle(mutex);
      return 0;
    }
    dlg.InitDefaultConf();
    dlg.Start();
    ::CloseHandle(mutex);
  }

  return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  InitializeDump();
  RegisterCrashHandlers();

  HRESULT hRes = ::CoInitialize(NULL);
  // If you are running on NT 4.0 or higher you can use the following call instead to 
  // make the EXE free threaded. This means that calls come in on a random RPC thread.
  //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  ATLASSERT(SUCCEEDED(hRes));

  // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
  ::DefWindowProc(NULL, 0, 0, 0L);

  AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

  hRes = _Module.Init(NULL, hInstance);
  ATLASSERT(SUCCEEDED(hRes));

  KBkInitSource::Instance().InitResource(L"");
  int nRet = Run(lpCmdLine, nCmdShow);

  _Module.Term();
  ::CoUninitialize();

  return nRet;
}