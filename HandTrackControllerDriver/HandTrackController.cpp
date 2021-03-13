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

		l_pose.qDriverFromHeadRotation.w = 0.707;
		l_pose.qDriverFromHeadRotation.x = 0.707;
		l_pose.qDriverFromHeadRotation.y = 0;
		l_pose.qDriverFromHeadRotation.z = 0;

		//D:\Program Files (x86)\Steam\config\chaperone_info.vrchap
		l_pose.vecWorldFromDriverTranslation[0] = 0.292041361;
		l_pose.vecWorldFromDriverTranslation[1] = -1.2410183;
		l_pose.vecWorldFromDriverTranslation[2] = -1.0129745;

		l_pose.qWorldFromDriverRotation.w = -0.423;
		l_pose.qWorldFromDriverRotation.x = 0;
		l_pose.qWorldFromDriverRotation.y = -0.906;
		l_pose.qWorldFromDriverRotation.z = 0;

		l_pose.vecPosition[0] = gesture->position.x;
		l_pose.vecPosition[1] = gesture->position.y;
		l_pose.vecPosition[2] = -gesture->position.z;

		l_pose.qRotation.w = -gesture->rotations[9].w;
		l_pose.qRotation.x = gesture->rotations[9].x;
		l_pose.qRotation.y = gesture->rotations[9].y;
		l_pose.qRotation.z = -gesture->rotations[9].z;
	}

	vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDeviceObjectId, l_pose, sizeof(vr::DriverPose_t));
	m_last_pose = l_pose;
}
#pragma endregion
