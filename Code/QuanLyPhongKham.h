#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

// 1. TRANG THAI & HANG SO DUNG CHUNG
namespace TrangThai {
    const string DANG_KHAM = "Dang kham", DA_KHAM = "Da kham", DA_HUY = "Da huy", GOI_KHAM = "Goi kham";
}

const vector<string> DS_KHOA = {
    "Tong quat", "Tim mach", "Tai Mui Hong", "Da lieu", "Nhi",
    "San phu khoa", "Rang ham mat", "Tieu hoa", "Ho hap", "Co xuong khop", "Than kinh", "Mat"
};

// Sử dụng biểu thức Lambda để tự động sinh 48 khung giờ (00:00 đến 23:30)
const vector<string> DS_KHUNG_GIO = []() {
    vector<string> ds;
    for (int h = 0; h < 24; h++) {
        string gio = (h < 10 ? "0" : "") + to_string(h);
        ds.push_back(gio + ":00");
        ds.push_back(gio + ":30");
    }
    return ds;
}();

const int K_TOI_DA = 5;
const int MUC_UU_TIEN_CAO = 1, MUC_UU_TIEN_TRUNG_BINH = 2, MUC_UU_TIEN_THAP = 3;

// 2. CAC STRUCT DU LIEU
struct HoSoBenhNhan {
    string maBenhNhan, hoTen, ngaySinh, sdt;
    int mucUuTien = MUC_UU_TIEN_THAP, thuTuDangKy = 0;
    string trangThai = TrangThai::GOI_KHAM;
};

struct LichHen {
    string maBN, ngay, khungGio, chuyenKhoa;
    string trangThai = TrangThai::DANG_KHAM; 
};

struct LuotKham { 
    string ngay, noiDung, trangThai; 
};
struct LichTaiKham { 
    string maBN, ngayTaiKham, gio, noiDung; 
};

// 3. HAM TIEN ICH
static int soSanhKhongPhanBietHoaThuong(const string &a, const string &b) {
    string x = a, y = b;
    transform(x.begin(), x.end(), x.begin(), ::tolower);
    transform(y.begin(), y.end(), y.begin(), ::tolower);
    return x.compare(y);
}

static string cattChuoi(const string &s) {
    size_t dau = s.find_first_not_of(" \t\r\n"), cuoi = s.find_last_not_of(" \t\r\n");
    return (dau == string::npos) ? "" : s.substr(dau, cuoi - dau + 1);
}

static bool ngayHopLe(const string &s, int namMin, int namMax) {
    if (s.size() != 10 || s[2] != '/' || s[5] != '/') return false;
    for (int i = 0; i < 10; i++) if (i != 2 && i != 5 && !isdigit((unsigned char)s[i])) return false;
    int ngay = stoi(s.substr(0, 2)), thang = stoi(s.substr(3, 2)), nam = stoi(s.substr(6, 4));
    if (nam < namMin || nam > namMax || thang < 1 || thang > 12 || ngay < 1) return false;
    int maxNgay = (thang == 2 && ((nam % 4 == 0 && nam % 100 != 0) || nam % 400 == 0)) ? 29 : vector<int>{31,28,31,30,31,30,31,31,30,31,30,31}[thang - 1];
    return ngay <= maxNgay;
}

// Gioi han ngay kham dung nam 2026
static bool ngayKhamHopLe(const string &s) { return ngayHopLe(s, 2026, 2026); }
// Gioi han ngay sinh tu nam 1600 den nam 2026
static bool ngaySinhHopLe(const string &s) { return ngayHopLe(s, 1600, 2026); }

static bool sdtHopLe(const string &s) {
    if (s.size() != 10 || s[0] != '0') return false;
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    return true;
}

static bool khungGioHopLe(const string &gio) { return find(DS_KHUNG_GIO.begin(), DS_KHUNG_GIO.end(), gio) != DS_KHUNG_GIO.end(); }

static bool chuyenKhoaHopLe(const string &khoa) {
    for (const auto &k : DS_KHOA) if (soSanhKhongPhanBietHoaThuong(k, khoa) == 0) return true;
    return false;
}

static string sinhMaBenhNhan(int soThuTu) {
    string so = to_string(soThuTu);
    return "BN" + string(6 - min((size_t)6, so.length()), '0') + so;
}

static int soPhutTrongNgay(const string &hhmm) { 
    return stoi(hhmm.substr(0, 2)) * 60 + stoi(hhmm.substr(3, 2)); 
}