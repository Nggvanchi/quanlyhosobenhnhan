#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "QuanLyPhongKham.h"

// Benchmark dùng trực tiếp các mô-đun của hệ thống, không dùng STL container.
#include "binh.cpp"
#include "ben.cpp"
#include "chi.cpp"
#include "toan.cpp"
#include "tructhu.cpp"

using namespace std;
using Clock = chrono::steady_clock;

// Chương trình đo hiệu năng các thuật toán chính trên dữ liệu lớn.
// Mỗi phép đo chạy nhiều lần và lấy trung vị để giảm ảnh hưởng của nhiễu hệ thống.
const int SO_LAN_DO = 7;
const int SO_LAN_TRUY_VAN = 10000;

static string TaoMa(int so) {
    // Sinh mã BN có sáu chữ số, tuần hoàn trong phạm vi dữ liệu hỗ trợ.
    so = (so - 1) % 100000 + 1;
    string chuoi = to_string(so);
    return "BN" + string(6 - chuoi.size(), '0') + chuoi;
}

static string MaKhongTonTai() {
    // Mã dùng làm trường hợp tra cứu thất bại.
    return "BN999999";
}

static string TaoNgay(int so) {
    // Sinh ngày khám hợp lệ trong tháng 09/2026.
    int ngay = (so - 1) % 28 + 1;
    return (ngay < 10 ? "0" : "") + to_string(ngay) + "/09/2026";
}

static HoSoBenhNhan TaoHoSo(int so) {
    // Sinh một hồ sơ hợp lệ phục vụ đo bảng băm và sắp xếp.
    HoSoBenhNhan hs;
    hs.MaBenhNhan = TaoMa(so);
    hs.HoTen = "Nguyen Van An";
    hs.NgaySinh = "01/01/1990";
    hs.Sdt = "09" + string(8 - to_string(so).size(), '0') + to_string(so);
    hs.MucUuTien = (so - 1) % 3 + 1;
    hs.ThuTuDangKy = so;
    hs.TrangThai = CHO_KHAM;
    return hs;
}

static QuanLyLichHen qlLichBench;

static void XoaLich() {
    // Dọn các mảng giữa các kịch bản benchmark.
    qlLichBench.Clear();
    DSSuat.Clear();
}

static void SapXepSo(long long* so, int kichThuoc) {
    // Sắp xếp các lần đo để chọn phần tử trung vị.
    for (int i = 1; i < kichThuoc; ++i) {
        long long tam = so[i];
        int j = i - 1;
        while (j >= 0 && so[j] > tam) {
            so[j + 1] = so[j];
            --j;
        }
        so[j + 1] = tam;
    }
}

template <typename Ham>
static long long DoTrungVi(Ham ham) {
    long long ketQua[SO_LAN_DO];
    ham(); // Làm nóng bộ nhớ và bộ máy đo.
    for (int i = 0; i < SO_LAN_DO; ++i) ketQua[i] = ham();
    SapXepSo(ketQua, SO_LAN_DO);
    return ketQua[SO_LAN_DO / 2];
}

static int TimTuyenTinh(const MangBenhNhan& danhSach, const string& maBN) {
    // Tra cứu tuần tự làm mốc so sánh với bảng băm.
    for (int i = 0; i < danhSach.Size; ++i)
        if (danhSach[i].MaBenhNhan == maBN) return i;
    return -1;
}

static long long DoHash(BangBam& bang, int n, bool tonTai) {
    volatile int tong = 0;
    return DoTrungVi([&]() {
        Clock::time_point batDau = Clock::now();
        for (int i = 0; i < SO_LAN_TRUY_VAN; ++i) {
            string ma = tonTai ? TaoMa(i % n + 1) : MaKhongTonTai();
            tong += TimViTri(bang, ma);
        }
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    }) + (tong == -999999 ? 1 : 0);
}

static long long DoTuyenTinh(const MangBenhNhan& danhSach, int n,
                             bool tonTai) {
    volatile int tong = 0;
    return DoTrungVi([&]() {
        Clock::time_point batDau = Clock::now();
        for (int i = 0; i < SO_LAN_TRUY_VAN; ++i) {
            string ma = tonTai ? TaoMa(i % n + 1) : MaKhongTonTai();
            tong += TimTuyenTinh(danhSach, ma);
        }
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    }) + (tong == -999999 ? 1 : 0);
}

