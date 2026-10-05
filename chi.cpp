#include <iostream>
#include "QuanLyPhongKham.h"

using namespace std;

// ============================================================================
// PHẦN 1: DỮ LIỆU LỊCH HẸN VÀ HÀM HỖ TRỢ
// ============================================================================

// In thông tin chi tiết một dòng lịch hẹn ra màn hình.
static void InMotDongLich(const LichHen& Lich, const HoSoBenhNhan& HoSo, int STT = 0) {
    if (STT > 0) cout << "STT dang ky: " << STT << " | ";
    cout << Lich.MaBN << " | " << HoSo.HoTen << " | " << Lich.KhungGio
         << " | Uu tien: " << HoSo.MucUuTien << " | Trang thai: " << HoSo.TrangThai << "\n";
}

// ============================================================================
// PHẦN 2: TÌM KIẾM NHỊ PHÂN (BINARY SEARCH) VÀ THÊM LỊCH
// ============================================================================

// Lower Bound: vị trí đầu tiên có (ngày, giờ) >= khóa tìm kiếm, O(log n).
int TimViTriDauTienTheoNgayGio(const QuanLyLichHen& QLLich, const string& Ngay, int Phut) {
    int lo = 0, hi = QLLich.IdxTheoGio.Size;
    long long targetKhoa = KhoaNgay(Ngay) * 1440 + Phut;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        const LichHen& Lich = QLLich.DanhSach[QLLich.IdxTheoGio[mid]];
        long long midKhoa = KhoaNgay(Lich.Ngay) * 1440 + SoPhutTrongNgay(Lich.KhungGio);
        if (midKhoa < targetKhoa) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// Upper Bound: vị trí đầu tiên có (ngày, giờ) > khóa tìm kiếm, O(log n).
int TimViTriDauTienSauNgayGio(const QuanLyLichHen& QLLich, const string& Ngay, int Phut) {
    int lo = 0, hi = QLLich.IdxTheoGio.Size;
    long long targetKhoa = KhoaNgay(Ngay) * 1440 + Phut;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        const LichHen& Lich = QLLich.DanhSach[QLLich.IdxTheoGio[mid]];
        long long midKhoa = KhoaNgay(Lich.Ngay) * 1440 + SoPhutTrongNgay(Lich.KhungGio);
        if (midKhoa <= targetKhoa) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// Thêm lịch vào danh sách gốc và chèn chỉ số vào mảng thời gian đã sắp xếp.
void ThemLich(QuanLyLichHen& QLLich, const LichHen& Lich) {
    QLLich.DanhSach.PushBack(Lich);
    int ViTri = TimViTriDauTienTheoNgayGio(QLLich, Lich.Ngay, SoPhutTrongNgay(Lich.KhungGio));
    QLLich.IdxTheoGio.PushBack(0);
    for (int i = QLLich.IdxTheoGio.Size - 1; i > ViTri; --i) QLLich.IdxTheoGio[i] = QLLich.IdxTheoGio[i - 1];
    QLLich.IdxTheoGio[ViTri] = QLLich.DanhSach.Size - 1;
}

// Kiểm tra bệnh nhân còn lịch hẹn ở trạng thái hoạt động ("Cho kham" hoặc "Dang kham").
bool CoLichHoatDong(const QuanLyLichHen& QLLich, const string& MaBN) {
    for (int i = 0; i < QLLich.DanhSach.Size; ++i)
        if (QLLich.DanhSach[i].MaBN == MaBN && (QLLich.DanhSach[i].TrangThai == CHO_KHAM || QLLich.DanhSach[i].TrangThai == DANG_KHAM)) return true;
    return false;
}

// ============================================================================
// PHẦN 3: CHỨC NĂNG TRA CỨU LỊCH HẸN
// ============================================================================

// Chức năng 3: Tìm và in danh sách lịch hẹn trong khoảng giờ [gioBD, gioKT] của một ngày.
void LayTheoKhoang(const QuanLyLichHen& QLLich, BangBam& HeThongHoSo, const string& Ngay, const string& GioBD, const string& GioKT) {
    if (!NgayTruyXuatHopLe(Ngay) || !KhungGioHopLe(GioBD) || !KhungGioHopLe(GioKT) || GioBD > GioKT) {
        cout << "Ngay hoac khoang gio khong hop le. Vui long nhap ngay DD/MM/YYYY va gio HH:MM (bat dau <= ket thuc).\n";
        return;
    }
    int L = TimViTriDauTienTheoNgayGio(QLLich, Ngay, SoPhutTrongNgay(GioBD));
    int R = TimViTriDauTienSauNgayGio(QLLich, Ngay, SoPhutTrongNgay(GioKT));
    bool CoKetQua = false;

    for (int i = L; i < R; ++i) {
        const LichHen& Lich = QLLich.DanhSach[QLLich.IdxTheoGio[i]];
        if (Lich.TrangThai != DA_HUY) {
            int v = TimViTri(HeThongHoSo, Lich.MaBN);
            if (v >= 0) { InMotDongLich(Lich, HeThongHoSo.DanhSach[v]); CoKetQua = true; }
        }
    }
    if (!CoKetQua) cout << "Khong co lich hen nao trong khoang gio da chon vao ngay " << Ngay << ".\n";
}

// Chức năng 2: In danh sách bệnh nhân có lịch hẹn trong ngày theo thứ tự đăng ký ban đầu.
void XemTheoThuTuDangKy(const QuanLyLichHen& QLLich, BangBam& HeThongHoSo, const string& Ngay) {
    if (!NgayTruyXuatHopLe(Ngay)) { cout << "Ngay khong hop le. Vui long nhap theo dinh dang DD/MM/YYYY.\n"; return; }
    int STT = 0;
    for (int i = 0; i < QLLich.DanhSach.Size; ++i) {
        const LichHen& Lich = QLLich.DanhSach[i];
        if (LichHenThuocNgay(Lich, Ngay) && Lich.TrangThai != DA_HUY) {
            int v = TimViTri(HeThongHoSo, Lich.MaBN);
            if (v >= 0) InMotDongLich(Lich, HeThongHoSo.DanhSach[v], ++STT);
        }
    }
    if (!STT) cout << "Khong co benh nhan dang ky vao ngay " << Ngay << ".\n";
}
