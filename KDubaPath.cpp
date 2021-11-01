#include "stdafx.h"
#include "KDubaPath.h"
#include <shlobj.h>
#include "publish/framework/KRegister2.h"

CString KDubaPath::GetModuleFullPath(HMODULE hModule) {
  DWORD dwRet = 0;
  CString strTemp;
  TCHAR szFileName[MAX_PATH + 1] = {0};

  dwRet = ::GetModuleFileName(hModule, szFileName, MAX_PATH);
  if (!dwRet)
    goto Exit0;

  strTemp = szFileName;

Exit0:
  return strTemp;
}

CString KDubaPath::PathToFileName(LPCTSTR szFullPath) {
  CString strPath(szFullPath);

  strPath.Replace(_T('/'), _T('\\'));
  int nPos = strPath.ReverseFind(_T('\\'));
  if (nPos == -1)
    return strPath;
  else
    return strPath.Right(strPath.GetLength() - nPos - 1);
}

CString KDubaPath::PathToFolderPath(LPCTSTR szFullPath) {
  CString strTemp;

  strTemp = szFullPath;
  int nPos = -1;
  nPos = strTemp.ReverseFind(_T('\\'));
  if (-1 == nPos) {
    nPos = strTemp.ReverseFind(_T('/'));
  }

  if (-1 == nPos)
    return _T("");

  return strTemp.Left(nPos + 1);
}

CString KDubaPath::GetModuleFolder(HMODULE hModule) {
  return PathToFolderPath(GetModuleFullPath(hModule));
}

CString KDubaPath::FileNameRemoveSuffix(LPCTSTR szFileName) {
  CString strTemp;

  strTemp = szFileName;
  int nPos = -1;
  nPos = strTemp.ReverseFind(_T('.'));

  if (-1 == nPos)
    return strTemp;

  return strTemp.Left(nPos);
}

CString KDubaPath::PathToSuffix(LPCTSTR szFullPath) {
  CString strTemp = PathToFileName(szFullPath);

  int nPos = -1;
  nPos = strTemp.ReverseFind(_T('.'));

  if (-1 == nPos)
    return strTemp;

  return strTemp.Mid(nPos + 1);
}

CString KDubaPath::GetParsentFolder(LPCTSTR szFullPath) {
  CString strParsentPath(szFullPath);

  PathRemoveBackslash(strParsentPath);
  PathRemoveFileSpec(strParsentPath);
  if (strParsentPath.IsEmpty())
    strParsentPath = szFullPath;

  return strParsentPath;
}

CString KDubaPath::GetRootPath(LPCTSTR szFullPath) {
  CString strRoot(szFullPath);
  CString strTempPath(szFullPath);

  int nPos = strTempPath.Find(_T('\\'));
  if (nPos != -1)
    strRoot = strTempPath.Left(nPos);

  return strRoot;
}

CString KDubaPath::LegalizePath(const CString& strPath) {
  CString strOriginPath = strPath;
  CString strData(strOriginPath);
  CString strFolderPath;
  CString strDstPath;

  while (1) {
    strFolderPath = GetRootPath(strData);
    if (strFolderPath == _T(".."))
      strDstPath = GetParsentFolder(strDstPath);
    else
      strDstPath.Append(strFolderPath);

    PathAddBackslash(strDstPath);
    strData = strData.Mid(strFolderPath.GetLength());
    strData.TrimLeft(L"\\");

    if (strData.IsEmpty())
      break;
  }

  return strDstPath;
}

void KDubaPath::PathAddBackslash(CString& strPath) {
  CString strTemp;
  strTemp = strPath.Right(1);

  if (strTemp != _T("\\") && strTemp != _T("/"))
    strPath.Append(_T("\\"));
}

void KDubaPath::PathRemoveBackslash(CString& strPath) {
  if (strPath.Right(1) == _T("\\"))
    strPath = strPath.Left(strPath.GetLength() - 1);
}

void KDubaPath::PathRemoveFileSpec(CString& strPath) {
  int nPos = strPath.ReverseFind(_T('\\'));
  if (nPos == -1) {
    strPath.Empty();
  } else {
    strPath = strPath.Left(nPos);
  }
}

BOOL KDubaPath::CreateDeepDirectory(LPCTSTR szPath) {
  BOOL bRetCode = FALSE;
  CString strPath(szPath);

  if (GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES)
    return TRUE;

  bRetCode = ::CreateDirectory(szPath, NULL);
  if (!bRetCode && ::GetLastError() != ERROR_ALREADY_EXISTS) {
    PathRemoveFileSpec(strPath);
    if (strPath.IsEmpty())
      return FALSE;

    bRetCode = CreateDeepDirectory(strPath);
    if (!bRetCode)
      return FALSE;

    bRetCode = ::CreateDirectory(szPath, NULL);
    if (!bRetCode && ::GetLastError() != ERROR_ALREADY_EXISTS)
      return FALSE;
  }

  return TRUE;
}

CString KDubaPath::GetDubaInstallPathFromReg() {
  CString strPath;
  CString strTemp;
  KRegister2 regKey;

  if (FALSE ==
      regKey.Open(HKEY_LOCAL_MACHINE, _T("software\\kingsoft\\antivirus")))
    goto Exit0;

  if (FALSE == regKey.Read(_T("ProgramPath"), strTemp))
    goto Exit0;

  strPath = strTemp;
  PathAddBackslash(strPath);

Exit0:
  return strPath;
}

