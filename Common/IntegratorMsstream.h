#ifndef INTEGRATOR_MSSTREAM_H
#define INTEGRATOR_MSSTREAM_H

#include "Common/BaseMsstream.h"

class msstream : public BaseMsstream
{
public:
  msstream(size_t size = 512) : BaseMsstream(size)
	{ 
	};

  msstream(const char *data) : BaseMsstream(data)
	{ 
	};

  msstream(char *data, size_t size) : BaseMsstream(data, size)
	{ 
	};

  virtual ~msstream()
	{
	};


//  virtual BaseMsstream& operator<<(const char *s);

};


#endif // INTEGRATOR_MSSTREAM_H

