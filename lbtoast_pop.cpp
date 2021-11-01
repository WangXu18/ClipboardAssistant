#include "stdafx.h"
#include "lbtoast_pop.h"

#include <regex>
#include "publish/framework/KRegister2.h"
#include "publish/framework/kdubapath.h"
#include "lbtoast_record.h"

static HWND g_hwnd;

namespace {
const long long UNIX_TIME_START = 0x019DB1DED53E8000;
const long TICKS_PER_SECOND = 10000000;
const long long SECONDS_PER_DAY = 86400;
}

CLbToast::CLbToast()
    : CBkDialogViewImplEx<CLbToast>(IDR_DLG_MAIN) {
  EnableShadow(TRUE);
  SetShadowSize(SHADOW_SIZE);
  SetShadowPaintHook(this);
  SetDrawBorder(FALSE);
}

CLbToast::~CLbToast() {
}

BOOL CLbToast::OnInitDialog(CWindow /*wndFocus*/,
                            LPARAM /*lInitParam*/) {
  g_hwnd = m_hWnd;
  return TRUE;
}

void CLbToast::OnDrawClipboard() {
  if (hwnd_nextviewer_) {
    ::SendMessage(hwnd_nextviewer_, WM_DRAWCLIPBOARD, NULL, NULL);
  }

  if (CanShow()) {
    UpdateDatas();
    ShowDialog();
  }
}

void CLbToast::OnChangeCbChain(CWindow wndRemove, CWindow wndAfter) {
  if (hwnd_nextviewer_ == wndRemove.m_hWnd) {
    hwnd_nextviewer_ = wndAfter.m_hWnd;
  } else if (hwnd_nextviewer_ != NULL) {
    ::SendMessage(hwnd_nextviewer_,
                  WM_CHANGECBCHAIN,
                  (WPARAM)wndRemove.m_hWnd,
                  (LPARAM)wndAfter.m_hWnd);
  }
}

int CLbToast::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  hwnd_nextviewer_ = ::SetClipboardViewer(m_hWnd);
  return 0;
}

void CLbToast::OnDestroy() {
  KillTimer(TIMER_ID_TASK);
  ::ChangeClipboardChain(m_hWnd, hwnd_nextviewer_);
  UnInitWorkThread();
  PostQuitMessage(NULL);
}

void CLbToast::OnTimer(UINT_PTR nIDEvent) {
  if (nIDEvent == TIMER_ID_TASK) {
    KillTimer(TIMER_ID_TASK);
    ShowWindow(SW_HIDE);
    UpdateCounts();
    if (menu_.IsShowing()) {
      menu_.OnHideMenu();
    }
  }
}

void CLbToast::OnMouseLeave() {
  SetMsgHandled(FALSE);
  if (IsWindowVisible()) {
    SetTimer(TIMER_ID_TASK, delay_seconds_ * 1000);
  }
}

void CLbToast::OnMouseMove(UINT nFlags, CPoint point) {
  SetMsgHandled(FALSE);
  KillTimer(TIMER_ID_TASK);
}

BOOL CLbToast::NotifyShadowPaint(HDC hDC, CRect& rct) {
  WINDOWPOS wndPos = {0};
  CBkExpendImg exmPandImg;

  wndPos.x = rct.left;
  wndPos.y = rct.top;
  wndPos.cx = rct.Width();
  wndPos.cy = rct.Height();

  exmPandImg.SetAttribute("pos", "0,0,-0,-0", TRUE);
  exmPandImg.BkSendMessage(WM_WINDOWPOSCHANGED, NULL, (LPARAM)&wndPos);
  exmPandImg.SetAttribute("margin_left", "55", TRUE);
  exmPandImg.SetAttribute("margin_right", "55", TRUE);
  exmPandImg.SetAttribute("margin_top", "55", TRUE);
  exmPandImg.SetAttribute("margin_bottom", "55", TRUE);
  exmPandImg.SetAttribute("skin", "mainwnd_shadow", TRUE);
  exmPandImg.BkSendMessage(WM_PAINT, (WPARAM)hDC);

  return FALSE;
}

