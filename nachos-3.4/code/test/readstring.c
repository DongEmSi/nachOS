#include "syscall.h"

char string[256];

int main() {
    PrintString("Nhap vao mot chuoi: ");
    ReadString(string, 255);
    PrintString(string);
    Halt();
}