#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include "QuanLyPhongKham.h"

// Nhúng trực tiếp các file module của các thành viên trong nhóm
#include "binh.cpp"
#include "ben.cpp"
#include "chi.cpp"
#include "toan.cpp"
#include "tructhu.cpp"

using namespace std;
using namespace std::chrono;

// Hàm hỗ trợ tự động nạp dữ liệu 100.000 bệnh nhân từ file data.csv vào bảng băm
void napDuLieuTuCSV(BangBam& heThongHoSo, const string& tenFile) {
    ifstream file(tenFile);
    if (!file.is_open()) {
        cout << "[Thong bao] Khong tim thay file '" << tenFile << "'. He thong khoi dong voi du lieu trong.\n";
        return;
    }

    string line;
    getline(file, line); // Bỏ qua dòng tiêu đề

    int demThanhCong = 0;
    auto t1 = high_resolution_clock::now();

    while (getline(file, line)) {
        stringstream ss(line);
        string maBN, hoTen, ngaySinh, sdt, trangThai;

        getline(ss, maBN, ',');
        getline(ss, hoTen, ',');
        getline(ss, ngaySinh, ',');
        getline(ss, sdt, ',');
        getline(ss, trangThai, ',');

        HoSoBenhNhan hs;
        hs.maBenhNhan = cattChuoi(maBN);
        hs.hoTen = cattChuoi(hoTen);
        hs.ngaySinh = cattChuoi(ngaySinh);
        hs.sdt = cattChuoi(sdt);
        hs.trangThai = cattChuoi(trangThai);
        if (hs.trangThai.empty()) hs.trangThai = TrangThai::GOI_KHAM;

        if (heThongHoSo.insertHoSo(hs)) {
            demThanhCong++;
        }
    }

    file.close();
    auto t2 = high_resolution_clock::now();
    double thoiGianNap = duration_cast<milliseconds>(t2 - t1).count() / 1000.0;

    cout << "\n==================================================\n";
    cout << "=> Da nap thanh cong " << demThanhCong << " ban ghi tu '" << tenFile << "'!\n";
    cout << "=> Thoi gian nap du lieu (Bulk Load): " << thoiGianNap << " giay.\n";
    cout << "==================================================\n";
}

