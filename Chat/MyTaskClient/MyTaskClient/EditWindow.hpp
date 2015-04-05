#ifndef _EDIT_WINDOW_HPP_
#define _EDIT_WINDOW_HPP_

#include <Windows.h>
#include <string>

class EditWindow
{

public:

	EditWindow(
		const HWND& _hWndParent
		, const unsigned _x
		, const unsigned _y
		, const unsigned _width
		, const unsigned _height
		, const std::string _defaultString = ""
		, const DWORD _style = NULL
		, const unsigned _maximumCharactersNumber = 1024
		);

	~EditWindow();

	std::string get();

	void set( const std::string _string );
	void addLine( const std::string _string );

	void setReadOnly( bool _enableReadOnly );

	void clear();

private:

	HWND m_handle;
	char * m_textBuffer;
	const unsigned int m_maximumCharactersNumber;

};

#endif _EDIT_WINDOW_HPP_