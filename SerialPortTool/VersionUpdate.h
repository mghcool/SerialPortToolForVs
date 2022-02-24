#pragma once
#pragma comment(lib, "version.lib")

#include <QObject>
#include <windows.h>
#include <tchar.h>

class VersionUpdate
{

public:
	static QString VersionUpdate::GetVersion()
	{

		int nMaxPathName = 4096; // Max length of file name/path
		TCHAR* pBuffer;
		UINT nItemLength;
		void* pData, * lpBuffer;
		QString sVersion;
		DWORD dwInfoSize, dwHandle;
		VS_FIXEDFILEINFO* pFileInfo;

		// Get the file path and name
		pBuffer = new TCHAR[nMaxPathName];
		GetModuleFileName(NULL, pBuffer, nMaxPathName - 1);

		// Get File Version Info size
		dwInfoSize = GetFileVersionInfoSize(pBuffer, &dwHandle);
		if (dwInfoSize > 0)
		{

			pData = new TCHAR[dwInfoSize];
			if (GetFileVersionInfo(pBuffer, dwHandle, dwInfoSize, pData))
				if (VerQueryValue(pData, _T("\\"), &lpBuffer, &nItemLength))
				{

					pFileInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					sVersion = QString("%1.%2.%3.%4")
						.arg(pFileInfo->dwProductVersionMS >> 16)
						.arg(pFileInfo->dwProductVersionMS & 0xFFFF)
						.arg(pFileInfo->dwProductVersionLS >> 16)
						.arg(pFileInfo->dwProductVersionLS & 0xFFFF);
					// Calculate the product version as a number, you can delete the next statement if you don't need it.
					DWORD dwProductVersion = (pFileInfo->dwProductVersionMS >> 16) * 1000 +
						(pFileInfo->dwProductVersionMS & 0xFFFF) * 100 +
						(pFileInfo->dwProductVersionLS >> 16) * 10 +
						(pFileInfo->dwProductVersionLS & 0xFFFF) * 1;

				}
			// Delete the data buffer
			delete[] pData;
		}
		// Get rid of the allocated string buffer
		delete[] pBuffer;
		return sVersion;
	}
private:
};