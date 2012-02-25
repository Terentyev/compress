#ifndef __LWZ_H_INCLUDE

#define __LWZ_H_INCLUDE

#include <string>
#include <map>
#include "worker.hpp"

class lwz : public worker
{
private:
	unsigned char m_next_code;
	map<string, unsigned char> m_table;
	map<unsigned char, string> m_rtable;

	bool read2chars( char *buf, bool silent );

protected:
	virtual bool cread_block();
	virtual void cwrite_block();
	virtual bool dread_block();
	virtual void dwrite_block();

public:
	lwz() : m_next_code( 1 ) {};
};

#endif // __LWZ_H_INCLUDE
