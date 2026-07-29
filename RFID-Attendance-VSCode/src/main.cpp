#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#define SS_PIN 5
#define RST_PIN 4

const char *ssid = "Wokwi-GUEST";
const char *matKhau = "";

// Máy chủ Python đang chạy trên máy Linux
const String relayURL =
  "http://host.wokwi.internal:3000/";

MFRC522 rfid(SS_PIN, RST_PIN);

// Lấy UID từ thẻ RFID
String layUID() {
  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();
  return uid;
}

// Kết nối Wi-Fi của Wokwi
bool ketNoiWiFi() {
  Serial.print("Dang ket noi WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, matKhau, 6);

  int dem = 0;

  while (WiFi.status() != WL_CONNECTED && dem < 30) {
    delay(500);
    Serial.print(".");
    dem++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nKhong ket noi duoc WiFi");
    return false;
  }

  Serial.println("\nKet noi WiFi thanh cong");

  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());

  return true;
}

// Hiển thị kết quả trả về
void hienThiKetQua(const String &ketQua) {
  Serial.println("Phan hoi tu he thong:");
  Serial.println(ketQua);

  if (
    ketQua.indexOf(
      "\"status\":\"DIEM_DANH_THANH_CONG\""
    ) >= 0
  ) {
    Serial.println("=> DIEM DANH THANH CONG");
  }
  else if (
    ketQua.indexOf(
      "\"status\":\"DA_DIEM_DANH\""
    ) >= 0
  ) {
    Serial.println("=> DA DIEM DANH");
  }
  else if (
    ketQua.indexOf(
      "\"status\":\"THE_KHONG_HOP_LE\""
    ) >= 0
  ) {
    Serial.println("=> THE KHONG HOP LE");
  }
  else if (
    ketQua.indexOf(
      "\"status\":\"THIEU_UID\""
    ) >= 0
  ) {
    Serial.println("=> THIEU UID");
  }
  else {
    Serial.println("=> KHONG XAC DINH DUOC KET QUA");
  }
}

// Gửi UID tới Python Relay
void guiDiemDanh(const String &uid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Mat ket noi WiFi");

    if (!ketNoiWiFi()) {
      Serial.println("Khong the gui UID");
      return;
    }
  }

  String url = relayURL + "?uid=" + uid;

  Serial.println("\nDang gui UID toi Local Relay...");
  Serial.println("UID: " + uid);
  Serial.println("URL: " + url);

  WiFiClient client;
  client.setTimeout(30000);

  HTTPClient http;
  http.setConnectTimeout(20000);
  http.setTimeout(30000);

  if (!http.begin(client, url)) {
    Serial.println("Khong khoi tao duoc HTTPClient");
    return;
  }

  int maHTTP = http.GET();

  Serial.print("Ma HTTP: ");
  Serial.println(maHTTP);

  if (maHTTP > 0) {
    String ketQua = http.getString();
    hienThiKetQua(ketQua);
  }
  else {
    Serial.print("Loi HTTP: ");
    Serial.println(http.errorToString(maHTTP));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  ketNoiWiFi();

  // SCK = 18, MISO = 19, MOSI = 23, SS = 5
  SPI.begin(18, 19, 23, SS_PIN);

  rfid.PCD_Init();
  delay(100);

  Serial.println("\n==============================");
  Serial.println("HE THONG DIEM DANH RFID");
  Serial.println("==============================");
  Serial.println("Moi quet the...");
}

void loop() {
  delay(10);

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uid = layUID();

  Serial.println("\n------------------------------");
  guiDiemDanh(uid);
  Serial.println("------------------------------");
  Serial.println("Moi quet the...");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1500);
}