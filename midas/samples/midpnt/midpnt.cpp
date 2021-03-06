/*       MidpNT.cpp
 *
 * MIDAS Module Player for Windows NT main program file
 *
 * $Id: midpnt.cpp,v 1.27 1997/07/31 10:56:31 pekangas Exp $
 *
 * Copyright 1996 Petteri Kangaslampi
*/

#include <windows.h>
#include <ddeml.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "midasdll.h"
#include "midpres.h"
#include "MidpNT.h"
#include "MidpList.h"
#include "MidpModeless.h"
#include "MidpView.h"
#include "SongInfo.h"
#include "InstList.h"
#include "ViewList.h"
#include "Archivers.h"
#include "Registry.h"
#include "midpecho.h"
#include "echoedit.h"

#define POLLRATE 100

#define NUMLINES 100
#define MAXLINE 255

#define MIDP_ADDTEXT WM_USER+100

HANDLE          instance;               /* program instance */

static char     *mainWinClass = "midpNTClass";  /* main window class name */

int             oversample = 0;

HWND            mainWinHandle;
HWND            editWinHandle;
int             editWidth, editHeight;
HANDLE          accelHandle;

static char     *textLines[NUMLINES];   /* all text lines */
static char     *text;
static int      numLines;
static char     loadFileName[MAX_PATH + 16];    /* fudge factor */

MIDASmodule     module = NULL;
MIDASmodulePlayHandle playHandle = 0;

char            defaultDir[MAX_PATH];
char            unzipDir[MAX_PATH];

LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT message, WPARAM wparam,
    LPARAM lparam);
void StartDDEServer(void);
void PlayFile(char *fileName);
int PlayDDE(char *fileName);
void ReadRegistry(void);
void WriteRegistry(void);
void RestoreMainWindow(void);
void SaveViews(void);
void RestoreViews(void);



DWORD           ddeInstance;
int             ddeInit = 0;

SongInfoView    *songInfoView = NULL;
InstListView    *instListView = NULL;
EchoEditView    *echoEditView = NULL;

midpList        midpModelessList;
midpViewWindowList viewWindowList;
midpViewList    viewList;

char            *iniFile = "MidpNT.ini";
char            *defaultDirectory = "";

char            *baseKey = "Software\\Sahara Surfers\\MidpNT";
char            *midasKey = "Software\\Sahara Surfers\\MidpNT\\MIDAS Setup";

static int      mainWinX, mainWinY, mainWinWidth, mainWinHeight;

int             viewsChildren = 1;

int             minimalUI = 0;

unsigned        filterMode = 0;

unsigned        defaultStereoSeparation = 0;

midpEchoEffectList echoEffectList;


void PrintEffects()
{
    midpEchoEffect *effect;
    midpEcho *echo;
    unsigned n;

    effect = echoEffectList.FirstEffect();
    while ( effect != NULL )
    {
        printf("Effect: %s:\n", effect->GetName());
        printf("Feedback %f, %u echoes\n", effect->GetFeedback(),
               effect->GetNumEchoes());
        for ( n = 0; n < effect->GetNumEchoes(); n++ )
        {
            echo = effect->GetEcho(n);
            printf("    Echo %u: Delay %f, Gain %f, Reverse %i\n",
                   n, echo->GetDelay(), echo->GetGain(),
                   echo->GetReverse());
        }
        effect = echoEffectList.NextEffect();
    }
}


void EchoEffectTest()
{
    midpEchoEffect *effect;
    midpEcho *echo;
        
    PrintEffects();

    effect = echoEffectList.NewEffect();
    effect->Rename("Superkaiku");
    effect->SetNumEchoes(1);
    effect->SetFeedback(0.5);
    echo=effect->GetEcho(0);
    echo->SetDelay(250);
    echo->SetGain(0.4);
    echo->SetReverse(0);
    effect->SaveToRegistry();

    PrintEffects();
}


/****************************************************************************\
*
* Function:     int WinMain(HANDLE thisInstance, HANDLE prevInstance, LPSTR
*                   commandLine, int showCommand);
*
* Description:  Windows main function - common initialization and message loop
*
\****************************************************************************/

