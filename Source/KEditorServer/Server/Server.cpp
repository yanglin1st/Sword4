// Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <signal.h>
#include "KSOEditor.h"
#include "Engine/KMemory.h"
#include "Common/KG_Memory.h"

#if defined( __GNUC__ ) && defined( _DEBUG )
#include "heap-checker.h"
#endif

#ifdef WIN32
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    if (g_pSOEditor)
    {
        KGLogPrintf(KGLOG_INFO, "Receive quit signal !\n");
        g_pSOEditor->Quit();
    }
    return true;
}

void SetConsoleInfo()
{
    HANDLE      hConsole    = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD       BufferSize  = {108, 320};
    SMALL_RECT  WinRect     = {0, 0, BufferSize.X - 1, 27};

    SetConsoleScreenBufferSize(hConsole, BufferSize);
    SetConsoleWindowInfo(hConsole, true, &WinRect);

#ifdef _DEBUG
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
#else
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif

    SetConsoleCtrlHandler(&ConsoleHandlerRoutine, true);
}

#else

void OnQuitSignal(int nSignal)
{
    if (g_pSOEditor)
    {
        KGLogPrintf(KGLOG_INFO, "Receive quit signal !\n");
        g_pSOEditor->Quit();
    }
}

BOOL SetDaemon()
{
    BOOL    bResult   = false;
    int     nRetCode  = 0;
    int     nNullFile = -1;
    rlimit  ResLimit;

    ResLimit.rlim_cur = SHRT_MAX;
    ResLimit.rlim_max = SHRT_MAX;

    nRetCode = setrlimit(RLIMIT_NOFILE, &ResLimit);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = getrlimit(RLIMIT_CORE, &ResLimit);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    ResLimit.rlim_cur = ResLimit.rlim_max;

    nRetCode = setrlimit(RLIMIT_CORE, &ResLimit);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    signal(SIGINT, OnQuitSignal);
    signal(SIGQUIT, OnQuitSignal);
    signal(SIGTERM, OnQuitSignal); 

    nNullFile = open("/dev/null", O_WRONLY);
    KGLOG_PROCESS_ERROR(nNullFile != -1);

    dup2(nNullFile, STDIN_FILENO);
    dup2(nNullFile, STDOUT_FILENO);
    dup2(nNullFile, STDERR_FILENO);

    bResult = true;
Exit0:
    if (nNullFile != -1)
    {
        close(nNullFile);
        nNullFile = -1;
    }
    return bResult;
}

#endif

int main(int argc, char* argv[])
{
    int         nResult             = false;
    int         nRetCode            = false;
    int         nLogInitFlag        = false;
    int         nMemInitFlag        = false;
    BOOL        bMemoryPoolInit     = false;
    BOOL        bHeapCheckInitFlag  = false;
    BOOL        bSOEditorInitFlag   = false;
    KGLOG_PARAM LogParam;
    char        szCWD[MAX_PATH];
    char*       pszRetCWD       = NULL;

#ifdef WIN32    
    SetConsoleInfo();
#endif

#ifdef __GNUC__
    nRetCode = fork();
    if (nRetCode != 0)
    {
        // if fork error(-1), exit !
        // if parent process(!=0), exit !
        return 0;
    }

    setsid();
    //umask(0);
#endif

#if defined( __GNUC__ ) && defined( _DEBUG )
    HeapLeakChecker HeapChecker("HeapChecker");
    bHeapCheckInitFlag = true;
#endif

    tzset(); // initialize time conversion information (timezone)

    pszRetCWD = getcwd(szCWD, sizeof(szCWD));    
    KG_PROCESS_ERROR(pszRetCWD);

    memset(&LogParam, 0, sizeof(LogParam));
    snprintf(LogParam.szPath, sizeof(LogParam.szPath), "%s/logs", szCWD);
    strcpy(LogParam.szIdent, "SOEditorServer");
    LogParam.nMaxLineEachFile = 65536;
    LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;

    nRetCode = KG_MemoryInit(NULL);
    KG_PROCESS_ERROR(nRetCode);
    nMemInitFlag = true;

    nRetCode = KMemory::Initialize(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    bMemoryPoolInit = true;

    KGLogPrintf(KGLOG_INFO, "%s\n", "Editor server for JXOnline III");
    KGLogPrintf(KGLOG_INFO, "%s\n", "Build at " __TIME__ " " __DATE__);
    
#if defined(_WIN64) || defined(__x86_64__)
    KGLogPrintf(KGLOG_INFO, "%s\n", "x64 Version");
#endif

#if defined( __GNUC__ ) && defined( _DEBUG )
	if (bHeapCheckInitFlag && HeapChecker.IsActive())
	{
		KGLogPrintf(KGLOG_INFO, "Perftools heap leak checker is active -- Performance may suffer");
	}
	else
	{
		KGLogPrintf(KGLOG_INFO, "Heap checker is not active, to activate set the HEAPCHECK environment variable.");
	}
#endif
    
    g_pSOEditor = new KSOEditor();
    KGLOG_PROCESS_ERROR(g_pSOEditor);

    nRetCode = g_pSOEditor->Init();
    KGLOG_PROCESS_ERROR(nRetCode);
    bSOEditorInitFlag = true;

    KGLogPrintf(KGLOG_INFO, "Editor server startup ... ... [OK]\n");

#ifdef WIN32
    KGLogPrintf(KGLOG_INFO, "Press [Ctrl + C] to exit !\n");
#endif

#ifdef __GNUC__
    nRetCode = SetDaemon();
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

    g_pSOEditor->Run();

    g_pSOEditor->UnInit();

    KGLogPrintf(KGLOG_INFO, "Game server exit !");

    nResult = true;
Exit0:

#ifdef WIN32
    puts("\nPress any key to exit !");
    getch();
#endif

    if (bSOEditorInitFlag)
    {
        g_pSOEditor->UnInit();
        bSOEditorInitFlag = false;
    }

    if (g_pSOEditor)
    {
        delete g_pSOEditor;
        g_pSOEditor = NULL;
    }

    if (bMemoryPoolInit)
    {
        KMemory::Finalize();
        bMemoryPoolInit = false;
    }

    if (nMemInitFlag)
    {
        KG_MemoryUnInit(NULL);
        nMemInitFlag = false;
    }

    if (nLogInitFlag)
    {
	    KGLogUnInit(NULL);
        nLogInitFlag = false;
    }

#if defined( __GNUC__ ) && defined( _DEBUG )
    if (bHeapCheckInitFlag && !HeapChecker.NoLeaks())
    {
        bHeapCheckInitFlag = false;
        assert(NULL == "Heap Memory Leak");
    }
#endif
    return 0;
}

