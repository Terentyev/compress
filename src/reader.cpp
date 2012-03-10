#include "reader.hpp"
#include <cstring>

void reader::save_point( bool clean )
{
	if ( m_buf_cur < m_buffer.size() && !clean )
	{
		m_buffer = m_buffer.substr( m_buf_cur, m_buffer.size() - m_buf_cur );
	}
	else
	{
		m_buffer = "";
	}
	m_buf_cur = 0;
	m_need_save = true;
}

void reader::load_point()
{
	m_buf_cur = 0;
	m_need_save = false;
}

size_t reader::read( char *buf, size_t count )
{
	size_t n = 0, n1 = 0;
	if ( m_buf_cur < m_buffer.size() )
	{
		n = min( count, m_buffer.size() - m_buf_cur );
		memcpy( buf, m_buffer.c_str() + m_buf_cur, n );
		m_buf_cur += n;
		count -= n;
	}

	if ( count > 0 )
	{
		m_stream->read( buf + n, count );
		n1 = m_stream->gcount();
	}
	
	if ( m_need_save && n1 > 0 )
	{
		m_buffer += string( buf + n, n1 );
		m_buf_cur = m_buffer.size();
	}

	return n + n1;
}

bool reader::eof()
{
	return ( m_stream->eof() ) && ( m_need_save || m_buf_cur >= m_buffer.size() );
}
