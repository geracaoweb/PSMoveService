#ifndef __PSMOVECLIENT_CAPI_H
#define __PSMOVECLIENT_CAPI_H

#include "PSMoveClient_export.h"
#include "ClientConstants.h"

// Defines a standard _PAUSE function
#if __cplusplus >= 199711L  // if C++11
    #include <thread>
    #define _PAUSE(ms) (std::this_thread::sleep_for(std::chrono::milliseconds(ms)))
#elif defined(_WIN32)       // if windows system
    #include <windows.h>
    #define _PAUSE(ms) (Sleep(ms))
#else                       // assume this is Unix system
    #include <unistd.h>
    #define _PAUSE(ms) (usleep(1000 * ms))
#endif

//-- macros -----
#if defined(__cplusplus) && defined(HAS_PROTOCOL_ACCESS)
#define GET_PSMOVEPROTOCOL_REQUEST(handle) \
    reinterpret_cast<const PSMoveProtocol::Request *>(handle)
#define GET_PSMOVEPROTOCOL_RESPONSE(handle) \
    reinterpret_cast<const PSMoveProtocol::Response *>(handle)
#define GET_PSMOVEPROTOCOL_EVENT(handle) \
    reinterpret_cast<const PSMoveProtocol::Response *>(handle) // events are a special case of responses
#endif // defined(__cplusplus) && defined(HAS_PROTOCOL_ACCESS)

// Wrapper Types
//--------------
typedef int PSMRequestID;
typedef const void * PSMResponseHandle;
typedef void *PSMRequestHandle;
typedef const void *PSMEventDataHandle;

//###HipsterSloth $REVIEW@cboulay - Can we replace with the <stdbool.h>?
typedef char PSMBool;
#define PSMFalse 0
#define PSMTrue 1

// Shared Constants
//-----------------
typedef enum _PSMResult
{
    PSMResult_Error                 = -1,
    PSMResult_Success               = 0,
    PSMResult_Timeout               = 1,
    PSMResult_RequestSent           = 2,
    PSMResult_Canceled              = 3,
} PSMResult;

typedef enum _PSMConnectionType
{
    PSMConnectionType_BLUETOOTH,
    PSMConnectionType_USB
    
} PSMConnectionType;

typedef enum _PSMButtonState {
    PSMButtonState_UP = 0x00,       // (00b) Not pressed
    PSMButtonState_PRESSED = 0x01,  // (01b) Down for one frame only
    PSMButtonState_DOWN = 0x03,     // (11b) Down for >1 frame
    PSMButtonState_RELEASED = 0x02, // (10b) Up for one frame only
} PSMButtonState;

typedef enum _PSMTrackingColorType
{
    PSMTrackingColorType_Magenta,    // R:0xFF, G:0x00, B:0xFF
    PSMTrackingColorType_Cyan,       // R:0x00, G:0xFF, B:0xFF
    PSMTrackingColorType_Yellow,     // R:0xFF, G:0xFF, B:0x00
    PSMTrackingColorType_Red,        // R:0xFF, G:0x00, B:0x00
    PSMTrackingColorType_Green,      // R:0x00, G:0xFF, B:0x00
    PSMTrackingColorType_Blue,       // R:0x00, G:0x00, B:0xFF
} PSMTrackingColorType;

typedef enum _PSMControllerDataStreamFlags
{
    defaultStreamOptions = 0x00,
    includePositionData = 0x01,
    includePhysicsData = 0x02,
    includeRawSensorData = 0x04,
    includeRawTrackerData = 0x08
} PSMControllerDataStreamFlags;

typedef enum _PSMControllerType
{
    PSMController_None= -1,
    PSMController_Move,
    PSMController_Navi
} PSMControllerType;

typedef enum _PSMTrackerType
{
    PSMTracker_PS3Eye
} PSMTrackerType;