int WINAPI WinMain(HANDLE thisInstance, HANDLE prevInstance,
    LPSTR commandLine, int showCommand)
{
    MSG         msg;
    WNDCLASS    wc;
    midpModeless *modeless;
    char        *c, *d, *end;
    int         inQuotes = 0;
    int         hadCharacters = 0;
//    char        kala[256];

    prevInstance = prevInstance;

    StartupMIDAS();

    if ( !MIDASreadConfigRegistry((DWORD) HKEY_CURRENT_USER, midasKey) )
    {
        if ( MIDASconfig() )
            MIDASwriteConfigRegistry((DWORD) HKEY_CURRENT_USER, midasKey);
    }

//    sprintf(kala, "commandLine: \"%s\"", commandLine);
//    MessageBox(NULL, kala, "Kala", MB_OK);

    /* Build file name to load: */
//    strcpy(loadFileName, commandLine);
    loadFileName[0] = 0;
    if ( strlen(commandLine) > 0 )
    {
        c = commandLine;

        while ( (*c != 0) && (isspace(*c)) )
            c++;

        if ( *c == '-' )
        {
            c++;

            switch ( *(c++) )
            {
                case 'm':
                    minimalUI = 1;
                    break;
            }

            while ( (*c != 0) && (!isspace(*c)) )
                c++;
        }

        d = loadFileName;
        end = d;

        while ( *c != 0 )
        {
            if ( *c == '\"' )
            {
                inQuotes ^= 1;
                hadCharacters = 1;
            }
            else
            {
                if ( (*c == ' ') && (!hadCharacters) )
                {
                    c++;
                    continue;
                }

                *(d++) = *c;

                if ( inQuotes || (*c != ' ') )
                    end = d;

                hadCharacters = 1;
            }
            c++;
        }

        *end = 0;
    }

//    sprintf(kala, "loadFileName: \"%s\"", loadFileName);
//    MessageBox(NULL, kala, "Kala", MB_OK);

    instance = thisInstance;

    if ( PlayDDE(loadFileName) )
        return 0;

    /* This is the first instance - now set up a DDE server: */
    StartDDEServer();

    /* Let's be a Very Important Process: */
    SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    ReadRegistry();

    songInfoView = new SongInfoView;
    viewList.AddView(songInfoView);
    instListView = new InstListView;
    viewList.AddView(instListView);
    echoEditView = new EchoEditView;
    viewList.AddView(echoEditView);

    /* Set up and register window class for the main window: */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = mainWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, "MIDP_ICON");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = "MAINMENU";
    wc.lpszClassName = mainWinClass;
    if ( RegisterClass(&wc) == 0 )
        return FALSE;

    if ( !minimalUI )
        RestoreMainWindow();
    else
    {
        mainWinX = mainWinY = 100;
        mainWinWidth = 320;
        mainWinHeight = 0;
    }

    /* Create main window: */

    mainWinHandle = CreateWindow(
        mainWinClass,                           /* class */
        "MIDAS Module Player for Windows NT",   /* caption */
        WS_OVERLAPPEDWINDOW,                    /* style */
        mainWinX,                               /* init. x pos */
        mainWinY,                               /* init. y pos */
        mainWinWidth,                           /* init. x size */
        mainWinHeight,                          /* init. y size */
        NULL,                                   /* parent window */
        NULL,                                   /* menu handle */
        instance,                               /* program handle */
        NULL                                    /* create parms */
    );

    if ( !mainWinHandle )
        return FALSE;

    if ( (accelHandle = LoadAccelerators(instance, "MAINACCELERATORS")) ==
        NULL )
    {
        MessageBox(NULL, "Coulnd't load accelerators", "MidpNT Initialization error",
            MB_OK | MB_ICONEXCLAMATION);

        return FALSE;
    }

    /* Display main window: */
    ShowWindow(mainWinHandle, showCommand);
    UpdateWindow(mainWinHandle);

    if ( !minimalUI )
        RestoreViews();

    /* Da Message Loop: */
    while ( GetMessage(&msg, NULL, NULL, NULL) )
    {
        if ( !TranslateAccelerator(mainWinHandle, accelHandle, &msg) )
        {
            modeless = (midpModeless*) midpModelessList.GetFirst();
            while ( modeless != NULL )
            {
                if ( IsDialogMessage(modeless-> hwnd, &msg) )
                    break;
                modeless = (midpModeless*) midpModelessList.GetNext();
            }

            if ( modeless == NULL )
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }
    }

    WriteRegistry();

    if ( ddeInit )
        DdeUninitialize(ddeInstance);

    return(msg.wParam);
}


void RestoreViews(void)
{
    int         i, n;
    Registry    reg;
    char        s[256];
    char        v[64];
    midpViewWindow *win;
    midpView    *view;

    reg.OpenKey(baseKey);
    n = reg.ValueDWORD("NumViewWindows", 0);

    for ( i = 0; i < n; i++ )
    {
        sprintf(s, "%s\\ViewWindow%i", baseKey, i);

        if ( reg.KeyExists(s) )
        {
            reg.OpenKey(s);
            reg.ValueString("ViewClass", "none", v, 63);

            view = viewList.FindView(v);
            win = view->CreateViewWindow(&reg);
        }
    }
}



