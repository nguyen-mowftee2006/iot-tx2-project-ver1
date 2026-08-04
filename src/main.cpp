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

static String uidLanTruoc = "";
static unsigned long thoiDiemDocLanTruoc = 0;


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


// Khoi phuc UID 7 byte cua the NFC xam trong Wokwi neu bo mo phong
// MFRC522 chi tra ve 4 byte dau.
static String khoiPhucUIDNFCWokwi(
  const String &uidDocDuoc
) {
  if (
    rfid.uid.size == 4 &&
    uidDocDuoc == UID_NFC_WOKWI_RUT_GON
  ) {
    return String(UID_NFC_WOKWI_DAY_DU);
  }

  return uidDocDuoc;
}


static bool laLanQuetLap(
  const String &uid
) {
  if (uid != uidLanTruoc) {
    return false;
  }

  return (
    millis() - thoiDiemDocLanTruoc
    < TRE_DOC_LAI_THE
  );
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

  // Buzzer và relay
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
  if (capNhatCua()) {
    hienThiCho();
  }

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

  String uidDocDuoc = layUID();
  String uid = khoiPhucUIDNFCWokwi(
    uidDocDuoc
  );

  // Dừng giao tiếp với thẻ hiện tại
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  if (laLanQuetLap(uid)) {
    // Gia hạn thời gian chờ nếu thẻ vẫn còn trên đầu đọc
    thoiDiemDocLanTruoc = millis();

    Serial.println(
      "Bo qua lan quet trung: " + uid
    );

    delay(50);
    return;
  }

  Serial.println();
  Serial.println(
    "=============================="
  );

  Serial.println(
    "Da phat hien the RFID"
  );

  if (uid != uidDocDuoc) {
    Serial.println(
      "UID MFRC522 tra ve: "
      + uidDocDuoc
    );

    Serial.println(
      "Da khoi phuc UID NFC Wokwi 7 byte"
    );
  }

  Serial.println(
    "UID: " + uid
  );

  // Gửi UID tới relay.py
  KetQuaDiemDanh ketQua =
    guiDiemDanh(uid);

  // Xử lý OLED, buzzer và relay
  xuLyKetQua(ketQua);

  uidLanTruoc = uid;
  thoiDiemDocLanTruoc = millis();
}
