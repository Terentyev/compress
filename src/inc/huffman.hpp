#ifndef __HUFFMAN_H_INCLUDE

#define __HUFFMAN_H_INCLUDE

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "worker.hpp"

class huffman : public worker
{
public:
	class tree_node
	{
	protected:
		size_t m_encoded_size;

		void _init();
		size_t _calculate_encoded_size( size_t depth = 0 );
		unsigned short _char_frequency( unsigned char ch ) const;

	public:
		typedef boost::shared_ptr<tree_node> ptr;

		ptr left;
		ptr right;
		bool chars[256];
		unsigned frequency;

		tree_node( unsigned char ch, unsigned fr = 1 );
		tree_node( const ptr left, const ptr right );

		static bool compare( ptr a, ptr b );

		bool empty() { return frequency == 0; };
		size_t encoded_size();
		bitset encode( unsigned char ch );
		bool decode( bitset &bits, size_t &idx, char &out );

		friend ostream& operator<<( ostream &stream, const tree_node &node );
	};
	typedef vector<tree_node::ptr> nodes;

protected:
	tree_node::ptr m_root;

	virtual bool cread_block();
	virtual void cwrite_block();
	virtual bool dread_block();
	virtual void dwrite_block();

	void build_tree( nodes &nodes );

public:
};

#endif // __HUFFMAN_H_INCLUDE
