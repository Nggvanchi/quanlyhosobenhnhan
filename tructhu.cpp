#include <iostream>
#include <string>
#include "QuanLyPhongKham.h"

using namespace std;

// ============================================================================
// PHẦN 1: QUẢN LÝ LỊCH SỬ KHÁM BỆNH(MEDICAL HISTORY)
// Lưu trữ các lượt khám bệnh đã hoàn tất của bệnh nhân vào mảng động MangLichSu.
// Chi phí thêm mới phần tử vào mảng là O(1) amortized.
// ============================================================================

// Thêm một lượt khám bệnh mới vào cuối mảng động lịch sử.
// Nhận vào mã bệnh nhân, ngày khám, nội dung/chuyên khoa khám và trạng thái("Kham xong").
void ThemLuotKham(MangLichSu &LichSu, const string &MaBN, const string &Ngay, const string &NoiDung, const string &TrangThai) {
    LichSu.PushBack({MaBN, Ngay, NoiDung, TrangThai});
}

// Duyệt và in toàn bộ lịch sử các lần khám bệnh của bệnh nhân theo mã định danh.
// Nếu bệnh nhân chưa từng khám lần nào, thông báo "Chua co lich su kham".
void InLichSu(const MangLichSu &LichSu, const string &MaBN) {
    int Dem = 0;
    for(int i = 0; i < LichSu.Size; ++i) {
        if(LichSu[i].MaBN != MaBN) continue;
        cout << "Ngay: " << LichSu[i].Ngay << "\nNoi dung: " << LichSu[i].NoiDung
             << "\nTrang thai: " << LichSu[i].TrangThai << "\n--------------------\n";
        ++Dem;
    }
    if(!Dem) cout << "Chua co lich su kham.\n";
}

// Logic nghiệp vụ DSA: Ghi nhận bệnh nhân hoàn tất khám bệnh, cập nhật trạng thái trong bảng băm,
// thêm lượt khám vào lịch sử và cập nhật hàng đợi ưu tiên.
bool GhiNhanKhamXong(BangBam &HeThongHoSo, QuanLyLichHen& QLLich, MangLichSu &LichSu, MangBenhNhan &HangDoiUuTien, const string &MaBN) {
    int ViTriHoSo = TimViTri(HeThongHoSo, MaBN);
    if (ViTriHoSo == -1) return false;

    string Ngay, ChuyenKhoa;
    // Chỉ bệnh nhân có lịch ở trạng thái "Dang kham" mới được ghi nhận khám xong.
    if (!DanhDauDaKham(QLLich, MaBN, Ngay, ChuyenKhoa)) {
        return false;
    }

    // Ghi lượt khám đã hoàn tất vào mảng lịch sử chung.
    ThemLuotKham(LichSu, MaBN, Ngay, ChuyenKhoa, DA_KHAM);

    // Cập nhật trạng thái và ngày khám gần nhất trong bảng băm hồ sơ.
    HeThongHoSo.DanhSach[ViTriHoSo].TrangThai = DA_KHAM;
    HeThongHoSo.DanhSach[ViTriHoSo].NgayKhamGanNhat = Ngay;
    HeThongHoSo.DanhSach[ViTriHoSo].NgayDatLich.clear();

    // Đồng bộ các lịch liên quan rồi loại bệnh nhân khỏi hàng đợi nếu không còn lịch hoạt động.
    DongBoTrangThaiHoSo(HeThongHoSo, QLLich, MaBN);
    if (!CoLichHoatDong(QLLich, MaBN)) XoaBenhNhanKhoiHangDoi(HangDoiUuTien, MaBN);
    return true;
}

