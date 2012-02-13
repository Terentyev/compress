#ifndef __LWZ_H_INCLUDE

#define __LWZ_H_INCLUDE

#include "worker.hpp"

class lwz : public worker
{
protected:
	bool readblock();
	void writeblock();
};

#endif // __LWZ_H_INCLUDE
