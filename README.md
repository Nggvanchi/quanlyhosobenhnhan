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

### 1. Quản lý và Tra cứu hồ sơ bệnh nhân (Yêu cầu bắt buộc MC1)
* **Nhận diện duy nhất:** Mỗi bệnh nhân được cấp một mã duy nhất (định dạng `BNxxxxxx`) giữ nguyên trong suốt các lần thăm khám.
* **Tra cứu chính xác:** Cho phép nhân viên tra cứu nhanh chóng và chính xác thông tin bệnh nhân (Họ tên, ngày sinh, SĐT, trạng thái khám...) chỉ thông qua Mã bệnh nhân bằng Bảng băm (Hash Table) O(1).

### 2. Điều phối hàng đợi ưu tiên & Truy xuất theo khoảng (Yêu cầu bắt buộc MC2)
* Hệ thống tự động sắp xếp hàng đợi dựa trên **mức độ khẩn cấp** của bệnh nhân bằng Counting Sort ổn định O(N).
* Các ca cấp cứu (Mức 1) hoặc ưu tiên cao (Mức 2) luôn được đẩy lên đầu hàng đợi để xử lý trước các bệnh nhân thông thường (Mức 3), xem bệnh nhân có mức độ ưu tiên cao nhất.
* Hỗ trợ truy xuất danh sách bệnh nhân theo thứ tự thời gian đăng ký hoặc theo khoảng thời gian khám cụ thể bằng mảng chỉ mục kết hợp Binary Search O(log N + K).

### 3. Đặt lịch, Hủy lịch và Kiểm soát suất khám (Yêu cầu tự phát hiện 1)
* Bệnh nhân có thể chủ động chọn chuyên khoa, ngày và khung giờ khám.
* Hệ thống tự động kiểm soát **Giới hạn sức chứa khung giờ**. Nếu một khung giờ đã đủ người (ví dụ tối đa 5 người/suất), hệ thống sẽ từ chối và yêu cầu chọn giờ khác để tránh quá tải.
* Hỗ trợ chức năng Hủy lịch, tự động giải phóng suất khám để bệnh nhân khác có thể đăng ký.

### 4. Quản lý lịch sử khám bệnh (Yêu cầu tự phát hiện 2)
* **Lịch sử khám:** Lưu trữ toàn bộ chuỗi lịch sử các lần đến khám của bệnh nhân (Ngày khám, chuyên khoa, trạng thái: *Đã khám/Đã hủy/Đang chờ*) bằng Dynamic Array tự co giãn không giới hạn, hỗ trợ tra cứu lịch sử theo mã bệnh nhân.

### 5. Nhắc lịch tái khám (Yêu cầu tự phát hiện 3)
* **Nhắc tái khám:** Cho phép bác sĩ thiết lập lịch hẹn tái khám và tự động nhắc nhở khi đến ngày, giúp bệnh nhân không bỏ lỡ phác đồ điều trị, quản lý linh hoạt bằng Dynamic Array.

## Hướng dẫn chạy thử

Dự án được thiết kế theo 3 tầng:

* **Presentation:** `main.cpp` chỉ hiển thị menu, nhận thao tác và gọi hàm.
* **DSA Core:** `binh.cpp`, `ben.cpp`, `chi.cpp`, `toan.cpp`, `tructhu.cpp` xử lý cấu trúc dữ liệu và nghiệp vụ.
* **Persistence:** `napdulieu.cpp` đọc và ghi dữ liệu CSV.

Các module `.cpp` vẫn được liên kết trực tiếp trong `main.cpp` theo yêu cầu của nhóm, nên bạn **chỉ cần biên dịch `main.cpp`**.

**Lưu ý:** Nếu không tìm thấy `data.csv` và `taikham.csv`, hệ thống sẽ khởi động với dữ liệu trống. Khi chọn thoát, hồ sơ và lịch đang hoạt động được lưu lại về `data.csv` và `taikham.csv`.

### Cách 1: Chạy bằng các phần mềm lập trình
Nếu bạn đang sử dụng Visual Studio Code, Dev-C++, Code::Blocks hoặc Visual Studio:
1. Mở thư mục chứa code của nhóm.
2. Mở file `main.cpp`.
3. Bấm nút **Run** (hoặc `F11` trên Dev-C++, nút ▷ Play trên VS Code) để biên dịch và chạy chương trình.

### Cách 2: Chạy bằng Terminal / Command Prompt
Nếu bạn quen dùng dòng lệnh, hãy mở Terminal tại thư mục chứa code và gõ:

```
# 1. Biên dịch file main.cpp
g++ main.cpp -o main

# 2. Chạy chương trình
# --- Trên Windows:
main.exe

# --- Trên Linux/macOS:
./main
```

## Benchmark hiệu năng MC1, MC2 và yêu cầu tự phát hiện

File `benchmark.cpp` đo thời gian thực tế bằng `std::chrono` cho toàn bộ
các yêu cầu chính. Mỗi phép đo có một lượt làm nóng, sau đó chạy 7 lần và
lấy trung vị; đồng thời chương trình kiểm tra tính đúng trước khi báo
`PASS`:

* **MC1 (Tra cứu bệnh nhân):** tra cứu Hash Table so với quét tuyến tính, gồm cả trường hợp mã tồn tại và mã không tồn tại.
* **MC2 (Truy xuất theo thứ tự ưu tiên, khoảng thời gian, mức ưu tiên, xem bệnh nhân mức độ ưu tiên cao nhất):**
  * Sắp xếp và phân bổ hàng đợi bằng Counting Sort ổn định theo mức độ ưu tiên và thứ tự đăng ký (xem bệnh nhân mức độ ưu tiên cao nhất).
  * Truy vấn khoảng thời gian bằng mảng chỉ số (`IdxTheoGio`) + Binary Search so với quét tuyến tính.
* **Yêu cầu tự phát hiện 1 (Đặt / Hủy lịch):** gọi trực tiếp các thao tác đặt lịch, hủy lịch và kiểm soát giới hạn sức chứa khung giờ.
* **Yêu cầu tự phát hiện 2 (Xem lịch sử khám):** thêm và tìm kiếm lịch sử khám bệnh của bệnh nhân bằng Dynamic Array.
* **Yêu cầu tự phát hiện 3 (Nhắc lịch tái khám):** thêm và lọc / nhắc lịch tái khám bằng Dynamic Array.

Biên dịch và chạy hai quy mô bắt buộc:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic benchmark.cpp -o benchmark.exe
.\benchmark.exe 10000
.\benchmark.exe 100000
```

Kết quả được in theo micro giây (`us`) và lưu thêm vào
`benchmark_results.csv`. Benchmark MC1 có cả mã tồn tại và mã không tồn tại;
MC2 đối chiếu số kết quả của chỉ mục với quét tuyến tính. Chạy mỗi quy mô
trên cùng máy và cùng tùy chọn `-O2` để so sánh công bằng.
Chương trình sinh dữ liệu cũng hỗ trợ chọn quy mô:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic data.cpp -o data.exe
.\data.exe 10000
.\data.exe 100000
```

`data.cpp` chỉ ghi tối đa 100.000 bản ghi và giữ nguyên định dạng CSV mà
tầng Persistence đang sử dụng.
