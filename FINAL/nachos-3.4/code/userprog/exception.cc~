
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

#define MaxFileLength 32
/*increase PC*/
void IncreasePC()
{
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}
// Input: Khong gian dia chi User(int) - gioi han cua buffer(int)
// Output: Bo nho dem Buffer(char*)
// Chuc nang: Sao chep vung nho User sang vung nho System
char *User2System(int virtAddr, int limit)
{
    int i; // chi so index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // can cho chuoi terminal
    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

/*add syscall handlers*/
void ExceptionHandlerHalt()
{
    DEBUG('a', "\n Shutdown, initiated by user program.");
    printf("\n Shutdown, initiated by user program.");
    interrupt->Halt();
}

void ExceptionHandlerReadInt()
{
    // Input: K co
    // Output: Tra ve so nguyen doc duoc tu man hinh console.
    // Chuc nang: Doc so nguyen tu man hinh console.
    char *buffer;
    int MAX_BUFFER = 255;
    buffer = new char[MAX_BUFFER + 1];
    int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER); // doc buffer toi da MAX_BUFFER ki tu, tra ve so ki tu doc dc
    int number = 0;                                         // so luu ket qua tra ve cuoi cung

    /* Qua trinh chuyen doi tu buffer sang so nguyen int */

    // Xac dinh so am hay so duong
    bool isNegative = false; // Gia thiet la so duong.
    int firstNumIndex = 0;
    int lastNumIndex = 0;
    if (buffer[0] == '-')
    {
        isNegative = true;
        firstNumIndex = 1;
        lastNumIndex = 1;
    }

    // Kiem tra tinh hop le cua so nguyen buffer
    for (int i = firstNumIndex; i < numbytes; i++)
    {
        if (buffer[i] == '.') /// 125.0000000 van la so
        {
            int j = i + 1;
            for (; j < numbytes; j++)
            {
                // So khong hop le
                if (buffer[j] != '0')
                {
                    printf("\n\n The integer number is not valid");
                    // DEBUG('a', "\n The integer number is not valid");
                    machine->WriteRegister(2, 0);
                    IncreasePC();
                    delete buffer;
                    return;
                }
            }
            // la so thoa cap nhat lastNumIndex
            lastNumIndex = i - 1;
            break;
        }
        else if (buffer[i] < '0' && buffer[i] > '9')
        {
            printf("\n\n The integer number is not valid");
            // DEBUG('a', "\n The integer number is not valid");
            machine->WriteRegister(2, 0);
            IncreasePC();
            delete buffer;
            return;
        }
        lastNumIndex = i;
    }

    // La so nguyen hop le, tien hanh chuyen chuoi ve so nguyen
    for (int i = firstNumIndex; i <= lastNumIndex; i++)
    {
        number = number * 10 + (int)(buffer[i] - 48);
    }

    // neu la so am thi * -1;
    if (isNegative)
    {
        number = number * -1;
    }
    machine->WriteRegister(2, number);
    delete buffer;
}
void ExceptionHandlerPrintInt()
{
    int n = machine->ReadRegister(4);
    /*int: [-2147483648 , 2147483647] --> max length = 11*/
    const int maxlen = 11;
    char num_string[maxlen] = {0};
    int tmp[maxlen] = {0}, i = 0, j = 0;
    if (n < 0)
    {
        n = -n;
        num_string[i++] = '-';
    }
    do
    {
        tmp[j++] = n % 10;
        n /= 10;
    } while (n);
    while (j)
        num_string[i++] = '0' + (char)tmp[--j];
    gSynchConsole->Write(num_string, i);
    machine->WriteRegister(2, 0);
}
void ExceptionHandlerReadChar()
{
    char c = 0;
    gSynchConsole->Read(&c, 1);
    machine->WriteRegister(2, (int)c);
}
void ExceptionHandlerPrintChar()
{
    char c = (char)machine->ReadRegister(4);
    gSynchConsole->Write(&c, 1);
    machine->WriteRegister(2, 0);
}
void ExceptionHandlerReadString()
{
    // Input: Buffer(char*), do dai toi da cua chuoi nhap vao(int)
    // Output: Khong co
    // Cong dung: Doc vao mot chuoi voi tham so la buffer va do dai toi da
    int virtAddr, length;
    char *buffer;
    virtAddr = machine->ReadRegister(4);    // Lay dia chi tham so buffer truyen vao tu thanh ghi so 4
    length = machine->ReadRegister(5);      // Lay do dai toi da cua chuoi nhap vao tu thanh ghi so 5
    buffer = User2System(virtAddr, length); // Copy chuoi tu vung nho User Space sang System Space
    gSynchConsole->Read(buffer, length);    // Goi ham Read cua SynchConsole de doc chuoi
    System2User(virtAddr, length, buffer);  // Copy chuoi tu vung nho System Space sang vung nho User Space
    delete[] buffer;
}
void ExceptionHandlerPrintString()
{
    // Input: Buffer(char*)
    // Output: Chuoi doc duoc tu buffer(char*)
    // Cong dung: Xuat mot chuoi la tham so buffer truyen vao ra man hinh
    int virtAddr;
    char *buffer;
    virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
    buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu
    int length = 0;
    while (buffer[length] != 0)
        length++;                             // Dem do dai that cua chuoi
    gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
    delete[] buffer;
}
void ExceptionHandlerWrite()
{
    // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
    // Output: -1: Loi, So byte write thuc su: Thanh cong, -2: Thanh cong
    // Cong dung: Ghi file voi tham so la buffer, so ky tu cho phep va id cua file
    int virtAddr = machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
    int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
    int id = machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
    int OldPos;
    int NewPos;
    char *buf;
    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
    if (id < 0 || id > 14)
    {
        printf("\nKhong the write vi id nam ngoai bang mo ta file.");
        machine->WriteRegister(2, -1);
        return;
    }
    // Kiem tra file co ton tai khong
    if (fileSystem->openf[id] == NULL)
    {
        printf("\nKhong the write vi file nay khong ton tai.");
        machine->WriteRegister(2, -1);
        return;
    }
    // Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
    if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
    {
        printf("\nKhong the write file stdin hoac file only read.");
        machine->WriteRegister(2, -1);
        return;
    }
    OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
    buf = User2System(virtAddr, charcount);          // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
    // Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
    if (fileSystem->openf[id]->type == 0)
    {
        if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
        {
            // So byte thuc su = NewPos - OldPos
            NewPos = fileSystem->openf[id]->GetCurrentPos();
            machine->WriteRegister(2, NewPos - OldPos);
            delete buf;
            return;
        }
    }
    if (fileSystem->openf[id]->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
    {
        int i = 0;
        while (buf[i] != 0 && buf[i] != '\n') // Vong lap de write den khi gap ky tu '\n'
        {
            gSynchConsole->Write(buf + i, 1); // Su dung ham Write cua lop SynchConsole
            i++;
        }
        buf[i] = '\n';
        gSynchConsole->Write(buf + i, 1); // Write ky tu '\n'
        machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
        delete buf;
        return;
    }
}
void ExceptionHandlerRead()
{
    // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
    // Output: -1: Loi, So byte read thuc su: Thanh cong, -2: Thanh cong
    // Cong dung: Doc file voi tham so la buffer, so ky tu cho phep va id cua file
    int virtAddr = machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
    int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
    int id = machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
    int OldPos;
    int NewPos;
    char *buf;
    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
    if (id < 0 || id > 14)
    {
        printf("\nKhong the read vi id nam ngoai bang mo ta file.");
        machine->WriteRegister(2, -1);
        return;
    }
    // Kiem tra file co ton tai khong
    if (fileSystem->openf[id] == NULL)
    {
        printf("\nKhong the read vi file nay khong ton tai.");
        machine->WriteRegister(2, -1);
        return;
    }
    if (fileSystem->openf[id]->type == 3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra ve -1
    {
        printf("\nKhong the read file stdout.");
        machine->WriteRegister(2, -1);
        return;
    }
    OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
    buf = User2System(virtAddr, charcount);          // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
    // Xet truong hop doc file stdin (type quy uoc la 2)
    if (fileSystem->openf[id]->type == 2)
    {
        // Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
        int size = gSynchConsole->Read(buf, charcount);
        System2User(virtAddr, size, buf); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
        machine->WriteRegister(2, size);  // Tra ve so byte thuc su doc duoc
        delete buf;
        return;
    }
    // Xet truong hop doc file binh thuong thi tra ve so byte thuc su
    if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
    {
        // So byte thuc su = NewPos - OldPos
        NewPos = fileSystem->openf[id]->GetCurrentPos();
        // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
        System2User(virtAddr, NewPos - OldPos, buf);
        machine->WriteRegister(2, NewPos - OldPos);
    }
    else
    {
        // Truong hop con lai la doc file co noi dung la NULL tra ve -2
        // printf("\nDoc file rong.");
        machine->WriteRegister(2, -2);
    }
    delete buf;
    return;
}
void ExceptionHandlerClose()
{
    // Input id cua file(OpenFileID)
    //  Output: 0: thanh cong, -1 that bai
    int fid = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
    if (fid >= 0 && fid <= 14)          // Chi xu li khi fid nam trong [0, 14]
    {
        if (fileSystem->openf[fid]) // neu mo file thanh cong
        {
            delete fileSystem->openf[fid]; // Xoa vung nho luu tru file
            fileSystem->openf[fid] = NULL; // Gan vung nho NULL
            machine->WriteRegister(2, 0);
            return;
        }
    }
    machine->WriteRegister(2, -1);
    return;
}
void ExceptionHandlerCreateFile()
{
    // Input: Dia chi tu vung nho user cua ten file
    // Output: -1 = Loi, 0 = Thanh cong
    // Chuc nang: Tao ra file voi tham so la ten file
    int virtAddr;
    char *filename;
    DEBUG('a', "\n SC_CreateFile call ...");
    DEBUG('a', "\n Reading virtual address of filename");

    virtAddr = machine->ReadRegister(4); // Doc dia chi cua file tu thanh ghi R4
    DEBUG('a', "\n Reading filename.");

    // Sao chep khong gian bo nho User sang System, voi do dang toi da la (32 + 1) bytes
    filename = User2System(virtAddr, MaxFileLength + 1);
    if (strlen(filename) == 0)
    {
        printf("\n File name is not valid");
        DEBUG('a', "\n File name is not valid");
        machine->WriteRegister(2, -1); // Return -1 vao thanh ghi R2
        return;
    }

    if (filename == NULL) // Neu khong doc duoc
    {
        printf("\n Not enough memory in system");
        DEBUG('a', "\n Not enough memory in system");
        machine->WriteRegister(2, -1); // Return -1 vao thanh ghi R2
        delete filename;
        return;
    }
    DEBUG('a', "\n Finish reading filename.");

    if (!fileSystem->Create(filename, 0)) // Tao file bang ham Create cua fileSystem, tra ve ket qua
    {
        // Tao file that bai
        printf("\n Error create file '%s'", filename);
        machine->WriteRegister(2, -1);
        delete filename;
        return;
    }

    // Tao file thanh cong
    machine->WriteRegister(2, 0);
    delete filename;
    return;
}
void ExceptionHandlerOpen()
{
    // Input: arg1: Dia chi cua chuoi name, arg2: type
    // Output: Tra ve OpenFileID neu thanh, -1 neu loi
    // Chuc nang: Tra ve ID cua file.

    // OpenFileID Open(char *name, int type)
    int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
    int type = machine->ReadRegister(5);     // Lay tham so type tu thanh ghi so 5
    char *filename;
    filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
    // Kiem tra xem OS con mo dc file khong

    int freeSlot = fileSystem->FindFreeSlot();
    if (freeSlot != -1)
    {
        if (type == 0 || type == 1) // type = 0 hoac 1
        {
            if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) // Mo file thanh cong
            {
                machine->WriteRegister(2, freeSlot); // tra ve OpenFileID
            }
        }
        else if (type == 2) // xu li stdin voi type quy uoc la 2
        {
            machine->WriteRegister(2, 0); // tra ve OpenFileID
        }
        else // xu li stdout voi type quy uoc la 3
        {
            machine->WriteRegister(2, 1); // tra ve OpenFileID
        }
        delete[] filename;
        return;
    }
    machine->WriteRegister(2, -1); // Khong mo duoc file return -1

    delete[] filename;
    return;
}
void ExceptionHandlerSeek()
{
    // Input: Vi tri(int), id cua file(OpenFileID)
    // Output: -1: Loi, Vi tri thuc su: Thanh cong
    // Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
    int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
    int id = machine->ReadRegister(5);  // Lay id cua file
    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
    if (id < 0 || id > 14)
    {
        printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
        machine->WriteRegister(2, -1);
        return;
    }
    // Kiem tra file co ton tai khong
    if (fileSystem->openf[id] == NULL)
    {
        printf("\nKhong the seek vi file nay khong ton tai.");
        machine->WriteRegister(2, -1);
        return;
    }
    // Kiem tra co goi Seek tren console khong
    if (id == 0 || id == 1)
    {
        printf("\nKhong the seek tren file console.");
        machine->WriteRegister(2, -1);
        return;
    }
    // Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
    pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
    if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
    {
        printf("\nKhong the seek file den vi tri nay.");
        machine->WriteRegister(2, -1);
    }
    else
    {
        // Neu hop le thi tra ve vi tri di chuyen thuc su trong file
        fileSystem->openf[id]->Seek(pos);
        machine->WriteRegister(2, pos);
    }
    return;
}
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

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    // Purpose: process the event SC_Create of System call
    // mã system call sẽ được đưa vào thanh ghi r2 (có thể xem lại phần xử lý cho
    // system call Halt trong tập tin start.s ở trên)
    // tham số thứ 1 sẽ được đưa vào thanh ghi r4
    // tham số thứ 2 sẽ được đưa vào thanh ghi r5
    // tham số thứ 3 sẽ được đưa vào thanh ghi r6
    // tham số thứ 4 sẽ được đưa vào thanh ghi r7
    // kết quả thực hiện của system call sẽ được đưa vào thanh ghi r2
    switch (which)
    {
    case NoException:
        return;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            ExceptionHandlerHalt();
            break;
        case SC_ReadInt:
            ExceptionHandlerReadInt();
            IncreasePC();
            break;
        case SC_PrintInt:
            ExceptionHandlerPrintInt();
            IncreasePC();
            break;
        case SC_ReadChar:
            ExceptionHandlerReadChar();
            IncreasePC();
            break;
        case SC_PrintChar:
            ExceptionHandlerPrintChar();
            IncreasePC();
            break;
        case SC_ReadString:
            ExceptionHandlerReadString();
            IncreasePC();
            break;
        case SC_PrintString:
            ExceptionHandlerPrintString();
            IncreasePC();
            break;
        case SC_CreateFile:
            ExceptionHandlerCreateFile();
            IncreasePC();
            break;
        case SC_Open:
            ExceptionHandlerOpen();
            IncreasePC();
            break;
        case SC_Close:
            ExceptionHandlerClose();
            IncreasePC();
            break;
        case SC_Read:
            ExceptionHandlerRead();
            IncreasePC();
            break;
        case SC_Write:
            ExceptionHandlerWrite();
            IncreasePC();
            break;
        case SC_Seek:
        {
            // Input: Vi tri(int), id cua file(OpenFileID)
            // Output: -1: Loi, Vi tri thuc su: Thanh cong
            // Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
            int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
            int id = machine->ReadRegister(5);  // Lay id cua file
            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
            if (id < 0 || id > 14)
            {
                printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
                machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            // Kiem tra file co ton tai khong
            if (fileSystem->openf[id] == NULL)
            {
                printf("\nKhong the seek vi file nay khong ton tai.");
                machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            // Kiem tra co goi Seek tren console khong
            if (id == 0 || id == 1)
            {
                printf("\nKhong the seek tren file console.");
                machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            // Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
            pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
            if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
            {
                printf("\nKhong the seek file den vi tri nay.");
                machine->WriteRegister(2, -1);
            }
            else
            {
                // Neu hop le thi tra ve vi tri di chuyen thuc su trong file
                fileSystem->openf[id]->Seek(pos);
                machine->WriteRegister(2, pos);
            }
            IncreasePC();
            return;
        }
        case SC_Exec:
        {
            // Input: vi tri int
            // Output: Fail return -1, Success: return id cua thread dang chay
            // SpaceId Exec(char *name);
            int virtAddr;
            virtAddr = machine->ReadRegister(4); // doc dia chi ten chuong trinh tu thanh ghi r4
            char *name;
            name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel

            if (name == NULL)
            {
                DEBUG('a', "\n Not enough memory in System");
                printf("\n Not enough memory in System");
                machine->WriteRegister(2, -1);
                // IncreasePC();
                return;
            }
            OpenFile *oFile = fileSystem->Open(name);
            if (oFile == NULL)
            {
                printf("\nExec:: Can't open this file.");
                machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }

            delete oFile;

            // Return child process id
            int id = pTab->ExecUpdate(name);
            machine->WriteRegister(2, id);

            delete[] name;
            IncreasePC();
            return;
        }
        case SC_Join:
        {
            // int Join(SpaceId id)
            // Input: id dia chi cua thread
            // Output:
            int id = machine->ReadRegister(4);

            int res = pTab->JoinUpdate(id);

            machine->WriteRegister(2, res);
            IncreasePC();
            return;
        }
        case SC_Exit:
        {
            // void Exit(int status);
            //  Input: status code
            int exitStatus = machine->ReadRegister(4);

            if (exitStatus != 0)
            {
                IncreasePC();
                return;
            }

            int res = pTab->ExitUpdate(exitStatus);
            // machine->WriteRegister(2, res);

            currentThread->FreeSpace();
            currentThread->Finish();
            IncreasePC();
            return;
        }
        case SC_CreateSemaphore:
        {
            // int CreateSemaphore(char* name, int semval).
            int virtAddr = machine->ReadRegister(4);
            int semval = machine->ReadRegister(5);

            char *name = User2System(virtAddr, MaxFileLength + 1);
            if (name == NULL)
            {
                DEBUG('a', "\n Not enough memory in System");
                printf("\n Not enough memory in System");
                machine->WriteRegister(2, -1);
                delete[] name;
                IncreasePC();
                return;
            }

            int res = semTab->Create(name, semval);

            if (res == -1)
            {
                DEBUG('a', "\n Khong the khoi tao semaphore");
                printf("\n Khong the khoi tao semaphore");
                machine->WriteRegister(2, -1);
                delete[] name;
                IncreasePC();
                return;
            }

            delete[] name;
            machine->WriteRegister(2, res);
            IncreasePC();
            return;
        }

        case SC_Wait:
        {
            // int Wait(char* name)
            int virtAddr = machine->ReadRegister(4);

            char *name = User2System(virtAddr, MaxFileLength + 1);
            if (name == NULL)
            {
                DEBUG('a', "\n Not enough memory in System");
                printf("\n Not enough memory in System");
                machine->WriteRegister(2, -1);
                delete[] name;
                IncreasePC();
                return;
            }

            int res = semTab->Wait(name);

            if (res == -1)
            {
                DEBUG('a', "\n Khong ton tai ten semaphore nay!");
                printf("\n Khong ton tai ten semaphore nay!");
                machine->WriteRegister(2, -1);
                delete[] name;
                IncreasePC();
                return;
            }

            delete[] name;
            machine->WriteRegister(2, res);
            IncreasePC();
            return;
        }
        case SC_Signal:
        {
            // int Signal(char* name)
            int virtAddr = machine->ReadRegister(4);

            char *name = User2System(virtAddr, MaxFileLength + 1);
            if (name == NULL)
            {
                DEBUG('a', "\n Not enough memory in System");
                printf("\n Not enough memory in System");
                machine->WriteRegister(2, -1);
                delete[] name;
                IncreasePC();
                return;
            }

            int res = semTab->Signal(name);

            if (res == -1)
            {
                DEBUG('a', "\n Khong ton tai ten semaphore nay!");
                printf("\n Khong ton tai ten semaphore nay!");
                machine->WriteRegister(2, -1);
                delete[] name;
                IncreasePC();
                return;
            }

            delete[] name;
            machine->WriteRegister(2, res);
            IncreasePC();
            return;
        }
        default:
            IncreasePC();
        }
        break;
        /*other exceptions: print a error message and halt the machine*/
    case PageFaultException:
        printf("Unexpected user mode exception PageFaultException\n");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        printf("Unexpected user mode exception ReadOnlyException\n");
        interrupt->Halt();
        break;
    case BusErrorException:
        printf("Unexpected user mode exception BusErrorException\n");
        interrupt->Halt();
        break;
    case AddressErrorException:
        printf("Unexpected user mode exception AddressErrorException\n");
        interrupt->Halt();
        break;
    case OverflowException:
        printf("Unexpected user mode exception OverflowException\n");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        printf("Unexpected user mode exception IllegalInstrException\n");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        printf("Unexpected user mode exception NumExceptionTypes\n");
        interrupt->Halt();
        break;
    default:
        printf("\n Unexpected user mode exception (%d %d)", which, type);
        interrupt->Halt();
    }
}

