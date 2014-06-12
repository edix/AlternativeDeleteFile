#include <windows.h>
#include <iostream>

//04913   FileDirectoryInformation = 1,
//04914   FileFullDirectoryInformation,
//04915   FileBothDirectoryInformation,
//04916   FileBasicInformation,
//04917   FileStandardInformation,
//04918   FileInternalInformation,
//04919   FileEaInformation,
//04920   FileAccessInformation,
//04921   FileNameInformation,
//04922   FileRenameInformation,
//04923   FileLinkInformation,
//04924   FileNamesInformation,
//04925   FileDispositionInformation,      13
//04926   FilePositionInformation,

#define FileDispositionInformation 13

typedef LONG (WINAPI *fNtSetInformationFile)( HANDLE FileHandle, PVOID IoStatusBlock, PVOID FileInformation, ULONG Length, DWORD FileInformationClass );
typedef LONG (WINAPI *fNtGetInformationFile)( HANDLE FileHandle, PVOID IoStatusBlock, PVOID FileInformation, ULONG Length, DWORD FileInformationClass );

struct FILE_DISPOSITION_INFORMATION
{
	BOOLEAN fDeleteFile;
};

BOOL MyDeleteFile(LPCWSTR lpFileName)
{
	HANDLE hFile;
	BOOL fResult = FALSE;
	FILE_DISPOSITION_INFORMATION FileInformation = { 0 };
	LPBYTE lpIoStatusBlock[255] = { 0 };
	fNtSetInformationFile NtSetInformationFile = NULL; 

	//
	// open file for delete
	//
	hFile = CreateFileW(lpFileName, DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		// 
		// get NtSetInformationFile
		//
		NtSetInformationFile = (fNtSetInformationFile)GetProcAddress( GetModuleHandle( L"ntdll" ), "NtSetInformationFile" );
		if (NtSetInformationFile)
		{
			//
			// set deletefile member in the file disposition information structure to TRUE
			// 
			FileInformation.fDeleteFile = TRUE;

			if (NtSetInformationFile(hFile, &lpIoStatusBlock, &FileInformation, sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation) >= 0)
			{
				//
				// file deleted after last CloseHandle
				//
				fResult = TRUE;
			}
		}
		CloseHandle(hFile);
	}
	return fResult;
}

int main(int argc, char* argv[])
{
	if (MyDeleteFile(L"d:\\test.txt"))
	{
		printf( "file deleted\n" );
	}
	else
	{
		printf( "can't delete file\n");
	}
}
