#include <iterator>
#include "worker.hpp"

void worker::dump2output( bool should_align )
{
	if ( m_bits_buf.empty() ) return;
	vector<char> out;
	bitset tmp;
	int buf_size = m_bits_buf.size();
	if ( should_align )
	{
		int n = 8 - buf_size % 8;
		for ( int i = 0; i < n; ++i ) tmp.push_back( false );
		for ( int i = 0; i < buf_size; ++i ) tmp.push_back( m_bits_buf[i] );
		m_bits_buf.resize( 0 );
	}
	else
	{
		int n = buf_size % 8;
		for ( int i = n; i < buf_size; ++i ) tmp.push_back( m_bits_buf[i] );
		m_bits_buf.resize( buf_size % 8 );
	}
	boost::to_block_range( tmp, back_inserter( out ) );
	for ( vector<char>::reverse_iterator i = out.rbegin(); i != out.rend(); ++i ) *m_output << *i;
}

size_t worker::read( char *buf, size_t count )
{
	return m_input.read( buf, count );
}

void worker::init( fstream &input, fstream &output )
{
	m_input = reader( &input );
	m_output = &output;
}

void worker::_operate( worker::read_func read, worker::write_func write, bool clean_buf )
{
	while ( 1 )
	{
		m_input.save_point( clean_buf );
		if ( !read( this ) ) break;
		m_input.load_point();
		write( this );
		dump2output( true );
	}
}

void worker::compress()
{
	_operate( &worker::cread_block, &worker::cwrite_block, false );
}

void worker::decompress()
{
	_operate( &worker::dread_block, &worker::dwrite_block, true );
}
