#include <iostream>
#include <string>
#include "QuanLyPhongKham.h"

using namespace std;

// Mang dong quan ly danh sach LuotKham (dung con tro tho theo de bai)
struct DynamicArray {
    LuotKham* data;
    int size;
    int capacity;
};

void khoiTao(DynamicArray& a) {
    a.size = 0;
    a.capacity = 2;
    a.data = new LuotKham[a.capacity];
}

void resize(DynamicArray& a) {
    int newCapacity = a.capacity * 2;
    LuotKham* newData = new LuotKham[newCapacity];

    for (int i = 0; i < a.size; i++) {
        newData[i] = a.data[i];
    }

    delete[] a.data;
    a.data = newData;
    a.capacity = newCapacity;
}

void themLuotKham(
    DynamicArray& a,
    const string& ngay,
    const string& noiDung,
    const string& trangThai
) {
    if (a.size == a.capacity) {
        resize(a);
    }

    // Gan truc tiep bang std::string, khong dung strcpy
    a.data[a.size].ngay = ngay;
    a.data[a.size].noiDung = noiDung;
    a.data[a.size].trangThai = trangThai;

    a.size++;
}

void inLichSu(const DynamicArray& a) {
    for (int i = 0; i < a.size; i++) {
        cout << "Ngay: " << a.data[i].ngay << endl;
        cout << "Noi dung: " << a.data[i].noiDung << endl;
        cout << "Trang thai: " << a.data[i].trangThai << endl;
        cout << "--------------------" << endl;
    }
}

void giaiPhong(DynamicArray& a) {
    delete[] a.data;
    a.data = nullptr;
    a.size = 0;
    a.capacity = 0;
}

// Chuyen tu main() thanh ham chuc nang rieng de goi tu main chung cua nhom
void chayDemoLichSuKham() {
    DynamicArray lichSu;
    khoiTao(lichSu);

    themLuotKham(
        lichSu,
        "01/09/2026",
        "Tim mach",
        TrangThai::DA_KHAM
    );

    themLuotKham(
        lichSu,
        "05/09/2026",
        "Noi khoa",
        TrangThai::DA_KHAM
    );

    themLuotKham(
        lichSu,
        "10/09/2026",
        "Tim mach",
        TrangThai::DA_HUY
    );

    inLichSu(lichSu);

    giaiPhong(lichSu);
}
