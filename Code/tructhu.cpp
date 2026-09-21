#include <iostream>
#include <string>
#include "QuanLyPhongKham.h"

using namespace std;

// Mang dong quan ly danh sach LuotKham
struct DynamicArray {
    LuotKham* data;
    int size;
    int capacity;
};

// Khoi tao mang dong
void khoiTao(DynamicArray& a) {
    a.size = 0;
    a.capacity = 2;
    a.data = new LuotKham[a.capacity];
}

// Nhan doi kich thuoc mang khi bi day
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

// Them mot luot kham vao cuoi mang
void themLuotKham(
    DynamicArray& a,
    const string& ngay,
    const string& noiDung,
    const string& trangThai
) {
    // Kiem tra suc chua truoc khi them
    if (a.size == a.capacity) {
        resize(a);
    }
    a.data[a.size].ngay = ngay;
    a.data[a.size].noiDung = noiDung;
    a.data[a.size].trangThai = trangThai;

    a.size++;
}

// In toan bo lich su kham cua benh nhan
void inLichSu(const DynamicArray& a) {
    if (a.size == 0) {
        cout << "Chua co lich su kham.\n";
        return;
    }
    
    for (int i = 0; i < a.size; i++) {
        cout << "Ngay: " << a.data[i].ngay << "\n";
        cout << "Noi dung: " << a.data[i].noiDung << "\n";
        cout << "Trang thai: " << a.data[i].trangThai << "\n";
        cout << "--------------------\n";
    }
}

void giaiPhong(DynamicArray& a) {
    delete[] a.data;
    a.data = nullptr;
    a.size = 0;
    a.capacity = 0;
}