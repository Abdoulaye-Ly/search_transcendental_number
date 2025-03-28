#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SearchSequence(HWND hwnd);
void CreateControls(HWND hwnd);

HWND hInput, hButton, hResult, hDropdown;

vector<int> computePrefixFunction(const wstring& pattern) {
    int m = pattern.length();
    vector<int> prefix(m, 0);
    int j = 0;
    for (int i = 1; i < m; i++) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = prefix[j - 1];
        }
        if (pattern[i] == pattern[j]) {
            j++;
        }
        prefix[i] = j;
    }
    return prefix;
}

int kmpSearch(const wstring& text, const wstring& pattern) {
    vector<int> prefix = computePrefixFunction(pattern);
    int j = 0;
    for (int i = 0; i < text.length(); i++) {
        while (j > 0 && text[i] != pattern[j]) {
            j = prefix[j - 1];
        }
        if (text[i] == pattern[j]) {
            j++;
        }
        if (j == pattern.length()) {
            return i - j + 1;
        }
    }
    return -1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"PiSearchWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Search in Transcendental Numbers", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateControls(hwnd);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            SearchSequence(hwnd);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateControls(HWND hwnd) {
    hDropdown = CreateWindowW(L"COMBOBOX", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        50, 20, 200, 100, hwnd, (HMENU)3, NULL, NULL);
    SendMessageW(hDropdown, CB_ADDSTRING, 0, (LPARAM)L"Pi");
    SendMessageW(hDropdown, CB_ADDSTRING, 0, (LPARAM)L"e");
    SendMessageW(hDropdown, CB_ADDSTRING, 0, (LPARAM)L"Phi");
    SendMessageW(hDropdown, CB_SETCURSEL, 0, 0);

    hInput = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        50, 70, 200, 20, hwnd, (HMENU)2, NULL, NULL);
    hButton = CreateWindowW(L"BUTTON", L"Search", WS_CHILD | WS_VISIBLE,
        270, 70, 100, 25, hwnd, (HMENU)1, NULL, NULL);
    hResult = CreateWindowW(L"STATIC", L"Result: ", WS_CHILD | WS_VISIBLE,
        50, 120, 320, 20, hwnd, NULL, NULL, NULL);
}

void SearchSequence(HWND hwnd) {
    wchar_t sequence[20];
    GetWindowTextW(hInput, sequence, 20);

    int selectedIndex = SendMessageW(hDropdown, CB_GETCURSEL, 0, 0);
    wstring filename;
    switch (selectedIndex) {
    case 0: filename = L"pi_digits.txt"; break;
    case 1: filename = L"e_digits.txt"; break;
    case 2: filename = L"phi_digits.txt"; break;
    default: filename = L"pi_digits.txt"; break;
    }

    wifstream file(filename);
    wstring number;
    if (file.is_open()) {
        getline(file, number);
        file.close();
    }
    else {
        SetWindowTextW(hResult, L"Error: Unable to open file");
        return;
    }

    int index = kmpSearch(number, wstring(sequence));
    wchar_t result[100];
    if (index != -1) {
        swprintf_s(result, L"Sequence found at index: %d", index);
    }
    else {
        swprintf_s(result, L"Sequence not found.");
    }
    SetWindowTextW(hResult, result);
}
