#ifndef LBTOAST_POP_H
#define LBTOAST_POP_H

#define WM_HIDE_DLG WM_USER + 100
#define WM_QUIT_APPLICATION WM_USER + 101

#ifndef LB_KEY_LENGTH
#define LB_KEY_LENGTH 128
#endif  // LB_KEY_LENGTH

#include "bkwin/CBkDialogMenu.h"
#include "lbtoast_menu.h"
#include "bkwin/bkedit/IBkTimer.h"
#include "bkwin/bkshadowdrawable.h"
#include "lbtoast_record.h"

class CLbToastMenu;

class CLbToast
    : public CBkDialogViewImplEx<CLbToast>
    , public IShadowPaintHook
    , public CBkDialogMenu::IBKMenuCmdListener {
 public:
  DECLARE_WND_CLASS(L"{da0f5c14-a050-45e4-bba6-87ecc99af3d5}");
  CLbToast();
  ~CLbToast();

  enum {
    SHADOW_SIZE = 55,
  };

  enum {
    TIMER_ID_TASK = 1,
  };

  enum {
    IDC_TEX_VERSION = 1001,
    // IDC_CHECKBOX = 1002,
    IDC_BTN_MENU  = 1003,
    IDC_TEX_CHECK = 1004,
  };

  enum btn {
    IDC_BTN_CLOSE = 2002,
    IDC_BTN_SETTING = 2003,
    IDC_BTN_OK = 2004,
    IDC_CHECKBOX = 2005,
    IDC_LINK_TEXT = 2009,
  };

  enum action {
    kShowType = 1,
    kClickType,
    kActionErrorType,
  };

  enum click {
    kPlaceholderType = 0,
    kOkType,
    kCloseType,
    kNoPopType,
    kLinkTextType,
    kClickErrorType,
  };

 protected:
  BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
    BK_NOTIFY_ID_COMMAND(IDC_BTN_SETTING, OnBtnSetting)
    BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnBtnClose)
    BK_NOTIFY_ID_COMMAND(IDC_BTN_OK, OnBtnOk)
    BK_NOTIFY_ID_COMMAND(IDC_LINK_TEXT, OnLinkTextClick)
  BK_NOTIFY_MAP_END()

  BEGIN_MSG_MAP_EX(CBkDialogViewImplEx<CLbToast>)
    MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DRAWCLIPBOARD(OnDrawClipboard)
    MSG_WM_CHANGECBCHAIN(OnChangeCbChain)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_TIMER(OnTimer)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_MOUSELEAVE(OnMouseLeave)
    MESSAGE_HANDLER_EX(WM_HIDE_DLG, OnHideDialog)
    MESSAGE_HANDLER_EX(WM_QUIT_APPLICATION, OnFinishApplication)
    CHAIN_MSG_MAP(CBkDialogViewImplEx<CLbToast>)
    REFLECT_NOTIFICATIONS_EX()
  END_MSG_MAP()

  int OnCreate(LPCREATESTRUCT lpCreateStruct);
  BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
  void OnDrawClipboard();
  void OnChangeCbChain(CWindow wndRemove, CWindow wndAfter);
  void OnDestroy();
  void OnTimer(UINT_PTR nIDEvent);
  void OnMouseLeave();
  void OnMouseMove(UINT nFlags, CPoint point);
  BOOL NotifyShadowPaint(HDC hDC, CRect& rct) override;
  void OnBkMenuCmd(CBkDialogMenu* pDialogMenu, LPCWSTR lpszMenuName);
  void OnBtnSetting();
  void OnBtnClose();
  void OnBtnOk();
  void OnLinkTextClick();
  void TuneUpBrowserWithLink();
  LRESULT OnHideDialog(UINT uMsg,WPARAM wParam,LPARAM lParam);
  LRESULT OnFinishApplication(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void UpdatePosition();
  bool IsLiebaoVip();
  bool IsClipAssistantEnable();
  bool CanShow();
  //bool IsBrowserInstalled(LPCTSTR subkey);
  bool IsDatasCopyFromSkipProcess();
  std::wstring GetExePath();
  bool IsSameAsLastData();
  bool IsClipboardHasUrls();
  bool IsUrlMatched(CString str);
  bool CheckPoptime();
  void ShowDialog();
  void HideDialog();
  CString GetClipboardText();
  void InitDefaultConf();
  bool GetValueWithKey(LPCWSTR category, LPCWSTR key);
  void UpdateDatas();
  void UpdateCounts();
  void Start();
  void ClearRecordData();
  long long GetCurUnixTime();
  std::vector<std::wstring> Split(const std::wstring& str,
                                  const char& delim = '$');
  BOOL CreateProcessNormally(LPCTSTR pszCmd, LPCTSTR szCommandLine, BOOL bShow);
  bool InitWorkThread();
  bool UnInitWorkThread();
  static bool IsServiceAlive();
  static DWORD WINAPI WorkThread(LPVOID pParam);

 private:
  bool valid_ = true;
  bool is_first_ = true;
  int disappear_counts_ = 10;
  int delay_seconds_ = 6;
  CString last_str_;
  CLbToastMenu menu_;
  std::vector<std::wstring> conf_data_;
  std::unique_ptr<lbtoast::ConfigRecord> record_;
  std::unique_ptr<lbtoast::ConfigRecord> record_data_;
  HANDLE event_ = NULL;
  HANDLE work_thread_ = NULL;
  HWND hwnd_nextviewer_ = NULL;
};

#endif  // LBTOAST_POP_H