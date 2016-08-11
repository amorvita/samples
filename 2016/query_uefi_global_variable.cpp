/*--

	Copyright (c) 2016 YoungJin Shin <codewiz@gmail.com>

	http://www.uefi.org/specifications

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

--*/

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>
#include <memory>

BOOL
SetPrivilege(HANDLE process, LPCWSTR name, BOOL on)
{
	HANDLE token;
	if(!OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES, &token))
		return FALSE;

	std::shared_ptr<void> token_closer(token, ::CloseHandle);
	
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	if(!LookupPrivilegeValueW(NULL, name, &tp.Privileges[0].Luid))
		return FALSE;

	tp.Privileges[0].Attributes = on ? SE_PRIVILEGE_ENABLED : 0;
	return AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL);
}

decltype(&GetFirmwareEnvironmentVariableA)
GetAddressOfGetFirmwareEnvironmentVariableA()
{
	auto kernel32 = GetModuleHandleW(L"kernel32.dll");
	if(!kernel32)
		return NULL;

	auto fn = reinterpret_cast
				<decltype(&GetFirmwareEnvironmentVariableA)>
					(GetProcAddress(kernel32, "GetFirmwareEnvironmentVariableA"));
	if(!fn)
		return NULL;

	return fn;
}

BOOL
IsUefiBIOS()
{
	auto fn = GetAddressOfGetFirmwareEnvironmentVariableA();

	static const LPCSTR NULL_GUID = "{00000000-0000-0000-0000-000000000000}";

	if(!fn("", NULL_GUID, NULL, 0))
	{
		DWORD ge = GetLastError();

		switch(ge)
		{
		case ERROR_INVALID_FUNCTION:
			return FALSE;

		case ERROR_NOACCESS:
			return TRUE;

		default:
			break;
		}
	}

	return FALSE;
}

BOOL
GetUefiGlobalVariable(LPCSTR name, LPVOID buffer, DWORD buffer_size)
{
	auto fn = GetAddressOfGetFirmwareEnvironmentVariableA();
	if(!fn)
		return FALSE;

	static const LPCSTR EFI_GLOBAL_VARIABLE_GUID = "{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}";

	return fn(name, EFI_GLOBAL_VARIABLE_GUID, buffer, buffer_size);
}

int main()
{
	SetPrivilege(GetCurrentProcess(), SE_SYSTEM_ENVIRONMENT_NAME, TRUE);

	if(!IsUefiBIOS())
	{
		printf("no uefi bios\n");
		return 0;
	}

	UCHAR setup_mode;
	if(!GetUefiGlobalVariable("SetupMode", &setup_mode, sizeof(setup_mode)))
	{
		printf("query fail %d\n", GetLastError());
		return 0;
	}

	printf("SetupMode = %d", setup_mode);
	return 0;
}

