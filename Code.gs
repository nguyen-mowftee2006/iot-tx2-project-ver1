function doGet(e) {
  const uid = chuanHoaUID(e.parameter.uid);

  if (uid == "") {
    return traJSON({
      success: false,
      status: "THIEU_UID"
    });
  }

  const ss = SpreadsheetApp.getActiveSpreadsheet();
  const shSV = ss.getSheetByName("SinhVien");
  const shDD = ss.getSheetByName("DiemDanh");

  if (shSV == null || shDD == null) {
    return traJSON({
      success: false,
      status: "KHONG_TIM_THAY_SHEET"
    });
  }

  // Tạo tiêu đề nếu sheet DiemDanh đang trống
  if (shDD.getLastRow() == 0) {
    shDD.appendRow([
      "ThoiGian",
      "UID",
      "MSSV",
      "HoTen",
      "Lop",
      "TrangThai"
    ]);
  }

  const dsSV = shSV.getDataRange().getDisplayValues();
  let sv = null;

  // Tìm sinh viên theo UID
  for (let i = 1; i < dsSV.length; i++) {
    const uidSheet = chuanHoaUID(dsSV[i][0]);

    if (
      uid == uidSheet ||
      uid.replace(/^0+/, "") == uidSheet.replace(/^0+/, "")
    ) {
      sv = {
        uid: uid,
        mssv: dsSV[i][1],
        hoTen: dsSV[i][2],
        lop: dsSV[i][3]
      };

      break;
    }
  }

  if (sv == null) {
    return traJSON({
      success: false,
      uid: uid,
      status: "THE_KHONG_HOP_LE"
    });
  }

  const dsDD = shDD.getDataRange().getDisplayValues();

  // Kiểm tra điểm danh trùng
  for (let i = 1; i < dsDD.length; i++) {
    const uidDaQuet = chuanHoaUID(dsDD[i][1]);

    if (
      uid == uidDaQuet ||
      uid.replace(/^0+/, "") == uidDaQuet.replace(/^0+/, "")
    ) {
      return traJSON({
        success: false,
        uid: sv.uid,
        mssv: sv.mssv,
        hoTen: sv.hoTen,
        lop: sv.lop,
        status: "DA_DIEM_DANH"
      });
    }
  }

  // Ghi điểm danh mới
  const dongMoi = shDD.getLastRow() + 1;

  shDD.getRange(dongMoi, 1).setValue(new Date());

  shDD.getRange(dongMoi, 2)
    .setNumberFormat("@")
    .setValue(sv.uid);

  shDD.getRange(dongMoi, 3, 1, 4).setValues([[
    sv.mssv,
    sv.hoTen,
    sv.lop,
    "DIEM_DANH_THANH_CONG"
  ]]);

  return traJSON({
    success: true,
    uid: sv.uid,
    mssv: sv.mssv,
    hoTen: sv.hoTen,
    lop: sv.lop,
    status: "DIEM_DANH_THANH_CONG"
  });
}

function chuanHoaUID(uid) {
  return String(uid || "")
    .replace(/:/g, "")
    .replace(/ /g, "")
    .toUpperCase();
}

function traJSON(duLieu) {
  return ContentService
    .createTextOutput(JSON.stringify(duLieu))
    .setMimeType(ContentService.MimeType.JSON);
}