void SaveViews(void)
{
    int         i;
    Registry    reg;
    char        s[256];
    midpViewWindow *win;

    reg.OpenKey(baseKey);
    reg.WriteDWORD("NumViewWindows", viewWindowList.NumWindows());

    for ( i = 0, win = viewWindowList.GetFirst(); win != NULL; i++,
        win = viewWindowList.GetNext() )
    {
        sprintf(s, "%s\\ViewWindow%i", baseKey, i);

        if ( !reg.KeyExists(s) )
            reg.CreateKey(s);
        reg.OpenKey(s);

        win->SaveState(&reg);
    }
}




void ReadRegistry(void)
{
    Registry    reg;

    if ( !reg.KeyExists(baseKey) )
        reg.CreateKey(baseKey);
    else
        reg.OpenKey(baseKey);

    reg.ValueString("DefaultDirectory", "c:\\", defaultDir, MAX_PATH);
    reg.ValueString("UnzipDirectory", "c:\\$$MidpNT$$", unzipDir, MAX_PATH);
    viewsChildren = reg.ValueDWORD("OpenViewsAsChildWindows", 1);
    defaultStereoSeparation = reg.ValueDWORD("DefaultStereoSeparation", 32);
    MIDASsetOption(MIDAS_OPTION_DEFAULT_STEREO_SEPARATION,
                   defaultStereoSeparation);
}


void WriteRegistry(void)
{
    Registry    reg;

    reg.OpenKey("Software\\Sahara Surfers\\MidpNT");
    reg.WriteString("DefaultDirectory", defaultDir);
    reg.WriteString("UnzipDirectory", unzipDir);
    reg.WriteDWORD("OpenViewsAsChildWindows", viewsChildren);
    reg.WriteDWORD("DefaultStereoSeparation", defaultStereoSeparation);    
}


void DDEError(char *function, UINT error)
{
    char        msg[128];
    sprintf(msg, "DDE Failure - %s: %u", function, error);
    MessageBox(NULL, msg, "MidpNT Initialization error",
        MB_OK | MB_ICONEXCLAMATION);
    CloseMIDAS();
    ExitProcess(17);
}



HDDEDATA CALLBACK DdeServerCallback(
    UINT  uType,	// transaction type
    UINT  uFmt,	// clipboard data format
    HCONV  hconv,	// handle of the conversation
    HSZ  hsz1,	// handle of a string
    HSZ  hsz2,	// handle of a string
    HDDEDATA  hdata,	// handle of a global memory object
    DWORD  dwData1,	// transaction-specific data
    DWORD  dwData2 	// transaction-specific data
   )
{
    int         len;
    char        *str;

    uType = uType;
    uFmt = uFmt;
    hconv = hconv;
    hsz1 = hsz1;
    hsz2 = hsz2;
    hdata = hdata;
    dwData1 = dwData1;
    dwData2 = dwData2;

    switch ( uType )
    {
        case XTYP_CONNECT:
            len = DdeQueryString(ddeInstance, hsz1, NULL, 0, CP_WINANSI);
            if ( len == 0 )
                Panic("DdeServerCallback: Topic string length 0");
            str = new char[len+1];
            if ( !DdeQueryString(ddeInstance, hsz1, str, len+1, CP_WINANSI) )
                Panic("DdeServerCallback: DdeQueryString() failed");

            if ( !memcmp(str, "PlayFile:", 9) )
            {
                if ( strlen(str) > 9 )
                {
//                    AddTextLine(str);
                    PlayFile(str+9);
                }
            }
            else
                Panic("DdeServerCallback: Bad topic");

            return (HDDEDATA) TRUE;
    }

    return (HDDEDATA) 0L;
}



void StartDDEServer(void)
{
    UINT        ddeError;
    HSZ         strHandle;

    if ( (ddeError = DdeInitialize(&ddeInstance, DdeServerCallback,
        APPCLASS_STANDARD, 0)) !=
        DMLERR_NO_ERROR )
        DDEError("Server DdeInitialize", ddeError);

    ddeInit = 1;

    if ( (strHandle = DdeCreateStringHandle(ddeInstance, "MidpNT",
        CP_WINANSI)) == 0L )
        DDEError("DdeCreateStringHandle", DdeGetLastError(ddeInstance));

    if ( DdeNameService(ddeInstance, strHandle, (HSZ) 0L, DNS_REGISTER) == 0 )
        DDEError("DdeNameService", DdeGetLastError(ddeInstance));

    if ( !DdeFreeStringHandle(ddeInstance, strHandle) )
        DDEError("DdeFreeStingHandle", DdeGetLastError(ddeInstance));
}



