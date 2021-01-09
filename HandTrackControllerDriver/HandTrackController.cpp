#include "HandTrackController.h"

#pragma region ITrackedDeviceServerDriver
vr::EVRInitError HandTrackController::Activate(uint32_t unObjectId)
{
	vr::VRDriverLog()->Log("HandTrackController::Activate: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::Activate: Exit.");
	return vr::VRInitError_None;
}

void HandTrackController::Deactivate()
{
	vr::VRDriverLog()->Log("HandTrackController::Deactivate: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::Deactivate: Exit.");
}

void HandTrackController::EnterStandby()
{
	vr::VRDriverLog()->Log("HandTrackController::EnterStandby: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::EnterStandby: Exit.");
}

void* HandTrackController::GetComponent(const char* pchComponentNameAndVersion)
{
	vr::VRDriverLog()->Log("HandTrackController::GetComponent: Enter.");
	void* l_result = nullptr;
	if (0 == strcmp(pchComponentNameAndVersion, vr::ITrackedDeviceServerDriver_Version))
	{
		l_result = dynamic_cast<vr::ITrackedDeviceServerDriver*>(this);
	}
	vr::VRDriverLog()->Log("HandTrackController::GetComponent: Exit.");
	return l_result;
}

void HandTrackController::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	vr::VRDriverLog()->Log("HandTrackController::DebugRequest: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::DebugRequest: Exit.");
}

vr::DriverPose_t HandTrackController::GetPose()
{
	vr::VRDriverLog()->Log("HandTrackController::GetPose: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::GetPose: Exit.");
	return m_pose;
}
#pragma endregion

#pragma region HandTrackController
void HandTrackController::UpdatePose()
{
	m_pose.deviceIsConnected = true;
}
#pragma endregion
