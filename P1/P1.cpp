#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <Psapi.h>
#include <Tlhelp32.h>

using namespace std;
HANDLE hData;
HANDLE writing_event;
HANDLE reading_event;
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

void createEvents() {
	writing_event = CreateEvent(NULL, FALSE, FALSE, "writing_event");
	if (writing_event == NULL)
		printf("P1: Nu s-a putut crea event de scriere: %d.\n", GetLastError());

	reading_event = CreateEvent(NULL, FALSE, FALSE, "reading_event");
	if (reading_event == NULL)
		printf("P1: Nu s-a putut crea event de citire: %d.\n", GetLastError());
}


void closeEvents() {
	CloseHandle(writing_event);
	CloseHandle(reading_event);
}

void write_to_mapped_file_event(int a, int b) {
	memcpy(pData, &a, sizeof(int));
	if (SetEvent(writing_event) == 0)
		printf("P1: Nu s-a putut seta eventul de scriere pentru a: %d\n", GetLastError());

	WaitForSingleObject(reading_event, INFINITE);
	
	memcpy(pData, &b, sizeof(int));
	if (SetEvent(writing_event) == 0)
		printf("P1: Nu s-a putut seta eventul de scriere pentru b: %d\n", GetLastError());
	
	ResetEvent(reading_event);

	WaitForSingleObject(reading_event, INFINITE);


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

void create_random_numbers(int i)
{ 
	int a = rand();
	int b = 2 * a;
	printf("P1: Perechea %d a: %d b: %d\n",i, a, b);
	write_to_mapped_file_event(a,b);
}


void use_events() {
	srand(time(NULL));
	createEvents();
	for (int i = 0; i < 200; i++) 
		create_random_numbers(i);
	closeEvents();

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