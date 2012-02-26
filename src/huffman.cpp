#include <iostream>
#include "huffman.hpp"

void print( const huffman::tree_node::ptr node, int s = 0 )
{
	for ( int i = 0; i < s; ++i ) cerr << " ";
	cerr << "'";
	for ( int i = 0; i < BUF_LEN; ++ i ) if ( node->chars[i] ) cerr << (char) i;
	cerr << "' " << node->frequency << endl;
	if ( node->left != NULL )
	{
		for ( int i = 0; i < s; ++i ) cerr << " ";
		cerr << "  left:" << endl;
		print( node->left, s + 4 );
	}
	if ( node->right != NULL )
	{
		for ( int i = 0; i < s; ++i ) cerr << " ";
		cerr << "  right:" << endl;
		print( node->right, s + 4 );
	}
	cerr << endl;
}

void huffman::build_tree( huffman::nodes &nodes )
{
	while ( nodes.size() > 1 )
	{
		nodes::iterator i1 = nodes.end();
		nodes::iterator i2 = nodes.end();
		for ( nodes::iterator i = nodes.begin(); i != nodes.end(); ++i )
		{
			if ( i1 == nodes.end() || (*i)->frequency <= (*i1)->frequency ) {
				i2 = i1;
				i1 = i;
			}
			else if ( i2 == nodes.end() || (*i)->frequency <= (*i2)->frequency ) i2 = i;
		}
		tree_node::ptr n1 = *i1;
		nodes.erase( i1 );
		tree_node::ptr n2 = *i2;
		nodes.erase( i2 );
		nodes.push_back( tree_node::ptr( new tree_node( n2, n1 ) ) );
	}

	m_root = nodes.back();
//	print( m_root );
}

bool huffman::cread_block()
{
	if ( m_input->eof() ) return false;

	nodes frequency( BUF_LEN );
	bool br = false;
	while ( !m_input->eof() && !br )
	{
		char chars[BUF_LEN];
		size_t i = 0;
		size_t n = read( chars, BUF_LEN );
		while ( i < n && !br )
		{
			unsigned char ch = (unsigned char) chars[i++];
			if ( frequency[ch] == NULL ) frequency[ch] = tree_node::ptr( new tree_node( ch ) );
			else if ( frequency[ch]->frequency == BUF_LEN - 1 ) br = true;
			else ++frequency[ch]->frequency;
		}
	}

	nodes::iterator fit = frequency.begin();
	while ( fit != frequency.end() )
	{
		if ( *fit == NULL ) fit = frequency.erase( fit );
		else ++fit;
	}

	if ( frequency.size() == 0 ) return false;

	build_tree( frequency );

	return true;
}

void huffman::cwrite_block()
{
	*m_output << *m_root << '\0' << '\0';
	vector<bitset> codes( BUF_LEN );

	while ( !m_input->eof() && !m_root->empty() )
	{
		char chars[BUF_LEN];
		size_t i = 0;
		size_t n = read( chars, min( (size_t) m_root->frequency, (size_t) BUF_LEN ) );
		while ( i < n )
		{
			unsigned char ch = (unsigned char) chars[i++];
			if ( codes[ch].size() == 0 ) codes[ch] = m_root->encode( ch );
			else --m_root->frequency;
			bitset tmp = codes[ch];
			for ( int j = 0; j < m_bits_buf.size(); ++j ) tmp.push_back( m_bits_buf[j] );
			m_bits_buf = tmp;
			dump2output();
		}
	}
}

bool huffman::dread_block()
{
	return !m_input->eof() && !m_input->fail();
}

