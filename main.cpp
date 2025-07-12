#define DEBUG
#include<Windows.h>
#include<float.h>
#include<stdio.h>
#include<iostream>
#include"resource.h"
#include"Constants.h"
#include"..\\LastError\\LastError.h"

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID SetSkin(HWND hwnd, CONST CHAR sz_skin[]);
VOID SetSkinFromDLL(HWND hwnd, CONST CHAR sz_skin[]);
VOID LoadFontFromDLL(HMODULE hFontModule, INT resourceID);
VOID LoadFontsFromDLL(HMODULE hFontModule);
VOID SetFont(HWND hwnd, CONST CHAR font_name[]);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна:
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbWndExtra = 0;
	wClass.cbClsExtra = 0;

	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	wClass.hInstance = hInstance;
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = g_sz_CLASS_NAME;
	wClass.lpfnWndProc = (WNDPROC)WndProc;

	if (!RegisterClassEx(&wClass))
	{
		MessageBox(NULL, "Class registration failed", "", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASS_NAME,
		g_sz_CLASS_NAME,
		//WS_OVERLAPPED | WS_SYSMENU /*| WS_THICKFRAME*/ | WS_MINIMIZEBOX /*| WS_MAXIMIZEBOX*/,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);	//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
	UpdateWindow(hwnd);			//			taskkill /f /im  calc.exe

	//3) Запуск цикла сообщений:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DOUBLE a = DBL_MIN;
	static DOUBLE b = DBL_MIN;
	static INT operation = 0;
	static BOOL input = FALSE;				//Пользователь ввел число
	static BOOL input_operation = FALSE;	//Пользователь ввел знак операции

	static INT index = 0;
	static INT font_index = 0;
	static BOOL window_color_changed = TRUE;

	switch (uMsg)
	{
	case WM_CREATE:
	{
#ifdef DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		system("CHCP 1251");
#endif // DEBUG
		HWND hEditDisplay = CreateWindowEx
		(
			NULL, "Edit", "0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_NOHIDESEL,
			g_i_BUTTON_START_X, g_i_START_Y,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd,
			(HMENU)IDC_EDIT_DISPLAY,
			GetModuleHandle(NULL),
			NULL
		);

		/*AddFontResourceEx("Fonts\\digital-7 (mono).ttf", FR_PRIVATE, 0);
		HFONT hFont = CreateFont
		(
			g_i_DISPLAY_HEIGHT - 2, g_i_DISPLAY_HEIGHT / 3,
			0,
			0,
			FW_BOLD,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			FF_DONTCARE,
			"digital-7 mono"
		);*/
		//SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

		INT iDigit = IDC_BUTTON_1;
		CHAR szDigit[2] = {};
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				szDigit[0] = iDigit - IDC_BUTTON_0 + '0';
				CreateWindowEx
				(
					NULL, "Button", szDigit,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
					g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * j,
					g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * i / 3,
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd,
					(HMENU)iDigit,
					GetModuleHandle(NULL),
					NULL
				);
				iDigit++;
			}
		}
		CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X, g_i_BUTTON_START_Y + (g_i_BUTTON_SPACE) * 3,
			g_i_BUTTON_SIZE_DOUBLE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL),
			NULL
		);
		CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 2,
			g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * 3,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL),
			NULL
		);
		//////////////////////////////////////////////////////
		for (int i = 0; i < 4; i++)
		{
			CreateWindowEx
			(
				NULL, "Button", g_sz_OPERATIONS[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 3, g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * (3 - i),
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd,
				(HMENU)(IDC_BUTTON_PLUS + i),
				GetModuleHandle(NULL),
				NULL
			);
		}
		for (int i = 0; i < 3; i++)
		{
			CreateWindowEx
			(
				NULL, "Button", g_sz_EDIT[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 4,
				g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * i,
				g_i_BUTTON_SIZE, i < 2 ? g_i_BUTTON_SIZE : g_i_BUTTON_SIZE_DOUBLE,
				hwnd,
				(HMENU)(IDC_BUTTON_BSP + i),
				GetModuleHandle(NULL),
				NULL
			);
		}
		//HICON hIcon = LoadIcon(GetModuleHandle(NULL), "ICO\\palm.ico");
		HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), "BMP\\0.bmp", IMAGE_BITMAP, LR_DEFAULTSIZE, LR_DEFAULTSIZE, 
			LR_LOADFROMFILE);

		//SetSkin(hwnd, "metal_mistral");
		SetSkinFromDLL(hwnd, "square_blue");
		HMODULE hFonts = LoadLibrary("Fonts.DLL");
		PrintLastError(GetLastError());
		LoadFontsFromDLL(hFonts);
		SetFont(hwnd, g_sz_FONT[font_index]);
	}
	break;

	case WM_CTLCOLOREDIT:
	{
		HDC hdcEdit = (HDC)wParam;	//HDC - Handler to Device Context
		SetBkMode(hdcEdit, OPAQUE);
		SetBkColor(hdcEdit, g_DISPLAY_BACKGROUND[index]);
		SetTextColor(hdcEdit, g_DISPLAY_FOREGROUND[index]);

		if (window_color_changed)
		{
			window_color_changed = FALSE;
			HBRUSH hbrBackground = CreateSolidBrush(g_WINDOW_BACKGROUND[index]);
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)hbrBackground);
			SendMessage(hwnd, WM_ERASEBKGND, wParam, 0);
			RedrawWindow(hwnd, NULL, NULL, RDW_ERASE);
			return (LRESULT)hbrBackground;
		}
	}
	break;
	case WM_COMMAND:
	{
		CHAR szDisplay[g_SIZE] = {};
		CHAR szDigit[2] = {};
		HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_POINT)
		{
			if (input == FALSE)SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"0");
			if (LOWORD(wParam) == IDC_BUTTON_POINT)	szDigit[0] = '.';
			else				szDigit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
			if (szDisplay[0] == '0' && szDisplay[1] != '.')szDisplay[0] = 0;
			if (szDigit[0] == '.' && strchr(szDisplay, '.'))break;
			//https://legacy.cplusplus.com/reference/cstring/strchr/
			//Функция strchr() выполняет поиск символа в строке.
			//Если символ найден, то strchr() возвращает указатель на найденный символ,
			//в противном случае возвращает 'nullptr' (физический 0);
			//0 - это ''false;
			//true - это все что НЕ 0;
			strcat(szDisplay, szDigit);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
			input = TRUE;
		}
		if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
		{
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
			if (input && a == DBL_MIN)a = atof(szDisplay);	//https://legacy.cplusplus.com/reference/cstdlib/atof/
			if (input)b = atof(szDisplay);
			//else break;
			//(input && a == DBL_MIN ? a : b) = atof(szDisplay);
			input = FALSE;
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);	//выполняем предыдущую операцию
			operation = LOWORD(wParam);		//и только после этого запоминаем введенную операцию
			input_operation = TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_BSP)
		{
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
			if (strlen(szDisplay) > 1)szDisplay[strlen(szDisplay) - 1] = 0;
			else szDisplay[0] = '0';
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
		}
		if (LOWORD(wParam) == IDC_BUTTON_CLR)
		{
			a = b = DBL_MIN;
			operation = 0;
			input = input_operation = FALSE;
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"0");
		}
		if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
		{
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
			if (input && a == DBL_MIN)a = atof(szDisplay);	//https://legacy.cplusplus.com/reference/cstdlib/atof/
			if (input) b = atof(szDisplay);
			if (a == DBL_MIN) break;
			//(input && a == DBL_MIN ? a : b) = atof(szDisplay);
			input = FALSE;
			switch (operation)
			{
			case IDC_BUTTON_PLUS:	a += b;		break;
			case IDC_BUTTON_MINUS:	a -= b;		break;
			case IDC_BUTTON_ASTER:	a *= b;		break;
			case IDC_BUTTON_SLASH:	a /= b;		break;
			}
			input_operation = FALSE;
			sprintf(szDisplay, "%g", a);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
		}

		//if (LOWORD(wParam) == IDC_EDIT_DISPLAY && HIWORD(wParam) == EN_SETFOCUS)
		SetFocus(hwnd);

	}
	break;

	case WM_KEYDOWN:
	{
		if (GetKeyState(VK_SHIFT) < 0)
		{
			if (wParam == 0x38)
			{
				SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0);
				//SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
			}
		}
		else if (wParam >= '0' && wParam <= '9')
		{
			SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - '0' + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
		}
		else if (wParam >= 0x60 && wParam <= 0x69)
		{
			SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - 0x60 + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
		}


		switch (wParam)
		{
		case VK_OEM_PERIOD:
		case VK_DECIMAL:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, TRUE, 0); break;
		case VK_ADD:
		case VK_OEM_PLUS:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, TRUE, 0); break;
		case VK_SUBTRACT:
		case VK_OEM_MINUS:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, TRUE, 0); break;
		case VK_MULTIPLY:	SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0); break;
		case VK_DIVIDE:
		case VK_OEM_2:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, TRUE, 0); break;
		case VK_BACK:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, TRUE, 0); break;
		case VK_ESCAPE:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, TRUE, 0); break;
		case VK_RETURN:		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, TRUE, 0); break;
		}
	}
	break;
	case WM_KEYUP:
	{
		if (GetKeyState(VK_SHIFT) < 0)
		{
			if (wParam == 0x38)
			{
				SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
			}
		}
		else if (wParam >= '0' && wParam <= '9')
		{
			SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - '0' + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, LOWORD(wParam) - '0' + IDC_BUTTON_0, 0);
		}
		else if (wParam >= 0x60 && wParam <= 0x69)
		{
			SendMessage(GetDlgItem(hwnd, LOWORD(wParam) - 0x60 + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, LOWORD(wParam) - 0x60 + IDC_BUTTON_0, 0);
		}


		switch (wParam)
		{
		case VK_OEM_PERIOD:
		case VK_DECIMAL:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_POINT, 0);
			break;
		case VK_ADD:
		case VK_OEM_PLUS:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_PLUS, 0);
			break;
		case VK_SUBTRACT:
		case VK_OEM_MINUS:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_MINUS, 0);
			break;
		case VK_MULTIPLY:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
			break;
		case VK_DIVIDE:
		case VK_OEM_2:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_SLASH, 0);
			break;
		case VK_BACK:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_BSP, 0);
			break;
		case VK_ESCAPE:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_CLR, 0);
			break;
		case VK_RETURN:
			SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, FALSE, 0);
			SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);
			break;
		}
	}
	break;

	case WM_CONTEXTMENU:
	{
		HMENU hMainMenu = CreatePopupMenu();
		InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_STRING, CM_EXIT, "Exit");
		InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_STRING, CM_SQUARE_BLUE, "Square Blue");
		InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_STRING, CM_METAL_MISTRAL, "Metal Mistral");
		InsertMenu(hMainMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		HMENU hMenuFonts = CreatePopupMenu();
		for (INT i = 0; i < 3; i++)
			InsertMenu(hMenuFonts, i, MF_BYPOSITION | MF_STRING, i + 301, g_sz_FONT[i]);
		InsertMenu(hMainMenu, 0, MF_POPUP | MF_BYPOSITION, (UINT_PTR)hMenuFonts, "Fonts");

		CheckMenuItem(hMenuFonts, font_index, MF_BYPOSITION | MF_CHECKED);
		CheckMenuItem(hMainMenu, index + 201, MF_BYCOMMAND | MF_CHECKED);

		BOOL item = TrackPopupMenuEx(hMainMenu, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_BOTTOMALIGN, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
		//TPM_RETURNCMD - возвращает ID_-ресурса выбранного элемента:
			//CM_EXIT						200
			//CM_SQUARE_BLUE				201
			//CM_METAL_MISTRAL				202
		//BOOL

		switch (item)
		{
		case CM_EXIT:			SendMessage(hwnd, WM_DESTROY, 0, 0);	break;
		case CM_SQUARE_BLUE:	SetSkinFromDLL(hwnd, "square_blue");	break;
		case CM_METAL_MISTRAL:	SetSkinFromDLL(hwnd, "metal_mistral");	break;
		}
		DestroyMenu(hMainMenu);

		if (item >= 201 && item <= 210)
		{
			index = item - CM_EXIT - 1;
			//SetSkin(hwnd, g_sz_SKIN[index]);
			SetSkinFromDLL(hwnd, g_sz_SKIN[index]);

			HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
			HDC hdcEditDisplay = GetDC(hEditDisplay);
			/////////////////////////////////////////////////////////////////////////////
			window_color_changed = TRUE;
			/////////////////////////////////////////////////////////////////////////////
			SendMessage(hwnd, WM_CTLCOLOREDIT, (WPARAM)hdcEditDisplay, 0);
			ReleaseDC(hEditDisplay, hdcEditDisplay);	//Контекст устройства обяхательно нужно освобождать

			CHAR sz_buffer[g_SIZE] = {};
			//sprintf(sz_buffer, "%i", item);;;;;;;;;;;;;;;
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)sz_buffer);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_buffer);
			//SetFocus(hEditDisplay);
		}
		if (item >= 301 && item <= 303)
		{
			font_index = item - 300 - 1;
			SetFont(hwnd, g_sz_FONT[font_index]);
		}
	}
	break;

	case WM_DESTROY:
		FreeConsole();
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}
LPSTR FormatLastError(DWORD dwErrorID)
{
	//DWORD dwErrorID = GetLastError();
	LPSTR lpszMessage = NULL;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_RUSSIAN_RUSSIA),
		(LPSTR)&lpszMessage,
		NULL,
		NULL
	);
	return lpszMessage;
}
VOID PrintLastError(DWORD dwErrorID)
{
	LPSTR lpszMessage = FormatLastError(GetLastError());
	std::cout << lpszMessage << std::endl;
	LocalFree(lpszMessage);
}
VOID SetSkin(HWND hwnd, CONST CHAR sz_skin[])
{
	std::cout << "SetSkin()" << std::endl;
	CHAR sz_filename[FILENAME_MAX] = {};
	for (int i = 0; i <= 9; i++)
	{
		sprintf(sz_filename, "BMP\\%s\\button_%i.bmp", sz_skin, i);
		HBITMAP bmpIcon = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			sz_filename,
			IMAGE_BITMAP,
			i == 0 ? g_i_BUTTON_SIZE_DOUBLE : g_i_BUTTON_SIZE,
			g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		PrintLastError(GetLastError());
		//MessageBox(hwnd, lpszMessage, "", MB_OK);
		SendMessage(GetDlgItem(hwnd, IDC_BUTTON_0 + i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpIcon);
	}
	for (int i = IDC_BUTTON_POINT; i <= IDC_BUTTON_EQUAL; i++)
	{
		sprintf(sz_filename, "BMP\\%s\\button_%s.bmp", sz_skin, g_sz_BUTTON_FILENAMES[i - IDC_BUTTON_POINT]);
		HBITMAP bmpIcon = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			sz_filename,
			IMAGE_BITMAP,
			g_i_BUTTON_SIZE,
			i == IDC_BUTTON_EQUAL ? g_i_BUTTON_SIZE_DOUBLE : g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpIcon);
	}
	std::cout << delimiter << std::endl;
}
VOID SetSkinFromDLL(HWND hwnd, CONST CHAR sz_skin[])
{
	HMODULE hButtonsModule = LoadLibrary(sz_skin);
	//Принципиально важно чтобы "*.dll"-файл находился в одном каталоге с нашим "*.exe"-файлом!!!
	//HINSTANCE hButtons = GetModuleHandle("Buttons.dll");

	for (int i = IDC_BUTTON_0; i <= IDC_BUTTON_EQUAL; i++)
	{
		HBITMAP bmpButton = (HBITMAP)LoadImage
		(
			hButtonsModule,
			MAKEINTRESOURCE(i),
			IMAGE_BITMAP,
			i == IDC_BUTTON_0 ? g_i_BUTTON_SIZE_DOUBLE : g_i_BUTTON_SIZE,
			i == IDC_BUTTON_EQUAL ? g_i_BUTTON_SIZE_DOUBLE : g_i_BUTTON_SIZE,
			LR_SHARED
		);
		PrintLastError(GetLastError());
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpButton);
	}
	FreeLibrary(hButtonsModule);
}

