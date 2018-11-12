#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <Psapi.h>
#include <Tlhelp32.h>

using namespace std;

void read_from_mapped_file() {
	HANDLE hData = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "data");
	if (hData == NULL) {
		printf("Cannot open file mapping. Error code: %d", GetLastError());
		return;
	}

	HANDLE hMutex = OpenMutex(SYNCHRONIZE, 1, "mutex_numere");
	WaitForSingleObject(hMutex, INFINITE);
	printf("Citesc a\n");
	int* a = (int*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0);
	if (a == NULL) {
		printf("Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return;
	}
	ReleaseMutex(hMutex);


	WaitForSingleObject(hMutex, INFINITE);
	printf("Citesc b\n");
    int* b = (int*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0);
	if (b == NULL) {
		printf("Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return;
	}
	ReleaseMutex(hMutex);
	printf("a: %d b: %d", *a, *b);
	if (*b == (*a) * 2)
		printf("Corect\n");
	else
		printf("Incorect\n");
	CloseHandle(hMutex);
	CloseHandle(hData);
	
}

int main()
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	read_from_mapped_file();
	return 0;
}