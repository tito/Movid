#ifndef MO_INVERT_MODULE_H
#define MO_INVERT_MODULE_H

#include <string>
#include "moImageFilterModule.h"

class moInvertModule : public moImageFilterModule {
public:
	moInvertModule();
	virtual ~moInvertModule();

protected:
	void applyFilter();

	MODULE_INTERNALS();
};

#endif

