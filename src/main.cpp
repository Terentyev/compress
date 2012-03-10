#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <execinfo.h>
#include <signal.h>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include "compress.hpp"
#include "worker.hpp"
#include "huffman.hpp"
#include "lzw.hpp"

using namespace std;

void usage(const char *app)
{
	cout << app << " [-a {huffman|lzw}] {-x|-c} -i <file> -o <file>" << endl
		<<  "	-a {huffman,lzw}     set compress algorithm (default huffman)" << endl
		<<  "	-x                   decompress file" << endl
		<<  "	-c                   compress file" << endl
		<<  "	-i <file>            input file" << endl
		<<  "	-o <file>            output file" << endl;
}

bool error( const char *msg )
{
	cerr << "Error: " << msg << endl;
	return false;
}

bool parseArgs( int argc, char **argv, int &algo, int &operation, fstream &input, fstream &output )
{
	int key = -1;
	for ( int i = 1; i < argc; ++i )
	{
		if ( key != -1 )
		{
			switch ( key )
			{
				case 0:
					if ( !strcmp( argv[i], ALGO_HUFFMAN_STR ) ) algo = ALGO_HUFFMAN;
					else if ( !strcmp( argv[i], ALGO_LZW_STR ) ) algo = ALGO_LZW;
					else return error( "Wrong compress algorithm" );
					break;
				case 1:
					if ( !boost::filesystem::exists( argv[i] ) ) return error ( "Can't find input file" );
					input.open( argv[i], fstream::binary | fstream::in );
					break;
				case 2:
					output.open( argv[i], fstream::binary | fstream::out );
					if ( !output.is_open() ) return error( "Can't open output file" );
					break;
			}
			key = -1;
		}
		else
		{
			if ( !strcmp( argv[i], "-a" ) ) key = 0;
			else if ( !strcmp( argv[i], "-i" ) ) key = 1;
			else if ( !strcmp( argv[i], "-o" ) ) key = 2;
			else if ( !strcmp( argv[i], "-x") || !strcmp( argv[i], "-c") )
			{
				if ( operation != -1 ) return error( "Wrong defined compress/decompress operation" );
				operation = !strcmp( argv[i], "-x" ) ? OP_DECOMPRESS : OP_COMPRESS;
			}
		}
	}
	return input.is_open() && output.is_open() && operation != -1;
}

void handler( int sig )
{
#define ARRAY_LEN 10
	void *array[ARRAY_LEN];
	size_t size;

	size = backtrace( array, ARRAY_LEN );
	fprintf( stderr, "Error: signal %d\n", sig );
	backtrace_symbols_fd( array, size, 2 );
	exit( 1 );
}

int main( int argc, char **argv )
{
	signal(SIGSEGV, handler);

	int algo = ALGO_HUFFMAN, op = -1;
	fstream input, output;

	if ( !parseArgs( argc, argv, algo, op, input, output ) )
	{
		usage( argv[0] );
		return 1;
	}

	boost::shared_ptr<worker> w;
	switch ( algo )
	{
		case ALGO_HUFFMAN:
			w = boost::shared_ptr<worker>(new huffman());
			break;
		case ALGO_LZW:
			w = boost::shared_ptr<worker>(new lzw());
			break;
		default:
			error( "Not implemented" );
			return 1;
	}
	w->init( input, output );
	if ( op == OP_COMPRESS ) w->compress();
	else w->decompress();
	input.close();
	output.close();

	return 0;
}
