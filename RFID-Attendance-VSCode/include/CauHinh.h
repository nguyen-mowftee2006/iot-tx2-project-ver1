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
constexpr uint16_t TRE_DOC_LAI_THE = 300;


// ==================== OLED SSD1306 ====================

constexpr uint8_t OLED_SDA_PIN = 21;
constexpr uint8_t OLED_SCL_PIN = 22;
constexpr uint8_t OLED_DIA_CHI = 0x3C;

constexpr int OLED_RONG = 128;
constexpr int OLED_CAO = 64;


// ==================== BUZZER ====================

constexpr uint8_t BUZZER_PIN = 25;


// ==================== RELAY CỬA ====================

constexpr uint8_t RELAY_PIN = 26;

// Relay Wokwi loại NPN, kích ở mức LOW
constexpr uint8_t RELAY_BAT = LOW;
constexpr uint8_t RELAY_TAT = HIGH;


// ==================== SERVO BARIE ====================

// Khôi phục lại cấu hình servo từ phiên bản trước để sát với môi trường hoạt động cũ.
constexpr uint8_t SERVO_PIN = 13;

constexpr int GOC_DONG = 0;
constexpr int GOC_MO = 90;

constexpr int TAN_SO_SERVO = 50;
constexpr int XUNG_SERVO_MIN = 500;
constexpr int XUNG_SERVO_MAX = 2400;

constexpr int BUOC_SERVO = 3;
constexpr int TRE_SERVO = 15;

// Thời gian giữ barie mở
constexpr uint16_t THOI_GIAN_MO_CUA = 1500;


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