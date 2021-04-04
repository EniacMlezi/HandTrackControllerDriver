#pragma once

#include <openvr_driver.h>
#include <string.h>

#if defined NDEBUG
#define HT_TRACE( log )
#else
#define HT_TRACE( log )												\
	{																\
	std::stringstream ht_traceSS;									\
	ht_traceSS << __FILE__ << "::" << __func__ << ": " << log;	\
	vr::VRDriverLog()->Log(ht_traceSS.str().c_str());				\
	}
#endif