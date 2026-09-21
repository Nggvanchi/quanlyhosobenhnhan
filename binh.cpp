#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "QuanLyPhongKham.h"   // header chung: HoSoBenhNhan, TrangThai, ngaySinhHopLe, sdtHopLe, sinhMaBenhNhan
using namespace std;

// =====================================================================
// 1. CÁC HÀM KIỂM TRA (VALIDATE)
//    (struct HoSoBenhNhan và trạng thái đã nằm trong header chung)
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

// ngaySinhHopLe() của header lo phần định dạng + đúng lịch (30/02, năm nhuận...).
// Nhưng nó chỉ chặn theo NĂM (1600..2026), nên ngày sau hôm nay trong năm nay vẫn lọt qua
// -> ta tự thêm luật "không lớn hơn hôm nay".
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
// 2. BẢNG BĂM (dò tuyến tính) CHO 100.000 BẢN GHI
// =====================================================================
struct BangBam {
    static const int SIZE = 150007;          // số nguyên tố > 100.000 x 1.5
    static const int MAX_BAN_GHI = 100000;   // giới hạn số hồ sơ

    vector<HoSoBenhNhan> danhSach;           // vector tự cấp phát + tự giải phóng
    int soLuong = 0;
    int demMaTudong = 0;                     // số thứ tự đã dùng để sinh mã

    BangBam() : danhSach(SIZE) {}            // tạo sẵn SIZE ô trống

    // Băm chuỗi: duyệt từng ký tự bằng chỉ số
    int hamBam(const string& maBN) {
        unsigned int tong = 0;
        for (size_t i = 0; i < maBN.length(); i++) {
            tong = tong * 31 + (unsigned char)maBN[i];
        }
        return tong % SIZE;
    }

    // Tìm vị trí của mã trong bảng. Không có -> trả về -1.
    // (traCuu, batDauKham, hoanThanhKham đều dùng chung hàm này)
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

    string traCuu(const string& maBN) {
        if (!validateMaBN(maBN)) return "Mã bệnh nhân không hợp lệ.";

        int viTri = timViTri(maBN);
        if (viTri == -1) return "Khong tim thay ho so benh nhan.";

        HoSoBenhNhan& h = danhSach[viTri];
        return "Ma BN: " + h.maBenhNhan +
               " | Ten: " + h.hoTen +
               " | Ngay Sinh: " + h.ngaySinh +
               " | SDT: " + h.sdt +
               " | Trang thai: " + h.trangThai;
    }

    // Thêm hồ sơ mới: định dạng mã do hàm chung sinhMaBenhNhan(số) tạo
    bool themHoSoMoi(const string& hoTen, const string& ngaySinh,
                     const string& sdt, string& maMoi) {
        if (soLuong >= MAX_BAN_GHI) return false;

        int soMoi = demMaTudong + 1;             // thử số kế tiếp, CHƯA ghi vào demMaTudong

        HoSoBenhNhan h;
        h.maBenhNhan = sinhMaBenhNhan(soMoi);
        h.hoTen = hoTen;
        h.ngaySinh = ngaySinh;
        h.sdt = sdt;
        // h.trangThai tự = GOI_KHAM (mặc định trong struct)

        if (!insertHoSo(h)) return false;        // thất bại -> demMaTudong giữ nguyên, không hụt số

        demMaTudong = soMoi;                     // thành công mới chốt số
        maMoi = h.maBenhNhan;
        return true;
    }

    // Gọi khám: chỉ cho phép GOI_KHAM -> DANG_KHAM
    bool batDauKham(const string& maBN) {
        if (!validateMaBN(maBN)) return false;

        int viTri = timViTri(maBN);
        if (viTri == -1) return false;                                       // không có hồ sơ
        if (danhSach[viTri].trangThai != TrangThai::GOI_KHAM) return false;  // sai trạng thái

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

// =====================================================================
// 3. HÀM CHẠY THỬ (thay cho main cũ; main của nhóm sẽ gọi hàm này)
// =====================================================================
void chayThuBangBam() {
    BangBam quanLy;
    string maTao;

    cout << "--- THEM 100.000 BAN GHI ---" << endl;
    clock_t start = clock();
    int thanhCong = 0;

    for (int i = 1; i <= 100000; i++) {
        if (quanLy.themHoSoMoi("Nguyen Van An", "15/03/1990", "0912345678", maTao)) {
            thanhCong++;
        }
    }
    clock_t end = clock();

    cout << "-> Thanh cong: " << thanhCong << " / 100000" << endl;
    cout << "-> Thoi gian: " << (double)(end - start) / CLOCKS_PER_SEC << " giay" << endl;

    cout << "\n--- TRA CUU ---" << endl;
    start = clock();
    cout << quanLy.traCuu("BN000001") << endl;
    cout << quanLy.traCuu("BN050000") << endl;
    cout << quanLy.traCuu("BN100000") << endl;
    end = clock();
    cout << "-> Thoi gian tra cuu 3 ho so: " << (double)(end - start) / CLOCKS_PER_SEC << " giay" << endl;

    cout << "\n--- DOI TRANG THAI ---" << endl;
    cout << "batDauKham(BN000001)      : " << quanLy.batDauKham("BN000001") << endl;    // 1
    cout << "batDauKham(BN000001) lan 2: " << quanLy.batDauKham("BN000001") << endl;    // 0 (dang kham roi)
    cout << "hoanThanhKham(BN000002)   : " << quanLy.hoanThanhKham("BN000002") << endl; // 0 (nhay coc)
    cout << "hoanThanhKham(BN000001)   : " << quanLy.hoanThanhKham("BN000001") << endl; // 1
    cout << "hoanThanhKham(BN000001) l2: " << quanLy.hoanThanhKham("BN000001") << endl; // 0 (da kham roi)
    cout << "batDauKham(BN999999)      : " << quanLy.batDauKham("BN999999") << endl;    // 0 (khong ton tai)
    cout << quanLy.traCuu("BN000001") << endl;
}
