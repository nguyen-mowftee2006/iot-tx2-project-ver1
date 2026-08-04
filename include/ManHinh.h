#ifndef MAN_HINH_H
#define MAN_HINH_H

#include <Arduino.h>

// Khởi tạo OLED SSD1306
bool khoiTaoManHinh();

// Hiển thị tối đa 4 dòng
void hienThiOLED(
  const String &dong1,
  const String &dong2 = "",
  const String &dong3 = "",
  const String &dong4 = ""
);

// Hiển thị màn hình chờ quét thẻ
void hienThiCho();

// Hiển thị UID đang được xử lý
void hienThiDangXuLy(
  const String &uid
);

// Hiển thị thông tin sinh viên
void hienThiSinhVien(
  const String &tieuDe,
  const String &hoTen,
  const String &mssv,
  const String &lop
);

// Hiển thị lỗi hệ thống
void hienThiLoi(
  const String &noiDung
);

#endif