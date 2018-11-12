#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <Psapi.h>
#include <Tlhelp32.h>

using namespace std;
HANDLE hData;
HANDLE writing_event;
HANDLE reading_event;

void open_mapped_file() {
	hData = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "data");
	if (hData == NULL) {
		printf("P2: Cannot open file mapping. Error code: %d\n", GetLastError());
		return;
	}
	
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
	if (b == a * 2)
		printf("P2: Perechea %d - Corect\n",i);
	else
		printf("P2: Perechea %d - Incorect\n",i);
}

void openEvents() {
	writing_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, "writing_event");
	if (writing_event == NULL)
		printf("P2: Nu s-a putut deschide eventul de scriere: %d\n", GetLastError());

	reading_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, "reading_event");
	if (reading_event == NULL)
		printf("P2: Nu s-a putut deschide eventul de citire: %d\n", GetLastError());
}

void closeEvents() {
	CloseHandle(writing_event);
	CloseHandle(reading_event);

}

void read_from_mapped_file_event(int i) {
	
	WaitForSingleObject(writing_event, INFINITE);
	int a = readValue();
	
	ResetEvent(writing_event);
	
	if (SetEvent(reading_event) == 0)
		printf("P1: Nu s-a putut seta eventul de citire pentru a: %d\n", GetLastError());


	WaitForSingleObject(writing_event, INFINITE);
	int b = readValue();


	if (SetEvent(reading_event) == 0)
		printf("P1: Nu s-a putut seta eventul de citire pentru b: %d\n", GetLastError());
	
	verification(a, b, i);
}


void use_events() {
	openEvents();
	for (int i = 0; i < 200; i++) 
		read_from_mapped_file_event(i);
	closeEvents();
}

int main()
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	open_mapped_file();
	
	use_events();

	CloseHandle(hData);
	return 0;
}