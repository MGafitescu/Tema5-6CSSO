#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <Psapi.h>
#include <Tlhelp32.h>

using namespace std;
HANDLE hData;

void open_mapped_file() {
	hData = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "data");
	if (hData == NULL) {
		printf("P2: Cannot open file mapping. Error code: %d\n", GetLastError());
		return;
	}
	
}

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

int readValue() {
	int* data;
	data = (int*)MapViewOfFile(hData, FILE_MAP_WRITE, 0, 0, 0);
	if (data == NULL) {
		printf("P2: Cannot get pointer to file mapping. Error code: %d", GetLastError());
		CloseHandle(hData);
		return -1;
	}
	return *data;
}

void verification(int a, int b, int i) {
	printf("P2: a: %d b: %d\n", a, b);
	if (b == a * 2)
		printf("P2: Perechea %d - Corect\n",i);
	else
		printf("P2: Perechea %d - Incorect\n",i);
}

void read_from_mapped_file_event(int i) {
	HANDLE writing_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, "writing_event");
	if (writing_event == NULL)
		printf("P2: Nu s-a putut deschide eventul de scriere: %d\n",GetLastError());
	else
		printf("P2: Eventul de scriere a fost deschis.\n");
	
	HANDLE reading_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, "reading_event");
	if (reading_event == NULL)
		printf("P2: Nu s-a putut deschide eventul de citire: %d\n", GetLastError());
	else
		printf("P2: Eventul de citire a fost deschis.\n");
	
	WaitForSingleObject(writing_event, INFINITE);
	printf("P2: Citesc a\n");
	int a = readValue();
	ResetEvent(writing_event);
	
	if (SetEvent(reading_event) == 0)
		printf("P1: Nu s-a putut seta eventul de citire pentru a: %d\n", GetLastError());
	else
		printf("P1: S-a setat eventul de citire pentru a\n");


	WaitForSingleObject(writing_event, INFINITE);
	printf("P2: Citesc b\n");
	int b = readValue();


	if (SetEvent(reading_event) == 0)
		printf("P1: Nu s-a putut seta eventul de citire pentru b: %d\n", GetLastError());
	else
		printf("P1: S-a setat eventul de citire pentru b\n");
	
	verification(a, b, i);

	CloseHandle(writing_event);
	CloseHandle(reading_event);

}


void use_events() {
	for (int i = 0; i < 200; i++)
		read_from_mapped_file_event(i);
}

int main()
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	open_mapped_file();
	
	use_events();

	CloseHandle(hData);
	return 0;
}