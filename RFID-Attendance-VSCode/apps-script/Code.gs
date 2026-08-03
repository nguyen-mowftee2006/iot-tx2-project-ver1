const TEN_SHEET = Object.freeze({
  SINH_VIEN: "SinhVien",
  GIAO_VIEN: "GiaoVien",
  PHIEN_HOC: "PhienHoc",
  DIEM_DANH: "DiemDanh"
});

const TRANG_THAI = Object.freeze({
  DIEM_DANH_THANH_CONG: "DIEM_DANH_THANH_CONG",
  DA_DIEM_DANH: "DA_DIEM_DANH",
  THE_KHONG_HOP_LE: "THE_KHONG_HOP_LE",
  PHIEN_DA_MO: "PHIEN_DA_MO",
  PHIEN_DA_DONG: "PHIEN_DA_DONG",
  PHIEN_CHUA_MO: "PHIEN_CHUA_MO",
  DANG_MO: "DANG_MO",
  DA_DONG: "DA_DONG",
  THIEU_UID: "THIEU_UID",
  KHONG_TIM_THAY_SHEET: "KHONG_TIM_THAY_SHEET",
  LOI_APPS_SCRIPT: "LOI_APPS_SCRIPT"
});

const CAU_TRUC_SHEET = Object.freeze({
  SinhVien: ["UID", "MSSV", "HoTen", "Lop"],
  GiaoVien: ["UID", "MaGV", "HoTen", "BoMon"],
  PhienHoc: [
    "MaBuoi",
    "UIDGiaoVien",
    "MaGV",
    "HoTenGiaoVien",
    "ThoiGianMo",
    "ThoiGianDong",
    "TrangThai"
  ],
  DiemDanh: [
    "ThoiGian",
    "UID",
    "MSSV",
    "HoTen",
    "Lop",
    "TrangThai",
    "MaBuoi"
  ]
});

const THOI_GIAN_CHO_KHOA_MS = 5000;
const THOI_GIAN_CHONG_QUET_LAP_GV_MS = 3000;
const KHOA_QUET_GIAO_VIEN_GAN_NHAT =
  "QUET_GIAO_VIEN_GAN_NHAT_V2";
const O_CHON_PHIEN_DASHBOARD = "C3";
const O_NUT_XEM_PHIEN = "F3";
const O_PHIEN_DANG_XEM = "K5";
const O_PHIEN_CHON_THU_CONG = "K6";


function onOpen() {
  SpreadsheetApp
    .getUi()
    .createMenu("RFID")
    .addItem("Cau hinh Dashboard", "cauHinhDashboard")
    .addItem("Xem phien da chon", "xemPhienDaChon")
    .addItem("Ve phien hien tai / gan nhat", "xemPhienMacDinh")
    .addToUi();

  cauHinhDashboard();
}


function onEdit(e) {
  if (
    !e
    || !e.range
    || e.range.getSheet().getName() != "Dashboard"
    || e.range.getA1Notation() != O_NUT_XEM_PHIEN
    || String(e.value).toUpperCase() != "TRUE"
  ) {
    return;
  }

  xemPhienDaChon();
}


function cauHinhDashboard() {
  const ss = SpreadsheetApp.getActiveSpreadsheet();
  const dashboard = ss.getSheetByName("Dashboard");
  const phienHoc = ss.getSheetByName(TEN_SHEET.PHIEN_HOC);

  if (dashboard == null || phienHoc == null) {
    return;
  }

  const soDongDanhSach = Math.max(
    phienHoc.getMaxRows() - 1,
    1
  );
  const danhSachMaBuoi = phienHoc.getRange(
    2,
    1,
    soDongDanhSach,
    1
  );
  const quyTacChonPhien = SpreadsheetApp
    .newDataValidation()
    .requireValueInRange(danhSachMaBuoi, true)
    .setAllowInvalid(false)
    .build();

  dashboard
    .getRange(O_CHON_PHIEN_DASHBOARD)
    .setDataValidation(quyTacChonPhien);

  dashboard
    .getRange(O_NUT_XEM_PHIEN)
    .insertCheckboxes()
    .setValue(false);

  dashboard
    .getRange(
      9,
      1,
      Math.max(dashboard.getLastRow() - 8, 1),
      6
    )
    .clearContent();

  const oChonPhien = dashboard.getRange(O_CHON_PHIEN_DASHBOARD);
  const maBuoiDangXem = dashboard
    .getRange(O_PHIEN_DANG_XEM)
    .getDisplayValue()
    .trim();

  if (
    oChonPhien.getDisplayValue().trim() == ""
    && maBuoiDangXem != ""
  ) {
    oChonPhien.setValue(maBuoiDangXem);
  }

  SpreadsheetApp.flush();
  capNhatDanhSachDashboard(ss);
}


