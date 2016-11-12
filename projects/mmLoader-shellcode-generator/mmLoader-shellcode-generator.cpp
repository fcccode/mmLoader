// mmLoader-shellcode-generator.cpp : Defines the entry point for the console application.
//
// 
#include "stdafx.h"
#include <windows.h>
#include "..\src\mmLoader\mmLoader.h"
#include <Strsafe.h>
#include <shlwapi.h>

#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/ENTRY:CodeBag")

// Set in the project configuration
//#ifdef _WIN64
//#pragma comment(linker, "/ORDER:functionorder_x64")
//#else
//#pragma comment(linker, "/ORDER:functionorder_x86")
//#endif

LPSTR head =
"/************************************************************************/\r\n"
"/* \r\n"
#ifdef _WIN64
" * This file is generated by mmLoaderShellCode Generator (x64)\r\n"
" * Target platform: 64 bit windows application\r\n"
#else
" * This file is generated by mmLoaderShellCode Generator (x86)\r\n"
" * Target platform: 32 bit windows application\r\n"
#endif
" *\r\n"
" * https://github.com/tishion \r\n"
" *\r\n"
"/************************************************************************/\r\n"
"//\r\n"
"#ifndef __MMLOADER_H_INCLUDED_\r\n"
"#define __MMLOADER_H_INCLUDED_\r\n"
"#pragma once\r\n"
"#include <windows.h>\r\n"
"\r\n"
"/// <summary>\r\n"
"/// Error codes.\r\n"
"/// </summary>\r\n"
"#define MMEC_OK							0\r\n"
"#define MMEC_BAD_PE_FORMAT				1\r\n"
"#define MMEC_ALLOCATED_MEMORY_FAILED	2\r\n"
"#define MMEC_INVALID_RELOCATION_BASE	3\r\n"
"#define MMEC_IMPORT_MODULE_FAILED		4\r\n"
"#define MMEC_PROTECT_SECTION_FAILED		5\r\n"
"#define MMEC_INVALID_ENTRY_POINT		6\r\n"
"\r\n"
"/// <summary>\r\n"
"/// Function table. These function will be used in the mmLoader.\r\n"
"/// </summary>\r\n"
"typedef struct __NTFUNCPTRS\r\n"
"{\r\n"
"	LPVOID pfnGetModuleHandleA;		// GetModuleHandleA\r\n"
"	LPVOID pfnLoadLibraryA;			// LoadLibraryA\r\n"
"	LPVOID pfnGetProcAddress;		// GetProcAddress\r\n"
"	LPVOID pfnVirtualAlloc;			// VirtualAlloc\r\n"
"	LPVOID pfnVirtualFree;			// VirtualFree\r\n"
"	LPVOID pfnVirtualProtect;		// VirtualProtect\r\n"
"	LPVOID pfnReversed_0;\r\n"
"	LPVOID pfnReversed_1;\r\n"
"	LPVOID pfnReversed_2;\r\n"
"	LPVOID pfnReversed_3;\r\n"
"	LPVOID pfnReversed_4;\r\n"
"} NTFUNCPTRSTABLE, *PNTFUNCPTRSTABLE;\r\n"
"\r\n"
"/// <summary>\r\n"
"/// Represents the memory module instance.\r\n"
"/// </summary>\r\n"
"typedef struct __MEMMODULE\r\n"
"{\r\n"
"	union								// MemModule base\r\n"
"	{\r\n"
"#if _WIN64\r\n"
"		ULONGLONG	iBase;\r\n"
"#else\r\n"
"		DWORD		iBase;\r\n"
"#endif\r\n"
"		HMODULE		hModule;\r\n"
"		LPVOID		lpBase;\r\n"
"		PIMAGE_DOS_HEADER pImageDosHeader;\r\n"
"	};\r\n"
"\r\n"
"	DWORD dwSizeOfImage;				// MemModule size\r\n"
"	DWORD dwCrc;						// MemModule crc32\r\n"
"	DWORD dwPageSize;					// SystemPageSize\r\n"
"	BOOL  bLoadOk;						// MemModule is loaded ok?\r\n"
"\r\n"
"	PNTFUNCPTRSTABLE pNtFuncptrsTable;	// Pointer to NT function pointers table \r\n"
"\r\n"
"	DWORD  dwErrorCode;				// Last error code\r\n"
"\r\n"
"	__MEMMODULE()\r\n"
"	{\r\n"
"		iBase = 0;\r\n"
"		dwSizeOfImage = 0;\r\n"
"		dwCrc = 0;\r\n"
"		bLoadOk = 0;\r\n"
"		pNtFuncptrsTable = 0;\r\n"
"		dwErrorCode = 0;\r\n"
"		\r\n"
"		SYSTEM_INFO sysInfo;\r\n"
"		::GetNativeSystemInfo(&sysInfo);\r\n"
"		dwPageSize = sysInfo.dwPageSize;\r\n"
"	}\r\n"
"} MEM_MODULE, *PMEM_MODULE;\r\n"
"\r\n"
"/// <summary>\r\n"
"/// Enums for MemModuleHelper.\r\n"
"/// </summary>\r\n"
"typedef enum _MMHELPER_METHOD\r\n"
"{\r\n"
"	MHM_BOOL_LOAD,\r\n"
"	MHM_VOID_FREE,\r\n"
"	MHM_FARPROC_GETPROC,\r\n"
"} MMHELPER_METHOD;\r\n"
"\r\n"
"/// <summary>\r\n"
"/// Type of the MemModuleHlper function.\r\n"
"/// </summary>\r\n"
"typedef LPVOID(__stdcall * Type_MemModuleHelper)(PMEM_MODULE, MMHELPER_METHOD, LPVOID, LPCSTR, BOOL);\r\n"
"\r\n"
"/************************************************************************\\\r\n"
" *\r\n"
" * Auxiliary Function:\r\n"
" *		use the mmLoader through this function after it is loaded from shell code.\r\n"
" *\r\n"
" * Parameters:\r\n"
" *		pMmeModule:\r\n"
" *\r\n"
" *		method:\r\n"
" *			Function to be used\r\n"
" *\r\n"
" *		lpPeModuleBuffer:\r\n"
" *			the raw data buffer of the pe module to be loaded, only valid when method == MHM_BOOL_LOAD\r\n"
" *			\r\n"
" *		lpProcName:\r\n"
" *			name of the proc to be retrieved, only valid when MHM_FARPROC_GETPROC\r\n"
" *			\r\n"
" *		bCallEntry:\r\n"
" *			need to call the module entry point?\r\n"
" *\r\n"
" *	return value:\r\n"
" *		when method == MHM_BOOL_LOAD\r\n"
" *			return the resulT of loading, TRUE or FALSE\r\n"
" *\r\n"
" *		when method MHM_VOID_FREE:\r\n"
" *			no return value\r\n"
" *\r\n"
" *		when method == MHM_FARPROC_GETPROC\r\n"
" *			return the address of the target proc, return NULL when failed to get the address\r\n"
" *\r\n"
" *\r\n"
" *\r\n"
"\\************************************************************************/\r\n"
"\r\n"
"/// <summary>\r\n"
"/// The byte array of the mmLoader shell code.\r\n"
"/// </summary>\r\n"
"unsigned char mmLoaderShellCode[] =\r\n"
"{\r\n";

