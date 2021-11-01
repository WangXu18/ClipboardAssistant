#ifndef _KDUBAPATH_H_
#define _KDUBAPATH_H_
#include <atlstr.h>

class KDubaPath {
 public:
  static CString GetModuleFullPath(HMODULE hModule);
  static CString GetDubaInstallPathFromReg();
  static CString GetLiebaoInstallPathFromReg();
  static CString GetPlatformLang(void);
  static CString GetModuleFolder(HMODULE hModule);
  static CString GetKavAppDataPath(HMODULE hModule);

  static CString PathToFileName(LPCTSTR szFullPath);
  static CString PathToFolderPath(LPCTSTR szFullPath);
  static CString FileNameRemoveSuffix(LPCTSTR szFileName);
  static CString PathToSuffix(LPCTSTR szFullPath);

  static CString GetParsentFolder(LPCTSTR szFullPath);
  static CString GetRootPath(LPCTSTR szFullPath);
  static CString LegalizePath(const CString& strPath);

  static void PathAddBackslash(CString& strPath);
  static void PathRemoveBackslash(CString& strPath);
  static void PathRemoveFileSpec(CString& strPath);

  static BOOL CreateDeepDirectory(LPCTSTR szPath);
  static BOOL DeleteDirectory(LPCTSTR szPath, BOOL bContinueWhenFail);
  static CString GetLiebaoRootInstallPath();
  static BOOL CopyDirectory(const CString& srcPath, const CString& dstPath);
};

#endif