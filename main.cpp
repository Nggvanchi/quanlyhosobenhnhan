#include <iostream>
#include <string>
#include "QuanLyPhongKham.h"

// Bao gồm trực tiếp các mô-đun thành phần để tạo một chương trình đơn.
#include "binh.cpp"
#include "ben.cpp"
#include "chi.cpp"
#include "toan.cpp"
#include "tructhu.cpp"
#include "napdulieu.cpp"

using namespace std;

int main() {
    // Khởi tạo các cấu trúc dữ liệu dùng chung trong suốt vòng đời chương trình.
    BangBam HeThongHoSo;            // Bảng băm lưu trữ và tra cứu hồ sơ bệnh nhân.
    QuanLyLichHen QLLichHen;        // Quản lý danh sách lịch hẹn và chỉ mục thời gian.
    MangBenhNhan HangDoiUuTien;     // Hàng đợi bệnh nhân chờ khám theo mức ưu tiên.
    MangLichSu LichSuKhamChung;     // Mảng động lưu các lượt khám đã hoàn tất.
    MangLichTaiKham DSTaiKham;      // Mảng động quản lý lịch hẹn tái khám.

    // Nạp dữ liệu ban đầu từ CSV vào bảng băm, hàng đợi và danh sách lịch.
    NapDuLieuTuCSV(HeThongHoSo, QLLichHen, HangDoiUuTien, "data.csv", &LichSuKhamChung);
    NapLichTaiKhamTuCSV(DSTaiKham, HeThongHoSo, "taikham.csv");

    int LuaChon;
    do {
        // Hiển thị menu và chờ người dùng chọn một nghiệp vụ.
        cout << "\n================ QUAN LY PHONG KHAM ================\n"
                "1. Tra cuu ho so benh nhan\n"
                "2. Xem benh nhan theo thu tu dang ky\n"
                "3. Xem benh nhan co lich hen trong mot khoang thoi gian\n"
                "4. Xem benh nhan theo muc do uu tien\n"
                "5. Xac dinh benh nhan co muc do uu tien cao nhat\n"
                "6. Dat / Huy lich kham\n"
                "7. Xem lai lich su kham cua benh nhan\n"
                "8. Nhac lich tai kham\n"
                "0. Thoat chuong trinh\n"
                "====================================================\n"
                "Nhap lua chon cua ban: ";

        // Kiểm tra lỗi nhập sai kiểu số; xóa trạng thái lỗi trước khi lặp lại.
        if (!(cin >> LuaChon)) {
            cout << "Loi: Vui long nhap mot so hop le!\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        string s1, s2, s3; // Các chuỗi tạm nhận mã, ngày và khung giờ từ bàn phím.
        switch (LuaChon) {
            case 1: // Tra cứu hồ sơ bệnh nhân theo mã.
                cout << "Nhap Ma BN: "; cin >> s1;
                XuLyTraCuu(HeThongHoSo, QLLichHen, s1);
                break;

            case 2: // Xem danh sách theo thứ tự đăng ký trong ngày.
                cout << "Nhap ngay dang ky (DD/MM/YYYY): "; cin >> s1;
                XemTheoThuTuDangKy(QLLichHen, HeThongHoSo, s1);
                break;

            case 3: // Xem lịch hẹn trong khoảng thời gian [giờ bắt đầu, giờ kết thúc].
                cout << "Nhap ngay (DD/MM/YYYY): "; cin >> s1;
                cout << "Nhap gio bat dau: "; cin >> s2;
                cout << "Nhap gio ket thuc: "; cin >> s3;
                LayTheoKhoang(QLLichHen, HeThongHoSo, s1, s2, s3);
                break;

            case 4: // Xem danh sách sắp xếp theo mức ưu tiên 1, 2, 3.
                cout << "Nhap ngay can xem (DD/MM/YYYY): "; cin >> s1;
                XemBenhNhanTheoMucUuTien(HeThongHoSo, QLLichHen, s1);
                break;

            case 5: // Xác định bệnh nhân có mức ưu tiên cao nhất cần khám trước.
                XuLyUuTienCaoNhat(HangDoiUuTien);
                break;

            case 6: // Đặt lịch khám mới hoặc hủy lịch.
                XuLyDatHuy(HeThongHoSo, QLLichHen, HangDoiUuTien);
                break;

            case 7: // Xem lịch sử hoặc ghi nhận bệnh nhân đã khám xong.
                cout << "Nhap Ma BN: "; cin >> s1;
                XuLyLichSu(HeThongHoSo, QLLichHen, LichSuKhamChung, HangDoiUuTien, s1);
                break;

            case 8: // Đăng ký hoặc xem danh sách lịch tái khám.
                XuLyNhacTaiKham(DSTaiKham, HeThongHoSo);
                break;

            case 0: // Lưu dữ liệu hiện tại vào CSV rồi thoát chương trình.
                LuuDuLieuVaoCSV(HeThongHoSo, QLLichHen, "data.csv");
                LuuLichTaiKhamVaoCSV(DSTaiKham, "taikham.csv");
                cout << "Thoat chuong trinh...\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (LuaChon != 0);

    return 0;
}
