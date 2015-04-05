#include "EditWindow.hpp"
#include <algorithm>

EditWindow::EditWindow( 
	const HWND& _hWndParent
	, const	unsigned _x
	, const	unsigned _y
	, const	unsigned _width
	, const	unsigned _height
	, const	std::string _defaultString
	, const	DWORD _style
	, const	unsigned _maximumCharactersNumber
	)
	: m_maximumCharactersNumber( _maximumCharactersNumber )
{
	m_handle = CreateWindowA( "edit", _defaultString.c_str(), _style | WS_CHILD | WS_VISIBLE, _x, _y, _width, _height, _hWndParent, NULL, NULL, NULL );

	SendMessage( m_handle, EM_LIMITTEXT, m_maximumCharactersNumber, 0 );

	m_textBuffer = new char[ m_maximumCharactersNumber ];
}

EditWindow::~EditWindow()
{
	delete[] m_textBuffer;
}

std::string EditWindow::get()
{
	GetWindowText( m_handle, m_textBuffer, m_maximumCharactersNumber );
	return std::string( m_textBuffer );
}

void EditWindow::set( const std::string _string )
{
	SetWindowText( m_handle, _string.c_str() );
}

void EditWindow::addLine( const std::string _string )
{
	std::string oldText = get();
	if ( oldText.empty() )
		oldText = _string;
	else
		oldText = oldText + "\r\n" + _string;
	set( oldText );

	SendMessage( m_handle, EM_LINESCROLL, 0, SendMessage( m_handle , EM_GETLINECOUNT, 0, 0) );
}

void EditWindow::clear()
{
	SetWindowText( m_handle, "" );
}

void EditWindow::setReadOnly( bool _enableReadOnly )
{
	SendMessage( m_handle, EM_SETREADONLY, _enableReadOnly, 0 );
}