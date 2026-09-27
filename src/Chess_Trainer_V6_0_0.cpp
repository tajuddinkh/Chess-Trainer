// Chess Trainer 6.0.0, native Windows C++ build
// ======================================
// Portable 64-bit Windows GUI. Responsive modern interface with Stockfish WDL evaluation bar.
// No Python, PyInstaller, .NET, Java or Go runtime.
// The only external runtime file is the official stockfish.exe placed beside this EXE.
//
// Final portable folder:
//   Chess_Trainer.exe
//   stockfish.exe
//
// This source deliberately uses the native Win32 API and a dedicated engine thread.
// Stockfish is started once with CREATE_NO_WINDOW and SW_HIDE, with stdin/stdout pipes.
// The GUI thread never waits for Stockfish, so engine analysis cannot freeze the window.

// We intentionally do not include windows.h or the C++ runtime. This keeps the produced
// executable small and dependency-free. The necessary Win32 declarations are below.

// --------------------------- Minimal Win32 types ---------------------------

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned int DWORD;
typedef int BOOL;
typedef long LONG;
typedef long long LRESULT;
typedef unsigned long long ULONG_PTR;
typedef unsigned long long UINT_PTR;
typedef unsigned long long WPARAM;
typedef long long LPARAM;
typedef void* HANDLE;
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HICON;
typedef void* HCURSOR;
typedef void* HBRUSH;
typedef void* HBITMAP;
typedef void* HDC;
typedef void* HFONT;
typedef void* HPEN;
typedef void* HGDIOBJ;
typedef void* HMENU;
typedef const wchar_t* LPCWSTR;
typedef wchar_t* LPWSTR;
typedef const void* LPCVOID;
typedef void* LPVOID;
typedef unsigned long long U64;

#define WINAPI __stdcall
#define CALLBACK __stdcall
#define TRUE 1
#define FALSE 0
#define NULLPTR nullptr

struct POINT { LONG x; LONG y; };
struct RECT { LONG left; LONG top; LONG right; LONG bottom; };
struct MINMAXINFO { POINT ptReserved; POINT ptMaxSize; POINT ptMaxPosition; POINT ptMinTrackSize; POINT ptMaxTrackSize; };
struct MSG {
    HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; DWORD lPrivate;
};
struct TRACKMOUSEEVENT { DWORD cbSize; DWORD dwFlags; HWND hwndTrack; DWORD dwHoverTime; };
struct PAINTSTRUCT {
    HDC hdc; BOOL fErase; RECT rcPaint; BOOL fRestore; BOOL fIncUpdate; BYTE rgbReserved[32];
};
typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
struct WNDCLASSEXW {
    UINT cbSize; UINT style; WNDPROC lpfnWndProc; int cbClsExtra; int cbWndExtra;
    HINSTANCE hInstance; HICON hIcon; HCURSOR hCursor; HBRUSH hbrBackground;
    LPCWSTR lpszMenuName; LPCWSTR lpszClassName; HICON hIconSm;
};
struct SECURITY_ATTRIBUTES { DWORD nLength; LPVOID lpSecurityDescriptor; BOOL bInheritHandle; };
struct STARTUPINFOW {
    DWORD cb; LPWSTR lpReserved; LPWSTR lpDesktop; LPWSTR lpTitle;
    DWORD dwX; DWORD dwY; DWORD dwXSize; DWORD dwYSize;
    DWORD dwXCountChars; DWORD dwYCountChars; DWORD dwFillAttribute; DWORD dwFlags;
    WORD wShowWindow; WORD cbReserved2; BYTE* lpReserved2;
    HANDLE hStdInput; HANDLE hStdOutput; HANDLE hStdError;
};
struct PROCESS_INFORMATION { HANDLE hProcess; HANDLE hThread; DWORD dwProcessId; DWORD dwThreadId; };
struct SYSTEMTIME { WORD wYear,wMonth,wDayOfWeek,wDay,wHour,wMinute,wSecond,wMilliseconds; };

extern "C" {
__declspec(dllimport) void WINAPI ExitProcess(UINT);
__declspec(dllimport) HINSTANCE WINAPI GetModuleHandleW(LPCWSTR);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HINSTANCE, LPWSTR, DWORD);
__declspec(dllimport) BOOL WINAPI CreatePipe(HANDLE*, HANDLE*, SECURITY_ATTRIBUTES*, DWORD);
__declspec(dllimport) BOOL WINAPI SetHandleInformation(HANDLE, DWORD, DWORD);
__declspec(dllimport) BOOL WINAPI CreateProcessW(LPCWSTR, LPWSTR, SECURITY_ATTRIBUTES*, SECURITY_ATTRIBUTES*, BOOL, DWORD, LPVOID, LPCWSTR, STARTUPINFOW*, PROCESS_INFORMATION*);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, LPVOID);
__declspec(dllimport) BOOL WINAPI ReadFile(HANDLE, LPVOID, DWORD, DWORD*, LPVOID);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE);
__declspec(dllimport) DWORD WINAPI WaitForSingleObject(HANDLE, DWORD);
__declspec(dllimport) HANDLE WINAPI CreateThread(SECURITY_ATTRIBUTES*, ULONG_PTR, DWORD (WINAPI *)(LPVOID), LPVOID, DWORD, DWORD*);
__declspec(dllimport) HANDLE WINAPI CreateEventW(SECURITY_ATTRIBUTES*, BOOL, BOOL, LPCWSTR);
__declspec(dllimport) BOOL WINAPI SetEvent(HANDLE);
__declspec(dllimport) void WINAPI Sleep(DWORD);
__declspec(dllimport) HANDLE WINAPI CreateFileW(LPCWSTR, DWORD, DWORD, SECURITY_ATTRIBUTES*, DWORD, DWORD, HANDLE);
__declspec(dllimport) void WINAPI GetLocalTime(SYSTEMTIME*);
__declspec(dllimport) U64 WINAPI GetTickCount64();

__declspec(dllimport) unsigned short WINAPI RegisterClassExW(const WNDCLASSEXW*);
__declspec(dllimport) HWND WINAPI CreateWindowExW(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
__declspec(dllimport) LRESULT WINAPI DefWindowProcW(HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) BOOL WINAPI ShowWindow(HWND, int);
__declspec(dllimport) BOOL WINAPI UpdateWindow(HWND);
__declspec(dllimport) BOOL WINAPI GetMessageW(MSG*, HWND, UINT, UINT);
__declspec(dllimport) BOOL WINAPI TranslateMessage(const MSG*);
__declspec(dllimport) LRESULT WINAPI DispatchMessageW(const MSG*);
__declspec(dllimport) BOOL WINAPI IsDialogMessageW(HWND, MSG*);
__declspec(dllimport) void WINAPI PostQuitMessage(int);
__declspec(dllimport) HDC WINAPI BeginPaint(HWND, PAINTSTRUCT*);
__declspec(dllimport) BOOL WINAPI EndPaint(HWND, const PAINTSTRUCT*);
__declspec(dllimport) BOOL WINAPI InvalidateRect(HWND, const RECT*, BOOL);
__declspec(dllimport) int WINAPI FillRect(HDC, const RECT*, HBRUSH);
__declspec(dllimport) BOOL WINAPI SetWindowTextW(HWND, LPCWSTR);
__declspec(dllimport) int WINAPI GetWindowTextW(HWND, LPWSTR, int);
__declspec(dllimport) int WINAPI GetWindowTextLengthW(HWND);
__declspec(dllimport) BOOL WINAPI EnableWindow(HWND, BOOL);
__declspec(dllimport) int WINAPI MessageBoxW(HWND, LPCWSTR, LPCWSTR, UINT);
__declspec(dllimport) LRESULT WINAPI SendMessageW(HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) HWND WINAPI SetFocus(HWND);
__declspec(dllimport) HWND WINAPI GetFocus();
__declspec(dllimport) short WINAPI GetKeyState(int);
__declspec(dllimport) BOOL WINAPI DestroyWindow(HWND);
__declspec(dllimport) HCURSOR WINAPI LoadCursorW(HINSTANCE, LPCWSTR);
__declspec(dllimport) HICON WINAPI LoadIconW(HINSTANCE, LPCWSTR);
__declspec(dllimport) HBRUSH WINAPI GetSysColorBrush(int);
__declspec(dllimport) BOOL WINAPI PostMessageW(HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) BOOL WINAPI GetClientRect(HWND, RECT*);
__declspec(dllimport) BOOL WINAPI MoveWindow(HWND, int, int, int, int, BOOL);
__declspec(dllimport) int WINAPI GetDlgCtrlID(HWND);
__declspec(dllimport) BOOL WINAPI IsWindowEnabled(HWND);
__declspec(dllimport) BOOL WINAPI SetProcessDPIAware();
__declspec(dllimport) BOOL WINAPI OpenClipboard(HWND);
__declspec(dllimport) BOOL WINAPI CloseClipboard();
__declspec(dllimport) BOOL WINAPI EmptyClipboard();
__declspec(dllimport) HANDLE WINAPI SetClipboardData(UINT, HANDLE);
__declspec(dllimport) HANDLE WINAPI GlobalAlloc(UINT, ULONG_PTR);
__declspec(dllimport) LPVOID WINAPI GlobalLock(HANDLE);
__declspec(dllimport) BOOL WINAPI GlobalUnlock(HANDLE);
__declspec(dllimport) BOOL WINAPI TrackMouseEvent(TRACKMOUSEEVENT*);
__declspec(dllimport) UINT_PTR WINAPI SetTimer(HWND, UINT_PTR, UINT, LPVOID);
__declspec(dllimport) BOOL WINAPI KillTimer(HWND, UINT_PTR);

__declspec(dllimport) HDC WINAPI CreateCompatibleDC(HDC);
__declspec(dllimport) BOOL WINAPI DeleteDC(HDC);
__declspec(dllimport) HBITMAP WINAPI CreateCompatibleBitmap(HDC, int, int);
__declspec(dllimport) BOOL WINAPI BitBlt(HDC, int, int, int, int, HDC, int, int, DWORD);
__declspec(dllimport) int WINAPI SetStretchBltMode(HDC, int);
__declspec(dllimport) BOOL WINAPI StretchBlt(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
__declspec(dllimport) HBRUSH WINAPI CreateSolidBrush(DWORD);
__declspec(dllimport) BOOL WINAPI DeleteObject(HGDIOBJ);
__declspec(dllimport) int WINAPI SetBkMode(HDC, int);
__declspec(dllimport) DWORD WINAPI SetTextColor(HDC, DWORD);
__declspec(dllimport) HFONT WINAPI CreateFontW(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);
__declspec(dllimport) HGDIOBJ WINAPI SelectObject(HDC, HGDIOBJ);
__declspec(dllimport) BOOL WINAPI TextOutW(HDC, int, int, LPCWSTR, int);
__declspec(dllimport) HPEN WINAPI CreatePen(int, int, DWORD);
__declspec(dllimport) BOOL WINAPI MoveToEx(HDC, int, int, POINT*);
__declspec(dllimport) BOOL WINAPI LineTo(HDC, int, int);
__declspec(dllimport) BOOL WINAPI Ellipse(HDC, int, int, int, int);
__declspec(dllimport) BOOL WINAPI Rectangle(HDC, int, int, int, int);
__declspec(dllimport) BOOL WINAPI Polygon(HDC, const POINT*, int);
__declspec(dllimport) BOOL WINAPI RoundRect(HDC, int, int, int, int, int, int);
__declspec(dllimport) int WINAPI DrawTextW(HDC, LPWSTR, int, RECT*, UINT);
__declspec(dllimport) HGDIOBJ WINAPI GetStockObject(int);

}

// --------------------------- CRT-free helper functions ---------------------------
extern "C" void* memcpy(void* d, const void* s, unsigned long long n) {
    BYTE* dd=(BYTE*)d; const BYTE* ss=(const BYTE*)s; for (unsigned long long i=0;i<n;i++) dd[i]=ss[i]; return d;
}
extern "C" void* memset(void* d, int v, unsigned long long n) {
    BYTE* dd=(BYTE*)d; for (unsigned long long i=0;i<n;i++) dd[i]=(BYTE)v; return d;
}
extern "C" int memcmp(const void* a,const void* b,unsigned long long n) {
    const BYTE* aa=(const BYTE*)a; const BYTE* bb=(const BYTE*)b; for(unsigned long long i=0;i<n;i++){if(aa[i]!=bb[i]) return aa[i]<bb[i]?-1:1;} return 0;
}
extern "C" void* memmove(void* d,const void* s,unsigned long long n) {
    BYTE* dd=(BYTE*)d; const BYTE* ss=(const BYTE*)s; if(dd<ss){for(unsigned long long i=0;i<n;i++)dd[i]=ss[i];}else if(dd>ss){for(unsigned long long i=n;i>0;i--)dd[i-1]=ss[i-1];}return d;
}

static int aLen(const char* s){int n=0; if(s) while(s[n]) n++; return n;}
static int wLen(const wchar_t* s){int n=0; if(s) while(s[n]) n++; return n;}
static void aCopy(char* d,int cap,const char* s){if(cap<=0)return;int i=0;while(s&&s[i]&&i<cap-1){d[i]=s[i];i++;}d[i]=0;}
static void wCopy(wchar_t* d,int cap,const wchar_t* s){if(cap<=0)return;int i=0;while(s&&s[i]&&i<cap-1){d[i]=s[i];i++;}d[i]=0;}
static void aAppend(char* d,int cap,const char* s){int n=aLen(d),i=0;while(s&&s[i]&&n<cap-1)d[n++]=s[i++];d[n]=0;}
static void wAppend(wchar_t* d,int cap,const wchar_t* s){int n=wLen(d),i=0;while(s&&s[i]&&n<cap-1)d[n++]=s[i++];d[n]=0;}
static void wAppendAscii(wchar_t* d,int cap,const char* s){int n=wLen(d),i=0;while(s&&s[i]&&n<cap-1)d[n++]=(wchar_t)(unsigned char)s[i++];d[n]=0;}
static void aAppendChar(char* d,int cap,char c){int n=aLen(d);if(n<cap-1){d[n]=c;d[n+1]=0;}}
static void wAppendChar(wchar_t* d,int cap,wchar_t c){int n=wLen(d);if(n<cap-1){d[n]=c;d[n+1]=0;}}
static void aAppendInt(char* d,int cap,int v){char t[16];int n=0;if(v==0){aAppendChar(d,cap,'0');return;}if(v<0){aAppendChar(d,cap,'-');v=-v;}while(v>0&&n<15){t[n++]=(char)('0'+v%10);v/=10;}for(int i=n-1;i>=0;i--)aAppendChar(d,cap,t[i]);}
static void wAppendInt(wchar_t* d,int cap,int v){char t[16]={0};aAppendInt(t,16,v);wAppendAscii(d,cap,t);}
static int aCmp(const char* a,const char* b){int i=0;while(a[i]&&b[i]&&a[i]==b[i])i++;return (unsigned char)a[i]-(unsigned char)b[i];}
static bool aStarts(const char* s,const char* p){int i=0;while(p[i]){if(s[i]!=p[i])return false;i++;}return true;}
static int aFind(const char* s,const char* key){if(!s||!key||!key[0])return -1;for(int i=0;s[i];i++){int j=0;while(key[j]&&s[i+j]==key[j])j++;if(!key[j])return i;}return -1;}
static bool aParseIntAt(const char* s,int pos,int* out){if(!s||!out||pos<0)return false;int sign=1,v=0,n=0;if(s[pos]=='-'){sign=-1;pos++;}else if(s[pos]=='+')pos++;while(s[pos]>='0'&&s[pos]<='9'){v=v*10+(s[pos]-'0');pos++;n++;}if(!n)return false;*out=v*sign;return true;}
static int iAbs(int x){return x<0?-x:x;}
static int iMax(int a,int b){return a>b?a:b;}
static int iMin(int a,int b){return a<b?a:b;}
static DWORD RGBc(int r,int g,int b){return (DWORD)(r | (g<<8) | (b<<16));}
static int lowWord(WPARAM v){return (int)(v&0xFFFF);}
static int signed16(int x){x&=0xFFFF;return (x&0x8000)?x-0x10000:x;}

// --------------------------- Win32 constants ---------------------------
static const DWORD CS_HREDRAW=0x0002, CS_VREDRAW=0x0001;
static const DWORD WS_OVERLAPPEDWINDOW=0x00CF0000, WS_CHILD=0x40000000, WS_VISIBLE=0x10000000, WS_BORDER=0x00800000, WS_VSCROLL=0x00200000, WS_TABSTOP=0x00010000, WS_CLIPCHILDREN=0x02000000;
static const DWORD WS_CAPTION=0x00C00000, WS_SYSMENU=0x00080000, WS_EX_CONTROLPARENT=0x00010000, WS_EX_CLIENTEDGE=0x00000200, ES_AUTOHSCROLL=0x0080;
static const DWORD BS_FLAT=0x00008000;
static const DWORD SS_CENTER=0x00000001, SS_CENTERIMAGE=0x00000200;
static const DWORD ES_MULTILINE=0x0004, ES_AUTOVSCROLL=0x0040, ES_READONLY=0x0800;
static const int SW_SHOW=5, SW_HIDE=0;
static const UINT WM_DESTROY=0x0002, WM_SIZE=0x0005, WM_ENABLE=0x000A, WM_PAINT=0x000F, WM_CLOSE=0x0010, WM_SETICON=0x0080, WM_COMMAND=0x0111, WM_TIMER=0x0113, WM_CTLCOLORSTATIC=0x0138, WM_LBUTTONDOWN=0x0201, WM_LBUTTONUP=0x0202, WM_RBUTTONDOWN=0x0204, WM_MOUSEMOVE=0x0200, WM_MOUSELEAVE=0x02A3, WM_KEYDOWN=0x0100, WM_SETFONT=0x0030, WM_ERASEBKGND=0x0014, WM_GETMINMAXINFO=0x0024, WM_CUT=0x0300, WM_COPY=0x0301, WM_PASTE=0x0302;
static const UINT WM_APP_ENGINE=0x8001;
static const UINT MB_OK=0x00000000, MB_YESNO=0x00000004, MB_YESNOCANCEL=0x00000003, MB_ICONERROR=0x10, MB_ICONWARNING=0x30, MB_ICONINFORMATION=0x40, MB_ICONQUESTION=0x20;
static const int IDYES=6, IDNO=7, IDCANCEL=2;
static const int COLOR_WINDOW=5, TRANSPARENT=1, PS_SOLID=0, NULL_BRUSH=5, NULL_PEN=8;
static const DWORD SRCCOPY=0x00CC0020;
static const int HALFTONE_MODE=4;
static const DWORD CREATE_NO_WINDOW=0x08000000, STARTF_USESHOWWINDOW=0x1, STARTF_USESTDHANDLES=0x100;
static const WORD SW_HIDE_WORD=0;
static const DWORD HANDLE_FLAG_INHERIT=0x1;
static const DWORD INFINITE=0xFFFFFFFFu, WAIT_OBJECT_0=0, WAIT_TIMEOUT=258;
static const DWORD NORMAL_PRIORITY_CLASS=0x20;
static const DWORD GENERIC_READ=0x80000000u, GENERIC_WRITE=0x40000000u, FILE_SHARE_READ=0x00000001u;
static const DWORD CREATE_ALWAYS=2, OPEN_EXISTING=3, FILE_ATTRIBUTE_NORMAL=0x00000080u;
static const HANDLE INVALID_HANDLE_VALUE_CONST=(HANDLE)(long long)-1;
static const int FW_NORMAL=400, DEFAULT_CHARSET=1, OUT_DEFAULT_PRECIS=0, CLIP_DEFAULT_PRECIS=0, CLEARTYPE_QUALITY=5, DEFAULT_PITCH=0;
static const int IDC_ARROW_ID=32512, IDI_APPLICATION_ID=32512, ICON_SMALL=0, ICON_BIG=1, VK_RETURN_KEY=13, VK_SPACE_KEY=32, VK_CONTROL_KEY=17, VK_A_KEY=65, VK_C_KEY=67, VK_V_KEY=86, VK_X_KEY=88;
static const DWORD TME_LEAVE=0x00000002;
static const UINT GMEM_MOVEABLE=0x0002, CF_UNICODETEXT=13;
static const UINT EM_SETMARGINS=0x00D3, EM_SETSEL=0x00B1; static const WPARAM EC_LEFTMARGIN=0x0001, EC_RIGHTMARGIN=0x0002;
static const UINT LB_ADDSTRING=0x0180, LB_RESETCONTENT=0x0184, LB_SETCURSEL=0x0186, LB_GETCURSEL=0x0188;
static const UINT DT_LEFT=0x0000, DT_CENTER=0x0001, DT_RIGHT=0x0002, DT_VCENTER=0x0004, DT_WORDBREAK=0x0010, DT_SINGLELINE=0x0020, DT_END_ELLIPSIS=0x00008000;

// --------------------------- Chess model ---------------------------
struct Move { int from; int to; char promo; BYTE flags; };
static const BYTE MF_CAPTURE=1, MF_EP=2, MF_CASTLE=4;
struct Position {
    char sq[64]; bool whiteToMove; BYTE castle; int ep; int halfmove; int fullmove;
};
struct HistoryEntry { Position before; Move move; char san[20]; };
struct Game {
    Position pos; HistoryEntry hist[512]; int histCount; U64 keys[513]; int keyCount;
};
static Game g_game;

static int fileOf(int s){return s&7;} static int rankOf(int s){return s>>3;}
static bool isWhitePiece(char p){return p>='A'&&p<='Z';}
static bool isBlackPiece(char p){return p>='a'&&p<='z';}
static bool sameColor(char p,bool white){return white?isWhitePiece(p):isBlackPiece(p);}
static bool opponentColor(char p,bool white){return white?isBlackPiece(p):isWhitePiece(p);}
static char upperPiece(char p){return (p>='a'&&p<='z')?(char)(p-'a'+'A'):p;}
static void squareName(int s,char out[3]){out[0]=(char)('a'+fileOf(s));out[1]=(char)('1'+rankOf(s));out[2]=0;}
static int parseSquare2(const char* s){if(!s||s[0]<'a'||s[0]>'h'||s[1]<'1'||s[1]>'8')return -1;return (s[1]-'1')*8+(s[0]-'a');}
static bool moveEq(const Move&a,const Move&b){return a.from==b.from&&a.to==b.to&&a.promo==b.promo;}

static Position initialPosition(){
    Position p; memset(&p,0,sizeof(p));
    const char* r1="RNBQKBNR"; const char* r2="PPPPPPPP"; const char* r7="pppppppp"; const char* r8="rnbqkbnr";
    for(int f=0;f<8;f++){p.sq[f]=r1[f];p.sq[8+f]=r2[f];p.sq[48+f]=r7[f];p.sq[56+f]=r8[f];}
    p.whiteToMove=true;p.castle=1|2|4|8;p.ep=-1;p.halfmove=0;p.fullmove=1;return p;
}

static int findKing(const Position* p,bool white){char k=white?'K':'k';for(int i=0;i<64;i++)if(p->sq[i]==k)return i;return -1;}
static bool rayAttacked(const Position* p,int f,int r,bool byWhite,const int dirs[][2],int dirCount,char a,char b){
    for(int i=0;i<dirCount;i++){int nf=f+dirs[i][0],nr=r+dirs[i][1];while(nf>=0&&nf<8&&nr>=0&&nr<8){char q=p->sq[nr*8+nf];if(q){if(sameColor(q,byWhite)){char u=upperPiece(q);if(u==a||u==b)return true;}break;}nf+=dirs[i][0];nr+=dirs[i][1];}}
    return false;
}
static bool isAttacked(const Position* p,int sq,bool byWhite){
    if(sq<0)return false;int f=fileOf(sq),r=rankOf(sq);
    // Pawn attacks.
    int pr=byWhite?r-1:r+1;
    if(pr>=0&&pr<8){for(int df=-1;df<=1;df+=2){int pf=f+df;if(pf>=0&&pf<8){char q=p->sq[pr*8+pf];if(q==(byWhite?'P':'p'))return true;}}}
    const int kn[8][2]={{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    for(int i=0;i<8;i++){int nf=f+kn[i][0],nr=r+kn[i][1];if(nf>=0&&nf<8&&nr>=0&&nr<8&&p->sq[nr*8+nf]==(byWhite?'N':'n'))return true;}
    for(int dr=-1;dr<=1;dr++)for(int df=-1;df<=1;df++)if(df||dr){int nf=f+df,nr=r+dr;if(nf>=0&&nf<8&&nr>=0&&nr<8&&p->sq[nr*8+nf]==(byWhite?'K':'k'))return true;}
    const int bd[4][2]={{1,1},{1,-1},{-1,1},{-1,-1}}; if(rayAttacked(p,f,r,byWhite,bd,4,'B','Q'))return true;
    const int rd[4][2]={{1,0},{-1,0},{0,1},{0,-1}}; if(rayAttacked(p,f,r,byWhite,rd,4,'R','Q'))return true;
    return false;
}
static bool inCheck(const Position* p,bool white){return isAttacked(p,findKing(p,white),!white);}

static void applyMoveRaw(Position* p,const Move& m){
    bool movingWhite=p->whiteToMove;char pc=p->sq[m.from];char captured=p->sq[m.to];bool isPawn=upperPiece(pc)=='P';bool capture=captured!=0||((m.flags&MF_EP)!=0);
    // Castling rights lost by king/rook movement.
    if(pc=='K')p->castle&=(BYTE)~3; if(pc=='k')p->castle&=(BYTE)~12;
    if(m.from==0)p->castle&=(BYTE)~2; if(m.from==7)p->castle&=(BYTE)~1; if(m.from==56)p->castle&=(BYTE)~8; if(m.from==63)p->castle&=(BYTE)~4;
    // Capturing a rook on its original square also removes that right.
    if(m.to==0&&captured=='R')p->castle&=(BYTE)~2; if(m.to==7&&captured=='R')p->castle&=(BYTE)~1; if(m.to==56&&captured=='r')p->castle&=(BYTE)~8; if(m.to==63&&captured=='r')p->castle&=(BYTE)~4;
    p->sq[m.from]=0;
    if(m.flags&MF_EP){int capSq=movingWhite?m.to-8:m.to+8;p->sq[capSq]=0;}
    p->sq[m.to]=pc;
    if(m.promo){p->sq[m.to]=movingWhite?(char)(m.promo-'a'+'A'):m.promo;}
    if(m.flags&MF_CASTLE){
        if(m.to==6){p->sq[5]='R';p->sq[7]=0;} else if(m.to==2){p->sq[3]='R';p->sq[0]=0;}
        else if(m.to==62){p->sq[61]='r';p->sq[63]=0;} else if(m.to==58){p->sq[59]='r';p->sq[56]=0;}
    }
    p->ep=-1;
    if(isPawn&&iAbs(m.to-m.from)==16)p->ep=(m.to+m.from)/2;
    p->halfmove=(isPawn||capture)?0:p->halfmove+1;
    if(!movingWhite)p->fullmove++;
    p->whiteToMove=!p->whiteToMove;
}

static void addMove(Move* out,int& n,int cap,int from,int to,char promo=0,BYTE flags=0){if(n<cap){out[n].from=from;out[n].to=to;out[n].promo=promo;out[n].flags=flags;n++;}}
static void addSliding(const Position* p,Move*out,int&n,int cap,int from,bool white,const int dirs[][2],int dirCount){
    int f=fileOf(from),r=rankOf(from);for(int i=0;i<dirCount;i++){int nf=f+dirs[i][0],nr=r+dirs[i][1];while(nf>=0&&nf<8&&nr>=0&&nr<8){int to=nr*8+nf;char q=p->sq[to];if(!q)addMove(out,n,cap,from,to);else{if(opponentColor(q,white)&&upperPiece(q)!='K')addMove(out,n,cap,from,to,0,MF_CAPTURE);break;}nf+=dirs[i][0];nr+=dirs[i][1];}}
}
static int pseudoMoves(const Position* p,Move*out,int cap){
    int n=0;bool w=p->whiteToMove;
    for(int from=0;from<64;from++){char pc=p->sq[from];if(!pc||!sameColor(pc,w))continue;char u=upperPiece(pc);int f=fileOf(from),r=rankOf(from);
        if(u=='P'){
            int dir=w?1:-1,start=w?1:6,promoRank=w?7:0;int nr=r+dir;
            if(nr>=0&&nr<8){int to=nr*8+f;if(!p->sq[to]){
                if(nr==promoRank){addMove(out,n,cap,from,to,'q');addMove(out,n,cap,from,to,'r');addMove(out,n,cap,from,to,'b');addMove(out,n,cap,from,to,'n');}
                else {addMove(out,n,cap,from,to);if(r==start){int to2=(r+2*dir)*8+f;if(!p->sq[to2])addMove(out,n,cap,from,to2);}}
            }
            for(int df=-1;df<=1;df+=2){int nf=f+df;if(nf<0||nf>=8)continue;to=nr*8+nf;char q=p->sq[to];bool ep=(to==p->ep&&!q);if((q&&opponentColor(q,w)&&upperPiece(q)!='K')||ep){BYTE fl=MF_CAPTURE|(ep?MF_EP:0);if(nr==promoRank){addMove(out,n,cap,from,to,'q',fl);addMove(out,n,cap,from,to,'r',fl);addMove(out,n,cap,from,to,'b',fl);addMove(out,n,cap,from,to,'n',fl);}else addMove(out,n,cap,from,to,0,fl);}}
            }
        } else if(u=='N'){
            const int d[8][2]={{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};for(int i=0;i<8;i++){int nf=f+d[i][0],nr=r+d[i][1];if(nf<0||nf>=8||nr<0||nr>=8)continue;int to=nr*8+nf;char q=p->sq[to];if(!q)addMove(out,n,cap,from,to);else if(opponentColor(q,w)&&upperPiece(q)!='K')addMove(out,n,cap,from,to,0,MF_CAPTURE);}
        } else if(u=='B'){const int d[4][2]={{1,1},{1,-1},{-1,1},{-1,-1}};addSliding(p,out,n,cap,from,w,d,4);
        } else if(u=='R'){const int d[4][2]={{1,0},{-1,0},{0,1},{0,-1}};addSliding(p,out,n,cap,from,w,d,4);
        } else if(u=='Q'){const int d[8][2]={{1,1},{1,-1},{-1,1},{-1,-1},{1,0},{-1,0},{0,1},{0,-1}};addSliding(p,out,n,cap,from,w,d,8);
        } else if(u=='K'){
            for(int dr=-1;dr<=1;dr++)for(int df=-1;df<=1;df++)if(df||dr){int nf=f+df,nr=r+dr;if(nf<0||nf>=8||nr<0||nr>=8)continue;int to=nr*8+nf;char q=p->sq[to];if(!q)addMove(out,n,cap,from,to);else if(opponentColor(q,w)&&upperPiece(q)!='K')addMove(out,n,cap,from,to,0,MF_CAPTURE);}
            // Castling, including attack checks on start, transit and destination.
            if(w&&from==4&&!inCheck(p,true)){
                if((p->castle&1)&&p->sq[7]=='R'&&!p->sq[5]&&!p->sq[6]&&!isAttacked(p,5,false)&&!isAttacked(p,6,false))addMove(out,n,cap,4,6,0,MF_CASTLE);
                if((p->castle&2)&&p->sq[0]=='R'&&!p->sq[1]&&!p->sq[2]&&!p->sq[3]&&!isAttacked(p,3,false)&&!isAttacked(p,2,false))addMove(out,n,cap,4,2,0,MF_CASTLE);
            } else if(!w&&from==60&&!inCheck(p,false)){
                if((p->castle&4)&&p->sq[63]=='r'&&!p->sq[61]&&!p->sq[62]&&!isAttacked(p,61,true)&&!isAttacked(p,62,true))addMove(out,n,cap,60,62,0,MF_CASTLE);
                if((p->castle&8)&&p->sq[56]=='r'&&!p->sq[57]&&!p->sq[58]&&!p->sq[59]&&!isAttacked(p,59,true)&&!isAttacked(p,58,true))addMove(out,n,cap,60,58,0,MF_CASTLE);
            }
        }
    }
    return n;
}
static int legalMovesFor(const Position* p,Move*out,int cap){
    Move pseudo[256];int pn=pseudoMoves(p,pseudo,256),n=0;bool mover=p->whiteToMove;
    for(int i=0;i<pn;i++){Position q=*p;applyMoveRaw(&q,pseudo[i]);if(!inCheck(&q,mover)&&n<cap)out[n++]=pseudo[i];}
    return n;
}

static bool hasEpCapture(const Position* p){if(p->ep<0)return false;int ef=fileOf(p->ep),er=rankOf(p->ep);bool w=p->whiteToMove;int pawnRank=w?er-1:er+1;char pawn=w?'P':'p';if(pawnRank<0||pawnRank>7)return false;for(int df=-1;df<=1;df+=2){int f=ef+df;if(f>=0&&f<8&&p->sq[pawnRank*8+f]==pawn)return true;}return false;}
static U64 positionKey(const Position* p){
    U64 h=1469598103934665603ULL;for(int i=0;i<64;i++){h^=(BYTE)p->sq[i];h*=1099511628211ULL;}h^=(BYTE)(p->whiteToMove?1:2);h*=1099511628211ULL;h^=p->castle;h*=1099511628211ULL;int ep=hasEpCapture(p)?p->ep:-1;h^=(BYTE)(ep+1);h*=1099511628211ULL;return h;
}
static bool insufficientMaterial(const Position* p){
    int minors=0,bishopCount=0;int bishopColor=-1;
    for(int s=0;s<64;s++){char q=p->sq[s];if(!q)continue;char u=upperPiece(q);if(u=='P'||u=='R'||u=='Q')return false;if(u=='N')minors++;else if(u=='B'){minors++;bishopCount++;int c=(fileOf(s)+rankOf(s))&1;if(bishopColor<0)bishopColor=c;else if(c!=bishopColor)bishopColor=2;}}
    if(minors<=1)return true;if(bishopCount==minors&&bishopColor!=2)return true;return false;
}

static void normalizeSAN(const char* in,char*out,int cap){int n=0;for(int i=0;in&&in[i]&&n<cap-1;i++){char c=in[i];if(c=='0')c='O';if(c=='+'||c=='#')continue;if(c!=' '&&c!='\t'&&c!='\r'&&c!='\n')out[n++]=c;}out[n]=0;}
static void sanForMove(const Position* p,const Move& m,const Move* legal,int legalN,char out[20]){
    out[0]=0;char pc=p->sq[m.from],u=upperPiece(pc);bool capture=(p->sq[m.to]!=0)||((m.flags&MF_EP)!=0);
    if(m.flags&MF_CASTLE){aCopy(out,20,m.to>m.from?"O-O":"O-O-O");}
    else {
        if(u!='P'){
            aAppendChar(out,20,u);bool any=false,sameFile=false,sameRank=false;
            for(int i=0;i<legalN;i++){const Move&o=legal[i];if(o.from==m.from||o.to!=m.to)continue;char op=p->sq[o.from];if(upperPiece(op)==u&&sameColor(op,p->whiteToMove)){any=true;if(fileOf(o.from)==fileOf(m.from))sameFile=true;if(rankOf(o.from)==rankOf(m.from))sameRank=true;}}
            if(any){if(!sameFile)aAppendChar(out,20,(char)('a'+fileOf(m.from)));else if(!sameRank)aAppendChar(out,20,(char)('1'+rankOf(m.from)));else{aAppendChar(out,20,(char)('a'+fileOf(m.from)));aAppendChar(out,20,(char)('1'+rankOf(m.from)));}}
        } else if(capture)aAppendChar(out,20,(char)('a'+fileOf(m.from)));
        if(capture)aAppendChar(out,20,'x');char sn[3];squareName(m.to,sn);aAppend(out,20,sn);
        if(m.promo){aAppendChar(out,20,'=');aAppendChar(out,20,(char)(m.promo-'a'+'A'));}
    }
    Position q=*p;applyMoveRaw(&q,m);if(inCheck(&q,q.whiteToMove)){Move replies[256];int rn=legalMovesFor(&q,replies,256);aAppendChar(out,20,rn==0?'#':'+');}
}

static void gameClockOnCommittedMove(bool moverWhite);
static void gameClockOnUndo(int ply,bool moverWhite);
static void debugQaAfterCommittedMove();
static void gameInit(){g_game.pos=initialPosition();g_game.histCount=0;g_game.keyCount=1;g_game.keys[0]=positionKey(&g_game.pos);}
static bool gamePush(const Move& m,char outSan[20]){
    Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);int idx=-1;for(int i=0;i<n;i++)if(moveEq(legal[i],m)){idx=i;break;}if(idx<0)return false;
    bool moverWhite=g_game.pos.whiteToMove;
    char san[20];sanForMove(&g_game.pos,legal[idx],legal,n,san);HistoryEntry &h=g_game.hist[g_game.histCount];h.before=g_game.pos;h.move=legal[idx];aCopy(h.san,20,san);g_game.histCount++;applyMoveRaw(&g_game.pos,legal[idx]);if(g_game.keyCount<513)g_game.keys[g_game.keyCount++]=positionKey(&g_game.pos);if(outSan)aCopy(outSan,20,san);
    gameClockOnCommittedMove(moverWhite);debugQaAfterCommittedMove();return true;
}
static bool gameUndo(){if(g_game.histCount<=0)return false;int removed=g_game.histCount-1;bool moverWhite=g_game.hist[removed].before.whiteToMove;gameClockOnUndo(removed,moverWhite);g_game.histCount--;g_game.pos=g_game.hist[g_game.histCount].before;if(g_game.keyCount>1)g_game.keyCount--;return true;}
static bool parseManualMove(const char* text,Move*out){
    char in[32];normalizeSAN(text,in,32);Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);
    // UCI fallback such as e2e4 or e7e8q.
    if(aLen(in)>=4){int f=parseSquare2(in),t=parseSquare2(in+2);char pr=0;if(aLen(in)>=5){char c=in[4];if(c>='A'&&c<='Z')c=(char)(c-'A'+'a');pr=c;}if(f>=0&&t>=0){for(int i=0;i<n;i++)if(legal[i].from==f&&legal[i].to==t&&legal[i].promo==pr){*out=legal[i];return true;}}}
    for(int i=0;i<n;i++){char san[20],norm[32];sanForMove(&g_game.pos,legal[i],legal,n,san);normalizeSAN(san,norm,32);if(aCmp(norm,in)==0){*out=legal[i];return true;}}
    return false;
}
static bool findLegalByUCIForPosition(const Position* p,const char* s,Move*out){if(!p||!s||aLen(s)<4)return false;int f=parseSquare2(s),t=parseSquare2(s+2);char pr=0;if(aLen(s)>=5)pr=s[4];Move legal[256];int n=legalMovesFor(p,legal,256);for(int i=0;i<n;i++)if(legal[i].from==f&&legal[i].to==t&&legal[i].promo==pr){*out=legal[i];return true;}return false;}
static bool findLegalByUCI(const char* s,Move*out){return findLegalByUCIForPosition(&g_game.pos,s,out);}
static void moveUCI(const Move&m,char out[6]){char a[3],b[3];squareName(m.from,a);squareName(m.to,b);out[0]=a[0];out[1]=a[1];out[2]=b[0];out[3]=b[1];if(m.promo){out[4]=m.promo;out[5]=0;}else out[4]=0;}

static bool gameEndStatus(wchar_t out[128]){
    Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);if(n==0){if(inCheck(&g_game.pos,g_game.pos.whiteToMove))wCopy(out,128,g_game.pos.whiteToMove?L"Checkmate, Black wins.":L"Checkmate, White wins.");else wCopy(out,128,L"Stalemate, draw.");return true;}
    if(insufficientMaterial(&g_game.pos)){wCopy(out,128,L"Draw by insufficient material.");return true;}
    U64 k=positionKey(&g_game.pos);int c=0;for(int i=0;i<g_game.keyCount;i++)if(g_game.keys[i]==k)c++;if(c>=3){wCopy(out,128,L"Draw can be claimed by threefold repetition.");return true;}
    if(g_game.pos.halfmove>=100){wCopy(out,128,L"Draw can be claimed by the fifty-move rule.");return true;}return false;
}

// --------------------------- Stockfish engine worker ---------------------------
static HWND g_mainHwnd=NULLPTR;
static HANDLE g_engineThread=NULLPTR,g_requestEvent=NULLPTR,g_engineProcess=NULLPTR,g_engineInWrite=NULLPTR,g_engineOutRead=NULLPTR;
static volatile bool g_engineQuit=false;
static volatile int g_requestToken=0;
static volatile int g_requestKind=0; // 1 = suggestion, 2 = evaluation, 3 = review, 4 = practice-opponent move
static volatile int g_requestPracticeElo=900;
static char g_requestMoves[4096];
static char g_engineBest[16];
static int g_engineWdlWin=0, g_engineWdlDraw=1000, g_engineWdlLoss=0;
static int g_engineScoreCp=0, g_engineMate=0;
static bool g_engineHasWdl=false;
static wchar_t g_engineError[512];
static wchar_t g_enginePathBuf[1024];
static char g_engineCmdBuf[4200];
static char g_engineLineBuf[2048];

enum EngineMessageCode { ENG_READY=1, ENG_BEST=2, ENG_ERROR=3, ENG_ANALYSIS_ERROR=4, ENG_EVAL=5, ENG_REVIEW_DONE=6, ENG_PRACTICE=7, ENG_ASSIST=8, ENG_TAG=9, ENG_CHOICES=10, ENG_THREAT=11, ENG_COACH=12 };
enum EngineRequestKind { REQ_SUGGEST=1, REQ_EVAL=2, REQ_REVIEW=3, REQ_PRACTICE=4, REQ_ASSIST=5, REQ_TAG=6, REQ_CHOICES=7, REQ_THREAT=8, REQ_COACH=9 };

// Assisted Mode evaluates the move the user chose before it is committed.
// It only intervenes for a serious evaluation loss or a newly allowed forced mate.
static char g_requestAssistMove[6];
static int g_requestAssistTerminal=200000; // 200000 = non-terminal candidate.
static char g_assistBest[16];
static char g_assistOpponentReply[16];
static int g_assistBaseScore=0,g_assistAfterScore=0;
static char g_engineChoice1[16],g_engineChoice2[16];
// Coach Mode analyses the learner's proposed move before the computer replies.
// It stores the strongest opponent reply and the learner's best follow-up so the
// teaching popup can explain a short, concrete plan in plain English and SAN.
static char g_coachOppReply[16],g_coachUserReply[16];
static int g_coachBeforeScore=0,g_coachAfterScore=0;

// Post-game review data. Stockfish evaluates every position in the finished game
// on the existing worker thread, never on the GUI thread.
static int g_reviewEvalWhite[513];
static int g_reviewPlyCount=0;
static int g_liveEvalWhite[513];
static BYTE g_liveEvalKnown[513];

struct PipeReader { HANDLE h; char buf[4096]; DWORD pos,len; };
static bool pipeReadChar(PipeReader* r,char* c){if(r->pos>=r->len){r->pos=0;r->len=0;DWORD got=0;if(!ReadFile(r->h,r->buf,4096,&got,NULLPTR)||got==0)return false;r->len=got;}*c=r->buf[r->pos++];return true;}
static bool pipeReadLine(PipeReader* r,char* line,int cap){int n=0;char c=0;while(pipeReadChar(r,&c)){if(c=='\r')continue;if(c=='\n'){line[n]=0;return true;}if(n<cap-1)line[n++]=c;}line[n]=0;return n>0;}
static bool engineWrite(const char* s){DWORD wrote=0;int n=aLen(s);return g_engineInWrite&&WriteFile(g_engineInWrite,s,(DWORD)n,&wrote,NULLPTR)&&wrote==(DWORD)n;}
static bool waitExact(PipeReader* r,const char* target){char line[2048];while(pipeReadLine(r,line,2048)){if(aCmp(line,target)==0)return true;}return false;}
static void setEngineErrorAscii(const char* s){g_engineError[0]=0;wAppendAscii(g_engineError,512,s);}
static void buildStockfishPath(wchar_t out[1024]){DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){out[0]=0;return;}int last=-1;for(int i=0;i<(int)n;i++)if(out[i]=='\\'||out[i]=='/')last=i;if(last<0){wCopy(out,1024,L"stockfish.exe");return;}out[last+1]=0;wAppend(out,1024,L"stockfish.exe");}

static void parseEngineInfo(const char* line){
    int k=aFind(line," wdl ");
    if(k>=0){int a=0,b=0,c=0;int p=k+5;if(aParseIntAt(line,p,&a)){while(line[p]&&line[p]!=' ')p++;while(line[p]==' ')p++;if(aParseIntAt(line,p,&b)){while(line[p]&&line[p]!=' ')p++;while(line[p]==' ')p++;if(aParseIntAt(line,p,&c)){g_engineWdlWin=a;g_engineWdlDraw=b;g_engineWdlLoss=c;g_engineHasWdl=true;}}}}
    k=aFind(line," score cp ");if(k>=0){int v=0;if(aParseIntAt(line,k+10,&v)){g_engineScoreCp=v;g_engineMate=0;}}
    k=aFind(line," score mate ");if(k>=0){int v=0;if(aParseIntAt(line,k+12,&v)){g_engineMate=v;}}
}

static void fallbackWdlFromScore(){
    if(g_engineHasWdl)return;
    if(g_engineMate){if(g_engineMate>0){g_engineWdlWin=1000;g_engineWdlDraw=0;g_engineWdlLoss=0;}else{g_engineWdlWin=0;g_engineWdlDraw=0;g_engineWdlLoss=1000;}return;}
    int cp=g_engineScoreCp;int acp=iAbs(cp);int edge=(acp*500)/(acp+650);int win=250,draw=500,loss=250;
    if(cp>=0){win=250+edge;loss=250-edge/2;draw=1000-win-loss;}else{loss=250+edge;win=250-edge/2;draw=1000-win-loss;}
    if(win<0)win=0;if(loss<0)loss=0;if(draw<0)draw=0;int sum=win+draw+loss;if(sum<=0){win=0;draw=1000;loss=0;}else{win=win*1000/sum;draw=draw*1000/sum;loss=1000-win-draw;}
    g_engineWdlWin=win;g_engineWdlDraw=draw;g_engineWdlLoss=loss;
}

static bool engineAnalysePosition(PipeReader* reader,const char* moves,int thinkMs,int* rawOut,char* bestOut,int bestCap){
    char* cmd=g_engineCmdBuf;aCopy(cmd,4200,"position startpos");if(moves&&moves[0]){aAppend(cmd,4200," moves ");aAppend(cmd,4200,moves);}aAppend(cmd,4200,"\n");
    g_engineHasWdl=false;g_engineScoreCp=0;g_engineMate=0;g_engineBest[0]=0;
    char go[64];aCopy(go,64,"go movetime ");aAppendInt(go,64,thinkMs);aAppend(go,64,"\n");
    if(!engineWrite(cmd)||!engineWrite(go))return false;
    char* line=g_engineLineBuf;bool sawBest=false;
    while(pipeReadLine(reader,line,2048)){
        if(aStarts(line,"info "))parseEngineInfo(line);
        if(aStarts(line,"bestmove ")){
            sawBest=true;int i=9,j=0;while(line[i]&&line[i]!=' '&&j<15)g_engineBest[j++]=line[i++];g_engineBest[j]=0;break;
        }
    }
    if(!sawBest)return false;
    int raw=g_engineMate?(g_engineMate>0?100000:-100000):g_engineScoreCp;if(rawOut)*rawOut=raw;
    if(bestOut&&bestCap>0){bestOut[0]=0;if(aCmp(g_engineBest,"(none)")!=0)aCopy(bestOut,bestCap,g_engineBest);}
    return true;
}

static DWORD WINAPI engineThreadProc(LPVOID){
    wchar_t* path=g_enginePathBuf;buildStockfishPath(path);
    SECURITY_ATTRIBUTES sa;sa.nLength=sizeof(sa);sa.lpSecurityDescriptor=NULLPTR;sa.bInheritHandle=TRUE;
    HANDLE childOutRead=NULLPTR,childOutWrite=NULLPTR,childInRead=NULLPTR,childInWrite=NULLPTR;
    if(!CreatePipe(&childOutRead,&childOutWrite,&sa,0)||!SetHandleInformation(childOutRead,HANDLE_FLAG_INHERIT,0)||!CreatePipe(&childInRead,&childInWrite,&sa,0)||!SetHandleInformation(childInWrite,HANDLE_FLAG_INHERIT,0)){
        setEngineErrorAscii("Could not create communication pipes for Stockfish.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ERROR,0);return 0;
    }
    STARTUPINFOW si;memset(&si,0,sizeof(si));si.cb=sizeof(si);si.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;si.wShowWindow=SW_HIDE_WORD;si.hStdInput=childInRead;si.hStdOutput=childOutWrite;si.hStdError=childOutWrite;
    PROCESS_INFORMATION pi;memset(&pi,0,sizeof(pi));
    // The key point: the console Stockfish process is created with no console at all.
    if(!CreateProcessW(path,NULLPTR,NULLPTR,NULLPTR,TRUE,CREATE_NO_WINDOW|NORMAL_PRIORITY_CLASS,NULLPTR,NULLPTR,&si,&pi)){
        CloseHandle(childOutRead);CloseHandle(childOutWrite);CloseHandle(childInRead);CloseHandle(childInWrite);
        wCopy(g_engineError,512,L"Could not start stockfish.exe. Place the official Stockfish executable beside Chess_Trainer.exe and name it stockfish.exe.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ERROR,0);return 0;
    }
    CloseHandle(pi.hThread);CloseHandle(childOutWrite);CloseHandle(childInRead);
    g_engineProcess=pi.hProcess;g_engineOutRead=childOutRead;g_engineInWrite=childInWrite;
    static PipeReader reader;reader.h=childOutRead;reader.pos=reader.len=0;
    if(!engineWrite("uci\n")||!waitExact(&reader,"uciok")||!engineWrite("setoption name Threads value 2\n")||!engineWrite("setoption name Hash value 64\n")||!engineWrite("setoption name UCI_ShowWDL value true\n")||!engineWrite("isready\n")||!waitExact(&reader,"readyok")){
        setEngineErrorAscii("Stockfish started but did not complete the UCI handshake.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ERROR,0);goto done;
    }
    PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_READY,0);
    while(!g_engineQuit){
        WaitForSingleObject(g_requestEvent,INFINITE);if(g_engineQuit)break;int token=g_requestToken;int kind=g_requestKind;
        char localMoves[4096];aCopy(localMoves,4096,g_requestMoves);

        if(kind==REQ_REVIEW){
            // Post-game review is always full-strength regardless of Practice mode.
            engineWrite("setoption name UCI_LimitStrength value false\n");
            engineWrite("setoption name Skill Level value 20\n");
            // Split the finished game into UCI move tokens once, then analyse the
            // starting position and every resulting position at a consistent 55 ms.
            char tokens[512][6];int moveCount=0,src=0;
            while(localMoves[src]&&moveCount<512){
                while(localMoves[src]==' ')src++;if(!localMoves[src])break;int j=0;
                while(localMoves[src]&&localMoves[src]!=' '&&j<5)tokens[moveCount][j++]=localMoves[src++];
                tokens[moveCount][j]=0;while(localMoves[src]&&localMoves[src]!=' ')src++;moveCount++;
            }
            char prefix[4096];prefix[0]=0;bool failed=false;
            for(int posIndex=0;posIndex<=moveCount;posIndex++){
                if(g_engineQuit||token!=g_requestToken){failed=true;break;}
                char* cmd=g_engineCmdBuf;aCopy(cmd,4200,"position startpos");if(prefix[0]){aAppend(cmd,4200," moves ");aAppend(cmd,4200,prefix);}aAppend(cmd,4200,"\n");
                g_engineHasWdl=false;g_engineScoreCp=0;g_engineMate=0;g_engineBest[0]=0;
                if(!engineWrite(cmd)||!engineWrite("go movetime 55\n")){failed=true;break;}
                char* line=g_engineLineBuf;bool gotBest=false;while(pipeReadLine(&reader,line,2048)){
                    if(aStarts(line,"info "))parseEngineInfo(line);
                    if(aStarts(line,"bestmove ")){gotBest=true;break;}
                }
                if(!gotBest){failed=true;break;}
                int raw=g_engineMate?(g_engineMate>0?100000:-100000):g_engineScoreCp;
                g_reviewEvalWhite[posIndex]=(posIndex&1)?-raw:raw;
                if(posIndex<moveCount){if(prefix[0])aAppendChar(prefix,4096,' ');aAppend(prefix,4096,tokens[posIndex]);}
            }
            if(!failed){g_reviewPlyCount=moveCount;PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_REVIEW_DONE,token);}
            continue;
        }

        if(kind==REQ_ASSIST){
            // Full-strength, short two-position comparison. First evaluate the position
            // before the user's intended move and remember Stockfish's safest move.
            engineWrite("setoption name UCI_LimitStrength value false\n");
            engineWrite("setoption name Skill Level value 20\n");
            int baseRaw=0;g_assistBest[0]=0;g_assistOpponentReply[0]=0;
            if(!engineAnalysePosition(&reader,localMoves,280,&baseRaw,g_assistBest,16)||!g_assistBest[0]){
                setEngineErrorAscii("Assisted Mode could not evaluate the current position.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);continue;
            }
            g_assistBaseScore=baseRaw; // Current side to move is the user, so POV is already correct.

            if(g_requestAssistTerminal!=200000){
                g_assistAfterScore=g_requestAssistTerminal;
            }else{
                char withCandidate[4096];aCopy(withCandidate,4096,localMoves);if(withCandidate[0])aAppendChar(withCandidate,4096,' ');aAppend(withCandidate,4096,g_requestAssistMove);
                int afterRaw=0;
                if(!engineAnalysePosition(&reader,withCandidate,320,&afterRaw,g_assistOpponentReply,16)){
                    setEngineErrorAscii("Assisted Mode could not evaluate the intended move.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);continue;
                }
                // After the candidate it is the opponent's turn. UCI scores are from
                // the side-to-move perspective, so invert to retain the user's POV.
                g_assistAfterScore=-afterRaw;
            }
            fallbackWdlFromScore();PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ASSIST,token);continue;
        }

        if(kind==REQ_COACH){
            engineWrite("setoption name UCI_LimitStrength value false\n");
            engineWrite("setoption name Skill Level value 20\n");
            int beforeRaw=0;char ignoredBest[16]={0};
            if(!engineAnalysePosition(&reader,localMoves,300,&beforeRaw,ignoredBest,16)){
                setEngineErrorAscii("Coach Mode could not evaluate the current position.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);continue;
            }
            g_coachBeforeScore=beforeRaw;
            char withCandidate[4096];aCopy(withCandidate,4096,localMoves);if(withCandidate[0])aAppendChar(withCandidate,4096,' ');aAppend(withCandidate,4096,g_requestAssistMove);
            int afterRaw=0;g_coachOppReply[0]=0;g_coachUserReply[0]=0;
            if(!engineAnalysePosition(&reader,withCandidate,380,&afterRaw,g_coachOppReply,16)){
                setEngineErrorAscii("Coach Mode could not evaluate the proposed move.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);continue;
            }
            // After the learner's move it is the computer's turn, so invert the score
            // to keep both values from the learner's point of view.
            g_coachAfterScore=-afterRaw;
            if(g_coachOppReply[0]){
                char withReply[4096];aCopy(withReply,4096,withCandidate);aAppendChar(withReply,4096,' ');aAppend(withReply,4096,g_coachOppReply);
                int responseRaw=0;engineAnalysePosition(&reader,withReply,300,&responseRaw,g_coachUserReply,16);
            }
            fallbackWdlFromScore();PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_COACH,token);continue;
        }

        if(kind==REQ_CHOICES){
            // On-demand Assisted Mode help. Ask unrestricted Stockfish for its top two
            // principal variations, but only when the user explicitly presses Ask Trainer.
            engineWrite("setoption name UCI_LimitStrength value false\n");
            engineWrite("setoption name Skill Level value 20\n");
            engineWrite("setoption name MultiPV value 2\n");
            char* cmd=g_engineCmdBuf;aCopy(cmd,4200,"position startpos");if(localMoves[0]){aAppend(cmd,4200," moves ");aAppend(cmd,4200,localMoves);}aAppend(cmd,4200,"\n");
            g_engineChoice1[0]=0;g_engineChoice2[0]=0;g_engineBest[0]=0;g_engineHasWdl=false;g_engineScoreCp=0;g_engineMate=0;
            bool failed=!engineWrite(cmd)||!engineWrite("go movetime 550\n");char* line=g_engineLineBuf;
            while(!failed&&pipeReadLine(&reader,line,2048)){
                if(aStarts(line,"info ")){
                    int mk=aFind(line," multipv "),pk=aFind(line," pv ");
                    if(pk>=0){int mpv=1;if(mk>=0){int parsed=1;aParseIntAt(line,mk+9,&parsed);mpv=parsed;}int p=pk+4,j=0;char mv[16]={0};while(line[p]&&line[p]!=' '&&j<15)mv[j++]=line[p++];mv[j]=0;if(j>=4){if(mpv==1)aCopy(g_engineChoice1,16,mv);else if(mpv==2)aCopy(g_engineChoice2,16,mv);}}
                }
                if(aStarts(line,"bestmove ")){int i=9,j=0;while(line[i]&&line[i]!=' '&&j<15)g_engineBest[j++]=line[i++];g_engineBest[j]=0;break;}
            }
            engineWrite("setoption name MultiPV value 1\n");
            if(!g_engineChoice1[0]&&g_engineBest[0]&&aCmp(g_engineBest,"(none)")!=0)aCopy(g_engineChoice1,16,g_engineBest);
            if(failed||!g_engineChoice1[0]){setEngineErrorAscii("Stockfish could not produce trainer suggestions.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);continue;}
            PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_CHOICES,token);continue;
        }

        // Full Trainer uses unrestricted Stockfish. Human Practice deliberately uses
        // Stockfish's built-in Skill Level only for the synthetic offline opponent.
        // Fair Play mode never requests a live engine move at all.
        if(kind==REQ_PRACTICE){
            int elo=g_requestPracticeElo;if(elo<800)elo=800;if(elo>2200)elo=2200;
            int skill=(elo-800)*20/1400;if(skill<0)skill=0;if(skill>20)skill=20;
            char opt[96];aCopy(opt,96,"setoption name UCI_LimitStrength value false\n");engineWrite(opt);
            aCopy(opt,96,"setoption name Skill Level value ");aAppendInt(opt,96,skill);aAppend(opt,96,"\n");engineWrite(opt);
        }else{
            engineWrite("setoption name UCI_LimitStrength value false\n");
            engineWrite("setoption name Skill Level value 20\n");
        }
        char* cmd=g_engineCmdBuf;aCopy(cmd,4200,"position startpos");if(localMoves[0]){aAppend(cmd,4200," moves ");aAppend(cmd,4200,localMoves);}aAppend(cmd,4200,"\n");
        g_engineHasWdl=false;g_engineScoreCp=0;g_engineMate=0;g_engineBest[0]=0;
        const char* goCmd=kind==REQ_SUGGEST?"go movetime 650\n":kind==REQ_PRACTICE?"go movetime 350\n":kind==REQ_THREAT?"go movetime 500\n":kind==REQ_TAG?"go movetime 120\n":"go movetime 180\n";
        if(!engineWrite(cmd)||!engineWrite(goCmd)){setEngineErrorAscii("Could not send the analysis command to Stockfish.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);continue;}
        char* line=g_engineLineBuf;bool gotBest=false;while(pipeReadLine(&reader,line,2048)){
            if(aStarts(line,"info "))parseEngineInfo(line);
            if(aStarts(line,"bestmove ")){int i=9,j=0;while(line[i]&&line[i]!=' '&&j<15)g_engineBest[j++]=line[i++];g_engineBest[j]=0;gotBest=(j>=4&&aCmp(g_engineBest,"(none)")!=0);break;}
        }
        fallbackWdlFromScore();
        if(kind==REQ_EVAL){PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_EVAL,token);}
        else if(kind==REQ_TAG){PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_TAG,token);}
        else if(kind==REQ_THREAT){PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_THREAT,token);}
        else if(!gotBest){setEngineErrorAscii("Stockfish did not return a usable move.");PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_ANALYSIS_ERROR,token);}
        else if(kind==REQ_PRACTICE)PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_PRACTICE,token);
        else PostMessageW(g_mainHwnd,WM_APP_ENGINE,ENG_BEST,token);
    }
    engineWrite("quit\n");
done:
    if(g_engineInWrite){CloseHandle(g_engineInWrite);g_engineInWrite=NULLPTR;}if(g_engineOutRead){CloseHandle(g_engineOutRead);g_engineOutRead=NULLPTR;}
    if(g_engineProcess){WaitForSingleObject(g_engineProcess,1200);CloseHandle(g_engineProcess);g_engineProcess=NULLPTR;}
    return 0;
}

static void diagEngineRequest(int kind,int token);
static void requestAnalysis(int token,int kind){
    g_requestMoves[0]=0;for(int i=0;i<g_game.histCount;i++){char u[6];moveUCI(g_game.hist[i].move,u);if(i)aAppendChar(g_requestMoves,4096,' ');aAppend(g_requestMoves,4096,u);}g_requestKind=kind;g_requestToken=token;diagEngineRequest(kind,token);SetEvent(g_requestEvent);
}
static void requestAssistAnalysis(int token,const Move& candidate,int terminalScore){
    g_requestMoves[0]=0;for(int i=0;i<g_game.histCount;i++){char u[6];moveUCI(g_game.hist[i].move,u);if(i)aAppendChar(g_requestMoves,4096,' ');aAppend(g_requestMoves,4096,u);}
    moveUCI(candidate,g_requestAssistMove);g_requestAssistTerminal=terminalScore;g_requestKind=REQ_ASSIST;g_requestToken=token;diagEngineRequest(REQ_ASSIST,token);SetEvent(g_requestEvent);
}
static void requestCoachAnalysis(int token,const Move& candidate){
    g_requestMoves[0]=0;for(int i=0;i<g_game.histCount;i++){char u[6];moveUCI(g_game.hist[i].move,u);if(i)aAppendChar(g_requestMoves,4096,' ');aAppend(g_requestMoves,4096,u);}
    moveUCI(candidate,g_requestAssistMove);g_requestKind=REQ_COACH;g_requestToken=token;diagEngineRequest(REQ_COACH,token);SetEvent(g_requestEvent);
}

// --------------------------- Application state and UI ---------------------------
enum {ID_WHITE=1001,ID_BLACK,ID_HISTORY,ID_SAN,ID_ENTER,ID_NEW,ID_FLIP,ID_UNDO,ID_CONFIRM,ID_DIFFERENT,ID_CORRECT,ID_MODE_FULL,ID_MODE_ASSISTED,ID_MODE_PRACTICE,ID_MODE_FAIRPLAY,ID_MODE_COACH,ID_ASSIST_FULL_ON,ID_ASSIST_FULL_OFF,ID_LOG,ID_COPY_PANEL,ID_PROFILE,ID_END_OPP_RESIGN=3501,ID_END_YOU_RESIGN,ID_END_OPP_ABANDON,ID_END_YOU_ABANDON,ID_END_OPP_TIMEOUT,ID_END_YOU_TIMEOUT,ID_END_DRAW,ID_END_CANCEL};
// Layout values are recalculated on every resize. Board size is always a multiple of 8,
// so squares remain crisp and perfectly aligned at every supported window size.
static int BOARD_X=18,BOARD_Y=84,BOARD_SIZE=704,SQ=88;
static int PANEL_X=742,PANEL_W=380,EVAL_X=758,EVAL_Y=122,EVAL_W=348,EVAL_H=26;
static int PANEL_TOP=84,PANEL_BOTTOM=788;
static int ANALYSIS_HEAD_Y=100,GAMELOG_HEAD_Y=230,ENTER_HEAD_Y=570,HISTORY_Y=266,HISTORY_H=290;
static int CLIENT_W=1160,CLIENT_H=860;
static const int HEADER_H=58, STATUS_H=32, FOOTER_H=26, MARGIN=14, CONTENT_GAP=16;
static HINSTANCE g_hInstance=NULLPTR;
static HWND hWhite=NULLPTR,hBlack=NULLPTR,hProfile=NULLPTR,hModeFull=NULLPTR,hModeAssisted=NULLPTR,hModePractice=NULLPTR,hModeFair=NULLPTR,hModeCoach=NULLPTR,hAssistFullOn=NULLPTR,hAssistFullOff=NULLPTR,hLog=NULLPTR,hCopyPanel=NULLPTR,hHistory=NULLPTR,hSan=NULLPTR,hEnter=NULLPTR,hNew=NULLPTR,hFlip=NULLPTR,hUndo=NULLPTR,hConfirm=NULLPTR,hDifferent=NULLPTR,hCorrect=NULLPTR,hStatus=NULLPTR;
static HWND hEndGame[8]={NULLPTR,NULLPTR,NULLPTR,NULLPTR,NULLPTR,NULLPTR,NULLPTR,NULLPTR};
static HFONT g_uiFont=NULLPTR,g_pieceFont=NULLPTR,g_smallFont=NULLPTR,g_titleFont=NULLPTR,g_headingFont=NULLPTR,g_statusFont=NULLPTR,g_footerFont=NULLPTR,g_monoFont=NULLPTR,g_monoBoldFont=NULLPTR,g_percentFont=NULLPTR,g_coordFont=NULLPTR,g_buttonFont=NULLPTR,g_actionButtonFont=NULLPTR;
static HBRUSH g_lightBrush=NULLPTR,g_darkBrush=NULLPTR,g_bgBrush=NULLPTR,g_cardBrush=NULLPTR;
static bool g_engineReady=false,g_userSet=false,g_userWhite=true,g_flipped=false,g_calculating=false,g_suggestionPending=false,g_gameOver=false,g_correctionMode=false;
static HWND g_hoverButton=NULLPTR; static int g_hoverSection=0; static int g_actionPhase=-1; static int g_completedInsightsVisibilityState=-1;
static Move g_suggested;static int g_selected=-1,g_positionVersion=1;
static bool g_evalValid=false;static int g_whiteWin=0,g_drawChance=1000,g_blackWin=0;
static wchar_t g_historyText[8192];

// Operating modes. Full Trainer preserves the original live coach. Assisted Mode
// records the user's real move first; if that move is a serious blunder, the trainer
// enters a two-turn Recovery Coach sequence and supplies the best move on each of the
// user's next two turns. Human Practice is an offline synthetic opponent. Fair Play
// records without live engine guidance.
enum TrainerMode { MODE_FULL=0, MODE_PRACTICE=1, MODE_FAIRPLAY=2, MODE_ASSISTED=3, MODE_COACH=4 };
static int g_trainerMode=MODE_FULL;
// The selected/base mode is deliberately separate from the temporary operational mode.
// Full Assist may borrow Full Trainer behaviour, but the game remains an Assisted Mode game.
static int g_gameBaseMode=MODE_FULL;
// V5.7 session identity is declared with the live game state so QA logging can include it.
static bool g_guestProfile=false;static DWORD g_activeProfileId=0;
static int g_practiceElo=900; // Approximate practice strength, mapped to Stockfish Skill Level.
static Move g_assistPendingMove;
static bool g_assistPending=false;
static int g_assistRecoveryRemaining=0;   // Best-move suggestions still to provide after a detected blunder.
static int g_assistRecoverySequence=0;    // Counts recovery sequences for clean status messaging.
// Per-ply training provenance. g_assistAccepted is retained for archive compatibility,
// but now means any accepted trainer-supplied user move in every training mode.
enum TrainingSource { TRAIN_NONE=0, TRAIN_INDEPENDENT=1, TRAIN_FULL_TRAINER=2, TRAIN_ASK=3, TRAIN_RECOVERY=4, TRAIN_EMERGENCY=5, TRAIN_FULL_ASSIST=6, TRAIN_COACH_INDEPENDENT=7 };
static BYTE g_assistAccepted[512];
static BYTE g_trainingSource[512];
// Rejections and Coach reviews are interaction counters rather than committed-ply counters.
static int g_trainingRejectedTotal=0,g_trainingAskRejected=0,g_trainingRecoveryRejected=0,g_trainingEmergencyRejected=0,g_trainingFullAssistRejected=0,g_trainingFullTrainerRejected=0,g_trainingCoachReviewed=0;
static int g_fullAssistOnCount=0,g_fullAssistOffCount=0;
static bool g_choiceRequestPending=false;       // Waiting for two explicit on-demand choices.
static bool g_onDemandSuggestion=false;         // Current pending suggestion came from Ask Trainer.
enum AssistGuideKind { GUIDE_NONE=0, GUIDE_RECOVERY=1, GUIDE_FULL=2, GUIDE_EMERGENCY=3, GUIDE_ASK=4 };
static bool g_assistFull=false;                  // User-controlled Full Assist inside Assisted Mode.
static bool g_assistEmergency=false;             // Bounded Emergency Rescue course-correction burst.
static int g_assistEmergencyRemaining=0;           // Automatic rescue suggestions left in the current burst.
static int g_assistEmergencyUsed=0;                // Rescue suggestions already supplied in this burst.
static int g_assistEmergencyExtensions=0;          // Extra forced-mate-only suggestions beyond the normal three.
static const int ASSIST_EMERGENCY_BASE_MOVES=3;
static const int ASSIST_EMERGENCY_MAX_EXTRA=2;
static bool g_threatCheckPending=false;
static bool g_assistEmergencyLatched=false;       // Prevents re-arming from the same continuous danger episode.
static int g_assistDangerClearScans=0;            // Two genuinely safer opponent scans are required before re-arming.

// Temporary board explanation overlay for Assisted Mode move-quality coaching.
static bool g_moveExplainActive=false;
static Move g_moveExplainPlayed;
static Move g_moveExplainBetter;
static Move g_moveExplainReply;
static bool g_moveExplainHasBetter=false,g_moveExplainHasReply=false;
static int g_moveExplainClass=0;
static int g_moveExplainLoss=0;
static wchar_t g_moveExplainText[620];
static int g_requestGuideKind=GUIDE_NONE;        // Why the current best-move request was launched.
static int g_suggestionGuideKind=GUIDE_NONE;     // Source of the suggestion currently shown on the board.
static const int ASSIST_BLUNDER_CP=220;          // Arm Recovery Coach at roughly 2.2 pawns or worse.
static const int ASSIST_EMERGENCY_CP=-600;       // Take over when the user's position is critically losing.
static Move g_coachPendingMove;
static bool g_coachPending=false;
static int g_coachElo=1200;
static const wchar_t* trainerModeName(int mode){return mode==MODE_ASSISTED?L"Assisted Mode":mode==MODE_PRACTICE?L"Human Practice":mode==MODE_FAIRPLAY?L"Fair Play / Record Only":mode==MODE_COACH?L"Coach Mode":L"Full Trainer";}

// --------------------------- Local diagnostic log ---------------------------
// V5.5 keeps a bounded in-memory text log and mirrors it to Chess_Trainer_Debug_Log.txt
// beside the executable. No network, telemetry, upload or background service is used.
static char g_debugLogBuffer[131072];
static int g_debugLogLen=0;
static bool g_debugLogReady=false;

// V5.6.5 runtime QA, game-time and universal training-dependency instrumentation. Times are measured by Chess
// Trainer between recorded moves and attributed to the side making that move.
static U64 g_gameClockStartMs=0,g_turnClockStartMs=0,g_userClockMs=0,g_opponentClockMs=0;
static U64 g_plyClockMs[513];
static bool g_gameClockActive=false;
static U64 g_engineDiagStartMs=0,g_engineDiagTotalMs=0,g_engineDiagMaxMs=0;
static int g_engineDiagKind=0,g_engineDiagToken=0,g_engineDiagRequests=0,g_engineDiagResults=0,g_engineDiagErrors=0,g_diagStateWarnings=0;
static void buildDebugLogPath(wchar_t out[1024]){
    DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){wCopy(out,1024,L"Chess_Trainer_Debug_Log.txt");return;}
    int last=-1;for(int i=0;out[i];i++)if(out[i]=='\\'||out[i]=='/')last=i;
    if(last<0){wCopy(out,1024,L"Chess_Trainer_Debug_Log.txt");return;}out[last+1]=0;wAppend(out,1024,L"Chess_Trainer_Debug_Log.txt");
}
static void debugFlush(){
    if(!g_debugLogReady)return;wchar_t path[1024];buildDebugLogPath(path);HANDLE h=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,NULLPTR,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULLPTR);if(h==INVALID_HANDLE_VALUE_CONST)return;DWORD wrote=0;WriteFile(h,g_debugLogBuffer,(DWORD)g_debugLogLen,&wrote,NULLPTR);CloseHandle(h);
}
static void debugAppendRaw(const char* level,const char* msg){
    if(!g_debugLogReady)return;SYSTEMTIME st;GetLocalTime(&st);char line[1200]={0};aAppendChar(line,1200,'[');aAppendInt(line,1200,st.wYear);aAppendChar(line,1200,'-');if(st.wMonth<10)aAppendChar(line,1200,'0');aAppendInt(line,1200,st.wMonth);aAppendChar(line,1200,'-');if(st.wDay<10)aAppendChar(line,1200,'0');aAppendInt(line,1200,st.wDay);aAppendChar(line,1200,' ');if(st.wHour<10)aAppendChar(line,1200,'0');aAppendInt(line,1200,st.wHour);aAppendChar(line,1200,':');if(st.wMinute<10)aAppendChar(line,1200,'0');aAppendInt(line,1200,st.wMinute);aAppendChar(line,1200,':');if(st.wSecond<10)aAppendChar(line,1200,'0');aAppendInt(line,1200,st.wSecond);aAppend(line,1200,"] [");aAppend(line,1200,level?level:"INFO");aAppend(line,1200,"] ");aAppend(line,1200,msg?msg:"");aAppend(line,1200,"\r\n");int n=aLen(line);if(g_debugLogLen+n>126000){const char* roll="\r\n--- log buffer rolled over; newest session data follows ---\r\n";g_debugLogLen=0;memset(g_debugLogBuffer,0,sizeof(g_debugLogBuffer));aCopy(g_debugLogBuffer,(int)sizeof(g_debugLogBuffer),roll);g_debugLogLen=aLen(g_debugLogBuffer);}if(g_debugLogLen+n<(int)sizeof(g_debugLogBuffer)-1){memcpy(g_debugLogBuffer+g_debugLogLen,line,n);g_debugLogLen+=n;g_debugLogBuffer[g_debugLogLen]=0;}debugFlush();
}
static void debugAppendWide(const char* level,const wchar_t* ws){char a[900]={0};int j=0;for(int i=0;ws&&ws[i]&&j<898;i++){wchar_t c=ws[i];a[j++]=(c>=32&&c<127)?(char)c:'?';}a[j]=0;debugAppendRaw(level,a);}

struct TrainingCounters {
    int playerMoves,independentMoves,assistedMoves,rejectedSuggestions,dependencyPercent,independentPercent;
    int askAccepted,askRejected,recoveryAccepted,recoveryRejected,emergencyAccepted,emergencyRejected;
    int fullAssistMoves,fullAssistRejected,fullAssistOnCount,fullAssistOffCount,fullTrainerAccepted,fullTrainerRejected,coachReviewed;
};
static bool trainingSourceIsAssisted(int source){return source==TRAIN_FULL_TRAINER||source==TRAIN_ASK||source==TRAIN_RECOVERY||source==TRAIN_EMERGENCY||source==TRAIN_FULL_ASSIST;}
static const char* trainingSourceName(int source){switch(source){case TRAIN_INDEPENDENT:return "independent";case TRAIN_FULL_TRAINER:return "full-trainer";case TRAIN_ASK:return "ask-trainer";case TRAIN_RECOVERY:return "recovery";case TRAIN_EMERGENCY:return "emergency";case TRAIN_FULL_ASSIST:return "full-assist";case TRAIN_COACH_INDEPENDENT:return "coach-reviewed";}return "none";}
static int trainingSourceFromGuide(int guide,bool onDemand){if(onDemand||guide==GUIDE_ASK)return TRAIN_ASK;if(guide==GUIDE_EMERGENCY)return TRAIN_EMERGENCY;if(guide==GUIDE_FULL)return TRAIN_FULL_ASSIST;return TRAIN_RECOVERY;}
static void resetTrainingTracking(){memset(g_trainingSource,0,sizeof(g_trainingSource));memset(g_assistAccepted,0,sizeof(g_assistAccepted));g_trainingRejectedTotal=0;g_trainingAskRejected=0;g_trainingRecoveryRejected=0;g_trainingEmergencyRejected=0;g_trainingFullAssistRejected=0;g_trainingFullTrainerRejected=0;g_trainingCoachReviewed=0;g_fullAssistOnCount=0;g_fullAssistOffCount=0;}
static void markTrainingPly(int ply,int source){
    if(ply<0||ply>=512)return;g_trainingSource[ply]=(BYTE)source;g_assistAccepted[ply]=trainingSourceIsAssisted(source)?1:0;
    char line[300]={0};aAppend(line,300,trainingSourceIsAssisted(source)?"assisted move accepted":"independent move accepted");aAppend(line,300," source=");aAppend(line,300,trainingSourceName(source));aAppend(line,300," ply=");aAppendInt(line,300,ply+1);debugAppendRaw("TRAINING",line);
}
static void unmarkTrainingPly(int ply){if(ply>=0&&ply<512){g_trainingSource[ply]=TRAIN_NONE;g_assistAccepted[ply]=0;}}
static void recordSuggestionRejected(int source){
    g_trainingRejectedTotal++;if(source==TRAIN_ASK)g_trainingAskRejected++;else if(source==TRAIN_RECOVERY)g_trainingRecoveryRejected++;else if(source==TRAIN_EMERGENCY)g_trainingEmergencyRejected++;else if(source==TRAIN_FULL_ASSIST)g_trainingFullAssistRejected++;else if(source==TRAIN_FULL_TRAINER)g_trainingFullTrainerRejected++;
    char line[320]={0};aAppend(line,320,"suggestion rejected source=");aAppend(line,320,trainingSourceName(source));aAppend(line,320," rejected_total=");aAppendInt(line,320,g_trainingRejectedTotal);debugAppendRaw("TRAINING",line);
}
static void recordCoachReviewed(){g_trainingCoachReviewed++;char line[220]={0};aAppend(line,220,"coach-reviewed move count=");aAppendInt(line,220,g_trainingCoachReviewed);debugAppendRaw("TRAINING",line);}
static void currentTrainingCounters(TrainingCounters* out);
static void currentAssistanceMetrics(int* userMoves,int* assistedMoves,int* independentMoves,int* independentPercent);

static const char* requestKindName(int kind){
    switch(kind){
        case REQ_SUGGEST:return "suggest"; case REQ_EVAL:return "eval"; case REQ_REVIEW:return "review";
        case REQ_PRACTICE:return "practice"; case REQ_ASSIST:return "assist"; case REQ_TAG:return "tag";
        case REQ_CHOICES:return "choices"; case REQ_THREAT:return "threat"; case REQ_COACH:return "coach";
    }return "unknown";
}
static void formatDurationMs(U64 ms,wchar_t out[32]){
    out[0]=0;U64 total=ms/1000ULL;int sec=(int)(total%60ULL),min=(int)((total/60ULL)%60ULL);U64 hrs=total/3600ULL;
    if(hrs>0){wAppendInt(out,32,(int)hrs);wAppend(out,32,L":");if(min<10)wAppend(out,32,L"0");wAppendInt(out,32,min);wAppend(out,32,L":");if(sec<10)wAppend(out,32,L"0");wAppendInt(out,32,sec);}
    else{wAppendInt(out,32,min);wAppend(out,32,L":");if(sec<10)wAppend(out,32,L"0");wAppendInt(out,32,sec);}
}
static void currentGameClock(U64* userMs,U64* opponentMs,U64* totalMs){
    U64 u=g_userClockMs,o=g_opponentClockMs;if(g_gameClockActive&&g_userSet){U64 now=GetTickCount64();U64 d=now>=g_turnClockStartMs?now-g_turnClockStartMs:0;if(g_game.pos.whiteToMove==g_userWhite)u+=d;else o+=d;}
    if(userMs)*userMs=u;if(opponentMs)*opponentMs=o;if(totalMs)*totalMs=u+o;
}
static void gameClockReset(){g_gameClockStartMs=0;g_turnClockStartMs=0;g_userClockMs=0;g_opponentClockMs=0;g_gameClockActive=false;memset(g_plyClockMs,0,sizeof(g_plyClockMs));}
static void diagGameReset(){g_engineDiagStartMs=0;g_engineDiagTotalMs=0;g_engineDiagMaxMs=0;g_engineDiagKind=0;g_engineDiagToken=0;g_engineDiagRequests=0;g_engineDiagResults=0;g_engineDiagErrors=0;g_diagStateWarnings=0;}
static void gameClockStart(){
    gameClockReset();diagGameReset();g_gameClockStartMs=GetTickCount64();g_turnClockStartMs=g_gameClockStartMs;g_gameClockActive=true;
    char line[220]={0};aAppend(line,220,"clock started; user_side=");aAppend(line,220,g_userWhite?"white":"black");aAppend(line,220," side_to_move=white");debugAppendRaw("TIME",line);
}
static void gameClockOnCommittedMove(bool moverWhite){
    if(!g_gameClockActive||!g_userSet)return;U64 now=GetTickCount64(),d=now>=g_turnClockStartMs?now-g_turnClockStartMs:0;bool userMove=(moverWhite==g_userWhite);
    if(userMove)g_userClockMs+=d;else g_opponentClockMs+=d;int ply=g_game.histCount-1;if(ply>=0&&ply<513)g_plyClockMs[ply]=d;g_turnClockStartMs=now;
    char line[340]={0};aAppend(line,340,"ply=");aAppendInt(line,340,g_game.histCount);aAppend(line,340," mover=");aAppend(line,340,userMove?"user":"opponent");aAppend(line,340," turn_ms=");aAppendInt(line,340,(int)(d>2147483647ULL?2147483647ULL:d));aAppend(line,340," user_total_ms=");aAppendInt(line,340,(int)(g_userClockMs>2147483647ULL?2147483647ULL:g_userClockMs));aAppend(line,340," opponent_total_ms=");aAppendInt(line,340,(int)(g_opponentClockMs>2147483647ULL?2147483647ULL:g_opponentClockMs));debugAppendRaw("TIME-MOVE",line);
}
static void gameClockOnUndo(int ply,bool moverWhite){
    if(g_gameClockActive)g_turnClockStartMs=GetTickCount64();if(ply>=0&&ply<513){U64 d=g_plyClockMs[ply];bool userMove=(moverWhite==g_userWhite);if(userMove){if(g_userClockMs>=d)g_userClockMs-=d;else g_userClockMs=0;}else{if(g_opponentClockMs>=d)g_opponentClockMs-=d;else g_opponentClockMs=0;}g_plyClockMs[ply]=0;}
    debugAppendRaw("TIME","undo/correction removed the recorded ply time and restarted the active turn timer.");
}
static void gameClockStop(){
    if(!g_gameClockActive)return;U64 now=GetTickCount64(),d=now>=g_turnClockStartMs?now-g_turnClockStartMs:0;if(g_userSet){if(g_game.pos.whiteToMove==g_userWhite)g_userClockMs+=d;else g_opponentClockMs+=d;}g_turnClockStartMs=now;g_gameClockActive=false;
    U64 total=g_userClockMs+g_opponentClockMs;char line[340]={0};aAppend(line,340,"clock stopped; total_ms=");aAppendInt(line,340,(int)(total>2147483647ULL?2147483647ULL:total));aAppend(line,340," user_ms=");aAppendInt(line,340,(int)(g_userClockMs>2147483647ULL?2147483647ULL:g_userClockMs));aAppend(line,340," opponent_ms=");aAppendInt(line,340,(int)(g_opponentClockMs>2147483647ULL?2147483647ULL:g_opponentClockMs));debugAppendRaw("TIME",line);
}
static void diagEngineRequest(int kind,int token){
    g_engineDiagStartMs=GetTickCount64();g_engineDiagKind=kind;g_engineDiagToken=token;g_engineDiagRequests++;char line[220]={0};aAppend(line,220,"request=");aAppend(line,220,requestKindName(kind));aAppend(line,220," token=");aAppendInt(line,220,token);aAppend(line,220," ply=");aAppendInt(line,220,g_game.histCount);debugAppendRaw("ENGINE-REQ",line);
}
static void diagEngineResult(int code,int token){
    if(code==ENG_READY||code==ENG_ERROR)return;U64 now=GetTickCount64();if(token!=g_engineDiagToken||!g_engineDiagStartMs){char stale[220]={0};aAppend(stale,220,"result_code=");aAppendInt(stale,220,code);aAppend(stale,220," token=");aAppendInt(stale,220,token);aAppend(stale,220," expected_token=");aAppendInt(stale,220,g_engineDiagToken);debugAppendRaw("ENGINE-PERF-STALE",stale);return;}
    U64 d=now>=g_engineDiagStartMs?now-g_engineDiagStartMs:0;g_engineDiagResults++;g_engineDiagTotalMs+=d;if(d>g_engineDiagMaxMs)g_engineDiagMaxMs=d;if(code==ENG_ANALYSIS_ERROR)g_engineDiagErrors++;
    char line[300]={0};aAppend(line,300,"request=");aAppend(line,300,requestKindName(g_engineDiagKind));aAppend(line,300," token=");aAppendInt(line,300,token);aAppend(line,300," result_code=");aAppendInt(line,300,code);aAppend(line,300," latency_ms=");aAppendInt(line,300,(int)(d>2147483647ULL?2147483647ULL:d));if(d>5000&&g_engineDiagKind!=REQ_REVIEW)aAppend(line,300," warning=slow");debugAppendRaw("ENGINE-PERF",line);g_engineDiagStartMs=0;
}
static void debugQaAfterCommittedMove(){
    if(!g_debugLogReady)return;int wk=0,bk=0;for(int i=0;i<64;i++){if(g_game.pos.sq[i]=='K')wk++;else if(g_game.pos.sq[i]=='k')bk++;}Move lm[256];int legal=legalMovesFor(&g_game.pos,lm,256);bool ok=(wk==1&&bk==1&&g_game.histCount>=0&&g_game.histCount<=512&&g_game.keyCount==g_game.histCount+1);
    char line[460]={0};aAppend(line,460,"ply=");aAppendInt(line,460,g_game.histCount);aAppend(line,460," side_to_move=");aAppend(line,460,g_game.pos.whiteToMove?"white":"black");aAppend(line,460," legal_moves=");aAppendInt(line,460,legal);aAppend(line,460," white_kings=");aAppendInt(line,460,wk);aAppend(line,460," black_kings=");aAppendInt(line,460,bk);aAppend(line,460," key_count=");aAppendInt(line,460,g_game.keyCount);aAppend(line,460," hist_count=");aAppendInt(line,460,g_game.histCount);aAppend(line,460," invariant=");aAppend(line,460,ok?"PASS":"FAIL");debugAppendRaw("QA-MOVE",line);if(!ok){g_diagStateWarnings++;debugAppendRaw("STATE-WARN","Core board/history invariant failed after committed move.");}
}
static U64 perftQa(const Position* p,int depth){if(depth<=0)return 1;Move mv[256];int n=legalMovesFor(p,mv,256);if(depth==1)return (U64)n;U64 nodes=0;for(int i=0;i<n;i++){Position q=*p;applyMoveRaw(&q,mv[i]);nodes+=perftQa(&q,depth-1);}return nodes;}
static void runExtendedSelfTest(){
    Position p=initialPosition();U64 p1=perftQa(&p,1),p2=perftQa(&p,2),p3=perftQa(&p,3),p4=perftQa(&p,4);bool ok=(p1==20&&p2==400&&p3==8902&&p4==197281);
    char line[340]={0};aAppend(line,340,"perft1=");aAppendInt(line,340,(int)p1);aAppend(line,340," perft2=");aAppendInt(line,340,(int)p2);aAppend(line,340," perft3=");aAppendInt(line,340,(int)p3);aAppend(line,340," perft4=");aAppendInt(line,340,(int)p4);aAppend(line,340," expected=20/400/8902/197281 overall=");aAppend(line,340,ok?"PASS":"FAIL");debugAppendRaw("SELFTEST-EXT",line);if(!ok)g_diagStateWarnings++;
}
static void debugGameQaSummary(int result,int reason){
    U64 u=0,o=0,t=0;currentGameClock(&u,&o,&t);TrainingCounters c;currentTrainingCounters(&c);char line[1080]={0};
    aAppend(line,1080,"result=");aAppendInt(line,1080,result);aAppend(line,1080," base_mode=");aAppendInt(line,1080,g_gameBaseMode);aAppend(line,1080," profile_id=");aAppendInt(line,1080,g_guestProfile?0:(int)g_activeProfileId);aAppend(line,1080," profile_guest=");aAppend(line,1080,g_guestProfile?"yes":"no");aAppend(line,1080," reason=");aAppendInt(line,1080,reason);aAppend(line,1080," plies=");aAppendInt(line,1080,g_game.histCount);
    aAppend(line,1080," player_moves=");aAppendInt(line,1080,c.playerMoves);aAppend(line,1080," independent_moves=");aAppendInt(line,1080,c.independentMoves);aAppend(line,1080," assisted_moves=");aAppendInt(line,1080,c.assistedMoves);aAppend(line,1080," rejected_suggestions=");aAppendInt(line,1080,c.rejectedSuggestions);aAppend(line,1080," dependency_pct=");aAppendInt(line,1080,c.dependencyPercent);aAppend(line,1080," independent_pct=");aAppendInt(line,1080,c.independentPercent);
    aAppend(line,1080," independent_accepted=");aAppendInt(line,1080,c.independentMoves);aAppend(line,1080," assisted_accepted=");aAppendInt(line,1080,c.assistedMoves);
    aAppend(line,1080," ask_accepted=");aAppendInt(line,1080,c.askAccepted);aAppend(line,1080," ask_rejected=");aAppendInt(line,1080,c.askRejected);aAppend(line,1080," recovery_accepted=");aAppendInt(line,1080,c.recoveryAccepted);aAppend(line,1080," recovery_rejected=");aAppendInt(line,1080,c.recoveryRejected);
    aAppend(line,1080," emergency_accepted=");aAppendInt(line,1080,c.emergencyAccepted);aAppend(line,1080," emergency_rejected=");aAppendInt(line,1080,c.emergencyRejected);aAppend(line,1080," full_assist_moves=");aAppendInt(line,1080,c.fullAssistMoves);aAppend(line,1080," full_assist_rejected=");aAppendInt(line,1080,c.fullAssistRejected);aAppend(line,1080," full_assist_on=");aAppendInt(line,1080,c.fullAssistOnCount);aAppend(line,1080," full_assist_off=");aAppendInt(line,1080,c.fullAssistOffCount);
    aAppend(line,1080," full_trainer_accepted=");aAppendInt(line,1080,c.fullTrainerAccepted);aAppend(line,1080," full_trainer_rejected=");aAppendInt(line,1080,c.fullTrainerRejected);aAppend(line,1080," coach_reviewed=");aAppendInt(line,1080,c.coachReviewed);
    aAppend(line,1080," total_ms=");aAppendInt(line,1080,(int)(t>2147483647ULL?2147483647ULL:t));aAppend(line,1080," user_ms=");aAppendInt(line,1080,(int)(u>2147483647ULL?2147483647ULL:u));aAppend(line,1080," opponent_ms=");aAppendInt(line,1080,(int)(o>2147483647ULL?2147483647ULL:o));
    aAppend(line,1080," engine_requests=");aAppendInt(line,1080,g_engineDiagRequests);aAppend(line,1080," engine_results=");aAppendInt(line,1080,g_engineDiagResults);aAppend(line,1080," engine_errors=");aAppendInt(line,1080,g_engineDiagErrors);aAppend(line,1080," engine_avg_ms=");aAppendInt(line,1080,g_engineDiagResults?(int)(g_engineDiagTotalMs/(U64)g_engineDiagResults):0);aAppend(line,1080," engine_max_ms=");aAppendInt(line,1080,(int)(g_engineDiagMaxMs>2147483647ULL?2147483647ULL:g_engineDiagMaxMs));aAppend(line,1080," state_warnings=");aAppendInt(line,1080,g_diagStateWarnings);debugAppendRaw("QA-GAME",line);
}

static void debugInit(){g_debugLogReady=true;g_debugLogLen=0;memset(g_debugLogBuffer,0,sizeof(g_debugLogBuffer));debugAppendRaw("START","Chess Trainer 6.0.0 diagnostic session started. Local logging only; no telemetry or network upload.");}
static void debugLogMove(const char* source,const Move& m,const char* san){char u[6]={0};moveUCI(m,u);char line[360]={0};aAppend(line,360,"move source=");aAppend(line,360,source?source:"unknown");aAppend(line,360," san=");aAppend(line,360,san?san:"?");aAppend(line,360," uci=");aAppend(line,360,u);aAppend(line,360," ply=");aAppendInt(line,360,g_game.histCount);aAppend(line,360," mode=");char mode[40]={0};const wchar_t* wm=trainerModeName(g_trainerMode);int j=0;for(int i=0;wm&&wm[i]&&j<38;i++)mode[j++]=(wm[i]<128)?(char)wm[i]:'?';mode[j]=0;aAppend(line,360,mode);debugAppendRaw("MOVE",line);}
static void debugLogAssistEvaluation(const Move& chosen,int before,int after,int loss,bool critical,bool mateDanger){
    char cu[6]={0};moveUCI(chosen,cu);char line[620]={0};aAppend(line,620,"candidate=");aAppend(line,620,cu);aAppend(line,620," before_cp=");aAppendInt(line,620,before);aAppend(line,620," after_cp=");aAppendInt(line,620,after);aAppend(line,620," loss_cp=");aAppendInt(line,620,loss);aAppend(line,620," critical=");aAppend(line,620,critical?"yes":"no");aAppend(line,620," mate_danger=");aAppend(line,620,mateDanger?"yes":"no");aAppend(line,620," best=");aAppend(line,620,g_assistBest[0]?g_assistBest:"?");aAppend(line,620," opponent_best_reply=");aAppend(line,620,g_assistOpponentReply[0]?g_assistOpponentReply:"?");debugAppendRaw("ASSIST-EVAL",line);
}
static void debugLogThreatDecision(int nowScore,int prevScore,bool checked,bool mateThreat,bool checkedDanger,bool suddenCollapse,bool latched){
    char line[620]={0};aAppend(line,620,"now_cp=");aAppendInt(line,620,nowScore);aAppend(line,620," prev_cp=");aAppendInt(line,620,prevScore);aAppend(line,620," in_check=");aAppend(line,620,checked?"yes":"no");aAppend(line,620," mate_threat=");aAppend(line,620,mateThreat?"yes":"no");if(mateThreat){aAppend(line,620," mate_in=");aAppendInt(line,620,-g_engineMate);}aAppend(line,620," checked_danger=");aAppend(line,620,checkedDanger?"yes":"no");aAppend(line,620," sudden_collapse=");aAppend(line,620,suddenCollapse?"yes":"no");aAppend(line,620," episode_latched=");aAppend(line,620,latched?"yes":"no");debugAppendRaw("THREAT-EVAL",line);
}

static void debugStateCheck(const char* where){
    if(!g_debugLogReady)return;char line[560]={0};bool issue=false;
    if(g_suggestionPending&&(!g_userSet||g_gameOver)){aAppend(line,560,"suggestion pending in invalid game state; ");issue=true;}
    if(g_trainerMode==MODE_ASSISTED&&g_suggestionPending&&g_userSet&&g_game.pos.whiteToMove!=g_userWhite){aAppend(line,560,"Assisted suggestion shown when it is not user's turn; ");issue=true;}
    if(g_assistRecoveryRemaining>2){aAppend(line,560,"recovery count exceeded 2; ");issue=true;}
    if(g_assistEmergencyUsed>ASSIST_EMERGENCY_BASE_MOVES+ASSIST_EMERGENCY_MAX_EXTRA){aAppend(line,560,"emergency count exceeded hard limit; ");issue=true;}
    if(g_assistFull&&g_trainerMode!=MODE_FULL){aAppend(line,560,"Full Assist flag active outside temporary Full Trainer; ");issue=true;}
    if(g_assistFull&&(g_assistRecoveryRemaining>0||g_assistEmergency)){aAppend(line,560,"Assisted rescue state leaked into Full Assist; ");issue=true;}
    if(g_game.histCount<0||g_game.histCount>512||g_game.keyCount!=g_game.histCount+1){aAppend(line,560,"history/key invariant mismatch; ");issue=true;}
    if(issue){g_diagStateWarnings++;aAppend(line,560,"where=");aAppend(line,560,where?where:"unknown");debugAppendRaw("STATE-WARN",line);}
}

static Move g_selfTestMovesA[256],g_selfTestMovesB[256];
static Position g_selfTestPosA,g_selfTestPosB;
static void runCoreSelfTest(){
    g_selfTestPosA=initialPosition();int n=legalMovesFor(&g_selfTestPosA,g_selfTestMovesA,256);bool ok=n==20;char line[300]={0};aAppend(line,300,"initial_legal_moves=");aAppendInt(line,300,n);aAppend(line,300," expected=20 result=");aAppend(line,300,ok?"PASS":"FAIL");
    bool e4=false;for(int i=0;i<n;i++){char u[6]={0};moveUCI(g_selfTestMovesA[i],u);if(aCmp(u,"e2e4")==0){g_selfTestPosB=g_selfTestPosA;applyMoveRaw(&g_selfTestPosB,g_selfTestMovesA[i]);int rn=legalMovesFor(&g_selfTestPosB,g_selfTestMovesB,256);aAppend(line,300,"; after_e4_black_legal=");aAppendInt(line,300,rn);if(rn!=20)ok=false;e4=true;break;}}
    if(!e4){aAppend(line,300,"; e2e4_missing");ok=false;}aAppend(line,300,"; overall=");aAppend(line,300,ok?"PASS":"FAIL");debugAppendRaw("SELFTEST",line);
}
static void openDebugLog(){
    debugAppendRaw("USER","Log button opened diagnostic log.");debugFlush();wchar_t path[1024];buildDebugLogPath(path);wchar_t cmd[1300]={0};wCopy(cmd,1300,L"notepad.exe \"");wAppend(cmd,1300,path);wAppend(cmd,1300,L"\"");STARTUPINFOW si;PROCESS_INFORMATION pi;memset(&si,0,sizeof(si));memset(&pi,0,sizeof(pi));si.cb=sizeof(si);if(CreateProcessW(NULLPTR,cmd,NULLPTR,NULLPTR,FALSE,NORMAL_PRIORITY_CLASS,NULLPTR,NULLPTR,&si,&pi)){CloseHandle(pi.hThread);CloseHandle(pi.hProcess);}else{wchar_t msg[1200]={0};wCopy(msg,1200,L"The debug log is saved here:\r\n\r\n");wAppend(msg,1200,path);MessageBoxW(g_mainHwnd,msg,L"Chess Trainer - Debug Log",MB_OK|MB_ICONINFORMATION);}
}


// --------------------------- Game intelligence / persistent history ---------------------------
// V4.2 retains the offline opening recogniser and Game Insights while making game-result
// recording explicit about which player resigned, abandoned or lost on time. Nothing changes the proven
// board/engine state machine from V3.7.

enum GameResultCode { RESULT_NONE=0, RESULT_WIN=1, RESULT_DRAW=2, RESULT_LOSS=3, RESULT_ABORTED=4 };
enum GameReasonCode {
    REASON_NONE=0, REASON_CHECKMATE=1, REASON_STALEMATE=2, REASON_INSUFFICIENT=3,
    REASON_THREEFOLD=4, REASON_FIFTY_MOVE=5, REASON_OPPONENT_RESIGNED=6,
    REASON_USER_RESIGNED=7, REASON_DRAW_AGREED=8, REASON_ABORTED=9,
    REASON_OPPONENT_ABANDONED=10, REASON_USER_ABANDONED=11,
    REASON_OPPONENT_TIMEOUT=12, REASON_USER_TIMEOUT=13, REASON_CANCELLED=14
};

struct SavedGameRecord {
    WORD year,month,day,hour,minute;
    BYTE result,reason,userWhite,reserved;
    WORD plies;
    char openingEco[8];
    char openingName[64];
    char finalMove[20];
};
struct HistoryStore {
    DWORD magic,version,count;
    SavedGameRecord games[200];
};
static const DWORD HISTORY_MAGIC=0x34485443u; // "CTH4"
static const DWORD HISTORY_VERSION=1;
static HistoryStore g_savedHistory;
static bool g_historyLoaded=false;
static bool g_gameRecorded=false;
static int g_panelMode=0; // 0 = Analysis/Game Log, 1 = Game Insights, 2 = History
static int g_lastMate=0;  // Stockfish mate score for the current position, if available
static int g_currentResult=RESULT_NONE,g_currentReason=REASON_NONE;
static int INSIGHTS_TOP=150, INSIGHTS_BOTTOM=560;

// V6.0.0 local Player Profiles. Profiles are deliberately stored beside the executable
// in a small fixed-size binary file. No account, cloud service, socket, HTTP library or
// background network service is used. A stable numeric profile ID survives renaming.
struct PlayerProfile {
    DWORD id;
    wchar_t name[32];
    int defaultRating;   // Starting / preferred baseline, retained for improvement tracking.
    int currentRating;   // Current locally tracked rating.
    WORD createdYear,createdMonth,createdDay,reserved;
};
struct ProfileStore {
    DWORD magic,version,count,activeId,nextId;
    PlayerProfile profiles[12];
};
static const DWORD PROFILE_MAGIC=0x31505443u; // CTP1
static const DWORD PROFILE_VERSION=1;
static ProfileStore g_profiles;
static bool g_profilesLoaded=false,g_profilesCreatedFresh=false;

static void buildProfilesPath(wchar_t out[1024]){
    DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){wCopy(out,1024,L"Chess_Trainer_Profiles.dat");return;}
    int last=-1;for(int i=0;i<(int)n;i++)if(out[i]=='\\'||out[i]=='/')last=i;
    if(last<0){wCopy(out,1024,L"Chess_Trainer_Profiles.dat");return;}out[last+1]=0;wAppend(out,1024,L"Chess_Trainer_Profiles.dat");
}
static int profileIndexById(DWORD id){for(DWORD i=0;i<g_profiles.count&&i<12;i++)if(g_profiles.profiles[i].id==id)return (int)i;return -1;}
static PlayerProfile* activePersistentProfile(){if(g_guestProfile)return NULLPTR;int i=profileIndexById(g_activeProfileId);return i>=0?&g_profiles.profiles[i]:NULLPTR;}
static const wchar_t* activeProfileName(){if(g_guestProfile)return L"Guest";PlayerProfile* p=activePersistentProfile();return p&&p->name[0]?p->name:L"Player";}
static DWORD persistentProfileId(){return g_guestProfile?0:g_activeProfileId;}
static void resetProfileStore(){
    memset(&g_profiles,0,sizeof(g_profiles));g_profiles.magic=PROFILE_MAGIC;g_profiles.version=PROFILE_VERSION;g_profiles.count=1;g_profiles.activeId=1;g_profiles.nextId=2;
    PlayerProfile &p=g_profiles.profiles[0];p.id=1;wCopy(p.name,32,L"Player 1");SYSTEMTIME st;GetLocalTime(&st);p.createdYear=st.wYear;p.createdMonth=st.wMonth;p.createdDay=st.wDay;
    g_activeProfileId=1;g_guestProfile=false;
}
static bool saveProfileStore(){
    if(!g_profilesLoaded)return false;g_profiles.activeId=g_activeProfileId;wchar_t path[1024];buildProfilesPath(path);HANDLE h=CreateFileW(path,GENERIC_WRITE,0,NULLPTR,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULLPTR);if(h==INVALID_HANDLE_VALUE_CONST)return false;DWORD wrote=0;BOOL ok=WriteFile(h,&g_profiles,(DWORD)sizeof(g_profiles),&wrote,NULLPTR);CloseHandle(h);return ok&&wrote==(DWORD)sizeof(g_profiles);
}
static void loadProfileStore(){
    resetProfileStore();g_profilesCreatedFresh=false;wchar_t path[1024];buildProfilesPath(path);HANDLE h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULLPTR,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULLPTR);
    if(h==INVALID_HANDLE_VALUE_CONST){g_profilesLoaded=true;g_profilesCreatedFresh=true;saveProfileStore();return;}
    DWORD got=0;BOOL ok=ReadFile(h,&g_profiles,(DWORD)sizeof(g_profiles),&got,NULLPTR);CloseHandle(h);
    bool valid=ok&&got==sizeof(g_profiles)&&g_profiles.magic==PROFILE_MAGIC&&g_profiles.version==PROFILE_VERSION&&g_profiles.count>=1&&g_profiles.count<=12;
    if(valid){for(DWORD i=0;i<g_profiles.count;i++)if(!g_profiles.profiles[i].id||!g_profiles.profiles[i].name[0]){valid=false;break;}}
    if(!valid){resetProfileStore();g_profilesCreatedFresh=true;}
    else{g_activeProfileId=g_profiles.activeId;if(profileIndexById(g_activeProfileId)<0)g_activeProfileId=g_profiles.profiles[0].id;g_guestProfile=false;if(g_profiles.nextId<=g_profiles.count)g_profiles.nextId=g_profiles.count+1;}
    g_profilesLoaded=true;if(g_profilesCreatedFresh)saveProfileStore();
}

// V5.5.0 detailed local archive. The original HistoryStore remains untouched for
// compatibility; this second file adds move logs, review tags and assistance flags
// for games recorded from V5.1 onward. Older history entries are migrated as summaries.
struct ArchivedGame {
    WORD year,month,day,hour,minute; BYTE result,reason,userWhite,mode; WORD plies; BYTE hasMoves,reviewReady;
    int ratingBefore,ratingAfter,opponentRating;
    char openingEco[8],openingName[64],finalMove[20];
    char san[256][20]; BYTE reviewClass[256],assistAccepted[256];
};
struct ArchiveStore { DWORD magic,version,count; ArchivedGame games[100]; };
static const DWORD ARCHIVE_MAGIC=0x31415443u; // CTA1
static const DWORD ARCHIVE_VERSION=1;
static ArchiveStore g_archive; static bool g_archiveLoaded=false; static int g_currentArchiveIndex=-1;
static bool g_historyDetail=false; static int g_historySelected=-1;

// Sidecar ownership map keeps the proven V5.x history/archive file formats intact.
// Existing pre-profile records are assigned once to the first/default profile.
struct ProfileGameMapStore { DWORD magic,version; DWORD historyProfile[200]; DWORD archiveProfile[100]; };
static const DWORD PROFILE_MAP_MAGIC=0x314D5443u; // CTM1
static const DWORD PROFILE_MAP_VERSION=1;
static ProfileGameMapStore g_profileGameMap;
static bool g_profileGameMapLoaded=false;

static void buildProfileMapPath(wchar_t out[1024]){
    DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){wCopy(out,1024,L"Chess_Trainer_Profile_Game_Map.dat");return;}
    int last=-1;for(int i=0;i<(int)n;i++)if(out[i]=='\\'||out[i]=='/')last=i;
    if(last<0){wCopy(out,1024,L"Chess_Trainer_Profile_Game_Map.dat");return;}out[last+1]=0;wAppend(out,1024,L"Chess_Trainer_Profile_Game_Map.dat");
}
static bool saveProfileGameMap(){
    if(!g_profileGameMapLoaded)return false;wchar_t path[1024];buildProfileMapPath(path);HANDLE h=CreateFileW(path,GENERIC_WRITE,0,NULLPTR,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULLPTR);if(h==INVALID_HANDLE_VALUE_CONST)return false;DWORD wrote=0;BOOL ok=WriteFile(h,&g_profileGameMap,(DWORD)sizeof(g_profileGameMap),&wrote,NULLPTR);CloseHandle(h);return ok&&wrote==(DWORD)sizeof(g_profileGameMap);
}
static void loadProfileGameMap(){
    memset(&g_profileGameMap,0,sizeof(g_profileGameMap));g_profileGameMap.magic=PROFILE_MAP_MAGIC;g_profileGameMap.version=PROFILE_MAP_VERSION;wchar_t path[1024];buildProfileMapPath(path);HANDLE h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULLPTR,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULLPTR);bool valid=false;
    if(h!=INVALID_HANDLE_VALUE_CONST){DWORD got=0;BOOL ok=ReadFile(h,&g_profileGameMap,(DWORD)sizeof(g_profileGameMap),&got,NULLPTR);CloseHandle(h);valid=ok&&got==sizeof(g_profileGameMap)&&g_profileGameMap.magic==PROFILE_MAP_MAGIC&&g_profileGameMap.version==PROFILE_MAP_VERSION;}
    if(!valid){memset(&g_profileGameMap,0,sizeof(g_profileGameMap));g_profileGameMap.magic=PROFILE_MAP_MAGIC;g_profileGameMap.version=PROFILE_MAP_VERSION;DWORD owner=g_activeProfileId?g_activeProfileId:1;for(DWORD i=0;i<g_savedHistory.count&&i<200;i++)g_profileGameMap.historyProfile[i]=owner;for(DWORD i=0;i<g_archive.count&&i<100;i++)g_profileGameMap.archiveProfile[i]=owner;}
    else{DWORD owner=g_activeProfileId?g_activeProfileId:1;bool changed=false;for(DWORD i=0;i<g_savedHistory.count&&i<200;i++)if(!g_profileGameMap.historyProfile[i]){g_profileGameMap.historyProfile[i]=owner;changed=true;}for(DWORD i=0;i<g_archive.count&&i<100;i++)if(!g_profileGameMap.archiveProfile[i]){g_profileGameMap.archiveProfile[i]=owner;changed=true;}if(changed)valid=false;}
    g_profileGameMapLoaded=true;if(!valid)saveProfileGameMap();
}
static bool historyOwnedByActive(int idx){return !g_guestProfile&&idx>=0&&idx<(int)g_savedHistory.count&&g_profileGameMapLoaded&&g_profileGameMap.historyProfile[idx]==g_activeProfileId;}
static bool archiveOwnedByActive(int idx){return !g_guestProfile&&idx>=0&&idx<(int)g_archive.count&&g_profileGameMapLoaded&&g_profileGameMap.archiveProfile[idx]==g_activeProfileId;}
static int activeArchiveCount(){if(g_guestProfile)return 0;int n=0;for(DWORD i=0;i<g_archive.count;i++)if(archiveOwnedByActive((int)i))n++;return n;}
static int recentActiveArchiveIndex(int visual){if(visual<0||g_guestProfile)return -1;int seen=0;for(int i=(int)g_archive.count-1;i>=0;i--)if(archiveOwnedByActive(i)){if(seen==visual)return i;seen++;}return -1;}
static void debugProfileQa(){
    char line[420]={0};aAppend(line,420,"profiles=");aAppendInt(line,420,(int)g_profiles.count);aAppend(line,420," active_id=");aAppendInt(line,420,(int)g_activeProfileId);aAppend(line,420," guest=");aAppend(line,420,g_guestProfile?"yes":"no");aAppend(line,420," active_history=");int hc=0;for(DWORD i=0;i<g_savedHistory.count;i++)if(g_profileGameMap.historyProfile[i]==g_activeProfileId)hc++;aAppendInt(line,420,hc);aAppend(line,420," active_archive=");aAppendInt(line,420,activeArchiveCount());aAppend(line,420," profile_store=");aAppend(line,420,g_profilesLoaded?"PASS":"FAIL");aAppend(line,420," ownership_map=");aAppend(line,420,g_profileGameMapLoaded?"PASS":"FAIL");debugAppendRaw("PROFILE-QA",line);
    if(!g_profilesLoaded||!g_profileGameMapLoaded||g_profiles.count<1||g_profiles.count>12||profileIndexById(g_activeProfileId)<0)g_diagStateWarnings++;
}

// Rating tracker. Chess.com uses a Glicko-family system, so two ratings alone are
// insufficient to reproduce its exact change. Chess Trainer therefore shows a
// clearly labelled Elo-style estimate and lets the user correct it to the actual
// Chess.com value after the game.
static int g_currentRating=0,g_ratingBeforeGame=0,g_opponentRating=0;
static int g_lastRatingChange=0,g_lastRatingAfter=0;
static const DWORD RATING_MAGIC=0x31525443u; // CTR1

// Post-game move review. This is intentionally called "Trainer review" because the
// labels are our deterministic thresholds, not Chess.com's proprietary classifications.
enum ReviewClass { RV_NONE=0,RV_BRILLIANT=1,RV_GREAT=2,RV_BOOK=3,RV_BEST=4,RV_EXCELLENT=5,RV_GOOD=6,RV_INACCURACY=7,RV_MISTAKE=8,RV_MISS=9,RV_BLUNDER=10,RV_MATE=11 };
static BYTE g_reviewClass[512];
static int g_reviewLoss[512];
static bool g_reviewInProgress=false,g_reviewReady=false;

struct OpeningDef { const char* moves; const char* eco; const char* name; };
static const OpeningDef g_openings[]={
    {"e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 b1c3 a7a6","B90","Sicilian Defence, Najdorf Variation"},
    {"e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4","B32","Sicilian Defence, Open Variation"},
    {"e2e4 c7c5 g1f3 b8c6 f1b5","B30","Sicilian Defence, Rossolimo Variation"},
    {"e2e4 c7c5 g1f3 e7e6 d2d4 c5d4 f3d4","B40","Sicilian Defence, French Variation"},
    {"e2e4 c7c5","B20","Sicilian Defence"},
    {"e2e4 e7e6 d2d4 d7d5 e4e5","C02","French Defence, Advance Variation"},
    {"e2e4 e7e6","C00","French Defence"},
    {"e2e4 c7c6 d2d4 d7d5","B12","Caro-Kann Defence"},
    {"e2e4 c7c6","B10","Caro-Kann Defence"},
    {"e2e4 d7d5 e4d5 d8d5","B01","Scandinavian Defence, Main Line"},
    {"e2e4 d7d5","B01","Scandinavian Defence"},
    {"e2e4 d7d6 d2d4 g8f6 b1c3 g7g6","B07","Pirc Defence"},
    {"e2e4 d7d6","B07","Pirc Defence"},
    {"e2e4 g7g6","B06","Modern Defence"},
    {"e2e4 g8f6","B02","Alekhine Defence"},
    {"e2e4 e7e5 g1f3 b8c6 f1b5","C60","Ruy Lopez"},
    {"e2e4 e7e5 g1f3 b8c6 f1c4","C50","Italian Game"},
    {"e2e4 e7e5 g1f3 b8c6 d2d4","C44","Scotch Game"},
    {"e2e4 e7e5 g1f3 b8c6 b1c3 g8f6","C47","Four Knights Game"},
    {"e2e4 e7e5 f2f4","C30","King's Gambit"},
    {"e2e4 e7e5 b1c3","C25","Vienna Game"},
    {"e2e4 e7e5","C20","Open Game"},
    {"e2e4","B00","King's Pawn Opening"},
    {"d2d4 d7d5 c2c4 d5c4","D20","Queen's Gambit Accepted"},
    {"d2d4 d7d5 c2c4 e7e6","D30","Queen's Gambit Declined"},
    {"d2d4 d7d5 c2c4 c7c6","D10","Slav Defence"},
    {"d2d4 d7d5 c2c4","D06","Queen's Gambit"},
    {"d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6","E60","King's Indian Defence"},
    {"d2d4 g8f6 c2c4 e7e6 b1c3 f8b4","E20","Nimzo-Indian Defence"},
    {"d2d4 g8f6 c2c4 e7e6 g1f3 b7b6","E12","Queen's Indian Defence"},
    {"d2d4 g8f6 c2c4 e7e6 g2g3 d7d5 f1g2","E00","Catalan Opening"},
    {"d2d4 g8f6 c2c4 c7c5 d4d5 b7b5","A57","Benko Gambit"},
    {"d2d4 g8f6 c2c4 c7c5 d4d5 e7e6","A60","Benoni Defence"},
    {"d2d4 f7f5","A80","Dutch Defence"},
    {"d2d4 g8f6","A45","Indian Game"},
    {"d2d4 d7d5 g1f3 g8f6 c1f4","D02","London System"},
    {"d2d4","A40","Queen's Pawn Game"},
    {"c2c4 e7e5","A20","English Opening, King's English"},
    {"c2c4 c7c5","A30","English Opening, Symmetrical Variation"},
    {"c2c4","A10","English Opening"},
    {"g1f3 d7d5 c2c4","A09","Reti Opening"},
    {"g1f3","A04","Reti Opening"},
    {"b2b3","A01","Larsen's Opening"},
    {"f2f4","A02","Bird's Opening"},
    {"g2g3","A00","King's Fianchetto Opening"}
};

static void buildCurrentUci(char* out,int cap){
    out[0]=0;
    for(int i=0;i<g_game.histCount;i++){
        char u[6];moveUCI(g_game.hist[i].move,u);
        if(i)aAppendChar(out,cap,' ');aAppend(out,cap,u);
    }
}
static bool openingPrefixMatches(const char* seq,const char* prefix){
    int n=aLen(prefix);for(int i=0;i<n;i++)if(seq[i]!=prefix[i])return false;
    return seq[n]==0||seq[n]==' ';
}
static bool recognizeOpening(char* name,int nameCap,char* eco,int ecoCap){
    if(g_game.histCount<=0){aCopy(name,nameCap,"Starting Position");aCopy(eco,ecoCap,"--");return false;}
    char seq[4096];buildCurrentUci(seq,4096);int best=-1,bestLen=-1;
    int count=(int)(sizeof(g_openings)/sizeof(g_openings[0]));
    for(int i=0;i<count;i++){int len=aLen(g_openings[i].moves);if(len>bestLen&&openingPrefixMatches(seq,g_openings[i].moves)){best=i;bestLen=len;}}
    if(best>=0){aCopy(name,nameCap,g_openings[best].name);aCopy(eco,ecoCap,g_openings[best].eco);return true;}
    aCopy(name,nameCap,"Unclassified Opening");aCopy(eco,ecoCap,"--");return false;
}
static bool isBookPly(int ply){
    if(ply<0||ply>=g_game.histCount)return false;char seq[4096]={0};
    for(int i=0;i<=ply;i++){char u[6];moveUCI(g_game.hist[i].move,u);if(i)aAppendChar(seq,4096,' ');aAppend(seq,4096,u);}
    int sl=aLen(seq),count=(int)(sizeof(g_openings)/sizeof(g_openings[0]));
    for(int i=0;i<count;i++){const char* line=g_openings[i].moves;bool ok=true;for(int j=0;j<sl;j++)if(line[j]!=seq[j]){ok=false;break;}if(ok&&(line[sl]==0||line[sl]==' '))return true;}
    return false;
}

static void buildHistoryPath(wchar_t out[1024]){
    DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){wCopy(out,1024,L"Chess_Trainer_History.dat");return;}
    int last=-1;for(int i=0;i<(int)n;i++)if(out[i]=='\\'||out[i]=='/')last=i;
    if(last<0){wCopy(out,1024,L"Chess_Trainer_History.dat");return;}out[last+1]=0;wAppend(out,1024,L"Chess_Trainer_History.dat");
}
static void resetHistoryStore(){memset(&g_savedHistory,0,sizeof(g_savedHistory));g_savedHistory.magic=HISTORY_MAGIC;g_savedHistory.version=HISTORY_VERSION;}
static void loadHistoryStore(){
    resetHistoryStore();wchar_t path[1024];buildHistoryPath(path);
    HANDLE h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULLPTR,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULLPTR);
    if(h==INVALID_HANDLE_VALUE_CONST){g_historyLoaded=true;return;}
    DWORD got=0;BOOL ok=ReadFile(h,&g_savedHistory,(DWORD)sizeof(g_savedHistory),&got,NULLPTR);CloseHandle(h);
    if(!(ok&&got>=12&&g_savedHistory.magic==HISTORY_MAGIC&&g_savedHistory.version==HISTORY_VERSION&&g_savedHistory.count<=200))resetHistoryStore();
    g_historyLoaded=true;
}
static bool saveHistoryStore(){
    wchar_t path[1024];buildHistoryPath(path);HANDLE h=CreateFileW(path,GENERIC_WRITE,0,NULLPTR,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULLPTR);
    if(h==INVALID_HANDLE_VALUE_CONST)return false;DWORD wrote=0;BOOL ok=WriteFile(h,&g_savedHistory,(DWORD)sizeof(g_savedHistory),&wrote,NULLPTR);CloseHandle(h);return ok&&wrote==(DWORD)sizeof(g_savedHistory);
}
static const wchar_t* resultLabel(int r){return r==RESULT_WIN?L"Win":r==RESULT_DRAW?L"Draw":r==RESULT_LOSS?L"Loss":r==RESULT_ABORTED?L"No result":L"In progress";}
static const wchar_t* reasonLabel(int r){
    switch(r){
        case REASON_CHECKMATE:return L"Checkmate";
        case REASON_STALEMATE:return L"Stalemate";
        case REASON_INSUFFICIENT:return L"Insufficient material";
        case REASON_THREEFOLD:return L"Threefold repetition";
        case REASON_FIFTY_MOVE:return L"Fifty-move rule";
        case REASON_OPPONENT_RESIGNED:return L"Opponent resigned";
        case REASON_USER_RESIGNED:return L"You resigned";
        case REASON_DRAW_AGREED:return L"Draw agreed";
        case REASON_ABORTED:return L"Game aborted (legacy)";
        case REASON_OPPONENT_ABANDONED:return L"Opponent abandoned";
        case REASON_USER_ABANDONED:return L"You abandoned";
        case REASON_OPPONENT_TIMEOUT:return L"Opponent lost on time";
        case REASON_USER_TIMEOUT:return L"You lost on time";
        case REASON_CANCELLED:return L"Game cancelled";
    }
    return L"In progress";
}
static int persistentTrainerMode(){return g_gameBaseMode;}
static int appendArchiveForCurrentGame(int result,int reason);
static void updateCurrentArchiveReview();
static void recordCurrentGame(int result,int reason){
    if(!g_userSet||g_gameRecorded)return;gameClockStop();debugGameQaSummary(result,reason);
    if(g_guestProfile){g_gameRecorded=true;g_currentArchiveIndex=-1;debugAppendRaw("PROFILE","Guest game completed; persistent career/history/rating storage intentionally skipped.");return;}
    if(!g_historyLoaded)loadHistoryStore();if(!g_profileGameMapLoaded)loadProfileGameMap();
    SavedGameRecord rec;memset(&rec,0,sizeof(rec));SYSTEMTIME st;GetLocalTime(&st);
    rec.year=st.wYear;rec.month=st.wMonth;rec.day=st.wDay;rec.hour=st.wHour;rec.minute=st.wMinute;rec.result=(BYTE)result;rec.reason=(BYTE)reason;rec.userWhite=g_userWhite?1:0;rec.reserved=(BYTE)persistentTrainerMode();rec.plies=(WORD)(g_game.histCount>65535?65535:g_game.histCount);
    recognizeOpening(rec.openingName,64,rec.openingEco,8);if(g_game.histCount>0)aCopy(rec.finalMove,20,g_game.hist[g_game.histCount-1].san);
    int histIdx=0;if(g_savedHistory.count<200){histIdx=(int)g_savedHistory.count;g_savedHistory.games[g_savedHistory.count++]=rec;}else{for(int i=1;i<200;i++){g_savedHistory.games[i-1]=g_savedHistory.games[i];g_profileGameMap.historyProfile[i-1]=g_profileGameMap.historyProfile[i];}histIdx=199;g_savedHistory.games[199]=rec;}
    g_profileGameMap.historyProfile[histIdx]=g_activeProfileId;saveHistoryStore();saveProfileGameMap();g_gameRecorded=true;g_currentArchiveIndex=appendArchiveForCurrentGame(result,reason);
}
static void computeCareerStats(int* attempts,int* wins,int* draws,int* losses,int* aborted){
    *attempts=*wins=*draws=*losses=*aborted=0;if(g_guestProfile)return;if(!g_historyLoaded)loadHistoryStore();if(!g_profileGameMapLoaded)loadProfileGameMap();
    for(DWORD i=0;i<g_savedHistory.count;i++){if(!historyOwnedByActive((int)i))continue;(*attempts)++;int r=g_savedHistory.games[i].result;if(r==RESULT_WIN)(*wins)++;else if(r==RESULT_DRAW)(*draws)++;else if(r==RESULT_LOSS)(*losses)++;else if(r==RESULT_ABORTED)(*aborted)++;}
}
static void buildArchivePath(wchar_t out[1024]){DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){wCopy(out,1024,L"Chess_Trainer_GameArchive.dat");return;}int last=-1;for(int i=0;i<(int)n;i++)if(out[i]=='\\'||out[i]=='/')last=i;if(last<0){wCopy(out,1024,L"Chess_Trainer_GameArchive.dat");return;}out[last+1]=0;wAppend(out,1024,L"Chess_Trainer_GameArchive.dat");}
static void resetArchive(){memset(&g_archive,0,sizeof(g_archive));g_archive.magic=ARCHIVE_MAGIC;g_archive.version=ARCHIVE_VERSION;}
static bool saveArchive(){wchar_t path[1024];buildArchivePath(path);HANDLE h=CreateFileW(path,GENERIC_WRITE,0,NULLPTR,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULLPTR);if(h==INVALID_HANDLE_VALUE_CONST)return false;DWORD wrote=0;BOOL ok=WriteFile(h,&g_archive,(DWORD)sizeof(g_archive),&wrote,NULLPTR);CloseHandle(h);return ok&&wrote==(DWORD)sizeof(g_archive);}
static void loadArchive(){
    // V5.1.0 created a full ArchiveStore as a local variable here. The archive is
    // roughly 0.6 MB, and this project intentionally builds without CRT stack probes.
    // That large pre-window stack frame could jump over Windows' guard page and make
    // the process terminate before the main GUI was ever created. Read directly into
    // the global archive buffer instead, then validate it in place.
    resetArchive();wchar_t path[1024];buildArchivePath(path);HANDLE h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULLPTR,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULLPTR);
    if(h!=INVALID_HANDLE_VALUE_CONST){DWORD got=0;BOOL ok=ReadFile(h,&g_archive,(DWORD)sizeof(g_archive),&got,NULLPTR);CloseHandle(h);if(!(ok&&got==sizeof(g_archive)&&g_archive.magic==ARCHIVE_MAGIC&&g_archive.version==ARCHIVE_VERSION&&g_archive.count<=100))resetArchive();}
    if(g_archive.count==0&&g_savedHistory.count>0){DWORD start=g_savedHistory.count>100?g_savedHistory.count-100:0;for(DWORD i=start;i<g_savedHistory.count;i++){SavedGameRecord &src=g_savedHistory.games[i];ArchivedGame &a=g_archive.games[g_archive.count++];memset(&a,0,sizeof(a));a.year=src.year;a.month=src.month;a.day=src.day;a.hour=src.hour;a.minute=src.minute;a.result=src.result;a.reason=src.reason;a.userWhite=src.userWhite;a.mode=src.reserved;a.plies=src.plies;a.hasMoves=0;a.reviewReady=0;aCopy(a.openingEco,8,src.openingEco);aCopy(a.openingName,64,src.openingName);aCopy(a.finalMove,20,src.finalMove);}saveArchive();}
    g_archiveLoaded=true;
}
static int appendArchiveForCurrentGame(int result,int reason){
    if(g_guestProfile)return -1;if(!g_archiveLoaded)loadArchive();if(!g_profileGameMapLoaded)loadProfileGameMap();int idx=0;if(g_archive.count<100){idx=(int)g_archive.count;g_archive.count++;}else{for(int i=1;i<100;i++){g_archive.games[i-1]=g_archive.games[i];g_profileGameMap.archiveProfile[i-1]=g_profileGameMap.archiveProfile[i];}idx=99;}
    // Populate the destination record in the global archive directly. Avoid another
    // multi-kilobyte local ArchivedGame object for the same no-stack-probe reason.
    ArchivedGame &a=g_archive.games[idx];memset(&a,0,sizeof(a));SYSTEMTIME st;GetLocalTime(&st);a.year=st.wYear;a.month=st.wMonth;a.day=st.wDay;a.hour=st.wHour;a.minute=st.wMinute;a.result=(BYTE)result;a.reason=(BYTE)reason;a.userWhite=g_userWhite?1:0;a.mode=(BYTE)persistentTrainerMode();a.plies=(WORD)iMin(g_game.histCount,256);a.hasMoves=1;a.reviewReady=g_reviewReady?1:0;a.ratingBefore=g_ratingBeforeGame;a.ratingAfter=g_lastRatingAfter;a.opponentRating=g_opponentRating;recognizeOpening(a.openingName,64,a.openingEco,8);if(g_game.histCount>0)aCopy(a.finalMove,20,g_game.hist[g_game.histCount-1].san);for(int i=0;i<a.plies;i++){aCopy(a.san[i],20,g_game.hist[i].san);a.reviewClass[i]=g_reviewClass[i];a.assistAccepted[i]=g_assistAccepted[i];}
    g_profileGameMap.archiveProfile[idx]=g_activeProfileId;saveArchive();saveProfileGameMap();return idx;
}
static void updateCurrentArchiveReview(){if(g_currentArchiveIndex<0||!g_archiveLoaded||g_currentArchiveIndex>=(int)g_archive.count)return;ArchivedGame &a=g_archive.games[g_currentArchiveIndex];int n=iMin((int)a.plies,iMin(g_game.histCount,256));for(int i=0;i<n;i++){a.reviewClass[i]=g_reviewClass[i];a.assistAccepted[i]=g_assistAccepted[i];}a.reviewReady=g_reviewReady?1:0;a.ratingAfter=g_lastRatingAfter;saveArchive();}

static void setStatus(const wchar_t* s);
static void invalidateAnalysis();

// --------------------------- Rating tracker ---------------------------
struct RatingFile { DWORD magic,version; int currentRating; };
static void buildRatingPath(wchar_t out[1024]){
    DWORD n=GetModuleFileNameW(NULLPTR,out,1024);if(n==0||n>=1023){wCopy(out,1024,L"Chess_Trainer_Rating.dat");return;}
    int last=-1;for(int i=0;i<(int)n;i++)if(out[i]=='\\'||out[i]=='/')last=i;
    if(last<0){wCopy(out,1024,L"Chess_Trainer_Rating.dat");return;}out[last+1]=0;wAppend(out,1024,L"Chess_Trainer_Rating.dat");
}
static void loadRatingStore(){
    g_currentRating=0;if(g_guestProfile)return;PlayerProfile* p=activePersistentProfile();if(!p)return;
    if(g_profilesCreatedFresh){wchar_t path[1024];buildRatingPath(path);HANDLE h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULLPTR,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULLPTR);if(h!=INVALID_HANDLE_VALUE_CONST){RatingFile r;memset(&r,0,sizeof(r));DWORD got=0;BOOL ok=ReadFile(h,&r,(DWORD)sizeof(r),&got,NULLPTR);CloseHandle(h);if(ok&&got==sizeof(r)&&r.magic==RATING_MAGIC&&r.version==1&&r.currentRating>=0&&r.currentRating<=10000){p->currentRating=r.currentRating;if(p->defaultRating<=0)p->defaultRating=r.currentRating;saveProfileStore();debugAppendRaw("PROFILE","Imported the legacy V5.x rating into the default Player Profile.");}}g_profilesCreatedFresh=false;}
    if(p->currentRating<=0&&p->defaultRating>0){p->currentRating=p->defaultRating;saveProfileStore();}g_currentRating=p->currentRating;
}
static void saveRatingStore(){
    if(g_guestProfile)return;PlayerProfile* p=activePersistentProfile();if(p){p->currentRating=g_currentRating;if(p->defaultRating<=0&&g_currentRating>0)p->defaultRating=g_currentRating;saveProfileStore();}
    // Keep the legacy single-rating file updated for safe rollback to V5.6.x. It reflects
    // only whichever persistent profile is currently selected; V5.7+ uses Profiles.dat.
    RatingFile r;r.magic=RATING_MAGIC;r.version=1;r.currentRating=g_currentRating;wchar_t path[1024];buildRatingPath(path);HANDLE h=CreateFileW(path,GENERIC_WRITE,0,NULLPTR,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULLPTR);if(h==INVALID_HANDLE_VALUE_CONST)return;DWORD wrote=0;WriteFile(h,&r,(DWORD)sizeof(r),&wrote,NULLPTR);CloseHandle(h);
}
static int expectedScore1000(int own,int opp){
    // Integer approximation of 1/(1+10^((opp-own)/400)), sampled every 50 points.
    static const int higherExpected[17]={500,571,640,703,760,808,849,882,909,930,947,960,969,977,981,987,990};
    int d=own-opp;bool ownHigher=d>=0;if(d<0)d=-d;if(d>=800)return ownHigher?990:10;int idx=d/50,rem=d%50;int a=higherExpected[idx],b=higherExpected[idx+1];int e=a+(b-a)*rem/50;return ownHigher?e:1000-e;
}
static int estimatedRatingDelta(int own,int opp,int result){
    if(own<=0||opp<=0||result==RESULT_ABORTED)return 0;int score=result==RESULT_WIN?1000:result==RESULT_DRAW?500:0;int diff=32*(score-expectedScore1000(own,opp));return diff>=0?(diff+500)/1000:-((-diff+500)/1000);
}
static void applyRatingResult(int result){
    g_lastRatingChange=0;g_lastRatingAfter=g_currentRating;if(g_guestProfile)return;if(g_ratingBeforeGame<=0||g_opponentRating<=0||result==RESULT_ABORTED)return;
    g_lastRatingChange=estimatedRatingDelta(g_ratingBeforeGame,g_opponentRating,result);g_lastRatingAfter=g_ratingBeforeGame+g_lastRatingChange;if(g_lastRatingAfter<0)g_lastRatingAfter=0;g_currentRating=g_lastRatingAfter;saveRatingStore();
}

static void invalidateGameLog();

// --------------------------- Piece counts / post-game review ---------------------------
static int countPieces(const Position* p,bool white){int n=0;for(int i=0;i<64;i++)if(p->sq[i]&&sameColor(p->sq[i],white))n++;return n;}
static int opponentPiecesLeftAfterPly(int ply){
    if(ply<0||ply>=g_game.histCount)return 16;Position q=g_game.hist[ply].before;bool moverWhite=q.whiteToMove;applyMoveRaw(&q,g_game.hist[ply].move);return countPieces(&q,!moverWhite);
}
static const wchar_t* reviewClassName(int c){
    switch(c){case RV_BRILLIANT:return L"Brilliant";case RV_GREAT:return L"Great";case RV_BOOK:return L"Book";case RV_BEST:return L"Best";case RV_EXCELLENT:return L"Excellent";case RV_GOOD:return L"Good";case RV_INACCURACY:return L"Inaccuracy";case RV_MISTAKE:return L"Mistake";case RV_MISS:return L"Miss";case RV_BLUNDER:return L"Blunder";case RV_MATE:return L"Mate";}return L"--";
}
static const wchar_t* logTagName(int c){const wchar_t* n=reviewClassName(c);return c==RV_NONE?L"":n;}
static DWORD logTagColor(int c){
    if(c==RV_BLUNDER)return RGBc(206,55,55);if(c==RV_MISS)return RGBc(218,74,64);if(c==RV_MISTAKE)return RGBc(221,126,48);if(c==RV_INACCURACY)return RGBc(221,167,35);if(c==RV_BRILLIANT)return RGBc(34,176,151);if(c==RV_BOOK)return RGBc(174,126,91);if(c==RV_GREAT)return RGBc(76,133,178);if(c==RV_BEST)return RGBc(75,151,56);if(c==RV_EXCELLENT)return RGBc(103,159,72);if(c==RV_GOOD)return RGBc(108,142,91);if(c==RV_MATE)return RGBc(45,96,42);return RGBc(108,117,125);
}
static BYTE classifyMoveQuality(int ply,int before,int after,int* lossOut){
    int loss=before-after;if(loss<0)loss=0;if(loss>9999)loss=9999;if(lossOut)*lossOut=loss;
    const char* san=(ply>=0&&ply<g_game.histCount)?g_game.hist[ply].san:"";
    if(aFind(san,"#")>=0)return RV_MATE;
    if(isBookPly(ply)&&loss<=90)return RV_BOOK;
    if(before>=350&&after<150&&loss>=180)return RV_MISS;
    bool tactical=aFind(san,"x")>=0||aFind(san,"+")>=0;
    if(loss<=8&&tactical&&after>=300&&aFind(san,"x")>=0&&aFind(san,"+")>=0)return RV_BRILLIANT;
    if(loss<=12&&tactical&&after>=180)return RV_GREAT;
    if(loss<=18)return RV_BEST;if(loss<=45)return RV_EXCELLENT;if(loss<=95)return RV_GOOD;if(loss<=165)return RV_INACCURACY;if(loss<=300)return RV_MISTAKE;return RV_BLUNDER;
}
static void classifyLivePly(int ply){
    if(ply<0||ply>=g_game.histCount||ply+1>512||!g_liveEvalKnown[ply]||!g_liveEvalKnown[ply+1])return;
    bool moverWhite=(ply%2)==0;int before=moverWhite?g_liveEvalWhite[ply]:-g_liveEvalWhite[ply];int after=moverWhite?g_liveEvalWhite[ply+1]:-g_liveEvalWhite[ply+1];
    int loss=0;g_reviewClass[ply]=classifyMoveQuality(ply,before,after,&loss);g_reviewLoss[ply]=loss;invalidateGameLog();
}
static void storeLiveEvalForCurrentPosition(){
    int ply=g_game.histCount;if(ply<0||ply>512)return;int raw=g_engineMate?(g_engineMate>0?100000:-100000):g_engineScoreCp;
    g_liveEvalWhite[ply]=g_game.pos.whiteToMove?raw:-raw;g_liveEvalKnown[ply]=1;if(ply>0)classifyLivePly(ply-1);
}
static void resetLiveMoveTags(){memset(g_liveEvalWhite,0,sizeof(g_liveEvalWhite));memset(g_liveEvalKnown,0,sizeof(g_liveEvalKnown));memset(g_reviewClass,0,sizeof(g_reviewClass));memset(g_reviewLoss,0,sizeof(g_reviewLoss));resetTrainingTracking();}
static void computeReviewClasses(){
    memset(g_reviewClass,0,sizeof(g_reviewClass));memset(g_reviewLoss,0,sizeof(g_reviewLoss));int n=g_reviewPlyCount;if(n>g_game.histCount)n=g_game.histCount;
    for(int i=0;i<n;i++){
        bool moverWhite=(i%2)==0;int before=moverWhite?g_reviewEvalWhite[i]:-g_reviewEvalWhite[i];int after=moverWhite?g_reviewEvalWhite[i+1]:-g_reviewEvalWhite[i+1];int loss=0;g_reviewClass[i]=classifyMoveQuality(i,before,after,&loss);g_reviewLoss[i]=loss;
    }
    g_reviewReady=true;g_reviewInProgress=false;
}
static void requestGameReview(){
    if(g_game.histCount<=0||!g_engineReady){g_reviewInProgress=false;g_reviewReady=false;return;}g_reviewInProgress=true;g_reviewReady=false;setStatus(L"Game complete. Reviewing the moves with Stockfish...");requestAnalysis(g_positionVersion,REQ_REVIEW);invalidateAnalysis();
}
static int worstReviewedPly(bool forUser){
    int best=-1,bestLoss=-1;for(int i=0;i<g_game.histCount&&i<g_reviewPlyCount;i++){bool userMove=((i&1)==0)==g_userWhite;if(userMove!=forUser)continue;if(g_reviewLoss[i]>bestLoss){bestLoss=g_reviewLoss[i];best=i;}}return bestLoss>=90?best:-1;
}
static void countReviewForSide(bool forUser,int counts[12]){
    for(int i=0;i<12;i++)counts[i]=0;for(int i=0;i<g_game.histCount&&i<g_reviewPlyCount;i++){bool userMove=((i&1)==0)==g_userWhite;if(userMove!=forUser)continue;int c=g_reviewClass[i];if(c>=0&&c<12)counts[c]++;}
}

static void humanMoveDescription(const Position* p,const Move& m,wchar_t out[256]);
static void appendCp(wchar_t* out,int cap,int cp);
static int estimatedNoAssistWinChance();
static void invalidateBoard();
static void currentTrainingCounters(TrainingCounters* out){
    if(!out)return;memset(out,0,sizeof(*out));out->rejectedSuggestions=g_trainingRejectedTotal;out->askRejected=g_trainingAskRejected;out->recoveryRejected=g_trainingRecoveryRejected;out->emergencyRejected=g_trainingEmergencyRejected;out->fullAssistRejected=g_trainingFullAssistRejected;out->fullAssistOnCount=g_fullAssistOnCount;out->fullAssistOffCount=g_fullAssistOffCount;out->fullTrainerRejected=g_trainingFullTrainerRejected;out->coachReviewed=g_trainingCoachReviewed;
    for(int i=0;i<g_game.histCount&&i<512;i++){bool userMove=((i&1)==0)==g_userWhite;if(!userMove)continue;out->playerMoves++;int source=g_trainingSource[i];if(source==TRAIN_NONE)source=g_assistAccepted[i]?TRAIN_RECOVERY:TRAIN_INDEPENDENT;if(trainingSourceIsAssisted(source)){out->assistedMoves++;if(source==TRAIN_ASK)out->askAccepted++;else if(source==TRAIN_RECOVERY)out->recoveryAccepted++;else if(source==TRAIN_EMERGENCY)out->emergencyAccepted++;else if(source==TRAIN_FULL_ASSIST)out->fullAssistMoves++;else if(source==TRAIN_FULL_TRAINER)out->fullTrainerAccepted++;}else out->independentMoves++;}
    out->dependencyPercent=out->playerMoves?(out->assistedMoves*100+out->playerMoves/2)/out->playerMoves:0;out->independentPercent=out->playerMoves?(out->independentMoves*100+out->playerMoves/2)/out->playerMoves:100;
}
static void currentAssistanceMetrics(int* userMoves,int* assistedMoves,int* independentMoves,int* independentPercent){TrainingCounters c;currentTrainingCounters(&c);if(userMoves)*userMoves=c.playerMoves;if(assistedMoves)*assistedMoves=c.assistedMoves;if(independentMoves)*independentMoves=c.independentMoves;if(independentPercent)*independentPercent=c.independentPercent;}
static bool assistedCanAcceptNextCoachMove(){int total=0,assisted=0,independent=0,pct=100;currentAssistanceMetrics(&total,&assisted,&independent,&pct);return independent*2>=total+1;}
static void assistedIndependentFloorMessage(){
    // This safeguard applies only to normal Assisted Mode. Temporary Full Trainer
    // (Full Assist ON) is intentionally outside this 50% limit.
    if(g_assistEmergency&&g_assistEmergencyRemaining>0){g_assistEmergencyRemaining--;g_assistEmergencyUsed++;debugAppendRaw("ASSIST-GUARD","Emergency rescue slot consumed because accepting it would violate the 50% independent-play floor.");}
    else if(g_assistRecoveryRemaining>0){g_assistRecoveryRemaining--;debugAppendRaw("ASSIST-GUARD","Recovery Coach slot consumed because accepting it would violate the 50% independent-play floor.");}
    setStatus(L"Independent-play safeguard: another coach move would reduce Assisted Mode below 50% independent play. This automatic assist slot has been skipped; play this turn yourself.");
    debugAppendRaw("ASSIST-GUARD","Coach move blocked to preserve the 50% minimum independent-play floor.");
}

static int trainingModeDetailRows(){int m=g_gameBaseMode;if(m==MODE_FULL)return 1;if(m==MODE_ASSISTED)return 4;if(m==MODE_COACH)return 1;if(m==MODE_PRACTICE)return 1;if(m==MODE_FAIRPLAY)return 1;return 0;}
static int insightSectionGap(bool compact){return compact?11:17;}
static int trainingCoreHeight(bool compact){int headerH=compact?24:28,rowH=compact?21:24;return headerH+7*rowH;}
static int trainingModeHeight(bool compact){int headerH=compact?24:28,detailH=compact?21:24;return headerH+trainingModeDetailRows()*detailH;}
static int trainingMetricsHeight(bool compact){return trainingCoreHeight(compact)+insightSectionGap(compact)+trainingModeHeight(compact);}
static void drawInsightGroupLabel(HDC dc,int y,const wchar_t* text){
    SelectObject(dc,g_buttonFont);SetTextColor(dc,RGBc(63,74,81));RECT r={EVAL_X,y,EVAL_X+EVAL_W,y+24};DrawTextW(dc,(LPWSTR)text,-1,&r,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    HPEN p=CreatePen(PS_SOLID,1,RGBc(226,231,234));HGDIOBJ op=SelectObject(dc,p);MoveToEx(dc,EVAL_X,y+23,NULLPTR);LineTo(dc,EVAL_X+EVAL_W,y+23);SelectObject(dc,op);DeleteObject(p);
}
static void drawSectionCard(HDC dc,int x0,int y,int x2,int h,DWORD fill=0){
    if(fill==0)fill=RGBc(252,253,253);HBRUSH card=CreateSolidBrush(fill);HPEN edge=CreatePen(PS_SOLID,1,RGBc(213,220,224));HGDIOBJ oldB=SelectObject(dc,card),oldP=SelectObject(dc,edge);RoundRect(dc,x0,y,x2,y+h,9,9);SelectObject(dc,oldP);SelectObject(dc,oldB);DeleteObject(edge);DeleteObject(card);
}
static void drawTrainingMetrics(HDC dc,int y,bool compact){
    TrainingCounters c;currentTrainingCounters(&c);int x0=EVAL_X,x2=EVAL_X+EVAL_W,metricW=EVAL_W*72/100,x1=x0+metricW,headerH=compact?24:28,rowH=compact?21:24,detailH=compact?21:24,coreH=trainingCoreHeight(compact),gap=insightSectionGap(compact),modeH=trainingModeHeight(compact);
    drawSectionCard(dc,x0,y,x2,coreH);
    HBRUSH hb=CreateSolidBrush(RGBc(237,243,234));RECT hr={x0+1,y+1,x2-1,y+headerH};FillRect(dc,&hr,hb);DeleteObject(hb);SelectObject(dc,g_buttonFont);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGBc(55,70,61));RECT h0={x0+9,y,x1-4,y+headerH};DrawTextW(dc,(LPWSTR)L"Training & Dependency",-1,&h0,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);RECT h1={x1,y,x2-6,y+headerH};DrawTextW(dc,(LPWSTR)L"Value",-1,&h1,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    const wchar_t* labels[7]={L"Player moves",L"Independent moves",L"Trainer-assisted moves",L"Rejected suggestions",L"Assistance dependency",L"Independent play",L"Est. no-assist win chance"};wchar_t vals[7][32];memset(vals,0,sizeof(vals));wAppendInt(vals[0],32,c.playerMoves);wAppendInt(vals[1],32,c.independentMoves);wAppendInt(vals[2],32,c.assistedMoves);wAppendInt(vals[3],32,c.rejectedSuggestions);wAppendInt(vals[4],32,c.dependencyPercent);wAppend(vals[4],32,L"%");wAppendInt(vals[5],32,c.independentPercent);wAppend(vals[5],32,L"%");wAppendInt(vals[6],32,estimatedNoAssistWinChance());wAppend(vals[6],32,L"%");
    SelectObject(dc,g_uiFont);for(int i=0;i<7;i++){int ry=y+headerH+i*rowH;if(i&1){HBRUSH ab=CreateSolidBrush(RGBc(247,249,250));RECT ar={x0+1,ry,x2-1,ry+rowH};FillRect(dc,&ar,ab);DeleteObject(ab);}SetTextColor(dc,RGBc(63,74,81));RECT lr={x0+9,ry,x1-5,ry+rowH};DrawTextW(dc,(LPWSTR)labels[i],-1,&lr,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);SetTextColor(dc,RGBc(45,96,42));RECT vr={x1,ry,x2-7,ry+rowH};DrawTextW(dc,vals[i],-1,&vr,DT_CENTER|DT_SINGLELINE|DT_VCENTER);}
    HPEN p=CreatePen(PS_SOLID,1,RGBc(224,229,232));HGDIOBJ op=SelectObject(dc,p);MoveToEx(dc,x1,y+headerH,NULLPTR);LineTo(dc,x1,y+coreH);for(int i=0;i<=7;i++){int yy=y+headerH+i*rowH;MoveToEx(dc,x0+1,yy,NULLPTR);LineTo(dc,x2-1,yy);}SelectObject(dc,op);DeleteObject(p);

    int my=y+coreH+gap;drawSectionCard(dc,x0,my,x2,modeH,RGBc(249,251,252));HBRUSH mb=CreateSolidBrush(RGBc(244,247,249));RECT mhr={x0+1,my+1,x2-1,my+headerH};FillRect(dc,&mhr,mb);DeleteObject(mb);SelectObject(dc,g_buttonFont);SetTextColor(dc,RGBc(63,74,81));RECT mh={x0+9,my,x2-8,my+headerH};DrawTextW(dc,(LPWSTR)L"Mode-specific Activity",-1,&mh,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    wchar_t detail[4][160];for(int i=0;i<4;i++)detail[i][0]=0;int dn=0;
    if(g_gameBaseMode==MODE_FULL){wAppend(detail[dn],160,L"Full Trainer recommendations: ");wAppendInt(detail[dn],160,c.fullTrainerAccepted);wAppend(detail[dn],160,L" accepted, ");wAppendInt(detail[dn],160,c.fullTrainerRejected);wAppend(detail[dn++],160,L" rejected");}
    else if(g_gameBaseMode==MODE_ASSISTED){wAppend(detail[dn],160,L"Ask Trainer: ");wAppendInt(detail[dn],160,c.askAccepted);wAppend(detail[dn],160,L" accepted, ");wAppendInt(detail[dn],160,c.askRejected);wAppend(detail[dn++],160,L" rejected");wAppend(detail[dn],160,L"Recovery Coach: ");wAppendInt(detail[dn],160,c.recoveryAccepted);wAppend(detail[dn],160,L" accepted, ");wAppendInt(detail[dn],160,c.recoveryRejected);wAppend(detail[dn++],160,L" rejected");wAppend(detail[dn],160,L"Emergency Rescue: ");wAppendInt(detail[dn],160,c.emergencyAccepted);wAppend(detail[dn],160,L" accepted, ");wAppendInt(detail[dn],160,c.emergencyRejected);wAppend(detail[dn++],160,L" rejected");wAppend(detail[dn],160,L"Full Assist: ");wAppendInt(detail[dn],160,c.fullAssistOnCount);wAppend(detail[dn],160,L" ON, ");wAppendInt(detail[dn],160,c.fullAssistOffCount);wAppend(detail[dn],160,L" OFF, ");wAppendInt(detail[dn],160,c.fullAssistMoves);wAppend(detail[dn++],160,L" moves");}
    else if(g_gameBaseMode==MODE_COACH){wAppend(detail[dn],160,L"Coach-reviewed moves: ");wAppendInt(detail[dn++],160,c.coachReviewed);}
    else if(g_gameBaseMode==MODE_PRACTICE){wAppend(detail[dn++],160,L"Computer opponent moves are excluded from assistance.");}
    else if(g_gameBaseMode==MODE_FAIRPLAY){wAppend(detail[dn++],160,L"No live assistance is available in Fair Play.");}
    SelectObject(dc,g_uiFont);for(int i=0;i<dn;i++){if(i&1){HBRUSH ab=CreateSolidBrush(RGBc(246,249,250));RECT ar={x0+1,my+headerH+i*detailH,x2-1,my+headerH+(i+1)*detailH};FillRect(dc,&ar,ab);DeleteObject(ab);}SetTextColor(dc,RGBc(78,88,94));RECT rr={x0+9,my+headerH+i*detailH,x2-8,my+headerH+(i+1)*detailH};DrawTextW(dc,detail[i],-1,&rr,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);}
}

static int userWinChancePercent(){
    int raw=g_userWhite?g_whiteWin:g_blackWin;int p=(raw+5)/10;if(p<0)p=0;if(p>100)p=100;return p;
}
static int estimatedNoAssistWinChance(){
    int base=userWinChancePercent();int total=0,assisted=0,ind=0,pct=100;currentAssistanceMetrics(&total,&assisted,&ind,&pct);
    int sumLoss=0,count=0;
    for(int i=0;i<g_game.histCount&&i<512;i++){bool userMove=((i&1)==0)==g_userWhite;if(!userMove||g_assistAccepted[i]||g_reviewClass[i]==RV_NONE)continue;int l=g_reviewLoss[i];if(l>=0&&l<9000){sumLoss+=l;count++;}}
    int avg=count?sumLoss/count:80;if(avg<35)avg=35;if(avg>240)avg=240;
    int penalty=(assisted*avg+19)/20;int est=base-penalty;if(est<0)est=0;if(est>100)est=100;return est;
}
static void clearMoveExplanation(){g_moveExplainActive=false;invalidateBoard();}
static void buildMoveExplanation(const Position* before,const Move& played,int cls,int loss){
    g_moveExplainActive=false;g_moveExplainClass=cls;g_moveExplainLoss=loss;g_moveExplainPlayed=played;g_moveExplainHasBetter=false;g_moveExplainHasReply=false;g_moveExplainText[0]=0;
    Move best,reply;if(g_assistBest[0]&&findLegalByUCIForPosition(before,g_assistBest,&best)){g_moveExplainBetter=best;g_moveExplainHasBetter=!moveEq(best,played);}
    Position after=*before;applyMoveRaw(&after,played);if(g_assistOpponentReply[0]&&findLegalByUCIForPosition(&after,g_assistOpponentReply,&reply)){g_moveExplainReply=reply;g_moveExplainHasReply=true;}
    const wchar_t* label=reviewClassName(cls);wAppend(g_moveExplainText,620,label);wAppend(g_moveExplainText,620,L": ");
    if(cls==RV_MISS)wAppend(g_moveExplainText,620,L"you missed a stronger opportunity");
    else if(cls==RV_BLUNDER)wAppend(g_moveExplainText,620,L"this move creates a major tactical or positional loss");
    else if(cls==RV_MISTAKE)wAppend(g_moveExplainText,620,L"this move gives the opponent a clear advantage");
    else wAppend(g_moveExplainText,620,L"this move is less accurate than your best available choice");
    if(loss>0&&loss<9000){wAppend(g_moveExplainText,620,L" (about ");appendCp(g_moveExplainText,620,loss);wAppend(g_moveExplainText,620,L" pawns).");}else wAppend(g_moveExplainText,620,L".");
    if(g_moveExplainHasReply){wchar_t d[256]={0};humanMoveDescription(&after,g_moveExplainReply,d);wAppend(g_moveExplainText,620,L" Opponent's strongest reply: ");wAppend(g_moveExplainText,620,d);wAppend(g_moveExplainText,620,L".");}
    if(g_moveExplainHasBetter){wchar_t d[256]={0};humanMoveDescription(before,g_moveExplainBetter,d);wAppend(g_moveExplainText,620,L" Better was: ");wAppend(g_moveExplainText,620,d);wAppend(g_moveExplainText,620,L".");}
    g_moveExplainActive=true;debugAppendWide("MOVE-EXPLAIN",g_moveExplainText);
}
static void setStatus(const wchar_t* s){SetWindowTextW(hStatus,s);debugAppendWide("STATUS",s);}
// Redraw helpers. Keeping invalidation local is important: repainting the entire
// parent window after every click caused child controls to visually flash on some
// Windows/GPU combinations. Child windows are also clipped from parent painting.
static void invalidateAll(){InvalidateRect(g_mainHwnd,NULLPTR,FALSE);}
static void invalidateBoard(){
    RECT r={BOARD_X-8,BOARD_Y-8,BOARD_X+BOARD_SIZE+8,BOARD_Y+BOARD_SIZE+8};
    InvalidateRect(g_mainHwnd,&r,FALSE);
}
static void invalidatePanelContent(){
    RECT r={PANEL_X,PANEL_TOP,PANEL_X+PANEL_W,PANEL_BOTTOM};
    InvalidateRect(g_mainHwnd,&r,FALSE);
}
static void invalidateAnalysis(){invalidatePanelContent();}
static void invalidateGameLog(){invalidatePanelContent();}
static void invalidateBoardAndAnalysis(){invalidateBoard();invalidateAnalysis();}
static bool boardInputAllowed(){
    if(!g_userSet||!g_engineReady||g_gameOver||g_calculating)return false;
    if(g_trainerMode==MODE_FULL)return !g_suggestionPending;
    if(g_trainerMode==MODE_ASSISTED)return !g_suggestionPending;
    if(g_trainerMode==MODE_PRACTICE||g_trainerMode==MODE_COACH)return g_game.pos.whiteToMove==g_userWhite;
    return true; // Fair Play records both players' real moves.
}
static wchar_t g_copyBuffer[32768];
static bool copyWideTextToClipboard(const wchar_t* text){
    if(!text||!OpenClipboard(g_mainHwnd))return false;EmptyClipboard();ULONG_PTR bytes=(ULONG_PTR)(wLen(text)+1)*sizeof(wchar_t);HANDLE mem=GlobalAlloc(GMEM_MOVEABLE,bytes);if(!mem){CloseClipboard();return false;}void* p=GlobalLock(mem);if(!p){CloseClipboard();return false;}memcpy(p,text,(unsigned long long)bytes);GlobalUnlock(mem);if(!SetClipboardData(CF_UNICODETEXT,mem)){CloseClipboard();return false;}CloseClipboard();return true;
}
static void appendCopyLine(wchar_t* out,int cap,const wchar_t* label,const wchar_t* value){wAppend(out,cap,label);wAppend(out,cap,L": ");wAppend(out,cap,value);wAppend(out,cap,L"\r\n");}
static void appendMoveListForCopy(wchar_t* out,int cap){
    for(int i=0;i<g_game.histCount;i+=2){wAppendInt(out,cap,i/2+1);wAppend(out,cap,L". ");wAppendAscii(out,cap,g_game.hist[i].san);if(i+1<g_game.histCount){wAppend(out,cap,L"   ");wAppendAscii(out,cap,g_game.hist[i+1].san);}wAppend(out,cap,L"\r\n");}
}
static void buildPanelCopyText(bool insights,wchar_t* out,int cap){
    out[0]=0;char on[64],eco[8];recognizeOpening(on,64,eco,8);wchar_t tmp[256]={0};
    wAppend(out,cap,L"Chess Trainer 6.0.0\r\n");wAppend(out,cap,insights?L"Game Insights\r\n":L"Analysis / Game Log\r\n");wAppend(out,cap,L"==============================\r\n");
    wCopy(tmp,256,activeProfileName());appendCopyLine(out,cap,L"Profile",tmp);
    wCopy(tmp,256,trainerModeName(g_gameBaseMode));if(g_gameBaseMode==MODE_PRACTICE){wAppend(tmp,256,L" ");wAppendInt(tmp,256,g_practiceElo);}else if(g_gameBaseMode==MODE_COACH){wAppend(tmp,256,L" ");wAppendInt(tmp,256,g_coachElo);}appendCopyLine(out,cap,L"Mode",tmp);
    tmp[0]=0;wAppendAscii(tmp,256,on);if(eco[0]&&eco[0]!='-'){wAppend(tmp,256,L" (");wAppendAscii(tmp,256,eco);wAppend(tmp,256,L")");}appendCopyLine(out,cap,L"Opening",tmp);
    wCopy(tmp,256,g_userSet?(g_userWhite?L"White":L"Black"):L"Not selected");appendCopyLine(out,cap,L"Playing as",tmp);
    tmp[0]=0;wAppendInt(tmp,256,(g_game.histCount+1)/2);appendCopyLine(out,cap,L"Moves",tmp);
    if(g_gameOver){tmp[0]=0;wAppend(tmp,256,resultLabel(g_currentResult));wAppend(tmp,256,L" - ");wAppend(tmp,256,reasonLabel(g_currentReason));appendCopyLine(out,cap,L"Result",tmp);}else appendCopyLine(out,cap,L"Status",g_userSet?L"In progress":L"Not started");
    U64 u=0,o=0,t=0;currentGameClock(&u,&o,&t);wchar_t tu[32]={0},to[32]={0},tt[32]={0};formatDurationMs(u,tu);formatDurationMs(o,to);formatDurationMs(t,tt);tmp[0]=0;wAppend(tmp,256,L"Total ");wAppend(tmp,256,tt);wAppend(tmp,256,L" | You ");wAppend(tmp,256,tu);wAppend(tmp,256,L" | Opp ");wAppend(tmp,256,to);appendCopyLine(out,cap,L"Game time",tmp);
    if(insights){
        TrainingCounters c;currentTrainingCounters(&c);wAppend(out,cap,L"\r\nTraining / dependency\r\n");
        tmp[0]=0;wAppendInt(tmp,256,c.playerMoves);appendCopyLine(out,cap,L"Player moves",tmp);tmp[0]=0;wAppendInt(tmp,256,c.independentMoves);appendCopyLine(out,cap,L"Independent moves",tmp);tmp[0]=0;wAppendInt(tmp,256,c.assistedMoves);appendCopyLine(out,cap,L"Trainer-assisted moves",tmp);tmp[0]=0;wAppendInt(tmp,256,c.rejectedSuggestions);appendCopyLine(out,cap,L"Rejected suggestions",tmp);tmp[0]=0;wAppendInt(tmp,256,c.dependencyPercent);wAppend(tmp,256,L"%");appendCopyLine(out,cap,L"Assistance dependency",tmp);tmp[0]=0;wAppendInt(tmp,256,c.independentPercent);wAppend(tmp,256,L"%");appendCopyLine(out,cap,L"Independent play",tmp);tmp[0]=0;wAppendInt(tmp,256,estimatedNoAssistWinChance());wAppend(tmp,256,L"%");appendCopyLine(out,cap,L"Est. no-assist win chance",tmp);
        if(g_gameBaseMode==MODE_FULL){tmp[0]=0;wAppendInt(tmp,256,c.fullTrainerAccepted);wAppend(tmp,256,L" accepted / ");wAppendInt(tmp,256,c.fullTrainerRejected);wAppend(tmp,256,L" rejected");appendCopyLine(out,cap,L"Full Trainer recommendations",tmp);}
        else if(g_gameBaseMode==MODE_ASSISTED){tmp[0]=0;wAppendInt(tmp,256,c.askAccepted);wAppend(tmp,256,L" accepted / ");wAppendInt(tmp,256,c.askRejected);wAppend(tmp,256,L" rejected");appendCopyLine(out,cap,L"Ask Trainer",tmp);tmp[0]=0;wAppendInt(tmp,256,c.recoveryAccepted);wAppend(tmp,256,L" accepted / ");wAppendInt(tmp,256,c.recoveryRejected);wAppend(tmp,256,L" rejected");appendCopyLine(out,cap,L"Recovery Coach",tmp);tmp[0]=0;wAppendInt(tmp,256,c.emergencyAccepted);wAppend(tmp,256,L" accepted / ");wAppendInt(tmp,256,c.emergencyRejected);wAppend(tmp,256,L" rejected");appendCopyLine(out,cap,L"Emergency Rescue",tmp);tmp[0]=0;wAppendInt(tmp,256,c.fullAssistOnCount);wAppend(tmp,256,L" ON / ");wAppendInt(tmp,256,c.fullAssistOffCount);wAppend(tmp,256,L" OFF / ");wAppendInt(tmp,256,c.fullAssistMoves);wAppend(tmp,256,L" moves");appendCopyLine(out,cap,L"Full Assist",tmp);}
        else if(g_gameBaseMode==MODE_COACH){tmp[0]=0;wAppendInt(tmp,256,c.coachReviewed);appendCopyLine(out,cap,L"Coach-reviewed moves",tmp);}
        wAppend(out,cap,L"\r\nTrainer Review\r\n");int uc[12],oc[12];countReviewForSide(true,uc);countReviewForSide(false,oc);const int classes[10]={RV_BRILLIANT,RV_GREAT,RV_BOOK,RV_BEST,RV_EXCELLENT,RV_GOOD,RV_INACCURACY,RV_MISTAKE,RV_MISS,RV_BLUNDER};for(int k=0;k<10;k++){int c=classes[k];tmp[0]=0;wAppendInt(tmp,256,uc[c]);wAppend(tmp,256,L" / ");wAppendInt(tmp,256,oc[c]);appendCopyLine(out,cap,reviewClassName(c),tmp);}
    }
    wAppend(out,cap,L"\r\nMoves\r\n");appendMoveListForCopy(out,cap);
}
static void copyCurrentPanel(){bool insights=(g_panelMode==1);buildPanelCopyText(insights,g_copyBuffer,32768);if(copyWideTextToClipboard(g_copyBuffer)){debugAppendRaw("COPY",insights?"Game Insights copied to clipboard.":"Analysis/Game Log copied to clipboard.");setStatus(insights?L"Game Insights copied to clipboard.":L"Analysis and Game Log copied to clipboard.");}else{debugAppendRaw("COPY-ERROR","Could not open the Windows clipboard.");MessageBoxW(g_mainHwnd,L"Chess Trainer could not access the Windows clipboard.",L"Copy",MB_OK|MB_ICONWARNING);}}

static void setEnabledStable(HWND hwnd,bool enabled){
    // Avoid sending WM_ENABLE to a control whose state has not changed. Re-enabling
    // every button after every move caused unnecessary child repaints on click.
    if(!hwnd)return;bool current=IsWindowEnabled(hwnd)!=0;if(current!=enabled)EnableWindow(hwnd,enabled?TRUE:FALSE);
}
static int currentActionPhase(){
    // Full Trainer keeps the original green/red turn-state behaviour. Assisted
    // Practice uses green only while Recovery Coach is actively offering a best move.
    if(!g_userSet||g_gameOver)return 0;
    if(g_trainerMode==MODE_FULL)return g_game.pos.whiteToMove==g_userWhite?1:2;
    if(g_trainerMode==MODE_ASSISTED&&g_suggestionPending&&g_game.pos.whiteToMove==g_userWhite)return 1;
    return 0;
}
static void updateControls(){
    bool completedInsights=(g_panelMode==1&&g_gameOver);int visibilityState=completedInsights?1:0;if(hCopyPanel){bool showCopy=(g_panelMode==0||g_panelMode==1);ShowWindow(hCopyPanel,showCopy?SW_SHOW:SW_HIDE);SetWindowTextW(hCopyPanel,g_panelMode==1?L"Copy Insights":L"Copy Analysis");setEnabledStable(hCopyPanel,showCopy);}
    bool showEndGame=(g_panelMode==1&&g_userSet&&!g_gameOver);for(int i=0;i<8;i++){if(hEndGame[i])ShowWindow(hEndGame[i],showEndGame?SW_SHOW:SW_HIDE);setEnabledStable(hEndGame[i],showEndGame);}
    if(g_completedInsightsVisibilityState!=visibilityState){g_completedInsightsVisibilityState=visibilityState;if(hSan)ShowWindow(hSan,completedInsights?SW_HIDE:SW_SHOW);if(hEnter)ShowWindow(hEnter,completedInsights?SW_HIDE:SW_SHOW);if(hUndo)ShowWindow(hUndo,completedInsights?SW_HIDE:SW_SHOW);if(hCorrect)ShowWindow(hCorrect,completedInsights?SW_HIDE:SW_SHOW);if(hConfirm)ShowWindow(hConfirm,completedInsights?SW_HIDE:SW_SHOW);if(hDifferent)ShowWindow(hDifferent,completedInsights?SW_HIDE:SW_SHOW);if(hNew)ShowWindow(hNew,SW_SHOW);if(hFlip)ShowWindow(hFlip,SW_SHOW);}
    bool canSelect=g_engineReady&&!g_userSet&&!g_calculating;setEnabledStable(hWhite,canSelect);setEnabledStable(hBlack,canSelect);setEnabledStable(hProfile,!g_calculating);
    setEnabledStable(hModeFull,!g_calculating);setEnabledStable(hModeAssisted,!g_calculating);setEnabledStable(hModePractice,!g_calculating);setEnabledStable(hModeFair,!g_calculating);setEnabledStable(hModeCoach,!g_calculating);
    // Full Assist is now a temporary bridge into genuine Full Trainer behaviour.
    // Keep the ON/OFF controls visible while that temporary Full Trainer state is
    // active so the user can return to normal Assisted Mode without ending the game.
    bool showAssistSwitch=((g_trainerMode==MODE_ASSISTED)||(g_trainerMode==MODE_FULL&&g_assistFull))&&g_panelMode==0&&!g_gameOver;if(hAssistFullOn)ShowWindow(hAssistFullOn,showAssistSwitch?SW_SHOW:SW_HIDE);if(hAssistFullOff)ShowWindow(hAssistFullOff,showAssistSwitch?SW_SHOW:SW_HIDE);
    setEnabledStable(hAssistFullOn,showAssistSwitch&&!g_calculating&&!g_assistFull);setEnabledStable(hAssistFullOff,showAssistSwitch&&!g_calculating&&g_assistFull);
    bool input=boardInputAllowed();setEnabledStable(hSan,input);setEnabledStable(hEnter,input);
    bool pendingAction=(g_trainerMode==MODE_FULL||g_trainerMode==MODE_ASSISTED)&&g_suggestionPending&&!g_calculating&&!g_gameOver;
    bool canAsk=(g_trainerMode==MODE_ASSISTED&&g_userSet&&!g_gameOver&&!g_calculating&&!g_suggestionPending&&!g_choiceRequestPending&&!g_assistFull&&!g_assistEmergency&&g_assistRecoveryRemaining<=0&&g_game.pos.whiteToMove==g_userWhite&&assistedCanAcceptNextCoachMove());
    if(g_trainerMode==MODE_ASSISTED){bool guided=g_suggestionPending||g_assistFull||(g_assistEmergency&&g_assistEmergencyRemaining>0)||g_assistRecoveryRemaining>0;SetWindowTextW(hConfirm,guided?L"Suggested Move":L"Ask Trainer");}else SetWindowTextW(hConfirm,L"Suggested Move");
    setEnabledStable(hConfirm,pendingAction||canAsk);setEnabledStable(hDifferent,pendingAction);
    setEnabledStable(hUndo,g_userSet&&g_game.histCount>0&&!g_calculating&&!g_gameOver);
    bool correct=false;if(g_userSet&&!g_gameOver&&!g_calculating&&!g_suggestionPending&&g_game.histCount>0)correct=(g_game.hist[g_game.histCount-1].before.whiteToMove==g_userWhite);setEnabledStable(hCorrect,correct);
    setEnabledStable(hNew,g_engineReady&&!g_calculating);setEnabledStable(hFlip,true);

    int phase=currentActionPhase();
    if(phase!=g_actionPhase){g_actionPhase=phase;if(hConfirm)InvalidateRect(hConfirm,NULLPTR,FALSE);if(hDifferent)InvalidateRect(hDifferent,NULLPTR,FALSE);}
    if(hModeFull)InvalidateRect(hModeFull,NULLPTR,FALSE);if(hModeAssisted)InvalidateRect(hModeAssisted,NULLPTR,FALSE);if(hModePractice)InvalidateRect(hModePractice,NULLPTR,FALSE);if(hModeFair)InvalidateRect(hModeFair,NULLPTR,FALSE);if(hModeCoach)InvalidateRect(hModeCoach,NULLPTR,FALSE);if(hAssistFullOn)InvalidateRect(hAssistFullOn,NULLPTR,FALSE);if(hAssistFullOff)InvalidateRect(hAssistFullOff,NULLPTR,FALSE);
}


static void registerModernButtonClass();
static void layoutUI();

// --------------------------- Rating entry dialog ---------------------------
static HWND g_ratingHwnd=NULLPTR,g_ratingUserEdit=NULLPTR,g_ratingOppEdit=NULLPTR;
static bool g_ratingSaved=false;
static int parseWInt(const wchar_t* s){int v=0,n=0;for(int i=0;s&&s[i];i++){if(s[i]>='0'&&s[i]<='9'){v=v*10+(s[i]-'0');n++;if(v>10000)return 10000;}}return n?v:0;}
static bool handleEditShortcut(MSG* m,HWND a,HWND b,HWND c){
    if(!m||m->message!=WM_KEYDOWN||(GetKeyState(VK_CONTROL_KEY)&0x8000)==0)return false;HWND f=GetFocus();if(f!=a&&f!=b&&f!=c)return false;UINT key=(UINT)m->wParam;
    if(key==VK_A_KEY){SendMessageW(f,EM_SETSEL,0,(LPARAM)-1);return true;}if(key==VK_C_KEY){SendMessageW(f,WM_COPY,0,0);return true;}if(key==VK_X_KEY){SendMessageW(f,WM_CUT,0,0);return true;}if(key==VK_V_KEY){SendMessageW(f,WM_PASTE,0,0);return true;}return false;
}
static void setControlInt(HWND h,int v){wchar_t t[24]={0};wAppendInt(t,24,v);SetWindowTextW(h,t);}
static LRESULT CALLBACK ratingProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT){
        PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);HBRUSH bg=CreateSolidBrush(RGBc(238,241,244));FillRect(dc,&rc,bg);DeleteObject(bg);
        HBRUSH card=CreateSolidBrush(RGBc(248,249,250));HPEN edge=CreatePen(PS_SOLID,1,RGBc(211,217,222));HGDIOBJ ob=SelectObject(dc,card),op=SelectObject(dc,edge);RoundRect(dc,14,12,rc.right-14,rc.bottom-14,14,14);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(edge);DeleteObject(card);
        SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,28,24,g_gameOver?L"Correct Rating":L"Game Ratings",g_gameOver?14:12);
        HBRUSH accent=CreateSolidBrush(RGBc(115,149,82));RECT ar={28,52,128,55};FillRect(dc,&ar,accent);DeleteObject(accent);
        SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(82,92,99));const wchar_t* ownLabel=g_gameOver?L"Actual current Chess.com rating":(!g_guestProfile&&g_currentRating>0?L"Profile rating (saved)":L"Your current Chess.com rating");TextOutW(dc,28,69,ownLabel,wLen(ownLabel));TextOutW(dc,250,69,L"Opponent rating",15);
        HBRUSH info=CreateSolidBrush(RGBc(239,243,238));RECT ir={28,142,rc.right-28,196};FillRect(dc,&ir,info);DeleteObject(info);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));RECT tr={40,151,rc.right-40,190};DrawTextW(dc,(LPWSTR)L"Chess.com uses a Glicko-family rating system. Chess Trainer shows an estimate, and you can enter the actual rating after the game.",-1,&tr,DT_LEFT|DT_WORDBREAK|DT_VCENTER);
        EndPaint(hwnd,&ps);return 0;
    }
    if(msg==WM_COMMAND){int id=lowWord(w);if(id==3003){wchar_t a[32]={0},b[32]={0};GetWindowTextW(g_ratingUserEdit,a,32);GetWindowTextW(g_ratingOppEdit,b,32);int ur=parseWInt(a),op=parseWInt(b);if(ur>0){if(g_gameOver){g_currentRating=ur;g_lastRatingAfter=ur;g_lastRatingChange=g_ratingBeforeGame>0?ur-g_ratingBeforeGame:0;}else{g_currentRating=ur;g_ratingBeforeGame=ur;}saveRatingStore();}g_opponentRating=op;g_ratingSaved=true;DestroyWindow(hwnd);return 0;}if(id==3004){DestroyWindow(hwnd);return 0;}}
    if(msg==WM_CLOSE){DestroyWindow(hwnd);return 0;}if(msg==WM_DESTROY){g_ratingHwnd=NULLPTR;return 0;}return DefWindowProcW(hwnd,msg,w,l);
}
static void showRatingDialog(){
    static bool registered=false;if(!registered){WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=ratingProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=NULLPTR;wc.lpszClassName=L"ChessTrainerRatings";RegisterClassExW(&wc);registered=true;}
    registerModernButtonClass();g_ratingSaved=false;g_ratingHwnd=CreateWindowExW(WS_EX_CONTROLPARENT,L"ChessTrainerRatings",g_gameOver?L"Chess Trainer - Correct Rating":L"Chess Trainer - Game Ratings",WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_CLIPCHILDREN,410,210,500,310,g_mainHwnd,NULLPTR,g_hInstance,NULLPTR);if(!g_ratingHwnd)return;
    g_ratingUserEdit=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,28,96,196,30,g_ratingHwnd,(HMENU)(ULONG_PTR)3001,g_hInstance,NULLPTR);SendMessageW(g_ratingUserEdit,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(g_ratingUserEdit,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((10&0xFFFF)|((10&0xFFFF)<<16)));
    g_ratingOppEdit=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,250,96,196,30,g_ratingHwnd,(HMENU)(ULONG_PTR)3002,g_hInstance,NULLPTR);SendMessageW(g_ratingOppEdit,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(g_ratingOppEdit,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((10&0xFFFF)|((10&0xFFFF)<<16)));
    HWND save=CreateWindowExW(0,L"ChessTrainerModernButton",L"Save",WS_CHILD|WS_VISIBLE|WS_TABSTOP,250,214,94,38,g_ratingHwnd,(HMENU)(ULONG_PTR)3003,g_hInstance,NULLPTR);HWND skip=CreateWindowExW(0,L"ChessTrainerModernButton",L"Skip",WS_CHILD|WS_VISIBLE|WS_TABSTOP,352,214,94,38,g_ratingHwnd,(HMENU)(ULONG_PTR)3004,g_hInstance,NULLPTR);SendMessageW(save,WM_SETFONT,(WPARAM)g_buttonFont,1);SendMessageW(skip,WM_SETFONT,(WPARAM)g_buttonFont,1);
    int showUser=g_gameOver?g_currentRating:(g_ratingBeforeGame>0?g_ratingBeforeGame:g_currentRating);if(showUser>0)setControlInt(g_ratingUserEdit,showUser);if(g_opponentRating>0)setControlInt(g_ratingOppEdit,g_opponentRating);if(!g_gameOver&&!g_guestProfile&&g_currentRating>0)EnableWindow(g_ratingUserEdit,FALSE);
    SetFocus((!g_gameOver&&!g_guestProfile&&g_currentRating>0)?g_ratingOppEdit:g_ratingUserEdit);EnableWindow(g_mainHwnd,FALSE);MSG m;while(g_ratingHwnd&&GetMessageW(&m,NULLPTR,0,0)>0){if(handleEditShortcut(&m,g_ratingUserEdit,g_ratingOppEdit,NULLPTR))continue;if(!IsDialogMessageW(g_ratingHwnd,&m)){TranslateMessage(&m);DispatchMessageW(&m);}}EnableWindow(g_mainHwnd,TRUE);SetFocus(g_mainHwnd);invalidatePanelContent();
}

// --------------------------- Player Profiles ---------------------------
// Profile management is local-only. The profile manager never opens a socket and never
// contacts any service. Guest play deliberately avoids persistent career/history/rating data.
static HWND g_profileHwnd=NULLPTR,g_profileList=NULLPTR,g_profileNameEdit=NULLPTR,g_profileCurrentEdit=NULLPTR,g_profileDefaultEdit=NULLPTR;
static int g_profileManagerSelection=0;
static void newGame();

struct ProfileStats {
    int games,wins,draws,losses,noResult,mode[5];
    int archivedGames,userMoves,assistedMoves,reviewedMoves,positiveMoves;
    int recentDep[5],recentQuality[5],recentCount;
    int recentRating[5],recentRatingCount;
};
static bool positiveReviewClass(int c){return (c>=RV_BRILLIANT&&c<=RV_GOOD)||c==RV_MATE;}
static void computeProfileStats(DWORD pid,ProfileStats* st){
    memset(st,0,sizeof(*st));if(!pid||!g_profileGameMapLoaded)return;
    for(DWORD i=0;i<g_savedHistory.count;i++)if(g_profileGameMap.historyProfile[i]==pid){st->games++;int r=g_savedHistory.games[i].result;if(r==RESULT_WIN)st->wins++;else if(r==RESULT_DRAW)st->draws++;else if(r==RESULT_LOSS)st->losses++;else st->noResult++;int m=g_savedHistory.games[i].reserved;if(m>=0&&m<5)st->mode[m]++;}
    int revDep[5]={0},revQual[5]={0},revN=0;
    for(int idx=(int)g_archive.count-1;idx>=0;idx--){if(g_profileGameMap.archiveProfile[idx]!=pid)continue;ArchivedGame &a=g_archive.games[idx];st->archivedGames++;int gm=0,ga=0,gr=0,gq=0;if(a.hasMoves){for(int j=0;j<a.plies&&j<256;j++){bool user=((j&1)==0)==(a.userWhite!=0);if(!user)continue;gm++;if(a.assistAccepted[j])ga++;int c=a.reviewClass[j];if(a.reviewReady&&c>RV_NONE){gr++;if(positiveReviewClass(c))gq++;}}}st->userMoves+=gm;st->assistedMoves+=ga;st->reviewedMoves+=gr;st->positiveMoves+=gq;if(revN<5&&gm>0){revDep[revN]=ga*100/gm;revQual[revN]=gr?gq*100/gr:0;revN++;}}
    st->recentCount=revN;for(int i=0;i<revN;i++){st->recentDep[i]=revDep[revN-1-i];st->recentQuality[i]=revQual[revN-1-i];}
    int rr[5]={0},rn=0;for(int idx=(int)g_archive.count-1;idx>=0&&rn<5;idx--){if(g_profileGameMap.archiveProfile[idx]!=pid)continue;int r=g_archive.games[idx].ratingAfter;if(r>0)rr[rn++]=r;}st->recentRatingCount=rn;for(int i=0;i<rn;i++)st->recentRating[i]=rr[rn-1-i];
}
static void refreshProfileButton(){if(hProfile)SetWindowTextW(hProfile,activeProfileName());}
static bool profileSwitchBlocked(){return g_userSet&&!g_gameOver;}
static void profileManagerLoadSelection(){
    if(!g_profileList)return;int sel=(int)SendMessageW(g_profileList,LB_GETCURSEL,0,0);if(sel<0||sel>=(int)g_profiles.count)sel=0;g_profileManagerSelection=sel;PlayerProfile &p=g_profiles.profiles[sel];SetWindowTextW(g_profileNameEdit,p.name);if(p.currentRating>0)setControlInt(g_profileCurrentEdit,p.currentRating);else SetWindowTextW(g_profileCurrentEdit,L"");if(p.defaultRating>0)setControlInt(g_profileDefaultEdit,p.defaultRating);else SetWindowTextW(g_profileDefaultEdit,L"");if(g_profileHwnd)InvalidateRect(g_profileHwnd,NULLPTR,FALSE);
}
static void profileManagerRefreshList(int selectIndex){
    if(!g_profileList)return;SendMessageW(g_profileList,LB_RESETCONTENT,0,0);for(DWORD i=0;i<g_profiles.count;i++){wchar_t label[64]={0};if(!g_guestProfile&&g_profiles.profiles[i].id==g_activeProfileId)wAppend(label,64,L"* ");wAppend(label,64,g_profiles.profiles[i].name);SendMessageW(g_profileList,LB_ADDSTRING,0,(LPARAM)label);}if(selectIndex<0||selectIndex>=(int)g_profiles.count)selectIndex=0;SendMessageW(g_profileList,LB_SETCURSEL,(WPARAM)selectIndex,0);g_profileManagerSelection=selectIndex;profileManagerLoadSelection();
}
static void activatePersistentProfile(int idx){
    if(idx<0||idx>=(int)g_profiles.count)return;if(profileSwitchBlocked()){MessageBoxW(g_profileHwnd,L"Finish or cancel the current game before switching Player Profiles.",L"Player Profiles",MB_OK|MB_ICONINFORMATION);return;}
    g_guestProfile=false;g_activeProfileId=g_profiles.profiles[idx].id;g_profiles.activeId=g_activeProfileId;saveProfileStore();loadRatingStore();g_ratingBeforeGame=g_currentRating;g_lastRatingAfter=g_currentRating;g_opponentRating=0;g_historyDetail=false;g_historySelected=-1;refreshProfileButton();char q[180]={0};aAppend(q,180,"selected persistent profile id=");aAppendInt(q,180,(int)g_activeProfileId);debugAppendRaw("PROFILE",q);newGame();profileManagerRefreshList(idx);invalidatePanelContent();
}
static void activateGuestProfile(){
    if(profileSwitchBlocked()){MessageBoxW(g_profileHwnd,L"Finish or cancel the current game before switching to Guest.",L"Player Profiles",MB_OK|MB_ICONINFORMATION);return;}
    g_guestProfile=true;g_currentRating=0;g_ratingBeforeGame=0;g_lastRatingAfter=0;g_lastRatingChange=0;g_opponentRating=0;g_historyDetail=false;g_historySelected=-1;refreshProfileButton();debugAppendRaw("PROFILE","Guest session selected. Persistent career/history/rating writes are disabled.");newGame();profileManagerRefreshList(profileIndexById(g_activeProfileId));invalidatePanelContent();
}
static void saveSelectedProfileEdits(){
    if(profileSwitchBlocked()){MessageBoxW(g_profileHwnd,L"Profile details cannot be changed during a live game.",L"Player Profiles",MB_OK|MB_ICONINFORMATION);return;}int idx=g_profileManagerSelection;if(idx<0||idx>=(int)g_profiles.count)return;wchar_t name[32]={0},cur[24]={0},def[24]={0};GetWindowTextW(g_profileNameEdit,name,32);GetWindowTextW(g_profileCurrentEdit,cur,24);GetWindowTextW(g_profileDefaultEdit,def,24);if(!name[0]){MessageBoxW(g_profileHwnd,L"Enter a player name before saving.",L"Player Profiles",MB_OK|MB_ICONWARNING);return;}int c=parseWInt(cur),d=parseWInt(def);if(c<=0&&d>0)c=d;if(d<=0&&c>0)d=c;PlayerProfile &p=g_profiles.profiles[idx];wCopy(p.name,32,name);p.currentRating=c;p.defaultRating=d;saveProfileStore();if(!g_guestProfile&&p.id==g_activeProfileId){g_currentRating=p.currentRating;g_ratingBeforeGame=g_currentRating;g_lastRatingAfter=g_currentRating;saveRatingStore();refreshProfileButton();}profileManagerRefreshList(idx);debugAppendRaw("PROFILE","Player Profile details saved.");
}
static void addNewProfile(){
    if(profileSwitchBlocked()){MessageBoxW(g_profileHwnd,L"Finish or cancel the current game before creating a Player Profile.",L"Player Profiles",MB_OK|MB_ICONINFORMATION);return;}if(g_profiles.count>=12){MessageBoxW(g_profileHwnd,L"Chess Trainer currently supports up to 12 local Player Profiles.",L"Player Profiles",MB_OK|MB_ICONINFORMATION);return;}int idx=(int)g_profiles.count++;PlayerProfile &p=g_profiles.profiles[idx];memset(&p,0,sizeof(p));p.id=g_profiles.nextId++;if(!p.id)p.id=g_profiles.nextId++;wchar_t nm[32]={0};wAppend(nm,32,L"Player ");wAppendInt(nm,32,idx+1);wCopy(p.name,32,nm);SYSTEMTIME st;GetLocalTime(&st);p.createdYear=st.wYear;p.createdMonth=st.wMonth;p.createdDay=st.wDay;saveProfileStore();profileManagerRefreshList(idx);SetFocus(g_profileNameEdit);debugAppendRaw("PROFILE","New local Player Profile created.");
}
static void drawProfileStatCard(HDC dc,int x,int y,int w,int h,const wchar_t* title){
    drawSectionCard(dc,x,y,x+w,h,RGBc(252,253,252));HBRUSH accent=CreateSolidBrush(RGBc(115,149,82));RECT strip={x+1,y+1,x+5,y+h-1};FillRect(dc,&strip,accent);DeleteObject(accent);
    SelectObject(dc,g_actionButtonFont);SetTextColor(dc,RGBc(45,96,42));RECT tr={x+16,y+5,x+w-12,y+32};DrawTextW(dc,(LPWSTR)title,-1,&tr,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    HPEN p=CreatePen(PS_SOLID,1,RGBc(229,234,231));HGDIOBJ op=SelectObject(dc,p);MoveToEx(dc,x+14,y+34,NULLPTR);LineTo(dc,x+w-12,y+34);SelectObject(dc,op);DeleteObject(p);
}
static void drawProfileStatRow(HDC dc,int x,int y,int w,const wchar_t* label,const wchar_t* value,bool alternate=false){
    if(alternate){HBRUSH b=CreateSolidBrush(RGBc(248,250,249));RECT r={x+6,y,x+w-6,y+27};FillRect(dc,&r,b);DeleteObject(b);}
    SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(103,112,118));RECT l={x+16,y,x+150,y+27};DrawTextW(dc,(LPWSTR)label,-1,&l,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
    SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(42,52,58));RECT v={x+156,y,x+w-16,y+27};DrawTextW(dc,(LPWSTR)value,-1,&v,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
}
static void drawProfileManagerStats(HDC dc,int idx){
    if(idx<0||idx>=(int)g_profiles.count)return;PlayerProfile &p=g_profiles.profiles[idx];ProfileStats st;computeProfileStats(p.id,&st);int x=342,w=602;SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,x,270,L"Profile Statistics",18);
    int dep=st.userMoves?st.assistedMoves*100/st.userMoves:0,ind=st.userMoves?100-dep:100,quality=st.reviewedMoves?st.positiveMoves*100/st.reviewedMoves:0;wchar_t v[300]={0};
    drawProfileStatCard(dc,x,302,w,104,L"Career performance");v[0]=0;wAppendInt(v,300,st.games);wAppend(v,300,L" games   |   ");wAppendInt(v,300,st.wins);wAppend(v,300,L" W   ");wAppendInt(v,300,st.draws);wAppend(v,300,L" D   ");wAppendInt(v,300,st.losses);wAppend(v,300,L" L");drawProfileStatRow(dc,x,340,w,L"Career record",v,false);
    v[0]=0;wAppend(v,300,L"Dependency ");wAppendInt(v,300,dep);wAppend(v,300,L"%   |   Independent ");wAppendInt(v,300,ind);wAppend(v,300,L"%");drawProfileStatRow(dc,x,369,w,L"Playing style",v,true);
    drawProfileStatCard(dc,x,420,w,94,L"Modes and archive");v[0]=0;wAppend(v,300,L"Full ");wAppendInt(v,300,st.mode[MODE_FULL]);wAppend(v,300,L"   |   Assisted ");wAppendInt(v,300,st.mode[MODE_ASSISTED]);wAppend(v,300,L"   |   Practice ");wAppendInt(v,300,st.mode[MODE_PRACTICE]);wAppend(v,300,L"   |   Fair ");wAppendInt(v,300,st.mode[MODE_FAIRPLAY]);wAppend(v,300,L"   |   Coach ");wAppendInt(v,300,st.mode[MODE_COACH]);drawProfileStatRow(dc,x,456,w,L"Mode activity",v,false);
    v[0]=0;wAppendInt(v,300,st.archivedGames);wAppend(v,300,L" saved games");drawProfileStatRow(dc,x,483,w,L"Archive",v,true);
    drawProfileStatCard(dc,x,528,w,148,L"Improvement trends");v[0]=0;if(p.defaultRating>0)wAppendInt(v,300,p.defaultRating);else wAppend(v,300,L"not set");wAppend(v,300,L" -> ");if(p.currentRating>0)wAppendInt(v,300,p.currentRating);else wAppend(v,300,L"not set");if(p.defaultRating>0&&p.currentRating>0){int d=p.currentRating-p.defaultRating;wAppend(v,300,L"  (");if(d>=0)wAppend(v,300,L"+");wAppendInt(v,300,d);wAppend(v,300,L")");}drawProfileStatRow(dc,x,564,w,L"Rating trend",v,false);
    v[0]=0;wAppendInt(v,300,quality);wAppend(v,300,L"% good-or-better   |   ");wAppendInt(v,300,st.reviewedMoves);wAppend(v,300,L" reviewed moves");drawProfileStatRow(dc,x,591,w,L"Move quality",v,true);
    v[0]=0;for(int i=0;i<st.recentRatingCount;i++){if(i)wAppend(v,300,L"  >  ");wAppendInt(v,300,st.recentRating[i]);}if(!v[0])wAppend(v,300,L"No completed rating history yet");drawProfileStatRow(dc,x,618,w,L"Recent ratings",v,false);
    v[0]=0;for(int i=0;i<st.recentCount;i++){if(i)wAppend(v,300,L"  >  ");wAppendInt(v,300,st.recentDep[i]);wAppend(v,300,L"%");}if(!v[0])wAppend(v,300,L"No dependency history yet");drawProfileStatRow(dc,x,645,w,L"Dependency",v,true);
}
static LRESULT CALLBACK profileProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);HBRUSH bg=CreateSolidBrush(RGBc(238,241,244));FillRect(dc,&rc,bg);DeleteObject(bg);HBRUSH card=CreateSolidBrush(RGBc(248,249,250));HPEN edge=CreatePen(PS_SOLID,1,RGBc(211,217,222));HGDIOBJ ob=SelectObject(dc,card),op=SelectObject(dc,edge);RoundRect(dc,14,12,rc.right-14,rc.bottom-14,14,14);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(edge);DeleteObject(card);SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,28,24,L"Player Profiles",15);HBRUSH accent=CreateSolidBrush(RGBc(115,149,82));RECT ar={28,52,152,55};FillRect(dc,&ar,accent);DeleteObject(accent);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));RECT intro={30,66,rc.right-30,102};DrawTextW(dc,(LPWSTR)L"Each profile keeps its own rating, career, History and improvement statistics. All data stays in this portable folder.",-1,&intro,DT_LEFT|DT_WORDBREAK|DT_VCENTER);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));TextOutW(dc,362,112,L"Player name",11);TextOutW(dc,362,184,L"Current rating",14);TextOutW(dc,622,184,L"Starting / default rating",25);drawProfileManagerStats(dc,g_profileManagerSelection);if(g_guestProfile){SetTextColor(dc,RGBc(156,103,38));RECT gr={30,640,304,692};DrawTextW(dc,(LPWSTR)L"Current session: Guest\nPersistent career/history writes are OFF.",-1,&gr,DT_LEFT|DT_WORDBREAK);}
        EndPaint(hwnd,&ps);return 0;}
    if(msg==WM_COMMAND){int id=lowWord(w);if(id==3400){profileManagerLoadSelection();return 0;}if(id==3401){addNewProfile();return 0;}if(id==3402){saveSelectedProfileEdits();return 0;}if(id==3403){activatePersistentProfile(g_profileManagerSelection);return 0;}if(id==3404){activateGuestProfile();return 0;}if(id==3405){DestroyWindow(hwnd);return 0;}}
    if(msg==WM_CLOSE){DestroyWindow(hwnd);return 0;}if(msg==WM_DESTROY){g_profileHwnd=NULLPTR;return 0;}return DefWindowProcW(hwnd,msg,w,l);
}
static void showProfileManager(){
    static bool registered=false;if(!registered){WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=profileProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=NULLPTR;wc.lpszClassName=L"ChessTrainerProfiles";RegisterClassExW(&wc);registered=true;}registerModernButtonClass();
    g_profileHwnd=CreateWindowExW(WS_EX_CONTROLPARENT,L"ChessTrainerProfiles",L"Chess Trainer - Player Profiles",WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_CLIPCHILDREN,210,55,1000,790,g_mainHwnd,NULLPTR,g_hInstance,NULLPTR);if(!g_profileHwnd)return;
    g_profileList=CreateWindowExW(0,L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|0x0001,30,124,274,506,g_profileHwnd,(HMENU)(ULONG_PTR)3400,g_hInstance,NULLPTR);SendMessageW(g_profileList,WM_SETFONT,(WPARAM)g_uiFont,1);
    g_profileNameEdit=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,362,138,582,30,g_profileHwnd,(HMENU)(ULONG_PTR)3410,g_hInstance,NULLPTR);SendMessageW(g_profileNameEdit,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(g_profileNameEdit,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((8&0xFFFF)|((8&0xFFFF)<<16)));
    g_profileCurrentEdit=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,362,210,230,30,g_profileHwnd,(HMENU)(ULONG_PTR)3411,g_hInstance,NULLPTR);SendMessageW(g_profileCurrentEdit,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(g_profileCurrentEdit,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((8&0xFFFF)|((8&0xFFFF)<<16)));
    g_profileDefaultEdit=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,622,210,322,30,g_profileHwnd,(HMENU)(ULONG_PTR)3412,g_hInstance,NULLPTR);SendMessageW(g_profileDefaultEdit,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(g_profileDefaultEdit,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((8&0xFFFF)|((8&0xFFFF)<<16)));
    HWND add=CreateWindowExW(0,L"ChessTrainerModernButton",L"New profile",WS_CHILD|WS_VISIBLE|WS_TABSTOP,30,696,164,42,g_profileHwnd,(HMENU)(ULONG_PTR)3401,g_hInstance,NULLPTR);HWND save=CreateWindowExW(0,L"ChessTrainerModernButton",L"Save changes",WS_CHILD|WS_VISIBLE|WS_TABSTOP,204,696,164,42,g_profileHwnd,(HMENU)(ULONG_PTR)3402,g_hInstance,NULLPTR);HWND use=CreateWindowExW(0,L"ChessTrainerModernButton",L"Use selected",WS_CHILD|WS_VISIBLE|WS_TABSTOP,378,696,164,42,g_profileHwnd,(HMENU)(ULONG_PTR)3403,g_hInstance,NULLPTR);HWND guest=CreateWindowExW(0,L"ChessTrainerModernButton",L"Use Guest",WS_CHILD|WS_VISIBLE|WS_TABSTOP,552,696,164,42,g_profileHwnd,(HMENU)(ULONG_PTR)3404,g_hInstance,NULLPTR);HWND close=CreateWindowExW(0,L"ChessTrainerModernButton",L"Close",WS_CHILD|WS_VISIBLE|WS_TABSTOP,726,696,218,42,g_profileHwnd,(HMENU)(ULONG_PTR)3405,g_hInstance,NULLPTR);HWND bs[]={add,save,use,guest,close};for(int i=0;i<5;i++)SendMessageW(bs[i],WM_SETFONT,(WPARAM)g_buttonFont,1);
    int idx=profileIndexById(g_activeProfileId);if(idx<0)idx=0;profileManagerRefreshList(idx);EnableWindow(g_mainHwnd,FALSE);MSG m;while(g_profileHwnd&&GetMessageW(&m,NULLPTR,0,0)>0){if(handleEditShortcut(&m,g_profileNameEdit,g_profileCurrentEdit,g_profileDefaultEdit))continue;if(!IsDialogMessageW(g_profileHwnd,&m)){TranslateMessage(&m);DispatchMessageW(&m);}}EnableWindow(g_mainHwnd,TRUE);SetFocus(g_mainHwnd);refreshProfileButton();invalidatePanelContent();
}

// --------------------------- On-demand Assisted Mode suggestions ---------------------------
static HWND g_choiceHwnd=NULLPTR;
static int g_choiceSelection=0;
static bool g_choiceHasSecond=false;
static wchar_t g_choiceText1[256],g_choiceText2[256];

static const wchar_t* pieceWord(char u){
    if(u=='K')return L"king";if(u=='Q')return L"queen";if(u=='R')return L"rook";if(u=='B')return L"bishop";if(u=='N')return L"knight";return L"pawn";
}
static const wchar_t* promotionWord(char p){if(p=='q')return L"queen";if(p=='r')return L"rook";if(p=='b')return L"bishop";if(p=='n')return L"knight";return L"piece";}
static void humanMoveDescription(const Position* p,const Move& m,wchar_t out[256]){
    out[0]=0;Move legal[256];int n=legalMovesFor(p,legal,256);char san[20]={0};sanForMove(p,m,legal,n,san);char from[3],to[3];squareName(m.from,from);squareName(m.to,to);char pc=p->sq[m.from],u=upperPiece(pc);bool capture=(p->sq[m.to]!=0)||((m.flags&MF_EP)!=0);
    if(m.flags&MF_CASTLE){wCopy(out,256,m.to>m.from?L"Castle on the king side":L"Castle on the queen side");}
    else if(capture){
        char cap=p->sq[m.to];if((m.flags&MF_EP)!=0)cap=p->whiteToMove?'p':'P';
        wAppend(out,256,L"Capture the ");wAppend(out,256,pieceWord(upperPiece(cap)));wAppend(out,256,L" on ");wAppendAscii(out,256,to);wAppend(out,256,L" with your ");wAppend(out,256,pieceWord(u));wAppend(out,256,L" from ");wAppendAscii(out,256,from);
    }else{
        wAppend(out,256,L"Move your ");wAppend(out,256,pieceWord(u));wAppend(out,256,L" from ");wAppendAscii(out,256,from);wAppend(out,256,L" to ");wAppendAscii(out,256,to);
    }
    if(m.promo){wAppend(out,256,L" and promote it to a ");wAppend(out,256,promotionWord(m.promo));}
    wAppend(out,256,L"   (");wAppendAscii(out,256,san);wAppend(out,256,L")");
}
static LRESULT CALLBACK choiceProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT){
        PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);HBRUSH bg=CreateSolidBrush(RGBc(238,241,244));FillRect(dc,&rc,bg);DeleteObject(bg);
        HBRUSH card=CreateSolidBrush(RGBc(248,249,250));HPEN edge=CreatePen(PS_SOLID,1,RGBc(211,217,222));HGDIOBJ ob=SelectObject(dc,card),op=SelectObject(dc,edge);RoundRect(dc,14,12,rc.right-14,rc.bottom-14,14,14);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(edge);DeleteObject(card);
        SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,28,24,L"Trainer Suggestions",19);HBRUSH accent=CreateSolidBrush(RGBc(115,149,82));RECT ar={28,52,154,55};FillRect(dc,&ar,accent);DeleteObject(accent);
        SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));RECT intro={28,62,rc.right-28,92};DrawTextW(dc,(LPWSTR)L"Choose either strong option. The plain-English description is shown first; standard chess notation is kept in brackets for learning.",-1,&intro,DT_LEFT|DT_WORDBREAK|DT_VCENTER);
        for(int i=0;i<2;i++){
            int y=i==0?102:190;DWORD fill=i==0?RGBc(239,246,236):RGBc(243,246,248);HBRUSH b=CreateSolidBrush(fill);HPEN p=CreatePen(PS_SOLID,1,RGBc(205,214,207));HGDIOBJ oo=SelectObject(dc,b),pp=SelectObject(dc,p);RoundRect(dc,28,y,rc.right-28,y+76,10,10);SelectObject(dc,pp);SelectObject(dc,oo);DeleteObject(p);DeleteObject(b);
            wchar_t label[16]={0};wCopy(label,16,i==0?L"OPTION 1":L"OPTION 2");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(68,112,58));TextOutW(dc,40,y+8,label,wLen(label));SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(34,42,49));RECT tx={40,y+27,rc.right-40,y+68};LPWSTR t=(LPWSTR)(i==0?g_choiceText1:g_choiceText2);DrawTextW(dc,t,-1,&tx,DT_LEFT|DT_WORDBREAK|DT_VCENTER);
        }
        if(!g_choiceHasSecond){SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(125,133,139));RECT tx={40,217,rc.right-40,252};DrawTextW(dc,(LPWSTR)L"Only one usable legal option was returned in this position.",-1,&tx,DT_LEFT|DT_WORDBREAK|DT_VCENTER);}
        EndPaint(hwnd,&ps);return 0;
    }
    if(msg==WM_COMMAND){int id=lowWord(w);if(id==3201){g_choiceSelection=1;DestroyWindow(hwnd);return 0;}if(id==3202&&g_choiceHasSecond){g_choiceSelection=2;DestroyWindow(hwnd);return 0;}if(id==3203){g_choiceSelection=0;DestroyWindow(hwnd);return 0;}}
    if(msg==WM_CLOSE){g_choiceSelection=0;DestroyWindow(hwnd);return 0;}if(msg==WM_DESTROY){g_choiceHwnd=NULLPTR;return 0;}return DefWindowProcW(hwnd,msg,w,l);
}
static int showTrainerChoiceDialog(const Move& first,const Move* second){
    static bool registered=false;if(!registered){WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=choiceProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=NULLPTR;wc.lpszClassName=L"ChessTrainerSuggestions";RegisterClassExW(&wc);registered=true;}
    registerModernButtonClass();g_choiceSelection=0;g_choiceHasSecond=second!=NULLPTR;humanMoveDescription(&g_game.pos,first,g_choiceText1);if(second)humanMoveDescription(&g_game.pos,*second,g_choiceText2);else wCopy(g_choiceText2,256,L"No second option is available in this position.");
    g_choiceHwnd=CreateWindowExW(WS_EX_CONTROLPARENT,L"ChessTrainerSuggestions",L"Chess Trainer - Suggestions",WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_CLIPCHILDREN,360,160,660,390,g_mainHwnd,NULLPTR,g_hInstance,NULLPTR);if(!g_choiceHwnd)return 0;
    HWND b1=CreateWindowExW(0,L"ChessTrainerModernButton",L"Use Option 1",WS_CHILD|WS_VISIBLE|WS_TABSTOP,168,294,140,40,g_choiceHwnd,(HMENU)(ULONG_PTR)3201,g_hInstance,NULLPTR);
    HWND b2=CreateWindowExW(0,L"ChessTrainerModernButton",L"Use Option 2",WS_CHILD|WS_VISIBLE|WS_TABSTOP,316,294,140,40,g_choiceHwnd,(HMENU)(ULONG_PTR)3202,g_hInstance,NULLPTR);
    HWND cancel=CreateWindowExW(0,L"ChessTrainerModernButton",L"Cancel",WS_CHILD|WS_VISIBLE|WS_TABSTOP,464,294,104,40,g_choiceHwnd,(HMENU)(ULONG_PTR)3203,g_hInstance,NULLPTR);SendMessageW(b1,WM_SETFONT,(WPARAM)g_buttonFont,1);SendMessageW(b2,WM_SETFONT,(WPARAM)g_buttonFont,1);SendMessageW(cancel,WM_SETFONT,(WPARAM)g_buttonFont,1);if(!g_choiceHasSecond)EnableWindow(b2,FALSE);
    SetFocus(b1);EnableWindow(g_mainHwnd,FALSE);MSG m;while(g_choiceHwnd&&GetMessageW(&m,NULLPTR,0,0)>0){if(!IsDialogMessageW(g_choiceHwnd,&m)){TranslateMessage(&m);DispatchMessageW(&m);}}EnableWindow(g_mainHwnd,TRUE);SetFocus(g_mainHwnd);return g_choiceSelection;
}

static void appendCp(wchar_t* out,int cap,int cp);

// --------------------------- Coach Mode teaching dialog ---------------------------
static HWND g_coachAdviceHwnd=NULLPTR;static int g_coachAdviceChoice=0;static wchar_t g_coachMoveLine[320],g_coachAssessment[320],g_coachReplyLine[320],g_coachPlanLine[320],g_coachSanLesson[320];
static void sanLesson(const char* san,wchar_t out[320]){
    out[0]=0;if(!san||!san[0]){wCopy(out,320,L"SAN: no notation available.");return;}wAppend(out,320,L"SAN lesson: ");wAppendAscii(out,320,san);wAppend(out,320,L". ");
    if(aStarts(san,"O-O-O")){wAppend(out,320,L"O-O-O means castle on the queen side.");return;}if(aStarts(san,"O-O")){wAppend(out,320,L"O-O means castle on the king side.");return;}
    char c=san[0];if(c=='K')wAppend(out,320,L"K means King. ");else if(c=='Q')wAppend(out,320,L"Q means Queen. ");else if(c=='R')wAppend(out,320,L"R means Rook. ");else if(c=='B')wAppend(out,320,L"B means Bishop. ");else if(c=='N')wAppend(out,320,L"N means Knight. ");else wAppend(out,320,L"No leading letter means a pawn move. ");
    if(aFind(san,"x")>=0)wAppend(out,320,L"x means capture. ");if(aFind(san,"+")>=0)wAppend(out,320,L"+ means check. ");if(aFind(san,"#")>=0)wAppend(out,320,L"# means checkmate. ");wAppend(out,320,L"The final square is the destination.");
}
static void coachMoveConsequences(const Position* before,const Move& m,int loss,wchar_t out[320]){
    out[0]=0;char from[3],to[3];squareName(m.from,from);squareName(m.to,to);char moving=before->sq[m.from],captured=before->sq[m.to];bool moverWhite=before->whiteToMove;bool capture=(captured!=0)||((m.flags&MF_EP)!=0);
    Position after=*before;applyMoveRaw(&after,m);bool givesCheck=inCheck(&after,after.whiteToMove);bool destinationAttacked=isAttacked(&after,m.to,after.whiteToMove);
    wAppend(out,320,L"Coach verdict: ");int cls=classifyMoveQuality(g_game.histCount,g_coachBeforeScore,g_coachAfterScore,NULLPTR);wAppend(out,320,reviewClassName(cls));wAppend(out,320,L". ");
    if(loss<=18)wAppend(out,320,L"This is very close to the strongest move in the position. ");
    else if(loss<=45)wAppend(out,320,L"This is a sound move and keeps most of your advantage. ");
    else {wAppend(out,320,L"Compared with the strongest move, this gives away about ");appendCp(out,320,loss);wAppend(out,320,L" pawns of evaluation. ");}
    if(capture){if((m.flags&MF_EP)!=0)captured=moverWhite?'p':'P';wAppend(out,320,L"You remove the opponent's ");wAppend(out,320,pieceWord(upperPiece(captured)));wAppend(out,320,L" on ");wAppendAscii(out,320,to);wAppend(out,320,L". ");}
    if(givesCheck)wAppend(out,320,L"It also gives check, so the opponent must answer the king threat. ");
    if(!(m.flags&MF_CASTLE)){wAppend(out,320,L"Your ");wAppend(out,320,pieceWord(upperPiece(moving)));wAppend(out,320,L" finishes on ");wAppendAscii(out,320,to);wAppend(out,320,destinationAttacked?L", where an opponent piece can attack it immediately. ":L", where it is not under an immediate direct attack. ");}
    if(m.flags&MF_CASTLE)wAppend(out,320,L"Castling improves king safety and connects your rook with the rest of the position. ");
}
static void coachReplyConsequences(const Position* beforeCandidate,const Move& candidate,const Move& reply,wchar_t out[320]){
    out[0]=0;Position q=*beforeCandidate;applyMoveRaw(&q,candidate);wchar_t d[256]={0};humanMoveDescription(&q,reply,d);char captured=q.sq[reply.to];bool capture=(captured!=0)||((reply.flags&MF_EP)!=0);bool replyWhite=q.whiteToMove;applyMoveRaw(&q,reply);bool givesCheck=inCheck(&q,q.whiteToMove);
    wAppend(out,320,L"Most likely reply: ");wAppend(out,320,d);wAppend(out,320,L". ");
    if(capture){if((reply.flags&MF_EP)!=0)captured=replyWhite?'p':'P';wAppend(out,320,L"That removes your ");wAppend(out,320,pieceWord(upperPiece(captured)));wAppend(out,320,L". ");}
    if(givesCheck)wAppend(out,320,L"It also checks your king, so you must respond immediately. ");
    else wAppend(out,320,L"This is the reply Stockfish considers the strongest way to challenge your idea. ");
}
static LRESULT CALLBACK coachAdviceProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);HBRUSH bg=CreateSolidBrush(RGBc(238,241,244));FillRect(dc,&rc,bg);DeleteObject(bg);HBRUSH card=CreateSolidBrush(RGBc(248,249,250));HPEN edge=CreatePen(PS_SOLID,1,RGBc(211,217,222));HGDIOBJ ob=SelectObject(dc,card),op=SelectObject(dc,edge);RoundRect(dc,14,12,rc.right-14,rc.bottom-14,14,14);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(edge);DeleteObject(card);SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,30,24,L"Coach Mode - Think Before the Reply",35);HBRUSH accent=CreateSolidBrush(RGBc(115,149,82));RECT ar={30,54,190,57};FillRect(dc,&ar,accent);DeleteObject(accent);
        SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(67,78,85));RECT r1={30,72,rc.right-30,118};DrawTextW(dc,g_coachMoveLine,-1,&r1,DT_LEFT|DT_WORDBREAK);HBRUSH warn=CreateSolidBrush(RGBc(239,243,238));RECT wr={28,124,rc.right-28,188};FillRect(dc,&wr,warn);DeleteObject(warn);SetTextColor(dc,RGBc(66,85,61));RECT r2={40,134,rc.right-40,180};DrawTextW(dc,g_coachAssessment,-1,&r2,DT_LEFT|DT_WORDBREAK);
        SetTextColor(dc,RGBc(62,72,79));RECT r3={30,198,rc.right-30,248};DrawTextW(dc,g_coachReplyLine,-1,&r3,DT_LEFT|DT_WORDBREAK);RECT r4={30,252,rc.right-30,302};DrawTextW(dc,g_coachPlanLine,-1,&r4,DT_LEFT|DT_WORDBREAK);HBRUSH lesson=CreateSolidBrush(RGBc(245,247,249));RECT lr={28,310,rc.right-28,376};FillRect(dc,&lr,lesson);DeleteObject(lesson);SetTextColor(dc,RGBc(82,92,99));RECT r5={40,320,rc.right-40,368};DrawTextW(dc,g_coachSanLesson,-1,&r5,DT_LEFT|DT_WORDBREAK);EndPaint(hwnd,&ps);return 0;}
    if(msg==WM_COMMAND){int id=lowWord(w);if(id==3301){g_coachAdviceChoice=1;DestroyWindow(hwnd);return 0;}if(id==3302){g_coachAdviceChoice=0;DestroyWindow(hwnd);return 0;}}
    if(msg==WM_CLOSE){g_coachAdviceChoice=0;DestroyWindow(hwnd);return 0;}if(msg==WM_DESTROY){g_coachAdviceHwnd=NULLPTR;return 0;}return DefWindowProcW(hwnd,msg,w,l);
}
static int showCoachAdviceDialog(const Move& candidate,const Move* opp,const Move* follow,int loss){
    static bool registered=false;if(!registered){WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=coachAdviceProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=NULLPTR;wc.lpszClassName=L"ChessTrainerCoachAdvice";RegisterClassExW(&wc);registered=true;}registerModernButtonClass();
    Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);char san[20]={0};sanForMove(&g_game.pos,candidate,legal,n,san);wchar_t desc[256]={0};humanMoveDescription(&g_game.pos,candidate,desc);g_coachMoveLine[0]=0;wAppend(g_coachMoveLine,320,L"Your proposed move: ");wAppend(g_coachMoveLine,320,desc);wAppend(g_coachMoveLine,320,L"  (");wAppendAscii(g_coachMoveLine,320,san);wAppend(g_coachMoveLine,320,L")");
    coachMoveConsequences(&g_game.pos,candidate,loss,g_coachAssessment);
    g_coachReplyLine[0]=0;g_coachPlanLine[0]=0;if(opp){Position q=g_game.pos;coachReplyConsequences(&q,candidate,*opp,g_coachReplyLine);applyMoveRaw(&q,candidate);Move ol[256];int on=legalMovesFor(&q,ol,256);char osan[20]={0};sanForMove(&q,*opp,ol,on,osan);applyMoveRaw(&q,*opp);if(follow){wchar_t fd[256]={0};humanMoveDescription(&q,*follow,fd);Move fl[256];int fn=legalMovesFor(&q,fl,256);char fsan[20]={0};sanForMove(&q,*follow,fl,fn,fsan);wAppend(g_coachPlanLine,320,L"Your best answer after that: ");wAppend(g_coachPlanLine,320,fd);wAppend(g_coachPlanLine,320,L". This is the move that best meets the opponent's threat while keeping your position together.");}else wCopy(g_coachPlanLine,320,L"No useful follow-up is available because the reply ends the game or leaves a terminal position.");}else{wCopy(g_coachReplyLine,320,L"There is no legal opponent reply because your move ends the game or reaches a terminal position.");wCopy(g_coachPlanLine,320,L"No follow-up is needed in a terminal position.");}sanLesson(san,g_coachSanLesson);
    g_coachAdviceChoice=0;g_coachAdviceHwnd=CreateWindowExW(WS_EX_CONTROLPARENT,L"ChessTrainerCoachAdvice",L"Chess Trainer - Coach Mode",WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_CLIPCHILDREN,330,110,760,500,g_mainHwnd,NULLPTR,g_hInstance,NULLPTR);if(!g_coachAdviceHwnd)return 0;HWND play=CreateWindowExW(0,L"ChessTrainerModernButton",L"Play this move",WS_CHILD|WS_VISIBLE|WS_TABSTOP,438,398,140,40,g_coachAdviceHwnd,(HMENU)(ULONG_PTR)3301,g_hInstance,NULLPTR);HWND retry=CreateWindowExW(0,L"ChessTrainerModernButton",L"Try another move",WS_CHILD|WS_VISIBLE|WS_TABSTOP,586,398,140,40,g_coachAdviceHwnd,(HMENU)(ULONG_PTR)3302,g_hInstance,NULLPTR);SendMessageW(play,WM_SETFONT,(WPARAM)g_buttonFont,1);SendMessageW(retry,WM_SETFONT,(WPARAM)g_buttonFont,1);SetFocus(retry);EnableWindow(g_mainHwnd,FALSE);MSG m;while(g_coachAdviceHwnd&&GetMessageW(&m,NULLPTR,0,0)>0){if(!IsDialogMessageW(g_coachAdviceHwnd,&m)){TranslateMessage(&m);DispatchMessageW(&m);}}EnableWindow(g_mainHwnd,TRUE);SetFocus(g_mainHwnd);return g_coachAdviceChoice;
}

// --------------------------- Human Practice strength dialog ---------------------------
static HWND g_practiceHwnd=NULLPTR,g_practiceEdit=NULLPTR;
static LRESULT CALLBACK practiceProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);HBRUSH bg=CreateSolidBrush(RGBc(238,241,244));FillRect(dc,&rc,bg);DeleteObject(bg);HBRUSH card=CreateSolidBrush(RGBc(248,249,250));HPEN edge=CreatePen(PS_SOLID,1,RGBc(211,217,222));HGDIOBJ ob=SelectObject(dc,card),op=SelectObject(dc,edge);RoundRect(dc,16,12,rc.right-16,rc.bottom-14,14,14);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(edge);DeleteObject(card);SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,32,24,L"Human Practice Opponent",23);HBRUSH accent=CreateSolidBrush(RGBc(115,149,82));RECT ar={32,54,196,57};FillRect(dc,&ar,accent);DeleteObject(accent);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));RECT intro={32,66,rc.right-32,102};DrawTextW(dc,(LPWSTR)L"Choose an approximate playing strength for the local computer opponent.",-1,&intro,DT_LEFT|DT_WORDBREAK|DT_VCENTER);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(72,83,90));TextOutW(dc,32,113,L"Opponent strength",17);RECT range={210,108,rc.right-32,132};SetTextColor(dc,RGBc(108,117,125));DrawTextW(dc,(LPWSTR)L"800 - 2200",-1,&range,DT_RIGHT|DT_SINGLELINE|DT_VCENTER);HBRUSH info=CreateSolidBrush(RGBc(239,243,238));RECT ir={32,170,rc.right-32,226};FillRect(dc,&ir,info);DeleteObject(info);SetTextColor(dc,RGBc(91,99,105));RECT tr={44,178,rc.right-44,218};DrawTextW(dc,(LPWSTR)L"This changes only the offline practice opponent. The number is an approximate training strength, not an exact Chess.com rating.",-1,&tr,DT_LEFT|DT_WORDBREAK|DT_VCENTER);EndPaint(hwnd,&ps);return 0;}
    if(msg==WM_COMMAND){int id=lowWord(w);if(id==3102){wchar_t t[32]={0};GetWindowTextW(g_practiceEdit,t,32);int v=parseWInt(t);if(v<800)v=800;if(v>2200)v=2200;g_practiceElo=v;g_requestPracticeElo=v;DestroyWindow(hwnd);return 0;}if(id==3103){DestroyWindow(hwnd);return 0;}}
    if(msg==WM_CLOSE){DestroyWindow(hwnd);return 0;}if(msg==WM_DESTROY){g_practiceHwnd=NULLPTR;return 0;}return DefWindowProcW(hwnd,msg,w,l);
}
static void showPracticeDialog(){
    static bool registered=false;if(!registered){WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=practiceProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=NULLPTR;wc.lpszClassName=L"ChessTrainerPracticeStrength";RegisterClassExW(&wc);registered=true;}registerModernButtonClass();
    g_practiceHwnd=CreateWindowExW(WS_EX_CONTROLPARENT,L"ChessTrainerPracticeStrength",L"Chess Trainer - Human Practice",WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_CLIPCHILDREN,420,210,560,350,g_mainHwnd,NULLPTR,g_hInstance,NULLPTR);if(!g_practiceHwnd)return;
    g_practiceEdit=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,32,136,190,38,g_practiceHwnd,(HMENU)(ULONG_PTR)3101,g_hInstance,NULLPTR);SendMessageW(g_practiceEdit,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(g_practiceEdit,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((10&0xFFFF)|((10&0xFFFF)<<16)));setControlInt(g_practiceEdit,g_practiceElo);
    HWND save=CreateWindowExW(0,L"ChessTrainerModernButton",L"Use strength",WS_CHILD|WS_VISIBLE|WS_TABSTOP,322,246,104,40,g_practiceHwnd,(HMENU)(ULONG_PTR)3102,g_hInstance,NULLPTR);HWND cancel=CreateWindowExW(0,L"ChessTrainerModernButton",L"Keep current",WS_CHILD|WS_VISIBLE|WS_TABSTOP,434,246,94,40,g_practiceHwnd,(HMENU)(ULONG_PTR)3103,g_hInstance,NULLPTR);SendMessageW(save,WM_SETFONT,(WPARAM)g_buttonFont,1);SendMessageW(cancel,WM_SETFONT,(WPARAM)g_buttonFont,1);
    SetFocus(g_practiceEdit);EnableWindow(g_mainHwnd,FALSE);MSG m;while(g_practiceHwnd&&GetMessageW(&m,NULLPTR,0,0)>0){if(!IsDialogMessageW(g_practiceHwnd,&m)){TranslateMessage(&m);DispatchMessageW(&m);}}EnableWindow(g_mainHwnd,TRUE);SetFocus(g_mainHwnd);invalidatePanelContent();
}

static bool detectGameEnd(int* result,int* reason,wchar_t status[160]){
    Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);
    if(n==0){
        if(inCheck(&g_game.pos,g_game.pos.whiteToMove)){
            bool winnerWhite=!g_game.pos.whiteToMove;*result=(winnerWhite==g_userWhite)?RESULT_WIN:RESULT_LOSS;*reason=REASON_CHECKMATE;
            wCopy(status,160,*result==RESULT_WIN?L"Checkmate, you have won the game.":L"Checkmate, your opponent has won the game.");
        }else{*result=RESULT_DRAW;*reason=REASON_STALEMATE;wCopy(status,160,L"Stalemate, the game is a draw.");}return true;
    }
    if(insufficientMaterial(&g_game.pos)){*result=RESULT_DRAW;*reason=REASON_INSUFFICIENT;wCopy(status,160,L"Draw by insufficient material.");return true;}
    U64 k=positionKey(&g_game.pos);int c=0;for(int i=0;i<g_game.keyCount;i++)if(g_game.keys[i]==k)c++;
    if(c>=3){*result=RESULT_DRAW;*reason=REASON_THREEFOLD;wCopy(status,160,L"Draw by threefold repetition.");return true;}
    if(g_game.pos.halfmove>=100){*result=RESULT_DRAW;*reason=REASON_FIFTY_MOVE;wCopy(status,160,L"Draw by the fifty-move rule.");return true;}
    return false;
}
static void buildGameSummary(wchar_t out[640],int result,int reason){
    out[0]=0;wAppend(out,640,L"Game complete\r\n\r\n");
    if(result==RESULT_WIN)wAppend(out,640,L"You won the game.\r\n");else if(result==RESULT_LOSS)wAppend(out,640,L"You lost the game.\r\n");else if(result==RESULT_DRAW)wAppend(out,640,L"The game was drawn.\r\n");else wAppend(out,640,L"The game was recorded as aborted.\r\n");
    wAppend(out,640,L"Reason: ");wAppend(out,640,reasonLabel(reason));wAppend(out,640,L"\r\n");
    char on[64],eco[8];recognizeOpening(on,64,eco,8);wAppend(out,640,L"Opening: ");wAppendAscii(out,640,on);if(eco[0]&&eco[0]!='-'){wAppend(out,640,L" (");wAppendAscii(out,640,eco);wAppend(out,640,L")");}wAppend(out,640,L"\r\n");
    wAppend(out,640,L"Moves: ");wAppendInt(out,640,(g_game.histCount+1)/2);if(g_game.histCount>0){wAppend(out,640,L"\r\nFinal move: ");wAppendAscii(out,640,g_game.hist[g_game.histCount-1].san);}
}
static void finishCurrentGame(int result,int reason,const wchar_t* status,bool showPopup){
    (void)showPopup;if(g_gameRecorded)return;g_gameOver=true;g_currentResult=result;g_currentReason=reason;g_panelMode=1;g_suggestionPending=false;g_suggestionGuideKind=GUIDE_NONE;g_requestGuideKind=GUIDE_NONE;g_threatCheckPending=false;g_selected=-1;g_calculating=false;
    applyRatingResult(result);recordCurrentGame(result,reason);
    if(g_gameBaseMode==MODE_ASSISTED&&g_assistFull&&g_trainerMode==MODE_FULL){g_assistFull=false;g_trainerMode=MODE_ASSISTED;debugAppendRaw("ASSIST","Game ended while Full Assist was active; temporary Full Trainer behaviour closed and Assisted Mode identity restored.");debugAppendRaw("MODE","Assisted Mode restored for completed-game display without counting a manual Full Assist OFF action.");}
    setStatus(status);layoutUI();updateControls();invalidateBoard();invalidateAnalysis();invalidateGameLog();
    // Keep the result inside Game Insights instead of showing a dated modal result
    // box. Stockfish then reviews the completed moves in the background.
    if(result!=RESULT_ABORTED&&g_game.histCount>0)requestGameReview();else{g_reviewInProgress=false;g_reviewReady=false;}
}
static bool finishIfGameEnded(bool showPopup){int result=0,reason=0;wchar_t status[160];if(!detectGameEnd(&result,&reason,status))return false;finishCurrentGame(result,reason,status,showPopup);return true;}
static void manualFinishGame(int result,int reason){
    if(!g_userSet||g_gameOver)return;
    wchar_t q[256]={0};wAppend(q,256,L"Record this result and end the current game?\r\n\r\n");wAppend(q,256,reasonLabel(reason));
    if(MessageBoxW(g_mainHwnd,q,L"Chess Trainer - End Game",MB_YESNO|MB_ICONQUESTION)!=IDYES)return;
    g_positionVersion++;const wchar_t* st=L"Game complete.";
    switch(reason){
        case REASON_OPPONENT_RESIGNED:st=L"Opponent resigned, you won the game.";break;
        case REASON_USER_RESIGNED:st=L"You resigned, game recorded as a loss.";break;
        case REASON_OPPONENT_ABANDONED:st=L"Opponent abandoned the game, you won.";break;
        case REASON_USER_ABANDONED:st=L"You abandoned the game, recorded as a loss.";break;
        case REASON_OPPONENT_TIMEOUT:st=L"Opponent lost on time, you won.";break;
        case REASON_USER_TIMEOUT:st=L"You lost on time, game recorded as a loss.";break;
        case REASON_DRAW_AGREED:st=L"Draw agreed, game recorded as a draw.";break;
        case REASON_CANCELLED:st=L"Game cancelled, recorded with no result and no rating change.";break;
        case REASON_ABORTED:st=L"Game aborted, recorded with no result and no rating change.";break;
    }
    finishCurrentGame(result,reason,st,false);
}
static void updateHistory(){
    // The Game Log is owner-drawn by the main window in v3.6. This avoids the plain
    // EDIT-control appearance and gives us clean columns, row highlighting and a
    // consistent modern look without introducing another repainting child window.
    g_historyText[0]=0;
    for(int i=0;i<g_game.histCount;i+=2){
        wAppendInt(g_historyText,8192,i/2+1);wAppend(g_historyText,8192,L". ");
        wAppendAscii(g_historyText,8192,g_game.hist[i].san);
        if(i+1<g_game.histCount){wAppend(g_historyText,8192,L"   ");wAppendAscii(g_historyText,8192,g_game.hist[i+1].san);}
        wAppend(g_historyText,8192,L"\r\n");
    }
    invalidateGameLog();
}
static void applyEngineEvaluationForSide(bool sideToMoveWhite){
    int w=g_engineWdlWin,d=g_engineWdlDraw,l=g_engineWdlLoss;
    if(sideToMoveWhite){g_whiteWin=w;g_drawChance=d;g_blackWin=l;}else{g_whiteWin=l;g_drawChance=d;g_blackWin=w;}
    int sum=g_whiteWin+g_drawChance+g_blackWin;if(sum<=0){g_whiteWin=0;g_drawChance=1000;g_blackWin=0;}else if(sum!=1000){g_whiteWin=g_whiteWin*1000/sum;g_drawChance=g_drawChance*1000/sum;g_blackWin=1000-g_whiteWin-g_drawChance;}
    g_lastMate=g_engineMate;g_evalValid=true;invalidateAnalysis();
}
static void applyEngineEvaluation(){applyEngineEvaluationForSide(g_game.pos.whiteToMove);}
static void clearEmergencyRescue(){if(g_assistEmergency)debugAppendRaw("ASSIST","Emergency Rescue cleared.");g_assistEmergency=false;g_assistEmergencyRemaining=0;g_assistEmergencyUsed=0;g_assistEmergencyExtensions=0;}
static void startEmergencyRescue(){if(!g_assistEmergency&&!g_assistEmergencyLatched){g_assistEmergency=true;g_assistEmergencyRemaining=ASSIST_EMERGENCY_BASE_MOVES;g_assistEmergencyUsed=0;g_assistEmergencyExtensions=0;g_assistDangerClearScans=0;debugAppendRaw("ASSIST","Emergency Rescue activated with a three-move base limit.");}}
static void latchEmergencyEpisode(){g_assistEmergencyLatched=true;g_assistDangerClearScans=0;debugAppendRaw("ASSIST","Emergency episode latched after automatic limit; same continuous danger will not auto-rearm.");}
static void resetEmergencyEpisodeLatch(){if(g_assistEmergencyLatched)debugAppendRaw("ASSIST","Emergency episode latch cleared after sustained recovery.");g_assistEmergencyLatched=false;g_assistDangerClearScans=0;}
static void checkNeedResponse();
static int assistedTerminalScore(const Move& m){
    Position q=g_game.pos;applyMoveRaw(&q,m);Move replies[256];int rn=legalMovesFor(&q,replies,256);
    if(rn==0){if(inCheck(&q,q.whiteToMove))return 100000;return 0;}
    if(insufficientMaterial(&q)||q.halfmove>=100)return 0;
    U64 k=positionKey(&q);int c=1;for(int i=0;i<g_game.keyCount;i++)if(g_game.keys[i]==k)c++;if(c>=3)return 0;
    return 200000;
}
static void commitRecordedMove(const Move& m){
    char san[20];if(!gamePush(m,san)){debugAppendRaw("ERROR","Attempted to commit a move that was no longer legal in the current position.");MessageBoxW(g_mainHwnd,L"That move is no longer legal in the current position.",L"Position changed",MB_OK|MB_ICONWARNING);g_selected=-1;updateControls();invalidateBoard();return;}debugLogMove("recorded",m,san);
    int committedPly=g_game.histCount-1;bool moverWhite=g_game.hist[committedPly].before.whiteToMove;if(committedPly>=0&&committedPly<512){if(moverWhite==g_userWhite)markTrainingPly(committedPly,g_trainerMode==MODE_COACH?TRAIN_COACH_INDEPENDENT:TRAIN_INDEPENDENT);else{g_trainingSource[committedPly]=TRAIN_NONE;g_assistAccepted[committedPly]=0;}g_reviewClass[committedPly]=RV_NONE;g_reviewLoss[committedPly]=0;}classifyLivePly(committedPly);
    g_positionVersion++;g_correctionMode=false;g_selected=-1;g_suggestionPending=false;g_lastMate=0;updateHistory();invalidateBoard();invalidateAnalysis();if(finishIfGameEnded(true))return;
    if(g_trainerMode==MODE_ASSISTED&&moverWhite!=g_userWhite){g_calculating=true;g_threatCheckPending=true;setStatus(L"Assisted Mode: checking the opponent's move and scanning for mating danger...");updateControls();requestAnalysis(g_positionVersion,REQ_THREAT);return;}
    debugStateCheck("commitRecordedMove");checkNeedResponse();
}

static void checkNeedResponse(){debugStateCheck("checkNeedResponse-entry");
    if(!g_userSet||g_gameOver)return;if(finishIfGameEnded(true))return;
    if(g_trainerMode==MODE_ASSISTED){
        if(g_game.pos.whiteToMove==g_userWhite&&((g_assistEmergency&&g_assistEmergencyRemaining>0)||g_assistRecoveryRemaining>0||g_assistFull)&&!assistedCanAcceptNextCoachMove()){
            if(g_assistFull)g_assistFull=false;g_calculating=false;g_suggestionPending=false;g_requestGuideKind=GUIDE_NONE;g_selected=-1;assistedIndependentFloorMessage();updateControls();invalidateBoard();invalidateAnalysis();return;
        }
        if(g_game.pos.whiteToMove==g_userWhite&&((g_assistEmergency&&g_assistEmergencyRemaining>0)||g_assistRecoveryRemaining>0||g_assistFull)){
            g_calculating=true;g_suggestionPending=false;g_selected=-1;
            if(g_assistEmergency&&g_assistEmergencyRemaining>0){g_requestGuideKind=GUIDE_EMERGENCY;wchar_t st[320]={0};if(g_assistEmergencyUsed<ASSIST_EMERGENCY_BASE_MOVES){wAppend(st,320,L"EMERGENCY RESCUE: calculating course-correction move ");wAppendInt(st,320,g_assistEmergencyUsed+1);wAppend(st,320,L"/3.");}else{wAppend(st,320,L"EMERGENCY EXTENSION: a forced mate threat still remains, so one extra defensive move is being calculated.");}setStatus(st);}
            else if(g_assistRecoveryRemaining>0){g_requestGuideKind=GUIDE_RECOVERY;wchar_t st[260]={0};wAppend(st,260,L"Recovery Coach: calculating best recovery move. Guided moves remaining: ");wAppendInt(st,260,g_assistRecoveryRemaining);wAppend(st,260,L".");setStatus(st);}
            else{g_requestGuideKind=GUIDE_FULL;setStatus(L"FULL ASSIST: calculating the best move for this turn...");}
            updateControls();invalidateBoard();invalidateAnalysis();requestAnalysis(g_positionVersion,REQ_SUGGEST);return;
        }
        g_calculating=false;g_suggestionPending=false;g_requestGuideKind=GUIDE_NONE;
        if(g_game.pos.whiteToMove==g_userWhite){if(g_assistRecoveryRemaining<=0&&!g_suggestionPending)g_assistRecoverySequence=0;setStatus(L"Assisted Mode: your turn. Play your own move, use Ask Trainer when wanted, or switch Full Assist ON.");}
        else if(g_assistEmergency&&g_assistEmergencyRemaining>0){wchar_t st[300]={0};wAppend(st,300,L"Emergency course correction has ");wAppendInt(st,300,g_assistEmergencyRemaining);wAppend(st,300,L" guided move");if(g_assistEmergencyRemaining!=1)wAppend(st,300,L"s");wAppend(st,300,L" left. Enter your opponent's move; the trainer will reassess before deciding whether more rescue is needed.");setStatus(st);}
        else if(g_assistEmergency)setStatus(L"The three-move emergency correction is complete. Enter your opponent's move; an extra suggestion will occur only if Stockfish still sees forced mate.");
        else if(g_assistRecoveryRemaining>0){wchar_t st[300]={0};wAppend(st,300,L"Recovery Coach remains active from the earlier serious move. Enter your opponent's move; ");wAppendInt(st,300,g_assistRecoveryRemaining);wAppend(st,300,g_assistRecoveryRemaining==1?L" guided move remains.":L" guided moves remain.");setStatus(st);}
        else setStatus(g_assistFull?L"Full Assist is ON. Enter your opponent's move; your next move will be supplied automatically.":L"Assisted Mode: enter your opponent's move.");
        updateControls();invalidateAnalysis();return;
    }
    if(g_trainerMode==MODE_FAIRPLAY){
        g_calculating=false;g_suggestionPending=false;g_evalValid=false;g_lastMate=0;
        setStatus(g_game.pos.whiteToMove==g_userWhite?L"Fair Play mode: enter the move you played.":L"Fair Play mode: enter your opponent's move.");updateControls();invalidateAnalysis();return;
    }
    if(g_trainerMode==MODE_COACH){
        g_suggestionPending=false;if(g_game.pos.whiteToMove==g_userWhite){g_calculating=false;setStatus(L"Coach Mode: make your move. Before the computer replies, the coach will explain the danger, best reply and SAN notation.");updateControls();invalidateAnalysis();return;}g_calculating=true;g_selected=-1;g_requestPracticeElo=g_coachElo;setStatus(L"Coach Mode: computer opponent is thinking...");updateControls();invalidateBoard();requestAnalysis(g_positionVersion,REQ_PRACTICE);return;
    }
    if(g_trainerMode==MODE_PRACTICE){
        g_suggestionPending=false;
        if(g_game.pos.whiteToMove==g_userWhite){g_calculating=false;setStatus(L"Human Practice: your move. The trainer will play the opponent automatically.");updateControls();invalidateAnalysis();return;}
        g_calculating=true;g_selected=-1;g_requestPracticeElo=g_practiceElo;setStatus(L"Human Practice: practice opponent is thinking...");updateControls();invalidateBoard();requestAnalysis(g_positionVersion,REQ_PRACTICE);return;
    }
    if(g_game.pos.whiteToMove==g_userWhite){g_calculating=true;g_suggestionPending=false;g_selected=-1;setStatus(L"Calculating best response...");updateControls();invalidateBoard();requestAnalysis(g_positionVersion,REQ_SUGGEST);}
    else {setStatus(L"Waiting for opponent's move...");updateControls();requestAnalysis(g_positionVersion,REQ_EVAL);}
}
static void selectSide(bool white){
    if(!g_engineReady)return;resetLiveMoveTags();g_userSet=true;g_userWhite=white;g_gameOver=false;g_gameRecorded=false;g_currentResult=RESULT_NONE;g_currentReason=REASON_NONE;g_correctionMode=false;g_selected=-1;g_suggestionPending=false;g_flipped=!white;g_evalValid=false;g_lastMate=0;g_reviewInProgress=false;g_reviewReady=false;g_opponentRating=0;g_lastRatingChange=0;g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;g_assistPending=false;g_choiceRequestPending=false;g_onDemandSuggestion=false;clearEmergencyRescue();resetEmergencyEpisodeLatch();g_threatCheckPending=false;g_coachPending=false;clearMoveExplanation();g_requestGuideKind=GUIDE_NONE;g_suggestionGuideKind=GUIDE_NONE;
    if(g_trainerMode==MODE_PRACTICE||g_trainerMode==MODE_COACH){g_ratingBeforeGame=0;g_lastRatingAfter=g_currentRating;}else{g_ratingBeforeGame=g_currentRating;g_lastRatingAfter=g_currentRating;}
    updateControls();invalidateBoardAndAnalysis();invalidateGameLog();
    if(g_trainerMode!=MODE_PRACTICE&&g_trainerMode!=MODE_COACH)showRatingDialog();
    gameClockStart();
    if(((g_trainerMode==MODE_PRACTICE||g_trainerMode==MODE_COACH)&&white)||(g_trainerMode==MODE_ASSISTED)){g_calculating=true;setStatus(g_trainerMode==MODE_ASSISTED?L"Assisted Mode: preparing turn and move-quality tracking...":g_trainerMode==MODE_COACH?L"Coach Mode: preparing your first coached turn...":L"Human Practice: preparing move-quality tracking...");updateControls();requestAnalysis(g_positionVersion,REQ_TAG);return;}
    checkNeedResponse();
}
static void newGame(){
    if(g_userSet&&!g_gameOver&&!g_gameRecorded&&g_game.histCount>0){
        int r=MessageBoxW(g_mainHwnd,L"The current game is still in progress.\r\n\r\nRecord it as cancelled/no result before starting a new game?",L"Chess Trainer - New Game",MB_YESNOCANCEL|MB_ICONQUESTION);
        if(r==IDCANCEL)return;if(r==IDYES)recordCurrentGame(RESULT_ABORTED,REASON_CANCELLED);
    }
    // Full Assist is a temporary Full Trainer bridge inside Assisted Mode.
    // A fresh game returns to the base Assisted Mode rather than silently becoming
    // a permanent Full Trainer game.
    if(g_assistFull&&g_trainerMode==MODE_FULL)g_trainerMode=MODE_ASSISTED;
    g_positionVersion++;g_requestToken=g_positionVersion;gameInit();gameClockReset();resetLiveMoveTags();g_userSet=false;g_gameOver=false;g_gameRecorded=false;g_currentResult=RESULT_NONE;g_currentReason=REASON_NONE;g_correctionMode=false;g_calculating=false;g_suggestionPending=false;g_selected=-1;g_evalValid=false;g_lastMate=0;g_panelMode=0;g_reviewInProgress=false;g_reviewReady=false;g_opponentRating=0;g_ratingBeforeGame=g_currentRating;g_lastRatingChange=0;g_lastRatingAfter=g_currentRating;g_assistPending=false;g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;g_choiceRequestPending=false;g_onDemandSuggestion=false;g_assistFull=false;clearEmergencyRescue();resetEmergencyEpisodeLatch();g_threatCheckPending=false;g_coachPending=false;clearMoveExplanation();g_requestGuideKind=GUIDE_NONE;g_suggestionGuideKind=GUIDE_NONE;updateHistory();setStatus(g_trainerMode==MODE_ASSISTED?L"Assisted Mode: select your side to start.":g_trainerMode==MODE_PRACTICE?L"Human Practice: select your side to start.":g_trainerMode==MODE_FAIRPLAY?L"Fair Play / Record Only: select your side to start.":g_trainerMode==MODE_COACH?L"Coach Mode: select your side to start.":L"Full Trainer: select your side to start.");layoutUI();updateControls();invalidateBoardAndAnalysis();invalidateGameLog();
}
static void resetForModeChange(){
    g_positionVersion++;g_requestToken=g_positionVersion;gameInit();gameClockReset();resetLiveMoveTags();g_userSet=false;g_gameOver=false;g_gameRecorded=false;g_currentResult=RESULT_NONE;g_currentReason=REASON_NONE;g_correctionMode=false;g_calculating=false;g_suggestionPending=false;g_selected=-1;g_evalValid=false;g_lastMate=0;g_panelMode=0;g_reviewInProgress=false;g_reviewReady=false;g_opponentRating=0;g_ratingBeforeGame=g_currentRating;g_lastRatingChange=0;g_lastRatingAfter=g_currentRating;g_assistPending=false;g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;g_choiceRequestPending=false;g_onDemandSuggestion=false;g_assistFull=false;clearEmergencyRescue();resetEmergencyEpisodeLatch();g_threatCheckPending=false;g_coachPending=false;clearMoveExplanation();g_requestGuideKind=GUIDE_NONE;g_suggestionGuideKind=GUIDE_NONE;updateHistory();
}
static void setAssistedFull(bool on);
static void setTrainerMode(int mode){
    if(mode<MODE_FULL||mode>MODE_COACH)return;
    // When Full Assist was entered from Assisted Mode, clicking the Assisted Mode
    // tab is equivalent to Full Assist OFF. Do not destroy/reset the current game.
    if(g_assistFull&&g_trainerMode==MODE_FULL&&mode==MODE_ASSISTED){setAssistedFull(false);return;}
    if(mode==g_trainerMode){if(mode==MODE_PRACTICE&&!g_userSet&&!g_calculating)showPracticeDialog();return;}
    if(g_calculating){MessageBoxW(g_mainHwnd,L"Please wait for the current engine calculation to finish before changing mode.",L"Chess Trainer - Mode",MB_OK|MB_ICONINFORMATION);return;}
    if(g_userSet&&!g_gameOver&&!g_gameRecorded&&g_game.histCount>0){
        int r=MessageBoxW(g_mainHwnd,L"Changing mode starts a new game.\r\n\r\nRecord the current game as cancelled/no result before switching?",L"Chess Trainer - Change Mode",MB_YESNOCANCEL|MB_ICONQUESTION);if(r==IDCANCEL)return;if(r==IDYES)recordCurrentGame(RESULT_ABORTED,REASON_CANCELLED);
    }
    g_trainerMode=mode;g_gameBaseMode=mode;resetForModeChange();debugAppendWide("MODE",trainerModeName(mode));
    if(mode==MODE_ASSISTED)setStatus(L"Assisted Mode: select your side. Play normally; after a serious blunder, Recovery Coach supplies the best move on your next two turns.");
    else if(mode==MODE_PRACTICE){showPracticeDialog();setStatus(L"Human Practice: select your side. The trainer will play the opponent automatically.");}
    else if(mode==MODE_FAIRPLAY)setStatus(L"Fair Play / Record Only: live engine assistance is disabled. Select your side.");
    else if(mode==MODE_COACH)setStatus(L"Coach Mode: select your side. Your proposed move will be explained before the computer replies, including SAN notation and a likely continuation.");
    else setStatus(L"Full Trainer: select your side to start.");
    layoutUI();updateControls();invalidateAll();
}

static void flipBoard(){g_flipped=!g_flipped;g_selected=-1;invalidateBoard();}
static void setAssistedFull(bool on){
    // Full Assist is no longer an Assisted-mode suggestion source. Turning it ON
    // temporarily changes the effective mode to the genuine Full Trainer engine
    // loop without resetting the board. Turning it OFF returns the same game to
    // normal Assisted Mode.
    bool temporaryFull=(g_assistFull&&g_trainerMode==MODE_FULL);
    if(g_calculating||g_gameOver)return;
    if(on){
        if(g_trainerMode!=MODE_ASSISTED||g_assistFull)return;

        // Full Trainer supersedes any partially armed Assisted guidance. Do not let
        // a stale Recovery/Emergency state take priority when Full Assist is chosen.
        if(g_suggestionPending)recordSuggestionRejected(trainingSourceFromGuide(g_suggestionGuideKind,g_onDemandSuggestion));
        g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;clearEmergencyRescue();
        g_assistPending=false;g_choiceRequestPending=false;g_onDemandSuggestion=false;
        g_threatCheckPending=false;g_requestGuideKind=GUIDE_NONE;g_suggestionGuideKind=GUIDE_NONE;
        g_suggestionPending=false;g_selected=-1;

        g_assistFull=true;g_fullAssistOnCount++;
        g_trainerMode=MODE_FULL;
        debugAppendRaw("ASSIST","Full Assist switched ON -> temporary Full Trainer mode. Assisted 50% safeguard suspended until Full Assist OFF.");
        debugAppendRaw("MODE","Temporary Full Trainer entered from Assisted Mode without resetting the game.");debugStateCheck("full-assist-on");

        setStatus(g_userSet?L"Full Assist ON: switched to Full Trainer behaviour. Chess Trainer will now supply every move until you switch Full Assist OFF.":L"Full Assist ON: temporary Full Trainer behaviour is active. Select your side to start.");
        layoutUI();updateControls();invalidateAll();
        if(g_userSet)checkNeedResponse();
        return;
    }

    if(!temporaryFull)return;

    // If a Full Trainer suggestion is currently displayed, its explicit dismissal is
    // part of the dependency audit before returning the next move to the user.
    if(g_suggestionPending)recordSuggestionRejected(TRAIN_FULL_ASSIST);
    g_suggestionPending=false;g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;
    g_requestGuideKind=GUIDE_NONE;g_selected=-1;
    g_assistFull=false;g_fullAssistOffCount++;
    g_trainerMode=MODE_ASSISTED;
    debugAppendRaw("ASSIST","Full Assist switched OFF -> returned to normal Assisted Mode.");
    debugAppendRaw("MODE","Assisted Mode restored without resetting the game.");debugStateCheck("full-assist-off");

    setStatus(g_userSet?L"Full Assist OFF: returned to normal Assisted Mode. Play your own move; Ask Trainer, Recovery Coach and Emergency Rescue are available again.":L"Full Assist OFF: returned to Assisted Mode. Select your side to start.");
    layoutUI();updateControls();invalidateAll();
    if(g_userSet)checkNeedResponse();
}
static void askTrainerForChoices(){
    if(g_trainerMode!=MODE_ASSISTED||!g_userSet||g_gameOver||g_calculating||g_suggestionPending||g_game.pos.whiteToMove!=g_userWhite||g_assistRecoveryRemaining>0||g_assistFull||(g_assistEmergency&&g_assistEmergencyRemaining>0))return;if(!assistedCanAcceptNextCoachMove()){assistedIndependentFloorMessage();updateControls();return;}
    g_choiceRequestPending=true;g_onDemandSuggestion=false;g_calculating=true;g_selected=-1;setStatus(L"Assisted Mode: finding two strong options for your next move...");updateControls();requestAnalysis(g_positionVersion,REQ_CHOICES);
}
static void processMove(const Move&m){
    if(!boardInputAllowed())return;
    if(g_trainerMode==MODE_COACH&&g_game.pos.whiteToMove==g_userWhite){g_coachPendingMove=m;g_coachPending=true;g_calculating=true;g_selected=-1;setStatus(L"Coach Mode: analysing your proposed move before the computer replies...");updateControls();invalidateBoard();requestCoachAnalysis(g_positionVersion,m);return;}
    if(g_trainerMode==MODE_ASSISTED&&g_game.pos.whiteToMove==g_userWhite){
        g_assistPendingMove=m;g_assistPending=true;g_calculating=true;g_selected=-1;g_suggestionPending=false;
        int terminalScore=assistedTerminalScore(m);setStatus(L"Assisted Mode: recording your move and checking whether Recovery Coach is needed...");updateControls();invalidateBoard();requestAssistAnalysis(g_positionVersion,m,terminalScore);return;
    }
    commitRecordedMove(m);
}
static void confirmSuggested(){
    if(g_trainerMode==MODE_ASSISTED&&!g_suggestionPending){askTrainerForChoices();return;}
    bool full=(g_trainerMode==MODE_FULL);bool assisted=(g_trainerMode==MODE_ASSISTED);
    if((!full&&!assisted)||!g_suggestionPending||g_calculating)return;if(assisted&&!assistedCanAcceptNextCoachMove()){g_suggestionPending=false;g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;g_selected=-1;assistedIndependentFloorMessage();updateControls();invalidateBoard();invalidateAnalysis();return;}Move m=g_suggested;bool wasOnDemand=g_onDemandSuggestion;int suggestionSource=g_suggestionGuideKind;g_suggestionPending=false;g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;char san[20];if(!gamePush(m,san)){debugAppendRaw("ERROR","Suggested move no longer matched current position at confirmation.");MessageBoxW(g_mainHwnd,L"The suggested move no longer matches the current position.",L"Position error",MB_OK|MB_ICONERROR);updateControls();return;}bool tempFullAssist=(full&&g_assistFull);debugLogMove(assisted?"coach-accepted":tempFullAssist?"full-assist":"full-trainer",m,san);
    int committedPly=g_game.histCount-1;int source=full?(tempFullAssist?TRAIN_FULL_ASSIST:TRAIN_FULL_TRAINER):trainingSourceFromGuide(suggestionSource,wasOnDemand);if(committedPly>=0&&committedPly<512){markTrainingPly(committedPly,source);g_reviewClass[committedPly]=RV_BEST;g_reviewLoss[committedPly]=0;invalidateGameLog();}
    g_positionVersion++;g_correctionMode=false;g_lastMate=0;updateHistory();invalidateBoard();invalidateAnalysis();if(finishIfGameEnded(true))return;
    if(full){setStatus(L"Waiting for opponent's move...");requestAnalysis(g_positionVersion,REQ_EVAL);}else{if(wasOnDemand||suggestionSource==GUIDE_ASK)setStatus(L"Trainer option recorded. Enter your opponent's reply.");else if(suggestionSource==GUIDE_FULL)setStatus(L"Full Assist move recorded. Enter your opponent's reply.");else if(suggestionSource==GUIDE_EMERGENCY){wchar_t es[300]={0};if(g_assistEmergencyRemaining>0){wAppend(es,300,L"Emergency course-correction move recorded. ");wAppendInt(es,300,g_assistEmergencyRemaining);wAppend(es,300,L" automatic rescue move");if(g_assistEmergencyRemaining!=1)wAppend(es,300,L"s");wAppend(es,300,L" remain. Enter the opponent's reply.");}else wCopy(es,300,L"The normal emergency correction allowance is complete. Enter the opponent's reply; only a continuing forced mate can justify an extra automatic suggestion.");setStatus(es);}checkNeedResponse();}updateControls();
}
static void playedDifferent(){
    bool full=(g_trainerMode==MODE_FULL);bool assisted=(g_trainerMode==MODE_ASSISTED);if((!full&&!assisted)||!g_suggestionPending||g_calculating)return;
    bool wasOnDemand=g_onDemandSuggestion;int guide=g_suggestionGuideKind;int source=full?(g_assistFull?TRAIN_FULL_ASSIST:TRAIN_FULL_TRAINER):trainingSourceFromGuide(guide,wasOnDemand);recordSuggestionRejected(source);g_suggestionPending=false;g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;g_correctionMode=true;g_selected=-1;
    setStatus(wasOnDemand||guide==GUIDE_ASK?L"Trainer option dismissed. Enter the move you actually played.":guide==GUIDE_FULL?L"Full Assist suggestion dismissed. Enter the move you actually played instead.":guide==GUIDE_EMERGENCY?L"Emergency suggestion dismissed. Enter the move you actually played; the trainer will reassess the position.":assisted?L"Recovery Coach suggestion dismissed. Enter the move you actually played.":L"Enter the move you actually played on the physical board.");updateControls();invalidateBoard();SetFocus(hSan);
}
static void correctLastMove(){
    if(g_gameOver){debugAppendRaw("CORRECTION-BLOCK","Completed game cannot be modified after its result/history/profile statistics are committed.");MessageBoxW(g_mainHwnd,L"This game is already complete and has been recorded. Start a new game if you want to continue playing.",L"Game already complete",MB_OK|MB_ICONINFORMATION);return;}
    if(g_calculating||g_game.histCount<=0)return;HistoryEntry &h=g_game.hist[g_game.histCount-1];if(h.before.whiteToMove!=g_userWhite){MessageBoxW(g_mainHwnd,L"The last recorded move belongs to your opponent. Undo that move first if you need to go further back.",L"Cannot correct yet",MB_OK|MB_ICONINFORMATION);return;}
    g_positionVersion++;g_suggestionPending=false;g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;g_requestGuideKind=GUIDE_NONE;g_choiceRequestPending=false;g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;clearEmergencyRescue();g_threatCheckPending=false;unmarkTrainingPly(g_game.histCount-1);gameUndo();g_correctionMode=true;g_selected=-1;g_gameOver=false;updateHistory();invalidateBoard();setStatus(L"Your last move was removed. Enter the move you actually played instead.");updateControls();SetFocus(hSan);
}
static void undoMove(){
    if(g_gameOver){debugAppendRaw("UNDO-BLOCK","Completed game cannot be modified after its result/history/profile statistics are committed.");MessageBoxW(g_mainHwnd,L"This game is already complete and has been recorded. Undo is disabled to keep History and Player Profile statistics consistent.",L"Game already complete",MB_OK|MB_ICONINFORMATION);return;}
    if(g_calculating||g_game.histCount<=0)return;g_positionVersion++;g_suggestionPending=false;g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;g_requestGuideKind=GUIDE_NONE;g_choiceRequestPending=false;g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;clearEmergencyRescue();g_threatCheckPending=false;g_selected=-1;g_gameOver=false;g_correctionMode=false;unmarkTrainingPly(g_game.histCount-1);gameUndo();updateHistory();invalidateBoard();checkNeedResponse();
}
static void manualMove(){
    if(!boardInputAllowed())return;wchar_t w[64];int n=GetWindowTextW(hSan,w,64);if(n<=0)return;char s[64];int j=0;for(int i=0;i<n&&j<63;i++)if(w[i]<128)s[j++]=(char)w[i];s[j]=0;Move m;if(!parseManualMove(s,&m)){MessageBoxW(g_mainHwnd,L"That is not a legal SAN move in the current position. Examples: e4, Nf3, O-O, exd8=Q.",L"Invalid move",MB_OK|MB_ICONWARNING);return;}SetWindowTextW(hSan,L"");processMove(m);
}

// Promotion popup is intentionally simple and modal, with four explicit buttons.
static int g_promoChoice=0;static HWND g_promoHwnd=NULLPTR;
static LRESULT CALLBACK promoProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_COMMAND){int id=lowWord(w);if(id>=2001&&id<=2004){g_promoChoice=id;DestroyWindow(hwnd);return 0;}}
    if(msg==WM_CLOSE){g_promoChoice=0;DestroyWindow(hwnd);return 0;}
    if(msg==WM_DESTROY){g_promoHwnd=NULLPTR;return 0;}return DefWindowProcW(hwnd,msg,w,l);
}
static char choosePromotion(){
    static bool registered=false;if(!registered){WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=CS_HREDRAW|CS_VREDRAW;wc.lpfnWndProc=promoProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=GetSysColorBrush(COLOR_WINDOW);wc.lpszClassName=L"ChessTrainerPromotion";RegisterClassExW(&wc);registered=true;}
    g_promoChoice=0;g_promoHwnd=CreateWindowExW(0,L"ChessTrainerPromotion",L"Choose promotion",WS_CAPTION|WS_SYSMENU|WS_VISIBLE,360,260,390,130,g_mainHwnd,NULLPTR,g_hInstance,NULLPTR);
    HWND b1=CreateWindowExW(0,L"BUTTON",L"Queen",WS_CHILD|WS_VISIBLE|WS_TABSTOP,12,24,82,30,g_promoHwnd,(HMENU)(ULONG_PTR)2001,g_hInstance,NULLPTR);
    HWND b2=CreateWindowExW(0,L"BUTTON",L"Rook",WS_CHILD|WS_VISIBLE|WS_TABSTOP,102,24,82,30,g_promoHwnd,(HMENU)(ULONG_PTR)2002,g_hInstance,NULLPTR);
    HWND b3=CreateWindowExW(0,L"BUTTON",L"Bishop",WS_CHILD|WS_VISIBLE|WS_TABSTOP,192,24,82,30,g_promoHwnd,(HMENU)(ULONG_PTR)2003,g_hInstance,NULLPTR);
    HWND b4=CreateWindowExW(0,L"BUTTON",L"Knight",WS_CHILD|WS_VISIBLE|WS_TABSTOP,282,24,82,30,g_promoHwnd,(HMENU)(ULONG_PTR)2004,g_hInstance,NULLPTR);
    SendMessageW(b1,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(b2,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(b3,WM_SETFONT,(WPARAM)g_uiFont,1);SendMessageW(b4,WM_SETFONT,(WPARAM)g_uiFont,1);
    EnableWindow(g_mainHwnd,FALSE);MSG msg;while(g_promoChoice==0&&GetMessageW(&msg,NULLPTR,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);if(!g_promoHwnd)break;}EnableWindow(g_mainHwnd,TRUE);SetFocus(g_mainHwnd);
    if(g_promoChoice==2001)return 'q';if(g_promoChoice==2002)return 'r';if(g_promoChoice==2003)return 'b';if(g_promoChoice==2004)return 'n';return 0;
}

static int displayToSquare(int col,int row){if(!g_flipped)return (7-row)*8+col;return row*8+(7-col);}
static void squareToDisplay(int sq,int&col,int&row){int f=fileOf(sq),r=rankOf(sq);if(!g_flipped){col=f;row=7-r;}else{col=7-f;row=r;}}
static void boardClick(int x,int y){
    if(!boardInputAllowed()||x<BOARD_X||x>=BOARD_X+BOARD_SIZE||y<BOARD_Y||y>=BOARD_Y+BOARD_SIZE)return;int col=(x-BOARD_X)/SQ,row=(y-BOARD_Y)/SQ,sq=displayToSquare(col,row);char pc=g_game.pos.sq[sq];
    if(g_selected<0){if(pc&&sameColor(pc,g_game.pos.whiteToMove)){g_selected=sq;invalidateBoard();}return;}
    if(sq==g_selected){g_selected=-1;invalidateBoard();return;}if(pc&&sameColor(pc,g_game.pos.whiteToMove)){g_selected=sq;invalidateBoard();return;}
    int from=g_selected;g_selected=-1;Move legal[256],cand[4];int n=legalMovesFor(&g_game.pos,legal,256),cn=0;for(int i=0;i<n;i++)if(legal[i].from==from&&legal[i].to==sq&&cn<4)cand[cn++]=legal[i];if(cn==0){MessageBoxW(g_mainHwnd,L"That move is not legal in the current position.",L"Illegal move",MB_OK|MB_ICONWARNING);invalidateBoard();return;}Move m=cand[0];if(cn>1){char p=choosePromotion();if(!p){invalidateBoard();return;}bool found=false;for(int i=0;i<cn;i++)if(cand[i].promo==p){m=cand[i];found=true;break;}if(!found)return;}processMove(m);
}

static wchar_t pieceGlyph(char p){switch(p){case 'K':return L'♔';case 'Q':return L'♕';case 'R':return L'♖';case 'B':return L'♗';case 'N':return L'♘';case 'P':return L'♙';case 'k':return L'♚';case 'q':return L'♛';case 'r':return L'♜';case 'b':return L'♝';case 'n':return L'♞';case 'p':return L'♟';}return 0;}
static wchar_t filledGlyphForWhite(char p){
    // Use the solid black-piece glyph as a white underlay, then draw the normal
    // white-piece outline glyph in dark ink. This gives white pieces a genuinely
    // white interior instead of allowing the board square colour to show through.
    switch(p){case 'K':return L'♚';case 'Q':return L'♛';case 'R':return L'♜';case 'B':return L'♝';case 'N':return L'♞';case 'P':return L'♟';}
    return 0;
}
static void squareCenter(int sq,int&x,int&y){int c,r;squareToDisplay(sq,c,r);x=BOARD_X+c*SQ+SQ/2;y=BOARD_Y+r*SQ+SQ/2;}
static void drawColoredArrow(HDC dc,int from,int to,DWORD color,int width){int x1,y1,x2,y2;squareCenter(from,x1,y1);squareCenter(to,x2,y2);int dx=x2-x1,dy=y2-y1,n=iMax(iAbs(dx),iAbs(dy));if(n<1)return;HPEN pen=CreatePen(PS_SOLID,width,color);HGDIOBJ oldPen=SelectObject(dc,pen);MoveToEx(dc,x1,y1,NULLPTR);LineTo(dc,x2,y2);int bx=x2-dx*28/n,by=y2-dy*28/n,px=-dy*14/n,py=dx*14/n;POINT pts[3]={{x2,y2},{bx+px,by+py},{bx-px,by-py}};HBRUSH br=CreateSolidBrush(color);HGDIOBJ oldBr=SelectObject(dc,br);Polygon(dc,pts,3);SelectObject(dc,oldBr);SelectObject(dc,oldPen);DeleteObject(br);DeleteObject(pen);}
static void drawArrow(HDC dc,int from,int to){drawColoredArrow(dc,from,to,RGBc(0,200,83),8);}
static int overlayArrowPenalty(const Move& m,int l,int t,int r,int b){
    int x1,y1,x2,y2;squareCenter(m.from,x1,y1);squareCenter(m.to,x2,y2);
    int al=iMin(x1,x2)-34,ar=iMax(x1,x2)+34,at=iMin(y1,y2)-34,ab=iMax(y1,y2)+34;
    int ix=iMin(r,ar)-iMax(l,al),iy=iMin(b,ab)-iMax(t,at);if(ix<=0||iy<=0)return 0;return ix*iy;
}
static void drawMoveExplanationOverlay(HDC dc){
    if(!g_moveExplainActive)return;
    if(g_moveExplainHasReply)drawColoredArrow(dc,g_moveExplainReply.from,g_moveExplainReply.to,RGBc(214,73,73),iMax(5,SQ/12));
    if(g_moveExplainHasBetter)drawColoredArrow(dc,g_moveExplainBetter.from,g_moveExplainBetter.to,RGBc(46,160,88),iMax(5,SQ/12));
    int cardW=iMin(BOARD_SIZE-40,720),cardH=112,left=BOARD_X+(BOARD_SIZE-cardW)/2;
    int topA=BOARD_Y+18,topB=BOARD_Y+BOARD_SIZE-cardH-18;
    int scoreA=0,scoreB=0;
    if(g_moveExplainHasReply){scoreA+=overlayArrowPenalty(g_moveExplainReply,left,topA,left+cardW,topA+cardH);scoreB+=overlayArrowPenalty(g_moveExplainReply,left,topB,left+cardW,topB+cardH);}
    if(g_moveExplainHasBetter){scoreA+=overlayArrowPenalty(g_moveExplainBetter,left,topA,left+cardW,topA+cardH);scoreB+=overlayArrowPenalty(g_moveExplainBetter,left,topB,left+cardW,topB+cardH);}
    int top=scoreA<=scoreB?topA:topB;
    DWORD edge=logTagColor(g_moveExplainClass);HBRUSH bg=CreateSolidBrush(RGBc(250,249,240));HPEN p=CreatePen(PS_SOLID,2,edge);HGDIOBJ ob=SelectObject(dc,bg),op=SelectObject(dc,p);RoundRect(dc,left,top,left+cardW,top+cardH,14,14);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(p);DeleteObject(bg);
    SelectObject(dc,g_headingFont);SetTextColor(dc,edge);SetBkMode(dc,TRANSPARENT);RECT tt={left+14,top+7,left+cardW-14,top+31};DrawTextW(dc,(LPWSTR)reviewClassName(g_moveExplainClass),-1,&tt,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(40,47,52));RECT tr={left+14,top+32,left+cardW-14,top+cardH-20};DrawTextW(dc,g_moveExplainText,-1,&tr,DT_LEFT|DT_WORDBREAK);
    SetTextColor(dc,RGBc(105,112,118));RECT hint={left+14,top+cardH-19,left+cardW-14,top+cardH-5};DrawTextW(dc,(LPWSTR)L"Click anywhere on the board to dismiss",-1,&hint,DT_RIGHT|DT_SINGLELINE|DT_VCENTER);
}
static bool pointInRectI(int x,int y,int l,int t,int r,int b){return x>=l&&x<r&&y>=t&&y<b;}

static void drawSectionIcon(HDC dc,int kind,int x,int y,DWORD color){
    HPEN pen=CreatePen(PS_SOLID,2,color);HGDIOBJ old=SelectObject(dc,pen);
    if(kind==1){ // analysis bars
        MoveToEx(dc,x,y+14,NULLPTR);LineTo(dc,x,y+7);MoveToEx(dc,x+6,y+14,NULLPTR);LineTo(dc,x+6,y+3);MoveToEx(dc,x+12,y+14,NULLPTR);LineTo(dc,x+12,y);
    }else if(kind==2){ // log lines
        for(int i=0;i<3;i++){MoveToEx(dc,x,y+i*6+2,NULLPTR);LineTo(dc,x+14,y+i*6+2);}
    }else{ // pencil
        MoveToEx(dc,x,y+13,NULLPTR);LineTo(dc,x+12,y+1);MoveToEx(dc,x+8,y+1,NULLPTR);LineTo(dc,x+13,y+6);MoveToEx(dc,x,y+13,NULLPTR);LineTo(dc,x+5,y+14);
    }
    SelectObject(dc,old);DeleteObject(pen);
}

static void drawSectionHeader(HDC dc,int kind,int y,const wchar_t* text,int textLen){
    // Hover changes colour only. Geometry is deliberately invariant, no changing
    // border, margin, underline width, padding or control size, which prevents
    // the visual vibration seen in the previous build.
    bool hover=(g_hoverSection==kind);
    DWORD fg=hover?RGBc(58,99,48):RGBc(34,42,49);
    drawSectionIcon(dc,kind,EVAL_X,y+2,fg);
    SelectObject(dc,g_headingFont);SetTextColor(dc,fg);TextOutW(dc,EVAL_X+24,y,text,textLen);
    HBRUSH accent=CreateSolidBrush(hover?RGBc(91,128,69):RGBc(115,149,82));
    RECT line={EVAL_X,y+28,EVAL_X+64,y+31};
    FillRect(dc,&line,accent);DeleteObject(accent);
}

static void drawGameLog(HDC dc){
    // Game Log V5.0: two clean player columns. Each move-quality tag sits beside
    // the exact White/Black move it describes, and the move counter lives in the
    // compact on-board summary instead of consuming a third table column.
    int x=EVAL_X,y=HISTORY_Y,w=EVAL_W,h=HISTORY_H;if(w<120||h<100)return;

    HBRUSH bg=CreateSolidBrush(RGBc(252,253,253));HPEN border=CreatePen(PS_SOLID,1,RGBc(211,217,222));HGDIOBJ ob=SelectObject(dc,bg),op=SelectObject(dc,border);RoundRect(dc,x,y,x+w,y+h,10,10);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(border);DeleteObject(bg);

    const int openingH=82;char openingName[64],openingEco[8];recognizeOpening(openingName,64,openingEco,8);
    SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));TextOutW(dc,x+12,y+7,L"Opening",7);
    wchar_t weco[20]={0};wAppendAscii(weco,20,openingEco);SetTextColor(dc,RGBc(91,128,69));RECT erc={x+w-72,y+5,x+w-12,y+25};DrawTextW(dc,weco,-1,&erc,DT_RIGHT|DT_SINGLELINE|DT_VCENTER);

    wchar_t won[128]={0};wAppendAscii(won,128,openingName);SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(34,42,49));RECT orc={x+12,y+23,x+w-12,y+58};DrawTextW(dc,won,-1,&orc,DT_LEFT|DT_WORDBREAK);

    int whiteNow=countPieces(&g_game.pos,true),blackNow=countPieces(&g_game.pos,false);wchar_t pieces[160]={0};wAppend(pieces,160,L"On board: White ");wAppendInt(pieces,160,whiteNow);wAppend(pieces,160,L"  |  Black ");wAppendInt(pieces,160,blackNow);wAppend(pieces,160,L"  |  Moves ");wAppendInt(pieces,160,(g_game.histCount+1)/2);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT prc={x+12,y+58,x+w-12,y+79};DrawTextW(dc,pieces,-1,&prc,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

    HPEN osep=CreatePen(PS_SOLID,1,RGBc(231,235,237));HGDIOBJ oop=SelectObject(dc,osep);MoveToEx(dc,x+8,y+openingH,NULLPTR);LineTo(dc,x+w-8,y+openingH);SelectObject(dc,oop);DeleteObject(osep);

    const int headerH=30;int tableY=y+openingH+1;HBRUSH hb=CreateSolidBrush(RGBc(237,243,234));RECT hdr={x+1,tableY,x+w-1,tableY+headerH};FillRect(dc,&hdr,hb);DeleteObject(hb);
    int numW=34,usable=w-numW-4,whiteW=usable/2,blackW=usable-whiteW;int whiteX=x+numW,blackX=whiteX+whiteW;

    // Strong turn-state indication using the same colours as Suggested Move / Different Move.
    // Green means it is the user's turn; red means it is the opponent's turn.
    bool activeWhite=false,usersTurn=false,showTurn=g_userSet&&!g_gameOver;
    if(showTurn){activeWhite=g_game.pos.whiteToMove;usersTurn=(activeWhite==g_userWhite);DWORD fill=usersTurn?RGBc(40,135,70):RGBc(204,69,72);DWORD edge=usersTurn?RGBc(28,103,52):RGBc(153,45,48);RECT tr=activeWhite?RECT{whiteX+1,tableY+2,whiteX+whiteW-1,tableY+headerH-2}:RECT{blackX+1,tableY+2,x+w-2,tableY+headerH-2};HBRUSH tb=CreateSolidBrush(fill);HPEN tp=CreatePen(PS_SOLID,1,edge);HGDIOBJ tob=SelectObject(dc,tb),top=SelectObject(dc,tp);RoundRect(dc,tr.left,tr.top,tr.right,tr.bottom,7,7);SelectObject(dc,top);SelectObject(dc,tob);DeleteObject(tp);DeleteObject(tb);}

    SelectObject(dc,g_smallFont);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGBc(76,88,96));RECT c0={x+10,tableY,x+numW,tableY+headerH};DrawTextW(dc,(LPWSTR)L"#",-1,&c0,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    RECT c1={whiteX+4,tableY,whiteX+whiteW-4,tableY+headerH};SetTextColor(dc,(showTurn&&activeWhite)?RGBc(255,255,255):RGBc(76,88,96));DrawTextW(dc,(LPWSTR)L"White",-1,&c1,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    RECT c2={blackX+4,tableY,x+w-5,tableY+headerH};SetTextColor(dc,(showTurn&&!activeWhite)?RGBc(255,255,255):RGBc(76,88,96));DrawTextW(dc,(LPWSTR)L"Black",-1,&c2,DT_CENTER|DT_SINGLELINE|DT_VCENTER);

    HPEN sep=CreatePen(PS_SOLID,1,RGBc(220,225,229));HGDIOBJ oldp=SelectObject(dc,sep);MoveToEx(dc,x+1,tableY+headerH,NULLPTR);LineTo(dc,x+w-1,tableY+headerH);MoveToEx(dc,whiteX,tableY,NULLPTR);LineTo(dc,whiteX,y+h-4);MoveToEx(dc,blackX,tableY,NULLPTR);LineTo(dc,blackX,y+h-4);SelectObject(dc,oldp);DeleteObject(sep);

    int bodyTop=tableY+headerH+1,bodyH=y+h-bodyTop-3;int rowH=28;int maxRows=bodyH/rowH;if(maxRows<1)maxRows=1;int pairCount=(g_game.histCount+1)/2;
    if(pairCount==0){SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(120,130,137));RECT empty={x+24,bodyTop+18,x+w-24,y+h-18};DrawTextW(dc,(LPWSTR)L"Moves will appear here\nas the game progresses.",-1,&empty,DT_CENTER|DT_VCENTER|DT_WORDBREAK);return;}

    // Long games use a head/tail view: the opening move pairs remain visible,
    // an ellipsis marks omitted middle moves, and the latest moves stay visible.
    int headRows=0,tailStart=0;bool compacted=pairCount>maxRows;if(compacted){headRows=maxRows>=8?4:2;int tailRows=maxRows-headRows-1;if(tailRows<1)tailRows=1;tailStart=pairCount-tailRows;}
    int visualRow=0;
    for(int pair=0;pair<pairCount&&visualRow<maxRows;pair++){
        if(compacted&&pair==headRows){
            int ry=bodyTop+visualRow*rowH;SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(125,134,140));RECT dots={x+8,ry,x+w-8,ry+rowH};DrawTextW(dc,(LPWSTR)L"... middle moves omitted ...",-1,&dots,DT_CENTER|DT_SINGLELINE|DT_VCENTER);visualRow++;pair=tailStart-1;continue;
        }
        if(compacted&&pair>headRows-1&&pair<tailStart)continue;
        int ry=bodyTop+visualRow*rowH;RECT rr={x+2,ry,x+w-2,ry+rowH};HBRUSH rb=CreateSolidBrush((visualRow&1)?RGBc(248,250,250):RGBc(253,254,254));FillRect(dc,&rr,rb);DeleteObject(rb);
        int whiteIndex=pair*2,blackIndex=whiteIndex+1,latest=g_game.histCount-1;bool latestWhite=(latest==whiteIndex),latestBlack=(latest==blackIndex);
        if(latestWhite){HBRUSH hi=CreateSolidBrush(RGBc(226,241,220));RECT hc={whiteX+1,ry+2,whiteX+whiteW-2,ry+rowH-2};FillRect(dc,&hc,hi);DeleteObject(hi);}else if(latestBlack){HBRUSH hi=CreateSolidBrush(RGBc(226,241,220));RECT hc={blackX+1,ry+2,x+w-2,ry+rowH-2};FillRect(dc,&hc,hi);DeleteObject(hi);}
        wchar_t moveNo[16]={0};wAppendInt(moveNo,16,pair+1);wAppend(moveNo,16,L".");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(129,139,145));RECT nr={x+10,ry,x+numW-4,ry+rowH};DrawTextW(dc,moveNo,-1,&nr,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

        // Each side gets its own move + quality tag. The tag is right beside the move
        // it grades, so there is no ambiguity about whether White or Black made it.
        int tagPart=iMin(58,iMax(44,whiteW*40/100));
        wchar_t wsan[64]={0};wAppendAscii(wsan,64,g_game.hist[whiteIndex].san);wAppend(wsan,64,L" (");wAppendInt(wsan,64,opponentPiecesLeftAfterPly(whiteIndex));wAppend(wsan,64,L")");SelectObject(dc,g_monoBoldFont?g_monoBoldFont:g_monoFont);SetTextColor(dc,latestWhite?RGBc(45,96,42):RGBc(34,42,49));RECT wr={whiteX+4,ry,whiteX+whiteW-tagPart-2,ry+rowH};DrawTextW(dc,wsan,-1,&wr,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
        const wchar_t* wt=logTagName(g_reviewClass[whiteIndex]);if(wt&&wt[0]){SelectObject(dc,g_smallFont);SetTextColor(dc,logTagColor(g_reviewClass[whiteIndex]));RECT wtr={whiteX+whiteW-tagPart,ry,whiteX+whiteW-4,ry+rowH};DrawTextW(dc,(LPWSTR)wt,-1,&wtr,DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);}

        if(blackIndex<g_game.histCount){
            int bTagPart=iMin(58,iMax(44,blackW*40/100));wchar_t bsan[64]={0};wAppendAscii(bsan,64,g_game.hist[blackIndex].san);wAppend(bsan,64,L" (");wAppendInt(bsan,64,opponentPiecesLeftAfterPly(blackIndex));wAppend(bsan,64,L")");SelectObject(dc,g_monoBoldFont?g_monoBoldFont:g_monoFont);SetTextColor(dc,latestBlack?RGBc(45,96,42):RGBc(34,42,49));RECT br={blackX+4,ry,x+w-bTagPart-4,ry+rowH};DrawTextW(dc,bsan,-1,&br,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
            const wchar_t* bt=logTagName(g_reviewClass[blackIndex]);if(bt&&bt[0]){SelectObject(dc,g_smallFont);SetTextColor(dc,logTagColor(g_reviewClass[blackIndex]));RECT btr={x+w-bTagPart-2,ry,x+w-5,ry+rowH};DrawTextW(dc,(LPWSTR)bt,-1,&btr,DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);}
        }
        HPEN line=CreatePen(PS_SOLID,1,RGBc(235,238,240));HGDIOBJ lp=SelectObject(dc,line);MoveToEx(dc,blackX,ry,NULLPTR);LineTo(dc,blackX,ry+rowH);MoveToEx(dc,x+8,ry+rowH-1,NULLPTR);LineTo(dc,x+w-8,ry+rowH-1);SelectObject(dc,lp);DeleteObject(line);visualRow++;
    }
}

static void panelTabRects(RECT* a,RECT* b,RECT* c){int y=PANEL_TOP+14,gap=6,usable=EVAL_W-gap*2;int aw=usable*28/100,bw=usable*44/100,cw=usable-aw-bw;a->left=EVAL_X;a->top=y;a->right=EVAL_X+aw;a->bottom=y+32;b->left=a->right+gap;b->top=y;b->right=b->left+bw;b->bottom=y+32;c->left=b->right+gap;c->top=y;c->right=EVAL_X+EVAL_W;c->bottom=y+32;(void)cw;}
static void drawPanelTabs(HDC dc){
    RECT a,b,c;panelTabRects(&a,&b,&c);SetBkMode(dc,TRANSPARENT);SelectObject(dc,g_uiFont);SetTextColor(dc,g_panelMode==0?RGBc(43,78,36):RGBc(100,109,116));DrawTextW(dc,(LPWSTR)L"Analysis",-1,&a,DT_CENTER|DT_SINGLELINE|DT_VCENTER);SetTextColor(dc,g_panelMode==1?RGBc(43,78,36):RGBc(100,109,116));DrawTextW(dc,(LPWSTR)L"Game Insights",-1,&b,DT_CENTER|DT_SINGLELINE|DT_VCENTER);SetTextColor(dc,g_panelMode==2?RGBc(43,78,36):RGBc(100,109,116));DrawTextW(dc,(LPWSTR)L"History",-1,&c,DT_CENTER|DT_SINGLELINE|DT_VCENTER);HBRUSH line=CreateSolidBrush(RGBc(115,149,82));RECT u=g_panelMode==0?RECT{a.left,a.bottom-3,a.right,a.bottom}:g_panelMode==1?RECT{b.left,b.bottom-3,b.right,b.bottom}:RECT{c.left,c.bottom-3,c.right,c.bottom};FillRect(dc,&u,line);DeleteObject(line);
}
static void drawInfoRow(HDC dc,int y,const wchar_t* label,const wchar_t* value){
    SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(108,117,125));RECT l={EVAL_X+8,y,EVAL_X+132,y+26};DrawTextW(dc,(LPWSTR)label,-1,&l,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    SelectObject(dc,g_buttonFont);SetTextColor(dc,RGBc(39,48,54));RECT v={EVAL_X+132,y,EVAL_X+EVAL_W-8,y+26};DrawTextW(dc,(LPWSTR)value,-1,&v,DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
}
static void drawGameTimeInfoRow(HDC dc,int y){
    U64 u=0,o=0,t=0;currentGameClock(&u,&o,&t);wchar_t tv[32]={0},uv[32]={0},ov[32]={0},value[128]={0};formatDurationMs(t,tv);formatDurationMs(u,uv);formatDurationMs(o,ov);
    wAppend(value,128,L"Total ");wAppend(value,128,tv);wAppend(value,128,L"  |  You ");wAppend(value,128,uv);wAppend(value,128,L"  |  Opp ");wAppend(value,128,ov);
    SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT l={EVAL_X+8,y,EVAL_X+82,y+24};DrawTextW(dc,(LPWSTR)L"Game time",-1,&l,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    SetTextColor(dc,RGBc(39,48,54));RECT v={EVAL_X+82,y,EVAL_X+EVAL_W-8,y+24};DrawTextW(dc,value,-1,&v,DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
}
static bool compactInsights(){return INSIGHTS_BOTTOM-INSIGHTS_TOP<430;}
static void ratingRowRect(RECT* r){bool compact=compactInsights();int top=INSIGHTS_TOP+38,row0=compact?58:62,step=compact?20:23;r->left=EVAL_X;r->top=top+row0+step*4;r->right=EVAL_X+EVAL_W;r->bottom=r->top+step;}
static int liveCurrentCardTop(){return INSIGHTS_TOP+38;}
static int liveCurrentCardHeight(bool compact){return compact?184:206;}
static int liveEndCardTop(bool compact){return liveCurrentCardTop()+liveCurrentCardHeight(compact)+insightSectionGap(compact);}
static int liveEndCardHeight(bool compact){return g_userSet?(compact?76:84):(compact?42:46);}
static int liveTrainingTop(bool compact){return liveEndCardTop(compact)+liveEndCardHeight(compact)+insightSectionGap(compact);}
static void outcomeRect(int idx,RECT* r){
    bool compact=compactInsights();int labelW=38,rightPad=8,gap=5,bh=compact?22:24;int usable=EVAL_W-labelW-rightPad-gap*3;int bw=usable/4;if(bw<48)bw=48;int col=idx<6?idx/2:3;int row=idx<6?(idx&1):idx-6;int firstY=liveEndCardTop(compact)+(compact?26:30);int x=EVAL_X+labelW+col*(bw+gap),y=firstY+row*(bh+4);r->left=x;r->top=y;r->right=x+bw;r->bottom=y+bh;
}
static void drawInsightButton(HDC dc,int idx,const wchar_t* text){
    RECT r;outcomeRect(idx,&r);HBRUSH br=CreateSolidBrush(RGBc(247,249,250));HPEN p=CreatePen(PS_SOLID,1,RGBc(198,207,212));HGDIOBJ ob=SelectObject(dc,br),op=SelectObject(dc,p);RoundRect(dc,r.left,r.top,r.right,r.bottom,8,8);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(p);DeleteObject(br);SelectObject(dc,g_buttonFont);SetTextColor(dc,RGBc(51,60,66));SetBkMode(dc,TRANSPARENT);DrawTextW(dc,(LPWSTR)text,-1,&r,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
}
static void appendRatingValue(wchar_t out[128]){
    out[0]=0;if(g_guestProfile){wCopy(out,128,L"Guest, session only");return;}if(g_trainerMode==MODE_PRACTICE||g_trainerMode==MODE_COACH){wCopy(out,128,g_trainerMode==MODE_COACH?L"Not used in Coach Mode":L"Not used in practice mode");return;}if(g_gameOver&&g_ratingBeforeGame>0&&g_opponentRating>0&&g_lastRatingAfter>0){wAppendInt(out,128,g_ratingBeforeGame);wAppend(out,128,L" -> ");wAppendInt(out,128,g_lastRatingAfter);wAppend(out,128,L"  (");if(g_lastRatingChange>=0)wAppend(out,128,L"+");wAppendInt(out,128,g_lastRatingChange);wAppend(out,128,L")  Edit");}
    else if(g_ratingBeforeGame>0||g_currentRating>0){wAppendInt(out,128,g_ratingBeforeGame>0?g_ratingBeforeGame:g_currentRating);if(g_opponentRating>0){wAppend(out,128,L" vs ");wAppendInt(out,128,g_opponentRating);}wAppend(out,128,L"  Edit");}
    else wCopy(out,128,L"Set ratings");
}
static void appendCp(wchar_t* out,int cap,int cp){if(cp<0){wAppend(out,cap,L"-");cp=-cp;}wAppendInt(out,cap,cp/100);wAppend(out,cap,L".");int d=(cp%100)/10;wAppendInt(out,cap,d);}
static void buildReviewMoveLine(int ply,bool user,wchar_t out[180]){
    out[0]=0;wAppend(out,180,user?L"You: ":L"Opponent: ");int moveNo=ply/2+1;wAppendInt(out,180,moveNo);if(ply&1)wAppend(out,180,L"...");else wAppend(out,180,L".");wAppendAscii(out,180,g_game.hist[ply].san);wAppend(out,180,L"  ");wAppend(out,180,reviewClassName(g_reviewClass[ply]));if(g_reviewLoss[ply]>0&&g_reviewLoss[ply]<9000){wAppend(out,180,L"  (-");appendCp(out,180,g_reviewLoss[ply]);wAppend(out,180,L")");}
}
static void drawReviewSummary(HDC dc,int y,int bottom){
    SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,EVAL_X,y,L"Trainer Review",14);y+=28;
    if(g_reviewInProgress){SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));RECT r={EVAL_X,y,EVAL_X+EVAL_W,y+52};DrawTextW(dc,(LPWSTR)L"Reviewing every move with Stockfish...\nThis normally takes only a few seconds.",-1,&r,DT_LEFT|DT_WORDBREAK);return;}
    if(!g_reviewReady){SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(120,128,134));TextOutW(dc,EVAL_X,y,L"No move review is available for this game.",39);return;}
    int uc[12],oc[12];countReviewForSide(true,uc);countReviewForSide(false,oc);
    const int classes[10]={RV_BRILLIANT,RV_GREAT,RV_BOOK,RV_BEST,RV_EXCELLENT,RV_GOOD,RV_INACCURACY,RV_MISTAKE,RV_MISS,RV_BLUNDER};
    const wchar_t* labels[10]={L"Brilliant",L"Great",L"Book",L"Best",L"Excellent",L"Good",L"Inaccuracy",L"Mistake",L"Miss",L"Blunder"};
    int headerH=21,rowH=17,totalH=headerH+rowH*10;if(y+totalH>bottom){rowH=15;totalH=headerH+rowH*10;}if(y+totalH>bottom){rowH=13;totalH=headerH+rowH*10;}if(y+totalH>bottom){rowH=12;totalH=headerH+rowH*10;}
    int metricW=EVAL_W*48/100,numW=(EVAL_W-metricW)/2;int x0=EVAL_X,x1=x0+metricW,x2=x1+numW,x3=EVAL_X+EVAL_W;
    HBRUSH hb=CreateSolidBrush(RGBc(237,243,234));RECT hr={x0,y,x3,y+headerH};FillRect(dc,&hr,hb);DeleteObject(hb);SelectObject(dc,rowH>=15?g_buttonFont:g_smallFont);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGBc(72,83,90));RECT h0={x0+7,y,x1-4,y+headerH};DrawTextW(dc,(LPWSTR)L"Quality",-1,&h0,DT_LEFT|DT_SINGLELINE|DT_VCENTER);RECT h1={x1,y,x2,y+headerH};DrawTextW(dc,(LPWSTR)L"You",-1,&h1,DT_CENTER|DT_SINGLELINE|DT_VCENTER);RECT h2={x2,y,x3,y+headerH};DrawTextW(dc,(LPWSTR)L"Opponent",-1,&h2,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    for(int i=0;i<10;i++){int ry=y+headerH+i*rowH;if(i&1){HBRUSH alt=CreateSolidBrush(RGBc(247,249,250));RECT rr={x0,ry,x3,ry+rowH};FillRect(dc,&rr,alt);DeleteObject(alt);}SetTextColor(dc,logTagColor(classes[i]));RECT lr={x0+7,ry,x1-4,ry+rowH};DrawTextW(dc,(LPWSTR)labels[i],-1,&lr,DT_LEFT|DT_SINGLELINE|DT_VCENTER);wchar_t a[16]={0},b[16]={0};wAppendInt(a,16,uc[classes[i]]);wAppendInt(b,16,oc[classes[i]]);SetTextColor(dc,RGBc(55,64,70));RECT ur={x1,ry,x2,ry+rowH},orr={x2,ry,x3,ry+rowH};DrawTextW(dc,a,-1,&ur,DT_CENTER|DT_SINGLELINE|DT_VCENTER);DrawTextW(dc,b,-1,&orr,DT_CENTER|DT_SINGLELINE|DT_VCENTER);}
    HPEN gp=CreatePen(PS_SOLID,1,RGBc(218,224,228));HGDIOBJ oldp=SelectObject(dc,gp);HGDIOBJ oldb=SelectObject(dc,GetStockObject(NULL_BRUSH));Rectangle(dc,x0,y,x3,y+totalH);SelectObject(dc,oldb);MoveToEx(dc,x1,y,NULLPTR);LineTo(dc,x1,y+totalH);MoveToEx(dc,x2,y,NULLPTR);LineTo(dc,x2,y+totalH);for(int i=0;i<=10;i++){int yy=y+headerH+i*rowH;MoveToEx(dc,x0,yy,NULLPTR);LineTo(dc,x3,yy);}SelectObject(dc,oldp);DeleteObject(gp);y+=totalH+5;
    int uw=worstReviewedPly(true),ow=worstReviewedPly(false);wchar_t n1[180]={0},n2[180]={0};if(uw>=0)buildReviewMoveLine(uw,true,n1);if(ow>=0)buildReviewMoveLine(ow,false,n2);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(150,67,67));if(uw>=0&&y+18<bottom){RECT r={EVAL_X,y,EVAL_X+EVAL_W,y+18};DrawTextW(dc,n1,-1,&r,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);y+=18;}if(ow>=0&&y+18<bottom){RECT r={EVAL_X,y,EVAL_X+EVAL_W,y+18};DrawTextW(dc,n2,-1,&r,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);}
}
static const wchar_t* archiveResultShort(int r){return r==RESULT_WIN?L"Win":r==RESULT_LOSS?L"Loss":r==RESULT_DRAW?L"Draw":L"No result";}
static void historyRowRect(int visual,RECT* r){int top=INSIGHTS_TOP+50;int rh=58;r->left=EVAL_X;r->right=EVAL_X+EVAL_W;r->top=top+visual*rh;r->bottom=r->top+rh-6;}
static void drawArchivedReviewTable(HDC dc,const ArchivedGame& a,int y){if(!a.reviewReady){HBRUSH b=CreateSolidBrush(RGBc(247,249,250));RECT r={EVAL_X,y,EVAL_X+EVAL_W,y+48};FillRect(dc,&r,b);DeleteObject(b);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT t={EVAL_X+8,y+5,EVAL_X+EVAL_W-8,y+43};DrawTextW(dc,(LPWSTR)L"Detailed Trainer Review is not available for this older summary entry.",-1,&t,DT_LEFT|DT_WORDBREAK|DT_VCENTER);return;}int countsU[12]={0},countsO[12]={0};if(a.hasMoves&&a.reviewReady){for(int i=0;i<a.plies&&i<256;i++){int c=a.reviewClass[i];bool u=((i&1)==0)==(a.userWhite!=0);if(c>=0&&c<12){if(u)countsU[c]++;else countsO[c]++;}}}const int cls[6]={RV_BRILLIANT,RV_GREAT,RV_BEST,RV_EXCELLENT,RV_MISTAKE,RV_BLUNDER};const wchar_t* lab[6]={L"Brilliant",L"Great",L"Best",L"Excellent",L"Mistake",L"Blunder"};int x0=EVAL_X,x3=EVAL_X+EVAL_W,x1=x0+EVAL_W*48/100,x2=x1+(x3-x1)/2,hh=20,rh=17;HBRUSH hb=CreateSolidBrush(RGBc(237,243,234));RECT h={x0,y,x3,y+hh};FillRect(dc,&h,hb);DeleteObject(hb);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(72,83,90));RECT q={x0+6,y,x1,y+hh};DrawTextW(dc,(LPWSTR)L"Quality",-1,&q,DT_LEFT|DT_VCENTER|DT_SINGLELINE);RECT u={x1,y,x2,y+hh};DrawTextW(dc,(LPWSTR)L"You",-1,&u,DT_CENTER|DT_VCENTER|DT_SINGLELINE);RECT o={x2,y,x3,y+hh};DrawTextW(dc,(LPWSTR)L"Opponent",-1,&o,DT_CENTER|DT_VCENTER|DT_SINGLELINE);for(int i=0;i<6;i++){int yy=y+hh+i*rh;SetTextColor(dc,logTagColor(cls[i]));RECT l={x0+6,yy,x1,yy+rh};DrawTextW(dc,(LPWSTR)lab[i],-1,&l,DT_LEFT|DT_VCENTER|DT_SINGLELINE);wchar_t av[12]={0},bv[12]={0};wAppendInt(av,12,countsU[cls[i]]);wAppendInt(bv,12,countsO[cls[i]]);SetTextColor(dc,RGBc(55,64,70));RECT ar={x1,yy,x2,yy+rh},br={x2,yy,x3,yy+rh};DrawTextW(dc,av,-1,&ar,DT_CENTER|DT_VCENTER|DT_SINGLELINE);DrawTextW(dc,bv,-1,&br,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}HPEN p=CreatePen(PS_SOLID,1,RGBc(218,224,228));HGDIOBJ op=SelectObject(dc,p);HGDIOBJ ob=SelectObject(dc,GetStockObject(NULL_BRUSH));Rectangle(dc,x0,y,x3,y+hh+rh*6);SelectObject(dc,ob);MoveToEx(dc,x1,y,NULLPTR);LineTo(dc,x1,y+hh+rh*6);MoveToEx(dc,x2,y,NULLPTR);LineTo(dc,x2,y+hh+rh*6);SelectObject(dc,op);DeleteObject(p);}
static void drawArchivedMoves(HDC dc,const ArchivedGame& a,int y,int bottom){SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,EVAL_X,y,L"Saved Move Log",14);y+=26;if(!a.hasMoves){SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT r={EVAL_X,y,EVAL_X+EVAL_W,bottom};DrawTextW(dc,(LPWSTR)L"This is an older summary-only history entry. Detailed move archives are saved automatically for games played with Chess Trainer 5.1 and later.",-1,&r,DT_LEFT|DT_WORDBREAK);return;}int pairs=(a.plies+1)/2,rowH=20,maxRows=(bottom-y)/rowH;if(maxRows<1)return;int startPair=pairs>maxRows?pairs-maxRows:0;for(int p=startPair;p<pairs;p++){int yy=y+(p-startPair)*rowH;wchar_t no[12]={0};wAppendInt(no,12,p+1);wAppend(no,12,L".");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(125,134,140));RECT nr={EVAL_X,yy,EVAL_X+28,yy+rowH};DrawTextW(dc,no,-1,&nr,DT_LEFT|DT_VCENTER|DT_SINGLELINE);int wi=p*2,bi=wi+1;SelectObject(dc,g_monoFont);SetTextColor(dc,RGBc(34,42,49));wchar_t wv[64]={0};wAppendAscii(wv,64,a.san[wi]);if(a.reviewClass[wi]){wAppend(wv,64,L"  ");wAppend(wv,64,reviewClassName(a.reviewClass[wi]));}RECT wr={EVAL_X+30,yy,EVAL_X+EVAL_W/2,yy+rowH};DrawTextW(dc,wv,-1,&wr,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);if(bi<a.plies){wchar_t bv[64]={0};wAppendAscii(bv,64,a.san[bi]);if(a.reviewClass[bi]){wAppend(bv,64,L"  ");wAppend(bv,64,reviewClassName(a.reviewClass[bi]));}RECT br={EVAL_X+EVAL_W/2+4,yy,EVAL_X+EVAL_W,yy+rowH};DrawTextW(dc,bv,-1,&br,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}}}
static void drawHistoryPanel(HDC dc){
    if(!g_archiveLoaded)loadArchive();if(!g_profileGameMapLoaded)loadProfileGameMap();SetBkMode(dc,TRANSPARENT);int top=INSIGHTS_TOP,bottom=INSIGHTS_BOTTOM;
    if(!g_historyDetail){
        SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));TextOutW(dc,EVAL_X,top,L"Game History",12);
        wchar_t ph[96]={0};wAppend(ph,96,L"Profile: ");wAppend(ph,96,activeProfileName());SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(82,117,71));RECT phr={EVAL_X+120,top,EVAL_X+EVAL_W,top+24};DrawTextW(dc,ph,-1,&phr,DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
        SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT help={EVAL_X,top+24,EVAL_X+EVAL_W,top+44};DrawTextW(dc,(LPWSTR)L"Select a game to reopen its saved summary, review and move log.",-1,&help,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
        int activeCount=activeArchiveCount();int show=iMin(activeCount,7);
        if(activeCount==0){SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(125,133,139));RECT empty={EVAL_X,top+86,EVAL_X+EVAL_W,top+160};DrawTextW(dc,(LPWSTR)(g_guestProfile?L"Guest games are intentionally not added to persistent History.":L"No archived games exist for this profile yet."),-1,&empty,DT_CENTER|DT_WORDBREAK|DT_VCENTER);return;}
        for(int v=0;v<show;v++){
            int idx=recentActiveArchiveIndex(v);if(idx<0)continue;RECT r;historyRowRect(v,&r);HBRUSH b=CreateSolidBrush(v&1?RGBc(247,249,250):RGBc(252,253,253));HPEN p=CreatePen(PS_SOLID,1,RGBc(218,224,228));HGDIOBJ ob=SelectObject(dc,b),op=SelectObject(dc,p);RoundRect(dc,r.left,r.top,r.right,r.bottom,8,8);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(p);DeleteObject(b);
            ArchivedGame &ag=g_archive.games[idx];wchar_t line[180]={0};wAppend(line,180,archiveResultShort(ag.result));wAppend(line,180,L"   ");wAppendInt(line,180,ag.day);wAppend(line,180,L"/");wAppendInt(line,180,ag.month);wAppend(line,180,L"/");wAppendInt(line,180,ag.year);SelectObject(dc,g_uiFont);SetTextColor(dc,ag.result==RESULT_WIN?RGBc(45,110,58):ag.result==RESULT_LOSS?RGBc(170,63,63):RGBc(70,82,88));RECT t={r.left+10,r.top+5,r.right-10,r.top+27};DrawTextW(dc,line,-1,&t,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
            wchar_t sub[180]={0};wAppendAscii(sub,180,ag.openingName);wAppend(sub,180,L"  |  ");wAppendInt(sub,180,(ag.plies+1)/2);wAppend(sub,180,L" moves");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(100,109,116));RECT sr={r.left+10,r.top+27,r.right-10,r.bottom-4};DrawTextW(dc,sub,-1,&sr,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
        }
        if(activeCount>7){SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));TextOutW(dc,EVAL_X,bottom-22,L"Showing the 7 most recent games for this profile.",48);}return;
    }
    if(g_historySelected<0||g_historySelected>=(int)g_archive.count||!archiveOwnedByActive(g_historySelected)){g_historyDetail=false;g_historySelected=-1;return;}
    ArchivedGame &ag=g_archive.games[g_historySelected];HBRUSH back=CreateSolidBrush(RGBc(239,243,238));RECT br={EVAL_X,top,EVAL_X+108,top+28};FillRect(dc,&br,back);DeleteObject(back);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(45,96,42));DrawTextW(dc,(LPWSTR)L"< Back to history",-1,&br,DT_CENTER|DT_VCENTER|DT_SINGLELINE);SelectObject(dc,g_headingFont);SetTextColor(dc,RGBc(34,42,49));RECT tr={EVAL_X+118,top,EVAL_X+EVAL_W,top+28};DrawTextW(dc,(LPWSTR)archiveResultShort(ag.result),-1,&tr,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    wchar_t opn[120]={0};wAppendAscii(opn,120,ag.openingName);wAppend(opn,120,L"  (");wAppendAscii(opn,120,ag.openingEco);wAppend(opn,120,L")");SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(45,96,42));RECT orc={EVAL_X,top+34,EVAL_X+EVAL_W,top+58};DrawTextW(dc,opn,-1,&orc,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
    wchar_t meta[180]={0};wAppendInt(meta,180,ag.day);wAppend(meta,180,L"/");wAppendInt(meta,180,ag.month);wAppend(meta,180,L"/");wAppendInt(meta,180,ag.year);wAppend(meta,180,L"  |  ");wAppendInt(meta,180,(ag.plies+1)/2);wAppend(meta,180,L" moves  |  ");wAppend(meta,180,ag.userWhite?L"Played White":L"Played Black");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT mr={EVAL_X,top+58,EVAL_X+EVAL_W,top+80};DrawTextW(dc,meta,-1,&mr,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);drawArchivedReviewTable(dc,ag,top+86);drawArchivedMoves(dc,ag,top+218,bottom);
}

static void drawGameInsights(HDC dc){
    bool compact=compactInsights();int displayMode=g_gameBaseMode;int top=liveCurrentCardTop();int bottom=INSIGHTS_BOTTOM;SetBkMode(dc,TRANSPARENT);
    // Dedicated toolbar row prevents Player Profile identity, Copy Insights and mode badge from colliding.
    SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(63,74,81));RECT pl={EVAL_X,INSIGHTS_TOP,EVAL_X+EVAL_W-130,INSIGHTS_TOP+30};wchar_t playerLine[96]={0};wAppend(playerLine,96,L"Player: ");wAppend(playerLine,96,activeProfileName());DrawTextW(dc,playerLine,-1,&pl,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
    char on[64],eco[8];recognizeOpening(on,64,eco,8);wchar_t opening[96]={0},weco[16]={0};wAppendAscii(opening,96,on);wAppendAscii(weco,16,eco);int cardH=liveCurrentCardHeight(compact);drawSectionCard(dc,EVAL_X,top,EVAL_X+EVAL_W,cardH);
    HBRUSH hb=CreateSolidBrush(RGBc(237,243,234));RECT hh={EVAL_X+1,top+1,EVAL_X+EVAL_W-1,top+32};FillRect(dc,&hh,hb);DeleteObject(hb);SelectObject(dc,g_buttonFont);SetTextColor(dc,RGBc(55,70,61));RECT ct={EVAL_X+10,top+2,EVAL_X+EVAL_W-160,top+31};DrawTextW(dc,(LPWSTR)L"Current Game",-1,&ct,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
    int badgeW=iMin(146,EVAL_W/3);RECT badge={EVAL_X+EVAL_W-badgeW-8,top+5,EVAL_X+EVAL_W-8,top+28};HBRUSH bb=CreateSolidBrush(RGBc(239,245,236));HPEN bp=CreatePen(PS_SOLID,1,RGBc(188,207,180));HGDIOBJ bob=SelectObject(dc,bb),bop=SelectObject(dc,bp);RoundRect(dc,badge.left,badge.top,badge.right,badge.bottom,8,8);SelectObject(dc,bop);SelectObject(dc,bob);DeleteObject(bp);DeleteObject(bb);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(45,96,42));DrawTextW(dc,(LPWSTR)trainerModeName(displayMode),-1,&badge,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
    SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(82,117,71));RECT ecoRc={EVAL_X+EVAL_W-60,top+34,EVAL_X+EVAL_W-10,top+56};DrawTextW(dc,weco,-1,&ecoRc,DT_RIGHT|DT_SINGLELINE|DT_VCENTER);SelectObject(dc,g_uiFont);SetTextColor(dc,RGBc(39,48,54));RECT openRc={EVAL_X+10,top+34,EVAL_X+EVAL_W-66,top+58};DrawTextW(dc,opening,-1,&openRc,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
    wchar_t side[16];wCopy(side,16,!g_userSet?L"Not selected":(g_userWhite?L"White":L"Black"));wchar_t moves[24]={0};wAppendInt(moves,24,(g_game.histCount+1)/2);wchar_t state[96]={0};if(g_gameOver){wAppend(state,96,resultLabel(g_currentResult));wAppend(state,96,L" - ");wAppend(state,96,reasonLabel(g_currentReason));}else wCopy(state,96,g_userSet?L"In progress":L"Not started");int row0=compact?58:62,step=compact?20:23;drawInfoRow(dc,top+row0,L"Playing as",side);drawInfoRow(dc,top+row0+step,L"Moves",moves);drawInfoRow(dc,top+row0+step*2,L"Status",state);
    wchar_t eval[64]={0};if(g_gameOver){if(g_currentResult==RESULT_WIN)wCopy(eval,64,L"Final: You won");else if(g_currentResult==RESULT_LOSS)wCopy(eval,64,L"Final: Opponent won");else if(g_currentResult==RESULT_DRAW)wCopy(eval,64,L"Final: Draw");else wCopy(eval,64,L"Final: No result");}else if(displayMode==MODE_FAIRPLAY)wCopy(eval,64,L"Post-game review only");else if(g_evalValid&&g_lastMate!=0){bool mateSide=g_lastMate>0?g_game.pos.whiteToMove:!g_game.pos.whiteToMove;wAppend(eval,64,mateSide?L"White mate in ":L"Black mate in ");wAppendInt(eval,64,iAbs(g_lastMate));}else if(g_evalValid){wAppend(eval,64,L"W ");wAppendInt(eval,64,(g_whiteWin+5)/10);wAppend(eval,64,L"%  D ");wAppendInt(eval,64,(g_drawChance+5)/10);wAppend(eval,64,L"%  B ");wAppendInt(eval,64,(g_blackWin+5)/10);wAppend(eval,64,L"%");}else if(displayMode==MODE_ASSISTED)wCopy(eval,64,L"Position analysis active");else if(displayMode==MODE_PRACTICE)wCopy(eval,64,L"Practice analysis active");else if(displayMode==MODE_COACH)wCopy(eval,64,L"Teaching analysis active");else wCopy(eval,64,L"Waiting for analysis");drawInfoRow(dc,top+row0+step*3,L"Evaluation",eval);
    wchar_t rating[128]={0};appendRatingValue(rating);drawInfoRow(dc,top+row0+step*4,L"Rating",rating);RECT rr;ratingRowRect(&rr);HPEN rp=CreatePen(PS_SOLID,1,RGBc(115,149,82));HGDIOBJ ro=SelectObject(dc,rp);MoveToEx(dc,rr.left+8,rr.bottom-2,NULLPTR);LineTo(dc,rr.right-8,rr.bottom-2);SelectObject(dc,ro);DeleteObject(rp);drawGameTimeInfoRow(dc,top+row0+step*5);
    int headerEnd=top+cardH;
    if(!g_gameOver){
        int endTop=liveEndCardTop(compact),endH=liveEndCardHeight(compact);drawSectionCard(dc,EVAL_X,endTop,EVAL_X+EVAL_W,endH,RGBc(250,251,252));SelectObject(dc,g_buttonFont);SetTextColor(dc,RGBc(63,74,81));RECT eh={EVAL_X+10,endTop+2,EVAL_X+EVAL_W-8,endTop+(compact?25:29)};DrawTextW(dc,(LPWSTR)L"End Game",-1,&eh,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
        if(g_userSet){RECT r0,r1;outcomeRect(0,&r0);outcomeRect(1,&r1);SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT ol={EVAL_X+7,r0.top,EVAL_X+33,r0.bottom};DrawTextW(dc,(LPWSTR)L"OPP",-1,&ol,DT_LEFT|DT_SINGLELINE|DT_VCENTER);RECT ul={EVAL_X+7,r1.top,EVAL_X+33,r1.bottom};DrawTextW(dc,(LPWSTR)L"YOU",-1,&ul,DT_LEFT|DT_SINGLELINE|DT_VCENTER);}
        int metricsY=liveTrainingTop(compact);if(g_userSet)drawTrainingMetrics(dc,metricsY,compact);int cy=g_userSet?metricsY+trainingMetricsHeight(compact)+10:metricsY;int attempts,wins,draws,losses,aborted;computeCareerStats(&attempts,&wins,&draws,&losses,&aborted);int completed=wins+draws+losses,winRate=completed?wins*100/completed:0;wchar_t stats[200]={0};wAppend(stats,200,L"Career: ");wAppendInt(stats,200,attempts);wAppend(stats,200,L" games  |  ");wAppendInt(stats,200,wins);wAppend(stats,200,L"W ");wAppendInt(stats,200,draws);wAppend(stats,200,L"D ");wAppendInt(stats,200,losses);wAppend(stats,200,L"L  |  Win ");wAppendInt(stats,200,winRate);wAppend(stats,200,L"%");if(aborted>0){wAppend(stats,200,L"  |  ");wAppendInt(stats,200,aborted);wAppend(stats,200,L" no result");}SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));RECT sr={EVAL_X,cy,EVAL_X+EVAL_W,cy+24};DrawTextW(dc,stats,-1,&sr,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);return;
    }
    int bodyTop=headerEnd+5;bool twoCol=EVAL_W>=620;int gap=16,leftX=EVAL_X,rightX=EVAL_X,leftW=EVAL_W,rightW=EVAL_W;if(twoCol){leftW=(EVAL_W-gap)/2;rightX=EVAL_X+leftW+gap;rightW=EVAL_W-leftW-gap;}
    int savedX=EVAL_X,savedW=EVAL_W;EVAL_X=leftX;EVAL_W=leftW;int cardY=bodyTop,resultCardH=compact?68:76;DWORD cardFill=g_currentResult==RESULT_WIN?RGBc(231,244,226):g_currentResult==RESULT_LOSS?RGBc(250,232,232):RGBc(239,242,244);DWORD cardEdge=g_currentResult==RESULT_WIN?RGBc(115,149,82):g_currentResult==RESULT_LOSS?RGBc(190,95,95):RGBc(175,184,190);HBRUSH cb=CreateSolidBrush(cardFill);HPEN cp=CreatePen(PS_SOLID,1,cardEdge);HGDIOBJ cob=SelectObject(dc,cb),cop=SelectObject(dc,cp);RoundRect(dc,EVAL_X,cardY,EVAL_X+EVAL_W,cardY+resultCardH,10,10);SelectObject(dc,cop);SelectObject(dc,cob);DeleteObject(cp);DeleteObject(cb);const wchar_t* title=g_currentResult==RESULT_WIN?L"VICTORY":g_currentResult==RESULT_LOSS?L"DEFEAT":g_currentResult==RESULT_DRAW?L"DRAW":L"ABORTED";SelectObject(dc,g_headingFont);SetTextColor(dc,g_currentResult==RESULT_LOSS?RGBc(150,62,62):RGBc(45,96,42));TextOutW(dc,EVAL_X+12,cardY+9,title,wLen(title));wchar_t detail[160]={0};wAppend(detail,160,reasonLabel(g_currentReason));if(g_game.histCount>0){wAppend(detail,160,L"  |  Final move ");wAppendAscii(detail,160,g_game.hist[g_game.histCount-1].san);}SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(70,80,86));RECT dr={EVAL_X+12,cardY+32,EVAL_X+EVAL_W-12,cardY+resultCardH-7};DrawTextW(dc,detail,-1,&dr,DT_LEFT|DT_WORDBREAK);
    int noteH=(g_ratingBeforeGame>0&&g_opponentRating>0)?36:0;if(noteH){wchar_t note[150]={0};wAppend(note,150,L"Trainer rating estimate: ");wAppendInt(note,150,g_ratingBeforeGame);wAppend(note,150,L" -> ");wAppendInt(note,150,g_lastRatingAfter);wAppend(note,150,L". Click Rating above to enter the actual Chess.com value.");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));RECT nr={EVAL_X,cardY+resultCardH+5,EVAL_X+EVAL_W,cardY+resultCardH+5+noteH};DrawTextW(dc,note,-1,&nr,DT_LEFT|DT_WORDBREAK);}
    int reviewY=cardY+resultCardH+noteH+10;if(twoCol)drawReviewSummary(dc,reviewY,bottom-4);else{int metricsH=trainingMetricsHeight(compact);int reviewBottom=bottom-metricsH-54;drawReviewSummary(dc,reviewY,reviewBottom);}
    EVAL_X=twoCol?rightX:savedX;EVAL_W=twoCol?rightW:savedW;int metricsY=twoCol?bodyTop:bottom-trainingMetricsHeight(compact)-30;if(g_userSet)drawTrainingMetrics(dc,metricsY,compact);int attempts,wins,draws,losses,aborted;computeCareerStats(&attempts,&wins,&draws,&losses,&aborted);int completed=wins+draws+losses,winRate=completed?wins*100/completed:0;wchar_t career[180]={0};wAppend(career,180,L"Career: ");wAppendInt(career,180,attempts);wAppend(career,180,L" games  |  ");wAppendInt(career,180,wins);wAppend(career,180,L"W ");wAppendInt(career,180,draws);wAppend(career,180,L"D ");wAppendInt(career,180,losses);wAppend(career,180,L"L  |  Win ");wAppendInt(career,180,winRate);wAppend(career,180,L"%");SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(91,99,105));int cy=metricsY+trainingMetricsHeight(compact)+7;RECT cr={EVAL_X,cy,EVAL_X+EVAL_W,cy+24};DrawTextW(dc,career,-1,&cr,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);EVAL_X=savedX;EVAL_W=savedW;
}

static bool handlePanelClick(int x,int y){
    RECT ta,tb,tc;panelTabRects(&ta,&tb,&tc);if(y>=ta.top&&y<=ta.bottom&&x>=EVAL_X&&x<=EVAL_X+EVAL_W){int mode=pointInRectI(x,y,ta.left,ta.top,ta.right,ta.bottom)?0:pointInRectI(x,y,tb.left,tb.top,tb.right,tb.bottom)?1:pointInRectI(x,y,tc.left,tc.top,tc.right,tc.bottom)?2:-1;if(mode>=0){if(mode!=g_panelMode){g_panelMode=mode;if(mode==2){g_historyDetail=false;g_historySelected=-1;}layoutUI();updateControls();RECT r={PANEL_X,PANEL_TOP,PANEL_X+PANEL_W,PANEL_BOTTOM};InvalidateRect(g_mainHwnd,&r,FALSE);}return true;}}
    if(g_panelMode==2){if(g_historyDetail){RECT back={EVAL_X,INSIGHTS_TOP,EVAL_X+108,INSIGHTS_TOP+28};if(pointInRectI(x,y,back.left,back.top,back.right,back.bottom)){g_historyDetail=false;g_historySelected=-1;invalidatePanelContent();return true;}}else{if(!g_archiveLoaded)loadArchive();if(!g_profileGameMapLoaded)loadProfileGameMap();int show=iMin(activeArchiveCount(),7);for(int v=0;v<show;v++){RECT r;historyRowRect(v,&r);if(pointInRectI(x,y,r.left,r.top,r.right,r.bottom)){g_historySelected=recentActiveArchiveIndex(v);if(g_historySelected>=0){g_historyDetail=true;invalidatePanelContent();}return true;}}}return true;}
    if(g_panelMode==1&&g_trainerMode!=MODE_PRACTICE&&g_trainerMode!=MODE_COACH){RECT rate;ratingRowRect(&rate);if(pointInRectI(x,y,rate.left,rate.top,rate.right,rate.bottom)){showRatingDialog();return true;}}
    return false;
}

static void drawModernChrome(HDC dc){
    RECT client;GetClientRect(g_mainHwnd,&client);
    HBRUSH head=CreateSolidBrush(RGBc(26,31,36));RECT hr={0,0,client.right,HEADER_H};FillRect(dc,&hr,head);DeleteObject(head);
    SetBkMode(dc,TRANSPARENT);
    // Header is intentionally clean: one title only. Branding is now in the footer.
    SelectObject(dc,g_titleFont);SetTextColor(dc,RGBc(250,251,252));RECT titleRc={MARGIN,0,MARGIN+185,HEADER_H};DrawTextW(dc,(LPWSTR)L"Chess Trainer",-1,&titleRc,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

    // Board shadow and crisp 1 px outline.
    HBRUSH shadow=CreateSolidBrush(RGBc(202,208,213));RECT sh={BOARD_X+5,BOARD_Y+6,BOARD_X+BOARD_SIZE+8,BOARD_Y+BOARD_SIZE+9};FillRect(dc,&sh,shadow);DeleteObject(shadow);

    // Compact side card. Its width is deliberately capped, so it can never consume a wide monitor.
    HBRUSH card=CreateSolidBrush(RGBc(248,249,250));HPEN border=CreatePen(PS_SOLID,1,RGBc(218,223,227));HGDIOBJ ob=SelectObject(dc,card),op=SelectObject(dc,border);RoundRect(dc,PANEL_X,PANEL_TOP,PANEL_X+PANEL_W,PANEL_BOTTOM,16,16);SelectObject(dc,op);SelectObject(dc,ob);DeleteObject(border);DeleteObject(card);

    drawPanelTabs(dc);
    if(g_panelMode==0){
        // Live position balance is useful in every training mode. Fair Play alone keeps
        // it disabled during a live human-game recording.
        bool showLive=(g_trainerMode!=MODE_FAIRPLAY)||g_gameOver;
        RECT outer={EVAL_X,EVAL_Y,EVAL_X+EVAL_W,EVAL_Y+EVAL_H};HBRUSH frame=CreateSolidBrush(RGBc(218,223,227));FillRect(dc,&outer,frame);DeleteObject(frame);
        if(!showLive){
            RECT inner={EVAL_X+2,EVAL_Y+2,EVAL_X+EVAL_W-2,EVAL_Y+EVAL_H-2};HBRUSH off=CreateSolidBrush(RGBc(224,229,232));FillRect(dc,&inner,off);DeleteObject(off);
            SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));const wchar_t* helper=L"Live engine analysis disabled.\nPost-game review only.";RECT msg={EVAL_X+4,EVAL_Y+EVAL_H+6,EVAL_X+EVAL_W-4,EVAL_Y+EVAL_H+54};DrawTextW(dc,(LPWSTR)helper,-1,&msg,DT_CENTER|DT_WORDBREAK|DT_VCENTER);
        }else{
            RECT inner={EVAL_X+2,EVAL_Y+2,EVAL_X+EVAL_W-2,EVAL_Y+EVAL_H-2};int iw=EVAL_W-4;
            int ww=g_evalValid?iw*g_whiteWin/1000:iw/3;int dw=g_evalValid?iw*g_drawChance/1000:iw/3;if(ww<0)ww=0;if(dw<0)dw=0;if(ww+dw>iw)dw=iw-ww;
            RECT wr={inner.left,inner.top,inner.left+ww,inner.bottom};RECT dr={wr.right,inner.top,wr.right+dw,inner.bottom};RECT brc={dr.right,inner.top,inner.right,inner.bottom};
            HBRUSH wb=CreateSolidBrush(RGBc(239,240,224)), db=CreateSolidBrush(RGBc(160,168,175)), bb=CreateSolidBrush(RGBc(43,48,54));FillRect(dc,&wr,wb);if(dw>0)FillRect(dc,&dr,db);FillRect(dc,&brc,bb);DeleteObject(wb);DeleteObject(db);DeleteObject(bb);
            int third=EVAL_W/3;int vals[3]={g_evalValid?(g_whiteWin+5)/10:0,g_evalValid?(g_drawChance+5)/10:0,g_evalValid?(g_blackWin+5)/10:0};const wchar_t* caps[3]={L"White",L"Draw",L"Black"};
            int pctTop=EVAL_Y+EVAL_H+8,pctBottom=pctTop+28,labelTop=pctBottom-1,labelBottom=labelTop+18;
            for(int i=0;i<3;i++){RECT pr={EVAL_X+i*third,pctTop,(i==2?EVAL_X+EVAL_W:EVAL_X+(i+1)*third),pctBottom};wchar_t pct[16]={0};wAppendInt(pct,16,vals[i]);wAppend(pct,16,L"%");SelectObject(dc,g_percentFont);SetTextColor(dc,RGBc(45,52,58));DrawTextW(dc,pct,-1,&pr,DT_CENTER|DT_SINGLELINE|DT_VCENTER);RECT cr={pr.left,labelTop,pr.right,labelBottom};SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));DrawTextW(dc,(LPWSTR)caps[i],-1,&cr,DT_CENTER|DT_SINGLELINE|DT_VCENTER);}
            if(g_evalValid&&g_lastMate!=0){wchar_t mate[64]={0};bool mateSide=g_lastMate>0?g_game.pos.whiteToMove:!g_game.pos.whiteToMove;wAppend(mate,64,mateSide?L"White mate in ":L"Black mate in ");wAppendInt(mate,64,iAbs(g_lastMate));SelectObject(dc,g_actionButtonFont);SetTextColor(dc,RGBc(190,52,52));RECT mr={EVAL_X,EVAL_Y+EVAL_H+51,EVAL_X+EVAL_W,EVAL_Y+EVAL_H+73};DrawTextW(dc,mate,-1,&mr,DT_CENTER|DT_SINGLELINE|DT_VCENTER);}
        }
        drawSectionHeader(dc,2,GAMELOG_HEAD_Y,L"Game Log",8);drawGameLog(dc);
    }else if(g_panelMode==1)drawGameInsights(dc);else drawHistoryPanel(dc);
    if(!(g_panelMode==1&&g_gameOver)){drawSectionHeader(dc,3,ENTER_HEAD_Y,L"Enter Move",10);
        // Helper is directly below the SAN field and aligned with the field's left edge.
        SelectObject(dc,g_smallFont);SetTextColor(dc,RGBc(108,117,125));TextOutW(dc,EVAL_X,ENTER_HEAD_Y+82,L"SAN examples: e4, Nf3, O-O, exd8=Q",34);
    }

    // Persistent footer, separated from the centred game-status line above it.
    int fy=client.bottom-FOOTER_H;
    HBRUSH fb=CreateSolidBrush(RGBc(244,246,248));RECT fr={0,fy,client.right,client.bottom};FillRect(dc,&fr,fb);DeleteObject(fb);
    HPEN fp=CreatePen(PS_SOLID,1,RGBc(218,223,227));HGDIOBJ fop=SelectObject(dc,fp);MoveToEx(dc,0,fy,NULLPTR);LineTo(dc,client.right,fy);SelectObject(dc,fop);DeleteObject(fp);
    SelectObject(dc,g_footerFont);SetTextColor(dc,RGBc(108,117,125));
    RECT frr={client.right/2,fy,client.right-MARGIN-48,client.bottom};DrawTextW(dc,(LPWSTR)L"Designed and Developed by Tajud Din | V6.0.0",-1,&frr,DT_RIGHT|DT_SINGLELINE|DT_VCENTER);
}
static void drawBoard(HDC dc){
    SetBkMode(dc,TRANSPARENT);for(int row=0;row<8;row++)for(int col=0;col<8;col++){RECT rc={BOARD_X+col*SQ,BOARD_Y+row*SQ,BOARD_X+(col+1)*SQ,BOARD_Y+(row+1)*SQ};int sq=displayToSquare(col,row);int f=fileOf(sq),r=rankOf(sq);bool light=((f+r)&1)==1;FillRect(dc,&rc,light?g_lightBrush:g_darkBrush);
        // Coordinates stay sharp because every square is an exact integer size.
        SelectObject(dc,g_coordFont);DWORD tc=light?RGBc(88,119,63):RGBc(235,236,208);SetTextColor(dc,tc);if(col==0){wchar_t t[2]={(wchar_t)('1'+r),0};TextOutW(dc,rc.left+4,rc.top+3,t,1);}if(row==7){wchar_t t[2]={(wchar_t)('a'+f),0};TextOutW(dc,rc.right-14,rc.bottom-18,t,1);}
    }
    HPEN boardPen=CreatePen(PS_SOLID,1,RGBc(188,196,201));HGDIOBJ oldBoardPen=SelectObject(dc,boardPen);HGDIOBJ nullBoardBrush=GetStockObject(NULL_BRUSH);HGDIOBJ oldBoardBrush=SelectObject(dc,nullBoardBrush);Rectangle(dc,BOARD_X,BOARD_Y,BOARD_X+BOARD_SIZE,BOARD_Y+BOARD_SIZE);SelectObject(dc,oldBoardBrush);SelectObject(dc,oldBoardPen);DeleteObject(boardPen);
    // Pieces remain vector/font glyphs at every size. White pieces are rendered in
    // two passes: a solid white silhouette underneath, then the dark white-piece
    // outline glyph. This closely matches the high-contrast Chess.com appearance.
    SelectObject(dc,g_pieceFont);
    for(int sq=0;sq<64;sq++){
        char p=g_game.pos.sq[sq];if(!p)continue;int c,r;squareToDisplay(sq,c,r);
        RECT pr={BOARD_X+c*SQ,BOARD_Y+r*SQ,BOARD_X+(c+1)*SQ,BOARD_Y+(r+1)*SQ};
        if(isWhitePiece(p)){
            wchar_t under[2]={filledGlyphForWhite(p),0};
            SetTextColor(dc,RGBc(250,250,247));
            DrawTextW(dc,under,1,&pr,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            wchar_t top[2]={pieceGlyph(p),0};
            SetTextColor(dc,RGBc(30,33,36));
            DrawTextW(dc,top,1,&pr,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }else{
            wchar_t t[2]={pieceGlyph(p),0};
            SetTextColor(dc,RGBc(20,23,25));
            DrawTextW(dc,t,1,&pr,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }
    }
    // Last move circles scale with the board.
    HGDIOBJ nullBr=GetStockObject(NULL_BRUSH);HGDIOBJ oldBr=SelectObject(dc,nullBr);if(g_game.histCount>0){int rad=iMax(18,SQ*30/100);HPEN yp=CreatePen(PS_SOLID,iMax(3,SQ/20),RGBc(255,227,82));HGDIOBJ op=SelectObject(dc,yp);for(int k=0;k<2;k++){int sq=k?g_game.hist[g_game.histCount-1].move.to:g_game.hist[g_game.histCount-1].move.from;int x,y;squareCenter(sq,x,y);Ellipse(dc,x-rad,y-rad,x+rad,y+rad);}SelectObject(dc,op);DeleteObject(yp);}
    if(g_selected>=0){HPEN cp=CreatePen(PS_SOLID,iMax(3,SQ/20),RGBc(0,188,212));HGDIOBJ op=SelectObject(dc,cp);int c,r;squareToDisplay(g_selected,c,r);int pad=iMax(3,SQ/25);Rectangle(dc,BOARD_X+c*SQ+pad,BOARD_Y+r*SQ+pad,BOARD_X+(c+1)*SQ-pad,BOARD_Y+(r+1)*SQ-pad);SelectObject(dc,op);DeleteObject(cp);}SelectObject(dc,oldBr);
    if(g_suggestionPending)drawArrow(dc,g_suggested.from,g_suggested.to);
    drawMoveExplanationOverlay(dc);
}

static LRESULT CALLBACK modernButtonProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    if(msg==WM_ERASEBKGND)return 1;
    // A button can change width when the main window is resized. Windows may otherwise
    // invalidate only the newly exposed strip, leaving the old rounded right edge visible
    // inside the enlarged control. Mark the whole client area dirty on every size change
    // so each button is always repainted as one continuous shape.
    if(msg==WM_SIZE){InvalidateRect(hwnd,NULLPTR,FALSE);return 0;}
    if(msg==WM_MOUSEMOVE){if(g_hoverButton!=hwnd){HWND old=g_hoverButton;g_hoverButton=hwnd;if(old)InvalidateRect(old,NULLPTR,FALSE);InvalidateRect(hwnd,NULLPTR,FALSE);}TRACKMOUSEEVENT t;t.cbSize=sizeof(t);t.dwFlags=TME_LEAVE;t.hwndTrack=hwnd;t.dwHoverTime=0;TrackMouseEvent(&t);return 0;}
    if(msg==WM_MOUSELEAVE){if(g_hoverButton==hwnd){g_hoverButton=NULLPTR;InvalidateRect(hwnd,NULLPTR,FALSE);}return 0;}
    if(msg==WM_ENABLE){InvalidateRect(hwnd,NULLPTR,FALSE);return 0;}
    if(msg==WM_PAINT){
        PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);
        int cw=rc.right-rc.left,ch=rc.bottom-rc.top;
        if(cw<=0||ch<=0){EndPaint(hwnd,&ps);return 0;}

        bool enabled=IsWindowEnabled(hwnd)!=0;bool hover=enabled&&g_hoverButton==hwnd;int id=GetDlgCtrlID(hwnd);
        DWORD fill=hover?RGBc(239,244,246):RGBc(247,249,250),edge=hover?RGBc(168,180,188):RGBc(205,212,218),text=RGBc(43,50,56);
        int borderWidth=1;

        // Preserve the existing colour/state logic exactly. Only the rasterisation
        // method below has changed.
        if(id==ID_WHITE||id==ID_BLACK){
            bool isWhite=(id==ID_WHITE);bool selected=g_userSet&&((isWhite&&g_userWhite)||(!isWhite&&!g_userWhite));
            fill=isWhite?RGBc(255,255,255):RGBc(20,22,24);
            text=isWhite?RGBc(20,22,24):RGBc(255,255,255);
            edge=selected?RGBc(255,193,7):(hover?RGBc(64,145,255):RGBc(145,153,160));
            borderWidth=selected?3:1;
        }else if(id==ID_MODE_FULL||id==ID_MODE_ASSISTED||id==ID_MODE_PRACTICE||id==ID_MODE_FAIRPLAY||id==ID_MODE_COACH){
            int mode=id==ID_MODE_ASSISTED?MODE_ASSISTED:id==ID_MODE_PRACTICE?MODE_PRACTICE:id==ID_MODE_FAIRPLAY?MODE_FAIRPLAY:id==ID_MODE_COACH?MODE_COACH:MODE_FULL;bool selected=mode==g_gameBaseMode;
            fill=selected?RGBc(72,106,58):(hover?RGBc(52,61,67):RGBc(38,44,49));edge=selected?RGBc(151,190,111):RGBc(93,104,112);text=RGBc(248,250,251);borderWidth=selected?2:1;
        }else if(id==ID_PROFILE){
            fill=hover?RGBc(52,61,67):RGBc(38,44,49);edge=RGBc(115,149,82);text=RGBc(248,250,251);borderWidth=1;
        }else if(id==ID_ASSIST_FULL_ON||id==ID_ASSIST_FULL_OFF){
            bool onButton=(id==ID_ASSIST_FULL_ON);bool selected=onButton?g_assistFull:!g_assistFull;
            if(onButton&&selected){fill=hover?RGBc(50,150,80):RGBc(40,135,70);edge=RGBc(28,103,52);text=RGBc(255,255,255);borderWidth=2;}
            else if(!onButton&&selected){fill=RGBc(82,91,98);edge=RGBc(62,70,76);text=RGBc(255,255,255);borderWidth=2;}
            else{fill=hover?RGBc(239,244,246):RGBc(247,249,250);edge=RGBc(190,199,205);text=RGBc(70,78,84);}
        }else if(id==ID_CONFIRM||id==ID_DIFFERENT){
            int phase=currentActionPhase();
            if(phase==1){fill=hover?RGBc(50,150,80):RGBc(40,135,70);edge=RGBc(28,103,52);text=RGBc(255,255,255);}
            else if(phase==2){fill=RGBc(204,69,72);edge=RGBc(153,45,48);text=RGBc(255,255,255);}
            else{fill=RGBc(226,230,233);edge=RGBc(199,205,210);text=RGBc(125,133,139);}
        }else if(!enabled){fill=RGBc(239,241,242);edge=RGBc(221,224,226);text=RGBc(157,164,169);}

        // Exact parent colour is used around the rounded shape. This is essential:
        // anti-aliased corner pixels are blended into the same colour as the real
        // parent surface, so no light/dark fringe can bleed from the child DC.
        bool headerButton=(id==ID_WHITE||id==ID_BLACK||id==ID_PROFILE||id==ID_MODE_FULL||id==ID_MODE_ASSISTED||id==ID_MODE_PRACTICE||id==ID_MODE_FAIRPLAY||id==ID_MODE_COACH);DWORD backdrop=headerButton?RGBc(26,31,36):(id==ID_LOG?RGBc(244,246,248):RGBc(248,249,250));

        // Final-size double buffer. Text is drawn here after downsampling so ClearType
        // remains crisp and is not softened by the supersampling pass.
        HDC finalDC=CreateCompatibleDC(dc);HBITMAP finalBmp=NULLPTR;HGDIOBJ oldFinal=NULLPTR;
        if(finalDC)finalBmp=CreateCompatibleBitmap(dc,cw,ch);
        if(!finalDC||!finalBmp){if(finalBmp)DeleteObject(finalBmp);if(finalDC)DeleteDC(finalDC);EndPaint(hwnd,&ps);return 0;}
        oldFinal=SelectObject(finalDC,finalBmp);
        HBRUSH back=CreateSolidBrush(backdrop);FillRect(finalDC,&rc,back);DeleteObject(back);

        // GDI RoundRect has no true anti-aliasing. Draw the button four times larger,
        // then shrink it with HALFTONE. Border and fill are both solid rounded shapes,
        // not stroked pens, so the bottom/right edge can never be clipped by a pen
        // centred on the child-window boundary.
        const int SS=4;
        const int highW=cw*SS,highH=ch*SS;
        HDC highDC=CreateCompatibleDC(dc);HBITMAP highBmp=NULLPTR;HGDIOBJ oldHigh=NULLPTR;
        if(highDC)highBmp=CreateCompatibleBitmap(dc,highW,highH);
        if(highDC&&highBmp){
            oldHigh=SelectObject(highDC,highBmp);
            RECT highRc={0,0,highW,highH};HBRUSH hb=CreateSolidBrush(backdrop);FillRect(highDC,&highRc,hb);DeleteObject(hb);

            const int outerMargin=SS;                 // one final pixel of clean backdrop
            const int outerRadius=10*SS;
            HBRUSH edgeBrush=CreateSolidBrush(edge);HGDIOBJ oldBrush=SelectObject(highDC,edgeBrush);HGDIOBJ oldPen=SelectObject(highDC,GetStockObject(NULL_PEN));
            RoundRect(highDC,outerMargin,outerMargin,highW-outerMargin,highH-outerMargin,outerRadius,outerRadius);
            SelectObject(highDC,oldPen);SelectObject(highDC,oldBrush);DeleteObject(edgeBrush);

            const int innerInset=outerMargin+borderWidth*SS;
            int innerRadius=outerRadius-borderWidth*SS;if(innerRadius<2*SS)innerRadius=2*SS;
            HBRUSH fillBrush=CreateSolidBrush(fill);oldBrush=SelectObject(highDC,fillBrush);oldPen=SelectObject(highDC,GetStockObject(NULL_PEN));
            RoundRect(highDC,innerInset,innerInset,highW-innerInset,highH-innerInset,innerRadius,innerRadius);
            SelectObject(highDC,oldPen);SelectObject(highDC,oldBrush);DeleteObject(fillBrush);

            SetStretchBltMode(finalDC,HALFTONE_MODE);
            StretchBlt(finalDC,0,0,cw,ch,highDC,0,0,highW,highH,SRCCOPY);
            SelectObject(highDC,oldHigh);DeleteObject(highBmp);DeleteDC(highDC);
        }else{
            // Allocation fallback. Still keep the shapes one pixel away from all
            // child-window edges so the old bottom/right clipping artefact cannot recur.
            if(highBmp)DeleteObject(highBmp);if(highDC)DeleteDC(highDC);
            HBRUSH edgeBrush=CreateSolidBrush(edge);HGDIOBJ oldBrush=SelectObject(finalDC,edgeBrush);HGDIOBJ oldPen=SelectObject(finalDC,GetStockObject(NULL_PEN));
            RoundRect(finalDC,1,1,cw-1,ch-1,10,10);SelectObject(finalDC,oldPen);SelectObject(finalDC,oldBrush);DeleteObject(edgeBrush);
            int ii=1+borderWidth;HBRUSH fillBrush=CreateSolidBrush(fill);oldBrush=SelectObject(finalDC,fillBrush);oldPen=SelectObject(finalDC,GetStockObject(NULL_PEN));
            RoundRect(finalDC,ii,ii,cw-ii,ch-ii,8,8);SelectObject(finalDC,oldPen);SelectObject(finalDC,oldBrush);DeleteObject(fillBrush);
        }

        wchar_t label[128];GetWindowTextW(hwnd,label,128);SetBkMode(finalDC,TRANSPARENT);SetTextColor(finalDC,text);
        SelectObject(finalDC,id==ID_LOG?g_footerFont:((id==ID_CONFIRM||id==ID_DIFFERENT)?g_actionButtonFont:g_buttonFont));
        RECT tr={8,0,cw-8,ch};DrawTextW(finalDC,label,-1,&tr,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

        BitBlt(dc,0,0,cw,ch,finalDC,0,0,SRCCOPY);
        SelectObject(finalDC,oldFinal);DeleteObject(finalBmp);DeleteDC(finalDC);
        EndPaint(hwnd,&ps);return 0;
    }
    if(msg==WM_LBUTTONUP){if(IsWindowEnabled(hwnd)){int id=GetDlgCtrlID(hwnd);HWND target=g_mainHwnd;if((id==3003||id==3004)&&g_ratingHwnd)target=g_ratingHwnd;else if((id==3102||id==3103)&&g_practiceHwnd)target=g_practiceHwnd;else if(id>=3201&&id<=3203&&g_choiceHwnd)target=g_choiceHwnd;else if(id>=3301&&id<=3302&&g_coachAdviceHwnd)target=g_coachAdviceHwnd;else if(id>=3401&&id<=3405&&g_profileHwnd)target=g_profileHwnd;PostMessageW(target,WM_COMMAND,(WPARAM)id,(LPARAM)hwnd);}return 0;}
    if(msg==WM_KEYDOWN && (w==VK_RETURN_KEY||w==VK_SPACE_KEY)){if(IsWindowEnabled(hwnd)){int id=GetDlgCtrlID(hwnd);HWND target=g_mainHwnd;if((id==3003||id==3004)&&g_ratingHwnd)target=g_ratingHwnd;else if((id==3102||id==3103)&&g_practiceHwnd)target=g_practiceHwnd;else if(id>=3201&&id<=3203&&g_choiceHwnd)target=g_choiceHwnd;else if(id>=3301&&id<=3302&&g_coachAdviceHwnd)target=g_coachAdviceHwnd;else if(id>=3401&&id<=3405&&g_profileHwnd)target=g_profileHwnd;PostMessageW(target,WM_COMMAND,(WPARAM)id,(LPARAM)hwnd);}return 0;}
    return DefWindowProcW(hwnd,msg,w,l);
}
static void registerModernButtonClass(){static bool done=false;if(done)return;WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=0;wc.lpfnWndProc=modernButtonProc;wc.hInstance=g_hInstance;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=GetSysColorBrush(COLOR_WINDOW);wc.lpszClassName=L"ChessTrainerModernButton";RegisterClassExW(&wc);done=true;}
static HWND createButton(LPCWSTR text,int id){HWND z=CreateWindowExW(0,L"ChessTrainerModernButton",text,WS_CHILD|WS_TABSTOP,0,0,100,34,g_mainHwnd,(HMENU)(ULONG_PTR)id,g_hInstance,NULLPTR);return z;}

static HWND createControl(LPCWSTR cls,LPCWSTR text,DWORD style,int x,int y,int w,int h,int id){HWND z=CreateWindowExW(0,cls,text,WS_CHILD|style,x,y,w,h,g_mainHwnd,(HMENU)(ULONG_PTR)id,g_hInstance,NULLPTR);SendMessageW(z,WM_SETFONT,(WPARAM)g_uiFont,1);return z;}

static void rebuildPieceFont(){
    if(g_pieceFont){DeleteObject(g_pieceFont);g_pieceFont=NULLPTR;}
    int ph=-(SQ*78/100);if(ph>-34)ph=-34;if(ph<-160)ph=-160;
    g_pieceFont=CreateFontW(ph,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI Symbol");
}
static int clampInt(int v,int lo,int hi){if(v<lo)return lo;if(v>hi)return hi;return v;}

// Move custom buttons without asking Windows to repaint only the exposed resize strip.
// We explicitly invalidate the full child afterwards. Combined with the double-buffered
// button painter this removes the extra trailing edge / duplicate segment seen after resize.
static void moveModernButton(HWND h,int x,int y,int w,int hgt){
    if(!h)return;
    MoveWindow(h,x,y,w,hgt,FALSE);
    InvalidateRect(h,NULLPTR,FALSE);
}

static void invalidateModernButtons(){
    HWND buttons[]={hWhite,hBlack,hProfile,hModeFull,hModeAssisted,hModePractice,hModeFair,hModeCoach,hEnter,hNew,hFlip,hUndo,hCorrect,hConfirm,hDifferent,hLog,hCopyPanel};
    for(int i=0;i<17;i++)if(buttons[i])InvalidateRect(buttons[i],NULLPTR,FALSE);for(int i=0;i<8;i++)if(hEndGame[i])InvalidateRect(hEndGame[i],NULLPTR,FALSE);
}

static void layoutUI(){
    if(!g_mainHwnd)return;RECT rc;GetClientRect(g_mainHwnd,&rc);CLIENT_W=rc.right;CLIENT_H=rc.bottom;

    // Responsive full-window layout. The chessboard stays perfectly square and pixel-aligned.
    // V5.6.5 also bounds the control panel on wide/ultrawide displays. Once the board is
    // height-limited, surplus horizontal space becomes balanced outer margin instead of
    // stretching Game Log / Game Insights to an impractical width.
    int headingGap=iMax(6,HEADER_H/10);
    int contentTop=HEADER_H+headingGap;
    int maxBoardH=CLIENT_H-contentTop-STATUS_H-FOOTER_H-MARGIN;
    int minPanel=340;int maxBoardW=CLIENT_W-minPanel-CONTENT_GAP-MARGIN*2;
    int bs=maxBoardW<maxBoardH?maxBoardW:maxBoardH;if(bs<360)bs=360;bs=(bs/8)*8;if(bs>1600)bs=1600;
    int oldSq=SQ;BOARD_SIZE=bs;SQ=BOARD_SIZE/8;

    // Keep the panel proportional to the board, but never wider than 820 logical pixels.
    // On ordinary/narrow windows there is no artificial whitespace: the panel simply uses
    // whatever width remains after the board. On ultrawide windows the useful workspace is
    // centred, preserving a balanced board/control relationship at 21:9 and wider ratios.
    int usableW=CLIENT_W-MARGIN*2;
    int remainingPanel=usableW-BOARD_SIZE-CONTENT_GAP;if(remainingPanel<minPanel)remainingPanel=minPanel;
    int proportionalPanel=BOARD_SIZE*74/100;if(proportionalPanel<480)proportionalPanel=480;if(proportionalPanel>820)proportionalPanel=820;
    PANEL_W=iMin(remainingPanel,proportionalPanel);if(PANEL_W<minPanel)PANEL_W=minPanel;
    int workspaceW=BOARD_SIZE+CONTENT_GAP+PANEL_W;
    BOARD_X=(workspaceW<usableW)?(CLIENT_W-workspaceW)/2:MARGIN;
    if(BOARD_X<MARGIN)BOARD_X=MARGIN;
    BOARD_Y=contentTop;PANEL_X=BOARD_X+BOARD_SIZE+CONTENT_GAP;
    PANEL_TOP=BOARD_Y;PANEL_BOTTOM=BOARD_Y+BOARD_SIZE;
    EVAL_X=PANEL_X+18;EVAL_W=PANEL_W-36;ANALYSIS_HEAD_Y=PANEL_TOP+14;EVAL_Y=PANEL_TOP+60;EVAL_H=26;
    GAMELOG_HEAD_Y=EVAL_Y+EVAL_H+((g_trainerMode==MODE_ASSISTED||g_assistFull)?122:g_trainerMode==MODE_COACH?88:64);HISTORY_Y=GAMELOG_HEAD_Y+36;

    // Reserve a compact, predictable block for move entry and six action buttons.
    const int bottomBlock=258;ENTER_HEAD_Y=PANEL_BOTTOM-bottomBlock;if(ENTER_HEAD_Y<HISTORY_Y+130)ENTER_HEAD_Y=HISTORY_Y+130;
    HISTORY_H=ENTER_HEAD_Y-HISTORY_Y-14;if(HISTORY_H<110)HISTORY_H=110;INSIGHTS_TOP=PANEL_TOP+60;INSIGHTS_BOTTOM=(g_panelMode==1&&g_gameOver)?PANEL_BOTTOM-58:ENTER_HEAD_Y-12;
    if(SQ!=oldSq)rebuildPieceFont();

    // Side selector and operating-mode controls stay in the header.
    int sideW=86,gap=8,top=13;int sideStart=CLIENT_W-MARGIN-sideW*2-gap;moveModernButton(hWhite,sideStart,top,sideW,32);moveModernButton(hBlack,CLIENT_W-MARGIN-sideW,top,sideW,32);
    int mg=4,mw1=92,mwA=116,mw2=120,mw3=82,mw4=100,total=mw1+mwA+mw2+mw3+mw4+mg*4;int mx=sideStart-14-total;int profileX=MARGIN+190,profileW=mx-profileX-10;if(profileW>180)profileW=180;if(profileW<76)profileW=76;moveModernButton(hProfile,profileX,top,profileW,32);moveModernButton(hModeFull,mx,top,mw1,32);moveModernButton(hModeAssisted,mx+mw1+mg,top,mwA,32);moveModernButton(hModePractice,mx+mw1+mg+mwA+mg,top,mw2,32);moveModernButton(hModeFair,mx+mw1+mg+mwA+mg+mw2+mg,top,mw3,32);moveModernButton(hModeCoach,mx+mw1+mg+mwA+mg+mw2+mg+mw3+mg,top,mw4,32);
    int assistY=EVAL_Y+EVAL_H+82,assistW=(EVAL_W-8)/2;moveModernButton(hAssistFullOn,EVAL_X,assistY,assistW,30);moveModernButton(hAssistFullOff,EVAL_X+assistW+8,assistY,assistW,30);

    if(hHistory)MoveWindow(hHistory,EVAL_X,HISTORY_Y,EVAL_W,HISTORY_H,TRUE);for(int i=0;i<8;i++){RECT er;outcomeRect(i,&er);moveModernButton(hEndGame[i],er.left,er.top,er.right-er.left,er.bottom-er.top);}
    if(hCopyPanel){int cw=iMin(118,EVAL_W/3);int cy=(g_panelMode==0)?GAMELOG_HEAD_Y-2:INSIGHTS_TOP;int cx=EVAL_X+EVAL_W-cw;moveModernButton(hCopyPanel,cx,cy,cw,28);}
    int inputY=ENTER_HEAD_Y+36;int enterW=82;MoveWindow(hSan,EVAL_X,inputY,EVAL_W-enterW-8,40,TRUE);moveModernButton(hEnter,EVAL_X+EVAL_W-enterW,inputY,enterW,40);

    int bw=(EVAL_W-8)/2,by=ENTER_HEAD_Y+108;const int bh=38;bool completedInsights=(g_panelMode==1&&g_gameOver);
    int newY=completedInsights?PANEL_BOTTOM-44:by;moveModernButton(hNew,EVAL_X,newY,bw,bh);moveModernButton(hFlip,EVAL_X+bw+8,newY,bw,bh);
    by+=46;moveModernButton(hUndo,EVAL_X,by,bw,bh);moveModernButton(hCorrect,EVAL_X+bw+8,by,bw,bh);
    by+=46;moveModernButton(hConfirm,EVAL_X,by,bw,bh);moveModernButton(hDifferent,EVAL_X+bw+8,by,bw,bh);

    MoveWindow(hStatus,MARGIN,CLIENT_H-FOOTER_H-STATUS_H,CLIENT_W-MARGIN*2,STATUS_H,TRUE);moveModernButton(hLog,CLIENT_W-MARGIN-40,CLIENT_H-FOOTER_H+4,40,FOOTER_H-8);
    invalidateAll();
    invalidateModernButtons();
}
static void buildUI(){
    // Win32 has no CSS font stack; Segoe UI is the native Windows equivalent of system-ui.
    g_uiFont=CreateFontW(-16,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_smallFont=CreateFontW(-13,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_headingFont=CreateFontW(-18,0,0,0,500,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_titleFont=CreateFontW(-27,0,0,0,600,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_statusFont=CreateFontW(-15,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_footerFont=CreateFontW(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_monoFont=CreateFontW(-15,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Consolas");
    g_monoBoldFont=CreateFontW(-15,0,0,0,700,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Consolas");
    g_percentFont=CreateFontW(-22,0,0,0,700,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_coordFont=CreateFontW(-14,0,0,0,700,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_buttonFont=CreateFontW(-14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    g_actionButtonFont=CreateFontW(-14,0,0,0,700,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    rebuildPieceFont();
    registerModernButtonClass();hWhite=createButton(L"White",ID_WHITE);hBlack=createButton(L"Black",ID_BLACK);hProfile=createButton(L"Player",ID_PROFILE);hModeFull=createButton(L"Full Trainer",ID_MODE_FULL);hModeAssisted=createButton(L"Assisted Mode",ID_MODE_ASSISTED);hModePractice=createButton(L"Human Practice",ID_MODE_PRACTICE);hModeFair=createButton(L"Fair Play",ID_MODE_FAIRPLAY);hModeCoach=createButton(L"Coach Mode",ID_MODE_COACH);hAssistFullOn=createButton(L"Full Assist ON",ID_ASSIST_FULL_ON);hAssistFullOff=createButton(L"Full Assist OFF",ID_ASSIST_FULL_OFF);hLog=createButton(L"Log",ID_LOG);hCopyPanel=createButton(L"Copy Analysis",ID_COPY_PANEL);const wchar_t* endTxt[8]={L"Resigned",L"Resigned",L"Abandoned",L"Abandoned",L"Timed out",L"Timed out",L"Draw",L"Cancel"};for(int i=0;i<8;i++)hEndGame[i]=createButton(endTxt[i],ID_END_OPP_RESIGN+i);
    // Game Log is now owner-drawn in the main surface; no stock EDIT child is used.
    hHistory=NULLPTR;
    hSan=createControl(L"EDIT",L"",WS_BORDER|WS_TABSTOP,0,0,100,40,ID_SAN);SendMessageW(hSan,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,(LPARAM)((10&0xFFFF)|((10&0xFFFF)<<16)));hEnter=createButton(L"Enter",ID_ENTER);
    hNew=createButton(L"New game",ID_NEW);hFlip=createButton(L"Flip board",ID_FLIP);
    hUndo=createButton(L"Undo last move",ID_UNDO);hCorrect=createButton(L"Correct my move",ID_CORRECT);
    hConfirm=createButton(L"Suggested Move",ID_CONFIRM);hDifferent=createButton(L"Different Move",ID_DIFFERENT);
    hStatus=createControl(L"STATIC",L"Starting local chess engine...",SS_CENTER|SS_CENTERIMAGE,0,0,100,STATUS_H,0);SendMessageW(hStatus,WM_SETFONT,(WPARAM)g_statusFont,1);
    g_lightBrush=CreateSolidBrush(RGBc(235,236,208));
    g_darkBrush=CreateSolidBrush(RGBc(115,149,82));
    g_bgBrush=CreateSolidBrush(RGBc(238,241,244));
    g_cardBrush=CreateSolidBrush(RGBc(248,249,250));
    layoutUI();
    // Children are created hidden, positioned once, then shown. This prevents the
    // first-frame ghost/double-outline artefact that disappeared only after resizing.
    HWND controls[]={hWhite,hBlack,hProfile,hModeFull,hModeAssisted,hModePractice,hModeFair,hModeCoach,hSan,hEnter,hNew,hFlip,hUndo,hCorrect,hConfirm,hDifferent,hStatus,hLog};
    for(int i=0;i<18;i++)if(controls[i])ShowWindow(controls[i],SW_SHOW);
    updateControls();
    for(int i=0;i<18;i++)if(controls[i]){InvalidateRect(controls[i],NULLPTR,FALSE);UpdateWindow(controls[i]);}
}
static void handleEngineMessage(int code,int token){
    diagEngineResult(code,token);
    if(code==ENG_READY){g_engineReady=true;debugAppendRaw("ENGINE","Stockfish worker ready.");if(!g_userSet)setStatus(g_trainerMode==MODE_ASSISTED?L"Assisted Mode: select your side to start.":g_trainerMode==MODE_PRACTICE?L"Human Practice: select your side to start.":g_trainerMode==MODE_FAIRPLAY?L"Fair Play / Record Only: select your side to start.":g_trainerMode==MODE_COACH?L"Coach Mode: select your side to start.":L"Full Trainer: select your side to start.");updateControls();return;}
    if(code==ENG_ERROR){g_engineReady=false;g_calculating=false;setStatus(L"Stockfish could not be started.");updateControls();MessageBoxW(g_mainHwnd,g_engineError,L"Stockfish error",MB_OK|MB_ICONERROR);return;}
    if(code==ENG_ANALYSIS_ERROR){if(token==g_positionVersion){debugAppendRaw("ENGINE-ERROR","Stockfish analysis request failed or returned unusable output.");g_calculating=false;g_suggestionPending=false;if(g_trainerMode==MODE_COACH&&g_coachPending){g_coachPending=false;setStatus(L"Coach analysis was unavailable. Try the move again or choose another move.");updateControls();}else if(g_trainerMode==MODE_ASSISTED&&g_choiceRequestPending){g_choiceRequestPending=false;g_onDemandSuggestion=false;setStatus(L"Assisted Mode could not produce the requested options. You can continue with your own move.");updateControls();}else if(g_trainerMode==MODE_ASSISTED&&g_assistPending){Move fallbackMove=g_assistPendingMove;g_assistPending=false;setStatus(L"Assisted Mode check was unavailable. Your real move has still been recorded.");commitRecordedMove(fallbackMove);}else if(g_trainerMode==MODE_ASSISTED&&g_threatCheckPending){g_threatCheckPending=false;setStatus(L"Threat scan was unavailable. Continuing Assisted Mode with the current guidance settings.");checkNeedResponse();}else{g_requestGuideKind=GUIDE_NONE;setStatus(L"Stockfish analysis failed.");updateControls();MessageBoxW(g_mainHwnd,g_engineError,L"Stockfish analysis error",MB_OK|MB_ICONERROR);}}return;}
    if(code==ENG_REVIEW_DONE){if(token!=g_positionVersion)return;computeReviewClasses();updateCurrentArchiveReview();setStatus(L"Game complete. Trainer Review is ready in Game Insights.");invalidatePanelContent();return;}
    if(code==ENG_EVAL){if(token!=g_positionVersion)return;storeLiveEvalForCurrentPosition();if(g_trainerMode!=MODE_FAIRPLAY)applyEngineEvaluation();return;}
    if(code==ENG_TAG){if(token!=g_positionVersion)return;storeLiveEvalForCurrentPosition();if(g_trainerMode!=MODE_FAIRPLAY)applyEngineEvaluation();g_calculating=false;checkNeedResponse();return;}
    if(code==ENG_THREAT){
        if(token!=g_positionVersion||g_trainerMode!=MODE_ASSISTED)return;g_threatCheckPending=false;storeLiveEvalForCurrentPosition();applyEngineEvaluation();g_calculating=false;
        bool userTurn=g_game.pos.whiteToMove==g_userWhite;bool checked=userTurn&&inCheck(&g_game.pos,g_userWhite);bool mateThreat=userTurn&&g_engineMate<0;int nowScore=mateThreat?-100000:g_engineScoreCp;int ply=g_game.histCount;int prevScore=nowScore;if(ply>0&&g_liveEvalKnown[ply-1])prevScore=g_userWhite?g_liveEvalWhite[ply-1]:-g_liveEvalWhite[ply-1];bool checkedDanger=userTurn&&!mateThreat&&checked&&nowScore<=-180;bool suddenCollapse=userTurn&&!mateThreat&&nowScore<=-650&&prevScore-nowScore>=250&&prevScore>-950;bool newEmergency=mateThreat||checkedDanger||suddenCollapse;
        debugLogThreatDecision(nowScore,prevScore,checked,mateThreat,checkedDanger,suddenCollapse,g_assistEmergencyLatched);
        if(g_assistEmergencyLatched){
            if(!mateThreat&&!checkedDanger&&nowScore>-400){g_assistDangerClearScans++;if(g_assistDangerClearScans>=2)resetEmergencyEpisodeLatch();}else g_assistDangerClearScans=0;
            if(g_assistEmergencyLatched&&newEmergency&&!g_assistEmergency){setStatus(L"Critical danger is still part of the same emergency episode. Automatic Rescue will not re-arm again; use Ask Trainer or Full Assist if you want more help.");checkNeedResponse();return;}
        }
        if(!g_assistEmergency&&newEmergency){g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;startEmergencyRescue();wchar_t st[360]={0};if(mateThreat){wAppend(st,360,L"EMERGENCY RESCUE: forced mate is detected against you");wAppend(st,360,L" (mate in ");wAppendInt(st,360,-g_engineMate);wAppend(st,360,L"). The trainer will provide up to three course-correction moves.");}else if(checkedDanger)wCopy(st,360,L"EMERGENCY RESCUE: the opponent's move has left your king in a seriously losing check. Up to three corrective moves are enabled.");else wCopy(st,360,L"EMERGENCY RESCUE: the opponent's last move caused a sudden severe evaluation collapse. Up to three corrective moves are enabled.");setStatus(st);}
        else if(g_assistEmergency&&g_assistEmergencyRemaining<=0){
            if(mateThreat&&g_assistEmergencyExtensions<ASSIST_EMERGENCY_MAX_EXTRA){g_assistEmergencyRemaining=1;g_assistEmergencyExtensions++;wchar_t st[360]={0};wAppend(st,360,L"EMERGENCY EXTENSION ");wAppendInt(st,360,g_assistEmergencyUsed+1);wAppend(st,360,L": the normal three-move correction is complete, but Stockfish still sees forced mate in ");wAppendInt(st,360,-g_engineMate);wAppend(st,360,L". One extra defensive move is justified.");setStatus(st);}
            else{bool limitMate=mateThreat;if(limitMate)latchEmergencyEpisode();clearEmergencyRescue();setStatus(limitMate?L"Emergency Rescue reached its automatic limit. Forced mate is still detected, but this same danger episode is now locked from auto-rearming. Use Ask Trainer or Full Assist if you want more help.":L"Emergency course correction completed. Control is back to you; the trainer will intervene again only after a genuinely new serious trigger.");}
        }else if(g_assistEmergency&&!mateThreat&&!checked&&nowScore>-180){clearEmergencyRescue();setStatus(L"Emergency course correction succeeded early. Control is back to you.");}
        checkNeedResponse();return;
    }
    if(code==ENG_ASSIST){
        if(token!=g_positionVersion||g_trainerMode!=MODE_ASSISTED||!g_assistPending)return;g_calculating=false;g_assistPending=false;
        Move chosen=g_assistPendingMove;int ply=g_game.histCount;if(ply>=0&&ply<512){g_liveEvalWhite[ply]=g_userWhite?g_assistBaseScore:-g_assistBaseScore;g_liveEvalKnown[ply]=1;g_liveEvalWhite[ply+1]=g_userWhite?g_assistAfterScore:-g_assistAfterScore;g_liveEvalKnown[ply+1]=1;}int loss=g_assistBaseScore-g_assistAfterScore;bool mateDanger=(g_assistAfterScore<=-90000&&g_assistBaseScore>-90000);bool critical=mateDanger||loss>=ASSIST_BLUNDER_CP||(g_assistBaseScore>=100&&g_assistAfterScore<=-100&&loss>=160);debugLogAssistEvaluation(chosen,g_assistBaseScore,g_assistAfterScore,loss,critical,mateDanger);int previewClass=classifyMoveQuality(ply,g_assistBaseScore,g_assistAfterScore,NULLPTR);Position previewBefore=g_game.pos;if(previewClass==RV_INACCURACY||previewClass==RV_MISTAKE||previewClass==RV_MISS||previewClass==RV_BLUNDER)buildMoveExplanation(&previewBefore,chosen,previewClass,loss);
        // The move has already been played on the real board, so Assisted Mode never
        // asks the user to undo or replace it. A serious mistake simply arms Recovery
        // Coach for the user's next two turns.
        if(critical){if(g_assistRecoveryRemaining<2)g_assistRecoveryRemaining=2;g_assistRecoverySequence++;debugAppendRaw("ASSIST","Serious user move detected; two-move Recovery Coach armed.");}
        if(mateDanger){g_assistRecoveryRemaining=0;g_assistRecoverySequence=0;startEmergencyRescue();}
        if(g_requestAssistTerminal==200000)applyEngineEvaluationForSide(!g_game.pos.whiteToMove);
        commitRecordedMove(chosen);
        return;
    }
    if(code==ENG_CHOICES){
        if(token!=g_positionVersion||g_trainerMode!=MODE_ASSISTED||!g_choiceRequestPending)return;g_choiceRequestPending=false;g_calculating=false;Move first,second;bool has1=findLegalByUCI(g_engineChoice1,&first);bool has2=g_engineChoice2[0]&&findLegalByUCI(g_engineChoice2,&second)&&(!has1||!moveEq(first,second));if(!has1){setStatus(L"Trainer suggestions no longer match the current position.");updateControls();return;}
        int choice=showTrainerChoiceDialog(first,has2?&second:NULLPTR);if(choice==1||(choice==2&&has2)){g_suggested=choice==1?first:second;g_suggestionPending=true;g_onDemandSuggestion=true;g_suggestionGuideKind=GUIDE_ASK;Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);char san[20];sanForMove(&g_game.pos,g_suggested,legal,n,san);wchar_t desc[256]={0};humanMoveDescription(&g_game.pos,g_suggested,desc);wchar_t st[360]={0};wAppend(st,360,L"Trainer option selected: ");wAppend(st,360,desc);wAppend(st,360,L". Play it on the board, then choose 'Suggested Move'.");setStatus(st);invalidateBoard(); }else{recordSuggestionRejected(TRAIN_ASK);g_onDemandSuggestion=false;g_suggestionGuideKind=GUIDE_NONE;setStatus(L"Assisted Mode: no trainer option selected. Continue with your own move.");}
        updateControls();return;
    }
    if(code==ENG_COACH){
        if(token!=g_positionVersion||g_trainerMode!=MODE_COACH||!g_coachPending)return;g_calculating=false;g_coachPending=false;Move candidate=g_coachPendingMove,opp,follow;Position before=g_game.pos;Position afterCandidate=before;applyMoveRaw(&afterCandidate,candidate);bool hasOpp=g_coachOppReply[0]&&findLegalByUCIForPosition(&afterCandidate,g_coachOppReply,&opp);int loss=g_coachBeforeScore-g_coachAfterScore;if(loss<0)loss=0;bool hasFollow=false;if(hasOpp){Position q=afterCandidate;applyMoveRaw(&q,opp);if(g_coachUserReply[0]){hasFollow=findLegalByUCIForPosition(&q,g_coachUserReply,&follow);}}
        recordCoachReviewed();int choice=showCoachAdviceDialog(candidate,hasOpp?&opp:NULLPTR,hasFollow?&follow:NULLPTR,loss);if(choice!=1){setStatus(L"Coach Mode: proposed move not played. Try another move and the coach will review it again.");updateControls();invalidateBoard();return;}
        char san[20];if(!gamePush(candidate,san)){setStatus(L"Coach Mode: the proposed move is no longer legal.");updateControls();return;}debugLogMove("coach-user",candidate,san);int userPly=g_game.histCount-1;if(userPly>=0&&userPly<512){markTrainingPly(userPly,TRAIN_COACH_INDEPENDENT);g_reviewClass[userPly]=classifyMoveQuality(userPly,g_coachBeforeScore,g_coachAfterScore,&g_reviewLoss[userPly]);}g_positionVersion++;updateHistory();invalidateBoard();invalidateAnalysis();if(finishIfGameEnded(true))return;
        if(hasOpp){Move actualOpp;if(findLegalByUCI(g_coachOppReply,&actualOpp)){char osan[20];gamePush(actualOpp,osan);debugLogMove("coach-opponent",actualOpp,osan);int opPly=g_game.histCount-1;if(opPly>=0&&opPly<512){g_reviewClass[opPly]=RV_BEST;g_reviewLoss[opPly]=0;}g_positionVersion++;updateHistory();applyEngineEvaluation();invalidateBoard();invalidateAnalysis();if(finishIfGameEnded(true))return;wchar_t st[360]={0};wAppend(st,360,L"Coach Mode: computer played ");wAppendAscii(st,360,osan);wAppend(st,360,L". Now make your next move; the coach will explain it before the next reply.");setStatus(st);updateControls();return;}}
        checkNeedResponse();return;
    }
    if(code==ENG_PRACTICE){
        if(token!=g_positionVersion||(g_trainerMode!=MODE_PRACTICE&&g_trainerMode!=MODE_COACH))return;storeLiveEvalForCurrentPosition();applyEngineEvaluation();g_calculating=false;Move m;if(!findLegalByUCI(g_engineBest,&m)){setStatus(g_trainerMode==MODE_COACH?L"Coach Mode computer could not produce a legal move.":L"Practice opponent could not produce a legal move.");updateControls();return;}
        Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);char san[20];sanForMove(&g_game.pos,m,legal,n,san);char pushed[20];if(!gamePush(m,pushed)){setStatus(L"Practice opponent move no longer matches the position.");updateControls();return;}debugLogMove("practice-opponent",m,pushed);
        g_positionVersion++;g_requestToken=g_positionVersion;g_lastMate=0;updateHistory();invalidateBoard();invalidateAnalysis();if(finishIfGameEnded(true))return;g_calculating=true;setStatus(g_trainerMode==MODE_COACH?L"Coach Mode: recording the computer move...":L"Human Practice: recording and grading the opponent's move...");updateControls();requestAnalysis(g_positionVersion,REQ_TAG);return;
    }
    if(code==ENG_BEST){
        g_onDemandSuggestion=false;bool full=(g_trainerMode==MODE_FULL);int guide=g_requestGuideKind;bool assistedGuide=(g_trainerMode==MODE_ASSISTED&&guide!=GUIDE_NONE);
        if(token!=g_positionVersion||(!full&&!assistedGuide))return;g_requestGuideKind=GUIDE_NONE;storeLiveEvalForCurrentPosition();if(g_trainerMode!=MODE_FAIRPLAY)applyEngineEvaluation();g_calculating=false;Move m;if(!findLegalByUCI(g_engineBest,&m)){g_suggestionGuideKind=GUIDE_NONE;setStatus(L"Stockfish returned an unusable move.");updateControls();return;}g_suggested=m;g_suggestionPending=true;g_suggestionGuideKind=full?GUIDE_NONE:guide;debugStateCheck("engine-suggestion-generated");Move legal[256];int n=legalMovesFor(&g_game.pos,legal,256);char san[20];sanForMove(&g_game.pos,m,legal,n,san);debugLogMove("suggested",m,san);char dest[3];squareName(m.to,dest);wchar_t st[420];st[0]=0;
        if(guide==GUIDE_EMERGENCY){int ordinal=g_assistEmergencyUsed+1;if(g_assistEmergencyRemaining>0)g_assistEmergencyRemaining--;g_assistEmergencyUsed++;if(ordinal<=ASSIST_EMERGENCY_BASE_MOVES){wAppend(st,420,L"EMERGENCY RESCUE ");wAppendInt(st,420,ordinal);wAppend(st,420,L"/3: play ");wAppendAscii(st,420,san);wAppend(st,420,L" to ");wAppendAscii(st,420,dest);wAppend(st,420,L". This is a temporary course-correction move. After at most three rescue moves, control returns to you unless forced mate still requires an explained extension.");}else{wAppend(st,420,L"EMERGENCY EXTENSION ");wAppendInt(st,420,ordinal);wAppend(st,420,L": play ");wAppendAscii(st,420,san);wAppend(st,420,L" to ");wAppendAscii(st,420,dest);wAppend(st,420,L". This extra move is being suggested only because the latest threat scan still detected forced mate after the normal three-move correction.");}}
        else if(guide==GUIDE_RECOVERY){int ordinal=3-g_assistRecoveryRemaining;if(ordinal<1)ordinal=1;if(ordinal>2)ordinal=2;g_assistRecoveryRemaining--;wAppend(st,420,L"RECOVERY COACH ");wAppendInt(st,420,ordinal);wAppend(st,420,L"/2: best recovery move ");wAppendAscii(st,420,san);wAppend(st,420,L", destination ");wAppendAscii(st,420,dest);wAppend(st,420,L". Play it, then choose 'Suggested Move'.");if(g_assistRecoveryRemaining>0){wAppend(st,420,L" One guided recovery move will remain after this.");}else{wAppend(st,420,L" This is the second guided recovery move; normal Assisted Mode resumes unless Full Assist or Emergency Rescue is active.");}}
        else if(guide==GUIDE_FULL){wAppend(st,420,L"FULL ASSIST: best move ");wAppendAscii(st,420,san);wAppend(st,420,L", destination ");wAppendAscii(st,420,dest);wAppend(st,420,L". Play it, then choose 'Suggested Move'. Full Assist will automatically guide your next turn as well.");}
        else if(aFind(san,"#")>=0){wAppend(st,420,L"CHECKMATE AVAILABLE: ");wAppendAscii(st,420,san);wAppend(st,420,L". Play this move to checkmate your opponent, then choose 'Suggested Move'.");}
        else if(g_engineMate>0){wAppend(st,420,L"Forced mate found, mate in ");wAppendInt(st,420,g_engineMate);wAppend(st,420,L". Suggested move: ");wAppendAscii(st,420,san);wAppend(st,420,L".");}
        else{wAppend(st,420,L"Suggested move: ");wAppendAscii(st,420,san);wAppend(st,420,L", destination ");wAppendAscii(st,420,dest);wAppend(st,420,L". Play it, then choose 'Suggested Move', or use 'Different Move'.");}
        setStatus(st);updateControls();invalidateBoard();
    }
}

static LRESULT CALLBACK mainWndProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l){
    // Suppress background erasure. Main WM_PAINT always paints the full surface,
    // so this prevents visible white flashes during hover and resize.
    if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_PAINT){
        PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);
        int cw=rc.right-rc.left,ch=rc.bottom-rc.top;HDC mem=CreateCompatibleDC(dc);HBITMAP bmp=NULLPTR;HGDIOBJ oldBmp=NULLPTR;
        if(mem&&cw>0&&ch>0)bmp=CreateCompatibleBitmap(dc,cw,ch);
        HDC out=(mem&&bmp)?mem:dc;
        if(mem&&bmp)oldBmp=SelectObject(mem,bmp);
        if(g_bgBrush)FillRect(out,&rc,g_bgBrush);drawModernChrome(out);drawBoard(out);
        if(mem&&bmp){int pw=ps.rcPaint.right-ps.rcPaint.left,ph=ps.rcPaint.bottom-ps.rcPaint.top;if(pw>0&&ph>0)BitBlt(dc,ps.rcPaint.left,ps.rcPaint.top,pw,ph,mem,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);SelectObject(mem,oldBmp);DeleteObject(bmp);}
        if(mem)DeleteDC(mem);EndPaint(hwnd,&ps);return 0;
    }
    if(msg==WM_CTLCOLORSTATIC){HDC dc=(HDC)w;if((HWND)l==hStatus){SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGBc(108,117,125));return (LRESULT)g_bgBrush;}}
    if(msg==WM_GETMINMAXINFO){MINMAXINFO* mm=(MINMAXINFO*)l;mm->ptMinTrackSize.x=1024;mm->ptMinTrackSize.y=740;return 0;}
    if(msg==WM_SIZE){layoutUI();return 0;}
    if(msg==WM_TIMER){if(w==9101&&g_panelMode==1&&g_userSet){RECT tr={PANEL_X,PANEL_TOP,PANEL_X+PANEL_W,PANEL_BOTTOM};InvalidateRect(hwnd,&tr,FALSE);}return 0;}
    if(msg==WM_MOUSEMOVE){
        int x=signed16((int)(l&0xFFFF)),y=signed16((int)((l>>16)&0xFFFF));int hs=0;
        if(pointInRectI(x,y,EVAL_X-8,ANALYSIS_HEAD_Y-6,EVAL_X+EVAL_W+8,ANALYSIS_HEAD_Y+32))hs=1;
        else if(pointInRectI(x,y,EVAL_X-8,GAMELOG_HEAD_Y-6,EVAL_X+EVAL_W+8,GAMELOG_HEAD_Y+32))hs=2;
        else if(pointInRectI(x,y,EVAL_X-8,ENTER_HEAD_Y-6,EVAL_X+EVAL_W+8,ENTER_HEAD_Y+32))hs=3;
        if(hs!=g_hoverSection){
            int old=g_hoverSection;g_hoverSection=hs;
            RECT rr;
            if(old){int yy=old==1?ANALYSIS_HEAD_Y:(old==2?GAMELOG_HEAD_Y:ENTER_HEAD_Y);rr.left=EVAL_X-8;rr.top=yy-6;rr.right=EVAL_X+EVAL_W+8;rr.bottom=yy+34;InvalidateRect(hwnd,&rr,FALSE);}
            if(hs){int yy=hs==1?ANALYSIS_HEAD_Y:(hs==2?GAMELOG_HEAD_Y:ENTER_HEAD_Y);rr.left=EVAL_X-8;rr.top=yy-6;rr.right=EVAL_X+EVAL_W+8;rr.bottom=yy+34;InvalidateRect(hwnd,&rr,FALSE);}
        }
        TRACKMOUSEEVENT t;t.cbSize=sizeof(t);t.dwFlags=TME_LEAVE;t.hwndTrack=hwnd;t.dwHoverTime=0;TrackMouseEvent(&t);return 0;
    }
    if(msg==WM_MOUSELEAVE){if(g_hoverSection){int old=g_hoverSection;g_hoverSection=0;int yy=old==1?ANALYSIS_HEAD_Y:(old==2?GAMELOG_HEAD_Y:ENTER_HEAD_Y);RECT rr={EVAL_X-8,yy-6,EVAL_X+EVAL_W+8,yy+34};InvalidateRect(hwnd,&rr,FALSE);}return 0;}
    if(msg==WM_LBUTTONDOWN||msg==WM_RBUTTONDOWN){int x=signed16((int)(l&0xFFFF)),y=signed16((int)((l>>16)&0xFFFF));if(g_moveExplainActive&&x>=BOARD_X&&x<BOARD_X+BOARD_SIZE&&y>=BOARD_Y&&y<BOARD_Y+BOARD_SIZE){debugAppendRaw("USER","Move explanation dismissed by board click.");clearMoveExplanation();return 0;}if(msg==WM_RBUTTONDOWN)return 0;if(handlePanelClick(x,y))return 0;boardClick(x,y);return 0;}
    if(msg==WM_COMMAND){switch(lowWord(w)){case ID_WHITE:selectSide(true);break;case ID_BLACK:selectSide(false);break;case ID_ENTER:manualMove();break;case ID_NEW:newGame();break;case ID_FLIP:flipBoard();break;case ID_UNDO:undoMove();break;case ID_CONFIRM:confirmSuggested();break;case ID_DIFFERENT:playedDifferent();break;case ID_CORRECT:correctLastMove();break;case ID_MODE_FULL:setTrainerMode(MODE_FULL);break;case ID_MODE_ASSISTED:setTrainerMode(MODE_ASSISTED);break;case ID_MODE_PRACTICE:setTrainerMode(MODE_PRACTICE);break;case ID_MODE_FAIRPLAY:setTrainerMode(MODE_FAIRPLAY);break;case ID_MODE_COACH:setTrainerMode(MODE_COACH);break;case ID_ASSIST_FULL_ON:setAssistedFull(true);break;case ID_ASSIST_FULL_OFF:setAssistedFull(false);break;case ID_LOG:openDebugLog();break;case ID_COPY_PANEL:copyCurrentPanel();break;case ID_PROFILE:showProfileManager();break;case ID_END_OPP_RESIGN:debugAppendRaw("END-GAME","Opponent resigned button pressed.");manualFinishGame(RESULT_WIN,REASON_OPPONENT_RESIGNED);break;case ID_END_YOU_RESIGN:debugAppendRaw("END-GAME","Player resigned button pressed.");manualFinishGame(RESULT_LOSS,REASON_USER_RESIGNED);break;case ID_END_OPP_ABANDON:debugAppendRaw("END-GAME","Opponent abandoned button pressed.");manualFinishGame(RESULT_WIN,REASON_OPPONENT_ABANDONED);break;case ID_END_YOU_ABANDON:debugAppendRaw("END-GAME","Player abandoned button pressed.");manualFinishGame(RESULT_LOSS,REASON_USER_ABANDONED);break;case ID_END_OPP_TIMEOUT:debugAppendRaw("END-GAME","Opponent timed-out button pressed.");manualFinishGame(RESULT_WIN,REASON_OPPONENT_TIMEOUT);break;case ID_END_YOU_TIMEOUT:debugAppendRaw("END-GAME","Player timed-out button pressed.");manualFinishGame(RESULT_LOSS,REASON_USER_TIMEOUT);break;case ID_END_DRAW:debugAppendRaw("END-GAME","Draw button pressed.");manualFinishGame(RESULT_DRAW,REASON_DRAW_AGREED);break;case ID_END_CANCEL:debugAppendRaw("END-GAME","Cancel-game button pressed.");manualFinishGame(RESULT_ABORTED,REASON_CANCELLED);break;}return 0;}
    if(msg==WM_APP_ENGINE){handleEngineMessage((int)w,(int)l);return 0;}
    if(msg==WM_CLOSE){if(g_userSet&&!g_gameOver&&!g_gameRecorded&&g_game.histCount>0){int r=MessageBoxW(hwnd,L"The current game is still in progress.\r\n\r\nRecord it as cancelled/no result before closing?",L"Chess Trainer",MB_YESNOCANCEL|MB_ICONQUESTION);if(r==IDCANCEL)return 0;if(r==IDYES)recordCurrentGame(RESULT_ABORTED,REASON_CANCELLED);}DestroyWindow(hwnd);return 0;}
    if(msg==WM_DESTROY){KillTimer(hwnd,9101);if(g_gameClockActive){U64 du=0,doo=0,dt=0;currentGameClock(&du,&doo,&dt);char q[320]={0};aAppend(q,320,"active-game partial total_ms=");aAppendInt(q,320,(int)(dt>2147483647ULL?2147483647ULL:dt));aAppend(q,320," user_ms=");aAppendInt(q,320,(int)(du>2147483647ULL?2147483647ULL:du));aAppend(q,320," opponent_ms=");aAppendInt(q,320,(int)(doo>2147483647ULL?2147483647ULL:doo));aAppend(q,320," plies=");aAppendInt(q,320,g_game.histCount);debugAppendRaw("QA-SESSION",q);}debugAppendRaw("STOP","Chess Trainer window closing.");g_engineQuit=true;if(g_engineInWrite)engineWrite("stop\n");if(g_requestEvent)SetEvent(g_requestEvent);if(g_engineThread){WaitForSingleObject(g_engineThread,2000);CloseHandle(g_engineThread);g_engineThread=NULLPTR;}if(g_requestEvent){CloseHandle(g_requestEvent);g_requestEvent=NULLPTR;}if(g_lightBrush)DeleteObject(g_lightBrush);if(g_darkBrush)DeleteObject(g_darkBrush);if(g_uiFont)DeleteObject(g_uiFont);if(g_smallFont)DeleteObject(g_smallFont);if(g_titleFont)DeleteObject(g_titleFont);if(g_headingFont)DeleteObject(g_headingFont);if(g_pieceFont)DeleteObject(g_pieceFont);if(g_statusFont)DeleteObject(g_statusFont);if(g_footerFont)DeleteObject(g_footerFont);if(g_monoFont)DeleteObject(g_monoFont);if(g_monoBoldFont)DeleteObject(g_monoBoldFont);if(g_percentFont)DeleteObject(g_percentFont);if(g_coordFont)DeleteObject(g_coordFont);if(g_buttonFont)DeleteObject(g_buttonFont);if(g_actionButtonFont)DeleteObject(g_actionButtonFont);if(g_bgBrush)DeleteObject(g_bgBrush);if(g_cardBrush)DeleteObject(g_cardBrush);PostQuitMessage(0);return 0;}
    return DefWindowProcW(hwnd,msg,w,l);
}
static int runApp(){
    SetProcessDPIAware();
    g_hInstance=GetModuleHandleW(NULLPTR);debugInit();gameInit();debugAppendRaw("STARTUP","Core globals initialised.");
    WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=0;wc.lpfnWndProc=mainWndProc;wc.hInstance=g_hInstance;HICON appIcon=LoadIconW(g_hInstance,(LPCWSTR)(ULONG_PTR)101);bool customIconLoaded=appIcon!=NULLPTR;if(!appIcon)appIcon=LoadIconW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDI_APPLICATION_ID);wc.hIcon=appIcon;wc.hCursor=LoadCursorW(NULLPTR,(LPCWSTR)(ULONG_PTR)IDC_ARROW_ID);wc.hbrBackground=NULLPTR;wc.lpszClassName=L"ChessTrainerNativeCpp";wc.hIconSm=appIcon;if(!RegisterClassExW(&wc)){debugAppendRaw("STARTUP-ERROR","RegisterClassExW failed.");return 1;}
    g_mainHwnd=CreateWindowExW(0,L"ChessTrainerNativeCpp",L"Chess Trainer 6.0.0",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,70,35,1360,920,NULLPTR,NULLPTR,g_hInstance,NULLPTR);if(!g_mainHwnd){debugAppendRaw("STARTUP-ERROR","CreateWindowExW failed.");return 2;}SendMessageW(g_mainHwnd,WM_SETICON,ICON_BIG,(LPARAM)appIcon);SendMessageW(g_mainHwnd,WM_SETICON,ICON_SMALL,(LPARAM)appIcon);debugAppendRaw("ICON-QA",customIconLoaded?"Custom Chess Trainer icon resource 101 loaded and applied to the top-level window.":"Custom icon resource 101 was unavailable; Windows fallback icon applied.");buildUI();ShowWindow(g_mainHwnd,SW_SHOW);UpdateWindow(g_mainHwnd);debugAppendRaw("STARTUP","Main window created and shown.");SetTimer(g_mainHwnd,9101,1000,NULLPTR);
    loadProfileStore();refreshProfileButton();debugAppendRaw("STARTUP","Player Profile store loaded. Local storage only.");loadHistoryStore();debugAppendRaw("STARTUP","History store loaded.");loadArchive();debugAppendRaw("STARTUP","Game archive loaded.");loadProfileGameMap();debugAppendRaw("STARTUP","Profile/history ownership map loaded.");loadRatingStore();refreshProfileButton();debugAppendRaw("STARTUP","Active Player Profile rating loaded.");debugProfileQa();g_ratingBeforeGame=g_currentRating;g_lastRatingAfter=g_currentRating;runCoreSelfTest();runExtendedSelfTest();debugAppendRaw("STARTUP","Core and extended self-tests completed.");
    g_requestEvent=CreateEventW(NULLPTR,FALSE,FALSE,NULLPTR);if(!g_requestEvent){setStatus(L"Could not initialise the Stockfish worker.");debugAppendRaw("STARTUP-ERROR","CreateEventW failed for engine worker.");}else{DWORD tid=0;g_engineThread=CreateThread(NULLPTR,0,engineThreadProc,NULLPTR,0,&tid);if(!g_engineThread){setStatus(L"Could not start the Stockfish worker thread.");debugAppendRaw("STARTUP-ERROR","CreateThread failed for engine worker.");}else debugAppendRaw("STARTUP","Stockfish worker thread created.");}
    MSG msg;while(GetMessageW(&msg,NULLPTR,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return 0;
}

// Custom GUI entry point, no console and no C runtime startup.
extern "C" void WINAPI wWinMainCRTStartup(){int rc=runApp();ExitProcess((UINT)rc);}
