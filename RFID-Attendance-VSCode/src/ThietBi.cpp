#include <Arduino.h>

#include "CauHinh.h"
#include "ThietBi.h"


static bool cuaDangMo = false;
static unsigned long thoiDiemMoCua = 0;
static bool buzzerSanSang = false;


// Khởi tạo buzzer và relay
void khoiTaoThietBi() {
  // Đặt relay về trạng thái đóng trước khi bật chân OUTPUT
  // để hạn chế relay nháy lúc ESP32 khởi động.
  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  pinMode(
    RELAY_PIN,
    OUTPUT
  );

  // Khởi tạo timer LEDC trước rồi mới gắn chân buzzer.
  // Thứ tự này tránh cảnh báo "LEDC is not initialized".
  buzzerSanSang = ledcSetup(
    BUZZER_KENH_LEDC,
    BUZZER_TAN_SO_KHOI_TAO,
    BUZZER_DO_PHAN_GIAI
  ) > 0;

  if (buzzerSanSang) {
    ledcAttachPin(
      BUZZER_PIN,
      BUZZER_KENH_LEDC
    );

    ledcWrite(
      BUZZER_KENH_LEDC,
      0
    );
  }
  else {
    Serial.println(
      "Khong khoi tao duoc buzzer"
    );
  }

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

  cuaDangMo = false;
}


// Mở cửa
void moCua() {
  digitalWrite(
    RELAY_PIN,
    RELAY_BAT
  );

  cuaDangMo = true;
  thoiDiemMoCua = millis();
}


// Kiểm tra và tự động đóng cửa mà không chặn chương trình
bool capNhatCua() {
  if (!cuaDangMo) {
    return false;
  }

  if (
    millis() - thoiDiemMoCua
    < THOI_GIAN_MO_CUA
  ) {
    return false;
  }

  dongCua();
  return true;
}


// Phát âm báo
void keuBuzzer(
  int soLan,
  int tanSo,
  int thoiGianKeu,
  int khoangNghi
) {
  if (!buzzerSanSang) {
    return;
  }

  for (int i = 0; i < soLan; i++) {
    ledcWriteTone(
      BUZZER_KENH_LEDC,
      tanSo
    );

    delay(
      thoiGianKeu
    );

    ledcWriteTone(
      BUZZER_KENH_LEDC,
      0
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
