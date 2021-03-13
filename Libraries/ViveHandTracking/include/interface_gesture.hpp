#ifndef INTERFACE_GESTURE_HPP
#define INTERFACE_GESTURE_HPP

#ifdef _WIN32
#define INTERFACE_GESTURE_EXPORTS __declspec(dllimport)
#else
#define INTERFACE_GESTURE_EXPORTS
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Enum for selecting computation backend.
enum GestureBackend {
  GestureBackendAuto = 0,  // default backend, use GPU on PC and CPU on Android, Recommended
  GestureBackendCPU = 1,   // use CPU, not supported on PC
  GestureBackendGPU = 2,   // use GPU, supported on PC/Android
};

// Enum for detection mode. Larger mode return more info, but runs more slowly. If a mode is not
// supported on a device, will fallback to previous supported mode.
enum GestureMode {
  GestureMode2DPoint = 0,   // Fastest mode, return one 2d point for hand, supported on all devices
  GestureMode3DPoint = 1,   // Return one 3d point for hand, supported on dual camera devices
  GestureModeSkeleton = 2,  // Return skeleton (21 points) for hand, supported on all devices
};

struct GestureOption {
  GestureBackend backend = GestureBackendAuto;
  GestureMode mode = GestureModeSkeleton;
  int maxFPS = -1;  // limit max fps of raw detection, only used when value in range [15, 90]
};

// Enum for predefined gesture classification
enum GestureType {
  GestureTypeUnknown = 0,  // All other gestures not in predefined set
  GestureTypePoint = 1,
  GestureTypeFist = 2,
  GestureTypeOK = 3,
  GestureTypeLike = 4,
  GestureTypeFive = 5,
  GestureTypeVictory = 6,
};

// Default threshold for pinch level. Levels higher than PINCH_LEVEL_THRESHOLD is pinching.
#define PINCH_LEVEL_THRESHOLD 0.7f

struct GestureVector3 {
  float x, y, z;
};

struct GestureQuaternion {
  float x, y, z, w;
};

// Struct containing information of pinch
struct GesturePinchInfo {
  // Returns pinch (thumb & index) level of the hand, within [0, 1], higher means more possible to
  // pinch. If you only need a boolean value for pinch or not, you can use isPinching instead.
  float pinchLevel;

  // Returns if currently pinching or not.
  // If you need a range value within [0, 1], you can use pinchLevel instead.
  inline bool isPinching() const { return pinchLevel > PINCH_LEVEL_THRESHOLD; }

  // Returns start position of the pinch ray.
  GestureVector3 pinchStart;

  // Returns direction of the pinch ray.
  GestureVector3 pinchDirection;
};

// Struct containing detection result for one hand
struct GestureResult {
  // Returns if this hand is left/right.
  bool isLeft;
  // Returns position of palm center, use this if only need hand position instead of 21 joints.
  GestureVector3 position;
  // Returns position of the hand joints. Meaning of this field is different based on actual mode.
  // 2DPoint & 3DPoint: Only first point is used as the position of hand.
  // Skeleton: An array of 21 keypoints of the hand.
  // +x is right, +y is up, +z is front. Unit is meter.
  // Use union to make code compatible with pervious version.
  // points: Index (3*i, 3*i+1, 3*i+2) composes a (x, y, z) point. There is total 21 points.
  union {
    float points[21 * 3];  // deprecated, please use joints instead
    GestureVector3 joints[21];
  };
  // Returns rotation of the hand joints. Meaning of this field is different based on actual mode.
  // 2DPoint & 3DPoint: Only first element is used as the rotation of hand.
  // Skeleton: Rotation for 21 keypoints of the hand.
  // Identity rotation (assume hand is five gesture): palm face front and fingers point upward.
  GestureQuaternion rotations[21];
  // Returns pre-defined gesture type.
  GestureType gesture;
  // Returns confidence of the hand, within [0, 1].
  float confidence;
  // Returns pinch information, since GesturePinchInfo for details.
  // Use union to make code compatible with pervious version.
  union {
    float pinchLevel;  // deprecated, please use pinch instead
    GesturePinchInfo pinch;
  };
};

// Enum for possible errors in gesture detection
enum GestureFailure {
  GestureFailureNone = 0,        // No error occurs
  GestureFailureOpenCL = -1,     // (Only on Windows) OpenCL is not supported on the machine
  GestureFailureCamera = -2,     // Start camera failed
  GestureFailureInternal = -10,  // Internal errors
  GestureFailureCPUOnPC = -11,   // CPU backend is not supported on Windows
};

/** Start detection with given option, non-blocking.
 * params:
 *   option: (in & out) A pointer to GestureOption. Mode of option may be modified if requirements
 *           is not met on the device. The resulting mode is the actual mode used in detection.
 *           If option is null, default option will be used (auto backend + best mode).
 * return: error code, see GestureFailure
 */
INTERFACE_GESTURE_EXPORTS GestureFailure StartGestureDetection(GestureOption* option);

// Stop the detection. Blocking call until the pipeline is actually stopped.
INTERFACE_GESTURE_EXPORTS void StopGestureDetection();

/**
 * Get detection result in world coordinate. Returns at most one left and one right hand.
 * You should call this function periodically to get latest results. (60/30 FPS on Windows/Android)
 * params:
 *   points: (return value) A pointer to an array of GestureResult. The pointer is valid until next
 *           call to GetGestureResult or StopGestureDetection. The pointer need NOT to be freed.
 *   frameIndex: (return value) A pointer to frame index (can be null). This can be used to check if
 *               results are updated. Set to -1 if detection is not started or stopped due to error.
 * return: number of detections, at most 2.
 * example:
 * const GestureResult* points = NULL;
 * int frameIndex;
 * int size = GetGestureResult(&points, &frameIndex);
 */
INTERFACE_GESTURE_EXPORTS int GetGestureResult(const GestureResult** points, int* frameIndex);

// This function should be called before StartGestureDetection to indicate if caller is providing
// camera transform or not. Default is false. Call it after StartGestureDetection has no use.
// If set to true, hmd positions are not queried from raw HMD API from OpenVR or WaveVR.
// The caller is responsible for managing hmd positions using either of the two methods below:
// 1) Call SetCameraTransform function every frame to provide HMD transform (recommended)
// 2) Apply camera transform to result points after GetGestureResult function call
// This is useful if camera transform is different from raw HMD data, e.g. teleporting.
INTERFACE_GESTURE_EXPORTS void UseExternalTransform(bool value);

// Only takes effect if UseExternalTransform(true) is called before StartGestureDetection.
// Set hmd transform for use. This function should be called regularly if HMD pose is changing.
// The transform is used until new one is set. Default transform is idendity.
INTERFACE_GESTURE_EXPORTS void SetCameraTransform(GestureVector3 position,
                                                  GestureQuaternion rotation);

#ifdef __cplusplus
}
#endif

#endif
