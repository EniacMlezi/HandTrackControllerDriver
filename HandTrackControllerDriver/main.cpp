#include <openvr_driver.h>
#include "HandTrackControllerServerTrackedDeviceProvider.h"

using namespace vr;

HandTrackControllerServerTrackedDeviceProvider g_server;

extern "C" __declspec(dllexport)
void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
	if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
	{
		return dynamic_cast<vr::IServerTrackedDeviceProvider*>(&g_server);
	}

	*pReturnCode = VRInitError_Init_InterfaceNotFound;
	return NULL;
}