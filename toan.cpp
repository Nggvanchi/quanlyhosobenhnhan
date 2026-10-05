#include <iostream>
#include "QuanLyPhongKham.h"

using namespace std;

// PHẦN 1: BỘ ĐẾM SỨC CHỨA KHUNG GIỜ VÀ MÃ TRẢ VỀ
const int DL_OK = 0, DL_DAU_VAO_SAI = 1, DL_DAY = 2, DL_TRUNG = 3; // Mã kết quả đặt lịch (thành công, lỗi dữ liệu, đầy, trùng)
const int HL_OK = 0, HL_KHONG_TIM_THAY = 1, HL_DA_HUY_ROI = 2, HL_DA_KHAM = 3; // Mã kết quả hủy lịch (thành công, không thấy, đã hủy, đã khám)
MangSuatKhung DSSuat; // Mảng động lưu số lượt đặt theo cặp (Ngay, KhungGio) để kiểm soát sức chứa K_TOI_DA = 5

// PHẦN 2: CÁC HÀM TÌM KIẾM VÀ KIỂM TRA ĐIỀU KIỆN
// Tìm vị trí bộ đếm số chỗ theo cặp (ngày, khung giờ); trả về -1 nếu khung giờ chưa có ai đặt
int TimSuat(const string& Ngay, const string& Gio) {
    for (int i = 0; i < DSSuat.Size; ++i) 
        if (DSSuat[i].Ngay == Ngay && DSSuat[i].KhungGio == Gio) return i; // Khớp ngày và khung giờ -> trả về vị trí
    return -1; // Khung giờ chưa từng có ai đặt lịch
}

// Kiểm tra khung giờ thuộc danh mục hợp lệ và nằm trong giờ làm việc [07:30 - 20:00]
bool GioKhamHopLe(const string& Gio) {
    if (!KhungGioHopLe(Gio)) return false; // Kiểm tra thuộc danh mục khung giờ hợp lệ để tránh lỗi chuỗi sai định dạng
    int p = SoPhutTrongNgay(Gio);          // Đổi HH:MM thành số phút tính từ 00:00
    return p >= 450 && p <= 1200;          // Trong khoảng làm việc 07:30 (450p) - 20:00 (1200p)
}

// Chuẩn hóa tên chuyên Khoa nhập vào về đúng tên chuẩn trong DS_KHOA (không phân biệt hoa/thường)
string ChuanHoaKhoa(const string& s) {
    for (int i = 0; i < SO_KHOA; ++i) 
        if (!SoSanhKhongPhanBietHoaThuong(DS_KHOA[i], s)) return DS_KHOA[i]; // Khớp tên -> trả về chuỗi chuẩn
    return ""; // Tên chuyên Khoa không hợp lệ
}

// Kiểm tra chuỗi nhập vào có phải số nguyên dương tối đa 2 chữ số (dùng để chọn số thứ tự)
static bool ChuoiToanSo(const string& s) {
    return !s.empty() && s.size() <= 2 && LaSo(s[0]) && (s.size() == 1 || LaSo(s[1])); // Đúng 1 hoặc 2 chữ số
}

// PHẦN 3: GIAO DIỆN CHỌN CHUYÊN Khoa VÀ KHUNG GIỜ
// Hiển thị danh sách 12 chuyên Khoa, hỗ trợ chọn bằng STT (1-12) hoặc nhập trực tiếp tên Khoa
string ChonChuyenKhoa() {
    for (int i = 0; i < SO_KHOA; ++i) cout << i + 1 << ". " << DS_KHOA[i] << "\n"; // In danh mục 12 Khoa
    for (string s; cout << "Chon chuyen Khoa (0 = huy): ", getline(cin, s); ) {
        if ((s = CatChuoi(s)) == "0") return ""; // Người dùng chọn 0 để hủy thao tác
        int v = ChuoiToanSo(s) ? stoi(s) : 0; // Chuyển chuỗi STT thành số nếu hợp lệ
        if (v >= 1 && v <= SO_KHOA) return DS_KHOA[v - 1]; // Chọn hợp lệ theo số thứ tự
        string k = ChuanHoaKhoa(s); // Kiểm tra hợp lệ theo tên chuyên Khoa nhập tay
        if (!k.empty()) return k; // Trả về tên Khoa chuẩn nếu khớp
        cout << "Loi: khong hop le, hay chon trong danh sach.\n"; // Báo lỗi và yêu cầu nhập lại
    }
    return "";
}

