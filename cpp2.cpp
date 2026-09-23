#include <bits/stdc++.h>
using namespace std;

struct HoSoBenhNhan{
    string maBenhNhan;
    string hoTen;
    int mucUuTien;
    // 1: cao nhat
    // 2: trung binh
    // 3: binh thuong
    int thuTuDangKy;
    // So thu tu đang ky
    // So cang nho -> dang ky cang som
    string trangThai;
};
// kiem tra muc uu tien
bool mucUuTienHopLe(int mucUuTien){
    return mucUuTien >= 1 && mucUuTien <= 3;
}
// them benh nhan
bool themBenhNhan(vector<HoSoBenhNhan> &HangDoi, HoSoBenhNhan BenhNhan){
    if(!mucUuTienHopLe(BenhNhan.mucUuTien)){
        cout << "Muc uu tien khong hop le." << endl;
        return false; // kiem tra muc uu tien
    }
    HangDoi.push_back(BenhNhan);
    return true;
}
// sap xep theo muc uu tien (co const de khong thay doi hang doi goc)
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

    // Buoc 3: dua benh nhan vao ket qua
    // duyet tu trai sang phai
    // cung priority -> truoc vẫn la truoc
    int pos[4];
    // khong dung luon start vi khi duyet can tang vi tri
    // pos[x] la vi tri tiep theo de dat benh nhan cu muc uu tien x
    // ban dau vi tri tiep theo cung la vi tri bat dau cua nhom do
    // sau moi lan pos[x]++ thi se doi sang o ke ben
    pos[1] = start[1];
    pos[2] = start[2];
    pos[3] = start[3];
    ketQua.resize(HangDoi.size()); // thay doi kich thuoc mang ketQua
    for(int i = 0; i < HangDoi.size(); i++){
        int priority = HangDoi[i].mucUuTien;
        ketQua[pos[priority]] = HangDoi[i];
        pos[priority]++; // co vi tri x roi => thang tiep theo x + 1
    }
}
// sap xep de xem lai toan bo hang doi theo danh sach uu tien
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
             << "Trang thai: " << ketQua[i].trangThai << endl;
    }
}
// lay benh nhan uu tien nhat ra khoi hang doi
bool layBenhNhanUuTienNhat(vector<HoSoBenhNhan> &HangDoi, HoSoBenhNhan &ketQua){
    if(HangDoi.empty()){
        cout << "Hang doi dang rong.\n";
        return false;
    }
    vector<HoSoBenhNhan> danhSachSapXep;
    sortTheoUuTien(HangDoi, danhSachSapXep);
    // nguoi dau tien la nguoi duoc uu tien nhat
    ketQua = danhSachSapXep[0];
    // tim theo ma benh nhan
    for(int i = 0; i < HangDoi.size(); i++){
        if(HangDoi[i].maBenhNhan == ketQua.maBenhNhan){
            HangDoi.erase(HangDoi.begin() + i);
            return true;
        }
    }   
    return false;
}
// benh nhan bat dau kham va khong xoa khoi hang doi
bool batDauKham(vector<HoSoBenhNhan> &HangDoi, string maBenhNhan){
    for(int i = 0; i < HangDoi.size(); i++){
        if(HangDoi[i].maBenhNhan == maBenhNhan){
            HangDoi[i].trangThai = "Dang kham";
            cout << "Benh nhan " << maBenhNhan << " da bat dau kham." << endl;
            return true;
        }
    }
    cout << "Khong tim thay benh nhan " << maBenhNhan << " trong hang doi." << endl;
    return false;
}
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

int main(){
    freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    vector<HoSoBenhNhan> HangDoi;
    for(int i = 0; i < 5; i++){
        // // nhap vao xau s roi dung stringstream tach ra roi day vao vector
        // string s; getline(cin, s);
        // stringstream ss(s);
        // string tmp;
        // vector<string> v;
        // while(getline(ss, tmp, '|'))
        //     v.push_back(tmp); // ki tu | co the thay bang ki tu khac tru ki tu khoang trong
        // string maBenhNhan = v[0];
        // string hoTen = v[1];
        // int mucUuTien = stoi(v[2]);
        // int thuTuDangKy = stoi(v[3]);
        // string trangThai = v[4];
        // themBenhNhan(HangDoi, {maBenhNhan, hoTen, mucUuTien, thuTuDangKy, trangThai});
        string maBenhNhan;
        string hoTen;
        int mucUuTien;
        int thuTuDangKy;
        string trangThai;
        cin >> maBenhNhan;
        cin.ignore();
        getline(cin, hoTen);
        cin >> mucUuTien >> thuTuDangKy;
        cin.ignore();
        getline(cin, trangThai);
        themBenhNhan(HangDoi, {maBenhNhan, hoTen, mucUuTien, thuTuDangKy, trangThai});
    }

    // sap xep theo muc do uu tien
    xemTheoThuTuUuTien(HangDoi);
    HoSoBenhNhan ketQua;
    // lay benh nhan uu tien nhat
    if(layBenhNhanUuTienNhat(HangDoi, ketQua)){
        cout << "\nBenh nhan duoc chon tiep theo:\n";
        cout << ketQua.maBenhNhan << " | " 
             << ketQua.hoTen << " | " 
             << "Uu tien: " << ketQua.mucUuTien << '\n';
    }
    xemTheoThuTuUuTien(HangDoi);
    // xem lai
    batDauKham(HangDoi, "BenhNhan000004");
    // bat dau kham
    xemTheoThuTuUuTien(HangDoi);
    // xem lai
}