HDDEDATA CALLBACK DdeClientCallback(
    UINT  uType,	// transaction type
    UINT  uFmt,	// clipboard data format
    HCONV  hconv,	// handle of the conversation
    HSZ  hsz1,	// handle of a string
    HSZ  hsz2,	// handle of a string
    HDDEDATA  hdata,	// handle of a global memory object
    DWORD  dwData1,	// transaction-specific data
    DWORD  dwData2 	// transaction-specific data
   )
{
    uType = uType;
    uFmt = uFmt;
    hconv = hconv;
    hsz1 = hsz1;
    hsz2 = hsz2;
    hdata = hdata;
    dwData1 = dwData1;
    dwData2 = dwData2;

    return (HDDEDATA) 0L;
}



int PlayDDE(char *fileName)
{
    char        *str;
    HSZ         server, topic;
    HCONV       conversation;
    UINT        ddeError;
    int         val;
    static DWORD       ddeInstance;
    char        *dummy;

    if ( strlen(fileName) )
    {
        str = new char[_MAX_PATH + 10];
        strcpy(str, "PlayFile:");
        /*
        if ( _fullpath(str+9, fileName, _MAX_PATH) == NULL )
            Panic("PlayDDE(): _fullpath failed");
        */
        if ( GetFullPathName(fileName, _MAX_PATH, str+9, &dummy) == 0 )
            Panic("PlayDDE(): GetFullPathName() failed");
    }
    else
        str = "PlayFile:";

    if ( (ddeError = DdeInitialize(&ddeInstance, DdeClientCallback,
        APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0)) !=
        DMLERR_NO_ERROR )
        DDEError("Client DdeInitialize", ddeError);

    ddeInit = 1;

    if ( (server = DdeCreateStringHandle(ddeInstance, "MidpNT",
        CP_WINANSI)) == 0L )
        DDEError("DdeCreateStringHandle", DdeGetLastError(ddeInstance));

    if ( (topic = DdeCreateStringHandle(ddeInstance, str,
        CP_WINANSI)) == 0L )
        DDEError("DdeCreateStringHandle", DdeGetLastError(ddeInstance));

    if ( (conversation = DdeConnect(ddeInstance, server, topic, NULL)) == 0L )
    {
        ddeError = DdeGetLastError(ddeInstance);
        if ( ddeError == DMLERR_NO_CONV_ESTABLISHED )
            val = 0;
        else
            DDEError("DdeConnect", ddeError);
    }
    else
    {
        val = 1;

        if ( !DdeDisconnect(conversation) )
            DDEError("DdeDisconnect", DdeGetLastError(ddeInstance));
    }

    if ( !DdeFreeStringHandle(ddeInstance, server) )
        DDEError("DdeFreeStingHandle", DdeGetLastError(ddeInstance));
    if ( !DdeFreeStringHandle(ddeInstance, topic) )
        DDEError("DdeFreeStingHandle", DdeGetLastError(ddeInstance));

    DdeUninitialize(ddeInstance);
    ddeInit = 0;

    return val;
}







void Panic(char *message)
{
    MessageBox(NULL, message, "MIDP PANIC",
        MB_OK | MB_ICONEXCLAMATION);
    CloseMIDAS();
    if ( ddeInit )
        DdeUninitialize(ddeInstance);
    ExitProcess(1);
}



char *FileNameFilters =
    "All Modules\0*.mod;*.s3m;*.xm;*.it;*.mdz;*.s3z;*.xmz;*.itz\0"
    "All Files\0*.*\0"
    "Unpacked Modules\0*.mod;*.s3m;*.xm;*.it\0"
    "Packed Modules\0*.mdz;*.s3z;*.xmz;*.itz;*.zip\0"
    "\0\0";


