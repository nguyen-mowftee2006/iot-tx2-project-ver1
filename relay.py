from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlencode, urlparse
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError

import json
import os
import socket
import time


# ===================== CẤU HÌNH =====================

PHIEN_BAN = "2026-08-03-v1"

APPS_SCRIPT_URL = (
    "https://script.google.com/macros/s/"
    "AKfycbwM4rLqlFTREbY9qLiqZQU5GhXspxh0M-vlNGXzJFHUmtcEcVgSNxBDKxASmV16-_hkAw"
    "/exec"
)

DIA_CHI = "0.0.0.0"
CONG = 3000

# Google Apps Script đôi khi phản hồi chậm. Relay nên chờ đủ lâu trước khi báo lỗi.
TIMEOUT_GOOGLE = 30


# ===================== SERVER =====================

class RelayServer(ThreadingHTTPServer):
    allow_reuse_address = True
    daemon_threads = True


class RelayHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def gui_json(self, ma_http: int, du_lieu: dict) -> None:
        noi_dung = json.dumps(du_lieu, ensure_ascii=False).encode("utf-8")

        self.send_response(ma_http)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(noi_dung)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("Connection", "close")
        self.end_headers()
        self.close_connection = True

        try:
            self.wfile.write(noi_dung)
        except (BrokenPipeError, ConnectionResetError):
            print(
                "ESP32 hoặc trình duyệt đã ngắt kết nối "
                "trước khi relay gửi xong phản hồi."
            )

    def do_GET(self) -> None:
        duong_dan = urlparse(self.path)

        # Endpoint kiểm tra relay
        if duong_dan.path == "/health":
            self.gui_json(200, {
                "success": True, "status": "RELAY_SAN_SANG", "version": PHIEN_BAN
            })
            return

        # Chỉ chấp nhận route /
        if duong_dan.path != "/":
            self.gui_json(404, {
                "success": False, "status": "LOI_DUONG_DAN", "path": duong_dan.path
            })
            return

        tham_so = parse_qs(duong_dan.query)
        uid = tham_so.get("uid", [""])[0]
        uid = uid.replace(":", "").replace(" ", "").upper().strip()

        if uid == "":
            self.gui_json(400, {"success": False, "status": "THIEU_UID"})
            return

        url_google = APPS_SCRIPT_URL + "?" + urlencode({"uid": uid})

        print("\n======================================")
        print(f"Nhan UID       : {uid}")
        print(f"URL Google     : {url_google}")
        print("Dang goi Google Apps Script...")

        bat_dau = time.monotonic()

        try:
            yeu_cau = Request(
                url_google,
                headers={
                    "User-Agent": "RFID-Attendance-Relay/1.0",
                    "Accept": "application/json",
                },
                method="GET",
            )

            # urlopen tự động đi theo redirect của Apps Script
            with urlopen(yeu_cau, timeout=TIMEOUT_GOOGLE) as phan_hoi:
                ma_google = phan_hoi.getcode()
                url_cuoi = phan_hoi.geturl()
                noi_dung_byte = phan_hoi.read()

            thoi_gian = time.monotonic() - bat_dau
            noi_dung = noi_dung_byte.decode("utf-8-sig", errors="replace").strip()

            print(f"HTTP Google    : {ma_google}")
            print(f"URL sau redirect: {url_cuoi}")
            print(f"Thoi gian      : {thoi_gian:.2f} giay")
            print(f"Phan hoi       : {noi_dung}")

            try:
                du_lieu = json.loads(noi_dung)
            except json.JSONDecodeError as loi:
                print("Loi: Apps Script khong tra JSON hop le.")
                self.gui_json(502, {
                    "success": False,
                    "status": "LOI_JSON_APPS_SCRIPT",
                    "message": str(loi),
                    "response": noi_dung[:300],
                })
                return

            if not isinstance(du_lieu, dict):
                self.gui_json(502, {
                    "success": False,
                    "status": "LOI_JSON_APPS_SCRIPT",
                    "message": "JSON tra ve khong phai object",
                })
                return

            if "status" not in du_lieu:
                self.gui_json(502, {
                    "success": False,
                    "status": "LOI_JSON_APPS_SCRIPT",
                    "message": "JSON khong co truong status",
                    "response": du_lieu,
                })
                return

            # Trả nguyên JSON chuẩn của Apps Script về ESP32
            self.gui_json(200, du_lieu)

            print("Chuyen tiep thanh cong.")
            print("======================================")

        except HTTPError as loi:
            thoi_gian = time.monotonic() - bat_dau

            try:
                noi_dung_loi = loi.read().decode("utf-8", errors="replace")
            except Exception:
                noi_dung_loi = ""

            print(f"HTTP Google    : {loi.code}")
            print(f"Thoi gian      : {thoi_gian:.2f} giay")
            print(f"Loi Apps Script: {loi}")
            print(f"Noi dung loi   : {noi_dung_loi[:300]}")

            # Relay trả 502, không trả nguyên 404 của Google
            self.gui_json(502, {
                "success": False,
                "status": "LOI_APPS_SCRIPT",
                "googleHttp": loi.code,
                "message": str(loi),
            })

        except socket.timeout:
            print(f"Google khong phan hoi sau {TIMEOUT_GOOGLE} giay.")
            self.gui_json(504, {
                "success": False,
                "status": "LOI_TIMEOUT_GOOGLE",
                "message": f"Google khong phan hoi sau {TIMEOUT_GOOGLE} giay",
            })

        except TimeoutError:
            print("Ket noi Google bi timeout.")
            self.gui_json(504, {"success": False, "status": "LOI_TIMEOUT_GOOGLE"})

        except URLError as loi:
            if isinstance(loi.reason, socket.timeout):
                trang_thai = "LOI_TIMEOUT_GOOGLE"
                ma_http = 504
            else:
                trang_thai = "LOI_KET_NOI_GOOGLE"
                ma_http = 502

            print(f"Loi ket noi Google: {loi}")
            self.gui_json(ma_http, {
                "success": False, "status": trang_thai, "message": str(loi)
            })

        except Exception as loi:
            print(f"Loi relay: {loi}")
            self.gui_json(500, {
                "success": False, "status": "LOI_RELAY", "message": str(loi)
            })

    def log_message(self, dinh_dang: str, *tham_so) -> None:
        # Tắt log mặc định của HTTP server
        return


# ===================== CHẠY SERVER =====================

if __name__ == "__main__":
    server = RelayServer((DIA_CHI, CONG), RelayHandler)

    print("======================================")
    print("RFID ATTENDANCE LOCAL RELAY")
    print(f"Phien ban     : {PHIEN_BAN}")
    print(f"File dang chay: {os.path.abspath(__file__)}")
    print(f"Dia chi       : http://127.0.0.1:{CONG}")
    print(f"Health check  : http://127.0.0.1:{CONG}/health")
    print(f"Apps Script   : {APPS_SCRIPT_URL}")
    print(f"Google timeout: {TIMEOUT_GOOGLE} giay")
    print("Nhan Ctrl + C de dung")
    print("======================================")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nDang dung relay...")
    finally:
        server.server_close()
        print("Da dung relay.")
