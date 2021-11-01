#include "stdafx.h"
#include "lbtoast_menu.h"
#include "lbtoast_record.h"

#include <functional>
#include "bkres/bkres.h"
#include "bkwin/CBkDialogViewImplEx.h"

#include "publish/framework/KRegister2.h"

CLbToastMenu::CLbToastMenu()
    : CBkDialogViewEx(IDR_DLG_SETTING) {
  SetDrawBorder(FALSE);
  EnableShadow(FALSE);
}

void CLbToastMenu::PopUp(HWND hWnd, CPoint pt) {
  m_hParentWnd = hWnd;
  if (!IsWindow()) {
    CreateEx(hWnd);
  }

  MoveWindow(pt.x, pt.y, 70, 36, FALSE);
  SetWindowPos(hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
  ::SetForegroundWindow(m_hWnd);
  SendMessage(WM_PAINT);
}

void CLbToastMenu::OnListItemMouseHover(int nListItem) {
  SetListItemChildCtrlVisible(
    IDC_LIST_CTRL, nListItem, IDC_MENU_HOVER_BG, TRUE);
}

void CLbToastMenu::OnListItemMouseLeave(int nListItem) {
  SetListItemChildCtrlVisible(
    IDC_LIST_CTRL, nListItem, IDC_MENU_HOVER_BG, FALSE);
}

void CLbToastMenu::OnListItemLButtonUp(int nListItem) {
  SetClipboardNoPop();
  ShowWindow(SW_HIDE);
  ::SendMessage(m_hParentWnd, WM_HIDE_DLG, 0, 0);
  lbtoast::ReportClipbooardAction(
    action::kClickType, click::kNoPopType, 1, 0);
}

BOOL CLbToastMenu::OnInitDialog(CWindow wndFocus, LPARAM lInitParam) {
  return TRUE;
}

BOOL CLbToastMenu::OnNcActivate(BOOL bActive) {
  SetMsgHandled(FALSE);
  if (!bActive) {
    ShowWindow(SW_HIDE);
  }

  return TRUE;
}

void CLbToastMenu::OnDestroy() {
  SetMsgHandled(FALSE);
}

void CLbToastMenu::OnHideMenu() {
  ShowWindow(SW_HIDE);
}

void CLbToastMenu::SetClipboardNoPop() {
  CRegKey reg;
  LONG ret = reg.Open(HKEY_CURRENT_USER, L"SOFTWARE\\liebao", KEY_SET_VALUE);
  if (ret != ERROR_SUCCESS) {
    return;
  }

  reg.SetDWORDValue(L"clipassistant", 0);
}

bool CLbToastMenu::IsShowing() {
  if (IsWindow() == NULL) {
    return false;
  }
  return true;
}