void huffman::dwrite_block()
{
	size_t n;
	char entry[2];
	nodes nodes;

	do
	{
		n = read( entry, 2 );
		if ( n < 2 && n > 0 )
		{
			cerr << "Unexpected EOF: " << entry[0] << endl;
			exit( 1 );
		}

		if ( entry[0] == 0 && entry[1] == 0 ) break;
		nodes.push_back( tree_node::ptr( new tree_node( (unsigned char) entry[0], (unsigned char) entry[1] ) ) );
	} while ( !m_input->eof() );

	if ( nodes.size() == 0 )
	{
		return;
		cerr << "Bad frequency table" << endl;
		exit( 1 );
	}

	build_tree( nodes );
	if ( m_root->empty() ) return;

	char chars[BUF_LEN];
	bitset bits;
	size_t need = m_root->encoded_size() / 8 + ( m_root->encoded_size() % 8 ? 1 : 0 );
	while ( !m_input->eof() && need > 0 )
	{
		n = read( chars, min( need, (size_t) BUF_LEN ) );
		need -= n;
		bitset tmp;
		while ( n-- > 0 ) tmp.append( chars[n] );
		for ( size_t i = 0; i < bits.size(); ++i ) tmp.push_back( bits[i] );
		bits = tmp;
		size_t i = bits.size();
		while ( !m_root->empty() )
		{
			char tmp;
			if ( m_root->decode( bits, i, tmp ) )
			{
				*m_output << tmp;
				continue;
			}
			bits.resize( i );
			break;
		}
	}
}

huffman::tree_node::tree_node( unsigned char ch, unsigned fr )
{
	this->_init();
	chars[ch] = true;
	frequency = fr;
}

huffman::tree_node::tree_node( const huffman::tree_node::ptr left, const huffman::tree_node::ptr right )
{
	this->_init();
	for ( int i = 0; i < BUF_LEN; ++i ) this->chars[i] = left->chars[i] || right->chars[i];
	this->frequency = left->frequency + right->frequency;
	this->left = left;
	this->right = right;
}

bool huffman::tree_node::compare( huffman::tree_node::ptr a, huffman::tree_node::ptr b )
{
	return a->frequency > b->frequency;
}

void huffman::tree_node::_init()
{
	for ( int i = 0; i < BUF_LEN; ++i ) this->chars[i] = false;
	frequency = 0;
	m_encoded_size = 0;
}

size_t huffman::tree_node::encoded_size()
{
	if ( !empty() && m_encoded_size == 0 ) m_encoded_size = _calculate_encoded_size();
	return m_encoded_size;
}

size_t huffman::tree_node::_calculate_encoded_size( size_t depth )
{
	if ( left != NULL ) return left->_calculate_encoded_size( depth + 1 ) + right->_calculate_encoded_size( depth + 1 );
	return depth * (size_t) frequency;
}

unsigned char huffman::tree_node::_char_frequency( unsigned char ch ) const
{
	if ( left == NULL ) return frequency;
	if ( left->chars[ch] ) return left->_char_frequency( ch );
	return right->_char_frequency( ch );
}

bitset huffman::tree_node::encode( unsigned char ch )
{
	bitset tmp;

	--frequency;
	if ( left != NULL && left->chars[ch] )
	{
		tmp = left->encode( ch );
		tmp.push_back( false );
	}
	else if ( right != NULL )
	{
		tmp = right->encode( ch );
		tmp.push_back( true );
	}

	return tmp;
}

bool huffman::tree_node::decode( bitset &bits, size_t &idx, char &out )
{
	if ( idx == 0 && left != NULL && right != NULL ) return false;

	--frequency;
	tree_node::ptr node;
	if ( left != NULL && !bits[idx - 1] ) node = left;
	if ( right != NULL && bits[idx - 1] ) node = right;

	if ( node != NULL )
	{
		if ( !node->decode( bits, --idx, out ) )
		{
			++idx;
			++frequency;
			return false;
		}
		else return true;
	}

	for ( int i = 0; i < BUF_LEN; ++i ) if ( chars[i] )
	{
		out = (char) i;
		return true;
	}

	cerr << "Not found code in Huffman-tree" << endl;
	exit( 1 );
}

ostream& operator<<( ostream &stream, const huffman::tree_node &node )
{
	for ( short i = 0; i < BUF_LEN; ++i )
	{
		if ( !node.chars[i] ) continue;
		stream << (char) i << (char) node._char_frequency( (unsigned char) i );
	}
	return stream;
}
