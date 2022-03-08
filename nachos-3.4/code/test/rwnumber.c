#include "syscall.h"

int main() {
	//int a = ReadInt();
	char* buf;
	int length;

	ReadString(buf, length);

	PrintString(buf);

	Halt();
}
