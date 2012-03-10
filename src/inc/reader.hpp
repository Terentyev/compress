#ifndef INCLUDE_READER_H
#define INCLUDE_READER_H

#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include "compress.hpp"

using namespace std;

class reader
{
private:
	istream *m_stream;
	string m_buffer;
	size_t m_buf_cur;
	bool m_need_save;

public:
	reader() {};
	reader( istream *stream ): m_stream( stream ) {};

	void save_point( bool clean );
	void load_point();
	size_t read( char *buf, size_t count );
	bool eof();
};

#endif // !INCLUDE_READER_H
