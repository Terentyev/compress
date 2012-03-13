#ifndef INCLUDE_TAR_H
#define INCLUDE_TAR_H

#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;

class tar_buf: public streambuf
{
private:
	vector<string> m_files;
	size_t m_file_cur;
	istream *m_stream;
	string m_buffer;
	size_t m_buf_cur;
	char m_char_buffer;

protected:
	int_type underflow();
	void open_file( size_t inc = 1 )
	{
		m_file_cur += inc;
		open_file( m_files[m_file_cur], boost::filesystem::file_size( m_files[m_file_cur] ) );
	};
	void open_file( const string file_name, size_t file_size, istream *stream = NULL );

public:
	explicit tar_buf( const vector<string> &files ): m_files( files ), m_file_cur( 0 ), m_stream( NULL ), m_buf_cur( 0 ), m_char_buffer( '\0' )
	{
		open_file( 0 );
	};
	explicit tar_buf( istream *stream ): m_file_cur( 0 ), m_stream( NULL ), m_buf_cur( 0 ), m_char_buffer( '\0' )
	{
		open_file( "file", (size_t) -1, stream );
	};
};

class tar: private tar_buf, public istream
{
public:
	tar( const vector<string> &files ): tar_buf( files ), istream( this ) {};
	tar( istream *stream ): tar_buf( stream ), istream( this ) {};
};

#endif // !INCLUDE_TAR_H
