// WindowsThread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>

/*
HANDLE CreateThread(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	__drv_aliasesMem LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId
	);

void GetLocalTime(
LPSYSTEMTIME lpSystemTime
);

CreateFileW(
_In_ LPCWSTR lpFileName,
_In_ DWORD dwDesiredAccess,
_In_ DWORD dwShareMode,
_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
_In_ DWORD dwCreationDisposition,
_In_ DWORD dwFlagsAndAttributes,
_In_opt_ HANDLE hTemplateFile
);

BOOL WriteFile(
HANDLE       hFile,
LPCVOID      lpBuffer,
DWORD        nNumberOfBytesToWrite,
LPDWORD      lpNumberOfBytesWritten,
LPOVERLAPPED lpOverlapped
);

HANDLE CreateEventW(
_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
_In_ BOOL bManualReset,
_In_ BOOL bInitialState,
_In_opt_ LPCWSTR lpName
);

*/

HANDLE g_Event;

DWORD WINAPI ThreadFunc(_In_ LPVOID lpParameter)
{
	//��ȡϵͳʱ�䲢д���ļ�
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);
	
	// �����ļ�
	HANDLE hFile = CreateFile(TEXT("time.txt"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error!!!\n");
		return -1;
	}

	char buf[64] = {0};
	sprintf(buf, "%04d-%02d-%02d-%02d-%02d-%02d", timeNow.wYear, timeNow.wMonth, timeNow.wDay, timeNow.wHour, timeNow.wMinute, timeNow.wSecond);
	DWORD written = 0;

	// д���ݵ��ļ�
	BOOL bRet = WriteFile(hFile, buf, strlen(buf), &written, NULL);
	if ( !bRet || (written != strlen(buf)) )
	{
		printf("WriteFile error!!!\n");
		CloseHandle(hFile);
		return -1;
	}
	// �ر��ļ����
	CloseHandle(hFile);

	// �����¼�
	SetEvent(g_Event);
}

int _tmain(int argc, _TCHAR* argv[])
{
	// �����¼�
	g_Event = CreateEvent(NULL, true, false, NULL);
	if (g_Event == NULL)
	{
		printf("CreateEvent err!!!\n");
		return -1;
	}

	// �����߳�
	HANDLE hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
	if (hThread == NULL)
	{
		printf("CreateThread err!!!\n");
		return -1;
	}

	// �ȴ��¼�
	if (WaitForSingleObject(g_Event, INFINITE) != WAIT_OBJECT_0)
	{
		printf("WaitForSingleObject err!!!\n");
		return -1;
	}

	char buf[64] = { 0 };
	DWORD readed = 0;

	// ���ļ�
	HANDLE hFile = CreateFile(TEXT("time.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error!!!\n");
		return -1;
	}

	// ���ļ���ȡ����
	BOOL bRet = ReadFile(hFile, buf, sizeof(buf), &readed, NULL);
	if (!bRet)
	{
		printf("ReadFile error!!!\n");
		CloseHandle(hFile);
		return -1;
	}

	// �ر��ļ����
	CloseHandle(hFile);

	std::cout << std::string(buf) << std::endl;

	system("pause");

	return 0;
}

