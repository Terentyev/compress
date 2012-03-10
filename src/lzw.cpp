#include "lzw.hpp"

bool lzw::cread_block()
{
	return !m_input.eof();
}

void lzw::cwrite_block()
{
	string str;
	char ch;

	read( &ch, 1 );
	str += ch;
	while ( !m_input.eof() )
	{
		read( &ch, 1 );

		if ( m_table.find( str + ch ) != m_table.end() ) str += ch;
		else
		{
			if ( m_table.find( str ) != m_table.end() ) *m_output << (char) m_table[ str ] << '\0';
			else *m_output << '\0' << str;

			m_table.erase( m_rtable[m_next_code] );
			m_table[ str + ch ] = m_next_code;
			m_rtable[m_next_code] = str + ch;
			++m_next_code;
			if ( m_next_code == 0 ) m_next_code = 1;

			str = string() + ch;
		}
	}

	if ( str != "" ) str = str.substr( 0, str.length() - 1 );
	if ( m_table.find( str ) != m_table.end() ) *m_output << (char) m_table[ str ] << '\0';
	else for ( string::iterator i = str.begin(); i != str.end(); ++i ) *m_output << '\0' << *i;
}

bool lzw::dread_block()
{
	return !m_input.eof();
}

void lzw::dwrite_block()
{
	string str, tmp;
	char buf[2];
	const unsigned char *codes = (unsigned char*) buf;

	if ( !read2chars( buf, false ) ) return;

	str += buf[1];

	while ( !m_input.eof() )
	{
		if ( !read2chars( buf, true ) ) break;
		if ( 0 != buf[0] )
		{
			if ( m_rtable.find( buf[0] ) == m_rtable.end() )
			{
				m_next_code = buf[0] + 1;
				m_rtable[ buf[0] ] = str == ""
					? ( tmp + tmp[0] )
					: ( str + str );
			}
			else
			{
				m_rtable[ m_next_code ] = ( str == "" ? tmp : str );
				m_rtable[ m_next_code ] += m_rtable[ buf[0] ][0];
				++m_next_code;
				if ( m_next_code == 0 ) m_next_code = 1;
			}
			*m_output << str << m_rtable[ buf[0] ];
			tmp = m_rtable[ buf[0] ];
			str = "";
		}
		else
		{
			if ( str != "" || tmp != "" )
			{
				if ( str != "" )
				{
					*m_output << str;
				}
				m_rtable[ m_next_code ] = str + tmp + buf[1];
				++m_next_code;
				if ( m_next_code == 0 ) m_next_code = 1;
			}
			str = string() + buf[1];
			tmp = "";
		}
	}

	*m_output << str;
}

bool lzw::read2chars( char *buf, bool silent )
{
	size_t n = read( buf, 2 );
	if ( n < 2 )
	{
		if ( !silent )
		{
			cerr << "Bad input format" << endl;
			exit( 1 );
		}
		return false;
	}
	return true;
}
