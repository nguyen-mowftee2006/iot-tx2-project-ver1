#include <Arduino.h>

#include "CauHinh.h"
#include "ThietBi.h"


// Khởi tạo buzzer và relay
void khoiTaoThietBi() {
  // Buzzer
  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  // Không gọi noTone() lúc khởi động
  // để tránh cảnh báo LEDC
  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  // Relay
  pinMode(
    RELAY_PIN,
    OUTPUT
  );

  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  Serial.println(
    "Khoi tao relay va buzzer thanh cong"
  );
}


// Đóng cửa
void dongCua() {
  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );
}


// Mở cửa
void moCua() {
  digitalWrite(
    RELAY_PIN,
    RELAY_BAT
  );
}


// Phát âm báo
void keuBuzzer(
  int soLan,
  int tanSo,
  int thoiGianKeu,
  int khoangNghi
) {
  for (int i = 0; i < soLan; i++) {
    tone(
      BUZZER_PIN,
      tanSo
    );

    delay(
      thoiGianKeu
    );

    noTone(
      BUZZER_PIN
    );

    if (i < soLan - 1) {
      delay(
        khoangNghi
      );
    }
  }
}


// Âm báo điểm danh thành công
void keuThanhCong() {
  keuBuzzer(
    1,
    1200,
    180,
    0
  );
}


// Âm báo đã điểm danh
void keuDaDiemDanh() {
  keuBuzzer(
    2,
    1000,
    160,
    150
  );
}


// Âm cảnh báo thẻ không hợp lệ
void keuTheKhongHopLe() {
  keuBuzzer(
    1,
    450,
    700,
    0
  );
}


// Âm báo lỗi hệ thống
void keuLoiHeThong() {
  keuBuzzer(
    3,
    650,
    150,
    130
  );
}