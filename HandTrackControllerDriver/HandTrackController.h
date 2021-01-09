#pragma once

#include <openvr_driver.h>

class HandTrackController final : public vr::ITrackedDeviceServerDriver
{
public:
#pragma region ITrackedDeviceServerDriver
	// ------------------------------------
	// Management Methods
	// ------------------------------------
	/** This is called before an HMD is returned to the application. It will always be
	* called before any display or tracking methods. Memory and processor use by the
	* ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
	* The pose listener is guaranteed to be valid until Deactivate is called, but
	* should not be used after that point. */
	vr::EVRInitError Activate(uint32_t unObjectId);

	/** This is called when The VR system is switching from this Hmd being the active display
	* to another Hmd being the active display. The driver should clean whatever memory
	* and thread use it can when it is deactivated */
	void Deactivate();

	/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
	void EnterStandby();

	/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
	* if the requested interface or version is not supported. */
	void* GetComponent(const char* pchComponentNameAndVersion);

	/** A VR Client has made this debug request of the driver. The set of valid requests is entirely
	* up to the driver and the client to figure out, as is the format of the response. Responses that
	* exceed the length of the supplied buffer should be truncated and null terminated */
	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	// ------------------------------------
	// Tracking Methods
	// ------------------------------------
	vr::DriverPose_t GetPose();
#pragma endregion

#pragma region HandTrackController
	void UpdatePose();
#pragma endregion

private:
	vr::DriverPose_t m_pose;
};

