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

void write_to_mapped_file_event(int a, int b) {
	HANDLE writing_event = CreateEvent(NULL, FALSE, FALSE, "writing_event");
	if (writing_event == NULL)
		printf("P1: Nu s-a putut crea event de scriere: %d.\n", GetLastError());
	else
		printf("P1: Eventul de scriere a fost creat.\n");

	HANDLE reading_event = CreateEvent(NULL, FALSE, FALSE, "reading_event");
	if (reading_event == NULL)
		printf("P1: Nu s-a putut crea event de citire: %d.\n", GetLastError());
	else
		printf("P1: Eventul de citire a fost creat.\n");
	
	printf("P1: Scriu a\n");
	memcpy(pData, &a, sizeof(int));
	if (SetEvent(writing_event) == 0)
		printf("P1: Nu s-a putut seta eventul de scriere pentru a: %d\n", GetLastError());
	else
		printf("P1: S-a setat eventul de scriere pentru a\n");

	WaitForSingleObject(reading_event, INFINITE);
	printf("P1: Scriu b\n");
	memcpy(pData, &b, sizeof(int));
	if (SetEvent(writing_event) == 0)
		printf("P1: Nu s-a putut seta eventul de scriere pentru b: %d\n", GetLastError());
	else
		printf("P1: S-a setat eventul de scriere pentru b\n");
	ResetEvent(reading_event);

	WaitForSingleObject(reading_event, INFINITE);

	CloseHandle(writing_event);
	CloseHandle(reading_event);

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
	printf("P1: a: %d b: %d\n", a, b);
	write_to_mapped_file_event(a,b);
}


void use_events() {
	for (int i = 0; i < 200; i++)
		create_random_numbers();
}

int main()
{
	create_mapped_file();
	start_process("\"C:\\Users\\Marian Gafitescu\\Desktop\\CSSO\\Tema5-6CSSO\\P2\\Debug\\P2.exe\"");
	
	use_events();

	CloseHandle(hData);
	getchar();

	return 0;
}