LPSTR tail =
"\r\n};\r\n"
"\r\n"
"#endif // __MMLOADERSHELLCODE_H_INCLUDED_";

/// <summary>
/// 
/// </summary>
void InitializeConsole()
{
	// Create a console
	if (!::AllocConsole())
		::MessageBox(NULL, _T("Failed to allocate console."), _T("Warning"), MB_OK);
}

/// <summary>
/// 
/// </summary>
/// <param name="message"></param>
void ConsoleWrite(LPCSTR message)
{
	if (!message)
		return;

	DWORD bytesWritten = 0;
	HANDLE h = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (!h || !::WriteConsoleA(h, message, lstrlenA(message), &bytesWritten, NULL))
		::MessageBoxA(NULL, message, "Warning", MB_OK);
}

/// 
/// </summary>
/// <param name="message"></param>
void ConsoleWriteW(LPCWSTR message)
{
	if (!message)
		return;

	DWORD bytesWritten = 0;
	HANDLE h = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (!h || !::WriteConsoleW(h, message, lstrlenW(message), &bytesWritten, NULL))
		::MessageBoxW(NULL, message, L"Warning", MB_OK);
}

#ifdef _WIN64
#	ifdef _DEBUG
#		define SHELLCODE_HEADER_FILE_NAME L"mmLoaderShellCode-x64-Debug.h"
#	else
#		define SHELLCODE_HEADER_FILE_NAME L"mmLoaderShellCode-x64-Release.h"
#	endif
#else
#	ifdef _DEBUG
#		define SHELLCODE_HEADER_FILE_NAME L"mmLoaderShellCode-x86-Debug.h"
#	else
#		define SHELLCODE_HEADER_FILE_NAME L"mmLoaderShellCode-x86-Release.h"
#	endif
#endif

/// <summary>
/// 
/// </summary>
#define mml_ZeroMemory(p, len) for (int i=0; i < len; i++) *(unsigned char*)p = 0

