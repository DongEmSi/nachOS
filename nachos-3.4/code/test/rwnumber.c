#include "syscall.h"
#include "copyright.h"

int main() {
	int a = ReadInt();
	PrintInt(a);

	Halt();
}
