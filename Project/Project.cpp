#include <windows.h>
#include <string>
#include <vector>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawPolygon(HDC hdc, const std::vector<POINT>& points, int offsetX, int offsetY);

const int NUM_POINTS = 6;
std::vector<POINT> points(NUM_POINTS);
HWND hwndInputs[NUM_POINTS * 2];
HWND hwndDrawButton, hwndResetButton;
COLORREF bgColor = RGB(245, 245, 200); // Цвет фона
const int OFFSET_X = 250; // Смещение по X
const int OFFSET_Y = 20; // Смещение по Y

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"PolygonDrawer";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, L"PolygonDrawer", L"Polygon Drawer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 800, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        for (int i = 0; i < NUM_POINTS; i++) {
            // Подписи у полей ввода
            CreateWindow(L"STATIC", (L"X" + std::to_wstring(i + 1)).c_str(), WS_CHILD | WS_VISIBLE, 20, 20 + i * 30, 20, 20, hwnd, nullptr, nullptr, nullptr);
            CreateWindow(L"STATIC", (L"Y" + std::to_wstring(i + 1)).c_str(), WS_CHILD | WS_VISIBLE, 95, 20 + i * 30, 20, 20, hwnd, nullptr, nullptr, nullptr);

            // Поля для ввода координат
            hwndInputs[i * 2] = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 40, 20 + i * 30, 50, 20, hwnd, nullptr, nullptr, nullptr);
            hwndInputs[i * 2 + 1] = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 115, 20 + i * 30, 50, 20, hwnd, nullptr, nullptr, nullptr);
        }
        hwndDrawButton = CreateWindow(L"BUTTON", L"Нарисовать", WS_CHILD | WS_VISIBLE, 20, 200, 145, 30, hwnd, (HMENU)1, nullptr, nullptr);
        hwndResetButton = CreateWindow(L"BUTTON", L"Сбросить", WS_CHILD | WS_VISIBLE, 20, 235, 145, 30, hwnd, (HMENU)2, nullptr, nullptr);
    }
                  break;

    case WM_COMMAND: {
        if (LOWORD(wParam) == 1 || LOWORD(wParam) == 2) {
            if (LOWORD(wParam) == 2) {  // Кнопка Reset
                for (int i = 0; i < NUM_POINTS; i++) {
                    SetWindowText(hwndInputs[i * 2], L"");
                    SetWindowText(hwndInputs[i * 2 + 1], L"");
                }
            }

            // Получение новых значений координат и их преобразование
            for (int i = 0; i < NUM_POINTS; i++) {
                wchar_t buffer[10];
                GetWindowText(hwndInputs[i * 2], buffer, 10);
                points[i].x = _wtoi(buffer);
                GetWindowText(hwndInputs[i * 2 + 1], buffer, 10);
                points[i].y = _wtoi(buffer);
            }

            // Перерисовка окна
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
                   break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Заливка фона
        HBRUSH hBrush = CreateSolidBrush(bgColor);
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        // Рисование многоугольника с учетом смещения
        DrawPolygon(hdc, points, OFFSET_X, OFFSET_Y);

        EndPaint(hwnd, &ps);
    }
                 break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void DrawPolygon(HDC hdc, const std::vector<POINT>& points, int offsetX, int offsetY) {
    if (points.size() >= 2) {
        MoveToEx(hdc, points[0].x + offsetX, points[0].y + offsetY, NULL);
        for (size_t i = 1; i < points.size(); i++) {
            LineTo(hdc, points[i].x + offsetX, points[i].y + offsetY);
        }
        LineTo(hdc, points[0].x + offsetX, points[0].y + offsetY);  // Замыкаем многоугольник
    }
}
