#ifndef LBTOAST_RECORD_H
#define LBTOAST_RECORD_H

#include "ksmodule/include/infoc2/KInfocNoKfmtFileWrap.h"
#include "ksmodule/publish/common/KIniWrapEx.h"

namespace lbtoast {
namespace {
#define KEY_LENGTH 128
}  // namespace

static void ReportClipbooardAction(int poptype, int click, int ver, int ext1) {
  KInfocNoKfmtFileWrap infoc;
  infoc.SetKfmt(
      L"liebao_clipboard:544 poptype:byte click:byte ver:byte ext1:byte");
  infoc.AddInt(L"poptype", poptype);
  infoc.AddInt(L"click", click);
  infoc.AddInt(L"ver", ver);
  infoc.AddInt(L"ext1", ext1);
  infoc.WriteCache();
}

class ConfigRecord {
 public:
  explicit ConfigRecord(CString path_str);
  ~ConfigRecord();

  void WriteConfig(LPCWSTR category, LPCWSTR key, LPCWSTR value);
  void SaveFile();
  CString GetStrConfig(LPCWSTR category, LPCWSTR key);
  int GetIntConfig(LPCWSTR category, LPCWSTR key);
  long GetLongFromConfigStr(LPCWSTR category, LPCWSTR key);
  bool IsKeyExist(LPCWSTR category);

 private:
  bool IsFileExist();

 private:
  bool is_opened_ = false;
  CString path_str_;
  KIniWrapEx ini_;
};
}

#endif  // LBTOAST_RECORD_H
