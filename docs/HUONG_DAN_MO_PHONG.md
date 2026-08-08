# Hệ thống điểm danh RFID ESP32

Dự án nhập môn IoT dùng ESP32 đọc thẻ RFID, gửi UID qua relay Python tới Google Apps Script và hiển thị kết quả trên OLED.

## Thành phần

- ESP32 DevKit
- RFID RC522
- OLED SSD1306 128x64
- Buzzer
- Relay mô phỏng cửa
- Google Sheets và Google Apps Script

## Kết nối chính

| Thiết bị | Chân ESP32 |
| --- | --- |
| RFID SDA/SS | GPIO 5 |
| RFID RST | GPIO 4 |
| RFID SCK | GPIO 18 |
| RFID MISO | GPIO 19 |
| RFID MOSI | GPIO 23 |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| Buzzer | GPIO 25 |
| Relay | GPIO 26 |

## Cách chạy

1. Mở dự án bằng VS Code có cài PlatformIO.
2. Build dự án để tạo firmware cho Wokwi.
3. Chạy relay local:

   ```powershell
   python relay.py
   ```

4. Kiểm tra relay tại `http://127.0.0.1:3000/health`.
5. Khởi động mô phỏng Wokwi và quét thẻ RFID.

## Thẻ giáo viên

Thẻ giáo viên dùng preset **Thẻ rỗng** của đầu đọc RC522 trên Wokwi:

- UID: `04:11:22:33:44:55:66`
- Phím chọn thẻ: `N`
- Phím quét thẻ: `T`

Thẻ giáo viên được lưu riêng trong sheet `GiaoVien` theo mẫu:

| UID | MaGV | HoTen | BoMon |
| --- | --- | --- | --- |
| `04:11:22:33:44:55:66` | `GV001` | `GIAO VIEN` | `DHTTMT` |

`Code.gs` dùng `PhienHoc` làm nguồn trạng thái chính và ghi `MaBuoi` vào từng dòng của `DiemDanh`. Sau khi cập nhật mã, cần triển khai lại Web App để phiên bản API mới có hiệu lực.

- Giáo viên quét lần đầu: tạo một dòng `DANG_MO` trong `PhienHoc`.
- Giáo viên quét lần tiếp theo: cập nhật chính dòng đó thành `DA_DONG`.
- Sinh viên chỉ được điểm danh khi phiên đang mở.
- Sinh viên chỉ được ghi một lần trong mỗi `MaBuoi`, nhưng được điểm danh lại ở buổi sau.
- Thẻ giáo viên không được ghi vào sheet `DiemDanh`.

## Kết quả xử lý

- `DIEM_DANH_THANH_CONG`: hiển thị sinh viên, phát âm báo, mở cửa 3 giây rồi tự đóng.
- `DA_DIEM_DANH`: thông báo đã điểm danh và giữ cửa đóng.
- `PHIEN_DA_MO`: thẻ giáo viên đã mở phiên điểm danh.
- `PHIEN_DA_DONG`: thẻ giáo viên đã đóng phiên điểm danh.
- `PHIEN_CHUA_MO`: sinh viên quét khi giáo viên chưa mở phiên.
- `THE_KHONG_HOP_LE`: từ chối truy cập và phát âm cảnh báo.
- Lỗi Wi-Fi, relay hoặc Google: đóng cửa và hiển thị lỗi hệ thống.

Các chân kết nối, thời gian chờ và địa chỉ relay được cấu hình trong `include/CauHinh.h`.
