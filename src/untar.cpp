#include "untar.hpp"


untar_buf::int_type untar_buf::overflow( untar_buf::int_type c )
{
	if ( traits_type::eq_int_type( c, traits_type::eof() ) )
	{
		return ( sync() == 0 ? traits_type::not_eof( c ) : traits_type::eof() );
	}

	char ch = traits_type::to_char_type( c );

	if ( !m_stream.is_open() && !m_get_size )
	{
		if ( ch == '\0' )
		{
			boost::filesystem::path path = m_path / m_buffer;
			boost::filesystem::create_directories( path.branch_path() );
			m_stream.open( path.string().c_str(), fstream::out );
			m_get_size = true;
			m_buffer = "";
			return traits_type::not_eof( c );
		}
		else m_buffer += ch;
	}

	if ( m_get_size )
	{
		m_buffer += ch;
		if ( m_buffer.size() >= sizeof( m_file_size ) )
		{
			m_get_size = false;
			m_file_size = *( (size_t*) (void*) m_buffer.c_str() );
			m_buffer = "";
			if ( 0 == m_file_size )
			{
				m_stream.close();
				m_stream.clear();
			}
		}
	}
	else
	{
		m_stream << ch;
		if ( 0 == --m_file_size )
		{
			m_stream.close();
			m_stream.clear();
		}
	}

	return traits_type::not_eof( c );
}
