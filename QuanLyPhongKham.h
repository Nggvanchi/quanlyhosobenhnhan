#pragma once
#include <string>
#include <cctype>

// Tệp tiêu đề trung tâm: khai báo cấu trúc dữ liệu, hằng số và nguyên mẫu
// dùng chung cho các mô-đun quản lý hồ sơ, lịch hẹn và lịch sử khám.
using namespace std;

// Mảng động tối giản, quản lý vùng nhớ bằng cấp phát thủ công.
// PushBack có chi phí trung bình O(1); Erase cần dồn phần tử nên là O(n).
template <typename T>
struct MangDong {
    T* Data = nullptr;
    int Size = 0, Capacity = 0;

    MangDong() = default;
    ~MangDong() { delete[] Data; }
    MangDong(const MangDong&) = delete;
    MangDong& operator=(const MangDong&) = delete;

    // Mở rộng sức chứa nhưng vẫn bảo toàn các phần tử hiện có.
    void Reserve(int cap) {
        if (cap <= Capacity) return;
        T* p = new T[Capacity = cap];
        for (int i = 0; i < Size; ++i) p[i] = Data[i];
        delete[] Data;
        Data = p;
    }

    // Thêm một phần tử vào cuối mảng, tự tăng sức chứa khi cần.
    void PushBack(const T& v) {
        if (Size == Capacity) Reserve(Capacity ? Capacity * 2 : 4);
        Data[Size++] = v;
    }

    // Xóa phần tử tại vị trí idx và dồn các phần tử phía sau sang trái.
    void Erase(int idx) {
        if (idx >= 0 && idx < Size) {
            for (int i = idx; i < Size - 1; ++i) Data[i] = Data[i + 1];
            --Size;
        }
    }

    // Giải phóng toàn bộ bộ nhớ và đưa mảng về trạng thái rỗng.
    void Clear() { delete[] Data; Data = nullptr; Size = Capacity = 0; }
    bool Empty() const { return !Size; }
    T& operator[](int i) { return Data[i]; }
    const T& operator[](int i) const { return Data[i]; }
};

// Các trạng thái thống nhất được dùng cho hồ sơ và lịch hẹn.
const string DANG_KHAM = "Dang kham", DA_KHAM = "Kham xong", DA_HUY = "Da huy", CHO_KHAM = "Cho kham";

const char* const DS_KHOA[] = {
    "Tong quat", "Tim mach", "Tai Mui Hong", "Da lieu", "Nhi", "San phu khoa",
    "Rang ham mat", "Tieu hoa", "Ho hap", "Co xuong khop", "Than kinh", "Mat"
};
const int SO_KHOA = 12, SO_KHUNG_GIO = 26, K_TOI_DA = 5;
const int MUC_UU_TIEN_CAO = 1, MUC_UU_TIEN_TRUNG_BINH = 2, MUC_UU_TIEN_THAP = 3;

// Danh mục 26 khung giờ khám hợp lệ trong giờ làm việc [07:30 - 20:00] (mỗi ca 30 phút)
const char* const DS_KHUNG_GIO[SO_KHUNG_GIO] = {
    "07:30", "08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30",
    "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30", "16:00",
    "16:30", "17:00", "17:30", "18:00", "18:30", "19:00", "19:30", "20:00"
};

struct HoSoBenhNhan {
    // Thông tin định danh, mức ưu tiên và trạng thái khám của một bệnh nhân.
    string MaBenhNhan, HoTen, NgaySinh, Sdt;
    int MucUuTien = MUC_UU_TIEN_THAP, ThuTuDangKy = 0;
    string TrangThai = CHO_KHAM, NgayDatLich, NgayTaiKham, NgayKhamGanNhat;
};

struct LichHen {
    // Một lượt đặt lịch, liên kết bệnh nhân với ngày, giờ và chuyên khoa.
    string MaBN, NgaySinh, Sdt, Ngay, KhungGio, ChuyenKhoa, TrangThai = CHO_KHAM;
};

struct LuotKham { string MaBN, Ngay, NoiDung, TrangThai; };
struct LichTaiKham { string MaBN, NgayTaiKham, Gio, NoiDung; };
struct SuatKhung { string Ngay, KhungGio; int SoDaDat = 0; };

using MangBenhNhan = MangDong<HoSoBenhNhan>;
using MangLichHen = MangDong<LichHen>;
using MangLichTaiKham = MangDong<LichTaiKham>;
using MangSuatKhung = MangDong<SuatKhung>;
using MangSoNguyen = MangDong<int>;
using MangLichSu = MangDong<LuotKham>;

