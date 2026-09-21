#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "QuanLyPhongKham.h" 

using namespace std;

// =====================================================================
// 1. CAC HAM KIEM TRA (VALIDATE)
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

// ngaySinhHopLe() cua header lo phan dinh dang + dung lich (30/02, nam nhuan...).
// Nhung no chi chan theo NAM (1600..2026), nen ngay sau hom nay trong nam nay van lot qua
// -> ta tu them luat "khong lon hon hom nay".
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
// 2. BANG BAM (do tuyen tinh) CHO 100.000 BAN GHI
// =====================================================================
struct BangBam {
    static const int SIZE = 150007;          // so nguyen to > 100.000 x 1.5
    static const int MAX_BAN_GHI = 100000;   // gioi han so ho so

    vector<HoSoBenhNhan> danhSach;           // vector tu cap phat + tu giai phong
    int soLuong = 0;
    int demMaTudong = 0;                     // so thu tu da dung de sinh ma

    BangBam() : danhSach(SIZE) {}            // tao san SIZE o trong

    // Bam chuoi: duyet tung ky tu bang chi so
    int hamBam(const string& maBN) {
        unsigned int tong = 0;
        for (size_t i = 0; i < maBN.length(); i++) {
            tong = tong * 31 + (unsigned char)maBN[i];
        }
        return tong % SIZE;
    }

    // Tim vi tri cua ma trong bang. Khong co -> tra ve -1.
    // (traCuu, batDauKham, hoanThanhKham deu dung chung ham nay)
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
            if (danhSach[viTri].maBenhNhan == h.maBenhNhan) return false; // trung ma
            viTri++;
            if (viTri == SIZE) viTri = 0;
            soLanTim++;
        }

        if (soLanTim >= SIZE) return false; // bang day

        danhSach[viTri] = h;
        soLuong++;
        return true;
    }

    string traCuu(const string& maBN) {
        if (!validateMaBN(maBN)) return "Ma benh nhan khong hop le.";

        int viTri = timViTri(maBN);
        if (viTri == -1) return "Khong tim thay ho so benh nhan.";

        HoSoBenhNhan& h = danhSach[viTri];
        return "Ma BN: " + h.maBenhNhan +
               " | Ten: " + h.hoTen +
               " | Ngay Sinh: " + h.ngaySinh +
               " | SDT: " + h.sdt +
               " | Trang thai: " + h.trangThai;
    }

    // Them ho so moi: dinh dang ma do ham chung sinhMaBenhNhan(so) tao
    bool themHoSoMoi(const string& hoTen, const string& ngaySinh,
                     const string& sdt, string& maMoi) {
        if (soLuong >= MAX_BAN_GHI) return false;

        int soMoi = demMaTudong + 1;             // thu so ke tiep, CHUA ghi vao demMaTudong

        HoSoBenhNhan h;
        h.maBenhNhan = sinhMaBenhNhan(soMoi);
        h.hoTen = hoTen;
        h.ngaySinh = ngaySinh;
        h.sdt = sdt;
        // h.trangThai tu = GOI_KHAM (mac dinh trong struct)

        if (!insertHoSo(h)) return false;        // that bai -> demMaTudong giu nguyen, khong hut so

        demMaTudong = soMoi;                     // thanh cong moi chot so
        maMoi = h.maBenhNhan;
        return true;
    }

    // Goi kham: chi cho phep GOI_KHAM -> DANG_KHAM
    bool batDauKham(const string& maBN) {
        if (!validateMaBN(maBN)) return false;

        int viTri = timViTri(maBN);
        if (viTri == -1) return false;                                       // khong co ho so
        if (danhSach[viTri].trangThai != TrangThai::GOI_KHAM) return false;  // sai trang thai

        danhSach[viTri].trangThai = TrangThai::DANG_KHAM;
        return true;
    }

    // Kham xong: chi cho phep DANG_KHAM -> DA_KHAM
    bool hoanThanhKham(const string& maBN) {
        if (!validateMaBN(maBN)) return false;

        int viTri = timViTri(maBN);
        if (viTri == -1) return false;
        if (danhSach[viTri].trangThai != TrangThai::DANG_KHAM) return false;

        danhSach[viTri].trangThai = TrangThai::DA_KHAM;
        return true;
    }
};