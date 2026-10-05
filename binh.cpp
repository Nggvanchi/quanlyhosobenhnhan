#include <iostream>
#include <ctime>
#include "QuanLyPhongKham.h"

using namespace std;

// ============================================================================
// PHẦN 1: KIỂM TRA DỮ LIỆU ĐẦU VÀO (VALIDATION)
// Đảm bảo dữ liệu hồ sơ chuẩn xác trước khi lưu trữ vào bảng băm.
// ============================================================================

// Kiểm tra mã bệnh nhân theo định dạng chuẩn: BNxxxxxx
// - Bắt buộc đúng 8 ký tự, bắt đầu bằng tiền tố "BN".
// - 6 ký tự tiếp theo phải là chữ số từ '0' đến '9'.
// - Độ phức tạp: O(1) vì chuỗi cố định 8 ký tự.
bool ValidateMaBN(const string& MaBN) {
    if (MaBN.size() != 8 || MaBN[0] != 'B' || MaBN[1] != 'N') return false;
    for (int i = 2; i < 8; ++i) if (!LaSo(MaBN[i])) return false;
    return true;
}

// Kiểm tra họ tên bệnh nhân:
// - Độ dài hợp lệ từ 2 đến 50 ký tự.
// - Không được chứa chữ số (chỉ chấp nhận chữ cái và khoảng trắng).
// - Độ phức tạp: O(L) với L là độ dài họ tên.
bool ValidateHoTen(const string& Ten) {
    if (Ten.size() < 2 || Ten.size() > 50) return false;
    for (char c : Ten) if (LaSo(c)) return false;
    return true;
}