extern MangSuatKhung DSSuat;

struct QuanLyLichHen {
    // Đóng gói mảng dữ liệu lịch hẹn gốc và mảng chỉ mục theo thời gian,
    // đảm bảo tính toàn vẹn (invariant) và loại bỏ biến toàn cục.
    MangLichHen DanhSach;
    MangSoNguyen IdxTheoGio;

    void Clear() { DanhSach.Clear(); IdxTheoGio.Clear(); }
    int Size() const { return DanhSach.Size; }
    bool Empty() const { return DanhSach.Empty(); }
    LichHen& operator[](int i) { return DanhSach[i]; }
    const LichHen& operator[](int i) const { return DanhSach[i]; }
};

const int SIZE_BANG_BAM = 150007, MAX_BAN_GHI = 100000;

struct BangBam {
    // Bảng băm hồ sơ dùng dò tuyến tính (linear probing), giúp tra cứu
    // trung bình O(1) thay vì phải duyệt tuần tự toàn bộ danh sách.
    HoSoBenhNhan* DanhSach = nullptr;
    int SoLuong = 0;
    MangSoNguyen ThuTu;

    BangBam() { DanhSach = new HoSoBenhNhan[SIZE_BANG_BAM]; }
    ~BangBam() { delete[] DanhSach; }
    BangBam(const BangBam&) = delete;
    BangBam& operator=(const BangBam&) = delete;
};

// Kiểm tra một ký tự có phải chữ số ASCII hay không.
inline bool LaSo(char C) { return C >= '0' && C <= '9'; }

// Cắt khoảng trắng ở đầu và cuối chuỗi dữ liệu đọc từ tệp hoặc bàn phím.
inline string CatChuoi(const string& S) {
    int l = 0, r = (int)S.size() - 1;
    while (l <= r && (S[l] == ' ' || S[l] == '\t' || S[l] == '\r' || S[l] == '\n')) ++l;
    while (r >= l && (S[r] == ' ' || S[r] == '\t' || S[r] == '\r' || S[r] == '\n')) --r;
    return (l > r) ? "" : S.substr(l, r - l + 1);
}

// So sánh hai chuỗi không phân biệt chữ hoa/chữ thường.
inline int SoSanhKhongPhanBietHoaThuong(const string &A, const string &B) {
    for (size_t i = 0; i < A.size() && i < B.size(); ++i) {
        int d = tolower((unsigned char)A[i]) - tolower((unsigned char)B[i]);
        if (d) return d;
    }
    return (int)A.size() - (int)B.size();
}

// Kiểm tra ngày theo định dạng DD/MM/YYYY và giới hạn năm đã cho.
inline bool NgayHopLe(const string &Ngay, int NamMin, int NamMax) {
    if (Ngay.size() != 10 || Ngay[2] != '/' || Ngay[5] != '/') return false;
    for (int i = 0; i < 10; ++i) if (i != 2 && i != 5 && !LaSo(Ngay[i])) return false;
    int d = stoi(Ngay.substr(0, 2)), m = stoi(Ngay.substr(3, 2)), y = stoi(Ngay.substr(6, 4));
    if (y < NamMin || y > NamMax || m < 1 || m > 12 || d < 1) return false;
    const int mDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxD = (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) ? 29 : mDays[m - 1];
    return d <= maxD;
}

// Kiểm tra ngày khám bệnh thuộc năm hiện tại (2026).
inline bool NgayKhamHopLe(const string &Ngay) { return NgayHopLe(Ngay, 2026, 2026); }

// Kiểm tra ngày sinh hợp lệ trong khoảng năm 1600 đến năm hiện tại.
inline bool NgaySinhHopLe(const string &Ngay) { return NgayHopLe(Ngay, 1600, 2026); }

// Kiểm tra số điện thoại gồm 10 chữ số và bắt đầu bằng chữ số 0.
inline bool SdtHopLe(const string &Sdt) {
    if (Sdt.size() != 10 || Sdt[0] != '0') return false;
    for (char c : Sdt) if (!LaSo(c)) return false;
    return true;
}

// Kiểm tra khung giờ có thuộc danh sách các khung giờ khám hợp lệ của phòng khám.
inline bool KhungGioHopLe(const string &Gio) {
    for (int i = 0; i < SO_KHUNG_GIO; ++i) if (DS_KHUNG_GIO[i] == Gio) return true;
    return false;
}

