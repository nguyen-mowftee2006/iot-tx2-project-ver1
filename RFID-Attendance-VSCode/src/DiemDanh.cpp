#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#include "CauHinh.h"
#include "DiemDanh.h"
#include "ManHinh.h"
#include "ThietBi.h"


// Chuẩn hóa UID trước khi gửi
static String chuanHoaUID(String uid) {
  uid.replace(":", "");
  uid.replace(" ", "");
  uid.trim();
  uid.toUpperCase();

  return uid;
}


// Lấy một giá trị trong chuỗi JSON
static String layGiaTriJSON(
  const String &json,
  const String &khoa
) {
  String mau = "\"" + khoa + "\"";

  int viTriKhoa = json.indexOf(mau);

  if (viTriKhoa < 0) {
    return "";
  }

  int viTriHaiCham = json.indexOf(
    ':',
    viTriKhoa + mau.length()
  );

  if (viTriHaiCham < 0) {
    return "";
  }

  int batDau = viTriHaiCham + 1;

  // Bỏ khoảng trắng
  while (
    batDau < json.length() &&
    (
      json[batDau] == ' ' ||
      json[batDau] == '\n' ||
      json[batDau] == '\r' ||
      json[batDau] == '\t'
    )
  ) {
    batDau++;
  }

  // Giá trị dạng chuỗi
  if (
    batDau < json.length() &&
    json[batDau] == '"'
  ) {
    batDau++;

    int ketThuc = batDau;

    while (ketThuc < json.length()) {
      if (
        json[ketThuc] == '"' &&
        json[ketThuc - 1] != '\\'
      ) {
        break;
      }

      ketThuc++;
    }

    if (ketThuc >= json.length()) {
      return "";
    }

    String giaTri = json.substring(
      batDau,
      ketThuc
    );

    giaTri.replace("\\\"", "\"");
    giaTri.replace("\\n", " ");
    giaTri.replace("\\r", "");
    giaTri.replace("\\\\", "\\");

    return giaTri;
  }

  // Giá trị không phải chuỗi
  int ketThuc = batDau;

  while (
    ketThuc < json.length() &&
    json[ketThuc] != ',' &&
    json[ketThuc] != '}'
  ) {
    ketThuc++;
  }

  String giaTri = json.substring(
    batDau,
    ketThuc
  );

  giaTri.trim();

  return giaTri;
}


// Đổi trạng thái lỗi thành nội dung hiển thị
static String layTenLoi(
  const String &trangThai
) {
  if (trangThai == TT_LOI_WIFI)
    return "MAT KET NOI WIFI";

  if (trangThai == TT_LOI_HTTP)
    return "LOI HTTP CLIENT";

  if (trangThai == TT_LOI_SERVER)
    return "KHONG GOI DUOC RELAY";

  if (trangThai == TT_LOI_APPS_SCRIPT)
    return "LOI APPS SCRIPT";

  if (trangThai == TT_TIMEOUT_GOOGLE)
    return "GOOGLE TIMEOUT";

  if (trangThai == TT_LOI_GOOGLE)
    return "LOI KET NOI GOOGLE";

  if (trangThai == TT_LOI_JSON_GOOGLE)
    return "APPS SCRIPT SAI JSON";

  if (trangThai == TT_LOI_RELAY)
    return "LOI LOCAL RELAY";

  if (trangThai == TT_LOI_DUONG_DAN)
    return "SAI DUONG DAN RELAY";

  if (trangThai == TT_KHONG_CO_SHEET)
    return "KHONG TIM THAY SHEET";

  if (trangThai == TT_THIEU_UID)
    return "THIEU UID";

  return "PHAN HOI KHONG HOP LE";
}


// Báo lỗi hệ thống
static void baoLoi(
  const String &noiDung
) {
  dongCua();

  hienThiLoi(
    noiDung
  );

  keuLoiHeThong();

  delay(700);
  hienThiCho();
}


// Kết nối Wi-Fi
bool ketNoiWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.print(
    "Dang ket noi WiFi"
  );

  hienThiOLED(
    "DANG KET NOI WIFI",
    "",
    "VUI LONG CHO...",
    ""
  );

  WiFi.mode(
    WIFI_STA
  );

  WiFi.begin(
    WIFI_SSID,
    WIFI_MAT_KHAU,
    WIFI_KENH
  );

  for (
    int i = 0;
    i < SO_LAN_THU_WIFI;
    i++
  ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(
        "\nKet noi WiFi thanh cong"
      );

      Serial.print(
        "IP: "
      );

      Serial.println(
        WiFi.localIP()
      );

      return true;
    }

    Serial.print(".");
    delay(TRE_THU_WIFI);
  }

  Serial.println(
    "\nKhong ket noi duoc WiFi"
  );

  return false;
}


