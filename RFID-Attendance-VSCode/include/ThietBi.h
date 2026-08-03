#ifndef THIET_BI_H
#define THIET_BI_H

#include <Arduino.h>

// Khởi tạo buzzer và relay
void khoiTaoThietBi();

// Đóng cửa
void dongCua();

// Mở cửa
void moCua();

// Phát âm báo theo thông số
void keuBuzzer(
  int soLan,
  int tanSo,
  int thoiGianKeu,
  int khoangNghi
);

// Âm báo điểm danh thành công
void keuThanhCong();

// Âm báo thẻ đã điểm danh
void keuDaDiemDanh();

// Âm cảnh báo thẻ không hợp lệ
void keuTheKhongHopLe();

// Âm báo lỗi hệ thống
void keuLoiHeThong();

#endif