typedef enum _PSMTrackerDriver
{
    PSMDriver_LIBUSB,
    PSMDriver_CL_EYE,
    PSMDriver_CL_EYE_MULTICAM,
    PSMDriver_GENERIC_WEBCAM
} PSMTrackerDriver;

// Client Geometry
//----------------

/// A 2D vector with float components.
typedef struct _PSMVector2f
{
    float x, y;
} PSMVector2f;

/// A 3D vector with float components.
typedef struct _PSMVector3f
{
    float x, y, z;
} PSMVector3f;

/// A 3D vector with int components.
typedef struct _PSMVector3i
{
    int x, y, z;
} PSMVector3i;

/// A quaternion rotation.
typedef struct _PSMQuatf
{
    float x, y, z, w;
} PSMQuatf;

/// Position and orientation together.
typedef struct _PSMPosef
{
    PSMVector3f  Position;
    PSMQuatf     Orientation;
} PSMPosef;

typedef struct _PSMovePhysicsData
{
    PSMVector3f LinearVelocity;
    PSMVector3f LinearAcceleration;
    PSMVector3f AngularVelocity;
    PSMVector3f AngularAcceleration;
    double       TimeInSeconds;
} PSMPhysicsData;

typedef struct _PSMRawSensorData
{
    PSMVector3i Magnetometer;
    PSMVector3f Accelerometer;
    PSMVector3f Gyroscope;
    double      TimeInSeconds;
} PSMRawSensorData;

typedef struct _PSMTrackingProjection
{
    enum eShapeType
    {
        INVALID_PROJECTION = -1,
        Ellipse,
        Quad,
    }                               shape_type;
    union{
        struct {
            PSMVector2f center;
            float half_x_extent;
            float half_y_extent;
            float angle;
        } ellipse;
        struct {
            PSMVector2f corners[4];
        } quad;
    }                               shape;
    
} PSMTrackingProjection;

// Controller State
//------------------
typedef struct _PSMRawTrackerData
{
    // Parallel arrays: ScreenLocations, Positions and the TrackerID associated with them
    PSMVector2f             ScreenLocations[PSMOVESERVICE_MAX_TRACKER_COUNT];
    PSMVector3f             RelativePositions[PSMOVESERVICE_MAX_TRACKER_COUNT];
    PSMTrackingProjection   TrackingProjections[PSMOVESERVICE_MAX_TRACKER_COUNT];
    int                     TrackerIDs[PSMOVESERVICE_MAX_TRACKER_COUNT];
    int                     ValidTrackerLocations;
} PSMRawTrackerData;

typedef struct _PSMPSMove
{
    bool                    bHasValidHardwareCalibration;
    bool                    bIsTrackingEnabled;
    bool                    bIsCurrentlyTracking;
    bool                    bHasUnpublishedState;
    
    char                    DevicePath[256];
    char                    DeviceSerial[128];
    char                    AssignedHostSerial[128];
    PSMBool                 PairedToHost;
    PSMConnectionType       ConnectionType;
    
    PSMTrackingColorType    TrackingColorType;
    PSMPosef                Pose;
    PSMPhysicsData          PhysicsData;
    PSMRawSensorData        RawSensorData;
    PSMRawTrackerData       RawTrackerData;
    
    PSMButtonState          TriangleButton;
    PSMButtonState          CircleButton;
    PSMButtonState          CrossButton;
    PSMButtonState          SquareButton;
    PSMButtonState          SelectButton;
    PSMButtonState          StartButton;
    PSMButtonState          PSButton;
    PSMButtonState          MoveButton;
    PSMButtonState          TriggerButton;
    unsigned char           TriggerValue;
    unsigned char           Rumble;
    unsigned char           LED_r, LED_g, LED_b;
    
} PSMPSMove;

