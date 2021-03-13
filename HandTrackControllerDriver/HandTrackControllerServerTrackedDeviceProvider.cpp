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

	m_leftcontroller = new HandTrackController(HandControllerHand::HCH_Left);
	m_rightcontroller = new HandTrackController(HandControllerHand::HCH_Right);
	vr::VRServerDriverHost()->TrackedDeviceAdded("HandTrackLeft", vr::TrackedDeviceClass_Controller, m_leftcontroller);
	vr::VRServerDriverHost()->TrackedDeviceAdded("HandTrackRight", vr::TrackedDeviceClass_Controller, m_rightcontroller);

	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Init: Exit.");
	return vr::VRInitError_None;
}

void HandTrackControllerServerTrackedDeviceProvider::Cleanup()
{
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Cleanup: Enter.");
	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
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
#ifdef TRACE
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::RunFrame: Enter.");
#endif // TRACE

	//TODO: Move this to Init. The TrackedCamera must be set-up before the GestureDetection can start.
	if (!m_initialized)
	{
		GestureOption option;
		option.mode = GestureModeSkeleton;
		option.backend = GestureBackendAuto;
		option.maxFPS = -1;
		GestureFailure result = StartGestureDetection(&option);
		if (result != GestureFailureNone)
		{
			vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Init: Failed to start Hand Tracking.");
			return;
		}
		m_initialized = true;
	}	

	const GestureResult* points = NULL;
	int frameIndex = -1;
	int size = GetGestureResult(&points, &frameIndex);
	if (frameIndex < 0) 
	{
		vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::RunFrame: Hand Tracking failed getting Gesture.");
		return;
	}
	else if (frameIndex == m_last_frameindex)
	{
		//m_leftcontroller->UpdatePose(nullptr);
		//m_rightcontroller->UpdatePose(nullptr);
		return;	//No new gesture yet, don't process.
	}
	m_last_frameindex = frameIndex;

	for (int i = 0; i < size; ++i)
	{
		if (points[i].isLeft)
		{
			m_leftcontroller->UpdatePose(&points[i]);
		}
		else
		{
			m_rightcontroller->UpdatePose(&points[i]);
		}
	}

#ifdef TRACE
	vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::RunFrame: Exit.");
#endif // TRACE
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