// Chức năng 7: Điều phối xem lịch sử hoặc ghi nhận hoàn tất khám bệnh:
// - Tùy chọn 1: Xem toàn bộ các lần khám trong quá khứ của bệnh nhân.
// - Tùy chọn 2: Ghi nhận bệnh nhân đã khám xong:
//   + Tự động xác định lịch hẹn đang ở trạng thái "Dang kham" theo mã BN (thông qua DanhDauDaKham).
//   + Ghi nhận lượt khám hoàn tất vào danh sách lịch sử chung.
//   + Cập nhật trạng thái "Kham xong" và ngày khám gần nhất vào hồ sơ trong bảng băm.
//   + Đồng bộ lại trạng thái và xóa bệnh nhân khỏi hàng đợi ưu tiên chờ khám.
void XuLyLichSu(BangBam &HeThongHoSo, QuanLyLichHen &QLLich, MangLichSu &LichSu, MangBenhNhan &HangDoiUuTien, const string &MaBN) {
    int ViTriHoSo = TimViTri(HeThongHoSo, MaBN);
    if(ViTriHoSo == -1) { cout << "=> Khong ton tai ma benh nhan nay.\n"; return; }
    int Chon;
    cout << "1. Xem lich su kham\n2. Ghi nhan kham xong\nChon thao tac: ";
    if(!(cin >> Chon)) {
        cin.clear(); cin.ignore(10000, '\n'); 
        cout << "Lua chon khong hop le.\n"; 
        return; 
    }

    if(Chon == 2) { // Ghi nhận khám xong
        if (GhiNhanKhamXong(HeThongHoSo, QLLich, LichSu, HangDoiUuTien, MaBN)) {
            cout << "=> Da ghi nhan lich su kham.\n";
        } else {
            cout << "=> Khong tim thay lich dang kham.\n";
        }
    } else if(Chon == 1) { // Xem lịch sử
        cout << "\n===== LICH SU KHAM CUA BENH NHAN: " << MaBN << " =====\n";
        InLichSu(LichSu, MaBN);
    } else cout << "Lua chon khong hop le.\n";
}

// ============================================================================
// PHẦN 2: QUẢN LÝ NHẮC LỊCH TÁI KHÁM(FOLLOW-UP APPOINTMENT)
// Lưu trữ các cuộc hẹn tái khám kèm ngày hẹn, giờ hẹn và ghi chú dặn dò của bác sĩ.
// ============================================================================

// Chức năng 8: Điều phối quản lý lịch hẹn tái khám:
// - Tùy chọn 1: Tạo lịch hẹn tái khám mới(kiểm tra mã BN, ngày hẹn, giờ hẹn, nội dung).
// - Tùy chọn 2: Hiển thị toàn bộ danh sách nhắc hẹn tái khám đã lưu trong hệ thống.
void XuLyNhacTaiKham(MangLichTaiKham &DSTaiKham, BangBam &HeThongHoSo) {
    int Chon;
    cout << "\n--- QUAN LY NHAC LICH TAI KHAM ---\n1. Tao lich hen tai kham moi\n2. Xem danh sach nhac tai kham\nChon thao tac: ";
    if(!(cin >> Chon)) {
        cin.clear(); cin.ignore(10000, '\n'); 
        cout << "Lua chon khong hop le.\n"; 
        return; 
    }

    if(Chon == 1) { // Tạo lịch tái khám
        LichTaiKham Lich;
        cout << "Nhap Ma BN: "; cin >> Lich.MaBN;
        int ViTri = TimViTri(HeThongHoSo, Lich.MaBN);
        if(ViTri == -1) { 
            cout << "=> Khong tim thay benh nhan.\n"; 
            return; 
        }
        cout << "Nhap Ngay tai kham: "; 
        cin >> Lich.NgayTaiKham;
        if(!NgayKhamHopLe(Lich.NgayTaiKham)) { 
            cout << "=> Ngay tai kham khong hop le.\n"; 
            return; 
        }
        cout << "Nhap gio: "; cin >> Lich.Gio;
        if(!GioKhamHopLe(Lich.Gio)) { 
            cout << "=> Gio tai kham khong hop le.\n"; 
            return; 
        }
        cout << "Nhap noi dung: ";
        cin.ignore(10000, '\n');
        getline(cin, Lich.NoiDung);
        DSTaiKham.PushBack(Lich);
        HeThongHoSo.DanhSach[ViTri].NgayTaiKham = Lich.NgayTaiKham;
        cout << "=> Da dang ky nhac lich.\n";
    } else if(Chon == 2) { // Xem toàn bộ danh sách nhắc tái khám
        if(DSTaiKham.Empty()) {
            cout << "=> Chua co lich tai kham.\n"; 
            return; 
        }
        for(int i = 0; i < DSTaiKham.Size; ++i)
            cout << i + 1 << ". " << DSTaiKham[i].MaBN << " | " << DSTaiKham[i].NgayTaiKham
                 << " | " << DSTaiKham[i].Gio << " | " << DSTaiKham[i].NoiDung << "\n";
    }
}