function xemPhienDaChon() {
  const ss = SpreadsheetApp.getActiveSpreadsheet();
  const dashboard = ss.getSheetByName("Dashboard");
  const phienHoc = ss.getSheetByName(TEN_SHEET.PHIEN_HOC);

  if (dashboard == null || phienHoc == null) {
    return;
  }

  const oNut = dashboard.getRange(O_NUT_XEM_PHIEN);
  const maBuoi = dashboard
    .getRange(O_CHON_PHIEN_DASHBOARD)
    .getDisplayValue()
    .trim();

  if (maBuoi == "") {
    dashboard
      .getRange(O_PHIEN_CHON_THU_CONG)
      .clearContent();
    oNut.setValue(false);
    ss.toast("Da ve phien hien tai / gan nhat", "Dashboard", 3);
    return;
  }

  if (!maBuoiTonTai(phienHoc, maBuoi)) {
    oNut.setValue(false);
    ss.toast("Ma buoi khong ton tai", "Dashboard", 3);
    return;
  }

  dashboard
    .getRange(O_PHIEN_CHON_THU_CONG)
    .setValue(maBuoi);

  oNut.setValue(false);
  SpreadsheetApp.flush();
  capNhatDanhSachDashboard(ss);
  ss.toast("Dang xem " + maBuoi, "Dashboard", 3);
}


function xemPhienMacDinh() {
  const ss = SpreadsheetApp.getActiveSpreadsheet();
  const dashboard = ss.getSheetByName("Dashboard");

  if (dashboard == null) {
    return;
  }

  dashboard
    .getRange(O_PHIEN_CHON_THU_CONG)
    .clearContent();

  SpreadsheetApp.flush();

  const maBuoi = dashboard
    .getRange(O_PHIEN_DANG_XEM)
    .getDisplayValue()
    .trim();

  dashboard
    .getRange(O_CHON_PHIEN_DASHBOARD)
    .setValue(maBuoi);

  dashboard
    .getRange(O_NUT_XEM_PHIEN)
    .setValue(false);

  SpreadsheetApp.flush();
  capNhatDanhSachDashboard(ss);
  ss.toast("Da ve phien hien tai / gan nhat", "Dashboard", 3);
}


function maBuoiTonTai(sheetPhienHoc, maBuoi) {
  const dongCuoi = sheetPhienHoc.getLastRow();

  if (dongCuoi < 2) {
    return false;
  }

  const danhSach = sheetPhienHoc
    .getRange(2, 1, dongCuoi - 1, 1)
    .getDisplayValues();

  for (let i = 0; i < danhSach.length; i++) {
    if (String(danhSach[i][0]).trim() == maBuoi) {
      return true;
    }
  }

  return false;
}


