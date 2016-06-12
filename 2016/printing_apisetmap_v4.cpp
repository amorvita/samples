/*--

	Copyright (c) 2016 YoungJin Shin <codewiz@gmail.com>

	http://jiniya.net/ng/2015/12/api-set-dll-name-binding/

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

#include <windows.h>
#include <stdio.h>

#define ADD_PTR(a, b) ((PVOID)((ULONG_PTR)(a) + (ULONG_PTR)(b)))

#pragma pack(1)

typedef struct _API_SET_MAP_DATA_ENTRY
{
	ULONG flags;
	ULONG name_offset;
	ULONG name_length;
	ULONG value_offset;
	ULONG value_length;
} API_SET_MAP_DATA_ENTRY, *PAPI_SET_MAP_DATA_ENTRY;


typedef struct _API_SET_MAP_DATA
{
	ULONG flags;
	ULONG count;
	API_SET_MAP_DATA_ENTRY entries[ANYSIZE_ARRAY];
} API_SET_MAP_DATA, *PAPI_SET_MAP_DATA;


typedef struct _API_SET_MAP_ENTRY
{
	ULONG flags;
	ULONG name_offset;
	ULONG name_length;
	ULONG alias_offset;
	ULONG alias_length;
	ULONG data_offset;
} API_SET_MAP_ENTRY, *PAPI_SET_MAP_ENTRY;



typedef struct _API_SET_MAP
{
	ULONG version;
	ULONG size;
	ULONG flags;
	ULONG count;
	API_SET_MAP_ENTRY entries[ANYSIZE_ARRAY];
} API_SET_MAP, *PAPI_SET_MAP;


#pragma pack()

int main()
{
	PVOID name_ptr;
	ULONG_PTR api_set_map;

	PAPI_SET_MAP pasm;
	PAPI_SET_MAP_DATA pasmd;

	WCHAR name[MAX_PATH];

	api_set_map = __readfsdword(0x30);
	api_set_map = *(ULONG_PTR *)(api_set_map + 0x38);


	pasm = (PAPI_SET_MAP) api_set_map;

	for(ULONG i = 0; i < pasm->count; ++i)
	{
		name_ptr = ADD_PTR(pasm, pasm->entries[i].name_offset);

		memset(name, 0, sizeof(name));
		memcpy(name, name_ptr, pasm->entries[i].name_length);

		pasmd = (PAPI_SET_MAP_DATA) ADD_PTR(pasm, pasm->entries[i].data_offset);

		printf("%ls => [%d]", name, pasmd->count);

		for(ULONG j = 0; j < pasmd->count; ++j)
		{
			name_ptr = ADD_PTR(pasm,  pasmd->entries[j].value_offset);

			memset(name, 0, sizeof(name));
			memcpy(name, name_ptr, pasmd->entries[j].value_length);

			printf(" %ls", name);
		}

		printf("\n");

	}

	return 0;
}
