#ifndef __WORKER_H_INCLUDE

#define __WORKER_H_INCLUDE

#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/function.hpp>
#include "compress.hpp"

using namespace std;

typedef boost::dynamic_bitset<unsigned char> bitset;

class worker
{
public:
	typedef boost::function<bool ( worker* )> read_func;
	typedef boost::function<void ( worker* )> write_func;

private:
	bitset m_bits_buf;

	void _operate( worker::read_func read, worker::write_func write );

protected:
	fstream *m_input;
	fstream *m_output;

	virtual bool cread_block() = 0;
	virtual void cwrite_block() = 0;
	virtual bool dread_block() = 0;
	virtual void dwrite_block() = 0;

	void dump2output( const bitset &bits );
	void dump2output( bool should_align = false );
	size_t read( char *buf, size_t count );

public:
	void init( fstream &input, fstream &output );

	void compress();
	void decompress();
};

#endif // __WORKER_H_INCLUDE