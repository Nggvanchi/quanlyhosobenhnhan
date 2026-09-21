// =====================================================================
// - DAT LICH & HUY LICH
// Dung lai tu QuanLyPhongKham.h: TrangThai, DS_KHOA, DS_KHUNG_GIO, K_TOI_DA, LichHen,
//   ngayKhamHopLe, khungGioHopLe, soPhutTrongNgay, soSanhKhongPhanBietHoaThuong, cattChuoi.
// Dung ham/bien o file khac (extern = khai bao de dung cai duoc dinh nghia o file kia):
//   chi.cpp  : dsLichChinh (kho lich hen chung), themLich()
//   binh.cpp : validateMaBN()   (ma BN do binh.cpp tao, minh khong tu sinh ma)
// Bien dich: g++ -std=c++11 -c nguoi4_datlich_huylich.cpp   (khong co main, ghep vao project chung)
// =====================================================================
#include <iostream>
#include <string>
#include <vector>
#include "QuanLyPhongKham.h"
using namespace std;

extern vector<LichHen> dsLichChinh;     // chi.cpp
void themLich(LichHen lh);              // chi.cpp
bool validateMaBN(const string& ma);    // binh.cpp

// Ma tra ve cua datLich va huyLich (file khac muon dung thi chep 2 dong const int nay vao QuanLyPhongKham.h)
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
// Tra chi so trong dsLichChinh, khong co thi tra -1.
int timLich(const string& ma, const string& ngay, const string& gio, const string& tt) {
    for (int i = 0; i < (int)dsLichChinh.size(); i++) {
        if (dsLichChinh[i].maBN == ma && dsLichChinh[i].ngay == ngay && dsLichChinh[i].khungGio == gio
            && (tt == "" || dsLichChinh[i].trangThai == tt))
            return i;
    }
    return -1;
}

// Gio kham hop le: co trong DS_KHUNG_GIO chung va nam trong 08:00 - 14:30
bool gioKhamHopLe(const string& gio) {
    if (!khungGioHopLe(gio)) return false;
    int p = soPhutTrongNgay(gio);
    return p >= 8 * 60 && p <= 14 * 60 + 30;
}

// Doi ten khoa (khong phan biet hoa/thuong) thanh TEN CHUAN trong DS_KHOA. Khong co thi tra "".
string chuanHoaKhoa(const string& s) {
    for (int i = 0; i < (int)DS_KHOA.size(); i++)
        if (soSanhKhongPhanBietHoaThuong(DS_KHOA[i], s) == 0) return DS_KHOA[i];
    return "";
}

// Nhu cau kham = 1 danh sach (Tong quat + cac khoa). Chon so hoac ten, sai thi hoi lai.
// Go 0 de huy (tra ve "").
string chonChuyenKhoa() {
    for (int i = 0; i < (int)DS_KHOA.size(); i++)
        cout << i + 1 << ". " << DS_KHOA[i] << "\n";

    while (true) {
        string s;
        cout << "Chon so hoac ten (0 = huy): ";
        if (!getline(cin, s)) return "";
        s = cattChuoi(s);
        if (s == "0") return "";

        bool toanSo = (s != "" && s.size() <= 2);      // toi da 2 chu so (1..12)
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

// ---- DAT LICH: kiem tra het roi moi ghi. Con cho -> them lich + tang dem; day -> DL_DAY ----
int datLich(const string& maBN, const string& ngay, const string& gio, const string& chuyenKhoa) {
    string khoa = chuanHoaKhoa(chuyenKhoa);           // luu TEN CHUAN de chuyen sang lich su kham
    if (!validateMaBN(maBN) || !ngayKhamHopLe(ngay) || !gioKhamHopLe(gio) || khoa == "")
        return DL_DAU_VAO_SAI;
    if (timLich(maBN, ngay, gio, TrangThai::DANG_KHAM) >= 0) return DL_TRUNG;   // da co lich con hieu luc

    int vt = timSuat(ngay, gio);
    if (vt >= 0 && dsSuat[vt].soDaDat >= K_TOI_DA) return DL_DAY;              // het cho
    if (vt < 0) {                                     // chua co bo dem -> tao khi co nguoi dat dau tien
        SuatKhung moi;
        moi.ngay = ngay;
        moi.khungGio = gio;
        moi.soDaDat = 0;
        dsSuat.push_back(moi);
        vt = (int)dsSuat.size() - 1;
    }

    LichHen lh;                                       // trangThai mac dinh = DANG_KHAM (con hieu luc)
    lh.maBN = maBN;
    lh.ngay = ngay;
    lh.khungGio = gio;
    lh.chuyenKhoa = khoa;
    themLich(lh);                                     // chi.cpp: them vao dsLichChinh + chen chi so theo gio
    dsSuat[vt].soDaDat++;
    return DL_OK;
}

// ---- HUY LICH: doi thanh "Da huy" (khong xoa), giam dem, tra lai suat ----
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

// ---- BAN GIAO CHO LICH SU KHAM (tructhu.cpp): lich -> "Da kham" va tra chuyenKhoa da luu luc dat ----
// Cach goi:  string khoa;
//            if (danhDauDaKham(ma, ngay, gio, khoa)) themLuotKham(lichSu, ngay, khoa, TrangThai::DA_KHAM);
bool danhDauDaKham(const string& maBN, const string& ngay, const string& gio, string& chuyenKhoa) {
    int i = timLich(maBN, ngay, gio, TrangThai::DANG_KHAM);
    if (i < 0) return false;
    dsLichChinh[i].trangThai = TrangThai::DA_KHAM;
    chuyenKhoa = dsLichChinh[i].chuyenKhoa;
    return true;
}
