#include <iostream>
#include <vector>
#include "QuanLyPhongKham.h"

using namespace std;

vector<LichHen> dsLichChinh; // Luu dung thu tu benh nhan dang ky
vector<int> idxTheoGio;      // Mang chi so (index) luon giu trang thai sort theo gio

// ===================================================================
// TIM KIEM NHI PHAN CAN DUOI (LOWER BOUND)
// Muc dich: Tim vi tri DAU TIEN co gio hen >= phut (de chen hoac lay khoang)
// ===================================================================
int timViTriDauTien_LonHonBang(int phut) {
    int lo = 0, hi = idxTheoGio.size();   
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int gioTaiMid = soPhutTrongNgay(dsLichChinh[idxTheoGio[mid]].khungGio);
        if (gioTaiMid < phut)
            lo = mid + 1;   
        else hi = mid;       
    }
    return lo; 
}

// ===================================================================
// TIM KIEM NHI PHAN CAN TREN (UPPER BOUND)
// Muc dich: Tim vi tri DAU TIEN co gio hen > phut (thuc su lon hon)
// ===================================================================
int timViTriDauTien_LonHonThucSu(int phut) {
    int lo = 0, hi = idxTheoGio.size();
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int gioTaiMid = soPhutTrongNgay(dsLichChinh[idxTheoGio[mid]].khungGio);
        if (gioTaiMid <= phut)
            lo = mid + 1;   
        else hi = mid;       
    }
    return lo;
}

// ===================================================================
// CHEN VAO MANG (INSERTION)
// ===================================================================
void themLich(LichHen lh) {
    dsLichChinh.push_back(lh);
    int idxMoi = dsLichChinh.size() - 1; 
    int phutMoi = soPhutTrongNgay(lh.khungGio);
    
    int viTriChen = timViTriDauTien_LonHonBang(phutMoi);
    
    idxTheoGio.push_back(0); 
    for (int i = (int)idxTheoGio.size() - 1; i > viTriChen; i--) {
        idxTheoGio[i] = idxTheoGio[i - 1];
    }
    idxTheoGio[viTriChen] = idxMoi;
}

// ===================================================================
// XEM THEO KHOANG THOI GIAN
// ===================================================================
void layTheoKhoang(const string& gioBD, const string& gioKT) {
    int pBD = soPhutTrongNgay(gioBD);
    int pKT = soPhutTrongNgay(gioKT);
    
    int l = timViTriDauTien_LonHonBang(pBD);   
    int r = timViTriDauTien_LonHonThucSu(pKT);    
    
    if (l >= r) {
        cout << "Khong co lich hen nao trong khoang " << gioBD << " - " << gioKT << "\n";
        return;
    }
    for (int i = l; i < r; i++) {
        const LichHen& lh = dsLichChinh[idxTheoGio[i]];
        cout << lh.maBN << " | " << lh.khungGio << " | " << lh.chuyenKhoa << "\n";
    }
}

// ===================================================================
// XEM THEO THU TU DANG KY
// ===================================================================
void xemTheoThuTuDangKy() {
    for (int i = 0; i < (int)dsLichChinh.size(); i++)
        cout << dsLichChinh[i].maBN << " | Dang ky thu " << i + 1
             << " | Gio hen " << dsLichChinh[i].khungGio << "\n";
}

// ===================================================================
// LOC THEO TRANG THAI: DANG KHAM
// ===================================================================
void xemDanhSachDangKham() {
    for (const auto& lh : dsLichChinh)
        if (lh.trangThai == TrangThai::DANG_KHAM) 
            cout << lh.maBN << " | " << lh.khungGio << " | " << lh.chuyenKhoa << "\n";
}