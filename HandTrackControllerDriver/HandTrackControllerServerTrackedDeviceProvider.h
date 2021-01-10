#pragma once
#include <openvr_driver.h>
#include <interface_gesture.hpp>
#include "HandTrackController.h"

class HandTrackControllerServerTrackedDeviceProvider final : public vr::IServerTrackedDeviceProvider
{
public:
	enum HandControllerHand : size_t
	{
		HCH_Left = 0U,
		HCH_Right = 1U,
	};

	/** initializes the driver. This will be called before any other methods are called.
	* If Init returns anything other than VRInitError_None the driver DLL will be unloaded.
	*
	* pDriverHost will never be NULL, and will always be a pointer to a IServerDriverHost interface
	*
	* pchUserDriverConfigDir - The absolute path of the directory where the driver should store user
	*	config files.
	* pchDriverInstallDir - The absolute path of the root directory for the driver.
	*/
	vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);

	/** cleans up the driver right before it is unloaded */
	void Cleanup();

	/** Returns the version of the ITrackedDeviceServerDriver interface used by this driver */
	const char* const* GetInterfaceVersions();

	/** Allows the driver do to some work in the main loop of the server. */
	void RunFrame();


	// ------------  Power State Functions ----------------------- //

	/** Returns true if the driver wants to block Standby mode. */
	bool ShouldBlockStandbyMode();

	/** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
	* state it has. */
	void EnterStandby();

	/** Called when the system is leaving Standby mode. The driver should switch itself back to
	full operation. */
	void LeaveStandby();

	HandTrackControllerServerTrackedDeviceProvider();
	~HandTrackControllerServerTrackedDeviceProvider();

private:
	HandTrackController m_leftcontroller;
	HandTrackController m_rightcontroller;

	int m_last_frameindex;
	bool m_initialized;

	static const char* const ms_interfaces[];
};