// Hiển thị danh mục khung giờ theo dạng bảng 4 cột và nhận lựa chọn theo số thứ tự (1-26)
string ChonKhungGioTheoSTT() {
    cout << "\n===== DANH SACH KHUNG GIO KHAM =====\n";
    for (int i = 0; i < SO_KHUNG_GIO; ++i) // In dạng 4 cột dùng tab \t
        cout << (i + 1 < 10 ? " " : "") << i + 1 << ". " << DS_KHUNG_GIO[i] << ((i + 1) % 4 == 0 ? "\n" : "\t");
    if (SO_KHUNG_GIO % 4 != 0) cout << "\n";
    cout << "Nhap STT khung Gio (1-" << SO_KHUNG_GIO << "): ";
    int STT; // Số thứ tự khung giờ người dùng chọn
    if (!(cin >> STT) || STT < 1 || STT > SO_KHUNG_GIO) { cin.clear(); cin.ignore(10000, '\n'); return ""; } // Nhập sai -> hủy
    return DS_KHUNG_GIO[STT - 1]; // Trả về chuỗi HH:MM tương ứng với số thứ tự
}

// Kiểm tra mã bệnh nhân đã tồn tại trong hàng đợi ưu tiên hay chưa (tránh trùng lặp trong hàng đợi)
static bool CoTrongHangDoi(const MangBenhNhan& HangDoi, const string& MaBN) {
    for (int i = 0; i < HangDoi.Size; ++i) if (HangDoi[i].MaBenhNhan == MaBN) return true; // Đã có trong hàng đợi
    return false; // Chưa có trong hàng đợi
}

// Cơ chế hủy mềm (Soft-delete): chuyển trạng thái sang "Da huy" và hoàn trả 1 suất trong DSSuat
static int GiaiPhongSuat(QuanLyLichHen& QLLich, int ViTri) {
    QLLich[ViTri].TrangThai = DA_HUY; // Hủy mềm: đổi trạng thái nhưng giữ nguyên dòng lịch sử
    int vt = TimSuat(QLLich[ViTri].Ngay, QLLich[ViTri].KhungGio); // Tìm vị trí bộ đếm sức chứa của khung giờ này
    if (vt >= 0 && DSSuat[vt].SoDaDat > 0) --DSSuat[vt].SoDaDat; // Giảm 1 suất đã đặt để nhường chỗ cho bệnh nhân khác
    return HL_OK; // Trả về mã hủy thành công
}

// Xác định mã lỗi khi không tìm thấy lịch hoạt động để hủy
static int MaLoiHuy(int DaKham, int BatKy) {
    return DaKham >= 0 ? HL_DA_KHAM : (BatKy >= 0 ? HL_DA_HUY_ROI : HL_KHONG_TIM_THAY); // Đã khám xong / Đã hủy trước đó / Không tồn tại
}

