#include <iostream>
#include <string>
#include <ctime>
#include "QuanLyPhongKham.h"   // header chung: HoSoBenhNhan, TrangThai, ngaySinhHopLe, sdtHopLe, sinhMaBenhNhan
using namespace std;

// =====================================================================
// 1. CÁC HÀM KIỂM TRA (VALIDATE)
// =====================================================================
bool laSo(char c) { return c >= '0' && c <= '9'; }

bool validateMaBN(const string& ma) {
    if (ma.length() != 8 || ma[0] != 'B' || ma[1] != 'N') return false;
    for (int i = 2; i < 8; i++) if (!laSo(ma[i])) return false;
    return true;
}

bool validateHoTen(const string& ten) {
    if (ten.length() < 2 || ten.length() > 50) return false;
    for (char c : ten) if (laSo(c)) return false;
    return true;
}

bool validateNgaySinh(const string& s) {
    if (!ngaySinhHopLe(s)) return false;

    int ngay  = stoi(s.substr(0, 2));
    int thang = stoi(s.substr(3, 2));
    int nam   = stoi(s.substr(6, 4));

    time_t now = time(0);
    tm* t = localtime(&now);
    int homNay   = (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
    int ngaySinh = nam * 10000 + thang * 100 + ngay;
    return ngaySinh <= homNay;
}

bool validateSDT(const string& sdt) { return sdtHopLe(sdt); }

// =====================================================================
// 2. BẢNG BĂM (dò tuyến tính) DÙNG MẢNG ĐỘNG CHO 100.000 BẢN GHI
// =====================================================================
struct BangBam {
    static const int SIZE = 150007;          // số nguyên tố > 100.000 x 1.5
    static const int MAX_BAN_GHI = 100000;   // giới hạn số hồ sơ

    HoSoBenhNhan* danhSach;                  // Mảng động quản lý danh sách hồ sơ
    int soLuong = 0;
    int demMaTudong = 0;                     // số thứ tự đã dùng để sinh mã

    // Constructor: Cấp phát mảng động
    BangBam() {
        danhSach = new HoSoBenhNhan[SIZE];
    }

    // Destructor: Giải phóng bộ nhớ mảng động khi kết thúc
    ~BangBam() {
        delete[] danhSach;
    }

    // Băm chuỗi: duyệt từng ký tự bằng chỉ số
    int hamBam(const string& maBN) {
        unsigned int tong = 0;
        for (size_t i = 0; i < maBN.length(); i++) {
            tong = tong * 31 + (unsigned char)maBN[i];
        }
        return tong % SIZE;
    }

    // Tìm vị trí của mã trong bảng. Không có -> trả về -1.
    int timViTri(const string& maBN) {
        int viTri = hamBam(maBN);
        int soLanTim = 0;

        while (!danhSach[viTri].maBenhNhan.empty() && soLanTim < SIZE) {
            if (danhSach[viTri].maBenhNhan == maBN) return viTri;
            viTri++;
            if (viTri == SIZE) viTri = 0;
            soLanTim++;
        }
        return -1;
    }

    bool insertHoSo(const HoSoBenhNhan& h) {
        if (soLuong >= MAX_BAN_GHI) return false;

        if (!validateMaBN(h.maBenhNhan) || !validateHoTen(h.hoTen) ||
            !validateNgaySinh(h.ngaySinh) || !validateSDT(h.sdt)) {
            return false;
        }

        int viTri = hamBam(h.maBenhNhan);
        int soLanTim = 0;

        while (!danhSach[viTri].maBenhNhan.empty() && soLanTim < SIZE) {
            if (danhSach[viTri].maBenhNhan == h.maBenhNhan) return false; // trùng mã
            viTri++;
            if (viTri == SIZE) viTri = 0;
            soLanTim++;
        }

        if (soLanTim >= SIZE) return false; // bảng đầy

        danhSach[viTri] = h;
        soLuong++;
        return true;
    }

    // Hàm tra cứu logic: Ngày khám, Thông tin lịch hẹn, Trạng thái khám hiện tại
    string traCuu(const string& maBN) {
        if (!validateMaBN(maBN)) return "Mã bệnh nhân không hợp lệ.";

        int viTri = timViTri(maBN);
        if (viTri == -1) return "Khong tim thay ho so benh nhan.";

        HoSoBenhNhan& h = danhSach[viTri];

        string ngayKhamDisplay;
        string thongTinLichHen;
        string trangThaiKham;

        // Kiểm tra xem bệnh nhân có đặt lịch khám hoặc lịch tái khám không
        if (!h.ngayDatLich.empty()) {
            ngayKhamDisplay = h.ngayDatLich;
            thongTinLichHen = "co";
            trangThaiKham   = "chua kham";
        } else if (!h.ngayTaiKham.empty()) {
            ngayKhamDisplay = h.ngayTaiKham;
            thongTinLichHen = "co";
            trangThaiKham   = "chua kham";
        } else {
            ngayKhamDisplay = h.ngayKhamGanNhat;
            thongTinLichHen = "chua co";
            trangThaiKham   = "da kham";
        }

        return "Ma BN: " + h.maBenhNhan +
               " | Ten: " + h.hoTen +
               " | Ngay Sinh: " + h.ngaySinh +
               " | SDT: " + h.sdt +
               " | Ngay kham: " + ngayKhamDisplay +
               " | Thong tin lich hen: " + thongTinLichHen +
               " | Trang thai kham: " + trangThaiKham;
    }

    // Thêm hồ sơ mới: định dạng mã do hàm chung sinhMaBenhNhan(số) tạo
    bool themHoSoMoi(const string& hoTen, const string& ngaySinh,
                     const string& sdt, string& maMoi) {
        if (soLuong >= MAX_BAN_GHI) return false;

        int soMoi = demMaTudong + 1;

        HoSoBenhNhan h;
        h.maBenhNhan = sinhMaBenhNhan(soMoi);
        h.hoTen = hoTen;
        h.ngaySinh = ngaySinh;
        h.sdt = sdt;

        if (!insertHoSo(h)) return false;

        demMaTudong = soMoi;
        maMoi = h.maBenhNhan;
        return true;
    }

    // Gọi khám: chỉ cho phép GOI_KHAM -> DANG_KHAM
    bool batDauKham(const string& maBN) {
        if (!validateMaBN(maBN)) return false;

        int viTri = timViTri(maBN);
        if (viTri == -1) return false;
        if (danhSach[viTri].trangThai != TrangThai::GOI_KHAM) return false;

        danhSach[viTri].trangThai = TrangThai::DANG_KHAM;
        return true;
    }

    // Khám xong: chỉ cho phép DANG_KHAM -> DA_KHAM
    bool hoanThanhKham(const string& maBN) {
        if (!validateMaBN(maBN)) return false;

        int viTri = timViTri(maBN);
        if (viTri == -1) return false;
        if (danhSach[viTri].trangThai != TrangThai::DANG_KHAM) return false;

        danhSach[viTri].trangThai = TrangThai::DA_KHAM;
        return true;
    }
};