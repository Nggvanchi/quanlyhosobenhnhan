#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "QuanLyPhongKham.h"

using namespace std;

// Đọc/ghi dữ liệu CSV và chuyển đổi dữ liệu tệp thành các cấu trúc trong bộ nhớ.

// Phân tích mức ưu tiên trong CSV; ô trống mặc định là mức thấp nhất.
static bool DocMucUuTien(const string& Chuoi, int& MucUuTien) {
    string S = CatChuoi(Chuoi);
    if (S.empty()) {
        MucUuTien = MUC_UU_TIEN_THAP;
        return true;
    }
    if (S.size() == 1 && S[0] >= '1' && S[0] <= '3') {
        MucUuTien = S[0] - '0';
        return true;
    }
    return false;
}

// Đọc hồ sơ, lịch hẹn và bộ đếm sức chứa từ tệp CSV.
// Các dòng sai định dạng được cảnh báo và bỏ qua, không làm dừng toàn bộ quá trình.
void NapDuLieuTuCSV(BangBam& HeThongHoSo, QuanLyLichHen& QLLich, MangBenhNhan& HangDoiUuTien, const string& TenFile, MangLichSu* LichSu) {
    ifstream file(TenFile);
    if (!file.is_open()) {
        cout << "[Thong bao] Khong tim thay file '" << TenFile << "'.\n"; 
        return; 
    }

    string line;
    getline(file, line);
    int SoDong = 1;
    while (getline(file, line)) {
        ++SoDong;
        stringstream ss(line);
        string f[9];
        bool Cot = true;
        for (int i = 0; i < 9; ++i) {
            if (!getline(ss, f[i], i < 8 ? ',' : '\n')) {
                Cot = false; 
                break; 
            }
        }
        if (!Cot) {
            cout << "[Canh bao] Bo qua dong CSV " << SoDong << ": thieu cot du lieu.\n";
            continue;
        }

        for (int i = 0; i < 9; ++i) f[i] = CatChuoi(f[i]);

        HoSoBenhNhan hs;
        hs.MaBenhNhan = f[0]; hs.HoTen = f[1]; hs.NgaySinh = f[2]; hs.Sdt = f[3];
        hs.TrangThai = f[4];
        if (!DocMucUuTien(f[5], hs.MucUuTien)) {
            cout << "[Canh bao] Bo qua dong CSV " << SoDong << ": muc uu tien khong hop le.\n";
            continue;
        }

        if (hs.TrangThai == "Cho kham") hs.TrangThai = CHO_KHAM;
        else if (hs.TrangThai == "Da kham" || hs.TrangThai == "Kham xong") hs.TrangThai = DA_KHAM;
        else if (hs.TrangThai == "Dang kham") hs.TrangThai = DANG_KHAM;

        bool LichHoatDong = (hs.TrangThai == CHO_KHAM || hs.TrangThai == DANG_KHAM);
        if (LichHoatDong) hs.NgayDatLich = f[6];
        else if (hs.TrangThai == DA_KHAM) {
            hs.NgayKhamGanNhat = f[6];
            if (LichSu) ThemLuotKham(*LichSu, hs.MaBenhNhan, f[6], f[8].empty() ? "Tong quat" : f[8], DA_KHAM);
        }

        if (!InsertHoSo(HeThongHoSo, hs)) continue;
        if (hs.TrangThai == CHO_KHAM) ThemBenhNhan(HangDoiUuTien, hs);

        if (LichHoatDong) {
            int vt = TimSuat(f[6], f[7]);
            if (vt < 0) { 
                DSSuat.PushBack({f[6], f[7], 0}); 
                vt = DSSuat.Size - 1; 
            }
            ++DSSuat[vt].SoDaDat;
            ThemLich(QLLich, {hs.MaBenhNhan, hs.NgaySinh, hs.Sdt, f[6], f[7], f[8], hs.TrangThai});
        } else if (hs.TrangThai == DA_KHAM && !f[6].empty()) {
            ThemLich(QLLich, {hs.MaBenhNhan, hs.NgaySinh, hs.Sdt, f[6], f[7], f[8], hs.TrangThai});
        }
    }
}

// Ghi thông tin một hồ sơ và lịch hẹn tương ứng ra luồng tệp CSV.
static void GhiMotDongHoSo(ofstream& File, const HoSoBenhNhan& HoSo, const LichHen* Lich) {
    if (HoSo.MaBenhNhan.empty()) return;
    string Ngay = Lich ? Lich->Ngay : HoSo.NgayKhamGanNhat;
    string Gio = Lich ? Lich->KhungGio : "";
    string Khoa = Lich ? Lich->ChuyenKhoa : (!HoSo.NgayKhamGanNhat.empty() ? "Tong quat" : "");
    File << HoSo.MaBenhNhan << "," << HoSo.HoTen << "," << HoSo.NgaySinh << "," << HoSo.Sdt << ","
         << HoSo.TrangThai << "," << HoSo.MucUuTien << "," << Ngay << "," << Gio << "," << Khoa << "\n";
}

