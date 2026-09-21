#include "QuanLyPhongKham.h"
#include <iostream>
#include <vector>

using namespace std;

// Kiem tra muc uu tien dung hang so trong file header chung
bool mucUuTienHopLe(int mucUuTien){
    return mucUuTien >= MUC_UU_TIEN_CAO && mucUuTien <= MUC_UU_TIEN_THAP;
}

// Them benh nhan
bool themBenhNhan(vector<HoSoBenhNhan> &HangDoi, HoSoBenhNhan BenhNhan){
    if(!mucUuTienHopLe(BenhNhan.mucUuTien)){
        cout << "Muc uu tien khong hop le.\n";
        return false;
    }
    HangDoi.push_back(BenhNhan);
    return true;
}

// Sap xep theo muc uu tien (co const de khong thay doi hang doi goc)
void sortTheoUuTien(const vector<HoSoBenhNhan> &HangDoi, vector<HoSoBenhNhan> &ketQua){
    int count[4] ={0};
    
    // Buoc 1: dem so benh nhan o moi muc do
    for(int i = 0; i < HangDoi.size(); i++){
        int priority = HangDoi[i].mucUuTien;
        count[priority]++;
    }

    // Buoc 2: tinh vi tri bat dau
    int start[4];
    start[1] = 0;
    start[2] = count[1];
    start[3] = count[1] + count[2];

    // Buoc 3: dua benh nhan vao ket qua (duyet tu trai sang phai de giu thu tu)
    int pos[4];
    pos[1] = start[1];
    pos[2] = start[2];
    pos[3] = start[3];
    ketQua.resize(HangDoi.size()); 
    
    for(int i = 0; i < HangDoi.size(); i++){
        int priority = HangDoi[i].mucUuTien;
        ketQua[pos[priority]] = HangDoi[i];
        pos[priority]++; 
    }
}

// Sap xep de xem lai toan bo hang doi theo danh sach uu tien
void xemTheoThuTuUuTien(const vector<HoSoBenhNhan> &HangDoi){
    if(HangDoi.empty()){
        cout << "Hang doi dang rong.\n";
        return;
    }
    vector<HoSoBenhNhan> ketQua;
    sortTheoUuTien(HangDoi, ketQua);
    cout << "\n===== DANH SACH THEO THU TU UU TIEN =====\n";
    for(int i = 0; i < ketQua.size(); i++){
        cout << i + 1 << ". " 
             << ketQua[i].maBenhNhan << " | " 
             << ketQua[i].hoTen << " | " 
             << "Uu tien: " << ketQua[i].mucUuTien << " | " 
             << "Dang ky: " << ketQua[i].thuTuDangKy << " | " 
             << "Trang thai: " << ketQua[i].trangThai << "\n";
    }
}

// Lay benh nhan uu tien nhat ra khoi hang doi
bool layBenhNhanUuTienNhat(vector<HoSoBenhNhan> &HangDoi, HoSoBenhNhan &ketQua){
    if(HangDoi.empty()){
        cout << "Hang doi dang rong.\n";
        return false;
    }
    vector<HoSoBenhNhan> danhSachSapXep;
    sortTheoUuTien(HangDoi, danhSachSapXep);
    
    // Nguoi dau tien la nguoi duoc uu tien nhat
    ketQua = danhSachSapXep[0];
    
    // Tim theo ma benh nhan de xoa khoi hang doi goc
    for(int i = 0; i < HangDoi.size(); i++){
        if(HangDoi[i].maBenhNhan == ketQua.maBenhNhan){
            HangDoi.erase(HangDoi.begin() + i);
            return true;
        }
    }   
    return false;
}

// Benh nhan bat dau kham va khong xoa khoi hang doi
bool batDauKham(vector<HoSoBenhNhan> &HangDoi, string maBenhNhan_TK){
    for(int i = 0; i < HangDoi.size(); i++){
        if(HangDoi[i].maBenhNhan == maBenhNhan_TK){
            // Su dung hang so tu QuanLyPhongKham.h
            HangDoi[i].trangThai = TrangThai::DANG_KHAM;
            cout << "Benh nhan " << maBenhNhan_TK << " da bat dau kham.\n";
            return true;
        }
    }
    cout << "Khong tim thay benh nhan " << maBenhNhan_TK << " trong hang doi.\n";
    return false;
}

// In toan bo hang doi hien tai ma khong sap xep
void inHangDoi(const vector<HoSoBenhNhan> &HangDoi){
    if(HangDoi.empty()){
        cout << "Hang doi dang rong.\n";
        return;
    }
    cout << "\n===== HANG DOI HIEN TAI =====\n";
    for(int i = 0; i < HangDoi.size(); i++){
        cout << i + 1 << ". "
             << HangDoi[i].maBenhNhan << " | "
             << HangDoi[i].hoTen << " | "
             << "Uu tien: " << HangDoi[i].mucUuTien << " | "
             << "Dang ky: " << HangDoi[i].thuTuDangKy << " | "
             << "Trang thai: " << HangDoi[i].trangThai
             << '\n';
    }
}