// Kiểm tra ngày sinh bệnh nhân:
// - Đúng định dạng DD/MM/YYYY, ngày tháng hợp lệ theo lịch (kể cả năm nhuận).
// - Năm sinh trong khoảng [1600, 2026] và không được vượt quá ngày hiện tại.
// - Độ phức tạp: O(1).
bool ValidateNgaySinh(const string& NgaySinh) {
    if (!NgaySinhHopLe(NgaySinh)) return false;
    time_t now = time(0);
    tm* t = localtime(&now);
    int HomNay = (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
    return KhoaNgay(NgaySinh) <= HomNay;
}

// ============================================================================
// PHẦN 2: BẢNG BĂM HỒ SƠ BỆNH NHÂN (HASH TABLE - LINEAR PROBING)
// Cấu trúc dữ liệu chính phục vụ tra cứu thông tin bệnh nhân trong O(1).
// Sử dụng phương pháp địa chỉ mở (Open Addressing) với dò tuyến tính (Linear Probing).
// Kích thước bảng SIZE = 150007 (số nguyên tố lớn hơn 1.5 lần MAX_BAN_GHI 100000)
// để hệ số tải alpha <= 0.67, giảm thiểu tối đa hiện tượng cụm cụm (clustering).
// ============================================================================

// Hàm băm (Hash Function): dùng đa thức Horner cơ số 31.
// - Công thức: hash = (hash * 31 + ASCII(c)) mod SIZE_BANG_BAM.
// - Số 31 là số nguyên tố lẻ, giúp phân bố khóa đồng đều trên toàn bảng.
// - Độ phức tạp: O(L) với L = 8 (chiều dài mã BN) -> tương đương O(1).
int HamBam(const string& MaBN) {
    unsigned int Tong = 0;
    for (char c : MaBN) Tong = Tong * 31 + (unsigned char)c;
    return Tong % SIZE_BANG_BAM;
}

// Dò tuyến tính (Linear Probing): giải quyết va chạm (Collision Resolution).
// - Xuất phát từ vị trí băm v = HamBam(MaBN).
// - Nếu ô đã có hồ sơ khác, dò tiếp sang (v + 1) % SIZE_BANG_BAM cho đến khi gặp:
//   1. Ô có mã BN trùng khớp (đã tồn tại).
//   2. Ô trống (vị trí có thể chèn mới).
// - Độ phức tạp: Trung bình O(1), trường hợp xấu nhất O(SIZE_BANG_BAM).
static int TimOCoTheDung(const BangBam& Bang, const string& MaBN) {
    int v = HamBam(MaBN);
    for (int i = 0; i < SIZE_BANG_BAM; ++i, v = (v + 1) % SIZE_BANG_BAM)
        if (Bang.DanhSach[v].MaBenhNhan.empty() || Bang.DanhSach[v].MaBenhNhan == MaBN) return v;
    return -1;
}

// Tìm vị trí của hồ sơ bệnh nhân theo mã trong bảng băm.
// - Trả về chỉ số ô nếu tìm thấy, hoặc -1 nếu không tồn tại trong hệ thống.
// - Độ phức tạp kỳ vọng: O(1).
int TimViTri(const BangBam& Bang, const string& MaBN) {
    int v = TimOCoTheDung(Bang, MaBN);
    return (v >= 0 && Bang.DanhSach[v].MaBenhNhan == MaBN) ? v : -1;
}

// Thêm một hồ sơ bệnh nhân mới vào bảng băm:
// - Kiểm tra tính hợp lệ của mã, họ tên, ngày sinh, số điện thoại.
// - Kiểm tra ngưỡng tải tối đa MAX_BAN_GHI để tránh bảng bị đầy.
// - Từ chối nếu mã bệnh nhân đã tồn tại (chống trùng lặp).
// - Độ phức tạp trung bình: O(1).
bool InsertHoSo(BangBam& Bang, const HoSoBenhNhan& HoSo) {
    if (Bang.SoLuong >= MAX_BAN_GHI || !ValidateMaBN(HoSo.MaBenhNhan) || !ValidateHoTen(HoSo.HoTen) ||
        !ValidateNgaySinh(HoSo.NgaySinh) || !SdtHopLe(HoSo.Sdt)) return false;
    int v = TimOCoTheDung(Bang, HoSo.MaBenhNhan);
    if (v < 0 || Bang.DanhSach[v].MaBenhNhan == HoSo.MaBenhNhan) return false;
    Bang.DanhSach[v] = HoSo;
    Bang.ThuTu.PushBack(v);
    ++Bang.SoLuong;
    return true;
}

// ============================================================================
// PHẦN 3: ĐỒNG BỘ VÀ TRA CỨU HỒ SƠ BỆNH NHÂN
// ============================================================================

// Đồng bộ trạng thái mới nhất của hồ sơ bệnh nhân từ danh sách lịch hẹn:
// - Ưu tiên lịch "Đang khám" gần nhất -> trạng thái thành "Đang khám".
// - Kế tiếp ưu tiên lịch "Chờ khám" gần nhất -> trạng thái thành "Chờ khám".
// - Nếu không còn lịch hẹn hoạt động, cập nhật lịch "Khám xong" gần nhất.
// - Bỏ qua các lịch ở trạng thái "Đã hủy".
void DongBoTrangThaiHoSo(BangBam& HeThongHoSo, const QuanLyLichHen& QLLich, const string& MaBN) {
    int ViTriHoSo = TimViTri(HeThongHoSo, MaBN);
    if (ViTriHoSo < 0) return;

    int ViTriDangKham = -1, ViTriChoKham = -1, ViTriDaKham = -1;
    long long kDangKham = -1, kChoKham = -1, kDaKham = -1;

    for (int i = 0; i < QLLich.Size(); ++i) {
        const LichHen& lich = QLLich[i];
        if (lich.MaBN != MaBN || lich.TrangThai == DA_HUY) continue;
        long long k = KhoaNgayGio(lich.Ngay, lich.KhungGio);
        if (lich.TrangThai == DANG_KHAM && k > kDangKham) { kDangKham = k; ViTriDangKham = i; }
        else if (lich.TrangThai == CHO_KHAM && k > kChoKham) { kChoKham = k; ViTriChoKham = i; }
        else if (lich.TrangThai == DA_KHAM && k > kDaKham) { kDaKham = k; ViTriDaKham = i; }
    }

    HoSoBenhNhan& HoSo = HeThongHoSo.DanhSach[ViTriHoSo];
    int ViTriHoatDong = ViTriDangKham >= 0 ? ViTriDangKham : ViTriChoKham;
    if (ViTriHoatDong >= 0) {
        HoSo.TrangThai = QLLich[ViTriHoatDong].TrangThai;
        HoSo.NgayDatLich = QLLich[ViTriHoatDong].Ngay;
    } else if (ViTriDaKham >= 0) {
        HoSo.TrangThai = DA_KHAM;
        HoSo.NgayKhamGanNhat = QLLich[ViTriDaKham].Ngay;
        HoSo.NgayDatLich.clear();
    }
}

// Chức năng 1: Tra cứu và hiển thị chi tiết hồ sơ bệnh nhân theo mã định danh.
// - Bước 1: Kiểm tra định dạng mã bệnh nhân.
// - Bước 2: Dùng bảng băm tra cứu vị trí trong O(1).
// - Bước 3: Đồng bộ trạng thái hồ sơ với danh sách lịch hẹn thực tế.
// - Bước 4: In đầy đủ thông tin: họ tên, ngày sinh, SĐT, ngày khám, trạng thái.
void XuLyTraCuu(BangBam& HeThongHoSo, const QuanLyLichHen& QLLich, const string& MaBN) {
    if (!ValidateMaBN(MaBN)) { cout << "=> Ma benh nhan khong hop le.\n"; return; }
    int ViTri = TimViTri(HeThongHoSo, MaBN);
    if (ViTri == -1) { cout << "=> Khong tim thay ho so benh nhan trong he thong.\n"; return; }

    DongBoTrangThaiHoSo(HeThongHoSo, QLLich, MaBN);
    HoSoBenhNhan& HoSo = HeThongHoSo.DanhSach[ViTri];
    string NgayKham = !HoSo.NgayDatLich.empty() ? HoSo.NgayDatLich : (!HoSo.NgayTaiKham.empty() ? HoSo.NgayTaiKham : HoSo.NgayKhamGanNhat);

    cout << "\n===== KET QUA TRA CUU HO SO =====\n"
         << "- Ho va ten: " << HoSo.HoTen << "\n"
         << "- Ngay sinh: " << HoSo.NgaySinh << "\n"
         << "- So dien thoai: " << HoSo.Sdt << "\n"
         << "- Ngay kham: " << (NgayKham.empty() ? "(Chua dang ky)" : NgayKham) << "\n"
         << "- Thong tin lich hen: " << (!HoSo.NgayDatLich.empty() || !HoSo.NgayTaiKham.empty() ? "Co" : "Chua co") << "\n"
         << "- Trang thai kham hien tai: " << HoSo.TrangThai << "\n";
}