int main() {
    // Khởi tạo các cấu trúc dữ liệu cốt lõi cho toàn hệ thống
    BangBam heThongHoSo;                 
    vector<HoSoBenhNhan> hangDoiUuTien;   
    DynamicArray lichSuKhamChung;        
    khoiTao(lichSuKhamChung);

    vector<LichTaiKham> dsTaiKham;       

    // Tự động nạp dữ liệu từ file data.csv khi khởi động
    napDuLieuTuCSV(heThongHoSo, "data.csv");

    int luaChon;
    do {
        cout << "\n================ QUAN LY PHONG KHAM ================\n";
        cout << "1. Tra cuu ho so benh nhan\n";
        cout << "2. Xem benh nhan theo thu tu thoi gian dang ky\n";
        cout << "3. Xem benh nhan co lich hen trong mot khoang thoi gian\n";
        cout << "4. Xac dinh benh nhan co muc do uu tien cao nhat\n";
        cout << "5. Dat / Huy lich kham\n";
        cout << "6. Xem lai lich su kham cua benh nhan theo ma BN\n";
        cout << "7. Nhac lich tai kham\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "====================================================\n";
        cout << "Nhap lua chon cua ban: ";

        if (!(cin >> luaChon)) {
            cout << "Loi: Vui long nhap vao mot so hop le!\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (luaChon) {
            case 1: {
                string maBN;
                cout << "Nhap Ma BN can tra cuu (VD: BN000001): ";
                cin >> maBN;

                auto t1 = high_resolution_clock::now();
                string ketQua = heThongHoSo.traCuu(maBN);
                auto t2 = high_resolution_clock::now();

                cout << "=> " << ketQua << "\n";
                cout << "=> Thoi gian truy xuat: " << duration_cast<microseconds>(t2 - t1).count() << " micro-giay.\n";
                break;
            }
            case 2: {
                cout << "\n===== DANH SACH BENH NHAN THEO THU TU DANG KY =====\n";
                xemTheoThuTuDangKy();
                break;
            }
            case 3: {
                string gioBD, gioKT;
                cout << "Nhap gio bat dau (HH:MM): "; cin >> gioBD;
                cout << "Nhap gio ket thuc (HH:MM): "; cin >> gioKT;
                cout << "\n===== LICH HEN TRONG KHOANG THOI GIAN =====\n";
                layTheoKhoang(gioBD, gioKT);
                break;
            }
            case 4: {
                if (hangDoiUuTien.empty()) {
                    cout << "=> Hang doi uu tien hang cho dang trong!\n";
                    break;
                }
                HoSoBenhNhan benhNhanUuTien;
                vector<HoSoBenhNhan> danhSachSapXep;
                sortTheoUuTien(hangDoiUuTien, danhSachSapXep);
                benhNhanUuTien = danhSachSapXep[0];

                cout << "\n===== BENH NHAN CO MUC UU TIEN CAO NHAT =====\n";
                cout << "Ma BN: " << benhNhanUuTien.maBenhNhan << " | Ho ten: " << benhNhanUuTien.hoTen 
                     << " | Muc uu tien: " << benhNhanUuTien.mucUuTien << " | Trang thai: " << benhNhanUuTien.trangThai << "\n";
                break;
            }
            case 5: {
                int subChoice;
                cout << "\n--- QUAN LY DAT / HUY LICH KHAM ---\n";
                cout << "1. Dat lich kham moi\n";
                cout << "2. Huy lich kham\n";
                cout << "Chon thao tac (1-2): ";
                cin >> subChoice;

                if (subChoice == 1) {
                    string maBN, ngay, khoa;
                    cout << "Nhap Ma BN: "; cin >> maBN;
                    cout << "Nhap ngay kham (DD/MM/YYYY): "; cin >> ngay;

                    // Goi ham chon khung gio theo so thu tu (1 - 48, cach nhau 30 phut)
                    string gio = chonKhungGioTheoSTT();
                    if (gio == "") {
                        cout << "=> Huy thao tac dat lich.\n";
                        break;
                    }

                    cout << "Chon chuyen khoa:\n";
                    khoa = chonChuyenKhoa();
                    if (khoa == "") {
                        cout << "=> Huy dat lich.\n";
                        break;
                    }

                    int ketQuaDat = datLich(maBN, ngay, gio, khoa);
                    if (ketQuaDat == DL_OK) {
                        cout << "=> Dat lich thanh cong cho khung gio " << gio << "!\n";
                        HoSoBenhNhan hs;
                        hs.maBenhNhan = maBN;
                        hs.mucUuTien = MUC_UU_TIEN_THAP;
                        hs.trangThai = TrangThai::GOI_KHAM;
                        themBenhNhan(hangDoiUuTien, hs);
                    } else if (ketQuaDat == DL_DAY) {
                        cout << "=> Loi: Khung gio nay da dat toi da so suat quy dinh.\n";
                    } else if (ketQuaDat == DL_TRUNG) {
                        cout << "=> Loi: Benh nhan da co lich hen dang hoat dong o khung gio nay.\n";
                    } else {
                        cout << "=> Loi: Thong tin dau vao khong hop le (Ma BN hoac Ngay khong dung quy dinh).\n";
                    }
                } else if (subChoice == 2) {
                    string maBN, ngay, gio;
                    cout << "Nhap Ma BN can huy lich: "; cin >> maBN;
                    cout << "Nhap ngay kham (DD/MM/YYYY): "; cin >> ngay;
                    cout << "Nhap khung gio (HH:MM): "; cin >> gio;

                    int ketQuaHuy = huyLich(maBN, ngay, gio);
                    if (ketQuaHuy == HL_OK) {
                        cout << "=> Huy lich kham thanh cong!\n";
                    } else if (ketQuaHuy == HL_DA_KHAM) {
                        cout << "=> Loi: Lich hen nay da duoc kham, khong thể huy.\n";
                    } else if (ketQuaHuy == HL_DA_HUY_ROI) {
                        cout << "=> Loi: Lich hen nay da bi huy truoc do.\n";
                    } else {
                        cout << "=> Loi: Khong tim thay lich hen phu hop.\n";
                    }
                }
                break;
            }
            case 6: {
                string maBN;
                cout << "Nhap Ma BN can xem lai lich su kham: ";
                cin >> maBN;
                
                string thongTinHoSo = heThongHoSo.traCuu(maBN);
                if (thongTinHoSo.find("Khong tim thay") != string::npos || thongTinHoSo.find("khong hop le") != string::npos) {
                    cout << "=> Khong ton tai ma benh nhan nay trong he thong!\n";
                    break;
                }

                cout << "\n===== LICH SU KHAM CUA BENH NHAN: " << maBN << " =====\n";
                inLichSu(lichSuKhamChung);
                break;
            }
            case 7: {
                int subChoice;
                cout << "\n--- QUAN LY NHAC LICH TAI KHAM ---\n";
                cout << "1. Tao lich hen tai kham moi\n";
                cout << "2. Xem danh sach nhac tai kham\n";
                cout << "Chon thao tac (1-2): ";
                cin >> subChoice;

                if (subChoice == 1) {
                    LichTaiKham ltk;
                    cout << "Nhap Ma BN: "; cin >> ltk.maBN;
                    cout << "Nhap ngay tai kham (DD/MM/YYYY): "; cin >> ltk.ngayTaiKham;
                    cout << "Nhap gio (HH:MM): "; cin >> ltk.gio;
                    cout << "Nhap noi dung tai kham: "; cin.ignore(); getline(cin, ltk.noiDung);

                    dsTaiKham.push_back(ltk);
                    cout << "=> Da dang ky nhac lich tai kham thanh cong!\n";
                } else if (subChoice == 2) {
                    if (dsTaiKham.empty()) {
                        cout << "=> Chua co lich tai kham nao duoc ghi nhan.\n";
                    } else {
                        cout << "\n===== DANH SACH NHAC LICH TAI KHAM =====\n";
                        for (size_t i = 0; i < dsTaiKham.size(); i++) {
                            cout << i + 1 << ". Ma BN: " << dsTaiKham[i].maBN 
                                 << " | Ngay: " << dsTaiKham[i].ngayTaiKham 
                                 << " | Gio: " << dsTaiKham[i].gio 
                                 << " | Noi dung: " << dsTaiKham[i].noiDung << "\n";
                        }
                    }
                }
                break;
            }
            case 0:
                cout << "Thoat chuong trinh...\n";
                giaiPhong(lichSuKhamChung);
                break;
            default:
                cout << "Lua chon khong hop le, vui long chon tu 0 den 7!\n";
        }
    } while (luaChon != 0);

    return 0;
}