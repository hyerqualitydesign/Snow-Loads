#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>

#define IDC_STATIC_RISK   1001
#define IDC_COMBO_RISK    1002
#define IDC_STATIC_WIND   1007
#define IDC_COMBO_WIND    1008
#define IDC_STATIC_PG     1003
#define IDC_EDIT_PG       1004
#define IDC_BUTTON_APPLY  1005
#define IDC_STATIC_RESULT 1006

HWND g_hCombo = NULL;
HWND g_hComboWind = NULL;
HWND g_hEdit = NULL;
HWND g_hResult = NULL;

void UpdateResult(HWND hwnd) {
    if (!g_hCombo || !g_hComboWind || !g_hEdit || !g_hResult) return;

    int idx = (int)SendMessage(g_hCombo, CB_GETCURSEL, 0, 0);
    if (idx == CB_ERR) idx = 0;

    char cat[16] = {0};
    SendMessageA(g_hCombo, CB_GETLBTEXT, idx, (LPARAM)cat);

    int wIdx = (int)SendMessage(g_hComboWind, CB_GETCURSEL, 0, 0);
    if (wIdx == CB_ERR) wIdx = 0;

    char wind[8] = {0};
    SendMessageA(g_hComboWind, CB_GETLBTEXT, wIdx, (LPARAM)wind);

    char pgBuf[64] = {0};
    GetWindowTextA(g_hEdit, pgBuf, sizeof(pgBuf));

    double pg = 0.0;
    try {
        pg = std::stod(pgBuf);
    } catch (...) {
        pg = 0.0;
    }

    std::ostringstream oss;
    oss << "Risk Category: " << cat << "\r\n";
    oss << "Wind Exposure: " << wind << "\r\n";
    oss << "Ground Snow Load: " << std::fixed << std::setprecision(1) << pg << " psf";

    SetWindowTextA(g_hResult, oss.str().c_str());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindowA("STATIC", "Risk Category:",
            WS_VISIBLE | WS_CHILD,
            20, 20, 120, 20, hwnd, (HMENU)IDC_STATIC_RISK, NULL, NULL);

        g_hCombo = CreateWindowA("COMBOBOX", "",
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
            150, 18, 80, 120, hwnd, (HMENU)IDC_COMBO_RISK, NULL, NULL);

        SendMessageA(g_hCombo, CB_ADDSTRING, 0, (LPARAM)"I");
        SendMessageA(g_hCombo, CB_ADDSTRING, 0, (LPARAM)"II");
        SendMessageA(g_hCombo, CB_ADDSTRING, 0, (LPARAM)"III");
        SendMessageA(g_hCombo, CB_ADDSTRING, 0, (LPARAM)"IV");
        SendMessageA(g_hCombo, CB_SETCURSEL, 1, 0);

        CreateWindowA("STATIC", "Wind Exposure:",
            WS_VISIBLE | WS_CHILD,
            20, 50, 120, 20, hwnd, (HMENU)IDC_STATIC_WIND, NULL, NULL);

        g_hComboWind = CreateWindowA("COMBOBOX", "",
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
            150, 48, 80, 120, hwnd, (HMENU)IDC_COMBO_WIND, NULL, NULL);

        SendMessageA(g_hComboWind, CB_ADDSTRING, 0, (LPARAM)"B");
        SendMessageA(g_hComboWind, CB_ADDSTRING, 0, (LPARAM)"C");
        SendMessageA(g_hComboWind, CB_ADDSTRING, 0, (LPARAM)"D");
        SendMessageA(g_hComboWind, CB_SETCURSEL, 1, 0);  // default to C

        CreateWindowA("STATIC", "Ground Snow Load (psf):",
            WS_VISIBLE | WS_CHILD,
            20, 80, 160, 20, hwnd, (HMENU)IDC_STATIC_PG, NULL, NULL);

        g_hEdit = CreateWindowA("EDIT", "25.0",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            190, 78, 100, 22, hwnd, (HMENU)IDC_EDIT_PG, NULL, NULL);

        CreateWindowA("BUTTON", "Apply",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            300, 77, 70, 24, hwnd, (HMENU)IDC_BUTTON_APPLY, NULL, NULL);

        g_hResult = CreateWindowA("STATIC",
            "Risk Category: II\r\nWind Exposure: C\r\nGround Snow Load: 25.0 psf",
            WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER,
            20, 125, 360, 70, hwnd, (HMENU)IDC_STATIC_RESULT, NULL, NULL);

        UpdateResult(hwnd);
        return 0;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wParam);
        WORD code = HIWORD(wParam);
        if (id == IDC_BUTTON_APPLY && code == BN_CLICKED) {
            UpdateResult(hwnd);
        } else if (id == IDC_COMBO_RISK && code == CBN_SELCHANGE) {
            UpdateResult(hwnd);
        } else if (id == IDC_COMBO_WIND && code == CBN_SELCHANGE) {
            UpdateResult(hwnd);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* className = "SnowLoadApp";

    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = className;

    if (!RegisterClassExA(&wc)) {
        return 1;
    }

    HWND hwnd = CreateWindowExA(
        0, className, "ASCE 7 Snow Loads",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 260,
        NULL, NULL, hInstance, NULL
    );
    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
