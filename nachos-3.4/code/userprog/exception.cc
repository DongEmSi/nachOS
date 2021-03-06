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
	// m?? system call s??? ???????c ????a v??o thanh ghi r2 (c?? th??? xem l???i ph???n x??? l?? cho
	// system call Halt trong t???p tin start.s ??? tr??n)
	// tham s??? th??? 1 s??? ???????c ????a v??o thanh ghi r4
	// tham s??? th??? 2 s??? ???????c ????a v??o thanh ghi r5
	// tham s??? th??? 3 s??? ???????c ????a v??o thanh ghi r6
	// tham s??? th??? 4 s??? ???????c ????a v??o thanh ghi r7
	// k???t qu??? th???c hi???n c???a system call s??? ???????c ????a v??o thanh ghi r2
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
					int maxBuf = 255; // Theo chu???n C++ th?? integer c?? 4 bytes.
					char* buf = new char[maxBuf + 1];
					bool isNegative = false;
					int start = 0, size, ans = 0;
					
					// ?????c s??? l?????ng c??c ch??? s??? t??? m??n h??nh.
					size = synchcons->Read(buf, maxBuf);

					// X??? l?? l???i khi ng?????i d??ng kh??ng nh???p g??.
					if (size == 0) {
						DEBUG('a', "\n Invalid number.");
						printf("\n\n Invalid number.");

						machine->WriteRegister(2, 0);
						IncreasePC();
						delete buf;
						return;
					}
					
					// Ki???m tra xem s??? ??ang ?????c c?? ph???i l?? s??? ??m hay kh??ng, n???u l?? s??? ??m th?? cu???i h??m ti???p t???c x??? l??.
					if (buf[0] == '-') {
						isNegative = true;
						start = 1; // V???n ?????c ti???p nh??ng b??? qua d???u tr???
					}

					for (int i = start; i < size; ++i) {
						if (buf[i] == '.') { // X??? l?? tr?????ng h???p c?? d???u ch???m, v?? d??? n???u ng?????i d??ng nh???p 1.00000 th?? v???n l?? 1.
							for (int j = i + 1; j < size; ++j) {
								if (buf[j] != '0') { // N???u c??c s??? sau d???u ch???m kh??c kh??ng th?? coi nh?? s??? ???? kh??ng h???p l??? v?? ??ang l??m ki???u int
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
						else if (buf[i] < '0' || buf[i] > '9') { // X??? l?? tr?????ng h???p ng?????i d??ng nh???p k?? t??? kh??ng ph???i l?? s???.
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
							if (start > 10 && !isNegative) { // X??? l?? tr?????ng h???p n???u nh?? s??? qu?? l???n, ta quy v??? s??? l???n nh???t c?? th??? c??. 
								ans = 2147483647;

								machine->WriteRegister(2, ans);
								IncreasePC();
								delete buf;
								return;
							}
							else if (start > 11 && isNegative) {
								ans = -2147483647; // X??? l?? t????ng t??? nh?? tr?????ng h???p s??? qu?? nh???
								machine->WriteRegister(2, ans);
								IncreasePC();
								delete buf;
								return;
							}
						}
					}

					if (isNegative) ans *= -1; // Ti???p t???c x??? l?? tr?????ng h???p n???u s??? ?????c v??o l?? s??? ??m.

					machine->WriteRegister(2, ans);
					IncreasePC();
					delete buf;
					return;
				}

				case SC_PrintInt2:
				{
					int number = machine->ReadRegister(4); // ?????c s??? t??? m??n h??nh ng?????i d??ng nh???p.

					if (number == 0) { // Tr?????ng h???p s??? l?? 0 th?? coi n?? l?? tr?????ng h???p ?????c bi???t
						synchcons->Write("0", 1);
						IncreasePC();
						break;
					}

					int maxBuf = 255;
					char *buf = new char[maxBuf + 1];
					bool isNegative = false;
					int start = 0;

					// X??? l?? tr?????ng h???p n???u l?? s??? ??m
					if (number < 0) {
						isNegative = true; // ????nh d???u ????? cu???i h??m x??? l??
						number *= -1; // Chuy???n th??nh s??? d????ng ????? t??nh to??n ???????c d??? h??n
						buf[start++] = '-'; // buf[0] = '-'
					}

					while (number != 0) { // Chuy???n ?????i s??? th??nh k?? t???
						buf[start++] = (number % 10) + '0';
						number /= 10;
					}

					if (isNegative) { // X??? l?? tr?????ng h???p s??? ??m
						buf[0] = '-';
						buf[start] = '\0';

						reverse(buf, 1, start - 1); // ?????c chu???i b??? ng?????c n??n ph???i ?????o l???i chu???i

						synchcons->Write(buf, start);
						IncreasePC();
						break;
					}

					buf[start] = '\0';

					// Tr?????ng h???p s??? l?? d????ng
					reverse(buf, 0, start - 1);
					synchcons->Write(buf, start);
					IncreasePC();
					break;
				}

				case SC_ReadChar:
				{
					// H??m ?????c m???t k?? t??? duy nh???t t??? ng???oi d??ng nh???p
					int maxBuf = 255;
					char* buf = new char[255];
					int numBuf = synchcons->Read(buf, maxBuf);

					// X??? l?? c??c ngo???i l??? t??? ng?????i d??ng nh???p v??o
					// N???u ng?????i d??ng nh???p v??o nhi???u h??n 1 k?? t??? s??? x???y ra l???i, ta c???n th??ng b??o l???i n??y:
					if (numBuf > 1)
					{
						printf("This is not a character.");
						DEBUG('a', "\nERROR: This is not a character.");
						machine->WriteRegister(2, 0);
					}
					// Ng?????i d??ng kh??ng nh???p ho???c ????y l?? k?? t??? r???ng
					else if (numBuf == 0)
					{
						printf("No input!");
						DEBUG('a', "\nERROR: No input!");
						machine->WriteRegister(2, 0);
					}
					else 
					{
						// L???y ????ng k?? t??? ?????u ti??n c???a chu???i v???a nh???p ????a v??o thanh ghi th??? 2.
						machine->WriteRegister(2, buf[0]);
					}

					IncreasePC();
					delete buf;
					break;
				}

				case SC_PrintChar:
				{
					// ??? thanh ghi 4 c?? k?? t??? ???????c l??u v??o, ta ?????c k?? t??? trong thanh ghi n??y b???ng l???nh ??? d?????i ????y:
					char c = (char)machine->ReadRegister(4);
					
					// S??? d???ng bi???n to??n c???c c???a l???p synchconsole ????? in k?? t??? bi???n c ra m??n h??nh, n?? l?? 1 byte
					synchcons->Write(&c, 1);
					IncreasePC();
					break;
				}

				case SC_ReadString:
				{
					// L???y ?????a ch??? t??? thanh ghi th??? 4
					int virtAddr = machine->ReadRegister(4);

					// L???y size c???a chu???i t??? thanh ghi th??? 5.
					int size = machine->ReadRegister(5);

					// N???u size l?? s??? ??m ho???c l?? 0 th?? ????y l?? chu???i kh??ng h???p l???, ta t???t h??? ??i???u h??nh ????? tr??nh b??? treo.
					if (size <= 0)
					{
						interrupt->Halt();
						return; 
					}

					// C???p ph??t v??ng nh??? cho bi???n buffer l??u chu???i.
					char* buf = new char[size + 1];

					// L???y ????? d??i t???i ??a m?? n?? ?????c ???????c ????? ki???m tra t??nh h???p l???.
					int check = synchcons->Read(buf, size);

					// N???u nh?? kh??ng ?????c ???????c th?? s??? ng???t h??? ??i???u h??nh.
					if (check == -1) 
					{
						interrupt->Halt();
						return; 
					} 
					
					// T????ng t??? cho ng?????i d??ng kh??ng nh???p g?? ho???c ???? l?? k?? t??? tr???ng
					if (check == 0) break;

					// Th??m v??o cu???i chu???i k?? t??? tho??t.
					buf[check] = '\0';

					// Copy gi?? tr??? t??? system space sang user space. 
					System2User(virtAddr, size + 1, buf);

					IncreasePC();
					delete buf;
					break;
				}

				case SC_PrintString:
				{
					int virtAddr;
					char* buf;

					// L???y ?????a ch??? t??? thanh ghi th??? 4
					virtAddr = machine->ReadRegister(4);

					// Bi???n buf c???a kernel l???y t??? user (?????a ch??? chu???i t??? th??nh ghi th??? 4)
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
					char* buf = new char[maxBuf];

					// H??m rand t???o s??? ng???u nhi??n, gi???i h???n kho???ng c???a n?? d?????i 100000
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
					// L???y tham s??? t??n t???p tin t??? thanh ghi r4
					virtAddr = machine->ReadRegister(4);
					DEBUG ('a',"\n Reading filename.");
					// MaxFileLength l?? = 32
					filename = User2System(virtAddr,MaxFileLength+1);
					if (filename == NULL){
						printf("\n Not enough memory in system");
						DEBUG('a',"\n Not enough memory in system");
						machine->WriteRegister(2,-1); // tr??? v??? l???i cho ch????ng
						// tr??nh ng?????i d??ng
						
						IncreasePC();
						delete filename;
						break;
					}
					DEBUG('a',"\n Finish reading filename.");
					//DEBUG(???a???,"\n File name : '"<<filename<<"'");
					// Create file with size = 0
					// D??ng ?????i t?????ng fileSystem c???a l???p OpenFile ????? t???o file,
					// vi???c t???o file n??y l?? s??? d???ng c??c th??? t???c t???o file c???a h??? ??i???u
					// h??nh Linux, ch??ng ta kh??ng qu???n ly tr???c ti???p c??c block tr??n
					// ????a c???ng c???p ph??t cho file, vi???c qu???n ly c??c block c???a file
					// tr??n ??? ????a l?? m???t ????? ??n kh??c
					if (!fileSystem->Create(filename,0)){
						printf("\n Error create file '%s'",filename);
						machine->WriteRegister(2,-1);
						IncreasePC();
						delete filename;
						break;
					}
					machine->WriteRegister(2,0); // tr??? v??? cho ch????ng tr??nh
					// ng?????i d??ng th??nh c??ng
					IncreasePC();
					delete filename;
					break;
				}
				case SC_Open:
				{
					// Output: Tra ve OpenFileID neu thanh cong, -1 neu loi
					//OpenFileID Open(char *name, int type)
					int virtAddr = machine->ReadRegister(4);
					char* filename;
					filename = User2System(virtAddr, MaxFileLength);
					int slot = fileSystem->findEmptyPosition();
					if ((fileSystem->opfile[slot] = fileSystem->Open(filename)) == NULL) {
						printf("%s: File not exist!\n", filename);
						interrupt->Halt();
					}
					if (slot != -1) {
						printf("\nMo file thanh cong: %s", filename);
						printf("\nSlot: %d", slot);
						machine->WriteRegister(2, slot);
						IncreasePC();
						delete[] filename;
						break;
					}
					machine->WriteRegister(2, -1); //Khong mo duoc file return -1

					delete[] filename;
					break;
				}

				case SC_Close:
				{
					// void Close(OpenFileId id);
					// Output: 0: thanh cong, -1 that bai
					int fileid = machine->ReadRegister(4);
					if (fileid >= 0 && fileid <= 9)
					{
						if (fileSystem->opfile[fileid]) //neu mo file thanh cong
						{
							printf("\nClose file");
							delete fileSystem->opfile[fileid]; //Xoa vung nho luu tru file
							fileSystem->opfile[fileid] = NULL; //Gan vung nho NULL
							machine->WriteRegister(2, 0);
							IncreasePC();
							break;
						}
					}
					//printf("\nClose file");
					machine->WriteRegister(2, -1);
					IncreasePC();
					break;
				}

				case SC_Read:
				{
					int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
					int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
					int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6 
					int OldPos, NewPos;
					char* buffer;
					// Check id in file board
					if (id < 0 || id > 9)
					{
						printf("\nFile board description out of range!");
						machine->WriteRegister(2, -1);
						IncreasePC();
						return;
					}
					if (fileSystem->opfile[id] == NULL)
					{
						printf("\nFile not exist!");
						machine->WriteRegister(2, -1);
						IncreasePC();
						return;
					}
					OldPos = fileSystem->opfile[id]->getCurPos(); // Kiem tra thanh cong thi lay vi tri OldPos
					buffer = User2System(virtAddr, charcount); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
					// Xet truong hop doc file binh thuong thi tra ve so byte thuc su
					if ((fileSystem->opfile[id]->Read(buffer, charcount)) > 0) {
						// So byte thuc su = NewPos - OldPos
						NewPos = fileSystem->opfile[id]->getCurPos();
						// Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su 
						System2User(virtAddr, NewPos - OldPos, buffer);
						machine->WriteRegister(2, NewPos - OldPos);
						//printf("%s", buffer);
						IncreasePC();
					}
					else {
						// Truong hop con lai la doc file co noi dung la NULL tra ve -2
						printf("\nEmpty file");
						machine->WriteRegister(2, -2);
						IncreasePC();
					}
					delete[] buffer;
					return;
				}
				case SC_Write:
				{
					int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
					int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
					int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6
					int OldPos, NewPos;
					char* buffer;
					if (id < 0 || id > 9)
					{
						printf("\nFile board description out of range!");
						machine->WriteRegister(2, -1);
						IncreasePC();
						return;
					}
					// Kiem tra file co ton tai khong
					if (fileSystem->opfile[id] == NULL)
					{
						printf("\nFile not exist!");
						machine->WriteRegister(2, -1);
						IncreasePC();
						return;
					}
					OldPos = fileSystem->opfile[id]->getCurPos(); // Kiem tra thanh cong thi lay vi tri OldPos
					buffer = User2System(virtAddr, charcount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
					if ((fileSystem->opfile[id]->Write(buffer, charcount)) > 0){
							// So byte thuc su = NewPos - OldPos
							printf("Runable\n");
							NewPos = fileSystem->opfile[id]->getCurPos();
							machine->WriteRegister(2, NewPos - OldPos);
							delete[] buffer;
							IncreasePC();
							return;
					}
				}
				default:
					printf("\n Unexpected user mode exception (%d %d)", which, type);
					interrupt->Halt();
					break;
			}
			break;
	}
}
