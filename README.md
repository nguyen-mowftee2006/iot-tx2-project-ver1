# Hệ thống điểm danh sinh viên bằng RFID

Dự án nhập môn IoT mô phỏng quy trình điểm danh bằng ESP32 và thẻ RFID. Giáo viên dùng thẻ để mở hoặc đóng phiên; sinh viên quét thẻ để điểm danh; kết quả được phản hồi trên OLED, buzzer, relay và lưu vào Google Sheets.

Repository này gồm mã nguồn mô phỏng và tài liệu kỹ thuật. Hãy chọn nội dung phù hợp từ bảng dưới đây thay vì đọc README theo thứ tự từ đầu đến cuối.

## Bắt đầu từ đâu?

| Nhu cầu | Tệp hoặc thư mục cần xem |
| --- | --- |
| Tìm hiểu toàn bộ kiến trúc và nguyên lý | [Tài liệu kỹ thuật](./BAO_CAO_TOM_TAT_DU_AN.md) |
| Build và chạy mô phỏng | [Hướng dẫn trong thư mục dự án](./RFID-Attendance-VSCode/README.md) |
| Xem firmware ESP32 | [`RFID-Attendance-VSCode/src/`](./RFID-Attendance-VSCode/src/) |
| Xem cấu hình chân và thời gian chờ | [`RFID-Attendance-VSCode/include/CauHinh.h`](./RFID-Attendance-VSCode/include/CauHinh.h) |
| Xem nghiệp vụ Google Sheets | [`RFID-Attendance-VSCode/apps-script/Code.gs`](./RFID-Attendance-VSCode/apps-script/Code.gs) |
| Xem sơ đồ mạch Wokwi | [`RFID-Attendance-VSCode/diagram.json`](./RFID-Attendance-VSCode/diagram.json) |

## Cấu trúc repository

```text
.
├── README.md                         # Trang dẫn đường này
├── BAO_CAO_TOM_TAT_DU_AN.md         # Tài liệu kỹ thuật đầy đủ
└── RFID-Attendance-VSCode/
    ├── README.md                     # Hướng dẫn build và mô phỏng
    ├── src/                          # Firmware ESP32
    ├── include/                      # Khai báo và cấu hình
    ├── apps-script/Code.gs           # Backend Google Apps Script
    ├── relay.py                      # Cầu nối chỉ dùng khi mô phỏng
    ├── diagram.json                  # Mạch Wokwi
    ├── platformio.ini                # Cấu hình PlatformIO
    └── wokwi.toml                    # Firmware dùng bởi Wokwi
```

## Chạy nhanh

Yêu cầu: VS Code, PlatformIO, Wokwi, Python 3 và một Google Apps Script Web App đã được cấu hình.

```powershell
cd RFID-Attendance-VSCode
pio run
python relay.py
```

Sau đó khởi động Wokwi và quét thẻ trong bảng điều khiển MFRC522. Quy trình thiết lập Google Sheets, Apps Script và kiểm tra relay được trình bày trong [hướng dẫn chạy mô phỏng](./RFID-Attendance-VSCode/README.md).

## Trạng thái

- Firmware hiện tại build thành công bằng PlatformIO.
- Các luồng mở phiên, điểm danh, chống trùng và đóng phiên đã được thử trên Wokwi.
- Đây là nguyên mẫu mô phỏng; phần cứng thật và kết nối HTTPS trực tiếp từ ESP32 chưa được kiểm thử.

Thông số chân nối, sơ đồ hệ thống thật, cấu trúc Google Sheets, trạng thái API, kết quả kiểm thử và các giới hạn được tập trung trong [tài liệu kỹ thuật](./BAO_CAO_TOM_TAT_DU_AN.md) để tránh lặp lại tại đây.
