#include "tar.hpp"
#include <fstream>
#include <boost/filesystem.hpp>

tar_buf::int_type tar_buf::underflow()
{
	if ( gptr() == NULL || gptr() >= egptr() )
	{
		if ( m_buf_cur >= m_buffer.size() )
		{
			if ( m_stream->eof() || !m_stream->read( &m_char_buffer, 1 ) )
			{
				if ( m_files.size() <= ( m_file_cur + 1 ) ) return traits_type::eof();
				open_file();
				m_char_buffer = m_buffer[m_buf_cur++];
			}
		 }
		else m_char_buffer = m_buffer[m_buf_cur++];
		setg( &m_char_buffer, &m_char_buffer, &m_char_buffer + 1 );
	}

	return traits_type::not_eof( m_char_buffer );
}

void tar_buf::open_file( const string file_name, size_t file_size, istream *stream )
{
	fstream *file_stream;
	if ( m_stream != NULL )
	{
		file_stream = (fstream*) m_stream;
		if ( file_stream->is_open() )
		{
			file_stream->close();
			file_stream->clear();
		}
		delete m_stream;
	}
	if ( stream != NULL ) m_stream = stream;
	else
	{
		file_stream = new fstream();
		file_stream->open( file_name.c_str(), fstream::in | fstream::binary );
		m_stream = file_stream;
	}
	m_buffer = file_name;
	m_buffer += '\0';
	m_buffer += string( (char*) &file_size, sizeof( file_size ) );
	m_buf_cur = 0;
}
