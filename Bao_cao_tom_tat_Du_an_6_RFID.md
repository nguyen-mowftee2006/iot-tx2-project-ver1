# BÁO CÁO TÓM TẮT — DỰ ÁN IoT SỐ 6
### Hệ thống điểm danh sinh viên bằng RFID

**Công nghệ:** ESP32 · MFRC522 · Python Relay · Google Apps Script · Google Sheets
**Môn học:** Internet vạn vật (IoT) — **Năm thực hiện:** 2026

---

## 1. Tổng quan đề tài

Hệ thống điểm danh sinh viên bằng thẻ RFID: mỗi sinh viên có một UID thẻ riêng. Khi quét thẻ qua đầu đọc MFRC522, ESP32 đọc UID và gửi qua mạng tới hệ thống xử lý, kết quả được lưu vào Google Sheets. Dashboard tự động cập nhật số lượng sinh viên, số đã/chưa điểm danh, tỷ lệ điểm danh và lịch sử quét gần nhất.

**Mục tiêu:**
- Nhận dạng sinh viên qua UID thẻ RFID, kiểm tra hợp lệ và chống điểm danh trùng.
- Lưu dữ liệu điểm danh theo thời gian lên Google Sheets, hiển thị trên Serial Monitor và Dashboard.
- Mô phỏng toàn hệ thống bằng Wokwi trong VS Code.

**Phạm vi hiện tại:** quản lý sinh viên theo UID, mã số, họ tên, lớp; kiểm tra trùng dựa trên UID đã có trong sheet DiemDanh; Dashboard thống kê chung cho tất cả lớp. Đây là bản nguyên mẫu phục vụ mô phỏng và làm nền tảng để mở rộng theo buổi học hoặc lọc theo lớp.

---

## 2. Kiến trúc hệ thống

**Luồng dữ liệu:** Thẻ RFID → MFRC522 → ESP32 → Python Relay → Apps Script → Google Sheets → Dashboard

Trong môi trường mô phỏng, ESP32 gửi UID qua HTTP tới `relay.py` chạy trên máy tính. Relay chuyển tiếp yêu cầu tới Apps Script qua HTTPS; Apps Script xử lý dữ liệu, cập nhật Sheets và trả JSON về ESP32.

**Thành phần chính:**

| Thành phần | Vai trò |
|---|---|
| ESP32 DevKit V4 | Bộ điều khiển trung tâm: đọc UID, kết nối Wi-Fi, gửi HTTP |
| MFRC522 | Đầu đọc thẻ RFID, giao tiếp SPI với ESP32 |
| Thẻ RFID | Chứa UID nhận dạng sinh viên |
| Wokwi (VS Code) | Mô phỏng ESP32, MFRC522 và thẻ RFID |
| PlatformIO | Biên dịch, quản lý thư viện cho ESP32 |
| Python Relay | Nhận UID từ ESP32, chuyển tiếp tới Apps Script |
| Google Apps Script | Xử lý nghiệp vụ điểm danh, trả JSON |
| Google Sheets | Lưu danh sách sinh viên, lịch sử điểm danh, Dashboard |

**Kết nối MFRC522 – ESP32:**

| Chân MFRC522 | Chân ESP32 | Chức năng |
|---|---|---|
| SDA/SS | GPIO 5 | Chọn thiết bị SPI |
| SCK | GPIO 18 | Xung nhịp SPI |
| MOSI | GPIO 23 | ESP32 → RC522 |
| MISO | GPIO 19 | RC522 → ESP32 |
| RST | GPIO 4 | Khởi động lại RC522 |
| 3.3V | 3V3 | Nguồn 3,3V |
| GND | GND | Nối đất |

---

## 3. Nguyên lý hoạt động

1. ESP32 kết nối Wi-Fi và khởi tạo SPI với MFRC522.
2. Sinh viên đưa thẻ RFID lại gần đầu đọc.
3. MFRC522 đọc UID; ESP32 chuẩn hóa thành chuỗi in hoa.
4. ESP32 gửi UID tới Python Relay bằng HTTP GET.
5. Relay chuyển tiếp UID tới Web App của Apps Script.
6. Apps Script tìm UID trong sheet SinhVien.
7. Không tìm thấy → trả `THE_KHONG_HOP_LE`.
8. UID đã có trong DiemDanh → trả `DA_DIEM_DANH`.
9. UID hợp lệ và chưa điểm danh → ghi dòng mới vào DiemDanh, trả `DIEM_DANH_THANH_CONG`.
10. ESP32 hiển thị phản hồi trên Serial Monitor; Dashboard tự cập nhật.

**Các trạng thái phản hồi:**

| Trạng thái | Ý nghĩa |
|---|---|
| `DIEM_DANH_THANH_CONG` | Thẻ hợp lệ, đã ghi vào bảng điểm danh |
| `DA_DIEM_DANH` | UID đã có trong bảng điểm danh |
| `THE_KHONG_HOP_LE` | UID không có trong danh sách sinh viên |
| `THIEU_UID` | Yêu cầu không chứa UID |
| `KHONG_TIM_THAY_SHEET` | Thiếu sheet bắt buộc |

