#include <Windows.h>
#include <iostream>

using namespace std;

#define BUF_SIZE sizeof(int)

int main() {
    DWORD pid = GetCurrentProcessId();
    HANDLE hMapFile;
    HANDLE hSemaphore;
    LPVOID pBuf;

    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("SharedMemory"));
    if (hMapFile == NULL) {
        cout << "OpenFileMapping failed" << endl;
        return 1;
    }

    pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
    if (pBuf == NULL) {
        cout << "MapViewOfFile failed" << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("SyncSemaphore"));
    if (hSemaphore == NULL) {
        cout << "OpenSemaphore failed" << endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 1;
    }

    int* sharedNum = (int*)pBuf;
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