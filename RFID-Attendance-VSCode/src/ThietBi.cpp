#include <Arduino.h>
#include <ESP32Servo.h>

#include "CauHinh.h"
#include "ThietBi.h"


static Servo thanhChan;

static bool servoSanSang = false;
static int gocHienTai = GOC_DONG;


static void quayServoTuTu(
  int gocBatDau,
  int gocKetThuc
) {
  if (!servoSanSang) {
    return;
  }

  thanhChan.write(gocKetThuc);
  gocHienTai = gocKetThuc;
}


static bool khoiTaoServo() {
  const uint8_t cacPin[] = {
    SERVO_PIN,
    27,
    14
  };

  for (uint8_t i = 0; i < sizeof(cacPin) / sizeof(cacPin[0]); i++) {
    int kenhServo = thanhChan.attach(
      cacPin[i],
      XUNG_SERVO_MIN,
      XUNG_SERVO_MAX
    );

    if (kenhServo >= 0 && thanhChan.attached()) {
      servoSanSang = true;
      thanhChan.write(GOC_DONG);
      gocHienTai = GOC_DONG;

      Serial.print("Khoi tao servo thanh cong tren pin ");
      Serial.print(cacPin[i]);
      Serial.print(" (channel ");
      Serial.print(kenhServo);
      Serial.println(")");

      return true;
    }

    Serial.print("Servo pin ");
    Serial.print(cacPin[i]);
    Serial.println(" khong hoat dong");
  }

  servoSanSang = false;
  Serial.println("Khong khoi tao duoc servo");
  return false;
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

  thanhChan.setPeriodHertz(TAN_SO_SERVO);
  khoiTaoServo();

  if (servoSanSang) {
    thanhChan.write(GOC_DONG);
    gocHienTai = GOC_DONG;
  }

  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  Serial.println(
    "Khoi tao relay va buzzer thanh cong"
  );
}


// Đóng cửa và hạ thanh chắn
void dongCua() {
  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  if (servoSanSang && gocHienTai != GOC_DONG) {
    quayServoTuTu(gocHienTai, GOC_DONG);
  }
}


// Mở cửa và giữ ở trạng thái mở
void moCua() {
  digitalWrite(
    RELAY_PIN,
    RELAY_BAT
  );

  if (servoSanSang && gocHienTai != GOC_MO) {
    quayServoTuTu(gocHienTai, GOC_MO);
  }
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