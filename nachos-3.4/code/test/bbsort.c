#include "syscall.h"
#define SIZE 50

int main()
{
    // Khai báo tất cả các biến trong quá trình làm bài
    int n, a[SIZE + 1], temp;
    int tt, i, j; 

    // Vòng lặp để xử lý các lỗi không đáng có từ người dùng 
    do 
    {
        PrintString("Nhap vao so n (voi 0 <= n <= 100): ");
        n = ReadInt(); // Hàm readint đọc dữ liệu người dùng vào biến n

        if (n > 100 || n < 0)
            PrintString("n da vuot qua dieu kien cho phep, vui long nhap lai. \n");

    } while (n > 100 || n < 0);

    // Thông báo để nhập các phần tử.
    for (i = 0; i < n; i++) 
    {
        PrintString("Nhap phan tu a[");
        PrintInt2(i);
        PrintString("]: ");
        a[i] = ReadInt();
    }

    // Vòng lặp để xử lý các lỗi nhập vào từ người dùng nếu khác hai số 0 và 1
    do 
    {
        PrintString("Nhap vao thu tu de sap xep [1]: theo chieu tang dan, [2]: theo chieu giam dan): ");
        tt = ReadInt();

        if (tt != 1 && tt != 2)
            PrintString("Nhap vao sai, moi nhap lai! \n");

    } while (tt != 1 && tt != 2);

    // Sử dụng sort bubble cho các chiều xuôi và chiều thuận.
    for (i = 0; i < n; i++) 
    {
        for (j = 0; j < n - 1; j++) 
        {
            if (tt == 2) 
            {
                if (a[j] < a[j + 1]) 
                {
                    temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                }
            }
            else if (tt == 1) 
            {
                if (a[j] > a[j + 1]) 
                {
                    temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                }
            }
        }
    }

    // In ra mảng sau khi đã sắp xếp xong.
    PrintString("Mang sau khi sort: ");
    for (i = 0; i < n; i++) 
    {
        PrintInt2(a[i]);
        PrintChar(' ');
    }

    Halt();
}