function capNhatDanhSachDashboard(ss) {
  const dashboard = ss.getSheetByName("Dashboard");
  const diemDanh = ss.getSheetByName(TEN_SHEET.DIEM_DANH);

  if (dashboard == null || diemDanh == null) {
    return;
  }

  const dongBatDau = 9;
  const soCotHienThi = 6;
  const soDongCanXoa = Math.max(
    dashboard.getLastRow() - dongBatDau + 1,
    1
  );

  dashboard
    .getRange(
      dongBatDau,
      1,
      soDongCanXoa,
      soCotHienThi
    )
    .clearContent();

  SpreadsheetApp.flush();

  const maBuoi = dashboard
    .getRange(O_PHIEN_DANG_XEM)
    .getDisplayValue()
    .trim();
  const dongCuoiDiemDanh = diemDanh.getLastRow();

  if (maBuoi == "" || dongCuoiDiemDanh < 2) {
    return;
  }

  const vungDiemDanh = diemDanh.getRange(
    2,
    1,
    dongCuoiDiemDanh - 1,
    7
  );
  const duLieuGoc = vungDiemDanh.getValues();
  const duLieuHienThi = vungDiemDanh.getDisplayValues();
  const danhSach = [];

  for (let i = 0; i < duLieuHienThi.length; i++) {
    if (String(duLieuHienThi[i][6]).trim() != maBuoi) {
      continue;
    }

    const thoiGian = duLieuGoc[i][0];

    danhSach.push({
      mocSapXep: thoiGian instanceof Date
        ? thoiGian.getTime()
        : i,
      soDong: i,
      giaTri: duLieuHienThi[i].slice(0, soCotHienThi)
    });
  }

  danhSach.sort(function(a, b) {
    if (a.mocSapXep != b.mocSapXep) {
      return b.mocSapXep - a.mocSapXep;
    }

    return b.soDong - a.soDong;
  });

  if (danhSach.length == 0) {
    return;
  }

  const dongCuoiCanDung = dongBatDau + danhSach.length - 1;

  if (dongCuoiCanDung > dashboard.getMaxRows()) {
    dashboard.insertRowsAfter(
      dashboard.getMaxRows(),
      dongCuoiCanDung - dashboard.getMaxRows()
    );
  }

  dashboard
    .getRange(
      dongBatDau,
      1,
      danhSach.length,
      soCotHienThi
    )
    .setValues(
      danhSach.map(function(dong) {
        return dong.giaTri;
      })
    );

  SpreadsheetApp.flush();
}


function doGet(e) {
  let khoa = null;

  try {
    const uid = chuanHoaUID(
      e && e.parameter
        ? e.parameter.uid
        : ""
    );

    if (uid == "") {
      return traJSON({
        success: false,
        status: TRANG_THAI.THIEU_UID
      });
    }

    khoa = LockService.getScriptLock();

    if (!khoa.tryLock(THOI_GIAN_CHO_KHOA_MS)) {
      return traJSON({
        success: false,
        status: TRANG_THAI.LOI_APPS_SCRIPT,
        message: "He thong dang xu ly mot lan quet khac, vui long quet lai"
      });
    }

    const ss = SpreadsheetApp.getActiveSpreadsheet();

    if (ss == null) {
      return traJSON({
        success: false,
        status: TRANG_THAI.LOI_APPS_SCRIPT,
        message: "Code.gs chua duoc gan voi Google Sheet"
      });
    }

    const cacSheet = layCacSheet(ss);

    if (cacSheet.thieu.length > 0) {
      return traJSON({
        success: false,
        status: TRANG_THAI.KHONG_TIM_THAY_SHEET,
        message: "Thieu sheet: " + cacSheet.thieu.join(", ")
      });
    }

    const loiCauTruc = kiemTraCauTrucCacSheet(cacSheet);

    if (loiCauTruc != "") {
      return traJSON({
        success: false,
        status: TRANG_THAI.LOI_APPS_SCRIPT,
        message: loiCauTruc
      });
    }

    const giaoVien = timGiaoVienTheoUID(
      cacSheet.giaoVien,
      uid
    );

    if (giaoVien != null) {
      const phanHoiLap = layPhanHoiGiaoVienGanNhat(uid);

      if (phanHoiLap != null) {
        phanHoiLap.quetLap = true;
        return traJSON(phanHoiLap);
      }

      const phanHoiGiaoVien = xuLyTheGiaoVien(
        ss,
        cacSheet.phienHoc,
        giaoVien
      );

      if (phanHoiGiaoVien.success) {
        luuPhanHoiGiaoVienGanNhat(
          uid,
          phanHoiGiaoVien
        );
      }

      return traJSON(phanHoiGiaoVien);
    }

    const sinhVien = timSinhVienTheoUID(
      cacSheet.sinhVien,
      uid
    );

    if (sinhVien == null) {
      return traJSON({
        success: false,
        uid: uid,
        status: TRANG_THAI.THE_KHONG_HOP_LE
      });
    }

    return traJSON(
      xuLyTheSinhVien(
        ss,
        cacSheet.phienHoc,
        cacSheet.diemDanh,
        sinhVien
      )
    );
  } catch (loi) {
    console.error(loi);

    return traJSON({
      success: false,
      status: TRANG_THAI.LOI_APPS_SCRIPT,
      message: layThongBaoLoi(loi)
    });
  } finally {
    if (khoa != null && khoa.hasLock()) {
      khoa.releaseLock();
    }
  }
}