// PHẦN 4: NGHIỆP VỤ ĐẶT VÀ HỦY LỊCH KHÁM
// Đặt lịch khám: kiểm tra điều kiện, chống trùng lịch cùng giờ, khống chế sức chứa K <= 5, tạo lịch mới
int DatLich(QuanLyLichHen& QLLich, const string& MaBN, const string& NgaySinh, const string& Sdt,
            const string& Ngay, const string& Gio, const string& ChuyenKhoa) {
    string Khoa = ChuanHoaKhoa(ChuyenKhoa); // Chuẩn hóa tên chuyên Khoa
    if (!ValidateMaBN(MaBN) || !NgaySinhHopLe(NgaySinh) || !NgayKhamHopLe(Ngay) || !GioKhamHopLe(Gio) || Khoa.empty() || !SdtHopLe(Sdt))
        return DL_DAU_VAO_SAI; // Dữ liệu đầu vào không hợp lệ

    // Kiểm tra chống trùng lịch: từ chối nếu bệnh nhân đã có lịch chờ hoặc đang khám trong cùng khung giờ
    for (int i = 0; i < QLLich.Size(); ++i)
        if (QLLich[i].MaBN == MaBN && QLLich[i].Ngay == Ngay && QLLich[i].KhungGio == Gio &&
            (QLLich[i].TrangThai == CHO_KHAM || QLLich[i].TrangThai == DANG_KHAM))
            return DL_TRUNG; // Báo trùng lịch hẹn

    int vt = TimSuat(Ngay, Gio); // Tra cứu số suất đã đặt của khung giờ này
    if (vt >= 0 && DSSuat[vt].SoDaDat >= K_TOI_DA) return DL_DAY; // Khung giờ đã đạt sức chứa tối đa 5 người
    if (vt < 0) { DSSuat.PushBack({Ngay, Gio, 0}); vt = DSSuat.Size - 1; } // Lần đầu có người đặt: khởi tạo bộ đếm mới

    ThemLich(QLLich, {MaBN, NgaySinh, Sdt, Ngay, Gio, Khoa}); // Thêm lịch vào danh sách và cập nhật mảng chỉ mục IdxTheoGio
    ++DSSuat[vt].SoDaDat; // Tăng số lượng đã đặt thêm 1
    return DL_OK; // Trả về mã thành công
}

// Hủy lịch chờ khám có thời điểm sớm nhất của bệnh nhân (dựa trên so sánh khóa ngày giờ KhoaNgayGio)
int HuyLichTheoMa(QuanLyLichHen& QLLich, const string& MaBN) {
    int ViTriChoKham = -1, ViTriDaKham = -1, ViTriBatKy = -1;
    long long KhoaGanNhat = 0; // Lưu khóa ngày giờ nhỏ nhất để tìm lịch diễn ra sớm nhất
    for (int i = 0; i < QLLich.Size(); ++i) {
        const LichHen& lich = QLLich[i];
        if (lich.MaBN != MaBN) continue; // Khác mã bệnh nhân -> bỏ qua
        if (ViTriBatKy == -1) ViTriBatKy = i; // Ghi nhận có lịch tồn tại trong hệ thống
        if (lich.TrangThai == CHO_KHAM) {
            long long k = KhoaNgayGio(lich.Ngay, lich.KhungGio); // Mã hóa YYYYMMDDHHMM thành số nguyên 64-bit
            if (ViTriChoKham == -1 || k < KhoaGanNhat) {
                ViTriChoKham = i;
                KhoaGanNhat = k; // Cập nhật mốc sớm nhất
            }
        } else if (lich.TrangThai == DA_KHAM) ViTriDaKham = i; // Lưu vết lịch đã khám xong
    }
    return ViTriChoKham >= 0 ? GiaiPhongSuat(QLLich, ViTriChoKham) : MaLoiHuy(ViTriDaKham, ViTriBatKy); // Giải phóng suất hoặc trả lỗi
}

