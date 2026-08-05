# Lưu đồ hoạt động hệ thống điểm danh RFID

## 1. Lưu đồ xử lý của ESP32

```mermaid
flowchart TD
    A([Khởi động ESP32]) --> B[Khởi tạo OLED, RFID, buzzer và relay]
    B --> C[Kết nối Wi-Fi]
    C --> D[Hiển thị màn hình chờ quét thẻ]
    D --> E{Có thẻ RFID mới?}
    E -- Không --> F{Cửa đã hết thời gian mở?}
    F -- Có --> G[Đóng cửa]
    F -- Không --> D
    G --> D

    E -- Có --> H[Đọc và chuẩn hóa UID]
    H --> I{UID bị quét lặp?}
    I -- Có --> D
    I -- Không --> J{Wi-Fi đã kết nối?}
    J -- Không --> K[Thử kết nối lại Wi-Fi]
    K --> L{Kết nối thành công?}
    L -- Không --> M[Hiển thị và phát âm báo lỗi]
    L -- Có --> N[Gửi UID đến relay.py]
    J -- Có --> N

    N --> O[Nhận và đọc kết quả JSON]
    O --> P{Trạng thái trả về}
    P -- Điểm danh thành công --> Q[Hiển thị sinh viên, phát âm báo và mở cửa]
    P -- Đã điểm danh --> R[Thông báo đã điểm danh, giữ cửa đóng]
    P -- Mở hoặc đóng phiên --> S[Thông báo trạng thái phiên của giáo viên]
    P -- Phiên chưa mở --> T[Thông báo chờ giáo viên]
    P -- Thẻ sai hoặc lỗi hệ thống --> M
    Q --> D
    R --> D
    S --> D
    T --> D
    M --> D
```

## 2. Lưu đồ xử lý của `relay.py`

```mermaid
flowchart TD
    A([Khởi động relay.py]) --> B[Mở HTTP server tại cổng 3000]
    B --> C[Chờ yêu cầu từ ESP32]
    C --> D{Đường dẫn yêu cầu}
    D -- /health --> E[Trả trạng thái relay sẵn sàng]
    D -- Đường dẫn khác --> F[Trả lỗi 404]
    D -- /?uid=... --> G[Đọc và chuẩn hóa UID]

    G --> H{UID có rỗng?}
    H -- Có --> I[Trả lỗi THIEU_UID]
    H -- Không --> J[Tạo URL và gửi UID đến Apps Script]
    J --> K{Kết nối thành công?}
    K -- Không --> L[Trả lỗi kết nối hoặc timeout]
    K -- Có --> M[Nhận phản hồi từ Apps Script]
    M --> N{HTTP và JSON hợp lệ?}
    N -- Không --> O[Trả lỗi Apps Script hoặc lỗi JSON]
    N -- Có --> P{JSON có trường status?}
    P -- Không --> O
    P -- Có --> Q[Chuyển nguyên JSON về ESP32]

    E --> C
    F --> C
    I --> C
    L --> C
    O --> C
    Q --> C
```

## 3. Lưu đồ xử lý của Google Apps Script

```mermaid
flowchart TD
    A([Nhận UID từ relay.py]) --> B[Chuẩn hóa UID và khóa xử lý]
    B --> C{UID và dữ liệu Sheets hợp lệ?}
    C -- Không --> D[Trả JSON báo lỗi]
    C -- Có --> E{UID thuộc giáo viên?}

    E -- Có --> F{Có phiên đang mở?}
    F -- Không --> G[Tạo phiên học mới]
    G --> H[Cập nhật Google Sheets và Dashboard]
    H --> I[Trả PHIEN_DA_MO]
    F -- Có --> J[Đóng phiên đang mở]
    J --> K[Cập nhật Google Sheets và Dashboard]
    K --> L[Trả PHIEN_DA_DONG]

    E -- Không --> M{UID thuộc sinh viên?}
    M -- Không --> N[Trả THE_KHONG_HOP_LE]
    M -- Có --> O{Có phiên đang mở?}
    O -- Không --> P[Trả PHIEN_CHUA_MO]
    O -- Có --> Q{Sinh viên đã điểm danh trong phiên?}
    Q -- Có --> R[Trả DA_DIEM_DANH]
    Q -- Không --> S[Ghi bản ghi điểm danh]
    S --> T[Cập nhật Dashboard]
    T --> U[Trả DIEM_DANH_THANH_CONG]

    D --> V[Giải phóng khóa và gửi JSON về relay.py]
    I --> V
    L --> V
    N --> V
    P --> V
    R --> V
    U --> V
```

Luồng kết nối chung: **Thẻ RFID → ESP32 → `relay.py` → Google Apps Script/Google Sheets → `relay.py` → ESP32 → OLED, buzzer và relay cửa**.
