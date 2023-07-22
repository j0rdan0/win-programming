

#include <stdio.h>
#include <windows.h>
#include "../kernel-testing/test.h"

int main(int argc, char*argv[])
{

    printf("from client\n");

    struct Message mes;
        mes.text = (char*)malloc(sizeof("debug") + 1);
        strcpy(mes.text, argv[1]);
   
    HANDLE hdevice = CreateFile(L"\\\\.\\TEST_DEV", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hdevice == INVALID_HANDLE_VALUE) {
        printf("failed opening device, err :%u\n",GetLastError());
    }
    DWORD ret;

    BOOL stat = WriteFile(hdevice, &mes, sizeof(mes), &ret, NULL);
    if (!stat) {
        printf("failed sending message to driver\n");
    }
    else {
        printf("sent message to driver\n");
    }
    CloseHandle(hdevice);

    return 0;
}


