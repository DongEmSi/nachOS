#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
	int len;
	char filename[maxlen +1];
	/*Create a file*/
	if (Create("helloworld.txt") == -1){
		// xuất thông báo lỗi tạo tập tin
		//printf("Loi !!! Khong the tao tap tin.\n");
	}
	else{
		// xuất thông báo tạo tập tin thành công
		//printf("Tao tap tin thanh cong.\n");
	}
	Halt();
}

