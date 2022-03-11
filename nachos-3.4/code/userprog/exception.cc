// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "time.h"
#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
// Input: - User space address (int)
//- Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf,0,limit+1);
	//printf("\n Filename u2s:");
	for (i = 0 ; i < limit ;i++)
	{
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: - User space address (int)
//- Limit of buffer (int)
//- Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	}while(i < len && oneChar != 0);

	return i;
}


void IncreasePC() {
	int pcAfter = machine->registers[NextPCReg] + 4;
	machine->registers[PrevPCReg] = machine->registers[PCReg];	// for debugging, in case we
												// are jumping into lala-land
    machine->registers[PCReg] = machine->registers[NextPCReg];
    machine->registers[NextPCReg] = pcAfter;
}

void reverse(char src[], int start, int end) {
    while (start < end){
        char temp = src[start]; //a l  => temp = a,   a = l -> l, l = temp = a -> a
        src[start] = src[end];
        src[end] = temp;
        start++;
        end--;
    }
}

void 
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    // Input: reg4 -filename (string)
	// Output: reg2 -1: error and 0: success
	// Purpose: process the event SC_Create of System call
	// mã system call sẽ được đưa vào thanh ghi r2 (có thể xem lại phần xử lý cho
	// system call Halt trong tập tin start.s ở trên)
	// tham số thứ 1 sẽ được đưa vào thanh ghi r4
	// tham số thứ 2 sẽ được đưa vào thanh ghi r5
	// tham số thứ 3 sẽ được đưa vào thanh ghi r6
	// tham số thứ 4 sẽ được đưa vào thanh ghi r7
	// kết quả thực hiện của system call sẽ được đưa vào thanh ghi r2
	switch (which) {
		case NoException:
			return;

		case PageFaultException:
			DEBUG('a', "\nPage fault.");
			printf("\n\nPage fault.");
			interrupt->Halt();
			break;

		case ReadOnlyException:
			DEBUG('a', "\nPage marked read-only");
			printf("\n\nPage marked read-only");
			interrupt->Halt();
			break;

		case BusErrorException:
			DEBUG('a', "\nInvalid physical address");
			printf("\n\nInvalid physical address");
			interrupt->Halt();
			break;

		case AddressErrorException:
			DEBUG('a', "\n Address error.");
			printf("\n\n Address error.");
			interrupt->Halt();
			break;

		case OverflowException:
			DEBUG('a', "\nOverflow !!!");
			printf("\n\nOverflow !!!");
			interrupt->Halt();
			break;

		case IllegalInstrException:
			DEBUG('a', "\nIllegal instr.");
			printf("\n\nIllegal instr.");
			interrupt->Halt();
			break;

		case NumExceptionTypes:
			DEBUG('a', "\nNumber exception types");
			printf("\n\nNumber exception types");
			interrupt->Halt();
			break;
		
		case SyscallException:
			switch (type){
				case SC_Halt:
					DEBUG('a', "\nShutdown, initiated by user program.");
					printf("\n\nShutdown, initiated by user program.");
					interrupt->Halt();
					break;

				case SC_Exit: 
				{
					Exit(0);
					IncreasePC();
					break;
				}
				//Nho them increasePC vao truoc break hoac return nhe 2 ban!

				case SC_ReadInt: 
				{
					int maxBuf = 255; // Theo chuẩn C++ thì integer có 4 bytes.
					char* buf = new char[maxBuf + 1];
					bool isNegative = false;
					int start = 0, size, ans = 0;
					
					// Đọc số lượng các chữ số từ màn hình.
					size = synchcons->Read(buf, maxBuf);

					// Xử lý lỗi khi người dùng không nhập gì.
					if (size == 0) {
						DEBUG('a', "\n Invalid number.");
						printf("\n\n Invalid number.");

						machine->WriteRegister(2, 0);
						IncreasePC();
						delete buf;
						return;
					}
					
					// Kiểm tra xem số đang đọc có phải là số âm hay không, nếu là số âm thì cuối hàm tiếp tục xử lý.
					if (buf[0] == '-') {
						isNegative = true;
						start = 1; // Vẫn đọc tiếp nhưng bỏ qua dấu trừ
					}

					for (int i = start; i < size; ++i) {
						if (buf[i] == '.') { // Xử lí trường hợp có dấu chấm, ví dụ nếu người dùng nhập 1.00000 thì vẫn là 1.
							for (int j = i + 1; j < size; ++j) {
								if (buf[j] != '0') { // Nếu các số sau dấu chấm khác không thì coi như số đó không hợp lệ vì đang làm kiểu int
									DEBUG('a', "\n Invalid number.");
									printf("\n\n Invalid number.");

									machine->WriteRegister(2, 0);
									IncreasePC();
									delete buf;
									return;
								}
							}
							machine->WriteRegister(2, ans);
							IncreasePC();
							delete buf;
							return;
						}
						else if (buf[i] < '0' || buf[i] > '9') { // Xử lý trường hợp người dùng nhập ký tự không phải là số.
							DEBUG('a', "\n Invalid number.");
							printf("\n\n Invalid number.");

							machine->WriteRegister(2, 0);
							IncreasePC();
							delete buf;
							return;
						}
						else {
							ans = ans*10 + (buf[i] - '0');
							++start;
							if (start > 10 && !isNegative) { // Xử lý trường hợp nếu như số quá lớn, ta quy về số lớn nhất có thể có. 
								ans = 2147483647;

								machine->WriteRegister(2, ans);
								IncreasePC();
								delete buf;
								return;
							}
							else if (start > 11 && isNegative) {
								ans = -2147483647; // Xử lý tương tự như trường hợp số quá nhỏ
								machine->WriteRegister(2, ans);
								IncreasePC();
								delete buf;
								return;
							}
						}
					}

					if (isNegative) ans *= -1; // Tiếp tục xử lý trường hợp nếu số đọc vào là số âm.

					machine->WriteRegister(2, ans);
					IncreasePC();
					delete buf;
					return;
				}

				case SC_PrintInt2:
				{
					int number = machine->ReadRegister(4); // Đọc số từ màn hình người dùng nhập.

					if (number == 0) { // Trường hợp số là 0 thì coi nó là trường hợp đặc biệt
						synchcons->Write("0", 1);
						IncreasePC();
						break;
					}

					int maxBuf = 255;
					char *buf = new char[maxBuf + 1];
					bool isNegative = false;
					int start = 0;

					// Xử lý trường hợp nếu là số âm
					if (number < 0) {
						isNegative = true; // Đánh dấu để cuối hàm xử lý
						number *= -1; // Chuyển thành số dương để tính toán được dễ hơn
						buf[start++] = '-'; // buf[0] = '-'
					}

					while (number != 0) { // Chuyển đổi số thành ký tự
						buf[start++] = (number % 10) + '0';
						number /= 10;
					}

					if (isNegative) { // Xử lý trường hợp số âm
						buf[0] = '-';
						buf[start] = '\0';

						reverse(buf, 1, start - 1); // Đọc chuỗi bị ngược nên phải đảo lại chuỗi

						synchcons->Write(buf, start);
						IncreasePC();
						break;
					}

					buf[start] = '\0';

					// Trường hợp số là dương
					reverse(buf, 0, start - 1);
					synchcons->Write(buf, start);
					IncreasePC();
					break;
				}

				case SC_ReadChar:
				{
					// Hàm đọc một ký tự duy nhất từ ngừoi dùng nhập
					int maxBuf = 255;
					char* buf = new char[255];
					int numBuf = synchcons->Read(buf, maxBuf);

					// Xử lý các ngoại lệ từ người dùng nhập vào
					// Nếu người dùng nhập vào nhiều hơn 1 kí tự sẽ xảy ra lỗi, ta cần thông báo lỗi này:
					if (numBuf > 1)
					{
						printf("This is not a character.");
						DEBUG('a', "\nERROR: This is not a character.");
						machine->WriteRegister(2, 0);
					}
					// Người dùng không nhập hoặc đây là kí tự rỗng
					else if (numBuf == 0)
					{
						printf("No input!");
						DEBUG('a', "\nERROR: No input!");
						machine->WriteRegister(2, 0);
					}
					else 
					{
						// Lấy đúng kí tự đầu tiên của chuỗi vừa nhập đưa vào thanh ghi thứ 2.
						machine->WriteRegister(2, buf[0]);
					}

					IncreasePC();
					delete buf;
					break;
				}

				case SC_PrintChar:
				{
					// Ở thanh ghi 4 có kí tự được lưu vào, ta đọc kí tự trong thanh ghi này bằng lệnh ở dưới đây:
					char c = (char)machine->ReadRegister(4);
					
					// Sử dụng biến toàn cục của lớp synchconsole để in kí tự biến c ra màn hình, nó là 1 byte
					synchcons->Write(&c, 1);
					IncreasePC();
					break;
				}

				case SC_ReadString:
				{
					// Lấy địa chỉ từ thanh ghi thứ 4
					int virtAddr = machine->ReadRegister(4);

					// Lấy size của chuỗi từ thanh ghi thứ 5.
					int size = machine->ReadRegister(5);

					// Nếu size là số âm hoặc là 0 thì đây là chuỗi không hợp lệ, ta tắt hệ điều hành để tránh bị treo.
					if (size <= 0)
					{
						interrupt->Halt();
						return; 
					}

					// Cấp phát vùng nhớ cho biến buffer lưu chuỗi.
					char* buf = new char[size + 1];

					// Lấy độ dài tối đa mà nó đọc được để kiểm tra tính hợp lệ.
					int check = synchcons->Read(buf, size);

					// Nếu như không đọc được thì sẽ ngắt hệ điều hành.
					if (check == -1) 
					{
						interrupt->Halt();
						return; 
					} 
					
					// Tương tự cho người dùng không nhập gì hoặc đó là kí tự trống
					if (check == 0) break;

					// Thêm vào cuối chuỗi kí tự thoát.
					buf[check] = '\0';

					// Copy giá trị từ system space sang user space. 
					System2User(virtAddr, size + 1, buf);

					IncreasePC();
					delete buf;
					break;
				}

				case SC_PrintString:
				{
					int virtAddr;
					char* buf;

					// Lấy địa chỉ từ thanh ghi thứ 4
					virtAddr = machine->ReadRegister(4);

					// Biến buf của kernel lấy từ user (địa chỉ chuỗi từ thành ghi thứ 4)
					buf = User2System(virtAddr, 255);

					int size = 0;
					while (buf[size] != '\0') size++;

					synchcons->Write(buf, size + 1);

					delete buf;
					IncreasePC();
					break;
				}

				case SC_Rand:
				{
					int start = 0;
					int maxBuf = 255;
					char* buf = new char[255];

					// Hàm rand tạo số ngẫu nhiên, giới hạn khoảng của nó dưới 100000
					RandomInit(time(NULL));
					int r = Random() % 100000;
					while (r != 0) {
						buf[start++] = (r % 10) + '0';
						r /= 10;
					}
					buf[start] = '\0';
					reverse(buf, 0, start - 1);
					synchcons->Write(buf, start);
					IncreasePC();
					break;
				}


				case SC_Create:
				{
					int virtAddr;
					char* filename;
					DEBUG('a',"\n SC_Create call ...");
					DEBUG('a',"\n Reading virtual address of filename");
					// Lấy tham số tên tập tin từ thanh ghi r4
					virtAddr = machine->ReadRegister(4);
					DEBUG ('a',"\n Reading filename.");
					// MaxFileLength là = 32
					filename = User2System(virtAddr,MaxFileLength+1);
					if (filename == NULL){
						printf("\n Not enough memory in system");
						DEBUG('a',"\n Not enough memory in system");
						machine->WriteRegister(2,-1); // trả về lỗi cho chương
						// trình người dùng
						
						//IncreasePC();
						delete filename;
						break;
					}
					DEBUG('a',"\n Finish reading filename.");
					//DEBUG(‘a’,"\n File name : '"<<filename<<"'");
					// Create file with size = 0
					// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
					// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
					// hành Linux, chúng ta không quản ly trực tiếp các block trên
					// đĩa cứng cấp phát cho file, việc quản ly các block của file
					// trên ổ đĩa là một đồ án khác
					if (!fileSystem->Create(filename,0)){
						printf("\n Error create file '%s'",filename);
						machine->WriteRegister(2,-1);
						//IncreasePC();
						delete filename;
						break;
					}
					machine->WriteRegister(2,0); // trả về cho chương trình
					// người dùng thành công
					//IncreasePC();
					delete filename;
					break;
				}
				default:
					printf("\n Unexpected user mode exception (%d %d)", which, type);
					interrupt->Halt();
					break;
			}
			break;
	}
}