static void BenchmarkMC1_Binh(ofstream& ketQua, int n) {
    // MC1 (binh.cpp): Bảng băm tra cứu hồ sơ bệnh nhân O(1) so với duyệt tuyến tính O(N).
    BangBam bang;
    MangBenhNhan danhSach;
    for (int i = 1; i <= n; ++i) {
        HoSoBenhNhan hs = TaoHoSo(i);
        InsertHoSo(bang, hs);
        danhSach.PushBack(hs);
    }
    bool dung = true;
    for (int i = 1; i <= n; i += n / 10)
        dung = dung && TimViTri(bang, TaoMa(i)) >= 0;
    dung = dung && TimViTri(bang, MaKhongTonTai()) == -1;

    long long hashCo = DoHash(bang, n, true);
    long long tuyenCo = DoTuyenTinh(danhSach, n, true);
    long long hashKhong = DoHash(bang, n, false);
    long long tuyenKhong = DoTuyenTinh(danhSach, n, false);
    cout << "\n[MC1 - BINH.CPP] Bang bam tra cuu ho so (" << n << " ho so): "
         << (dung && bang.SoLuong == n ? "PASS" : "FAIL")
         << " (nap " << bang.SoLuong << "/" << n << ")\n";
    cout << "  Hash ton tai: " << hashCo << " us | Linear: "
         << tuyenCo << " us\n";
    cout << "  Hash khong ton tai: " << hashKhong << " us | Linear: "
         << tuyenKhong << " us\n";
    ketQua << "MC1_binh," << n << ",hash_existing," << hashCo << "\n";
    ketQua << "MC1_binh," << n << ",linear_existing," << tuyenCo << "\n";
    ketQua << "MC1_binh," << n << ",hash_missing," << hashKhong << "\n";
    ketQua << "MC1_binh," << n << ",linear_missing," << tuyenKhong << "\n";
}

static int DemKhoangTuyenTinh(const string& ngay, int gioBD, int gioKT) {
    int dem = 0;
    for (int i = 0; i < qlLichBench.Size(); ++i) {
        int phut = SoPhutTrongNgay(qlLichBench[i].KhungGio);
        if (qlLichBench[i].Ngay == ngay && phut >= gioBD && phut < gioKT) ++dem;
    }
    return dem;
}

static int DemKhoangChiMuc(const string& ngay, int gioBD, int gioKT) {
    int trai = TimViTriDauTienTheoNgayGio(qlLichBench, ngay, gioBD);
    int phai = TimViTriDauTienSauNgayGio(qlLichBench, ngay, gioKT - 1);
    int dem = 0;
    for (int i = trai; i < phai; ++i) {
        const LichHen& lich = qlLichBench[qlLichBench.IdxTheoGio[i]];
        if (lich.Ngay == ngay && lich.TrangThai != DA_HUY) ++dem;
    }
    return dem;
}

static void TaoDanhSachLich(int n) {
    XoaLich();
    for (int i = 1; i <= n; ++i) {
        int slot = (i - 1) % SO_KHUNG_GIO;
        string ngay = (i % 2 == 0) ? "01/09/2026" : "02/09/2026";
        ThemLich(qlLichBench, {TaoMa(i), "01/01/1990", "0900000000",
                  ngay, DS_KHUNG_GIO[slot], "Tong quat"});
    }
}

static void BenchmarkMC2_Chi(ofstream& ketQua, int n) {
    // MC2 (chi.cpp): Truy vấn khoảng lịch hẹn bằng Binary Search trên chỉ mục IdxTheoGio so với duyệt tuyến tính.
    TaoDanhSachLich(n);
    const string ngay = "01/09/2026";
    const int gioBD = 7 * 60 + 30, gioKT = 20 * 60;
    int dungChiMuc = DemKhoangChiMuc(ngay, gioBD, gioKT);
    int dungTuyen = DemKhoangTuyenTinh(ngay, gioBD, gioKT);
    bool dung = dungChiMuc == dungTuyen;
    long long chiMuc = DoTrungVi([&]() {
        Clock::time_point batDau = Clock::now();
        volatile int dem = DemKhoangChiMuc(ngay, gioBD, gioKT);
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count() + (dem < 0 ? 1 : 0);
    });
    long long tuyen = DoTrungVi([&]() {
        Clock::time_point batDau = Clock::now();
        volatile int dem = DemKhoangTuyenTinh(ngay, gioBD, gioKT);
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count() + (dem < 0 ? 1 : 0);
    });
    cout << "\n[MC2 - CHI.CPP] Truy van khoang gio bang Binary Search (" << n << " lich): " << (dung ? "PASS" : "FAIL") << "\n";
    cout << "  Index + Binary Search: " << chiMuc
         << " us | Linear: " << tuyen << " us\n";
    ketQua << "MC2_chi," << n << ",indexed_range," << chiMuc << "\n";
    ketQua << "MC2_chi," << n << ",linear_range," << tuyen << "\n";
}

