#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#define SS_PIN 5
#define RST_PIN 4

#define OLED_SDA 21
#define OLED_SCL 22
#define BUZZER_PIN 25
#define RELAY_PIN 26
#define SERVO_PIN 13

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

// Relay Wokwi kiểu NPN:
// LOW  -> COM nối NO -> cửa mở
// HIGH -> COM nối NC -> cửa đóng
const int RELAY_BAT = LOW;
const int RELAY_TAT = HIGH;

// Góc của thanh chắn barie
const int GOC_DONG = 0;
const int GOC_MO = 90;

const char *ssid = "Wokwi-GUEST";
const char *matKhau = "";

const String relayURL =
  "http://host.wokwi.internal:3000/";

MFRC522 rfid(SS_PIN, RST_PIN);

Adafruit_SSD1306 oled(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

Servo thanhChan;

bool oledSanSang = false;

// Rút gọn chuỗi để không tràn màn hình OLED
String rutGon(const String &chuoi, int doDaiToiDa) {
  if (chuoi.length() <= doDaiToiDa) {
    return chuoi;
  }

  return chuoi.substring(0, doDaiToiDa);
}

// Hiển thị tối đa bốn dòng trên OLED
void hienThiOLED(
  const String &dong1,
  const String &dong2 = "",
  const String &dong3 = "",
  const String &dong4 = ""
) {
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

// Tách họ tên thành tối đa hai dòng
void tachHoTen(
  const String &hoTen,
  String &dong1,
  String &dong2
) {
  const int doDaiToiDa = 21;

  if (hoTen.length() <= doDaiToiDa) {
    dong1 = hoTen;
    dong2 = "";
    return;
  }

  int viTriTach = hoTen.lastIndexOf(
    ' ',
    doDaiToiDa
  );

  if (viTriTach <= 0) {
    dong1 = hoTen.substring(
      0,
      doDaiToiDa
    );

    dong2 = hoTen.substring(
      doDaiToiDa
    );
  }
  else {
    dong1 = hoTen.substring(
      0,
      viTriTach
    );

    dong2 = hoTen.substring(
      viTriTach + 1
    );
  }

  dong2 = rutGon(
    dong2,
    doDaiToiDa
  );
}

// Hiển thị thông tin sinh viên
void hienThiSinhVien(
  const String &tieuDe,
  const String &hoTen,
  const String &mssv,
  const String &lop
) {
  if (!oledSanSang) {
    return;
  }

  String tenDong1;
  String tenDong2;

  tachHoTen(
    hoTen,
    tenDong1,
    tenDong2
  );

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextWrap(false);

  oled.setCursor(0, 0);
  oled.println(
    rutGon(tieuDe, 21)
  );

  oled.setCursor(0, 13);
  oled.println(
    rutGon(tenDong1, 21)
  );

  oled.setCursor(0, 25);
  oled.println(
    rutGon(tenDong2, 21)
  );

  oled.setCursor(0, 42);
  oled.println(
    "MSSV: " + rutGon(mssv, 15)
  );

  oled.setCursor(0, 54);
  oled.println(
    "LOP: " + rutGon(lop, 16)
  );

  oled.display();
}

// Màn hình chờ
void hienThiCho() {
  hienThiOLED(
    "HE THONG DIEM DANH",
    "",
    "MOI QUET THE...",
    ""
  );
}

// Phát âm báo
void keuBuzzer(
  int soLan,
  int tanSo,
  int thoiGian,
  int khoangNghi
) {
  for (int i = 0; i < soLan; i++) {
    tone(BUZZER_PIN, tanSo);
    delay(thoiGian);

    noTone(BUZZER_PIN);

    if (i < soLan - 1) {
      delay(khoangNghi);
    }
  }
}

// Quay servo từ từ để thấy rõ thanh chắn gạt lên hoặc hạ xuống
void quayServoTuTu(
  int gocBatDau,
  int gocKetThuc
) {
  if (gocBatDau < gocKetThuc) {
    for (
      int goc = gocBatDau;
      goc <= gocKetThuc;
      goc += 3
    ) {
      thanhChan.write(goc);
      delay(15);
    }
  }
  else {
    for (
      int goc = gocBatDau;
      goc >= gocKetThuc;
      goc -= 3
    ) {
      thanhChan.write(goc);
      delay(15);
    }
  }
}

// Đóng cửa và hạ thanh chắn
void dongCua() {
  thanhChan.write(GOC_DONG);

  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  Serial.println(
    "=> CUA DANG DONG"
  );
}

// Mở cửa và nâng thanh chắn
void moCua() {
  Serial.println(
    "=> DANG MO CUA"
  );

  hienThiOLED(
    "CUA DANG MO",
    "",
    "VUI LONG CHO...",
    ""
  );

  digitalWrite(
    RELAY_PIN,
    RELAY_BAT
  );

  quayServoTuTu(
    GOC_DONG,
    GOC_MO
  );

  Serial.println(
    "=> CUA DA MO"
  );

  hienThiOLED(
    "CUA DA MO",
    "",
    "VUI LONG DI QUA",
    ""
  );

  // Giữ barie mở trong 1,5 giây
  delay(1500);

  hienThiOLED(
    "CUA DANG DONG",
    "",
    "VUI LONG CHO...",
    ""
  );

  quayServoTuTu(
    GOC_MO,
    GOC_DONG
  );

  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  Serial.println(
    "=> CUA DA DONG"
  );

  delay(250);
}

// Báo điểm danh thành công
void baoThanhCong(
  const String &hoTen,
  const String &mssv,
  const String &lop
) {
  hienThiSinhVien(
    "DIEM DANH THANH CONG",
    hoTen,
    mssv,
    lop
  );

  keuBuzzer(
    1,
    1200,
    180,
    0
  );

  delay(250);

  moCua();
  hienThiCho();
}

// Báo sinh viên đã điểm danh
void baoDaDiemDanh(
  const String &hoTen,
  const String &mssv,
  const String &lop
) {
  hienThiSinhVien(
    "DA DIEM DANH",
    hoTen,
    mssv,
    lop
  );

  keuBuzzer(
    2,
    1000,
    160,
    150
  );

  delay(250);

  // UID vẫn hợp lệ nên vẫn cho phép mở cửa
  moCua();
  hienThiCho();
}

// Báo thẻ không hợp lệ
void baoTheKhongHopLe() {
  dongCua();

  hienThiOLED(
    "THE KHONG HOP LE",
    "",
    "TU CHOI TRUY CAP",
    ""
  );

  keuBuzzer(
    1,
    450,
    700,
    0
  );

  delay(700);
  hienThiCho();
}

// Báo lỗi hệ thống
void baoLoiHeThong(
  const String &noiDung
) {
  dongCua();

  hienThiOLED(
    "LOI HE THONG",
    "",
    noiDung,
    "VUI LONG THU LAI"
  );

  keuBuzzer(
    3,
    650,
    150,
    130
  );

  delay(700);
  hienThiCho();
}

// Lấy một giá trị chuỗi trong JSON
String layGiaTriJSON(
  const String &json,
  const String &khoa
) {
  String mau = "\"" + khoa + "\"";

  int viTriKhoa = json.indexOf(mau);

  if (viTriKhoa < 0) {
    return "";
  }

  int viTriHaiCham = json.indexOf(
    ':',
    viTriKhoa + mau.length()
  );

  if (viTriHaiCham < 0) {
    return "";
  }

  int batDau = viTriHaiCham + 1;

  while (
    batDau < json.length() &&
    (
      json[batDau] == ' ' ||
      json[batDau] == '\n' ||
      json[batDau] == '\r' ||
      json[batDau] == '\t'
    )
  ) {
    batDau++;
  }

  if (
    batDau < json.length() &&
    json[batDau] == '"'
  ) {
    batDau++;

    int ketThuc = batDau;

    while (ketThuc < json.length()) {
      if (
        json[ketThuc] == '"' &&
        json[ketThuc - 1] != '\\'
      ) {
        break;
      }

      ketThuc++;
    }

    if (ketThuc >= json.length()) {
      return "";
    }

    String giaTri = json.substring(
      batDau,
      ketThuc
    );

    giaTri.replace("\\\"", "\"");
    giaTri.replace("\\n", " ");

    return giaTri;
  }

  int ketThuc = batDau;

  while (
    ketThuc < json.length() &&
    json[ketThuc] != ',' &&
    json[ketThuc] != '}'
  ) {
    ketThuc++;
  }

  String giaTri = json.substring(
    batDau,
    ketThuc
  );

  giaTri.trim();

  return giaTri;
}

// Lấy UID từ thẻ RFID
String layUID() {
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

// Kết nối Wi-Fi của Wokwi
bool ketNoiWiFi() {
  Serial.print(
    "Dang ket noi WiFi"
  );

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    ssid,
    matKhau,
    6
  );

  int dem = 0;

  while (
    WiFi.status() != WL_CONNECTED &&
    dem < 30
  ) {
    delay(500);
    Serial.print(".");
    dem++;
  }

  if (
    WiFi.status() != WL_CONNECTED
  ) {
    Serial.println(
      "\nKhong ket noi duoc WiFi"
    );

    return false;
  }

  Serial.println(
    "\nKet noi WiFi thanh cong"
  );

  Serial.print(
    "Dia chi IP: "
  );

  Serial.println(
    WiFi.localIP()
  );

  return true;
}

// Xử lý phản hồi từ hệ thống
void hienThiKetQua(
  const String &ketQua
) {
  Serial.println(
    "Phan hoi tu he thong:"
  );

  Serial.println(ketQua);

  String trangThai =
    layGiaTriJSON(
      ketQua,
      "status"
    );

  String hoTen =
    layGiaTriJSON(
      ketQua,
      "hoTen"
    );

  String mssv =
    layGiaTriJSON(
      ketQua,
      "mssv"
    );

  String lop =
    layGiaTriJSON(
      ketQua,
      "lop"
    );

  if (
    trangThai ==
    "DIEM_DANH_THANH_CONG"
  ) {
    Serial.println(
      "=> DIEM DANH THANH CONG"
    );

    baoThanhCong(
      hoTen,
      mssv,
      lop
    );
  }
  else if (
    trangThai ==
    "DA_DIEM_DANH"
  ) {
    Serial.println(
      "=> DA DIEM DANH"
    );

    baoDaDiemDanh(
      hoTen,
      mssv,
      lop
    );
  }
  else if (
    trangThai ==
    "THE_KHONG_HOP_LE"
  ) {
    Serial.println(
      "=> THE KHONG HOP LE"
    );

    baoTheKhongHopLe();
  }
  else if (
    trangThai ==
    "THIEU_UID"
  ) {
    Serial.println(
      "=> THIEU UID"
    );

    baoLoiHeThong(
      "THIEU UID"
    );
  }
  else {
    Serial.println(
      "=> PHAN HOI KHONG HOP LE"
    );

    baoLoiHeThong(
      "PHAN HOI KHONG HOP LE"
    );
  }
}

// Gửi UID tới Python Relay
void guiDiemDanh(
  const String &uid
) {
  if (
    WiFi.status() != WL_CONNECTED
  ) {
    Serial.println(
      "Mat ket noi WiFi"
    );

    hienThiOLED(
      "MAT KET NOI WIFI",
      "",
      "DANG KET NOI LAI...",
      ""
    );

    if (!ketNoiWiFi()) {
      Serial.println(
        "Khong the gui UID"
      );

      baoLoiHeThong(
        "KHONG CO WIFI"
      );

      return;
    }
  }

  String url =
    relayURL + "?uid=" + uid;

  Serial.println(
    "\nDang gui UID toi Local Relay..."
  );

  Serial.println(
    "UID: " + uid
  );

  Serial.println(
    "URL: " + url
  );

  hienThiOLED(
    "DANG XU LY...",
    "",
    "UID: " + uid,
    ""
  );

  WiFiClient client;
  client.setTimeout(30000);

  HTTPClient http;
  http.setConnectTimeout(20000);
  http.setTimeout(30000);

  if (!http.begin(client, url)) {
    Serial.println(
      "Khong khoi tao duoc HTTPClient"
    );

    baoLoiHeThong(
      "LOI HTTP CLIENT"
    );

    return;
  }

  int maHTTP = http.GET();

  Serial.print(
    "Ma HTTP: "
  );

  Serial.println(maHTTP);

  if (maHTTP > 0) {
    String ketQua =
      http.getString();

    http.end();

    hienThiKetQua(ketQua);
  }
  else {
    String loi =
      http.errorToString(maHTTP);

    http.end();

    Serial.print(
      "Loi HTTP: "
    );

    Serial.println(loi);

    baoLoiHeThong(
      "KHONG GOI DUOC SERVER"
    );
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  noTone(BUZZER_PIN);

  pinMode(
    RELAY_PIN,
    OUTPUT
  );

  digitalWrite(
    RELAY_PIN,
    RELAY_TAT
  );

  // Khởi tạo servo barie
  thanhChan.setPeriodHertz(50);

  thanhChan.attach(
    SERVO_PIN,
    500,
    2400
  );

  thanhChan.write(GOC_DONG);

  delay(300);

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  oledSanSang = oled.begin(
    SSD1306_SWITCHCAPVCC,
    OLED_ADDRESS
  );

  if (!oledSanSang) {
    Serial.println(
      "Khong khoi tao duoc OLED"
    );
  }
  else {
    hienThiOLED(
      "HE THONG DIEM DANH",
      "",
      "DANG KHOI DONG...",
      ""
    );
  }

  bool wifiThanhCong =
    ketNoiWiFi();

  // SCK = 18, MISO = 19, MOSI = 23, SS = 5
  SPI.begin(
    18,
    19,
    23,
    SS_PIN
  );

  rfid.PCD_Init();
  delay(100);

  Serial.println(
    "\n=============================="
  );

  Serial.println(
    "HE THONG DIEM DANH RFID"
  );

  Serial.println(
    "=============================="
  );

  if (!wifiThanhCong) {
    hienThiOLED(
      "LOI WIFI",
      "",
      "SE THU LAI",
      "KHI QUET THE"
    );

    keuBuzzer(
      3,
      650,
      150,
      130
    );

    delay(700);
  }

  dongCua();
  hienThiCho();

  Serial.println(
    "Moi quet the..."
  );
}

void loop() {
  delay(10);

  if (
    !rfid.PICC_IsNewCardPresent()
  ) {
    return;
  }

  if (
    !rfid.PICC_ReadCardSerial()
  ) {
    return;
  }

  String uid = layUID();

  Serial.println(
    "\n------------------------------"
  );

  guiDiemDanh(uid);

  Serial.println(
    "------------------------------"
  );

  Serial.println(
    "Moi quet the..."
  );

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Chờ ngắn để tránh đọc lại cùng một thẻ
  delay(300);
}