function layCacSheet(ss) {
  const ketQua = {
    sinhVien: ss.getSheetByName(TEN_SHEET.SINH_VIEN),
    giaoVien: ss.getSheetByName(TEN_SHEET.GIAO_VIEN),
    phienHoc: ss.getSheetByName(TEN_SHEET.PHIEN_HOC),
    diemDanh: ss.getSheetByName(TEN_SHEET.DIEM_DANH),
    thieu: []
  };

  if (ketQua.sinhVien == null) {
    ketQua.thieu.push(TEN_SHEET.SINH_VIEN);
  }

  if (ketQua.giaoVien == null) {
    ketQua.thieu.push(TEN_SHEET.GIAO_VIEN);
  }

  if (ketQua.phienHoc == null) {
    ketQua.thieu.push(TEN_SHEET.PHIEN_HOC);
  }

  if (ketQua.diemDanh == null) {
    ketQua.thieu.push(TEN_SHEET.DIEM_DANH);
  }

  return ketQua;
}


function kiemTraCauTrucCacSheet(cacSheet) {
  const danhSach = [
    [cacSheet.sinhVien, TEN_SHEET.SINH_VIEN],
    [cacSheet.giaoVien, TEN_SHEET.GIAO_VIEN],
    [cacSheet.phienHoc, TEN_SHEET.PHIEN_HOC],
    [cacSheet.diemDanh, TEN_SHEET.DIEM_DANH]
  ];

  for (let i = 0; i < danhSach.length; i++) {
    const sheet = danhSach[i][0];
    const tenSheet = danhSach[i][1];
    const tieuDeMongDoi = CAU_TRUC_SHEET[tenSheet];
    const tieuDeThucTe = sheet
      .getRange(1, 1, 1, tieuDeMongDoi.length)
      .getDisplayValues()[0];

    for (let cot = 0; cot < tieuDeMongDoi.length; cot++) {
      if (String(tieuDeThucTe[cot]).trim() != tieuDeMongDoi[cot]) {
        return (
          "Sai cau truc sheet "
          + tenSheet
          + " tai cot "
          + (cot + 1)
          + ". Can tieu de: "
          + tieuDeMongDoi[cot]
        );
      }
    }
  }

  return "";
}


function timGiaoVienTheoUID(sheet, uid) {
  const dong = timDongTheoUID(sheet, uid, 4);

  if (dong == null) {
    return null;
  }

  return {
    uid: uid,
    uidLuu: dong[0],
    maGV: dong[1],
    hoTen: dong[2],
    boMon: dong[3]
  };
}


function timSinhVienTheoUID(sheet, uid) {
  const dong = timDongTheoUID(sheet, uid, 4);

  if (dong == null) {
    return null;
  }

  return {
    uid: uid,
    uidLuu: dong[0],
    mssv: dong[1],
    hoTen: dong[2],
    lop: dong[3]
  };
}


function timDongTheoUID(sheet, uid, soCot) {
  const dongCuoi = sheet.getLastRow();

  if (dongCuoi < 2) {
    return null;
  }

  const duLieu = sheet
    .getRange(2, 1, dongCuoi - 1, soCot)
    .getDisplayValues();

  for (let i = 0; i < duLieu.length; i++) {
    if (chuanHoaUID(duLieu[i][0]) == uid) {
      return duLieu[i];
    }
  }

  return null;
}