// Ghi các hồ sơ cùng lịch đang hoạt động hoặc đã khám về CSV để lần chạy sau nạp lại được.
// Sử dụng bảng con trỏ trung gian để ánh xạ trực tiếp từ Bảng băm sang Lịch hẹn trong O(N),
// loại bỏ hoàn toàn vòng lặp lồng O(N^2) gây nghẽn hiệu năng khi lưu 100.000 bản ghi.
bool LuuDuLieuVaoCSV(const BangBam& HeThongHoSo, const QuanLyLichHen& QLLich, const string& TenFile) {
    ofstream File(TenFile);
    if (!File.is_open()) { 
        cout << "[Loi] Khong the ghi tep '" << TenFile << "'.\n"; 
        return false; 
    }

    File << "MaBenhNhan,HoTen,NgaySinh,Sdt,TrangThai,MucUuTien,NgayKham,KhungGio,ChuyenKhoa\n";

    // Khởi tạo bảng con trỏ trỏ trực tiếp đến lịch phù hợp nhất của từng vị trí trong bảng băm:
    const LichHen** LichCuaHoSo = new const LichHen*[SIZE_BANG_BAM]();
    for (int j = 0; j < QLLich.Size(); ++j) {
        if (QLLich[j].TrangThai == DA_HUY) continue;
        int v = TimViTri(HeThongHoSo, QLLich[j].MaBN);
        if (v >= 0) {
            const LichHen* HienTai = LichCuaHoSo[v];
            if (!HienTai || QLLich[j].TrangThai == DANG_KHAM ||
                (QLLich[j].TrangThai == CHO_KHAM && HienTai->TrangThai != DANG_KHAM) ||
                (QLLich[j].TrangThai == DA_KHAM && !HienTai)) LichCuaHoSo[v] = &QLLich[j];
        }
    }

    if (!HeThongHoSo.ThuTu.Empty()) {
        for (int k = 0; k < HeThongHoSo.ThuTu.Size; ++k) {
            int i = HeThongHoSo.ThuTu[k];
            GhiMotDongHoSo(File, HeThongHoSo.DanhSach[i], LichCuaHoSo[i]);
        }
    } else {
        for (int i = 0; i < SIZE_BANG_BAM; ++i) {
            GhiMotDongHoSo(File, HeThongHoSo.DanhSach[i], LichCuaHoSo[i]);
        }
    }

    delete[] LichCuaHoSo;
    return true;
}

// Nạp danh sách lịch hẹn tái khám từ file CSV vào mảng động và cập nhật hồ sơ bệnh nhân.
void NapLichTaiKhamTuCSV(MangLichTaiKham& DSTaiKham, BangBam& HeThongHoSo, const string& TenFile) {
    ifstream File(TenFile);
    if (!File.is_open()) return;

    string Line;
    getline(File, Line); // Bỏ qua tiêu đề
    while (getline(File, Line)) {
        stringstream ss(Line);
        string f[4];
        for (int i = 0; i < 4; ++i) getline(ss, f[i], i < 3 ? ',' : '\n');
        for (int i = 0; i < 4; ++i) f[i] = CatChuoi(f[i]);

        if (f[0].empty() || !NgayKhamHopLe(f[1]) || !GioKhamHopLe(f[2])) continue;

        DSTaiKham.PushBack({f[0], f[1], f[2], f[3]});
        int v = TimViTri(HeThongHoSo, f[0]);
        if (v >= 0) HeThongHoSo.DanhSach[v].NgayTaiKham = f[1];
    }
}

// Lưu toàn bộ danh sách lịch hẹn tái khám hiện có ra file CSV.
bool LuuLichTaiKhamVaoCSV(const MangLichTaiKham& DSTaiKham, const string& TenFile) {
    ofstream File(TenFile);
    if (!File.is_open()) return false;

    File << "MaBenhNhan,NgayTaiKham,gio,NoiDung\n";
    for (int i = 0; i < DSTaiKham.Size; ++i) {
        File << DSTaiKham[i].MaBN << "," << DSTaiKham[i].NgayTaiKham << ","
             << DSTaiKham[i].Gio << "," << DSTaiKham[i].NoiDung << "\n";
    }
    return true;
}
