# quanlyhosobenhnhan
Đồ án cấu trúc dữ liệu và giải  - Hệ thống hàng đợi và hồ sơ bệnh nhân phòng khám

## Danh sách thành viên nhóm và Vai trò

| Họ và Tên | MSSV | Phân công / Vai trò |
| :--- | :---: | :--- |
| Mai Xuân Bình | 25110149 | Tra cứu hồ sơ, kiểm tra tính hợp lệ dữ liệu. |
| Trần Ngọc Bền | 25110148 | Quản lý bệnh nhân ưu tiên, sắp xếp hàng đợi. |
| Nguyễn Văn Chí | 25110154 | Quản lý và tìm kiếm lịch hẹn theo khoảng thời gian/trạng thái. |
| Nguyễn Quốc Toàn | 25110368 | Xử lý nghiệp vụ đặt lịch, hủy lịch và kiểm soát suất khám. |
| Võ Nguyễn Trúc Thư | 25110359 | Quản lý, lưu trữ lịch sử khám bệnh và nhắc lịch tái khám. |

## Giới thiệu đồ án

Phòng khám hàng ngày phải tiếp nhận và quản lý một lượng lớn bệnh nhân. Việc tìm kiếm tuần tự hồ sơ hoặc quản lý lịch khám bằng sổ sách truyền thống không còn hiệu quả khi số lượng bệnh nhân (N) lên đến hàng chục nghìn người. 

Đồ án "Hệ Thống Hàng Đợi và Hồ Sơ Bệnh Nhân Phòng Khám" được xây dựng nhằm giải quyết bài toán số hóa quy trình vận hành của phòng khám, đảm bảo tốc độ tra cứu siêu tốc, điều phối hàng đợi thông minh và quản lý phác đồ điều trị của bệnh nhân không bị đứt quãng.

**Các tính năng nổi bật của hệ thống:**

### 1. Quản lý và Tra cứu hồ sơ quy mô lớn (Yêu cầu bắt buộc)
* **Nhận diện duy nhất:** Mỗi bệnh nhân được cấp một mã duy nhất (định dạng `BNxxxxxx`) giữ nguyên trong suốt các lần thăm khám.
* **Tra cứu chính xác:** Cho phép nhân viên tra cứu nhanh chóng và chính xác thông tin bệnh nhân (Họ tên, ngày sinh, SĐT, trạng thái khám...) chỉ thông qua Mã bệnh nhân.

### 2. Điều phối hàng đợi thông minh (Yêu cầu bắt buộc)
* Hệ thống tự động sắp xếp hàng đợi dựa trên **mức độ khẩn cấp** của bệnh nhân. 
* Các ca cấp cứu (Mức 1) hoặc ưu tiên cao (Mức 2) luôn được đẩy lên đầu hàng đợi để xử lý trước các bệnh nhân thông thường (Mức 3), đảm bảo an toàn y tế.
* Hỗ trợ truy xuất danh sách bệnh nhân theo thứ tự thời gian đăng ký hoặc theo một khoảng thời gian khám cụ thể.

### 3. Đặt lịch, Hủy lịch và Kiểm soát suất khám (Yêu cầu tự phát hiện)
* Bệnh nhân có thể chủ động chọn chuyên khoa, ngày và khung giờ khám.
* Hệ thống tự động kiểm soát **Giới hạn sức chứa khung giờ**. Nếu một khung giờ đã đủ người (ví dụ tối đa 5 người/suất), hệ thống sẽ từ chối và yêu cầu chọn giờ khác để tránh quá tải.
* Hỗ trợ chức năng Hủy lịch, tự động giải phóng suất khám để bệnh nhân khác có thể đăng ký.

### 4. Quản lý lịch sử và Nhắc lịch tái khám (Yêu cầu tự phát hiện)
* **Lịch sử khám:** Lưu trữ toàn bộ chuỗi lịch sử các lần đến khám của bệnh nhân (Ngày khám, chuyên khoa, trạng thái: *Đã khám/Đã hủy/Đang chờ*). Cấu trúc dữ liệu tự động co giãn để lưu trữ số lượng lượt khám không giới hạn.
* **Nhắc tái khám:** Cho phép bác sĩ thiết lập lịch hẹn tái khám và tự động nhắc nhở khi đến ngày, giúp bệnh nhân không bỏ lỡ phác đồ điều trị.

## Hướng dẫn chạy thử

Dự án được thiết kế module hóa nhưng đã được liên kết toàn bộ vào file `main.cpp`. Do đó, bạn **chỉ cần chạy duy nhất file `main.cpp`** là toàn bộ hệ thống sẽ hoạt động mà không cần cấu hình phức tạp.

**Lưu ý:** Nhóm không tải lên file dữ liệu ban đầu. Hệ thống sẽ tự động nhận diện và khởi động với dữ liệu trống nếu không tìm thấy file data.

### Cách 1: Chạy bằng các phần mềm lập trình
Nếu bạn đang sử dụng Visual Studio Code, Dev-C++, Code::Blocks hoặc Visual Studio:
1. Mở thư mục chứa code của nhóm.
2. Mở file `main.cpp`.
3. Bấm nút **Run** (hoặc `F11` trên Dev-C++, nút ▷ Play trên VS Code) để biên dịch và chạy chương trình.

### Cách 2: Chạy bằng Terminal / Command Prompt
Nếu bạn quen dùng dòng lệnh, hãy mở Terminal tại thư mục chứa code và gõ:

```bash
# 1. Biên dịch file main.cpp
g++ main.cpp -o main

# 2. Chạy chương trình
# --- Trên Windows:
main.exe

# --- Trên Linux/macOS:
./main