// Gửi UID tới relay.py
KetQuaDiemDanh guiDiemDanh(
  const String &uid
) {
  KetQuaDiemDanh ketQua;

  String uidChuan =
    chuanHoaUID(uid);

  if (uidChuan == "") {
    ketQua.trangThai =
      TT_THIEU_UID;

    return ketQua;
  }

  if (
    WiFi.status() != WL_CONNECTED &&
    !ketNoiWiFi()
  ) {
    ketQua.trangThai =
      TT_LOI_WIFI;

    return ketQua;
  }

  String url =
    String(RELAY_URL)
    + "?uid="
    + uidChuan;

  Serial.println(
    "\n=============================="
  );

  Serial.println(
    "Dang gui UID toi relay"
  );

  Serial.println(
    "UID: " + uidChuan
  );

  Serial.println(
    "URL: " + url
  );

  hienThiDangXuLy(
    uidChuan
  );

  WiFiClient client;
  HTTPClient http;

  client.setTimeout(
    HTTP_RESPONSE_TIMEOUT
  );

  http.setConnectTimeout(
    HTTP_CONNECT_TIMEOUT
  );

  http.setTimeout(
    HTTP_RESPONSE_TIMEOUT
  );

  http.setReuse(false);

  if (!http.begin(client, url)) {
    ketQua.trangThai =
      TT_LOI_HTTP;

    ketQua.thongBao =
      "http.begin that bai";

    return ketQua;
  }

  int maHTTP = http.GET();

  ketQua.maHTTP = maHTTP;

  Serial.print(
    "Ma HTTP: "
  );

  Serial.println(
    maHTTP
  );

  if (maHTTP <= 0) {
    ketQua.trangThai =
      TT_LOI_SERVER;

    ketQua.thongBao =
      http.errorToString(maHTTP);

    Serial.println(
      "Loi HTTP: "
      + ketQua.thongBao
    );

    http.end();

    return ketQua;
  }

  String noiDung =
    http.getString();

  http.end();

  Serial.println(
    "Phan hoi relay:"
  );

  Serial.println(
    noiDung
  );

  // Tách trực tiếp chuỗi JSON
  ketQua.trangThai =
    layGiaTriJSON(
      noiDung,
      "status"
    );

  ketQua.uid =
    layGiaTriJSON(
      noiDung,
      "uid"
    );

  ketQua.mssv =
    layGiaTriJSON(
      noiDung,
      "mssv"
    );

  ketQua.hoTen =
    layGiaTriJSON(
      noiDung,
      "hoTen"
    );

  ketQua.lop =
    layGiaTriJSON(
      noiDung,
      "lop"
    );

  ketQua.thongBao =
    layGiaTriJSON(
      noiDung,
      "message"
    );

  if (ketQua.trangThai == "") {
    ketQua.trangThai =
      TT_PHAN_HOI_LOI;

    ketQua.thongBao =
      "Khong tim thay status";
  }

  Serial.println(
    "Trang thai: "
    + ketQua.trangThai
  );

  Serial.println(
    "=============================="
  );

  return ketQua;
}


// Xử lý kết quả điểm danh
void xuLyKetQua(
  const KetQuaDiemDanh &ketQua
) {
  const String &trangThai =
    ketQua.trangThai;

  // Điểm danh thành công
  if (trangThai == TT_THANH_CONG) {
    Serial.println(
      "=> DIEM DANH THANH CONG"
    );

    hienThiSinhVien(
      "DIEM DANH THANH CONG",
      ketQua.hoTen,
      ketQua.mssv,
      ketQua.lop
    );

    keuThanhCong();
    delay(250);

    hienThiOLED(
      "CUA DANG MO",
      "",
      "VUI LONG CHO...",
      ""
    );

    moCua();
    hienThiCho();

    return;
  }

  // Đã điểm danh nhưng vẫn là thẻ hợp lệ
  if (trangThai == TT_DA_DIEM_DANH) {
    Serial.println(
      "=> DA DIEM DANH"
    );

    hienThiSinhVien(
      "DA DIEM DANH",
      ketQua.hoTen,
      ketQua.mssv,
      ketQua.lop
    );

    keuDaDiemDanh();
    delay(250);

    hienThiOLED(
      "CUA DANG MO",
      "",
      "VUI LONG CHO...",
      ""
    );

    moCua();
    hienThiCho();

    return;
  }

  // Thẻ không tồn tại
  if (trangThai == TT_THE_LOI) {
    Serial.println(
      "=> THE KHONG HOP LE"
    );

    dongCua();

    hienThiOLED(
      "THE KHONG HOP LE",
      "",
      "TU CHOI TRUY CAP",
      ""
    );

    keuTheKhongHopLe();

    delay(700);
    hienThiCho();

    return;
  }

  // Các trạng thái lỗi
  String tenLoi =
    layTenLoi(
      trangThai
    );

  Serial.println(
    "=> LOI: "
    + tenLoi
  );

  if (ketQua.thongBao != "") {
    Serial.println(
      "Chi tiet: "
      + ketQua.thongBao
    );
  }

  baoLoi(
    tenLoi
  );
}