#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <Psapi.h>
#include <Tlhelp32.h>

using namespace std;
HANDLE hData;
unsigned char* pData;


int create_mapped_file() {
	hData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024 * 1024, "data");
	if (hData == NULL) {
		printf("Cannot create file mapping. Error code: %d\n", GetLastError());
		return 0;
	}

    pData = (unsigned char*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0);
	if (pData == NULL) {
		printf("Cannot get pointer to file mapping. Error code: %d\n", GetLastError());
		CloseHandle(hData);
		return 0;
	}

}

void write_to_mapped_file(int a, int b) {
	
	HANDLE hMutex = CreateMutex(NULL, TRUE, "mutex_numere");
	printf("Scriu a\n");
	memcpy(pData, &a, sizeof(int));
	ReleaseMutex(hMutex);

	WaitForSingleObject(hMutex, INFINITE);
	printf("Scriu b\n");
	memcpy(pData, &b, sizeof(int));
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	
}

void start_process(char *process_name)
{
	bool created;
	PROCESS_INFORMATION infos;
	STARTUPINFO start;
	ZeroMemory(&start, sizeof(start));
	start.cb = sizeof(start);
	ZeroMemory(&infos, sizeof(infos));

	created = CreateProcess(NULL, process_name, NULL, NULL, FALSE, 0, NULL, NULL, &start, &infos);
	if (created == 0)
	{
		printf("Nu s-a putut crea procesul %s. Eroare: %d.\n", process_name, GetLastError());
		return;
	}
	printf("Procesul %s cu pid-ul %d si thread-ul principal %d a fost creat cu succes.\n", process_name, infos.dwProcessId, infos.dwThreadId);
	CloseHandle(infos.hProcess);
	CloseHandle(infos.hThread);
}

void create_random_numbers()
{
	srand(time(NULL));   // Initialization, should only be called once.
	int a = rand();
	int b = 2 * a;
	printf("a: %d b: %d\n", a, b);
	write_to_mapped_file(a,b);
}



int main()
{
	create_mapped_file();
	start_process("\"C:\\Users\\Marian Gafitescu\\Desktop\\CSSO\\Tema5-6CSSO\\P2\\Debug\\P2.exe\"");
	create_random_numbers();
	getchar();
	return 0;
}