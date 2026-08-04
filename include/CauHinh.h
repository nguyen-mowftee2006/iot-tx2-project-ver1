#ifndef CAU_HINH_H
#define CAU_HINH_H

#include <Arduino.h>

// ==================== RFID RC522 ====================

constexpr uint8_t RFID_SS_PIN = 5;
constexpr uint8_t RFID_RST_PIN = 4;

constexpr uint8_t SPI_SCK_PIN = 18;
constexpr uint8_t SPI_MISO_PIN = 19;
constexpr uint8_t SPI_MOSI_PIN = 23;

// Thời gian tránh đọc lại cùng một thẻ
constexpr uint16_t TRE_DOC_LAI_THE = 1000;

// Wokwi hien thi the NFC xam voi UID 7 byte, nhung mot so phien ban
// mo phong MFRC522 chi tra ve 4 byte dau. Hai gia tri nay giup khoi phuc
// dung UID cua the truoc khi gui len API.
constexpr char UID_NFC_WOKWI_RUT_GON[] = "04112233";
constexpr char UID_NFC_WOKWI_DAY_DU[] = "04112233445566";


// ==================== OLED SSD1306 ====================

constexpr uint8_t OLED_SDA_PIN = 21;
constexpr uint8_t OLED_SCL_PIN = 22;
constexpr uint8_t OLED_DIA_CHI = 0x3C;

constexpr int OLED_RONG = 128;
constexpr int OLED_CAO = 64;


// ==================== BUZZER ====================

constexpr uint8_t BUZZER_PIN = 25;
constexpr uint8_t BUZZER_KENH_LEDC = 0;
constexpr uint8_t BUZZER_DO_PHAN_GIAI = 10;
constexpr uint16_t BUZZER_TAN_SO_KHOI_TAO = 1000;


// ==================== RELAY CỬA ====================

constexpr uint8_t RELAY_PIN = 26;

// Relay Wokwi loại NPN, kích ở mức LOW
constexpr uint8_t RELAY_BAT = LOW;
constexpr uint8_t RELAY_TAT = HIGH;

// Thời gian giữ cửa mở sau khi điểm danh thành công
constexpr uint16_t THOI_GIAN_MO_CUA = 3000;


// ==================== WIFI ====================

constexpr char WIFI_SSID[] = "Wokwi-GUEST";
constexpr char WIFI_MAT_KHAU[] = "";

constexpr uint8_t WIFI_KENH = 6;
constexpr uint8_t SO_LAN_THU_WIFI = 20;
constexpr uint16_t TRE_THU_WIFI = 400;


// ==================== LOCAL RELAY ====================

constexpr char RELAY_URL[] =
  "http://host.wokwi.internal:3000/";

// Relay có thể phải chờ Google Apps Script lâu hơn một chút.
// ESP32 cần chờ lâu hơn để tránh báo lỗi sai khi relay vẫn đang xử lý.
constexpr uint16_t HTTP_CONNECT_TIMEOUT = 10000;
constexpr uint16_t HTTP_RESPONSE_TIMEOUT = 30000;

#endif