CString KDubaPath::GetPlatformLang(void) {
  KRegister2 reg;
  CString strLang;
  CString strTemp;

  LPCTSTR szKeyLang = _T("Lang");
  LPCTSTR szRegKisCommon = _T("SOFTWARE\\Kingsoft\\KISCommon");

  if (FALSE == reg.Open(HKEY_LOCAL_MACHINE, szRegKisCommon))
    goto Exit0;

  if (FALSE == reg.Read(szKeyLang, strTemp))
    goto Exit0;

  strLang = strTemp;
  strLang.MakeLower();

Exit0:
  if (strLang.IsEmpty())
    strLang = _T("chs");

  return strLang;
}

BOOL KDubaPath::DeleteDirectory(LPCTSTR szDir, BOOL bContinueWhenFail) {
  BOOL bReturn = FALSE;
  CString sDir;
  CString sFindPath;
  WIN32_FIND_DATA fData;
  HANDLE hFind = INVALID_HANDLE_VALUE;

  sDir = szDir;
  PathAddBackslash(sDir);

  sFindPath.Format(_T("%s*.*"), sDir);

  hFind = ::FindFirstFile(sFindPath, &fData);
  if (hFind == INVALID_HANDLE_VALUE)
    goto Exit0;

  do {
    if (0 == _tcscmp(fData.cFileName, _T(".")) ||
        0 == _tcscmp(fData.cFileName, _T("..")))
      continue;

    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      CString sSubfolder;
      sSubfolder.Format(_T("%s%s\\"), sDir, fData.cFileName);
      if ((FALSE == DeleteDirectory(sSubfolder, bContinueWhenFail)) &&
          (!bContinueWhenFail))
        goto Exit0;
    } else {
      CString sFileName = fData.cFileName;
      sFileName.MakeLower();
      if ((FALSE == DeleteFile(sDir + sFileName)) && (!bContinueWhenFail))
        goto Exit0;
    }

  } while (::FindNextFile(hFind, &fData) != 0);

  bReturn = TRUE;

Exit0:
  if (hFind != INVALID_HANDLE_VALUE)
    ::FindClose(hFind);

  RemoveDirectory(sDir);
  PathRemoveBackslash(sDir);
  RemoveDirectory(sDir);

  return bReturn;
}

CString KDubaPath::GetKavAppDataPath(HMODULE hModule) {
  CString strPath;
  TCHAR szPath[MAX_PATH] = {0};

  if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_APPDATA, FALSE)) {
    strPath = szPath;
    KDubaPath::PathAddBackslash(strPath);
  } else {
    strPath = KDubaPath::GetModuleFolder((HMODULE)&__ImageBase);
    strPath.Append(_T("appdata\\"));
  }

  if (!strPath.IsEmpty()) {
    strPath.Append(_T("kingsoft\\kis\\"));

    KDubaPath::CreateDeepDirectory(strPath);
  }

  return strPath;
}

CString KDubaPath::GetLiebaoInstallPathFromReg() {
  CString strPath;
  CString strTemp;
  KRegister2 regKey;

  if (FALSE == regKey.Open(HKEY_LOCAL_MACHINE, _T("software\\liebao")))
    goto Exit0;

  if (FALSE == regKey.Read(_T("ProgramPath"), strTemp))
    goto Exit0;

  strPath = strTemp;
  PathAddBackslash(strPath);

Exit0:
  return strPath;
}

CString KDubaPath::GetLiebaoRootInstallPath() {
  KRegister2 reg;
  CString strTemp;

  LPCTSTR lpszInstallPath = _T("Install Path Dir");
  LPCTSTR lpszKeyPath = L"SOFTWARE\\liebao";

  if (FALSE == reg.Open(HKEY_LOCAL_MACHINE, lpszKeyPath))
    goto Exit0;

  if (FALSE == reg.Read(lpszInstallPath, strTemp))
    goto Exit0;

Exit0:
  return strTemp;
}

BOOL KDubaPath::CopyDirectory(const CString& srcPath, const CString& dstPath) {
  if (!CreateDeepDirectory(dstPath)) {
    return FALSE;
  }
  CString strSrcPath = srcPath;
  CString strDstPath = dstPath;
  KDubaPath::PathAddBackslash(strSrcPath);
  KDubaPath::PathAddBackslash(strDstPath);
  CString wsPath = strSrcPath + L"*.*";
  WIN32_FIND_DATAW finddata = {0};
  HANDLE hFind = FindFirstFileW(wsPath, &finddata);
  if (hFind == INVALID_HANDLE_VALUE) {
    return FALSE;
  }
  do {
    if (0 == wcscmp(finddata.cFileName, L".") ||
        0 == wcscmp(finddata.cFileName, L".."))
      continue;
    if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (!CopyDirectory(strSrcPath + finddata.cFileName,
                         strDstPath + finddata.cFileName)) {
        FindClose(hFind);
        return FALSE;
      }
    } else {
      if (!::CopyFileW(strSrcPath + finddata.cFileName,
                       strDstPath + finddata.cFileName, FALSE)) {
        FindClose(hFind);
        return FALSE;
      }
    }
  } while (FindNextFileW(hFind, &finddata));
  FindClose(hFind);
  return TRUE;
}
