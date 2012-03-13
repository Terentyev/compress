#include "ctar.hpp"

bool ctar::cread_block()
{
	return !m_input.eof();
}

void ctar::cwrite_block()
{
	char ch;
	while ( !m_input.eof() )
	{
		if ( !read( &ch, 1 ) ) break;
		*m_output << ch;
	}
}

bool ctar::dread_block()
{
	return !m_input.eof();
}

void ctar::dwrite_block()
{
	char ch;
	while ( !m_input.eof() )
	{
		if ( !read( &ch, 1 ) ) break;
		*m_output << ch;
	}
}