void CLbToast::OnBkMenuCmd(CBkDialogMenu* pDialogMenu, LPCWSTR lpszMenuName) {
  CString strCmd;
  strCmd.Format(L"OnBkss%s=", lpszMenuName);
  MessageBox(strCmd);
}

void CLbToast::OnBtnSetting() {
  CRect rcWnd;
  GetItemRect(IDC_BTN_SETTING, rcWnd);
  ClientToScreen(&rcWnd);
  CPoint pt;
  pt.x = rcWnd.left - 6;
  pt.y = rcWnd.bottom;
  menu_.PopUp(m_hWnd, pt);
  ClearRecordData();
}

void CLbToast::OnBtnClose() {
  KillTimer(TIMER_ID_TASK);
  ShowWindow(SW_HIDE);
  ClearRecordData();
  lbtoast::ReportClipbooardAction(
    action::kClickType, click::kCloseType, 1, 0);
}

void CLbToast::OnBtnOk() {
  TuneUpBrowserWithLink();
  ClearRecordData();
  lbtoast::ReportClipbooardAction(
    action::kClickType, click::kOkType, 1, 0);
}

void CLbToast::OnLinkTextClick() {
  TuneUpBrowserWithLink();
  ClearRecordData();
  lbtoast::ReportClipbooardAction(
    action::kClickType, click::kLinkTextType, 1, 0);
}

void CLbToast::TuneUpBrowserWithLink() {
  ShowWindow(SW_HIDE);

  CString liebao_main_exe_path = KDubaPath::GetLiebaoInstallPathFromReg();
  KDubaPath::PathAddBackslash(liebao_main_exe_path);
  liebao_main_exe_path.Append(L"liebao.exe");
  if (::PathFileExists(liebao_main_exe_path) == TRUE) {
    CreateProcessNormally(liebao_main_exe_path, last_str_, SW_NORMAL);
  }
}

LRESULT CLbToast::OnHideDialog(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  ShowWindow(SW_HIDE);
  return TRUE;
}

LRESULT CLbToast::OnFinishApplication(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  DestroyWindow();
  return TRUE;
}