---

## 4. Cấu trúc dữ liệu trên Google Sheets

**Sheet SinhVien** — danh sách thẻ được phép dùng: `UID`, `MSSV`, `HoTen`, `Lop`.

**Sheet DiemDanh** — lịch sử điểm danh thành công: `ThoiGian`, `UID`, `MSSV`/`HoTen`/`Lop` (lấy từ SinhVien), `TrangThai`.

**Sheet Dashboard** — tổng hợp tự động: tổng số sinh viên, số đã/chưa điểm danh, tỷ lệ điểm danh, biểu đồ và danh sách quét gần nhất. Cập nhật theo công thức mỗi khi DiemDanh có dữ liệu mới.

---

## 5. Các khối chương trình chính

| Khối/Hàm | Nhiệm vụ |
|---|---|
| `layUID()` | Ghép byte UID thành chuỗi HEX, chuyển in hoa |
| `ketNoiWiFi()` | Kết nối và kiểm tra trạng thái Wi-Fi |
| `guiDiemDanh()` | Tạo URL kèm UID, gửi HTTP GET, nhận phản hồi |
| `hienThiKetQua()` | Phân tích JSON, in kết quả lên Serial Monitor |
| `setup()` | Khởi tạo Serial, Wi-Fi, SPI, MFRC522 |
| `loop()` | Chờ thẻ mới, đọc UID, thực hiện điểm danh |
| `relay.py` | HTTP server cục bộ, nhận UID và chuyển tiếp tới Apps Script |
| `doGet(e)` | Nhận UID, kiểm tra dữ liệu, ghi điểm danh, trả JSON |

---

## 6. Kết quả đạt được

- Mô phỏng thành công ESP32 và MFRC522 đọc nhiều loại thẻ RFID trên Wokwi.
- UID được đối chiếu chính xác với danh sách sinh viên trên Google Sheets.
- Phân biệt được ba trạng thái: điểm danh thành công, thẻ chưa đăng ký, đã điểm danh.
- Dữ liệu điểm danh ghi kèm thời gian, UID, mã số, họ tên, lớp.
- Dashboard hiển thị và tự cập nhật thống kê khi có dữ liệu mới.
- Mã nguồn quản lý bằng Git, lưu trên GitHub.

---

## 7. Ưu điểm, hạn chế và hướng phát triển

**Ưu điểm**
- Kiến trúc đầy đủ: thiết bị → mạng → API → lưu trữ → Dashboard.
- Chi phí thấp, dễ mô phỏng và triển khai thử nghiệm.
- Dữ liệu trực quan, dễ kiểm tra trên Google Sheets.
- Có kiểm tra thẻ hợp lệ và chống điểm danh lặp lại.
- Dễ mở rộng thêm chức năng mà không đổi phần cứng chính.

**Hạn chế**
- Kiểm tra trùng trên toàn bộ sheet DiemDanh, chưa tách theo buổi học.
- Dashboard thống kê chung cho tất cả lớp.
- Bản demo phụ thuộc máy tính chạy Python Relay.
- Chưa có xác thực API (token/API key).
- Chưa có LCD, LED hay còi báo trạng thái ngoài Serial Monitor.

**Hướng phát triển**
- Thêm sheet BuoiHoc, kiểm tra trùng theo UID kết hợp MaBuoi.
- Cho phép lọc Dashboard theo lớp, môn học hoặc ngày.
- Thêm LED xanh/đỏ, buzzer hoặc màn hình LCD/OLED.
- Đưa Relay lên máy chủ hoạt động liên tục, hoặc xây API riêng.
- Dùng POST, token xác thực và phân quyền quản trị.
- Xuất báo cáo theo buổi học, gửi thông báo tự động.

---

## 8. Câu hỏi vấn đáp thường gặp

**Đề tài giải quyết vấn đề gì?**
Tự động nhận dạng sinh viên bằng UID thẻ RFID, ghi nhận thời gian điểm danh và lưu lên Google Sheets — giảm thao tác nhập thủ công, dễ thống kê.

**Tại sao chọn ESP32?**
Có Wi-Fi tích hợp, đủ chân giao tiếp SPI, dễ lập trình bằng Arduino Framework, phù hợp ứng dụng IoT cần gửi dữ liệu qua mạng.

**MFRC522 giao tiếp với ESP32 bằng chuẩn nào?**
SPI, qua các chân SCK, MOSI, MISO, SS/SDA và RST.

**UID là gì và dùng để làm gì?**
Là mã nhận dạng của thẻ RFID; hệ thống dùng UID để tìm sinh viên tương ứng trong sheet SinhVien.

