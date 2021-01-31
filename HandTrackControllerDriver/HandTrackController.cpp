#include "HandTrackController.h"

#pragma region ITrackedDeviceServerDriver
vr::EVRInitError HandTrackController::Activate(uint32_t unObjectId)
{
	vr::VRDriverLog()->Log("HandTrackController::Activate: Enter.");
	vr::EVRInitError l_resultError = vr::VRInitError_Driver_Failed;
	if (m_trackedDeviceObjectId == vr::k_unTrackedDeviceIndexInvalid)
	{
		m_trackedDeviceObjectId = unObjectId;
		m_propertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_trackedDeviceObjectId);

		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, "handtrack_controller");
		vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);

		switch (m_hand)
		{
		case HandControllerHand::HCH_Left:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "oculus_rifts_controller_left");
			break;
		case HandControllerHand::HCH_Right:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
			vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "oculus_rifts_controller_right");
			break;
		default:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_OptOut);
			vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "generic_controller");
		}

		// Set controller profile
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_InputProfilePath_String, "{handtrack}/input/handtrack_controller_bindings.json");

		std::string controller_handedness_str = m_hand == HandControllerHand::HCH_Left  ? "left" : "right";
		std::string controller_ready_file = "{handtrack}/icons/controller_ready_" + controller_handedness_str + ".png";
		std::string controller_not_ready_file = "{handtrack}/icons/controller_not_ready_" + controller_handedness_str + ".png";
		const char* controller_ready_file_cstr = controller_ready_file.c_str();
		const char* controller_not_ready_file_cstr = controller_not_ready_file.c_str();

		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceOff_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, controller_not_ready_file_cstr); 
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandbyAlert_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, controller_not_ready_file_cstr);
		
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReady_String, controller_ready_file_cstr);

		l_resultError = vr::VRInitError_None;
	}
	vr::VRDriverLog()->Log("HandTrackController::Activate: Exit.");
	return l_resultError;
}

void HandTrackController::Deactivate()
{
	vr::VRDriverLog()->Log("HandTrackController::Deactivate: Enter.");
	m_trackedDeviceObjectId = vr::k_unTrackedDeviceIndexInvalid;
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
	return m_last_pose;
}
#pragma endregion

#pragma region HandTrackController
HandTrackController::HandTrackController(HandControllerHand hand)
:	m_hand(hand),
	m_vecPosition { 0, 0, 0 },
	m_qRotation { 1, 0, 0, 0 },
	m_trackedDeviceObjectId(vr::k_unTrackedDeviceIndexInvalid),
	m_propertyContainer(vr::k_ulInvalidPropertyContainer),
	m_last_pose({0})
{
}

void HandTrackController::UpdatePose(const GestureResult *gesture)
{
	vr::DriverPose_t l_pose = { 0 };

	l_pose.deviceIsConnected = true;
	if (nullptr == gesture)
	{
		l_pose.poseIsValid = false;
		l_pose.result = vr::TrackingResult_Running_OutOfRange;
	}
	else
	{
		l_pose.poseIsValid = true;
		l_pose.result = vr::TrackingResult_Running_OK;

		l_pose.qDriverFromHeadRotation.w = 1;
		l_pose.qWorldFromDriverRotation.w = 1;

		if (m_hand == HandControllerHand::HCH_Left)
		{
			UpdateLeftHand();
		}

		l_pose.vecWorldFromDriverTranslation[0] = 0.302428544;
		l_pose.vecWorldFromDriverTranslation[1] = -1.25425005;
		l_pose.vecWorldFromDriverTranslation[2] = -1.22407806;

		l_pose.qWorldFromDriverRotation.w = -0.383;
		l_pose.qWorldFromDriverRotation.x = 0;
		l_pose.qWorldFromDriverRotation.y = -0.924;
		l_pose.qWorldFromDriverRotation.z = 0;

		/*l_pose.vecPosition[0] = m_vecPosition[0];
		l_pose.vecPosition[1] = m_vecPosition[1];
		l_pose.vecPosition[2] = m_vecPosition[2];*/

		l_pose.vecPosition[0] = gesture->points[0];
		l_pose.vecPosition[1] = gesture->points[1];
		l_pose.vecPosition[2] = -gesture->points[2];
		
		l_pose.qRotation.w = 1;
		l_pose.qRotation.x = 0;
		l_pose.qRotation.y = 0;
		l_pose.qRotation.z = 0;

		/*std::string controller_handedness_str = m_hand == HandControllerHand::HCH_Left ? "left" : "right";
		std::string gesturePoseMsg = "HandTrackController::UpdatePose: " + controller_handedness_str + " [" + std::to_string(gesture->points[0]) + "] [" + std::to_string(gesture->points[1]) + "] [" + std::to_string(gesture->points[2]) + "]";
		vr::VRDriverLog()->Log(gesturePoseMsg.c_str());*/
	}

	vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDeviceObjectId, l_pose, sizeof(vr::DriverPose_t));
	m_last_pose = l_pose;
}

void HandTrackController::UpdateLeftHand()
{
	//if ((GetAsyncKeyState(70) & 0x8000) != 0) { //F
	//	cyaw += 0.1;
	//}
	//if ((GetAsyncKeyState(72) & 0x8000) != 0) { //H
	//	cyaw += -0.1;                                       
	//}
	//if ((GetAsyncKeyState(84) & 0x8000) != 0) { //T
	//	croll += 0.1;
	//}
	//if ((GetAsyncKeyState(71) & 0x8000) != 0) { //G
	//	croll += -0.1;                                       
	//}
	//if ((GetAsyncKeyState(66) & 0x8000) != 0) { //B
	//	cpitch = 0;
	//	croll = 0;
	//}

	if ((GetAsyncKeyState(65) & 0x8000) != 0) { //A
		m_vecPosition[0] += -0.01;                                
	}
	if ((GetAsyncKeyState(68) & 0x8000) != 0) { //D
		m_vecPosition[0] += 0.01;
	}
	if ((GetAsyncKeyState(81) & 0x8000) != 0) { //Q
		m_vecPosition[1] += 0.01;
	}
	if ((GetAsyncKeyState(69) & 0x8000) != 0) { //E
		m_vecPosition[1] += -0.01;
	}
	if ((GetAsyncKeyState(87) & 0x8000) != 0) { //W
		m_vecPosition[2] += -0.01;
	}
	if ((GetAsyncKeyState(83) & 0x8000) != 0) { //S
		m_vecPosition[2] += 0.01;
	}
	if ((GetAsyncKeyState(82) & 0x8000) != 0) { //R
		m_vecPosition[0] = 0;
		m_vecPosition[1] = 0;
		m_vecPosition[2] = 0;
	}   
}

static double CalculateVectorMagnitude(double x, double y, double z)
{
	return sqrt((x * x) + (y * y) + (z * z));
}

static double CalculateVectorDot(double x1, double y1, double z1, double x2, double y2, double z2)
{
	return (x1 * x2) + (y1 * y2) + (z1 * z2);
}
#pragma endregion
