#include "Client.hpp"
#include "EditWindow.hpp"
#include <thread>
#include <time.h>

Client client( "0.0.0.0" );

HINSTANCE hInst;
TCHAR szTitle[] = "Client";
TCHAR szWindowClass[ ] = "ClientClass";

ATOM MyRegisterClass( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

std::string makeMessage( std::string _nickName, std::string _message );
void Thread( EditWindow * _chatWindows, EditWindow * _serverIp, HWND isConnected );

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
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH ) ( COLOR_MENUTEXT );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassEx( &wcex );
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow( szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL );

	RECT winRect;
	GetWindowRect( hWnd, &winRect );
	RECT clientRect;
	GetClientRect( hWnd, &clientRect );
	int sizeOfBorder = winRect.right - clientRect.right - winRect.left;
	int sizeOfMenuBar = winRect.bottom - clientRect.bottom - winRect.top;
	SetWindowPos( hWnd, HWND_TOP, 0, 0, 610 + sizeOfBorder, 597 + sizeOfMenuBar, SWP_NOMOVE );

	if ( !hWnd )
		return FALSE;

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	return TRUE;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static HWND connectToServer, isConnected, sendToServer, clearChatWindow;
	static EditWindow *serverIp, *nickName, *chatWindow, *myMessage;

	switch ( message )
	{
		case WM_COMMAND:
		{
			if ( ( HWND ) lParam == connectToServer )
			{
				if ( client.isClientConnectWithServer() )
					return 0;

				client.reconnect( serverIp->get() );
				client.isClientConnectWithServer() ?
						SendMessage( isConnected, BM_SETCHECK, BST_CHECKED, 0 )
					:	SendMessage( isConnected, BM_SETCHECK, BST_UNCHECKED, 0 );

				if ( !client.isClientConnectWithServer( ) )
					return 0;

				serverIp->setReadOnly( true );
				std::thread thread( Thread, chatWindow, serverIp, isConnected );
				thread.detach();
			}
			else if ( ( HWND ) lParam == sendToServer )
			{
				if ( client.isClientConnectWithServer() )
				{
					client.sendMessage( makeMessage( nickName->get( ), myMessage->get( ) ) );
					myMessage->clear();
				}
			}
			else if ( ( HWND ) lParam == clearChatWindow )
			{
				chatWindow->clear();
			}

		}	break;

		case WM_CREATE:
		{
			serverIp = new EditWindow( hWnd, 5, 5, 150, 16, "192.168.0.100", NULL, 15 );
			connectToServer = CreateWindowA( "button", "Соеденить", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | BS_CENTER, 160, 5, 100, 16, hWnd, NULL, NULL, NULL );
			isConnected = CreateWindowA( "button", "Статус соеденения", BS_CHECKBOX | WS_CHILD | WS_VISIBLE, 265, 5, 150, 16, hWnd, NULL, NULL, NULL );
			client.isClientConnectWithServer() ? SendMessage( isConnected, BM_SETCHECK, BST_CHECKED, 0 ) : SendMessage( isConnected, BM_SETCHECK, BST_UNCHECKED, 0 );

			nickName = new EditWindow( hWnd, 5, 26, 410, 16, "Ник", ES_CENTER );
			clearChatWindow = CreateWindowA( "button", "Очистить чат", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | BS_CENTER, 420, 5, 186, 38, hWnd, NULL, NULL, NULL );

			chatWindow = new EditWindow( hWnd, 5, 47, 600, 500, "", ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL, 2966547 );

			myMessage = new EditWindow( hWnd, 5, 552, 410, 40, "Моё сообщение", ES_MULTILINE, 1024 );
			sendToServer = CreateWindowA( "button", "Отправить", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | BS_CENTER, 420, 552, 185, 40, hWnd, NULL, NULL, NULL );

		}	break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
		
			hdc = BeginPaint( hWnd, &ps );

			EndPaint( hWnd, &ps );

		}	break;

		case WM_DESTROY:

			delete serverIp;
			delete nickName;
			delete chatWindow;
			delete myMessage;

			PostQuitMessage( 0 );
			break;

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

std::string makeMessage( std::string _nickName, std::string _message )
{
	time_t rawTime;
	time( &rawTime );
	tm timeInfo;
	localtime_s( &timeInfo, &rawTime );

	char hour[ 3 ];
	char minute[ 3 ];

	_itoa_s( timeInfo.tm_hour, hour, 10 );
	_itoa_s( timeInfo.tm_min, minute, 10 );

	std::string result("[");
	return result + hour + ":" + minute + "] " + _nickName + ": " + _message;
}

void Thread( EditWindow * _chatWindows, EditWindow * _serverIp, HWND isConnected )
{
	std::string message;
	while ( client.recvMessage( message ) )
		_chatWindows->addLine( message );

	_chatWindows->addLine( "Соединение потерянно. Перезагрузите клиент для установки нового соединения." );
	_serverIp->setReadOnly( false );
	SendMessage( isConnected, BM_SETCHECK, BST_UNCHECKED, 0 );
}