/// <summary>
/// 
/// </summary>
#define mml_hextoascii(hex) 

/// <summary>
/// Frees the memory module.
/// </summary>
extern void mmLoaderCodeEnd();

/// <summary>
/// main function.
/// </summary>
/// <returns></returns>
int CodeBag()
{
	// Initialize the console
	InitializeConsole();

	LPCWSTR pFolderPath = SHELLCODE_HEADER_FILE_NAME;
	WCHAR pathBuffer[MAX_PATH];
	for (int i = 0; i < MAX_PATH; i++) pathBuffer[i] = 0;

	// Get command line
	LPCWSTR pCmd = ::GetCommandLineW();

	// Parse command line
	int nCount = 0;
	LPWSTR* pArgs = NULL;
	if (pCmd)
		pArgs = ::CommandLineToArgvW(pCmd, &nCount);
	if (nCount >= 2)
	{
		if (::GetFullPathNameW(pArgs[1], MAX_PATH, pathBuffer, NULL) <= 0)
		{
			ConsoleWrite("Failed to get the full path of the folder: ");
			ConsoleWriteW(pArgs[2]);
			ConsoleWrite("\r\n");
			return -1;
		}

		if (!::PathFileExistsW(pathBuffer))
		{
			ConsoleWrite("Path does not exist: ");
			ConsoleWriteW(pathBuffer);
			ConsoleWrite("\r\n");
			return -1;
		}

		if (::PathCombineW(pathBuffer, pathBuffer, SHELLCODE_HEADER_FILE_NAME) <= 0)
		{
			ConsoleWrite("Failed to build file path.\r\n");
			return -1;
		}

		pFolderPath = pathBuffer;
	}

	// Get code start and end address
	unsigned char* pStart = (unsigned char*)&MemModuleHelper;
	unsigned char* pEnd = (unsigned char*)&mmLoaderCodeEnd;

	// Get code length
	ULONGLONG codeLength = (pEnd - pStart);

	// Get the buffer length
	size_t textLength = 512 * 1024;

	// Allocate the heap buffer for the file content
	ConsoleWrite("Allocating buffer..\r\n");
	LPSTR pBuffer = (LPSTR)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, textLength);
	if (!pBuffer)
	{
		ConsoleWrite("Failed to allocate memory buffer.\r\n");
		return -1;
	}

	// Start to build the file content
	ConsoleWrite("Building the file content..\r\n");\
	::StringCchCatA(pBuffer, textLength, head);
	unsigned char charTable[] = { "0123456789ABCDEF" };
	unsigned char buf[3] = { 0, 0, 0 };
	int n = 0;
	for (unsigned char* p = pStart; p < pEnd; p++)
	{
		// Start one row
		if (0 == n++) ::StringCchCatA(pBuffer, textLength, "\t");
		{
			// Hex to string
			::StringCchCatA(pBuffer, textLength, "0x");
			buf[0] = charTable[*p >> 4];
			buf[1] = charTable[*p & 0x0f];
			::StringCchCatA(pBuffer, textLength, (char*)buf);
			::StringCchCatA(pBuffer, textLength, ", ");
		}
		// End one row
		if (n == 16 && !(n = 0)) ::StringCchCatA(pBuffer, textLength, "\r\n");
	}
	::StringCchCatA(pBuffer, textLength, tail);

	ConsoleWrite("File content build done:\r\n\r\n");
	ConsoleWrite(pBuffer);

	// Get the valid string length
	::StringCchLengthA(pBuffer, textLength, &textLength);

	ConsoleWrite("Create file mmLoaderShellCode.h\r\n");
	// Create file to save the content
	HANDLE h = 	::CreateFileW(pFolderPath, FILE_WRITE_ACCESS, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	DWORD dwBytesWritten = 0;
	if (INVALID_HANDLE_VALUE == h || NULL == h)
	{
		ConsoleWrite("Failed to create file \"mmLoaderShellCode\".\r\n");
		return -1;
	}
	else
	{
		// Write the string content to the disk file
		if (!::WriteFile(h, pBuffer, (DWORD)textLength, &dwBytesWritten, NULL))
		{
			ConsoleWrite("Failed to write content to file \"mmLoaderShellCode\".\r\n");
			return -1;
		}

		::FlushFileBuffers(h);
		::CloseHandle(h);
	}

	// Free the content buffer
	::HeapFree(::GetProcessHeap(), NULL, pBuffer);

	// Wait for the return key
	ConsoleWrite("\r\nShell code generated done.\r\n");
	//::WaitForSingleObject(::GetCurrentProcess(), INFINITE);

	::ExitProcess(0);
}