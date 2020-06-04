/*В каталоге имеется набор текстовых файлов.Разрабатываемое при -
ложение состоит из двух потоков, которые работают по следующей схеме :
1) первый поток(читатель) асинхронным образом считывает содержи -
мое одного файла;
2) поток - читатель уведомляет второй поток(писатель) о том, что содер -
жимое файла прочитано и может быть передано писателю;
3) поток - писатель получает от первого потока содержимое файла и асин -
хронным образом записывает полученную строку в конец выходного файла;
4) поток - писатель уведомляет читателя о том, что строка записана в вы -
ходной файл и можно приступать к чтению следующего файла;
5) процедура повторяется с п.1, пока не закончится список файлов.
В результате должна быть произведена конкатенация(объединение)
входных текстовых файлов в один результирующий.
Функции чтения - записи должны быть выделены в динамическую библио -
теку, подключены на этапе выполнения программы и выгружены после отра -
ботки основного цикла.*/
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

HANDLE hFileToRead, hEventRead, hEventWrite, hFileToWrite, hMainEvent;//дискрипторы событий
char* buffer;
DWORD nBytesToRead;
HINSTANCE hLib;

DWORD read_thread(LPVOID *) {
	DWORD nBytesRead;
	char fileName[3][20] = { "D:\\1.txt\0", "D:\\2.txt\0", "D:\\3.txt\0" };
	hEventRead = CreateEvent(NULL, TRUE, FALSE, "event1");//создание события (несигнальное с ручным сбросом)
	if (hEventRead == NULL) {
		printf("Error 2:%d", GetLastError());
		system("pause");
		exit(-1);
	}
	char*(*pFunction)(HANDLE, DWORD);
	(FARPROC&)pFunction = GetProcAddress(hLib, "readFile");//адрес функции
	for (int i = 0; i < 3; i++) {
		CloseHandle(hFileToRead);//закрытие дескриптора
		hFileToRead = CreateFile(fileName[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFileToRead == INVALID_HANDLE_VALUE) {
			printf("Error %d", GetLastError());
			system("pause");
			exit(-1);
		}
		nBytesToRead = GetFileSize(hFileToRead, NULL);//размер файла
		buffer = (char*)calloc(nBytesToRead + 1, sizeof(char));
		buffer = pFunction(hFileToRead, nBytesToRead);
		SetEvent(hEventRead);//свободное состояние события
		WaitForSingleObject(hEventWrite, INFINITE);//ожидание события
		ResetEvent(hEventWrite);//занятое состояние
	}
	CloseHandle(hEventRead);//закрытие 
	CloseHandle(hFileToRead);//дескрипторов
	return 0;
}

DWORD write_thread(LPVOID *) {
	DWORD nBytesWrite, Offset = 0;
	if (hFileToWrite == INVALID_HANDLE_VALUE) {
		printf("Error 3:%d", GetLastError());
		system("pause");
		exit(-1);
	}
	DWORD(*pFunction)(HANDLE, DWORD, char*, DWORD);
	(FARPROC&)pFunction = GetProcAddress(hLib, "writeFile");
	hEventWrite = CreateEvent(NULL, TRUE, FALSE, "event2");
	for (int i = 0; i < 3; i++) {
		WaitForSingleObject(hEventRead, INFINITE);
		ResetEvent(hEventRead);
		Offset += pFunction(hFileToWrite, nBytesToRead, buffer, Offset);
		SetEvent(hEventWrite);
	}
	CloseHandle(hFileToWrite);
	SetEvent(hMainEvent);//разблокировка 
	return 0;
}

int main() {
	HANDLE rthread, wthread;
	unsigned long thread_ID;
	hFileToWrite = CreateFile("D:\\result.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	Sleep(2000);
	hMainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hLib = LoadLibrary(TEXT("myDLL.dll"));
	rthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)read_thread, NULL, 0, &thread_ID);//поток чтения
	wthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)write_thread, NULL, 0, &thread_ID);//поток записи
	WaitForSingleObject(hMainEvent, INFINITE);
	CloseHandle(rthread);
	CloseHandle(wthread);
	free(buffer);
	FreeLibrary(hLib);
	system("pause");
	return 0;
}