VOID LoadFontFromDLL(HMODULE hFontModule, INT resourceID)
{	
	HRSRC hFntRes = FindResource(hFontModule, MAKEINTRESOURCE(resourceID), MAKEINTRESOURCE(RT_FONT));
	HGLOBAL hFntMem = LoadResource(hFontModule, hFntRes);
	VOID* fntData = LockResource(hFntMem);
	DWORD nFonts = 0;
	DWORD len = SizeofResource(hFontModule, hFntRes);
	AddFontMemResourceEx(fntData, len, NULL, &nFonts);
}

VOID LoadFontsFromDLL(HMODULE hFontModule)
{
	for (int i = 301; i < 304; i++)
	{
		LoadFontFromDLL(hFontModule, i);
	}
}

//VOID SetFont(HWND hEdit, CONST CHAR* font_name)
//{
//	HFONT hNewFont = CreateFont
//	(
//		16,                       // Height of font
//		0,                         // Average character width
//		0,                         // Angle of escapement
//		0,                         // Base-line orientation angle
//		FW_NORMAL,                 // Font weight
//		FALSE,                     // Italic attribute option
//		FALSE,                     // Underline attribute option
//		FALSE,                     // Strikeout attribute option
//		ANSI_CHARSET,              // Character set
//		OUT_DEFAULT_PRECIS,        // Output precision
//		CLIP_DEFAULT_PRECIS,       // Clipping precision
//		ANTIALIASED_QUALITY,       // Output quality
//		FF_DONTCARE,               // Family and pitch
//		font_name                    // Font name
//	);
//
//	// Check if the font creation was successful
//	if (hNewFont) 
//	{
//		// Get the old font
//		HFONT hOldFont = (HFONT)SendMessage(hEdit, WM_GETFONT, 0, 0);
//
//		// Set the new font for the Edit control
//		SendMessage(hEdit, WM_SETFONT, (WPARAM)hNewFont, TRUE);
//
//		// Optionally: Delete the old font to prevent memory leaks
//		if (hOldFont) 
//		{
//			DeleteObject(hOldFont);
//		}
//	}
//	else 
//	{
//		// Handle the error: Display a message or log the failure.
//		std::cerr << "Error creating font: " << font_name << std::endl;
//	}
//}

VOID SetFont(HWND hwnd, CONST CHAR font_name[])
{
	HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
	//AddFontResourceEx("Fonts\\digital-7 (mono).ttf", FR_PRIVATE, 0);
		HFONT hFont = CreateFont
		(
			g_i_DISPLAY_HEIGHT - 2, g_i_DISPLAY_HEIGHT / 3,
			0,
			0,
			FW_BOLD,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			FF_DONTCARE,
			font_name
		);
		SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
}