typedef struct _PSMPSNavi
{
    PSMButtonState L1Button;
    PSMButtonState L2Button;
    PSMButtonState L3Button;
    PSMButtonState CircleButton;
    PSMButtonState CrossButton;
    PSMButtonState PSButton;
    PSMButtonState TriggerButton;
    PSMButtonState DPadUpButton;
    PSMButtonState DPadRightButton;
    PSMButtonState DPadDownButton;
    PSMButtonState DPadLeftButton;
    unsigned char TriggerValue;
    unsigned char Stick_XAxis;
    unsigned char Stick_YAxis;
} PSMPSNavi;

typedef struct _PSMController
{
    PSMControllerType ControllerType;
    union
    {
        PSMPSMove PSMoveState;
        PSMPSNavi PSNaviState;
    }               ControllerState;
    bool            bValid;
    unsigned int    ControllerID;
    int             OutputSequenceNum;
    int             InputSequenceNum;
    bool            IsConnected;
    long long       DataFrameLastReceivedTime;
    float           DataFrameAverageFPS;
} PSMController;

// Tracker State
//--------------
typedef struct _PSMClientTrackerInfo
{
    // ID of the tracker in the service
    int tracker_id;

    // Tracker USB properties
    PSMTrackerType tracker_type;
    PSMTrackerDriver tracker_driver;
    char device_path[128];

    // Video stream properties
    char shared_memory_name[64];

    // Camera Intrinsic properties
    PSMVector2f tracker_focal_lengths; // pixels
    PSMVector2f tracker_principal_point; // pixels
    PSMVector2f tracker_screen_dimensions; // pixels
    float tracker_hfov; // degrees
    float tracker_vfov; // degrees
    float tracker_znear; // cm
    float tracker_zfar; // cm

    // Camera Extrinsic properties
    PSMPosef tracker_pose;
} PSMClientTrackerInfo;

typedef struct _PSMTracker
{
    // Tracker Static Properties
    PSMClientTrackerInfo tracker_info;

    // Tracker Streaming State
    int listener_count;
    PSMBool is_connected;
    int sequence_num;
    long long data_frame_last_received_time;
    float data_frame_average_fps;

    // SharedVideoFrameReadOnlyAccessor used by config tool
    void *opaque_shared_memory_accesor;
} PSMTracker;

// Service Events
//------------------
typedef struct _PSMEventMessage
{
    enum eEventType
    {
        // Client Events
        connectedToService,
        failedToConnectToService,
        disconnectedFromService,

        // Service Events
        opaqueServiceEvent, // Need to have protocol access to see what kind of event this is
        controllerListUpdated,
        trackerListUpdated,
    } event_type;

    // Opaque handle that can be converted to a <const PSMoveProtocol::Response *> pointer
    // using GET_PSMOVEPROTOCOL_EVENT(handle) if you linked against the PSMoveProtocol lib.
    PSMEventDataHandle event_data_handle;
} PSMEventMessage;

// Service Responses
//------------------
typedef struct _PSMResponsePayload_ControllerList
{
    int controller_id[PSMOVESERVICE_MAX_CONTROLLER_COUNT];
    PSMControllerType controller_type[PSMOVESERVICE_MAX_CONTROLLER_COUNT];
    int count;
} PSMResponsePayload_ControllerList;

typedef struct _PSMResponsePayload_TrackerList
{
    PSMClientTrackerInfo trackers[PSMOVESERVICE_MAX_TRACKER_COUNT];
    int count;
} PSMResponsePayload_TrackerList;

typedef struct _PSMResponsePayload_HMDTrackingSpace
{
    PSMPosef origin_pose;
} PSMResponsePayload_HMDTrackingSpace;

