#include "stdafx.h"
#include "lbtoast_record.h"

#include <string>

lbtoast::ConfigRecord::ConfigRecord(CString path_str) {
  path_str_ = path_str;
  if (!path_str_.IsEmpty() &&
      ini_.LoadFile(path_str_, emIniEncodingUtf8, TRUE) == TRUE) {
    is_opened_ = true;
  }
}

lbtoast::ConfigRecord::~ConfigRecord() {
}

bool lbtoast::ConfigRecord::IsFileExist() {
  if (path_str_.IsEmpty()) {
    return false;
  }

  if (::PathFileExists(path_str_) == TRUE) {
    return true;
  }
  return false;
}

void lbtoast::ConfigRecord::WriteConfig(LPCWSTR category,
                                        LPCWSTR key,
                                        LPCWSTR value) {
  if (is_opened_) {
    ini_.Write(category, key, value);
  }
}

void lbtoast::ConfigRecord::SaveFile() {
  if (is_opened_) {
    ini_.SaveFile(path_str_, emIniEncodingUtf8);
  }
}

CString lbtoast::ConfigRecord::GetStrConfig(LPCWSTR category, LPCWSTR key) {
  CString res;
  if (is_opened_) {
    res = ini_.Read(category, key, L"");
  }
  return  res;
}

int lbtoast::ConfigRecord::GetIntConfig(LPCWSTR category, LPCWSTR key) {
  int res = 0;
  if (is_opened_) {
    res = ini_.Read(category, key, 0);
  }
  return res;
}

long lbtoast::ConfigRecord::GetLongFromConfigStr(LPCWSTR category,
                                                 LPCWSTR key) {
  long res = -1;
  CString str = GetStrConfig(category, key);
  std::wstring tmpStr = str.GetBuffer(str.GetLength());
  bool is_number = true;
  for (auto ch : tmpStr) {
    if (!isdigit(ch)) {
      is_number = false;
      break;
    }
  }
  if (is_number) {
    res = std::stol(tmpStr);
  }
  return res;
}

bool lbtoast::ConfigRecord::IsKeyExist(LPCWSTR category) {
  if (is_opened_) {
    if (ini_.HasApp(category) != TRUE) {
      return false;
    }
    return true;
  }
  return false;
}