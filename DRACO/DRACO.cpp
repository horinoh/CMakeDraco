// DRACO.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DRACO.h"

#include <iostream>
#ifndef SAFE_FCLOSE
#define SAFE_FCLOSE(x) if(nullptr != x) { fclose(x); x = nullptr; }
#endif
FILE* StdOut = nullptr;
FILE* StdErr = nullptr;

#pragma warning (push)
#pragma warning (disable : 4267)
#pragma push_macro("ERROR")
#undef ERROR
#include "draco/compression/decode.h"
#pragma pop_macro("ERROR")
#pragma warning (pop)

#include <fstream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DRACO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRACO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRACO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DRACO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
	{
		setlocale(LC_ALL, "");

		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		SetConsoleTitle(TEXT("DRACO"));
		freopen_s(&StdOut, "CON", "w", stdout);
		freopen_s(&StdErr, "CON", "w", stderr);

		const std::string FilePath("../../draco/javascript/example/models/bunny.drc");
		//!< draco_encoder.exe -i <InputFile> -o <OutputFile> Ç∆ÇµÇƒçÏê¨ÇµÇΩ
		//const std::string FilePath("../../draco/testdata/bun_zipper.ply.drc");
		//const std::string FilePath("../../draco/testdata/mat_test.drc");
		//const std::string FilePath("../../draco/testdata/cube_att.obj.drc");
		//const std::string FilePath("../../draco/testdata/sphere.obj.drc");
		//const std::string FilePath("../../draco/testdata/point_cloud_test_pos.drc");
		std::ifstream InFile(FilePath, std::ios::binary);
		std::string Data;
		Data.assign(std::istreambuf_iterator<char>(InFile), std::istreambuf_iterator<char>());

		draco::DecoderBuffer Buffer;
		Buffer.Init(Data.data(), Data.size());
		const auto StatusOr = draco::Decoder::GetEncodedGeometryType(&Buffer);
		const auto GeomType = StatusOr.value();
		if (draco::INVALID_GEOMETRY_TYPE != GeomType)
		{
			static const auto OutputLimit = 10;

			draco::Decoder Decoder;
			auto StatusOr = Decoder.DecodeMeshFromBuffer(&Buffer);
			const auto Mesh = std::move(StatusOr).value();
			if(nullptr != Mesh) {
				const auto Attr_POSITION = Mesh->GetNamedAttribute(draco::GeometryAttribute::POSITION);
				if (nullptr != Attr_POSITION) {
					std::cout << "---- Positions ----" << std::endl;
					std::array<float, 3> Value;
					for (auto i = 0; i < Attr_POSITION->size(); ++i) {
						if (Attr_POSITION->ConvertValue(static_cast<draco::AttributeValueIndex>(i), Value.data())) {
							if (OutputLimit > i) {
								std::cout << Value[0] << ", " << Value[1] << ", " << Value[2] << std::endl;
							}
							else if (OutputLimit == i) {
								std::cout << "..." << std::endl;
							}
						}
					}
				}

				const auto Attr_NORMAL = Mesh->GetNamedAttribute(draco::GeometryAttribute::NORMAL);
				if (nullptr != Attr_NORMAL) {
					std::cout << "---- Normals ----" << std::endl;
					std::array<float, 3> Value;
					for (auto i = 0; i < Attr_NORMAL->size(); ++i) {
						if (Attr_NORMAL->ConvertValue(static_cast<draco::AttributeValueIndex>(i), Value.data())) {
							if (OutputLimit > i) {
								std::cout << Value[0] << ", " << Value[1] << ", " << Value[2] << std::endl;
							}
							else if (OutputLimit == i) {
								std::cout << "..." << std::endl;
							}
						}
					}
				}

				const auto Attr_COLOR = Mesh->GetNamedAttribute(draco::GeometryAttribute::COLOR);
				if (nullptr != Attr_COLOR) {
					std::cout << "---- Colors ----" << std::endl;
					std::array<float, 4> Value;
					for (auto i = 0; i < Attr_COLOR->size(); ++i) {
						if (Attr_COLOR->ConvertValue(static_cast<draco::AttributeValueIndex>(i), Value.data())) {
							if (OutputLimit > i) {
								std::cout << Value[0] << ", " << Value[1] << ", " << Value[2] << ", " << Value[3] << std::endl;
							}
							else if (OutputLimit == i) {
								std::cout << "..." << std::endl;
							}
						}
					}
				}

				const auto Attr_TEX_COORD = Mesh->GetNamedAttribute(draco::GeometryAttribute::TEX_COORD);
				if (nullptr != Attr_TEX_COORD) {
					std::cout << "---- TexCoord ----" << std::endl;
					std::array<float, 2> Value;
					for (auto i = 0; i < Attr_TEX_COORD->size(); ++i) {
						if (Attr_TEX_COORD->ConvertValue(static_cast<draco::AttributeValueIndex>(i), Value.data())) {
							if (OutputLimit > i) {
								std::cout << Value[0] << ", " << Value[1] << std::endl;
							}
							else if (OutputLimit == i) {
								std::cout << "..." << std::endl;
							}
						}
					}
				}

				std::cout << "---- Faces ----" << std::endl;
				if (draco::TRIANGULAR_MESH == GeomType) {
					for (auto i = 0; i < Mesh->num_faces(); ++i) {
					const auto Face = Mesh->face(static_cast<draco::FaceIndex>(i));
						for (auto j = 0; j < 3; ++j) {
							const auto Index = Attr_POSITION->mapped_index(Face[j]);

							if (OutputLimit > i ) {
								std::cout << Index << ", ";
							}
							else if (OutputLimit == i && 0 == j) {
								std::cout << "..." << std::endl;
							}
						}
						if (i < OutputLimit) {
							std::cout << std::endl;
						}
					}
				}
				else if (draco::POINT_CLOUD == GeomType) {
					//!< ...
				}
			}
		}
	}
		break;
	case WM_KEYDOWN:
		if (VK_ESCAPE == wParam) {
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);

		SAFE_FCLOSE(StdOut);
		SAFE_FCLOSE(StdErr);
		FreeConsole();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