**Tại sao dùng Google Sheets?**
Vừa lưu dữ liệu trên đám mây vừa hỗ trợ công thức, biểu đồ, Dashboard — không cần cài máy chủ cơ sở dữ liệu riêng.

**Apps Script và Python Relay đóng vai trò gì?**
Apps Script là lớp xử lý nghiệp vụ: nhận UID, tìm sinh viên, kiểm tra trùng, ghi dữ liệu, trả JSON. Relay nhận HTTP từ ESP32 mô phỏng và chuyển tiếp UID tới Apps Script qua HTTPS, rồi trả phản hồi về ESP32.

**Dữ liệu đi qua hệ thống như thế nào?**
RC522 đọc UID → ESP32 gửi tới Relay → Relay gửi tới Apps Script → Apps Script đọc/ghi Sheets → JSON trả ngược về ESP32.

**Hệ thống nhận biết thẻ hợp lệ và chống điểm danh trùng bằng cách nào?**
Apps Script chuẩn hóa UID rồi so với cột UID trong SinhVien để xác định hợp lệ. Trước khi ghi dòng mới, kiểm tra UID đã có trong DiemDanh chưa — nếu có thì trả `DA_DIEM_DANH`.

**Nếu sinh viên thuộc lớp khác thì sao?**
Lop chỉ là thông tin đi kèm; miễn UID có trong SinhVien vẫn được điểm danh. Dashboard hiện thống kê chung, có thể nâng cấp lọc theo lớp.

**Nếu quét thẻ chưa đăng ký hoặc mất Wi-Fi thì sao?**
Thẻ chưa đăng ký: hệ thống không ghi vào DiemDanh, trả `THE_KHONG_HOP_LE`. Mất Wi-Fi: ESP32 không gửi được UID, chương trình kiểm tra và thử kết nối lại trước khi gửi.

**JSON dùng để làm gì? Tại sao cột UID phải định dạng văn bản?**
JSON là định dạng phản hồi giữa Apps Script, Relay và ESP32, chứa các trường như success, UID, họ tên, lớp, status. Cột UID cần định dạng văn bản vì một số UID bắt đầu bằng số 0 — nếu Sheets coi là số, số 0 đầu có thể mất, làm sai kết quả so sánh.

**Hệ thống hiện có tách điểm danh theo ngày/buổi học không?**
Chưa. Nếu UID còn trong DiemDanh, hệ thống vẫn báo đã điểm danh. Hướng nâng cấp: thêm MaBuoi, kiểm tra theo cặp UID–MaBuoi.

**Khác biệt giữa mô phỏng và triển khai thực tế?**
Mô phỏng dùng ESP32/RC522 ảo trên Wokwi. Triển khai thật thay bằng phần cứng thật; phần xử lý dữ liệu có thể giữ nguyên hoặc chuyển Relay lên máy chủ.

**Đây có phải một hệ thống IoT hoàn chỉnh không?**
Có — đủ thiết bị nhận dạng, bộ điều khiển, kết nối mạng, dịch vụ xử lý đám mây, lưu trữ dữ liệu và giao diện Dashboard.

**Hạn chế lớn nhất và ưu tiên phát triển tiếp theo là gì?**
Hạn chế lớn nhất: chưa quản lý theo buổi học và còn phụ thuộc máy tính chạy Relay trong bản mô phỏng. Ưu tiên phát triển: thêm bảng BuoiHoc và kiểm tra trùng theo UID kết hợp MaBuoi, giúp hệ thống dùng được qua nhiều buổi học mà không cần xóa dữ liệu cũ.

---

## 9. Bài giới thiệu ngắn khi trình bày

> Đề tài là hệ thống điểm danh sinh viên bằng RFID sử dụng ESP32 và Google Sheets. Mỗi sinh viên có một UID thẻ riêng trong sheet SinhVien. Khi quét thẻ, MFRC522 gửi UID cho ESP32 qua SPI; ESP32 gửi UID tới Python Relay, sau đó chuyển tới Google Apps Script. Apps Script kiểm tra UID hợp lệ, kiểm tra đã điểm danh chưa, rồi ghi kết quả vào sheet DiemDanh. Dashboard tự động thống kê tổng số sinh viên, số đã/chưa điểm danh và tỷ lệ điểm danh. Hệ thống xử lý được ba trường hợp: điểm danh thành công, thẻ không hợp lệ và đã điểm danh. Hướng phát triển tiếp theo là quản lý theo buổi học, lọc theo lớp và thêm LED/buzzer báo trạng thái trực tiếp.

---

## 10. Liên kết dự án

- **Google Sheets:** https://docs.google.com/spreadsheets/d/1D707gA9baIK5HzmGh-4D3SpqO9DLWUXdBuUu1J39mVk/edit?gid=1513199782#gid=1513199782
- **Google Apps Script:** https://script.google.com/u/0/home/projects/10EpwK3sJsaTSH24K8oZGOUNiLH44VngeRLpB_64aTbX6ix1EMU46rFtF/edit
