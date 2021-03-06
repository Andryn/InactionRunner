// InactionRunner.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "InactionRunner.h"

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Please use:\n%s <Elapsed time in sec> <Command 1> [Command 2] [Command N]\n", argv[0]);
		return 0;
	}
	if (argc > MAX_COUNT_COMMAND+2) {
		printf("Very more commands for run %s. Maximum number commands %d.\n", argv[0], MAX_COUNT_COMMAND);
		return 0;
	}

	int i = 0;
	LASTINPUTINFO lpi;
	lpi.cbSize = sizeof(lpi);
	DWORD currentTime;
	DWORD timeElapsed;

	STARTUPINFO cif;
	ZeroMemory(&cif, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi[MAX_COUNT_COMMAND];
	DWORD exitCode = 0;

	DWORD maxTimeElapsed;
	char* cmd[MAX_COUNT_COMMAND];
	//-----------------
	printf("Execute parameters:\n");
	maxTimeElapsed = atoi(argv[1]) * 1000;
	printf("Elapsed time: %d sec\n", maxTimeElapsed / 1000);
	for (i = 0; i < argc-2; i++) {
		cmd[i] = argv[i+2];
		printf("Command %d: %s\n", i+1, cmd[i]);
		ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
	}
	
	printf("Press [Enter] to exit programm.\n");
	printf("Press [Space] to kill all child process.\n");
	Sleep(500);

	while (true) {
		
		if (GetKeyState(VK_RETURN) & (1 << (sizeof(SHORT) * 8 - 1))) { //выход из цикла по Enter
			break;
		}
		if (GetKeyState(VK_SPACE) & (1 << (sizeof(SHORT) * 8 - 1))) { //Прибить все дочерние процессы
			for (i = 0; i < argc - 2; i++) {
				if (pi[i].hProcess != 0 & GetExitCodeProcess(pi[i].hProcess, &exitCode) != FALSE) {
					TerminateProcess(pi[i].hProcess, 0);
				}
			}
		}
		

		GetLastInputInfo(&lpi); //Простой
		currentTime = GetTickCount(); //Текущий таймер
		Sleep(100); //Задержка

		timeElapsed = currentTime - lpi.dwTime; //Время простоя
		if (timeElapsed > maxTimeElapsed) {
			for (i = 0; i < argc - 2; i++) {
				if (pi[i].hProcess == 0 || GetExitCodeProcess(pi[i].hProcess, &exitCode) == FALSE || exitCode != STILL_ACTIVE) {
					printf("Time Elapsed = %d sec\n", timeElapsed/1000);

					printf("Try run %s\n", cmd[i]);
					if (CreateProcess(NULL, CharToLPWSTR(cmd[i]), NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi[i]) == TRUE) {
						printf("Run process %s success, pid=%p\n", cmd[i], pi[i].hProcess);
					}
					else {
						printf("Can't execute %s\n", cmd[i]);
					}
					Sleep(500);
				}
			}

		}
	}
    return 0;
}

LPWSTR CharToLPWSTR(LPCSTR char_string)
{
	LPWSTR res;
	DWORD res_len = MultiByteToWideChar(1251, 0, char_string, -1, NULL, 0);
	res = (LPWSTR)GlobalAlloc(GPTR, (res_len + 1) * sizeof(WCHAR));
	MultiByteToWideChar(1251, 0, char_string, -1, res, res_len);
	return res;
}