char *GetFileName(char *windowTitle)
{
    static OPENFILENAME *ofn = NULL;
    DWORD       error;

    if ( ofn == NULL )
    {
        if ( (ofn = (OPENFILENAME*) malloc(sizeof(OPENFILENAME))) == NULL )
            return NULL;

        ofn->lStructSize = sizeof(OPENFILENAME);
        ofn->hwndOwner = 0;
        ofn->hInstance = 0;
        ofn->lpstrFilter = (LPCSTR) FileNameFilters;
        ofn->lpstrCustomFilter = NULL;
        ofn->nMaxCustFilter = 0;
        ofn->nFilterIndex = 1;
        if ( (ofn->lpstrFile = (char*) malloc(1024)) == NULL )
        {
            ofn = NULL;
            return NULL;
        }
        memset(ofn->lpstrFile, 0, 1024);
        ofn->nMaxFile = 1023;
        ofn->lpstrFileTitle = NULL;
        ofn->nMaxFileTitle = 0;
        ofn->lpstrInitialDir = defaultDir;  //!
        ofn->lpstrTitle = windowTitle;
        ofn->nFileOffset = 0;
        ofn->nFileExtension = 0;
        ofn->lpstrDefExt = 0;
        ofn->Flags = OFN_EXPLORER;
    }
    else
        ofn->lpstrInitialDir = NULL;

    ofn->lpstrTitle = windowTitle;

    if ( GetOpenFileName(ofn) == FALSE )
    {
        error = CommDlgExtendedError();
        return NULL;
    }

    return ofn->lpstrFile;
}


void ShowTextBottom(void)
{
    int         firstVisible;
    int         lines;
    int         newFirst;
    int         textAreaHeight;
    int         visibleLines;
    HFONT       oldFont, font;
    HDC         hdc;
    TEXTMETRIC  textMetric;

    font = (HFONT) SendMessage(editWinHandle, WM_GETFONT, 0, 0);
    firstVisible = (int) SendMessage(editWinHandle, EM_GETFIRSTVISIBLELINE, 0, 0);
    lines = (int) SendMessage(editWinHandle, EM_GETLINECOUNT, 0, 0);
    textAreaHeight = editHeight - GetSystemMetrics(SM_CYHSCROLL);

    hdc = GetDC(editWinHandle);
    oldFont = SelectObject(hdc, font);
    if ( GetTextMetrics(hdc, &textMetric) == FALSE )
        return; /* FIXME */
    SelectObject(hdc, oldFont);

    if ( textMetric.tmHeight > 0 )
        visibleLines = textAreaHeight / textMetric.tmHeight;
    else
        visibleLines = 1;

    newFirst = lines - visibleLines - 1;
    if ( newFirst < 0 )
        newFirst = 0;
    if ( newFirst > (lines-1) )
        newFirst = lines-1;

    SendMessage(editWinHandle, EM_LINESCROLL, 0, newFirst - firstVisible);
}




void BuildText(void)
{
    int         i;

    text[0] = 0;
    for ( i = 0; i < numLines; i++ )
    {
        strcat(text, textLines[i]);
        strcat(text, "\r\n");
    }
}



void AddTextLine(char *textLine)
{
    int         i;
    char        *temp;

    if ( numLines == NUMLINES )
    {
        temp = textLines[0];
        for ( i = 0; i < (NUMLINES - 1); i++ )
            textLines[i] = textLines[i+1];
        textLines[NUMLINES-1] = temp;
        numLines--;
    }

    strncpy(textLines[numLines], textLine, MAXLINE);
    numLines++;

    BuildText();

    SendMessage(editWinHandle, WM_SETREDRAW, (WPARAM) FALSE, 0);
    SendMessage(editWinHandle, WM_SETTEXT, 0, (LPARAM) text);
    ShowTextBottom();
    SendMessage(editWinHandle, WM_SETREDRAW, (WPARAM) TRUE, 0);
    InvalidateRect(editWinHandle, NULL, FALSE);
    UpdateWindow(editWinHandle);
}




void InitText(void)
{
    int         i;

    text = (char*) malloc(NUMLINES * (MAXLINE+1));
    text[0] = 0;

    for ( i = 0; i < NUMLINES; i++ )
    {
        textLines[i] = (char*) malloc(MAXLINE+1);
        textLines[i][0] = 0;
    }
}



