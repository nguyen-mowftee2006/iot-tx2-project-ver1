#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#include "CauHinh.h"
#include "ManHinh.h"
#include "ThietBi.h"
#include "DiemDanh.h"


MFRC522 rfid(
  RFID_SS_PIN,
  RFID_RST_PIN
);


// Lấy UID từ thẻ RFID
static String layUID() {
  String uid = "";

  for (
    byte i = 0;
    i < rfid.uid.size;
    i++
  ) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(
      rfid.uid.uidByte[i],
      HEX
    );
  }

  uid.toUpperCase();

  return uid;
}


// Khởi tạo RFID RC522
static void khoiTaoRFID() {
  SPI.begin(
    SPI_SCK_PIN,
    SPI_MISO_PIN,
    SPI_MOSI_PIN,
    RFID_SS_PIN
  );

  rfid.PCD_Init();

  Serial.println(
    "Khoi tao RFID thanh cong"
  );

  Serial.println(
    "Moi quet the..."
  );
}


void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println(
    "=============================="
  );

  Serial.println(
    "HE THONG DIEM DANH RFID"
  );

  Serial.println(
    "=============================="
  );

  // OLED
  khoiTaoManHinh();

  // Buzzer, relay và servo
  khoiTaoThietBi();

  // RFID RC522
  khoiTaoRFID();

  // Kết nối Wi-Fi
  if (!ketNoiWiFi()) {
    Serial.println(
      "WiFi chua san sang"
    );

    hienThiLoi(
      "KHONG KET NOI WIFI"
    );

    keuLoiHeThong();
    delay(700);
  }

  hienThiCho();
}


void loop() {
  // Chưa phát hiện thẻ mới
  if (!rfid.PICC_IsNewCardPresent()) {
    delay(10);
    return;
  }

  // Không đọc được dữ liệu thẻ
  if (!rfid.PICC_ReadCardSerial()) {
    delay(10);
    return;
  }

  String uid = layUID();

  Serial.println();
  Serial.println(
    "=============================="
  );

  Serial.println(
    "Da phat hien the RFID"
  );

  Serial.println(
    "UID: " + uid
  );

  // Dừng giao tiếp với thẻ hiện tại
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Gửi UID tới relay.py
  KetQuaDiemDanh ketQua =
    guiDiemDanh(uid);

  // Xử lý OLED, buzzer, relay và servo
  xuLyKetQua(ketQua);

  // Tránh đọc lại cùng một thẻ ngay lập tức
  delay(TRE_DOC_LAI_THE);
}