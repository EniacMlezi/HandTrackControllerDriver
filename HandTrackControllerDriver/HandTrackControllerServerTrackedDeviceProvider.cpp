#include "HandTrackControllerServerTrackedDeviceProvider.h"

const char* const HandTrackControllerServerTrackedDeviceProvider::ms_interfaces[]
{
	vr::ITrackedDeviceServerDriver_Version,
	vr::IServerTrackedDeviceProvider_Version,
	nullptr
};

HandTrackControllerServerTrackedDeviceProvider::HandTrackControllerServerTrackedDeviceProvider()
{
}

HandTrackControllerServerTrackedDeviceProvider::~HandTrackControllerServerTrackedDeviceProvider()
{
}

vr::EVRInitError HandTrackControllerServerTrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Init: Enter.");

	vr::VRServerDriverHost()->TrackedDeviceAdded("HandTrackLeft", vr::TrackedDeviceClass_Controller, &m_leftcontroller);
	vr::VRServerDriverHost()->TrackedDeviceAdded("HandTrackRight", vr::TrackedDeviceClass_Controller, &m_rightcontroller);

	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Init: Exit.");
	return vr::VRInitError_None;
}

void HandTrackControllerServerTrackedDeviceProvider::Cleanup()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Cleanup: Enter.");
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Cleanup: Exit.");
}

const char* const* HandTrackControllerServerTrackedDeviceProvider::GetInterfaceVersions()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::GetInterfaceVersions: Enter.");
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::GetInterfaceVersions: Exit.");
	return ms_interfaces;
}

void HandTrackControllerServerTrackedDeviceProvider::RunFrame()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::RunFrame: Enter.");
	m_leftcontroller.UpdatePose();
	m_rightcontroller.UpdatePose();
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::RunFrame: Exit.");
}

bool HandTrackControllerServerTrackedDeviceProvider::ShouldBlockStandbyMode()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::ShouldBlockStandbyMode: Enter.");
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::ShouldBlockStandbyMode: Exit.");
	return false;
}

void HandTrackControllerServerTrackedDeviceProvider::EnterStandby()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::EnterStandby: Enter.");
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::EnterStandby: Exit.");
}

void HandTrackControllerServerTrackedDeviceProvider::LeaveStandby()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::LeaveStandby: Enter.");
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::LeaveStandby: EXit.");
}