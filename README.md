# Hệ thống điểm danh IoT bằng RFID

Dự án nhập môn IoT: sinh viên quét thẻ RFID để điểm danh, giáo viên dùng thẻ riêng để mở/đóng phiên học. Mô phỏng trên Wokwi (ESP32 + RC522 + OLED + buzzer + relay), dữ liệu lưu trên Google Sheets qua Google Apps Script.

## Cách hoạt động

1. Giáo viên quét thẻ → mở phiên điểm danh mới.
2. Sinh viên quét thẻ trong lúc phiên đang mở → được ghi nhận (mỗi người 1 lần/phiên).
3. Giáo viên quét lại → đóng phiên, ngừng nhận điểm danh.
4. Kết quả hiển thị qua OLED + buzzer + relay/LED (giả lập mở cửa), đồng thời ghi vào Google Sheets và cập nhật Dashboard.

## Kiến trúc

```
Thẻ RFID → RC522 → ESP32 → relay.py (Python, local)
                              ↓ HTTPS
                    Google Apps Script → Google Sheets (data + Dashboard)
```

- **ESP32**: đọc UID qua SPI, hiển thị OLED (I2C), điều khiển buzzer/relay, gửi UID qua Wi-Fi.
- **relay.py**: cầu nối giữa Wokwi (chưa gọi HTTPS trực tiếp được) và Apps Script.
- **Code.gs**: xử lý nghiệp vụ — check thẻ hợp lệ, quản lý phiên, chống điểm danh trùng, ghi Sheets, trả JSON.
- **Google Sheets**: lưu `SinhVien`, `GiaoVien`, `PhienHoc`, `DiemDanh`, và `Dashboard` để xem thống kê.

## Cấu trúc thư mục

```
RFID-Attendance-VSCode/
├── src/            # main.cpp, DiemDanh.cpp, ManHinh.cpp, ThietBi.cpp
├── include/         # CauHinh.h (pin, wifi, timeout...)
├── apps-script/      # Code.gs
├── relay.py          # relay HTTP local (port 3000)
├── diagram.json       # sơ đồ mạch Wokwi
└── platformio.ini
```

## Chạy thử

```bash
# 1. Build firmware (VS Code + PlatformIO)
pio run

# 2. Chạy relay local
python relay.py

# 3. Start simulation trên Wokwi, quét thẻ trong panel MFRC522
```

Thẻ giáo viên dùng preset NFC Tag xám trên Wokwi (UID `04:11:22:33:44:55:66`).

## Trạng thái hiện tại

Nguyên mẫu chạy được trên môi trường mô phỏng (Wokwi), chưa có phần cứng thật, chưa có auth/token cho API, chưa test tự động. Chi tiết đầy đủ (sơ đồ, bảng chân cắm, giới hạn, hướng phát triển...) xem tại [`BAO_CAO_TOM_TAT_DU_AN.md`](./BAO_CAO_TOM_TAT_DU_AN.md).
