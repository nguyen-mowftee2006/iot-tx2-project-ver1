#include <Arduino.h>
#include <ESP32Servo.h>

#include "CauHinh.h"
#include "ThietBi.h"


static Servo thanhChan;

static bool servoSanSang = false;
static int gocHienTai = GOC_DONG;


// Quay servo từ từ
static void quayServoTuTu(
  int gocBatDau,
  int gocKetThuc
) {
  if (!servoSanSang) {
    return;
  }

  if (gocBatDau < gocKetThuc) {
    for (
      int goc = gocBatDau;
      goc <= gocKetThuc;
      goc += BUOC_SERVO
    ) {
      thanhChan.write(goc);
      delay(TRE_SERVO);
    }
  }
  else {
    for (
      int goc = gocBatDau;
      goc >= gocKetThuc;
      goc -= BUOC_SERVO
    ) {
      thanhChan.write(goc);
      delay(TRE_SERVO);
    }
  }

  thanhChan.write(gocKetThuc);
  gocHienTai = gocKetThuc;
}


// Khởi tạo buzzer, relay và servo
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

  // Servo
  thanhChan.setPeriodHertz(
    TAN_SO_SERVO
  );

  int kenhServo = thanhChan.attach(
    SERVO_PIN,
    XUNG_SERVO_MIN,
    XUNG_SERVO_MAX
  );

  if (kenhServo > 0) {
    servoSanSang = true;

    thanhChan.write(
      GOC_DONG
    );

    gocHienTai = GOC_DONG;

    Serial.println(
      "Khoi tao servo thanh cong"
    );
  }
  else {
    servoSanSang = false;

    Serial.println(
      "Khong khoi tao duoc servo"
    );
  }

  Serial.println(
    "Khoi tao relay va buzzer thanh cong"
  );

  Serial.println(
    "=> CUA DANG DONG"
  );
}


// Đóng cửa và hạ thanh chắn
void dongCua() {
  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  if (
    servoSanSang &&
    gocHienTai != GOC_DONG
  ) {
    quayServoTuTu(
      gocHienTai,
      GOC_DONG
    );
  }

  Serial.println(
    "=> CUA DA DONG"
  );
}


// Mở cửa rồi tự đóng lại
void moCua() {
  Serial.println(
    "=> DANG MO CUA"
  );

  digitalWrite(
    RELAY_PIN,
    RELAY_BAT
  );

  if (
    servoSanSang &&
    gocHienTai != GOC_MO
  ) {
    quayServoTuTu(
      gocHienTai,
      GOC_MO
    );
  }

  Serial.println(
    "=> CUA DA MO"
  );

  delay(
    THOI_GIAN_MO_CUA
  );

  Serial.println(
    "=> DANG DONG CUA"
  );

  if (
    servoSanSang &&
    gocHienTai != GOC_DONG
  ) {
    quayServoTuTu(
      gocHienTai,
      GOC_DONG
    );
  }

  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  Serial.println(
    "=> CUA DA DONG"
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