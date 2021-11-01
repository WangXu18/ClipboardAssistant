#ifndef LBTOAST_MENU_H
#define LBTOAST_MENU_H

#define WM_HIDE_DLG WM_USER + 100

class CLbToastMenu : public CBkDialogViewEx {
 public:
  CLbToastMenu();
  void PopUp(HWND hwnd, CPoint pt);
  void OnHideMenu();
  bool IsShowing();

  enum {
    IDC_LIST_CTRL = 1000,
    IDC_MENE_ITEM_1 = 1010,
    IDC_MENU_NORMAL_BG = 1011,
    IDC_MENU_HOVER_BG = 1012,
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
    kClickErrorType,
  };

 public:
  BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
  BK_LISTWND_NOTIFY_BEGIN(IDC_LIST_CTRL)
  BK_LISTWND_LISTITEM_MOUSEHOVER(OnListItemMouseHover)
  BK_LISTWND_LISTITEM_MOUSELEAVE(OnListItemMouseLeave)
  BK_LISTWND_LISTITEM_LBUTTONUP(OnListItemLButtonUp)
  BK_LISTWND_NOTIFY_END()
  BK_NOTIFY_MAP_END()

  BEGIN_MSG_MAP_EX(CLbToastMenu)
    MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_NCACTIVATE(OnNcActivate)
    MSG_WM_DESTROY(OnDestroy)
    CHAIN_MSG_MAP(CBkDialogViewEx)
  END_MSG_MAP()

 protected:
  void OnListItemMouseHover(int nListItem);
  void OnListItemMouseLeave(int nListItem);
  void OnListItemLButtonUp(int nListItem);
  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
  BOOL OnNcActivate(BOOL bActive);
  void OnDestroy();
  void SetClipboardNoPop();

 private:
  HWND m_hParentWnd;
};


#endif  // LBTOAST_MENU_H