# Lưu đồ hoạt động hệ thống điểm danh RFID

## 1. Lưu đồ xử lý của ESP32

```mermaid
flowchart TD
    A([Khởi động ESP32]) --> B[Khởi tạo OLED, RFID, buzzer và relay]
    B --> C[Kết nối Wi-Fi]
    C --> D[Hiển thị màn hình chờ quét thẻ]
    D --> E{Có thẻ RFID mới?}
    E -- Không --> D
    E -- Có --> F[Đọc và chuẩn hóa UID]
    F --> G{Thẻ bị quét lặp?}
    G -- Có --> D
    G -- Không --> H[Gửi UID đến relay.py]
    H --> I[Nhận kết quả JSON]
    I --> J{Điểm danh thành công?}
    J -- Có --> K[Hiển thị thông tin, phát âm báo và mở cửa]
    K --> L[Tự động đóng cửa sau thời gian cài đặt]
    L --> D
    J -- Không --> M[Thông báo trạng thái hoặc lỗi, giữ cửa đóng]
    M --> D
```

## 2. Lưu đồ xử lý của `relay.py`

```mermaid
flowchart TD
    A([Khởi động relay.py]) --> B[Mở HTTP server tại cổng 3000]
    B --> C[Chờ yêu cầu từ ESP32]
    C --> D[Đọc và chuẩn hóa UID]
    D --> E{Yêu cầu hợp lệ?}
    E -- Không --> F[Trả lỗi về ESP32]
    E -- Có --> G[Gửi UID đến Apps Script]
    G --> H[Nhận phản hồi]
    H --> I{Phản hồi hợp lệ?}
    I -- Không --> F
    I -- Có --> J[Chuyển JSON về ESP32]
    F --> C
    J --> C
```

## 3. Lưu đồ xử lý của Google Apps Script

```mermaid
flowchart TD
    A([Nhận UID từ relay.py]) --> B{Yêu cầu hợp lệ?}
    B -- Không --> C[Trả JSON báo lỗi]
    B -- Có --> D{Là thẻ giáo viên?}
    D -- Có --> E{Phiên đang mở?}
    E -- Có --> F[Đóng phiên]
    E -- Không --> G[Mở phiên mới]
    D -- Không --> H{Là thẻ sinh viên?}
    H -- Không --> I[Trả thẻ không hợp lệ]
    H -- Có --> J{Phiên đang mở?}
    J -- Không --> K[Trả phiên chưa mở]
    J -- Có --> L{Đã điểm danh?}
    L -- Có --> M[Trả đã điểm danh]
    L -- Không --> N[Ghi điểm danh thành công]
    F --> O[Trả JSON về relay.py]
    G --> O
    I --> O
    K --> O
    M --> O
    N --> O
    C --> O
```

Luồng kết nối chung: **Thẻ RFID → ESP32 → `relay.py` → Google Apps Script/Google Sheets → `relay.py` → ESP32 → OLED, buzzer và relay cửa**.
