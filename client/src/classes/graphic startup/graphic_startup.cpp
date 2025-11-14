#include <classes/Class_Include.hpp>
#include <server/preperation/preperation.hpp>
#include <server/connection/connection.hpp>

namespace n_graphic {

    UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND hStatusText;
    HWND hConnectText;
    std::string status = "Idle";

    void UpdateStatusText()
    {
        // Convert status string to wstring
        std::wstring statusW = L"Status: " + std::wstring(n_shared_packet::status.begin(), n_shared_packet::status.end());
        SetWindowTextW(hStatusText, statusW.c_str());

        // Auto-resize the control
        HDC hdc = GetDC(hStatusText);
        SIZE size;
        GetTextExtentPoint32W(hdc, statusW.c_str(), (int)statusW.length(), &size);
        ReleaseDC(hStatusText, hdc);
        MoveWindow(hStatusText, 150, 25, size.cx + 10, size.cy + 5, TRUE);
    }

    void UpdateConnectionText()
    {
        std::wstring connW = Client::client()->get_instance()->is_connected() ? L"Connection: Connected" : L"Connection: Disconnected";
        SetWindowTextW(hConnectText, connW.c_str());

        // Auto-resize
        HDC hdc = GetDC(hConnectText);
        SIZE size;
        GetTextExtentPoint32W(hdc, connW.c_str(), (int)connW.length(), &size);
        ReleaseDC(hConnectText, hdc);
        MoveWindow(hConnectText, 150, 50, size.cx + 10, size.cy + 5, TRUE);
    }

    int Startup()
    {
        const char CLASS_NAME[] = "NativeClientWindow";

		//configure window class
        WNDCLASS wc = {};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"NativeClientWindow";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClass(&wc);
        //create our gui
        HWND hwnd = CreateWindowEx(
            WS_EX_TOPMOST,
            L"NativeClientWindow",
            L"Client Window",
            WS_OVERLAPPEDWINDOW,
            100, 100, 500, 300,
            nullptr,
            nullptr,
            wc.hInstance,
            nullptr
        );

        if (!hwnd)
            return 1;

		//widgets

        CreateWindow(L"BUTTON", L"Dump me!", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 20, 100, 30, hwnd, (HMENU)1, wc.hInstance, nullptr);

        CreateWindow(L"BUTTON", L"Clear Console", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 60, 100, 30, hwnd, (HMENU)2, wc.hInstance, nullptr);

        std::wstring statusW = L"Status: " + std::wstring(n_shared_packet::status.begin(), n_shared_packet::status.end());
        hStatusText = CreateWindow(
            L"STATIC",
            statusW.c_str(),
            WS_VISIBLE | WS_CHILD,
            150, 25, 300, 20, hwnd, nullptr, wc.hInstance, nullptr
        );

        hConnectText = CreateWindow(
            L"STATIC", L"Connection: ", WS_VISIBLE | WS_CHILD,
            150, 50, 300, 20, hwnd, nullptr, wc.hInstance, nullptr
        );

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
        SetTimer(hwnd, 1, 100, nullptr);
        // Main message loop
        MSG msg = {};
        while (true)
        {
			// Process window messages
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT)
                    return 0;
            }
        }

        return 0;
    }

	// Window procedure to handle messages

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case 1: 
                if (Client::client()->get_instance()->is_connected())
                {
                     n_server_preperation::dumpdata();
                    Client::client()->get_instance()->SendFile(n_shared_packet::packetpath);
                }
                else
                {
                    status = "Operation locked, no endpoint available";
                }
                UpdateStatusText();
                break;

            case 2: 
                system("cls");
                break;
            }
            break;

        case WM_SIZE:
            g_ResizeWidth = LOWORD(lParam);
            g_ResizeHeight = HIWORD(lParam);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_TIMER:
            UpdateStatusText();
            UpdateConnectionText();
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        return 0;
    }
}