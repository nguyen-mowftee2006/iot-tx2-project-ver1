#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "CauHinh.h"
#include "ManHinh.h"

Adafruit_SSD1306 oled(OLED_RONG, OLED_CAO, &Wire, -1);

static bool oledSanSang = false;

// Rút gọn chuỗi để không tràn OLED
static String rutGon(const String &chuoi, int doDaiToiDa) {
  if (chuoi.length() <= doDaiToiDa) {
    return chuoi;
  }

  return chuoi.substring(0, doDaiToiDa);
}

// Tách họ tên thành tối đa hai dòng
static void tachHoTen(const String &hoTen, String &dong1, String &dong2) {
  const int doDaiToiDa = 21;

  if (hoTen.length() <= doDaiToiDa) {
    dong1 = hoTen;
    dong2 = "";
    return;
  }

  int viTriTach = hoTen.lastIndexOf(' ', doDaiToiDa);

  if (viTriTach <= 0) {
    dong1 = hoTen.substring(0, doDaiToiDa);

    dong2 = hoTen.substring(doDaiToiDa);
  } else {
    dong1 = hoTen.substring(0, viTriTach);

    dong2 = hoTen.substring(viTriTach + 1);
  }

  dong2 = rutGon(dong2, doDaiToiDa);
}

// Khởi tạo OLED
bool khoiTaoManHinh() {
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  oledSanSang = oled.begin(SSD1306_SWITCHCAPVCC, OLED_DIA_CHI);

  if (!oledSanSang) {
    Serial.println("Khong khoi tao duoc OLED");

    return false;
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextWrap(false);
  oled.display();

  Serial.println("Khoi tao OLED thanh cong");

  hienThiOLED("HE THONG RFID", "", "DANG KHOI DONG...", "");

  return true;
}

// Hiển thị tối đa bốn dòng
void hienThiOLED(const String &dong1, const String &dong2, const String &dong3,
                 const String &dong4) {
  if (!oledSanSang) {
    return;
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextWrap(false);

  oled.setCursor(0, 0);
  oled.println(rutGon(dong1, 21));

  oled.setCursor(0, 16);
  oled.println(rutGon(dong2, 21));

  oled.setCursor(0, 32);
  oled.println(rutGon(dong3, 21));

  oled.setCursor(0, 48);
  oled.println(rutGon(dong4, 21));

  oled.display();
}

// Màn hình chờ
void hienThiCho() { hienThiOLED("HE THONG DIEM DANH", "", "MOI QUET THE...", ""); }

// Hiển thị UID đang xử lý
void hienThiDangXuLy(const String &uid) { hienThiOLED("DANG XU LY...", "", "UID:", uid); }

// Hiển thị thông tin sinh viên
void hienThiSinhVien(const String &tieuDe, const String &hoTen, const String &mssv,
                     const String &lop) {
  if (!oledSanSang) {
    return;
  }

  String tenDong1;
  String tenDong2;

  tachHoTen(hoTen, tenDong1, tenDong2);

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextWrap(false);

  oled.setCursor(0, 0);
  oled.println(rutGon(tieuDe, 21));

  oled.setCursor(0, 13);
  oled.println(rutGon(tenDong1, 21));

  oled.setCursor(0, 25);
  oled.println(rutGon(tenDong2, 21));

  oled.setCursor(0, 42);
  oled.println("MSSV: " + rutGon(mssv, 15));

  oled.setCursor(0, 54);
  oled.println("LOP: " + rutGon(lop, 16));

  oled.display();
}

// Hiển thị lỗi hệ thống
void hienThiLoi(const String &noiDung) {
  hienThiOLED("LOI HE THONG", "", noiDung, "VUI LONG THU LAI");
}
