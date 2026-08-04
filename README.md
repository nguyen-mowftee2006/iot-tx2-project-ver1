# Hệ thống điểm danh sinh viên bằng RFID

Dự án nhập môn IoT mô phỏng quy trình điểm danh bằng ESP32 và thẻ RFID. Giáo viên dùng thẻ để mở hoặc đóng phiên; sinh viên quét thẻ để điểm danh; kết quả được phản hồi trên OLED, buzzer, relay và lưu vào Google Sheets.

Repository này gồm mã nguồn mô phỏng và tài liệu kỹ thuật. Hãy chọn nội dung phù hợp từ bảng dưới đây thay vì đọc README theo thứ tự từ đầu đến cuối.

## Bắt đầu từ đâu?

| Nhu cầu | Tệp hoặc thư mục cần xem |
| --- | --- |
| Tìm hiểu toàn bộ kiến trúc và nguyên lý | [Tài liệu kỹ thuật](./docs/TAI_LIEU_KY_THUAT.md) |
| Xem hướng dẫn sử dụng và chạy mô phỏng | [Hướng dẫn mô phỏng](./docs/HUONG_DAN_MO_PHONG.md) |
| Mở Google Sheets của dự án | [Liên kết Google Sheets](./docs/GOOGLE_SHEETS.md) |
| Xem firmware ESP32 | [`src/`](./src/) |
| Xem cấu hình chân và thời gian chờ | [`include/CauHinh.h`](./include/CauHinh.h) |
| Xem nghiệp vụ Google Sheets | [`apps-script/Code.gs`](./apps-script/Code.gs) |
| Xem sơ đồ mạch Wokwi | [`diagram.json`](./diagram.json) |

## Cấu trúc repository

```text
.
├── README.md                  # Trang dẫn đường
├── docs/                      # Tài liệu và liên kết tham khảo
│   ├── TAI_LIEU_KY_THUAT.md
│   ├── HUONG_DAN_MO_PHONG.md
│   └── GOOGLE_SHEETS.md
├── src/                       # Firmware ESP32
├── include/                   # Header và cấu hình firmware
├── apps-script/               # Backend Google Apps Script
│   └── Code.gs
├── relay.py                   # Cầu nối chỉ dùng khi mô phỏng
├── diagram.json               # Sơ đồ mạch Wokwi
├── platformio.ini             # Cấu hình PlatformIO
└── wokwi.toml                 # Firmware dùng bởi Wokwi
```

## Chạy nhanh

Yêu cầu: VS Code, PlatformIO, Wokwi, Python 3 và một Google Apps Script Web App đã được cấu hình.

```powershell
pio run
python relay.py
```

Chạy các lệnh trên ngay tại thư mục gốc của repository. Sau đó khởi động Wokwi và quét thẻ trong bảng điều khiển MFRC522. Thông tin thẻ giáo viên, chân kết nối và quy trình sử dụng được trình bày trong [hướng dẫn mô phỏng](./docs/HUONG_DAN_MO_PHONG.md).

## Trạng thái

- Firmware hiện tại build thành công bằng PlatformIO.
- Các luồng mở phiên, điểm danh, chống trùng và đóng phiên đã được thử trên Wokwi.
- Đây là nguyên mẫu mô phỏng; phần cứng thật và kết nối HTTPS trực tiếp từ ESP32 chưa được kiểm thử.

Thông số chân nối, sơ đồ hệ thống thật, cấu trúc Google Sheets, trạng thái API, kết quả kiểm thử và các giới hạn được tập trung trong [tài liệu kỹ thuật](./docs/TAI_LIEU_KY_THUAT.md) để tránh lặp lại tại đây.