typedef struct _PSMResponseMessage
{
    // Fields common to all responses
    //----
    // The id of the request this response is from
    PSMRequestID request_id;

    // Whether this request succeeded, failed, or was canceled
    PSMResult result_code;

    // Opaque handle that can be converted to a <const PSMoveProtocol::Request *> pointer
    // using GET_PSMOVEPROTOCOL_REQUEST(handle) if you linked against the PSMoveProtocol lib.
    PSMResponseHandle opaque_request_handle;

    // Opaque handle that can be converted to a <const PSMoveProtocol::Response *> pointer
    // using GET_PSMOVEPROTOCOL_RESPONSE(handle) if you linked against the PSMoveProtocol lib.
    PSMResponseHandle opaque_response_handle;

    // Payload data specific to a subset of the responses
    //----
    union
    {
        PSMResponsePayload_ControllerList controller_list;
        PSMResponsePayload_TrackerList tracker_list;
        PSMResponsePayload_HMDTrackingSpace hmd_tracking_space;
    } payload;

    enum eResponsePayloadType
    {
        _responsePayloadType_Empty,
        _responsePayloadType_ControllerList,
        _responsePayloadType_TrackerList,
        _responsePayloadType_HMDTrackingSpace,

        _responsePayloadType_Count
    } payload_type;
} PSMResponseMessage;

typedef void(*PSMResponseCallback)(const PSMResponseMessage *response, void *userdata);

// Message Container
//------------------
struct PSMMessage
{
    union{
        PSMEventMessage event_data;
        PSMResponseMessage response_data;
    };

    enum eMessagePayloadType
    {
        _messagePayloadType_Event,
        _messagePayloadType_Response,

        _messagePayloadType_Count
    } payload_type;
};

// Interface
//----------

PSM_PUBLIC_FUNCTION(const char*) PSM_GetVersionString();
PSM_PUBLIC_FUNCTION(PSMResult) PSM_Initialize(const char* host, const char* port);  //"localhost", "9512"
PSM_PUBLIC_FUNCTION(PSMResult) PSM_Shutdown();
PSM_PUBLIC_FUNCTION(PSMResult) PSM_Update();

/// Blocking Controller Methods
PSM_PUBLIC_FUNCTION(int) PSM_GetControllerList(PSMController** controllers);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_RegisterAsControllerListener(PSMController *controller);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_StartControllerDataStream(PSMController *controller, unsigned int data_stream_flags);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_StopControllerDataStream(PSMController *controller);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_SetControllerLEDColor(PSMController *controller, PSMTrackingColorType tracking_color);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_ResetControllerPose(PSMController *controller);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_DeregisterAsControllerListener(PSMController *controller);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_UpdateController(PSMController *controller);

/// Async Controller Methods
PSM_PUBLIC_FUNCTION(PSMResult) PSM_StartControllerDataStreamAsync(PSMController *controller, unsigned int data_stream_flags, PSMRequestID *out_request_id);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_StopControllerDataStreamAsync(PSMController *controller, PSMRequestID *out_request_id);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_SetControllerLEDColorAsync(PSMController *controller, PSMTrackingColorType tracking_color, PSMRequestID *out_request_id);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_ResetControllerPoseAsync(PSMController *controller, PSMRequestID *out_request_id);

/// Async Message Handling API
PSM_PUBLIC_FUNCTION(PSMResult) PSM_PollNextMessage(PSMMessage *message, size_t message_size);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_SendOpaqueRequest(PSMRequestHandle request_handle, PSMRequestID *out_request_id);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_RegisterCallback(PSMRequestID request_id, PSMResponseCallback callback, void *callback_userdata);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_CancelCallback(PSMRequestID request_id);
PSM_PUBLIC_FUNCTION(PSMResult) PSM_EatResponse(PSMRequestID request_id);

/// Tracker Methods
//PSM_PUBLIC_FUNCTION(PSMResult) PSM_GetTrackerList(PSMTracker** controllers, unsigned int timeout_msec);
//PSM_PUBLIC_FUNCTION(PSMResult) PSM_StartTrackerDataStream(PSMTracker* tracker);
//PSM_PUBLIC_FUNCTION(PSMResult) PSM_StopTrackerDataStream(PSMTracker* tracker);
//PSM_PUBLIC_FUNCTION(PSMResult) PSM_GetHMDTrackingSpaceSettings();



#endif