#include "syscall.h"

int main() {
	int fileid, fileids;
	Create("test2.txt");
	fileid = Open("test.txt");
	Read("", 20, fileid);
	fileids = Open("test2.txt");
	PrintInt2(fileids);
	Write("", 20, fileids);
	Halt();
	/*char* buf;
	int length;

	ReadString(buf, length);

	PrintString(buf);

	Halt();*/
}