BOOL CALLBACK PreferencesDialogProc(HWND hwnd, UINT message, WPARAM wparam,
    LPARAM lparam)
{
    char str[32];
    
    lparam = lparam;                    // get rid of a stupid warning

    switch ( message )
    {
        case WM_INITDIALOG:
            SendDlgItemMessage(hwnd, PREFERENCES_DEFAULTDIR, WM_SETTEXT,
                0, (LPARAM) defaultDir);
            SendDlgItemMessage(hwnd, PREFERENCES_UNZIPDIR, WM_SETTEXT,
                0, (LPARAM) unzipDir);
            SendDlgItemMessage(hwnd, PREFERENCES_VIEWSCHILDREN, BM_SETCHECK,
                (WPARAM) viewsChildren, 0);
            itoa(defaultStereoSeparation, str, 10);
            SendDlgItemMessage(hwnd, PREFERENCES_DEFAULTSTEREOSEPARATION,
                               WM_SETTEXT, 0, (LPARAM) str);
            return TRUE;

        case WM_COMMAND:
            switch ( LOWORD(wparam) )
            {
                case IDOK:
                    SendDlgItemMessage(hwnd, PREFERENCES_DEFAULTDIR,
                        WM_GETTEXT, MAX_PATH - 1, (LPARAM) defaultDir);
                    SendDlgItemMessage(hwnd, PREFERENCES_UNZIPDIR,
                        WM_GETTEXT, MAX_PATH - 1, (LPARAM) unzipDir);
                    viewsChildren = SendDlgItemMessage(hwnd,
                        PREFERENCES_VIEWSCHILDREN, BM_GETCHECK, 0, 0);
                    SendDlgItemMessage(hwnd,
                                       PREFERENCES_DEFAULTSTEREOSEPARATION,
                                       WM_GETTEXT, 31, (LPARAM) str);
                    if ( atoi(str) < 0 )
                        defaultStereoSeparation = 0;
                    else
                        defaultStereoSeparation = atoi(str);
                    if ( defaultStereoSeparation > 64 )
                        defaultStereoSeparation = 64;
                    MIDASsetOption(MIDAS_OPTION_DEFAULT_STEREO_SEPARATION,
                                   defaultStereoSeparation);
                    EndDialog(hwnd, 0);
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
    }

    return FALSE;
}



void PlayFile(char *fileName)
{
    if ( module != NULL )
    {
        StopFreeModule(playHandle, module);
        module = NULL;
        playHandle = 0;
    }

    if ( IsArchive(fileName) )
    {
        module = LoadArchive(fileName);

        if ( module != NULL )
            playHandle = PlayModule(module);
    }
    else
    {
        module = LoadModule(fileName);

        if ( module != NULL )
            playHandle = PlayModule(module);
    }

    viewWindowList.SongChanged();
}


void RestoreMainWindow(void)
{
    Registry    reg;

    reg.OpenKey(baseKey);

    if ( reg.ValueExists("MainWindowX") )
    {
        mainWinX = reg.ValueDWORD("MainWindowX", 100);
        mainWinY = reg.ValueDWORD("MainWindowY", 100);
        mainWinWidth = reg.ValueDWORD("MainWindowWidth", 200);
        mainWinHeight = reg.ValueDWORD("MainWindowHeight", 200);
    }
    else
        mainWinX = mainWinY = mainWinWidth = mainWinHeight = CW_USEDEFAULT;
}


void SaveMainWindow(void)
{
    Registry    reg;
    RECT        rect;

    reg.OpenKey(baseKey);

    GetWindowRect(mainWinHandle, &rect);
    reg.WriteDWORD("MainWindowX", rect.left);
    reg.WriteDWORD("MainWindowY", rect.top);
    reg.WriteDWORD("MainWindowWidth", rect.right - rect.left);
    reg.WriteDWORD("MainWindowHeight", rect.bottom - rect.top);
}



void ListInstruments(void)
{
    char        str[256];
    unsigned    i;
    MIDASmoduleInfo moduleInfo;
    MIDASinstrumentInfo instrumentInfo;

    if ( module != NULL )
    {
        MIDASgetModuleInfo(module, &moduleInfo);

        for ( i = 0; i < moduleInfo.numInstruments; i++ )
        {
            MIDASgetInstrumentInfo(module, i, &instrumentInfo);
            sprintf(str, "%02X: %s", i + 1, instrumentInfo.instName);
            AddTextLine(str);
        }
    }
}






/****************************************************************************\
*
* Function:     LRESULT mainWindowProc(HWND hwnd, UINT message, WPARAM wparam,
*                   LPARAM lparam);
*
* Description:  Main window message processing function
*
\****************************************************************************/

LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT message, WPARAM wparam,
    LPARAM lparam)
{
    char        *fileName;
    int         i;
    char        aboutText[256];
    MIDASplayStatus playStatus;
    MIDASmoduleInfo moduleInfo;
    midpEchoEffect *effect;

    switch ( message )
    {
        case WM_CREATE:
            /* Create edit box for console text: */
            editWinHandle = CreateWindow(
                "EDIT",                 /* Window class */
                NULL,                   /* Window title */
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_LEFT |
                    ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL |
                    ES_READONLY,
                0, 0, 0, 0,             /* set size in WM_SIZE message */
                hwnd,                   /* parent window */
                (HMENU) ID_EDIT,        /* edit control ID */
                instance,
                NULL);                  /* no window creation data */

            mainWinHandle = hwnd;

            editWidth = editHeight = 0;

            /* Initialize text: */
            InitText();

//            AddTextLine("MIDAS Module Player for Windows NT initialized");
//            AddTextLine("");
            AddTextLine("MIDAS Module Player for Windows NT");
            AddTextLine("Copyright 1996,1997 Petteri Kangaslampi");
            AddTextLine("Version " FULLVERSTR ", built " __DATE__ ".");
            AddTextLine("");
            sprintf(aboutText, "Using MIDAS Digital Audio System v%s,",
                MIDASgetVersionString());
            AddTextLine(aboutText);
            AddTextLine("Copyright 1996,1997 Housemarque Inc.");

            AddTextLine("");

            /* Set edit control text: */
            SendMessage(editWinHandle, WM_SETTEXT, 0, (LPARAM) text);

            InitMIDAS();

            AddTextLine("MIDAS Digital Audio System initialized");

            if ( strlen(loadFileName) != 0 )
                PlayFile(loadFileName);

/*            EchoEffectTest();*/

            break;

        case WM_SIZE:
            /* Make the edit control the size of the window's client area: */
            MoveWindow(editWinHandle,
                0, 0,           /* starting x- and y-coordinates */
                LOWORD(lparam), /* width of client area          */
                HIWORD(lparam), /* height of client area         */
                TRUE);          /* repaint window                */
            editWidth = LOWORD(lparam);
            editHeight = HIWORD(lparam);
            ShowTextBottom();
            return 0;

        case WM_COMMAND:
            switch ( LOWORD(wparam) )
            {
                case MENU_FILE_OPEN:
                    fileName = GetFileName("Open File");
                    if ( fileName != NULL )
                        PlayFile(fileName);
                    break;

                case MENU_FILE_CLOSE:
                    if ( module != NULL )
                    {
                        StopFreeModule(playHandle, module);
                        module = NULL;
                        playHandle = 0;
                    }
                    viewWindowList.SongChanged();
                    break;

                case MENU_FILE_EXIT:
                    DestroyWindow(hwnd);
                    break;

                case MENU_OPTIONS_SOUND:
                    if ( module != NULL )
                    {
                        StopFreeModule(playHandle, module);
                        module = NULL;
                        playHandle = 0;
                    }
                    viewWindowList.SongChanged();
                    effect = currentEffect;
                    if ( currentEffect != NULL )
                        currentEffect->Deactivate();
                    CloseMIDAS();
                    if ( MIDASconfig() )
                        MIDASwriteConfigRegistry((DWORD) HKEY_CURRENT_USER,
                                                 midasKey);
                    InitMIDAS();
                    if ( effect != NULL )
                        effect->Activate();
                    break;

                case MENU_VIEWS_SONGINFO:
                    songInfoView->CreateViewWindow();
                    break;

                case MENU_VIEWS_INSTLIST:
                    instListView->CreateViewWindow();
                    break;

                case MENU_VIEWS_ECHOEFFECTS:
                    echoEditView->CreateViewWindow();
                    break;

                case MENU_PLAYING_NEXTPOSITION:
                    if ( module != NULL )
                    {
                        MIDASgetPlayStatus(playHandle, &playStatus);
                        MIDASgetModuleInfo(module, &moduleInfo);
                        if ( playStatus.position < (moduleInfo.songLength-1) )
                            MIDASsetPosition(playHandle,
                                             playStatus.position + 1);
                    }
                    break;

                case MENU_PLAYING_PREVPOSITION:
                    if ( module != NULL )
                    {
                        MIDASgetPlayStatus(playHandle, &playStatus);
                        MIDASgetModuleInfo(module, &moduleInfo);
                        if ( playStatus.position > 0 )
                            MIDASsetPosition(playHandle,
                                             playStatus.position - 1);
                    }
                    break;

                case MENU_PLAYING_CENTERCHANNELS:
                    if ( module != NULL )
                    {
                        MIDASgetModuleInfo(module, &moduleInfo);
                        for ( i = 0; i < (int) moduleInfo.numChannels; i++ )
                        {
                            /* Kluge: */
                            if ( !MIDASsetSamplePanning(
                                (MIDASsamplePlayHandle) i + 1,
                                MIDAS_PAN_MIDDLE))
                                MIDASerror();
                        }
                    }
                    break;

                case MENU_PLAYING_FILTER:
                    filterMode++;
                    if ( filterMode > 2 )
                        filterMode = 0;
                    MIDASsetOption(MIDAS_OPTION_FILTER_MODE, filterMode);
                    sprintf(aboutText, "Filter mode %u", filterMode);
                    AddTextLine(aboutText);
                    break;

                case MENU_OPTIONS_PREFERENCES:
                    DialogBox(instance, "PREFERENCES", hwnd, (DLGPROC)
                        &PreferencesDialogProc);
                    break;

                case MENU_HELP_ABOUT:
                    sprintf(aboutText, "MIDAS Module Player for "
                        "Windows NT\nCopyright 1996,1997 Petteri Kangaslampi\n"
                        "Version " FULLVERSTR ", built " __DATE__ ".\n\n"
                        "Using MIDAS Digital Audio System v%s\n"
                        "Copyright 1996,1997 Housemarque Inc.",
                        MIDASgetVersionString());
                    MessageBox(mainWinHandle, aboutText, "About MidpNT",
                        MB_OK | MB_ICONINFORMATION);
                    break;

                case MENU_OPTIONS_SAVE_WIN:
                    SaveViews();
                    SaveMainWindow();
                    break;

                case MENU_OPTIONS_RESTORE_WIN:
                    RestoreViews();
                    RestoreMainWindow();
                    break;

                case MENU_PLAYING_LIST_INSTRUMENTS:
                    ListInstruments();
                    break;
            }
            break;

        case WM_DESTROY:
            CloseMIDAS();
            PostQuitMessage(0);
            break;

        default:
            return(DefWindowProc(hwnd, message, wparam, lparam));
    }

    return 0;
}



