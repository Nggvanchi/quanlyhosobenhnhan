#include "QuanLyPhongKham.h"
#include <iostream>

using namespace std;

// ============================================================================
// PHẦN 1: KIỂM TRA ĐIỀU KIỆN VÀ QUẢN LÝ HÀNG ĐỢI CHỜ
// ============================================================================

// Kiểm tra ngày có nằm trong khoảng năm hợp lệ hay không.
bool NgayTruyXuatHopLe(const string &Ngay) {
  return NgayHopLe(Ngay, 1600, 2026);
}

// Kiểm tra lịch hẹn có thuộc đúng ngày cần tra cứu hay không.
bool LichHenThuocNgay(const LichHen &Lich, const string &Ngay) {
  return Lich.Ngay == Ngay;
}

// Kiểm tra mức ưu tiên hợp lệ (1: cao, 2: trung bình, 3: thấp).
bool MucUuTienHopLe(int Muc) {
  return Muc >= MUC_UU_TIEN_CAO && Muc <= MUC_UU_TIEN_THAP;
}

// Thêm bệnh nhân vào hàng đợi và tự cấp số thứ tự đăng ký nếu chưa có.
bool ThemBenhNhan(MangBenhNhan &HangDoi, HoSoBenhNhan BenhNhan) {
  if (!MucUuTienHopLe(BenhNhan.MucUuTien)) {
    cout << "Muc uu tien khong hop le.\n";
    return false;
  }
  static int ThuTuTiepTheo = 1;
  if (BenhNhan.ThuTuDangKy == 0) BenhNhan.ThuTuDangKy = ThuTuTiepTheo++;
  HangDoi.PushBack(BenhNhan);
  return true;
}

// Xóa bệnh nhân khỏi hàng đợi khi đã khám xong hoặc hủy lịch.
void XoaBenhNhanKhoiHangDoi(MangBenhNhan &HangDoi, const string &MaBN) {
  for (int i = 0; i < HangDoi.Size; ++i) {
    if (HangDoi[i].MaBenhNhan == MaBN) {
      HangDoi.Erase(i);
      return;
    }
  }
}

// ============================================================================
// PHẦN 2: SẮP XẾP THEO MỨC ƯU TIÊN (COUNTING SORT)
// ============================================================================

// Sắp xếp danh sách bệnh nhân theo mức ưu tiên 1, 2, 3 và giữ nguyên thứ tự
// đăng ký. Độ phức tạp thời gian: O(N) vì miền giá trị chỉ có 3 mức ưu tiên {1,
// 2, 3}. Tối ưu: Sắp xếp mảng chỉ số int theo Counting Sort ổn định.
static void CountingSortTheoUuTien(MangBenhNhan &DanhSach, MangDong<string> &GioHen) {
  if (DanhSach.Size <= 1) return;
  int Dem[4] = {}, ViTriBatDau[4] = {};

  // Đếm số lượng bệnh nhân theo từng mức ưu tiên.
  for (int i = 0; i < DanhSach.Size; ++i) Dem[DanhSach[i].MucUuTien]++;

  // Tính vị trí bắt đầu của từng nhóm ưu tiên.
  for (int m = 2; m <= 3; ++m) ViTriBatDau[m] = ViTriBatDau[m - 1] + Dem[m - 1];

  // Sắp xếp mảng chỉ số nguyên theo Counting Sort ổn định (chỉ di chuyển số
  // nguyên 4-byte).
  int *IdxKetQua = new int[DanhSach.Size];
  for (int i = 0; i < DanhSach.Size; ++i) IdxKetQua[ViTriBatDau[DanhSach[i].MucUuTien]++] = i;

  // Hoán chuyển dữ liệu vào danh sách theo thứ tự chỉ số.
  HoSoBenhNhan *KetQua = new HoSoBenhNhan[DanhSach.Size];
  string *GioKetQua = new string[DanhSach.Size];
  for (int i = 0; i < DanhSach.Size; ++i) {
    KetQua[i] = DanhSach[IdxKetQua[i]];
    GioKetQua[i] = GioHen[IdxKetQua[i]];
  }

  for (int i = 0; i < DanhSach.Size; ++i) {
    DanhSach[i] = KetQua[i];
    GioHen[i] = GioKetQua[i];
  }

  delete[] IdxKetQua;
  delete[] KetQua;
  delete[] GioKetQua;
}

// ============================================================================
// PHẦN 3: SO SÁNH ĐA TIÊU CHÍ VÀ HIỂN THỊ DANH SÁCH ƯU TIÊN
// ============================================================================

