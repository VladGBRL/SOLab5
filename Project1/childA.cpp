#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define BUF_SIZE sizeof(int)

int main() {
    DWORD pid = GetCurrentProcessId();
    HANDLE hMapFile;
    HANDLE hSemaphore;
    LPVOID pBuf;

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, TEXT("SharedMemory"));
    if (hMapFile == NULL) {
        cout << "CreateFileMapping failed" << endl;
        return 1;
    }

    pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
    if (pBuf == NULL) {
        cout << "MapViewOfFile failed" << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    hSemaphore = CreateSemaphore(NULL, 1, 1, TEXT("SyncSemaphore"));
    if (hSemaphore == NULL) {
        cout << "CreateSemaphore failed" << endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 1;
    }

    int* sharedNum = (int*)pBuf;
    *sharedNum = 1;

    srand((unsigned)time(NULL));
    while (*sharedNum <= 1000) {

        WaitForSingleObject(hSemaphore, INFINITE);

        int coinFlip = rand() % 2;
        if (coinFlip == 1) {
            (*sharedNum)++;
            cout << "Proces " << pid << ": Scris numarul " << *sharedNum << " in memorie." << endl;
        }

        ReleaseSemaphore(hSemaphore, 1, NULL);
        Sleep(100);
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);

    return 0;
}