/*
 * $Log: midpnt.cpp,v $
 * Revision 1.27  1997/07/31 10:56:31  pekangas
 * Renamed from MIDAS Sound System to MIDAS Digital Audio System
 *
 * Revision 1.26  1997/07/29 13:28:53  pekangas
 * Changed to use MIDAS internal setup for sound options
 *
 * Revision 1.25  1997/07/12 12:04:48  pekangas
 * Changed references to "MIDAS Digital Audio System" to "MIDAS Digital Audio System"
 *
 * Revision 1.24  1997/07/11 13:36:39  pekangas
 * Added filters and total gain to echoes.
 * Fixed crashes when output mode was changed when an echo effect was
 * active.
 *
 * Revision 1.23  1997/07/10 18:41:27  pekangas
 * Added echo effect editor and echo effect support
 *
 * Revision 1.22  1997/07/09 14:19:35  pekangas
 * Changed to use Visual C resource editor instead of Resource Workshop
 *
 * Revision 1.21  1997/07/09 09:05:14  pekangas
 * Added default stereo separation setup
 *
 * Revision 1.20  1997/07/08 19:19:50  pekangas
 * Added new MIDAS setup, changed version to 0.3.0 and added a possibility
 * to compile a console version
 *
 * Revision 1.19  1997/06/30 11:30:04  pekangas
 * Added oversampling mixing mode support
 * Added IT support
 *
 * Revision 1.18  1997/05/20 20:45:00  pekangas
 * Added filter toggle
 *
 * Revision 1.17  1997/05/02 13:21:57  pekangas
 * Changed to work with new module playback functions and keep track of
 * module playing handles
 *
 * Revision 1.16  1997/02/05 22:57:53  pekangas
 * Fixed small problems caused by RCS->CVS move.
 * Changed to use new build system - GNU Make now required.
 *
 * Revision 1.15  1997/01/25 13:17:44  pekangas
 * Rewrote archive support, added an icon
 *
 * Revision 1.14  1997/01/17 17:51:53  pekangas
 * Fixed a Visual C warning
 *
 * Revision 1.13  1997/01/17 17:41:35  pekangas
 * Text is also added to main window when in minimal UI mode
 * (it's not usually visible though)
 *
 * Revision 1.12  1997/01/17 00:08:23  pekangas
 * Added minimal UI mode - command line switch "-m"
 *
 * Revision 1.11  1997/01/14 22:25:11  pekangas
 * Finally fixed (?) all command line arguments with spaces
 *
 * Revision 1.10  1997/01/14 20:47:40  pekangas
 * Fixed channel centering (a stupid off-by-one error)
 *
 * Revision 1.9  1997/01/14 17:42:08  pekangas
 * Changed to use MIDAS DLL API
 *
 * Revision 1.8  1997/01/14 16:36:14  pekangas
 * Fixed support for files names with spaces as command line arguments
 *
 * Revision 1.7  1996/08/03 13:06:08  pekangas
 * Added 1 to "view instruments" instrument numbers
 *
 * Revision 1.6  1996/08/02  20:14:30  pekangas
 * Added View Instruments -function
 *
 * Revision 1.5  1996/07/16  20:20:01  pekangas
 * Added an error message if loading accelerators fails
 * (this happens if resources are not found)
 *
 * Revision 1.4  1996/07/16  19:11:59  pekangas
 * Fixed WinMain definition for Visual C
 *
 * Revision 1.3  1996/07/04  17:48:41  pekangas
 * Converted to LFs
 *
*/
