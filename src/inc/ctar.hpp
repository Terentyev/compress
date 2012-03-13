#ifndef __CTAR_H_INCLUDE

#define __CTAR_H_INCLUDE

#include "worker.hpp"

class ctar : public worker
{
public:
protected:
	virtual bool cread_block();
	virtual void cwrite_block();
	virtual bool dread_block();
	virtual void dwrite_block();
};

#endif // __CTAR_H_INCLUDE
