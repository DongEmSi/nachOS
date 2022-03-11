#include "syscall.h"

/* Viết chương trình help, CT help dùng để in ra các dòng giới thiệu cơ bản về nhóm và mô
tả vắn tắt về chương trình sort và ascii. (thật ra: chỉ việc gọi system call
PrintString(char[])). */ 

int main()
{
    PrintString("Gioi thieu ve nhom 2: \n");
    PrintString("20127640: Ngo Dong Thuc. \n");
    PrintString("20127645: Le Dong Dong. \n");
    PrintString("20127258: Hoang Phuoc Nguyen. \n"); 
    PrintString("-----------------------------.\n");
    PrintString("Mo ta van tat ve chuong trinh: \n"); 
    PrintString("[1] In bang ma ASCII: Mot chuong trinh don gian ung dung nhung system calls da hoc in ra bang ma ASCII. \n"); 
    PrintString("Cach khoi dong chuong trinh: Vao folder chua code, mo terminal nhap ./userprog/nachos –rs 1023 –x ./test/ascii.\n");
    PrintString("[2] Sort so bang sot noi bot: \n");
    PrintString("B1: Nguoi dung nhap vao n, voi n la tong cac phan tu trong mang. \n");
    PrintString("B2: Nguoi dung chon 1 de sort mang tang dan, chon 2 de sort mang giam dan. \n");
    PrintString("Cach khoi dong chuong trinh: Vao folder chua code, mo terminal nhap ./userprog/nachos –rs 1023 –x ./test/bbsort. \n"); 

    Halt();
}