function xuLyTheGiaoVien(ss, sheetPhienHoc, giaoVien) {
  const ketQuaTimPhien = timPhienDangMo(sheetPhienHoc);

  if (ketQuaTimPhien.loi != "") {
    return {
      success: false,
      status: TRANG_THAI.LOI_APPS_SCRIPT,
      message: ketQuaTimPhien.loi
    };
  }

  const thoiGian = new Date();

  if (ketQuaTimPhien.phien != null) {
    const phien = ketQuaTimPhien.phien;

    sheetPhienHoc
      .getRange(phien.soDong, 6)
      .setNumberFormat("dd/MM/yyyy HH:mm:ss")
      .setValue(thoiGian);

    sheetPhienHoc
      .getRange(phien.soDong, 7)
      .setValue(TRANG_THAI.DA_DONG);

    SpreadsheetApp.flush();
    capNhatDanhSachDashboard(ss);

    return taoPhanHoiGiaoVien(
      giaoVien,
      phien.maBuoi,
      TRANG_THAI.PHIEN_DA_DONG,
      false
    );
  }

  const maBuoi = taoMaBuoi(
    ss,
    sheetPhienHoc,
    thoiGian
  );
  const dongMoi = sheetPhienHoc.getLastRow() + 1;
  const vungMoi = sheetPhienHoc.getRange(dongMoi, 1, 1, 7);

  vungMoi.setNumberFormat("@");
  sheetPhienHoc
    .getRange(dongMoi, 5, 1, 2)
    .setNumberFormat("dd/MM/yyyy HH:mm:ss");

  vungMoi.setValues([[
    maBuoi,
    giaoVien.uidLuu,
    giaoVien.maGV,
    giaoVien.hoTen,
    thoiGian,
    "",
    TRANG_THAI.DANG_MO
  ]]);

  SpreadsheetApp.flush();
  capNhatDanhSachDashboard(ss);

  return taoPhanHoiGiaoVien(
    giaoVien,
    maBuoi,
    TRANG_THAI.PHIEN_DA_MO,
    true
  );
}


function taoPhanHoiGiaoVien(
  giaoVien,
  maBuoi,
  trangThai,
  phienDangMo
) {
  return {
    success: true,
    status: trangThai,
    uid: giaoVien.uid,
    mssv: giaoVien.maGV,
    maGV: giaoVien.maGV,
    hoTen: giaoVien.hoTen,
    lop: giaoVien.boMon,
    vaiTro: "GIAO_VIEN",
    maBuoi: maBuoi,
    phienDangMo: phienDangMo
  };
}


function xuLyTheSinhVien(
  ss,
  sheetPhienHoc,
  sheetDiemDanh,
  sinhVien
) {
  const ketQuaTimPhien = timPhienDangMo(sheetPhienHoc);

  if (ketQuaTimPhien.loi != "") {
    return {
      success: false,
      status: TRANG_THAI.LOI_APPS_SCRIPT,
      message: ketQuaTimPhien.loi
    };
  }

  if (ketQuaTimPhien.phien == null) {
    return taoPhanHoiSinhVien(
      sinhVien,
      TRANG_THAI.PHIEN_CHUA_MO,
      "",
      false
    );
  }

  const maBuoi = ketQuaTimPhien.phien.maBuoi;

  if (daDiemDanhTrongPhien(
    sheetDiemDanh,
    sinhVien.uid,
    maBuoi
  )) {
    return taoPhanHoiSinhVien(
      sinhVien,
      TRANG_THAI.DA_DIEM_DANH,
      maBuoi,
      false
    );
  }

  const dongMoi = sheetDiemDanh.getLastRow() + 1;
  const vungMoi = sheetDiemDanh.getRange(dongMoi, 1, 1, 7);

  vungMoi.setNumberFormat("@");
  sheetDiemDanh
    .getRange(dongMoi, 1)
    .setNumberFormat("dd/MM/yyyy HH:mm:ss");

  vungMoi.setValues([[
    new Date(),
    sinhVien.uidLuu,
    sinhVien.mssv,
    sinhVien.hoTen,
    sinhVien.lop,
    TRANG_THAI.DIEM_DANH_THANH_CONG,
    maBuoi
  ]]);

  SpreadsheetApp.flush();
  capNhatDanhSachDashboard(ss);

  return taoPhanHoiSinhVien(
    sinhVien,
    TRANG_THAI.DIEM_DANH_THANH_CONG,
    maBuoi,
    true
  );
}


function taoPhanHoiSinhVien(
  sinhVien,
  trangThai,
  maBuoi,
  thanhCong
) {
  return {
    success: thanhCong,
    status: trangThai,
    uid: sinhVien.uid,
    mssv: sinhVien.mssv,
    hoTen: sinhVien.hoTen,
    lop: sinhVien.lop,
    maBuoi: maBuoi
  };
}


