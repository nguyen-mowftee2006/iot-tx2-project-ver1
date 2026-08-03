#ifndef DIEM_DANH_H
#define DIEM_DANH_H

#include <Arduino.h>

// Trạng thái Apps Script
const String TT_THANH_CONG = "DIEM_DANH_THANH_CONG";
const String TT_DA_DIEM_DANH = "DA_DIEM_DANH";
const String TT_THE_LOI = "THE_KHONG_HOP_LE";
const String TT_THIEU_UID = "THIEU_UID";
const String TT_KHONG_CO_SHEET = "KHONG_TIM_THAY_SHEET";

// Trạng thái relay
const String TT_LOI_APPS_SCRIPT = "LOI_APPS_SCRIPT";
const String TT_TIMEOUT_GOOGLE = "LOI_TIMEOUT_GOOGLE";
const String TT_LOI_GOOGLE = "LOI_KET_NOI_GOOGLE";
const String TT_LOI_JSON_GOOGLE = "LOI_JSON_APPS_SCRIPT";
const String TT_LOI_RELAY = "LOI_RELAY";
const String TT_LOI_DUONG_DAN = "LOI_DUONG_DAN";

// Lỗi ESP32
const String TT_LOI_WIFI = "LOI_WIFI";
const String TT_LOI_HTTP = "LOI_HTTP";
const String TT_LOI_SERVER = "LOI_GOI_SERVER";
const String TT_PHAN_HOI_LOI = "PHAN_HOI_KHONG_HOP_LE";

struct KetQuaDiemDanh {
  int maHTTP = 0;

  String trangThai = "";
  String uid = "";
  String mssv = "";
  String hoTen = "";
  String lop = "";
  String thongBao = "";
};

bool ketNoiWiFi();

KetQuaDiemDanh guiDiemDanh(
  const String &uid
);

void xuLyKetQua(
  const KetQuaDiemDanh &ketQua
);

#endif