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
	HT_TRACE("Enter.");

	m_leftcontroller = new HandTrackController(HandControllerHand::HCH_Left);
	m_rightcontroller = new HandTrackController(HandControllerHand::HCH_Right);
	vr::VRServerDriverHost()->TrackedDeviceAdded("HandTrackLeft", vr::TrackedDeviceClass_Controller, m_leftcontroller);
	vr::VRServerDriverHost()->TrackedDeviceAdded("HandTrackRight", vr::TrackedDeviceClass_Controller, m_rightcontroller);
	
	vr::ETrackedPropertyError l_error;
	std::string installPath = vr::VRProperties()->GetStringProperty(pDriverContext->GetDriverHandle(), vr::Prop_InstallPath_String, &l_error);

	std::stringstream pathStream;
	pathStream << installPath << "\\bin\\";
#if defined(_WIN64)
	pathStream << "win64";
#elif defined(_WIN32)
	pathStream << "win32";
#else
#error Only WIN64 or WIN32 supported.
#endif
	pathStream << "\\HandTrackControllerMonitor.exe";

	STARTUPINFO l_info = { sizeof(l_info) };
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	if (!CreateProcess(converter.from_bytes(pathStream.str()).c_str(), NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &l_info, &m_monitorProcessInfo))
	{
		vr::VRDriverLog()->Log("HandTrackControllerServerTrackedDeviceProvider::Init: Failed starting HandTrackMonitor.");
		return vr::VRInitError_Init_Internal;
	}

	HT_TRACE("Exit.");
	return vr::VRInitError_None;
}

void HandTrackControllerServerTrackedDeviceProvider::Cleanup()
{
	WaitForSingleObject(m_monitorProcessInfo.hProcess, INFINITE);
	CloseHandle(m_monitorProcessInfo.hProcess);
	CloseHandle(m_monitorProcessInfo.hThread);
	
	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* HandTrackControllerServerTrackedDeviceProvider::GetInterfaceVersions()
{
	HT_TRACE("Enter.");
	HT_TRACE("Exit.");
	return ms_interfaces;
}

void HandTrackControllerServerTrackedDeviceProvider::RunFrame()
{
	HT_TRACE("Enter.");

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
		m_leftcontroller->UpdatePose(nullptr);
		m_rightcontroller->UpdatePose(nullptr);
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

	HT_TRACE("Exit.");
}

bool HandTrackControllerServerTrackedDeviceProvider::ShouldBlockStandbyMode()
{
	HT_TRACE("Enter.");
	HT_TRACE("Exit.");
	return false;
}

void HandTrackControllerServerTrackedDeviceProvider::EnterStandby()
{
	HT_TRACE("Enter.");
	HT_TRACE("Exit.");
}

void HandTrackControllerServerTrackedDeviceProvider::LeaveStandby()
{
	HT_TRACE("Enter.");
	HT_TRACE("EXit.");
}