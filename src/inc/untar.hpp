#ifndef INCLUDE_UNTAR_H
#define INCLUDE_UNTAR_H

#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;

class untar_buf: public streambuf
{
private:
	fstream m_stream;
	boost::filesystem::path m_path;
	string m_buffer;
	bool m_get_size;
	size_t m_file_size;

protected:
	int_type overflow( int_type c );

public:
	explicit untar_buf( const string &dest ): m_path( dest ) {};
};

class untar: private untar_buf, public ostream
{
private:
	ostream m_stream;
public:
	untar( const string &dest ): untar_buf( dest ), ostream( this ) {};
};

#endif // INCLUDE_UNTAR_H