void CLbToast::UpdatePosition() {
  CRect rcWindow;
  CRect rcWorkArea;

  GetWindowRect(&rcWindow);
  ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

  rcWindow.MoveToX(rcWorkArea.right/2 - 244);
  rcWindow.MoveToY(rcWorkArea.top + 14);
  MoveWindow(rcWindow, TRUE);
  ::SetWindowPos(m_hWnd, HWND_TOP,
                 NULL, NULL, NULL, NULL,
                 SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

bool CLbToast::IsLiebaoVip() {
  CRegKey reg;
  LONG lret = reg.Open(HKEY_CURRENT_USER, L"SOFTWARE\\liebao", KEY_QUERY_VALUE);
  if (lret != ERROR_SUCCESS) {
    return false;
  }
  DWORD dwVipType = 0;
  lret = reg.QueryDWORDValue(L"vtp", dwVipType);
  if (lret != ERROR_SUCCESS || dwVipType == 0) {
    return false;
  }
  return true;
}

bool CLbToast::IsClipAssistantEnable() {
  CRegKey reg;
  LONG lret = reg.Open(HKEY_CURRENT_USER,
                       L"SOFTWARE\\liebao",
                       KEY_QUERY_VALUE | KEY_SET_VALUE);
  if (lret != ERROR_SUCCESS) {
    return false;
  }

  DWORD clip_value = 0;
  lret = reg.QueryDWORDValue(L"clipassistant", clip_value);
  if (lret == ERROR_FILE_NOT_FOUND) {
    return true;
  }

  if (clip_value == 0) {
    return false;
  }
  return true;
}

bool CLbToast::CanShow() {
  bool res = false;
  do {
    if (is_first_) {
      is_first_ = false;
      break;
    }

    if (IsDatasCopyFromSkipProcess()) {
      break;
    }

    if (!IsClipboardHasUrls()) {
      break;
    }

    if (IsSameAsLastData()) {
      break;
    }

    if (!valid_) {
      break;
    }

    if (IsLiebaoVip()) {
      break;
    }

    if (!IsClipAssistantEnable()) {
      break;
    }

    if (GetValueWithKey(L"clipboard", L"is_cur_user_temp_no_pop")) {
      if (!CheckPoptime()) {
        break;
      }
    }

    /*if (IsBrowserInstalled(_T("SOFTWARE\\SogouExplorer")) ||
        IsBrowserInstalled(_T("SOFTWARE\\2345Explorer"))) {
      break;
    }*/

    res = true;
  } while (false);

  return res;
}

//bool CLbToast::IsBrowserInstalled(LPCTSTR subkey) {
//  bool res = true;
//  KRegister2 regKey;
//  DWORD dwResult = ERROR_SUCCESS;
//
//  dwResult = regKey.Open(HKEY_LOCAL_MACHINE, subkey);
//  if (dwResult == FALSE) {
//    dwResult = regKey.Open(HKEY_CURRENT_USER, subkey);
//    if (dwResult == FALSE) {
//      res = false;
//    }
//  }
//
//  return res;
//}

bool CLbToast::IsDatasCopyFromSkipProcess() {
  std::wstring dest_str = GetExePath();
  if (dest_str.empty()) {
    return false;
  }

  std::transform(dest_str.begin(), dest_str.end(), dest_str.begin(), ::tolower);
  if (conf_data_.empty()) {
    return false;
  }

  for (auto application : conf_data_) {
    if (dest_str.find(application) != std::string::npos) {
      return true;
    }
  }
  return false;
}

std::wstring CLbToast::GetExePath() {
  std::wstring exe_str;

  HWND hWnd = GetForegroundWindow();
  if (hWnd == NULL) {
    return exe_str;
  }
  DWORD process_id = 0;
  DWORD thread_id = ::GetWindowThreadProcessId(hWnd, &process_id);
  if (!thread_id || !process_id) {
    return exe_str;
  }

  MODULEENTRY32W me = {sizeof me};
  HANDLE hProcessShot =
      ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
  if (hProcessShot == INVALID_HANDLE_VALUE) {
    return exe_str;
  }
  if (::Module32First(hProcessShot, &me) == NULL) {
    ::CloseHandle(hProcessShot);
    return exe_str;
  }
  ::CloseHandle(hProcessShot);
  exe_str = me.szExePath;

  return exe_str;
}

bool CLbToast::IsSameAsLastData() {
  LPCTSTR clip_str = GetClipboardText();
  if (last_str_.Compare(clip_str) == 0) {
    return true;
  }
  return false;
}

bool CLbToast::IsClipboardHasUrls() {
  CString clip_str = GetClipboardText();
  if (IsUrlMatched(clip_str)) {
    return true;
  }
  return false;
}

bool CLbToast::IsUrlMatched(CString str) {
  std::wstring w_url = str.GetBuffer(str.GetLength());
  std::wregex pattern(
      L"([hH][tT]{2}[pP]://|[hH][tT]{2}[pP][sS]://"
      L"|[wW]{3}.|[wW][aA][pP].|[fF][tT][pP].|[fF][iI][lL][eE].)[-A-Za-z0-9+&@#"
      L"/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]");
  if (std::regex_match(w_url, pattern)) {
    return true;
  }
  return false;
}

bool CLbToast::CheckPoptime() {
  long next_pop_time =
    record_->GetLongFromConfigStr(L"clipboard", L"next_pop_time");
  long long cur_time = GetCurUnixTime();

  if (cur_time >= next_pop_time) {
    record_->WriteConfig(L"clipboard", L"is_cur_user_temp_no_pop", L"false");
    record_->WriteConfig(L"clipboard", L"next_pop_time", L"0");
    record_->WriteConfig(L"clipboard", L"disappear_count", L"0");
    record_->SaveFile();
    return true;
  }
  return false;
}

void CLbToast::ShowDialog() {
  ShowWindow(SW_SHOWNOACTIVATE);
  UpdatePosition();

  KillTimer(TIMER_ID_TASK);
  SetTimer(TIMER_ID_TASK, delay_seconds_ * 1000);

  lbtoast::ReportClipbooardAction(
    action::kShowType, click::kPlaceholderType, 1, 0);
}

void CLbToast::HideDialog() {
  ShowWindow(SW_HIDE);
}

CString CLbToast::GetClipboardText() {
  CString clip_data;
  LPCTSTR clip_str = nullptr;
  BOOL result = ::OpenClipboard(m_hWnd);
  if (result == FALSE) {
    return clip_data;
  }

  HGLOBAL hData = ::GetClipboardData(CF_UNICODETEXT);
  if (hData != NULL) {
    clip_str = (LPCTSTR)GlobalLock(hData);
    clip_data = clip_str;
    GlobalUnlock(hData);
  }

  CloseClipboard();
  return clip_data;
}

void CLbToast::InitDefaultConf() {
  CString user_data_dir = KDubaPath::GetLiebaoRootInstallPath();
  KDubaPath::PathAddBackslash(user_data_dir);
  user_data_dir.Append(L"User Data\\lbtoast.dat");

  CString data_dir = KDubaPath::GetLiebaoInstallPathFromReg();
  KDubaPath::PathAddBackslash(data_dir);
  data_dir.Append(L"data\\local_lbtoast.dat");

  record_ = std::make_unique<lbtoast::ConfigRecord>(user_data_dir);
  record_data_ = std::make_unique<lbtoast::ConfigRecord>(data_dir);

  if (!record_->IsKeyExist(L"clipboard")) {
    record_->WriteConfig(L"clipboard", L"is_cur_user_temp_no_pop", L"false");
    record_->WriteConfig(L"clipboard", L"next_pop_time", L"0");
    record_->WriteConfig(L"clipboard", L"disappear_count", L"0");
    record_->SaveFile();
  }

  if (record_->IsKeyExist(L"clipboard")) {
    CString is_valid_str =
        record_data_->GetStrConfig(L"clipboard", L"is_valid");
    valid_ = (is_valid_str.Compare(L"true") == 0) ? true : false;
    disappear_counts_ =
        record_data_->GetIntConfig(L"clipboard", L"disappear_counts");
    delay_seconds_ = record_data_->GetIntConfig(L"clipboard", L"delay_seconds");
    CString tmp_str =
      record_data_->GetStrConfig(L"clipboard", L"skip_copy_from");
    tmp_str = tmp_str.MakeLower();
    std::wstring ret_str = tmp_str.GetBuffer(tmp_str.GetLength());
    conf_data_ = Split(ret_str);
  }
}

bool CLbToast::GetValueWithKey(LPCWSTR category, LPCWSTR key) {
  CString str = record_->GetStrConfig(category, key);
  if (!str.CompareNoCase(L"true")) {
    return true;
  }
  return false;
}

void CLbToast::UpdateDatas() {
  last_str_ = GetClipboardText();

  CString tmp_str = last_str_;
  if (tmp_str.GetLength() > 34) {
    tmp_str = tmp_str.Left(34);
    tmp_str.Append(L"...");
  }
  SetItemText(IDC_LINK_TEXT, tmp_str);
}

void CLbToast::UpdateCounts() {
  int disappear_cur_count =
      record_->GetIntConfig(L"clipboard", L"disappear_count");
  disappear_cur_count++;
  auto tmp_str = std::to_wstring(disappear_cur_count);
  record_->WriteConfig(L"clipboard", L"disappear_count", tmp_str.data());
  if (disappear_cur_count >= disappear_counts_) {
    long long time = GetCurUnixTime();
    time += 30 * SECONDS_PER_DAY;
    auto time_str = std::to_wstring(time);
    record_->WriteConfig(L"clipboard", L"is_cur_user_temp_no_pop", L"true");
    record_->WriteConfig(L"clipboard", L"next_pop_time", time_str.data());
  }
  record_->SaveFile();
}

void CLbToast::Start() {
  CreateEx(NULL, 0, NULL);
  ShowWindow(SW_HIDE);
  _ModalMessageLoop();
}

void CLbToast::ClearRecordData() {
  record_->WriteConfig(L"clipboard", L"disappear_count", L"0");
  record_->SaveFile();
}

long long CLbToast::GetCurUnixTime() {
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;

  return (li.QuadPart - UNIX_TIME_START) / TICKS_PER_SECOND;
}

std::vector<std::wstring> CLbToast::Split(const std::wstring& str,
                                          const char& delim /*= '$'*/) {
  std::vector<std::wstring> res;
  if (str.empty()) {
    return res;
  }

  size_t last_pos = str.find_first_not_of(delim, 0);
  size_t pos = str.find(delim, last_pos);
  while (last_pos != std::string::npos) {
    res.emplace_back(str.substr(last_pos, pos - last_pos));
    last_pos = str.find_first_not_of(delim, pos);
    pos = str.find(delim, last_pos);
  }
  return res;
}

BOOL CLbToast::CreateProcessNormally(LPCTSTR pszCmd,
                                     LPCTSTR szCommandLine,
                                     BOOL bShow) {
  if (pszCmd == NULL) {
    return FALSE;
  }

  CString strExePath;
  strExePath.Format(_T("\"%s\" %s"), pszCmd, szCommandLine);

  STARTUPINFO si = {sizeof(si)};
  PROCESS_INFORMATION pi = {};

  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = bShow ? SW_SHOW : SW_HIDE;
  si.lpDesktop = _T("Winsta0\\Default");

  BOOL bRet = ::CreateProcess(NULL, (LPTSTR)(LPCTSTR)strExePath, NULL, NULL,
                              FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
                              NULL, NULL, &si, &pi);

  if (pi.hThread) {
    ::CloseHandle(pi.hThread);
    pi.hThread = NULL;
  }
  if (pi.hProcess) {
    ::CloseHandle(pi.hProcess);
    pi.hProcess = NULL;
  }
  return bRet;
}

bool CLbToast::InitWorkThread() {
  event_ = ::CreateEventW(NULL, TRUE, FALSE, NULL);
  if (event_ == NULL) {
    return false;
  }

  work_thread_ = ::CreateThread(NULL, 0, WorkThread, (LPVOID)event_, 0, NULL);
  if (work_thread_ == NULL) {
    ::CloseHandle(event_);
    event_ = NULL;
    return false;
  }
  return true;
}

bool CLbToast::UnInitWorkThread() {
  if (event_ != NULL) {
    SetEvent(event_);

    if (work_thread_ != NULL) {
      if (::WaitForSingleObject(work_thread_, 500) != WAIT_OBJECT_0) {
        ::TerminateThread(work_thread_, 0);
      }
    }
  }

  if (work_thread_ != NULL) {
    ::CloseHandle(work_thread_);
    work_thread_ = NULL;
  }

  if (event_ != NULL) {
    ::CloseHandle(event_);
    event_ = NULL;
  }
  return true;
}

bool CLbToast::IsServiceAlive() {
  HANDLE work_mutex =
      ::OpenMutex(SYNCHRONIZE, FALSE, L"Global\\KNSERVICE Mutex");
  if (work_mutex == NULL) {
    return false;
  }
  ::CloseHandle(work_mutex);
  return true;
}

DWORD WINAPI CLbToast::WorkThread(LPVOID pParam) {
  HANDLE event = (HANDLE)pParam;

  do {
    DWORD wait = ::WaitForSingleObject(event, 30 * 1000);
    if (wait != WAIT_TIMEOUT) {
      break;
    }

    if (!IsServiceAlive()) {
      ::PostMessage(g_hwnd, WM_QUIT_APPLICATION, 0, 0);
      break;
    }
  } while (true);
  return 0;
}