// Chức năng 6: Điều phối giao diện người dùng cho thao tác đặt lịch mới hoặc hủy lịch khám
void XuLyDatHuy(BangBam& HeThongHoSo, QuanLyLichHen& QLLich, MangBenhNhan& HangDoiUuTien) {
    int Chon; // Lưu lựa chọn thao tác (1: Đặt lịch, 2: Hủy lịch)
    cout << "\n--- QUAN LY DAT / HUY LICH KHAM ---\n1. Dat lich kham moi\n2. Huy lich kham\nChon thao tac: ";
    if (!(cin >> Chon)) { cin.clear(); cin.ignore(10000, '\n'); cout << "=> Lua chon khong hop le.\n"; return; }
    string MaBN;
    cout << "Nhap Ma BN: "; cin >> MaBN;
    int v = TimViTri(HeThongHoSo, MaBN); // Tra cứu vị trí hồ sơ trong bảng băm với chi phí O(1)
    if (v == -1) { cout << "=> Khong tim thay benh nhan.\n"; return; } // Bệnh nhân chưa đăng ký hồ sơ
    HoSoBenhNhan& HoSo = HeThongHoSo.DanhSach[v]; // Lấy tham chiếu đến hồ sơ bệnh nhân trong bảng băm

    if (Chon == 1) { // THAO TÁC 1: ĐẶT LỊCH KHÁM MỚI
        string Ngay, Gio, Khoa;
        cout << "Nhap Ngay kham: "; cin >> Ngay;
        if ((Gio = ChonKhungGioTheoSTT()).empty()) return; // Người dùng hủy hoặc nhập sai STT khung giờ
        cin.ignore(10000, '\n'); // Xóa bộ đệm bàn phím
        if ((Khoa = ChonChuyenKhoa()).empty()) return; // Người dùng hủy chọn chuyên Khoa
        int kq = DatLich(QLLich, HoSo.MaBenhNhan, HoSo.NgaySinh, HoSo.Sdt, Ngay, Gio, Khoa); // Tiến hành đặt lịch
        if (kq == DL_OK) {
            cout << "=> Dat lich thanh cong.\n";
            DongBoTrangThaiHoSo(HeThongHoSo, QLLich, HoSo.MaBenhNhan); // Cập nhật trạng thái "Cho kham" trong bảng băm
            if (!CoTrongHangDoi(HangDoiUuTien, HoSo.MaBenhNhan)) ThemBenhNhan(HangDoiUuTien, HoSo); // Đưa vào hàng đợi ưu tiên
        } else cout << (kq == DL_DAY ? "=> Khung Gio da day.\n" : (kq == DL_TRUNG ? "=> Lich da ton tai.\n" : "=> Thong tin khong hop le.\n"));
    } else if (Chon == 2) { // THAO TÁC 2: HỦY LỊCH KHÁM
        int k = HuyLichTheoMa(QLLich, MaBN); // Tự động tìm và hủy lịch chờ khám sớm nhất của bệnh nhân này
        if (k == HL_OK) {
            DongBoTrangThaiHoSo(HeThongHoSo, QLLich, MaBN); // Cập nhật lại trạng thái trong bảng băm
            if (!CoLichHoatDong(QLLich, MaBN)) XoaBenhNhanKhoiHangDoi(HangDoiUuTien, MaBN); // Xóa khỏi hàng đợi nếu không còn lịch nào
            cout << "=> Huy lich thanh cong.\n";
        } else cout << (k == HL_DA_KHAM ? "=> Lich da duoc kham.\n" : (k == HL_DA_HUY_ROI ? "=> Lich da bi huy.\n" : "=> Khong tim thay lich hen.\n"));
    }
}

// Chuyển lịch hẹn sang "Khám xong", trả về ngày và tên chuyên Khoa tương ứng (dùng khi bác sĩ hoàn tất khám)
bool DanhDauDaKham(QuanLyLichHen& QLLich, const string& MaBN, string& Ngay, string& ChuyenKhoa) {
    for (int i = 0; i < QLLich.Size(); ++i) {
        if (QLLich[i].MaBN == MaBN && QLLich[i].TrangThai == DANG_KHAM) {
            QLLich[i].TrangThai = DA_KHAM; // Cập nhật trạng thái lịch thành đã khám xong
            Ngay = QLLich[i].Ngay;                     // Lấy ngày khám thực tế của lịch hẹn
            ChuyenKhoa = QLLich[i].ChuyenKhoa;         // Lấy tên chuyên Khoa để ghi vào lịch sử khám bệnh
            return true;                               // Báo đánh dấu thành công
        }
    }
    return false; // Không tìm thấy lịch đang khám phù hợp
}