// Chuyển giờ dạng HH:MM thành tổng số phút kể từ 00:00.
inline int SoPhutTrongNgay(const string &Gio) {
    if (Gio.size() < 5) return 0;
    return ((Gio[0] - '0') * 10 + (Gio[1] - '0')) * 60 + ((Gio[3] - '0') * 10 + (Gio[4] - '0'));
}
// Mã hóa ngày thành số YYYYMMDD để so sánh theo thứ tự thời gian.
inline long long KhoaNgay(const string& Ngay) {
    if (Ngay.size() != 10) return 0;
    long long dd = (Ngay[0] - '0') * 10 + (Ngay[1] - '0'), mm = (Ngay[3] - '0') * 10 + (Ngay[4] - '0');
    long long yy = (Ngay[6] - '0') * 1000 + (Ngay[7] - '0') * 100 + (Ngay[8] - '0') * 10 + (Ngay[9] - '0');
    return yy * 10000 + mm * 100 + dd;
}
// Mã hóa kết hợp ngày và giờ thành một khóa số duy nhất để sắp xếp.
inline long long KhoaNgayGio(const string& Ngay, const string& Gio) {
    long long kn = KhoaNgay(Ngay);
    if (!kn) return 0;
    int gio = (Gio.size() == 5) ? ((Gio[0] - '0') * 10 + (Gio[1] - '0')) * 100 + ((Gio[3] - '0') * 10 + (Gio[4] - '0')) : 0;
    return kn * 10000 + gio;
}

void NapDuLieuTuCSV(BangBam&, QuanLyLichHen&, MangBenhNhan&, const string&, MangLichSu* = nullptr);
bool LuuDuLieuVaoCSV(const BangBam&, const QuanLyLichHen&, const string&);
void NapLichTaiKhamTuCSV(MangLichTaiKham&, BangBam&, const string&);
bool LuuLichTaiKhamVaoCSV(const MangLichTaiKham&, const string&);
void ThemLuotKham(MangLichSu&, const string&, const string&, const string&, const string&);
bool ValidateMaBN(const string&);
bool ValidateHoTen(const string&);
bool ValidateNgaySinh(const string&);
bool NgayTruyXuatHopLe(const string&);
bool LichHenThuocNgay(const LichHen&, const string&);
void XuLyTraCuu(BangBam&, const QuanLyLichHen&, const string&);
void DongBoTrangThaiHoSo(BangBam&, const QuanLyLichHen&, const string&);
int TimViTriUuTienCaoNhat(const MangBenhNhan&);
void XuLyUuTienCaoNhat(const MangBenhNhan&);
void XemBenhNhanTheoMucUuTien(BangBam&, const QuanLyLichHen&, const string&);
void XuLyDatHuy(BangBam&, QuanLyLichHen&, MangBenhNhan&);
int TimSuat(const string&, const string&);
bool GioKhamHopLe(const string&);
bool CoLichHoatDong(const QuanLyLichHen&, const string&);
int DatLich(QuanLyLichHen&, const string&, const string&, const string&, const string&, const string&, const string&);
int HuyLichTheoMa(QuanLyLichHen&, const string&);
bool DanhDauDaKham(QuanLyLichHen&, const string&, string&, string&);
bool GhiNhanKhamXong(BangBam&, QuanLyLichHen&, MangLichSu&, MangBenhNhan&, const string&);
void XuLyLichSu(BangBam&, QuanLyLichHen&, MangLichSu&, MangBenhNhan&, const string&);
void XuLyNhacTaiKham(MangLichTaiKham&, BangBam&);
int HamBam(const string&);
int TimViTri(const BangBam&, const string&);
bool InsertHoSo(BangBam&, const HoSoBenhNhan&);
int TimViTriDauTienTheoNgayGio(const QuanLyLichHen&, const string&, int);
int TimViTriDauTienSauNgayGio(const QuanLyLichHen&, const string&, int);
void ThemLich(QuanLyLichHen&, const LichHen&);
void XemTheoThuTuDangKy(const QuanLyLichHen&, BangBam&, const string&);
void LayTheoKhoang(const QuanLyLichHen&, BangBam&, const string&, const string&, const string&);
bool ThemBenhNhan(MangBenhNhan&, HoSoBenhNhan);
void XoaBenhNhanKhoiHangDoi(MangBenhNhan&, const string&);
