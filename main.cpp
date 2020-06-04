#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <process.h>
#include <string.h>


__declspec(dllexport)
char* readFile(HANDLE hFileToRead, DWORD nBytesToR)
{
	DWORD nBytesRead, nBytesToRead;
	HANDLE hEventRead = CreateEvent(NULL, TRUE, FALSE, TEXT("event1"));
	char* buffer;
	nBytesToRead = GetFileSize(hFileToRead, NULL);
	buffer = (char*)calloc(nBytesToRead + 1, sizeof(char));
	OVERLAPPED rOverLapped;//информация о асинхронных входах и выходах
	rOverLapped.Offset = 0;
	rOverLapped.OffsetHigh = 0;
	rOverLapped.hEvent = hEventRead;
	bool bResult = ReadFile(hFileToRead, buffer, nBytesToRead, &nBytesRead, &rOverLapped);
	WaitForSingleObject(hEventRead, INFINITE);
	CloseHandle(hEventRead);
	return buffer;
}

__declspec(dllexport)
DWORD writeFile(HANDLE hFileToWrite, DWORD nBytesToW, char* buffer, DWORD Offset)
{
	DWORD nBytesWrite;
	HANDLE hEventWrite = CreateEvent(NULL, TRUE, FALSE, TEXT("event2"));
	OVERLAPPED wOverLapped;
	wOverLapped.Offset = Offset;//слова смещения
	wOverLapped.OffsetHigh = 0;
	wOverLapped.hEvent = hEventWrite;
	bool bResult = WriteFile(hFileToWrite, buffer, nBytesToW, &nBytesWrite, &wOverLapped);
	WaitForSingleObject(hEventWrite, INFINITE);
	CloseHandle(hEventWrite);
	return nBytesWrite;
}