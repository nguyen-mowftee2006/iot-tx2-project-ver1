from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlencode, urlparse
from urllib.request import urlopen
from urllib.error import HTTPError, URLError
import json


APPS_SCRIPT_URL = (
    "https://script.google.com/macros/s/"
    "AKfycbwM4rLqlFTREbY9qLiqZQU5GhXspxh0M-vlNGXzJFHUmtcEcVgSNxBDKxASmV16-_hkAw"
    "/exec"
)


class RelayHandler(BaseHTTPRequestHandler):
    def gui_json(self, ma_http: int, du_lieu: dict) -> None:
        noi_dung = json.dumps(du_lieu, ensure_ascii=False).encode("utf-8")

        self.send_response(ma_http)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(noi_dung)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()

        self.wfile.write(noi_dung)

    def do_GET(self) -> None:
        tham_so = parse_qs(urlparse(self.path).query)

        uid = tham_so.get("uid", [""])[0]
        uid = uid.replace(":", "").replace(" ", "").upper()

        if not uid:
            self.gui_json(400, {
                "success": False,
                "status": "THIEU_UID"
            })
            return

        url_google = APPS_SCRIPT_URL + "?" + urlencode({"uid": uid})

        print(f"Nhan UID: {uid}")
        print("Dang chuyen tiep den Apps Script...")

        try:
            # urlopen tự đi theo redirect của Google Apps Script
            with urlopen(url_google, timeout=20) as phan_hoi:
                noi_dung = phan_hoi.read()

                self.send_response(phan_hoi.status)
                self.send_header(
                    "Content-Type",
                    "application/json; charset=utf-8"
                )
                self.send_header("Content-Length", str(len(noi_dung)))
                self.send_header("Cache-Control", "no-store")
                self.end_headers()

                self.wfile.write(noi_dung)

            print("Chuyen tiep thanh cong\n")

        except HTTPError as loi:
            self.gui_json(loi.code, {
                "success": False,
                "status": "LOI_APPS_SCRIPT",
                "message": str(loi)
            })

        except URLError as loi:
            self.gui_json(502, {
                "success": False,
                "status": "LOI_KET_NOI_GOOGLE",
                "message": str(loi)
            })

        except Exception as loi:
            self.gui_json(500, {
                "success": False,
                "status": "LOI_RELAY",
                "message": str(loi)
            })

    def log_message(self, format: str, *args) -> None:
        return


if __name__ == "__main__":
    dia_chi = "0.0.0.0"
    cong = 3000

    server = ThreadingHTTPServer((dia_chi, cong), RelayHandler)

    print("======================================")
    print("RFID ATTENDANCE LOCAL RELAY")
    print(f"Dang chay tai http://127.0.0.1:{cong}")
    print("Nhan Ctrl + C de dung")
    print("======================================")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nDa dung Relay.")
        server.server_close()