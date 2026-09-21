#include <iostream>
#include <string>
#include <vector>
#include "QuanLyPhongKham.h"

using namespace std;

// Khai bao lien ket toi mang lich hen toan cuc tu file chi_6.cpp
extern vector<LichHen> dsLichChinh;     
void themLich(LichHen lh);              
bool validateMaBN(const string& ma);    

// Ma tra ve cua datLich va huyLich
const int DL_OK = 0, DL_DAU_VAO_SAI = 1, DL_DAY = 2, DL_TRUNG = 3;
const int HL_OK = 0, HL_KHONG_TIM_THAY = 1, HL_DA_HUY_ROI = 2, HL_DA_KHAM = 3;

// Bo dem suat: (ngay, khung gio) -> so nguoi da dat, toi da K_TOI_DA
struct SuatKhung {
    string ngay, khungGio;
    int soDaDat;
};
vector<SuatKhung> dsSuat;

// Tim bo dem cua (ngay, gio). Tra chi so trong dsSuat, khong co thi tra -1.
int timSuat(const string& ngay, const string& gio) {
    for (int i = 0; i < (int)dsSuat.size(); i++)
        if (dsSuat[i].ngay == ngay && dsSuat[i].khungGio == gio) return i;
    return -1;
}

// Tim lich cua maBN o (ngay, gio) co trang thai tt (tt = "" nghia la trang thai nao cung duoc).
int timLich(const string& ma, const string& ngay, const string& gio, const string& tt) {
    for (int i = 0; i < (int)dsLichChinh.size(); i++) {
        if (dsLichChinh[i].maBN == ma && dsLichChinh[i].ngay == ngay && dsLichChinh[i].khungGio == gio
            && (tt == "" || dsLichChinh[i].trangThai == tt))
            return i;
    }
    return -1;
}

// Gio kham hop le: co trong DS_KHUNG_GIO chung va nam trong khoang hop le
bool gioKhamHopLe(const string& gio) {
    if (!khungGioHopLe(gio)) return false;
    int p = soPhutTrongNgay(gio);
    return p >= 0 && p <= 23 * 60 + 30; // Ho tro ca 48 khung gio tu 00:00 den 23:30
}

// Doi ten khoa (khong phan biet hoa/thuong) thanh TEN CHUAN trong DS_KHOA. Khong co thi tra "".
string chuanHoaKhoa(const string& s) {
    for (int i = 0; i < (int)DS_KHOA.size(); i++)
        if (soSanhKhongPhanBietHoaThuong(DS_KHOA[i], s) == 0) return DS_KHOA[i];
    return "";
}

// Chon chuyen khoa tu danh sach hoac nhap ten truc tiep (0 = huy)
string chonChuyenKhoa() {
    for (int i = 0; i < (int)DS_KHOA.size(); i++)
        cout << i + 1 << ". " << DS_KHOA[i] << "\n";

    while (true) {
        string s;
        cout << "Chon so hoac ten (0 = huy): ";
        if (!getline(cin, s)) return "";
        s = cattChuoi(s);
        if (s == "0") return "";

        bool toanSo = (s != "" && s.size() <= 2);      
        for (int i = 0; i < (int)s.size(); i++)
            if (s[i] < '0' || s[i] > '9') toanSo = false;

        if (toanSo) {
            int v = stoi(s);
            if (v >= 1 && v <= (int)DS_KHOA.size()) return DS_KHOA[v - 1];
        } else if (chuanHoaKhoa(s) != "") {
            return chuanHoaKhoa(s);
        }
        cout << "Loi: khong hop le, hay chon trong danh sach.\n";
    }
}

// ===================================================================
// HAM CHON KHUNG GIO THEO SO THU TU (1 - 48, cach nhau 30 phut)
// ===================================================================
string chonKhungGioTheoSTT() {
    cout << "\n===== DANH SACH CAC KHUNG GIO TRONG NGAY =====\n";
    for (size_t i = 0; i < DS_KHUNG_GIO.size(); i++) {
        cout << (i + 1) << ". " << DS_KHUNG_GIO[i] << "\t";
        if ((i + 1) % 4 == 0) cout << "\n"; // In 4 cot cho de nhin
    }
    cout << "\n----------------------------------------------\n";

    while (true) {
        string s;
        cout << "Chon so thu tu khung gio (1 - " << DS_KHUNG_GIO.size() << ", 0 = huy): ";
        if (!getline(cin, s)) return "";
        s = cattChuoi(s);
        if (s == "0") return "";

        bool toanSo = !s.empty();
        for (char c : s) {
            if (!isdigit((unsigned char)c)) toanSo = false;
        }

        if (toanSo) {
            int chon = stoi(s);
            if (chon >= 1 && chon <= (int)DS_KHUNG_GIO.size()) {
                return DS_KHUNG_GIO[chon - 1]; 
            }
        }
    }
}

// ---- DAT LICH: kiem tra hop le, kiem tra trung lich, kiem tra het cho (K_TOI_DA) ----
int datLich(const string& maBN, const string& ngay, const string& gio, const string& chuyenKhoa) {
    string khoa = chuanHoaKhoa(chuyenKhoa);           
    if (!validateMaBN(maBN) || !ngayKhamHopLe(ngay) || !gioKhamHopLe(gio) || khoa == "")
        return DL_DAU_VAO_SAI;
    if (timLich(maBN, ngay, gio, TrangThai::DANG_KHAM) >= 0) return DL_TRUNG;   

    int vt = timSuat(ngay, gio);
    if (vt >= 0 && dsSuat[vt].soDaDat >= K_TOI_DA) return DL_DAY;              
    if (vt < 0) {                                     
        SuatKhung moi;
        moi.ngay = ngay;
        moi.khungGio = gio;
        moi.soDaDat = 0;
        dsSuat.push_back(moi);
        vt = (int)dsSuat.size() - 1;
    }

    LichHen lh;                                       
    lh.maBN = maBN;
    lh.ngay = ngay;
    lh.khungGio = gio;
    lh.chuyenKhoa = khoa;
    lh.trangThai = TrangThai::DANG_KHAM;
    themLich(lh);                                     
    dsSuat[vt].soDaDat++;
    return DL_OK;
}

// ---- HUY LICH: chuyen trang thai thanh "Da huy", giam dem suat ----
int huyLich(const string& maBN, const string& ngay, const string& gio) {
    int i = timLich(maBN, ngay, gio, TrangThai::DANG_KHAM);
    if (i < 0) {
        if (timLich(maBN, ngay, gio, TrangThai::DA_KHAM) >= 0) return HL_DA_KHAM;
        if (timLich(maBN, ngay, gio, "") >= 0) return HL_DA_HUY_ROI;
        return HL_KHONG_TIM_THAY;
    }
    dsLichChinh[i].trangThai = TrangThai::DA_HUY;

    int vt = timSuat(ngay, gio);
    if (vt >= 0 && dsSuat[vt].soDaDat > 0) dsSuat[vt].soDaDat--;
    return HL_OK;
}

// ---- DANH DAU DA KHAM: chuyen lich sang trang thai "Da kham" de ban giao cho module lich su (tructhu) ----
bool danhDauDaKham(const string& maBN, const string& ngay, const string& gio, string& chuyenKhoa) {
    int i = timLich(maBN, ngay, gio, TrangThai::DANG_KHAM);
    if (i < 0) return false;
    dsLichChinh[i].trangThai = TrangThai::DA_KHAM;
    chuyenKhoa = dsLichChinh[i].chuyenKhoa;
    return true;
}