# Dịch vụ Google

Dự án sử dụng Google Sheets làm cơ sở dữ liệu đám mây và Google Apps Script làm API web.

## Google Sheets

[Mở bảng điểm danh RFID trên Google Sheets](https://docs.google.com/spreadsheets/d/1D707gA9baIK5HzmGh-4D3SpqO9DLWUXdBuUu1J39mVk/edit?gid=1513199782#gid=1513199782)

Bảng tính gồm các sheet:

- `SinhVien`: lưu UID thẻ RFID và thông tin sinh viên
- `DiemDanh`: lưu lịch sử điểm danh
- `Dashboard`: hiển thị số liệu thống kê điểm danh

## Google Apps Script

[Mở dự án Google Apps Script](https://script.google.com/u/0/home/projects/10EpwK3sJsaTSH24K8oZGOUNiLH44VngeRLpB_64aTbX6ix1EMU46rFtF/edit)

Google Apps Script thực hiện các nhiệm vụ:

- Nhận UID từ hệ thống
- Tìm sinh viên tương ứng trong sheet `SinhVien`
- Kiểm tra thẻ có hợp lệ hay không
- Kiểm tra sinh viên đã điểm danh hay chưa
- Ghi dữ liệu mới vào sheet `DiemDanh`
- Trả kết quả về dưới dạng JSON

> Liên kết chỉnh sửa Google Apps Script chỉ mở được khi đăng nhập bằng tài khoản Google có quyền truy cập dự án.