// So sánh 2 bệnh nhân theo đa tiêu chí ưu tiên:
// 1. Mức ưu tiên nhỏ hơn (1: Khẩn cấp > 2: Trung bình > 3: Thấp).
// 2. Nếu cùng mức: Ngày đăng ký mới hơn được ưu tiên khám trước.
// 3. Nếu cùng ngày: Số thứ tự đăng ký nhỏ hơn (đến trước) được ưu tiên trước.
static bool UuTienHon(const HoSoBenhNhan &A, const HoSoBenhNhan &B) {
  if (A.MucUuTien != B.MucUuTien) return A.MucUuTien < B.MucUuTien;
  long long NgayA = KhoaNgay(A.NgayDatLich), NgayB = KhoaNgay(B.NgayDatLich);
  if (NgayA != NgayB) return NgayA > NgayB;
  return A.ThuTuDangKy < B.ThuTuDangKy;
}

// Chức năng 4: In danh sách bệnh nhân có lịch trong ngày, sắp xếp theo mức ưu
// tiên bằng Counting Sort O(N).
void XemBenhNhanTheoMucUuTien(BangBam &HeThongHoSo, const QuanLyLichHen &QLLich, const string &Ngay) {
  MangBenhNhan DanhSachNgay;
  MangDong<string> GioHen;
  for (int i = 0; i < QLLich.Size(); ++i) {
    if (QLLich[i].Ngay == Ngay && QLLich[i].TrangThai != DA_HUY) {
      int v = TimViTri(HeThongHoSo, QLLich[i].MaBN);
      if (v != -1) {
        DanhSachNgay.PushBack(HeThongHoSo.DanhSach[v]);
        GioHen.PushBack(QLLich[i].KhungGio);
      }
    }
  }
  CountingSortTheoUuTien(DanhSachNgay, GioHen);
  cout << "\n===== BENH NHAN THEO MUC DO UU TIEN NGAY " << Ngay << " =====\n";
  for (int i = 0; i < DanhSachNgay.Size; ++i) {
    cout << i + 1 << ". " << DanhSachNgay[i].MaBenhNhan << " | "
         << DanhSachNgay[i].HoTen << " | Gio hen: " << GioHen[i]
         << " | Uu tien: " << DanhSachNgay[i].MucUuTien
         << " | Trang thai: " << DanhSachNgay[i].TrangThai << "\n";
  }
  if (DanhSachNgay.Empty()) cout << "Khong co benh nhan nao co lich vao ngay nay.\n";
}

// Thuật toán DSA: Duyệt tuyến tính O(N) tìm vị trí bệnh nhân có mức ưu tiên cao nhất
int TimViTriUuTienCaoNhat(const MangBenhNhan &HangDoi) {
  if (HangDoi.Empty()) return -1;
  int ViTriTotNhat = -1;
  for (int i = 0; i < HangDoi.Size; ++i) {
    if (HangDoi[i].TrangThai != CHO_KHAM) continue;
    if (ViTriTotNhat == -1 || UuTienHon(HangDoi[i], HangDoi[ViTriTotNhat])) ViTriTotNhat = i;
  }
  return ViTriTotNhat;
}

// Chức năng 5: Xác định bệnh nhân có mức độ ưu tiên cao nhất bằng thuật toán
// duyệt tìm cực trị O(N). Tránh over-engineering: Không dùng Min-Heap, chỉ
// duyệt tuyến tính 1 lượt để tìm ra người tối ưu nhất.
void XuLyUuTienCaoNhat(const MangBenhNhan &HangDoi) {
  if (HangDoi.Empty()) {
    cout << "=> Hang doi uu tien dang trong!\n";
    return;
  }

  int ViTriTotNhat = TimViTriUuTienCaoNhat(HangDoi);
  if (ViTriTotNhat == -1) {
    cout << "Khong co benh nhan nao dang cho kham.\n";
    return;
  }

  const HoSoBenhNhan &HoSo = HangDoi[ViTriTotNhat];
  cout << "\n===== BENH NHAN CO MUC UU TIEN CAO NHAT =====\n"
       << "1. " << HoSo.MaBenhNhan << " | " << HoSo.HoTen
       << " | Uu tien: " << HoSo.MucUuTien
       << " | Ngay dang ky: " << HoSo.NgayDatLich
       << " | Thu tu: " << HoSo.ThuTuDangKy << " | Trang thai: " << HoSo.TrangThai
       << "\n";
}
