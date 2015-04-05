#include "Server.hpp"
#include <algorithm>
#include <thread>

Server server;

HWND hWnd;
HINSTANCE hInst;
TCHAR szTitle[] = "Black list";
TCHAR szWindowClass[ ] = "ServerClass";

ATOM MyRegisterClass( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
void Thread();

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;

	MyRegisterClass( hInstance );

	if ( !InitInstance( hInstance, nCmdShow ) )
		return FALSE;

	while ( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return ( int ) msg.wParam;
}

ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = NULL;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( NULL, IDI_HAND );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassEx( &wcex );
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	hInst = hInstance;

	hWnd = CreateWindowA( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL );

	if ( !hWnd )
		return FALSE;

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	return TRUE;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static std::map< HWND, std::string > connectionsLog;

	switch ( message )
	{
		case WM_CREATE:
		{
			server.isCorrectConnection() ?
					SetWindowText( hWnd, "Server is running" )
				:	SetWindowText( hWnd, "SERVER ERROR. FIX AND RELOAD PLS." );

			if ( server.isCorrectConnection() )
			{
				std::thread thread( Thread );
				thread.detach( );
			}

		}	break;

		case WM_COMMAND:
		{
			auto it = std::find_if( connectionsLog.begin(), connectionsLog.end(), [&] ( std::pair< HWND, std::string > logPair )
			{
				return logPair.first == ( HWND ) lParam;
			} );

			if ( it != connectionsLog.end() )
			{
				LRESULT checkBoxState = SendMessage( it->first, BM_GETCHECK, 0, 0 );
				if ( checkBoxState == BST_CHECKED )
					server.blackListAdd( it->second );
				else if ( checkBoxState == BST_UNCHECKED )
					server.blackListErase( it->second );
			}

		}	break;

		case WM_PAINT:
		{
			static PAINTSTRUCT ps;
			static HDC hdc;

			static int vCheckBoxPos = 0, hCheckBoxPos = 0;

			std::for_each( server.connectionsBegin( ), server.connectionsEnd( ), [&] ( std::pair< SOCKET, std::string > connectionPair )
			{
				auto it = std::find_if( connectionsLog.begin(), connectionsLog.end(), [&] ( std::pair< HWND, std::string > logPair )
				{
					return logPair.second == connectionPair.second;
				} );

				if ( it == connectionsLog.end() )
				{
					HWND newCheckBox =
						CreateWindow( "button", connectionPair.second.c_str(), BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
						hCheckBoxPos, vCheckBoxPos, 200, 20, hWnd, NULL, NULL, NULL );

					connectionsLog.insert( std::make_pair( newCheckBox, connectionPair.second ) );

					vCheckBoxPos += 25;
					if ( vCheckBoxPos > 800 )
					{
						vCheckBoxPos = 0;
						hCheckBoxPos += 220;
					}
				}
			} );

			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );

		}	break;

		case WM_DESTROY:
		{
			
			PostQuitMessage( 0 );

		}	break;

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

void Thread()
{
	if ( server.acceptNewSocket() )
	{
		InvalidateRect( hWnd, NULL, FALSE );
		std::thread thread( Thread );
		thread.detach();
	}
	else
		SetWindowText( hWnd, "SERVER ERROR. FIX AND RELOAD PLS." );
}