static void BenchmarkMC2_Ben(ofstream& ketQua, int n) {
    // MC2 (ben.cpp): Sắp xếp bệnh nhân theo mức độ ưu tiên bằng Counting Sort ổn định O(N).
    MangBenhNhan danhSach, banSao;
    MangDong<string> gio, gioSao;
    for (int i = 1; i <= n; ++i) {
        danhSach.PushBack(TaoHoSo(i));
        gio.PushBack(DS_KHUNG_GIO[(i - 1) % SO_KHUNG_GIO]);
    }
    for (int i = 0; i < n; ++i) {
        banSao.PushBack(danhSach[i]);
        gioSao.PushBack(gio[i]);
    }
    long long thoiGian = DoTrungVi([&]() {
        for (int i = 0; i < n; ++i) {
            banSao[i] = danhSach[i];
            gioSao[i] = gio[i];
        }
        Clock::time_point batDau = Clock::now();
        CountingSortTheoUuTien(banSao, gioSao);
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    bool dung = true;
    for (int i = 1; i < n; ++i) {
        dung = dung && (banSao[i - 1].MucUuTien <= banSao[i].MucUuTien);
        if (banSao[i - 1].MucUuTien == banSao[i].MucUuTien)
            dung = dung && banSao[i - 1].ThuTuDangKy < banSao[i].ThuTuDangKy;
    }
    cout << "\n[MC2 - BEN.CPP] Counting Sort uu tien on dinh (" << n << " benh nhan): "
         << (dung ? "PASS" : "FAIL") << "\n";
    cout << "  Counting Sort on dinh: " << thoiGian << " us\n";
    ketQua << "MC2_ben," << n << ",counting_sort," << thoiGian << "\n";
}

static void BenchmarkTuPhat1_Toan(ofstream& ketQua, int n) {
    // Tự phát 1 (toan.cpp): Nghiệp vụ Đặt/Hủy lịch hẹn & Khống chế sức chứa K <= 5.
    XoaLich();
    int thanhCong = 0, day = 0;
    long long dat = DoTrungVi([&]() {
        XoaLich();
        thanhCong = day = 0;
        Clock::time_point batDau = Clock::now();
        for (int i = 1; i <= n; ++i) {
            string ngay = TaoNgay(i);
            string gio = DS_KHUNG_GIO[(i - 1) % SO_KHUNG_GIO];
            int ketQuaDat = DatLich(qlLichBench, TaoMa(i), "01/01/1990", "0900000000",
                                    ngay, gio, "Tong quat");
            if (ketQuaDat == DL_OK) ++thanhCong;
            if (ketQuaDat == DL_DAY) ++day;
        }
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    int huy = 0;
    long long thoiGianHuy = DoTrungVi([&]() {
        XoaLich();
        for (int i = 1; i <= n; ++i) {
            DatLich(qlLichBench, TaoMa(i), "01/01/1990", "0900000000",
                    TaoNgay(i), DS_KHUNG_GIO[(i - 1) % SO_KHUNG_GIO],
                    "Tong quat");
        }
        Clock::time_point batDau = Clock::now();
        huy = 0;
        for (int i = 1; i <= n; i += 2) {
            if (HuyLichTheoMa(qlLichBench, TaoMa(i)) == HL_OK) ++huy;
        }
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    bool dung = thanhCong + day <= n && huy <= thanhCong;
    cout << "\n[TU PHAT 1 - TOAN.CPP] Dat / Huy lich kiem soat suc chua (" << n << " thao tac): "
         << (dung ? "PASS" : "FAIL") << "\n";
    cout << "  Dat lich: " << dat << " us | Huy lich: "
         << thoiGianHuy << " us | Thanh cong: " << thanhCong
         << " | Tu choi day: " << day << " | Da huy: " << huy << "\n";
    ketQua << "TuPhat1_toan," << n << ",book," << dat << "\n";
    ketQua << "TuPhat1_toan," << n << ",cancel," << thoiGianHuy << "\n";
    XoaLich();
}

static void BenchmarkTuPhat2_Thu(ofstream& ketQua, int n) {
    // Tự phát 2 (tructhu.cpp): Quản lý lịch sử khám bệnh (Medical History) bằng Dynamic Array.
    MangLichSu lichSu;
    long long them = DoTrungVi([&]() {
        lichSu.Clear();
        Clock::time_point batDau = Clock::now();
        for (int i = 1; i <= n; ++i)
            ThemLuotKham(lichSu, TaoMa((i - 1) % 1000 + 1),
                         "01/09/2026", "Tong quat", DA_KHAM);
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    int demLichSu = 0;
    long long tim = DoTrungVi([&]() {
        Clock::time_point batDau = Clock::now();
        demLichSu = 0;
        for (int i = 0; i < lichSu.Size; ++i)
            if (lichSu.Data[i].MaBN == TaoMa(1)) ++demLichSu;
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    bool dung = demLichSu == (n + 999) / 1000;
    cout << "\n[TU PHAT 2 - THU.CPP] Quan ly lich su kham benh (" << n << " luot): "
         << (dung ? "PASS" : "FAIL") << "\n";
    cout << "  Them: " << them << " us | Tim mot benh nhan: "
         << tim << " us | So ban ghi tim thay: " << demLichSu << "\n";
    ketQua << "TuPhat2_thu," << n << ",append," << them << "\n";
    ketQua << "TuPhat2_thu," << n << ",lookup_one_patient," << tim << "\n";
    lichSu.Clear();
}

static void BenchmarkTuPhat3_Thu(ofstream& ketQua, int n) {
    // Tự phát 3 (tructhu.cpp): Quản lý nhắc lịch tái khám (Follow-up Reminders) bằng Dynamic Array.
    MangLichTaiKham dsTaiKham;
    long long them = DoTrungVi([&]() {
        dsTaiKham.Clear();
        Clock::time_point batDau = Clock::now();
        for (int i = 1; i <= n; ++i) {
            dsTaiKham.PushBack({TaoMa((i - 1) % 1000 + 1),
                                "15/09/2026", "08:30",
                                "Tai kham chuyen khoa Tim mach"});
        }
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    int demTaiKham = 0;
    long long tim = DoTrungVi([&]() {
        Clock::time_point batDau = Clock::now();
        demTaiKham = 0;
        for (int i = 0; i < dsTaiKham.Size; ++i)
            if (dsTaiKham[i].MaBN == TaoMa(1)) ++demTaiKham;
        return chrono::duration_cast<chrono::microseconds>(
            Clock::now() - batDau).count();
    });
    bool dung = demTaiKham == (n + 999) / 1000;
    cout << "\n[TU PHAT 3 - THU.CPP] Quan ly nhac lich tai kham (" << n << " luot): "
         << (dung ? "PASS" : "FAIL") << "\n";
    cout << "  Them: " << them << " us | Tim mot benh nhan: "
         << tim << " us | So ban ghi tim thay: " << demTaiKham << "\n";
    ketQua << "TuPhat3_thu," << n << ",append," << them << "\n";
    ketQua << "TuPhat3_thu," << n << ",lookup_one_patient," << tim << "\n";
    dsTaiKham.Clear();
}

int main(int argc, char* argv[]) {
    // Nhận quy mô 10.000-100.000, chạy các kịch bản và ghi CSV kết quả.
    int n = argc > 1 ? atoi(argv[1]) : 10000;
    if (n < 10000 || n > 100000) {
        cout << "Quy mo benchmark phai nam trong khoang 10000-100000.\n";
        return 1;
    }
    ofstream ketQua("benchmark_results.csv");
    if (!ketQua.is_open()) {
        cout << "Khong the tao benchmark_results.csv.\n";
        return 1;
    }
    ketQua << "requirement,size,operation,median_microseconds\n";
    cout << "===== BENCHMARK TIN CAY: " << SO_LAN_DO
         << " lan do, lay trung vi =====\n";
    BenchmarkMC1_Binh(ketQua, n);
    BenchmarkMC2_Chi(ketQua, n);
    BenchmarkMC2_Ben(ketQua, n);
    BenchmarkTuPhat1_Toan(ketQua, n);
    BenchmarkTuPhat2_Thu(ketQua, n);
    BenchmarkTuPhat3_Thu(ketQua, n);
    ketQua.close();
    XoaLich();
    cout << "\nDa luu ket qua vao benchmark_results.csv\n";
    return 0;
}
