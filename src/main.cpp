#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#ifndef _WIN32
#include <execinfo.h>
#include <signal.h>
#else
#endif
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include "compress.hpp"
#include "worker.hpp"
#include "huffman.hpp"
#include "lzw.hpp"
#include "ctar.hpp"
#include "tar.hpp"
#include "untar.hpp"

using namespace std;

void usage(const char *app)
{
	cout << app << " [-a {huffman|lzw}] {-x|-c} [-o <file>] [<input file>[ <input file>...]]" << endl
		<<  "	-a {huffman,lzw,tar} set compress algorithm (default huffman)" << endl
		<<  "	-x                   decompress file" << endl
		<<  "	-c                   compress file" << endl
		<<  "	-o <file>            output file" << endl
		<<  "If output file is not specified out put to STDOUT." << endl;
}

bool error( const char *msg )
{
	cerr << "Error: " << msg << endl;
	return false;
}

bool parseArgs( int argc, char **argv, int &algo, int &operation, vector<string> &inputs, string &output )
{
	int key = -1;
	bool keys_end = false;
	for ( int i = 1; i < argc; ++i )
	{
		if ( key != -1 )
		{
			switch ( key )
			{
				case 0:
					if ( !strcmp( argv[i], ALGO_HUFFMAN_STR ) ) algo = ALGO_HUFFMAN;
					else if ( !strcmp( argv[i], ALGO_LZW_STR ) ) algo = ALGO_LZW;
					else if ( !strcmp( argv[i], ALGO_TAR_STR ) ) algo = ALGO_TAR;
					else return error( "Wrong compress algorithm" );
					break;
				case 1:
					output = argv[i];
					break;
			}
			key = -1;
		}
		else
		{
			if ( !keys_end && 0 == strcmp( argv[i], "-a" ) ) key = 0;
			else if ( !keys_end && 0 == strcmp( argv[i], "-o" ) ) key = 1;
			else if ( !keys_end && ( 0 == strcmp( argv[i], "-x") || 0 == strcmp( argv[i], "-c") ) )
			{
				if ( operation != -1 ) return error( "Wrong defined compress/decompress operation" );
				operation = !strcmp( argv[i], "-x" ) ? OP_DECOMPRESS : OP_COMPRESS;
			}
			else
			{
				keys_end = true;
				if ( !boost::filesystem::exists( argv[i] ) ) return error ( "Can't find input file" );
				inputs.push_back( argv[i] );
			}
		}
	}
	return operation != -1;
}

#ifndef _WIN32
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
#endif

int main( int argc, char **argv )
{    
#ifndef _WIN32
	signal(SIGSEGV, handler);
#endif

	int algo = ALGO_HUFFMAN, op = -1;
	string output;
	vector<string> inputs;

	if ( !parseArgs( argc, argv, algo, op, inputs, output ) )
	{
		usage( argv[0] );
		return 1;
	}

	boost::shared_ptr<worker> w;
	switch ( algo )
	{
		case ALGO_HUFFMAN:
			w = boost::shared_ptr<worker>( new huffman() );
			break;
		case ALGO_LZW:
			w = boost::shared_ptr<worker>( new lzw() );
			break;
		case ALGO_TAR:
			w = boost::shared_ptr<worker>( new ctar() );
			break;
		default:
			return error( "Not implemented" );
	}

	istream *in = &cin;
	if ( OP_COMPRESS == op && !inputs.empty() ) in = new tar( inputs );
	else if ( OP_COMPRESS == op ) in = new tar( &cin );
	else if ( OP_DECOMPRESS == op && inputs.size() == 1 )
	{
		fstream *fin = new fstream();
		fin->open( inputs[0].c_str(), fstream::in );
		in = fin;
	}
	else if ( inputs.size() > 0 ) return error( "Can't define input correctly" );

	ostream *out = &cout;
	if ( OP_COMPRESS == op && "" != output )
	{
		out = new fstream;
		( (fstream*) out )->open( output.c_str(), fstream::binary | fstream::out );
		if ( !( (fstream*) out )->is_open() ) return error( "Can't open output file" );
	}
	else if ( OP_DECOMPRESS == op ) out = new untar( ( "" != output ) ? output : "." );

	w->init( in, out );

	if ( op == OP_COMPRESS ) w->compress();
	else w->decompress();

	if ( in != &cin ) delete in;

	if ( out != &cout ) delete out;

	return 0;
}
