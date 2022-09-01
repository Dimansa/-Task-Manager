#include<iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

#ifdef UNICODE
#define tcout wcout
#elif
#define tcout cout
#endif 

using namespace std;

class SmartHandle
{
public:

	SmartHandle(HANDLE handle)
	{
		_handle = handle;
	}

	~SmartHandle()
	{
		if (_handle)
		{
			CloseHandle(_handle);
		}
	}

	operator HANDLE()
	{
		return _handle;
	}

	HANDLE handle()
	{
		return _handle;
	}

private:
	HANDLE _handle = NULL;
};

int Finished()
{
	DWORD processid, fdwExit;
	int q = 0;
	do {
		cout << "Введите id процесса (указан под каждым процессом) или введите 0 если хотите закончить " << endl;
		if (cout.fail() == true)
			q = 1;
		while (!(cin >> processid))
		{
			cout << "Введено некорректное значение" << endl;
			if (cout.fail() == true)
				q = 2;
			cin.clear();
			cin.ignore(32767, '\n');
		}
		cin.ignore(32767, '\n');
		if (processid != 0)
		{
			SmartHandle hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_VM_READ, FALSE, processid);
			if (hProcess == NULL)
				q = 3;
			GetExitCodeProcess(hProcess, &fdwExit);
			if (fdwExit == 0)
				q = 4;
			if (TerminateProcess(hProcess, fdwExit) == 0)
				q = 5;
		}
	} while (processid != 0);
	return q;
}

int main()
{
	int q = 0;
	if (setlocale(LC_ALL, "Russian") == NULL)
		q = 1;
	SmartHandle processSnap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
	if (!processSnap)
	{
		q = 2;
	}
	vector<PROCESSENTRY32> processInfos;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(processSnap, &pe) == FALSE)
	{
		q = 3;
	}
	do
	{
		processInfos.push_back(pe);
	} while (Process32Next(processSnap, &pe));

	for (const auto &pe : processInfos)
	{
		tcout << pe.szExeFile << endl << pe.th32ProcessID << endl;
		if (tcout.fail() == true)
			q = 4;
	}
	q = Finished();
	return q;
}