function timPhienDangMo(sheetPhienHoc) {
  const dongCuoi = sheetPhienHoc.getLastRow();
  const ketQua = {
    phien: null,
    loi: ""
  };

  if (dongCuoi < 2) {
    return ketQua;
  }

  const duLieu = sheetPhienHoc
    .getRange(2, 1, dongCuoi - 1, 7)
    .getDisplayValues();

  for (let i = 0; i < duLieu.length; i++) {
    const trangThai = String(duLieu[i][6]).trim().toUpperCase();

    if (trangThai != TRANG_THAI.DANG_MO) {
      continue;
    }

    if (ketQua.phien != null) {
      ketQua.phien = null;
      ketQua.loi = "Co nhieu hon mot phien DANG_MO trong sheet PhienHoc";
      return ketQua;
    }

    ketQua.phien = {
      soDong: i + 2,
      maBuoi: String(duLieu[i][0]).trim(),
      uidGiaoVien: chuanHoaUID(duLieu[i][1])
    };
  }

  if (
    ketQua.phien != null
    && ketQua.phien.maBuoi == ""
  ) {
    ketQua.phien = null;
    ketQua.loi = "Phien DANG_MO dang bi thieu MaBuoi";
  }

  return ketQua;
}


function daDiemDanhTrongPhien(
  sheetDiemDanh,
  uid,
  maBuoi
) {
  const dongCuoi = sheetDiemDanh.getLastRow();

  if (dongCuoi < 2) {
    return false;
  }

  const duLieu = sheetDiemDanh
    .getRange(2, 1, dongCuoi - 1, 7)
    .getDisplayValues();

  for (let i = 0; i < duLieu.length; i++) {
    if (
      chuanHoaUID(duLieu[i][1]) == uid
      && String(duLieu[i][6]).trim() == maBuoi
    ) {
      return true;
    }
  }

  return false;
}


function taoMaBuoi(ss, sheetPhienHoc, thoiGian) {
  const muiGio = (
    ss.getSpreadsheetTimeZone()
    || Session.getScriptTimeZone()
    || "Asia/Ho_Chi_Minh"
  );
  const ngay = Utilities.formatDate(
    thoiGian,
    muiGio,
    "yyMMdd"
  );
  const tienTo = "B" + ngay + "-";
  const dongCuoi = sheetPhienHoc.getLastRow();
  let soThuTuLonNhat = 0;

  if (dongCuoi >= 2) {
    const danhSachMaBuoi = sheetPhienHoc
      .getRange(2, 1, dongCuoi - 1, 1)
      .getDisplayValues();
    const mauMaBuoi = new RegExp(
      "^" + tienTo + "([0-9]+)$"
    );

    for (let i = 0; i < danhSachMaBuoi.length; i++) {
      const ketQua = String(danhSachMaBuoi[i][0])
        .trim()
        .match(mauMaBuoi);

      if (ketQua != null) {
        soThuTuLonNhat = Math.max(
          soThuTuLonNhat,
          Number(ketQua[1])
        );
      }
    }
  }

  let soThuTu = String(soThuTuLonNhat + 1);

  while (soThuTu.length < 2) {
    soThuTu = "0" + soThuTu;
  }

  return tienTo + soThuTu;
}


function layPhanHoiGiaoVienGanNhat(uid) {
  const giaTri = PropertiesService
    .getScriptProperties()
    .getProperty(KHOA_QUET_GIAO_VIEN_GAN_NHAT);

  if (giaTri == null || giaTri == "") {
    return null;
  }

  try {
    const duLieu = JSON.parse(giaTri);
    const doTre = Date.now() - Number(duLieu.thoiGian);

    if (
      duLieu.uid == uid
      && doTre >= 0
      && doTre < THOI_GIAN_CHONG_QUET_LAP_GV_MS
      && duLieu.phanHoi
    ) {
      return duLieu.phanHoi;
    }
  } catch (loi) {
    console.warn(loi);
  }

  return null;
}


function luuPhanHoiGiaoVienGanNhat(uid, phanHoi) {
  PropertiesService
    .getScriptProperties()
    .setProperty(
      KHOA_QUET_GIAO_VIEN_GAN_NHAT,
      JSON.stringify({
        uid: uid,
        thoiGian: Date.now(),
        phanHoi: phanHoi
      })
    );
}


function chuanHoaUID(uid) {
  return String(uid || "")
    .replace(/[^0-9a-fA-F]/g, "")
    .toUpperCase();
}


function layThongBaoLoi(loi) {
  if (loi && loi.message) {
    return String(loi.message);
  }

  return String(loi || "Loi khong xac dinh");
}


function traJSON(duLieu) {
  return ContentService
    .createTextOutput(JSON.stringify(duLieu))
    .setMimeType(ContentService.MimeType.JSON);
}
