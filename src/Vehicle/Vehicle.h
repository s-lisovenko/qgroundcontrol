/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>
#include <QtPositioning/QGeoCoordinate>
#include <QtQmlIntegration/QtQmlIntegration>

#include "HealthAndArmingCheckReport.h"
#include "MAVLinkStreamConfig.h"
#include "QGCMapCircle.h"
#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "SysStatusSensorInfo.h"
#include "VehicleLinkManager.h"

#include "TerrainFactGroup.h"
#include "VehicleFactGroup.h"
#include "VehicleClockFactGroup.h"
#include "VehicleDistanceSensorFactGroup.h"
#include "VehicleEFIFactGroup.h"
#include "VehicleEscStatusFactGroup.h"
#include "VehicleEstimatorStatusFactGroup.h"
#include "VehicleGeneratorFactGroup.h"
#include "VehicleGPS2FactGroup.h"
#include "VehicleGPSFactGroup.h"
#include "VehicleHygrometerFactGroup.h"
#include "VehicleLocalPositionFactGroup.h"
#include "VehicleLocalPositionSetpointFactGroup.h"
#include "VehicleRPMFactGroup.h"
#include "VehicleSetpointFactGroup.h"
#include "VehicleTemperatureFactGroup.h"
#include "VehicleVibrationFactGroup.h"
#include "VehicleWindFactGroup.h"
#include "GimbalController.h"

class Actuators;
class AutoPilotPlugin;
class Autotune;
class ComponentInformationManager;
class EventHandler;
class FirmwarePlugin;
class FTPManager;
class GeoFenceManager;
class ImageProtocolManager;
class StatusTextHandler;
class InitialConnectStateMachine;
class Joystick;
class LinkInterface;
class MAVLinkLogManager;
class MissionManager;
class ParameterManager;
class QGCCameraManager;
class RallyPointManager;
class RemoteIDManager;
class RequestMessageTest;
class SendMavCommandWithHandlerTest;
class SendMavCommandWithSignallingTest;
class StandardModes;
class TerrainAtCoordinateQuery;
class TerrainProtocolHandler;
class TrajectoryPoints;
class VehicleBatteryFactGroup;
class VehicleObjectAvoidance;
#ifdef QGC_UTM_ADAPTER
class UTMSPVehicle;
#endif

namespace events {
namespace parser {
class ParsedEvent;
}
}

Q_DECLARE_LOGGING_CATEGORY(VehicleLog)

class Vehicle : public VehicleFactGroup
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_MOC_INCLUDE("AutoPilotPlugin.h")
    Q_MOC_INCLUDE("TrajectoryPoints.h")
    Q_MOC_INCLUDE("ParameterManager.h")
    Q_MOC_INCLUDE("VehicleObjectAvoidance.h")
    Q_MOC_INCLUDE("Autotune.h")
    Q_MOC_INCLUDE("RemoteIDManager.h")
    Q_MOC_INCLUDE("QGCCameraManager.h")
    Q_MOC_INCLUDE("Actuators.h")
    Q_MOC_INCLUDE("MAVLinkLogManager.h")
    Q_MOC_INCLUDE("LinkInterface.h")

    friend class InitialConnectStateMachine;
    friend class VehicleLinkManager;
    friend class VehicleBatteryFactGroup;           // Allow VehicleBatteryFactGroup to call _addFactGroup
    friend class SendMavCommandWithSignallingTest;  // Unit test
    friend class SendMavCommandWithHandlerTest;     // Unit test
    friend class RequestMessageTest;                // Unit test
    friend class GimbalController;                  // Allow GimbalController to call _addFactGroup

public:
    Vehicle(LinkInterface*          link,
            int                     vehicleId,
            int                     defaultComponentId,
            MAV_AUTOPILOT           firmwareType,
            MAV_TYPE                vehicleType,
            QObject*                parent = nullptr);

    // Pass these into the offline constructor to create an offline vehicle which tracks the offline vehicle settings
    static const MAV_AUTOPILOT    MAV_AUTOPILOT_TRACK = static_cast<MAV_AUTOPILOT>(-1);
    static const MAV_TYPE         MAV_TYPE_TRACK = static_cast<MAV_TYPE>(-1);

    // The following is used to create a disconnected Vehicle for use while offline editing.
    Vehicle(MAV_AUTOPILOT           firmwareType,
            MAV_TYPE                vehicleType,
            QObject*                parent = nullptr);

    ~Vehicle();

    enum CheckList {
        CheckListNotSetup = 0,
        CheckListPassed,
        CheckListFailed,
    };
    Q_ENUM(CheckList)

    Q_PROPERTY(int                  id                          READ id                                                             CONSTANT)
    Q_PROPERTY(AutoPilotPlugin*     autopilotPlugin             MEMBER _autopilotPlugin                                             CONSTANT)
    Q_PROPERTY(QGeoCoordinate       coordinate                  READ coordinate                                                     NOTIFY coordinateChanged)
    Q_PROPERTY(QGeoCoordinate       homePosition                READ homePosition                                                   NOTIFY homePositionChanged)
    Q_PROPERTY(QGeoCoordinate       armedPosition               READ armedPosition                                                  NOTIFY armedPositionChanged)
    Q_PROPERTY(bool                 armed                       READ armed                      WRITE setArmedShowError             NOTIFY armedChanged)
    Q_PROPERTY(bool                 autoDisarm                  READ autoDisarm                                                     NOTIFY autoDisarmChanged)
    Q_PROPERTY(bool                 flightModeSetAvailable      READ flightModeSetAvailable                                         CONSTANT)
    Q_PROPERTY(QStringList          flightModes                 READ flightModes                                                    NOTIFY flightModesChanged)
    Q_PROPERTY(QString              flightMode                  READ flightMode                 WRITE setFlightMode                 NOTIFY flightModeChanged)
    Q_PROPERTY(TrajectoryPoints*    trajectoryPoints            MEMBER _trajectoryPoints                                            CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  cameraTriggerPoints         READ cameraTriggerPoints                                            CONSTANT)
    Q_PROPERTY(float                latitude                    READ latitude                                                       NOTIFY coordinateChanged)
    Q_PROPERTY(float                longitude                   READ longitude                                                      NOTIFY coordinateChanged)
    Q_PROPERTY(bool                 joystickEnabled             READ joystickEnabled            WRITE setJoystickEnabled            NOTIFY joystickEnabledChanged)
    Q_PROPERTY(int                  rcRSSI                      READ rcRSSI                                                         NOTIFY rcRSSIChanged)
    Q_PROPERTY(bool                 px4Firmware                 READ px4Firmware                                                    NOTIFY firmwareTypeChanged)
    Q_PROPERTY(bool                 apmFirmware                 READ apmFirmware                                                    NOTIFY firmwareTypeChanged)
    Q_PROPERTY(bool                 soloFirmware                READ soloFirmware               WRITE setSoloFirmware               NOTIFY soloFirmwareChanged)
    Q_PROPERTY(bool                 genericFirmware             READ genericFirmware                                                CONSTANT)
    Q_PROPERTY(uint                 messagesReceived            READ messagesReceived                                               NOTIFY messagesReceivedChanged)
    Q_PROPERTY(uint                 messagesSent                READ messagesSent                                                   NOTIFY messagesSentChanged)
    Q_PROPERTY(uint                 messagesLost                READ messagesLost                                                   NOTIFY messagesLostChanged)
    Q_PROPERTY(bool                 airship                     READ airship                                                        NOTIFY vehicleTypeChanged)
    Q_PROPERTY(bool                 fixedWing                   READ fixedWing                                                      NOTIFY vehicleTypeChanged)
    Q_PROPERTY(bool                 multiRotor                  READ multiRotor                                                     NOTIFY vehicleTypeChanged)
    Q_PROPERTY(bool                 vtol                        READ vtol                                                           NOTIFY vehicleTypeChanged)
    Q_PROPERTY(bool                 rover                       READ rover                                                          NOTIFY vehicleTypeChanged)
    Q_PROPERTY(bool                 sub                         READ sub                                                            NOTIFY vehicleTypeChanged)
    Q_PROPERTY(bool        supportsThrottleModeCenterZero       READ supportsThrottleModeCenterZero                                 CONSTANT)
    Q_PROPERTY(bool                supportsNegativeThrust       READ supportsNegativeThrust                                         CONSTANT)
    Q_PROPERTY(bool                 supportsJSButton            READ supportsJSButton                                               CONSTANT)
    Q_PROPERTY(bool                 supportsRadio               READ supportsRadio                                                  CONSTANT)
    Q_PROPERTY(bool               supportsMotorInterference     READ supportsMotorInterference                                      CONSTANT)
    Q_PROPERTY(QString              prearmError                 READ prearmError                WRITE setPrearmError                NOTIFY prearmErrorChanged)
    Q_PROPERTY(int                  motorCount                  READ motorCount                                                     CONSTANT)
    Q_PROPERTY(bool                 coaxialMotors               READ coaxialMotors                                                  CONSTANT)
    Q_PROPERTY(bool                 xConfigMotors               READ xConfigMotors                                                  CONSTANT)
    Q_PROPERTY(bool                 isOfflineEditingVehicle     READ isOfflineEditingVehicle                                        CONSTANT)
    Q_PROPERTY(QString              brandImageIndoor            READ brandImageIndoor                                               NOTIFY firmwareTypeChanged)
    Q_PROPERTY(QString              brandImageOutdoor           READ brandImageOutdoor                                              NOTIFY firmwareTypeChanged)
    Q_PROPERTY(int                  sensorsPresentBits          READ sensorsPresentBits                                             NOTIFY sensorsPresentBitsChanged)
    Q_PROPERTY(int                  sensorsEnabledBits          READ sensorsEnabledBits                                             NOTIFY sensorsEnabledBitsChanged)
    Q_PROPERTY(int                  sensorsHealthBits           READ sensorsHealthBits                                              NOTIFY sensorsHealthBitsChanged)
    Q_PROPERTY(int                  sensorsUnhealthyBits        READ sensorsUnhealthyBits                                           NOTIFY sensorsUnhealthyBitsChanged) ///< Combination of enabled and health
    Q_PROPERTY(QString              missionFlightMode           READ missionFlightMode                                              CONSTANT)
    Q_PROPERTY(QString              pauseFlightMode             READ pauseFlightMode                                                CONSTANT)
    Q_PROPERTY(QString              rtlFlightMode               READ rtlFlightMode                                                  CONSTANT)
    Q_PROPERTY(QString              smartRTLFlightMode          READ smartRTLFlightMode                                             CONSTANT)
    Q_PROPERTY(bool                 supportsSmartRTL            READ supportsSmartRTL                                               CONSTANT)
    Q_PROPERTY(QString              landFlightMode              READ landFlightMode                                                 CONSTANT)
    Q_PROPERTY(QString              takeControlFlightMode       READ takeControlFlightMode                                          CONSTANT)
    Q_PROPERTY(QString              followFlightMode            READ followFlightMode                                               CONSTANT)
    Q_PROPERTY(QString              motorDetectionFlightMode    READ motorDetectionFlightMode                                       CONSTANT)
    Q_PROPERTY(QString              stabilizedFlightMode        READ stabilizedFlightMode                                           CONSTANT)
    Q_PROPERTY(QString              firmwareTypeString          READ firmwareTypeString                                             NOTIFY firmwareTypeChanged)
    Q_PROPERTY(QString              vehicleTypeString           READ vehicleTypeString                                              NOTIFY vehicleTypeChanged)
    Q_PROPERTY(QString              vehicleImageOpaque          READ vehicleImageOpaque                                             CONSTANT)
    Q_PROPERTY(QString              vehicleImageOutline         READ vehicleImageOutline                                            CONSTANT)
    Q_PROPERTY(int                  telemetryRRSSI              READ telemetryRRSSI                                                 NOTIFY telemetryRRSSIChanged)
    Q_PROPERTY(int                  telemetryLRSSI              READ telemetryLRSSI                                                 NOTIFY telemetryLRSSIChanged)
    Q_PROPERTY(unsigned int         telemetryRXErrors           READ telemetryRXErrors                                              NOTIFY telemetryRXErrorsChanged)
    Q_PROPERTY(unsigned int         telemetryFixed              READ telemetryFixed                                                 NOTIFY telemetryFixedChanged)
    Q_PROPERTY(unsigned int         telemetryTXBuffer           READ telemetryTXBuffer                                              NOTIFY telemetryTXBufferChanged)
    Q_PROPERTY(int                  telemetryLNoise             READ telemetryLNoise                                                NOTIFY telemetryLNoiseChanged)
    Q_PROPERTY(int                  telemetryRNoise             READ telemetryRNoise                                                NOTIFY telemetryRNoiseChanged)
    Q_PROPERTY(QVariant          mainStatusIndicatorContentItem READ mainStatusIndicatorContentItem                                 CONSTANT)
    Q_PROPERTY(QVariantList         toolIndicators              READ toolIndicators                                                 NOTIFY toolIndicatorsChanged)
    Q_PROPERTY(QVariantList         modeIndicators              READ modeIndicators                                                 NOTIFY modeIndicatorsChanged)
    Q_PROPERTY(bool              initialPlanRequestComplete     READ initialPlanRequestComplete                                     NOTIFY initialPlanRequestCompleteChanged)
    Q_PROPERTY(QVariantList         staticCameraList            READ staticCameraList                                               CONSTANT)
    Q_PROPERTY(QGCCameraManager*    cameraManager               READ cameraManager                                                  NOTIFY cameraManagerChanged)
    Q_PROPERTY(QString              hobbsMeter                  READ hobbsMeter                                                     NOTIFY hobbsMeterChanged)
    Q_PROPERTY(bool                 inFwdFlight                 READ inFwdFlight                                                    NOTIFY inFwdFlightChanged)
    Q_PROPERTY(bool                 vtolInFwdFlight             READ vtolInFwdFlight            WRITE setVtolInFwdFlight            NOTIFY vtolInFwdFlightChanged)
    Q_PROPERTY(bool                 supportsTerrainFrame        READ supportsTerrainFrame                                           NOTIFY firmwareTypeChanged)
    Q_PROPERTY(quint64              mavlinkSentCount            READ mavlinkSentCount                                               NOTIFY mavlinkStatusChanged)
    Q_PROPERTY(quint64              mavlinkReceivedCount        READ mavlinkReceivedCount                                           NOTIFY mavlinkStatusChanged)
    Q_PROPERTY(quint64              mavlinkLossCount            READ mavlinkLossCount                                               NOTIFY mavlinkStatusChanged)
    Q_PROPERTY(float                mavlinkLossPercent          READ mavlinkLossPercent                                             NOTIFY mavlinkStatusChanged)
    Q_PROPERTY(GimbalController*    gimbalController            READ gimbalController                                               CONSTANT)
    Q_PROPERTY(bool                 hasGripper                  READ hasGripper                                                     CONSTANT)
    Q_PROPERTY(bool                 isROIEnabled                READ isROIEnabled                                                   NOTIFY isROIEnabledChanged)
    Q_PROPERTY(CheckList            checkListState              READ checkListState             WRITE setCheckListState             NOTIFY checkListStateChanged)
    Q_PROPERTY(bool                 readyToFlyAvailable         READ readyToFlyAvailable                                            NOTIFY readyToFlyAvailableChanged)  ///< true: readyToFly signalling is available on this vehicle
    Q_PROPERTY(bool                 readyToFly                  READ readyToFly                                                     NOTIFY readyToFlyChanged)
    Q_PROPERTY(QObject*             sysStatusSensorInfo         READ sysStatusSensorInfo                                            CONSTANT)
    Q_PROPERTY(bool                 allSensorsHealthy           READ allSensorsHealthy                                              NOTIFY allSensorsHealthyChanged)    //< true: all sensors in SYS_STATUS reported as healthy
    Q_PROPERTY(bool                 requiresGpsFix              READ requiresGpsFix                                                 NOTIFY requiresGpsFixChanged)
    Q_PROPERTY(double               loadProgress                READ loadProgress                                                   NOTIFY loadProgressChanged)
    Q_PROPERTY(bool                 initialConnectComplete      READ isInitialConnectComplete                                       NOTIFY initialConnectComplete)

    // The following properties relate to Orbit status
    Q_PROPERTY(bool             orbitActive     READ orbitActive        NOTIFY orbitActiveChanged)
    Q_PROPERTY(QGCMapCircle*    orbitMapCircle  READ orbitMapCircle     CONSTANT)

    // Vehicle state used for guided control
    Q_PROPERTY(bool     flying                  READ flying                                         NOTIFY flyingChanged)       ///< Vehicle is flying
    Q_PROPERTY(bool     landing                 READ landing                                        NOTIFY landingChanged)      ///< Vehicle is in landing pattern (DO_LAND_START)
    Q_PROPERTY(bool     guidedMode              READ guidedMode                 WRITE setGuidedMode NOTIFY guidedModeChanged)   ///< Vehicle is in Guided mode and can respond to guided commands
    Q_PROPERTY(bool     guidedModeSupported     READ guidedModeSupported                            CONSTANT)                   ///< Guided mode commands are supported by this vehicle
    Q_PROPERTY(bool     pauseVehicleSupported   READ pauseVehicleSupported                          CONSTANT)                   ///< Pause vehicle command is supported
    Q_PROPERTY(bool     orbitModeSupported      READ orbitModeSupported                             CONSTANT)                   ///< Orbit mode is supported by this vehicle
    Q_PROPERTY(bool     roiModeSupported        READ roiModeSupported                               CONSTANT)                   ///< Orbit mode is supported by this vehicle
    Q_PROPERTY(bool     takeoffVehicleSupported READ takeoffVehicleSupported                        CONSTANT)                   ///< Takeoff supported
    Q_PROPERTY(bool     guidedTakeoffSupported  READ guidedTakeoffSupported                         CONSTANT)                   ///< Guided takeoff supported
    Q_PROPERTY(bool     changeHeadingSupported  READ changeHeadingSupported                         CONSTANT)                   ///< Change Heading supported
    Q_PROPERTY(QString  gotoFlightMode          READ gotoFlightMode                                 CONSTANT)                   ///< Flight mode vehicle is in while performing goto
    Q_PROPERTY(bool     haveMRSpeedLimits       READ haveMRSpeedLimits                              NOTIFY haveMRSpeedLimChanged)
    Q_PROPERTY(bool     haveFWSpeedLimits       READ haveFWSpeedLimits                              NOTIFY haveFWSpeedLimChanged)

    Q_PROPERTY(ParameterManager*        parameterManager    READ parameterManager   CONSTANT)
    Q_PROPERTY(VehicleLinkManager*      vehicleLinkManager  READ vehicleLinkManager CONSTANT)
    Q_PROPERTY(VehicleObjectAvoidance*  objectAvoidance     READ objectAvoidance    CONSTANT)
    Q_PROPERTY(Autotune*                autotune            READ autotune           CONSTANT)
    Q_PROPERTY(RemoteIDManager*         remoteIDManager     READ remoteIDManager    CONSTANT)

    // FactGroup object model properties

    Q_PROPERTY(FactGroup*           vehicle         READ vehicleFactGroup           CONSTANT)
    Q_PROPERTY(FactGroup*           gps             READ gpsFactGroup               CONSTANT)
    Q_PROPERTY(FactGroup*           gps2            READ gps2FactGroup              CONSTANT)
    Q_PROPERTY(FactGroup*           wind            READ windFactGroup              CONSTANT)
    Q_PROPERTY(FactGroup*           vibration       READ vibrationFactGroup         CONSTANT)
    Q_PROPERTY(FactGroup*           temperature     READ temperatureFactGroup       CONSTANT)
    Q_PROPERTY(FactGroup*           clock           READ clockFactGroup             CONSTANT)
    Q_PROPERTY(FactGroup*           setpoint        READ setpointFactGroup          CONSTANT)
    Q_PROPERTY(FactGroup*           escStatus       READ escStatusFactGroup         CONSTANT)
    Q_PROPERTY(FactGroup*           estimatorStatus READ estimatorStatusFactGroup   CONSTANT)
    Q_PROPERTY(FactGroup*           terrain         READ terrainFactGroup           CONSTANT)
    Q_PROPERTY(FactGroup*           distanceSensors READ distanceSensorFactGroup    CONSTANT)
    Q_PROPERTY(FactGroup*           localPosition   READ localPositionFactGroup     CONSTANT)
    Q_PROPERTY(FactGroup*           localPositionSetpoint READ localPositionSetpointFactGroup CONSTANT)
    Q_PROPERTY(FactGroup*           hygrometer      READ hygrometerFactGroup        CONSTANT)
    Q_PROPERTY(FactGroup*           generator       READ generatorFactGroup         CONSTANT)
    Q_PROPERTY(FactGroup*           efi             READ efiFactGroup               CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  batteries       READ batteries                  CONSTANT)
    Q_PROPERTY(Actuators*           actuators       READ actuators                  CONSTANT)
    Q_PROPERTY(HealthAndArmingCheckReport* healthAndArmingCheckReport READ healthAndArmingCheckReport CONSTANT)

    Q_PROPERTY(int      firmwareMajorVersion        READ firmwareMajorVersion       NOTIFY firmwareVersionChanged)
    Q_PROPERTY(int      firmwareMinorVersion        READ firmwareMinorVersion       NOTIFY firmwareVersionChanged)
    Q_PROPERTY(int      firmwarePatchVersion        READ firmwarePatchVersion       NOTIFY firmwareVersionChanged)
    Q_PROPERTY(int      firmwareVersionType         READ firmwareVersionType        NOTIFY firmwareVersionChanged)
    Q_PROPERTY(QString  firmwareVersionTypeString   READ firmwareVersionTypeString  NOTIFY firmwareVersionChanged)
    Q_PROPERTY(int      firmwareCustomMajorVersion  READ firmwareCustomMajorVersion NOTIFY firmwareCustomVersionChanged)
    Q_PROPERTY(int      firmwareCustomMinorVersion  READ firmwareCustomMinorVersion NOTIFY firmwareCustomVersionChanged)
    Q_PROPERTY(int      firmwareCustomPatchVersion  READ firmwareCustomPatchVersion NOTIFY firmwareCustomVersionChanged)
    Q_PROPERTY(QString  gitHash                     READ gitHash                    NOTIFY gitHashChanged)
    Q_PROPERTY(quint64  vehicleUID                  READ vehicleUID                 NOTIFY vehicleUIDChanged)
    Q_PROPERTY(QString  vehicleUIDStr               READ vehicleUIDStr              NOTIFY vehicleUIDChanged)

    Q_PROPERTY(bool     mavlinkSigning              READ mavlinkSigning             NOTIFY mavlinkSigningChanged)

    /// Resets link status counters
    Q_INVOKABLE void resetCounters  ();

    Q_INVOKABLE void virtualTabletJoystickValue(double roll, double pitch, double yaw, double thrust);

    /// Command vehicle to return to launch
    Q_INVOKABLE void guidedModeRTL(bool smartRTL);

    /// Command vehicle to land at current location
    Q_INVOKABLE void guidedModeLand();

    /// Command vehicle to takeoff from current location
    Q_INVOKABLE void guidedModeTakeoff(double altitudeRelative);

    /// @return The minimum takeoff altitude (relative) for guided takeoff.
    Q_INVOKABLE double minimumTakeoffAltitudeMeters();

    /// @return Maximum horizontal speed multirotor.
    Q_INVOKABLE double maximumHorizontalSpeedMultirotor();

    /// @return Maximum equivalent airspeed.
    Q_INVOKABLE double maximumEquivalentAirspeed();

    /// @return Minumum equivalent airspeed.
    Q_INVOKABLE double minimumEquivalentAirspeed();

    /// Command vehicle to move to specified location (altitude is ignored)
    Q_INVOKABLE void guidedModeGotoLocation(const QGeoCoordinate& gotoCoord, double forwardFlightLoiterRadius = 0.0f);

    /// Command vehicle to change altitude
    ///     @param altitudeChange If > 0, go up by amount specified, if < 0, go down by amount specified
    ///     @param pauseVehicle true: pause vehicle prior to altitude change
    Q_INVOKABLE void guidedModeChangeAltitude(double altitudeChange, bool pauseVehicle);

    /// Command vehicle to change yaw
    ///     @param coordinate to rotate towards
    Q_INVOKABLE void guidedModeChangeHeading(const QGeoCoordinate &headingCoord);

    /// Command vehicle to change groundspeed
    ///     @param groundspeed Target horizontal groundspeed
    Q_INVOKABLE void guidedModeChangeGroundSpeedMetersSecond(double groundspeed);
    /// Command vehicle to change equivalent airspeed
    ///     @param airspeed Target equivalent airspeed
    Q_INVOKABLE void guidedModeChangeEquivalentAirspeedMetersSecond(double airspeed);

    /// Command vehicle to orbit given center point
    ///     @param centerCoord Orbit around this point
    ///     @param radius Distance from vehicle to centerCoord
    ///     @param amslAltitude Desired vehicle altitude
    Q_INVOKABLE void guidedModeOrbit(const QGeoCoordinate& centerCoord, double radius, double amslAltitude);

    /// Command vehicle to keep given point as ROI
    ///     @param centerCoord ROI coordinates
    Q_INVOKABLE void guidedModeROI(const QGeoCoordinate& centerCoord);
    Q_INVOKABLE void stopGuidedModeROI();

    /// Command vehicle to pause at current location. If vehicle supports guide mode, vehicle will be left
    /// in guided mode after pause.
    Q_INVOKABLE void pauseVehicle();

    /// Command vehicle to kill all motors no matter what state
    Q_INVOKABLE void emergencyStop();

    /// Command vehicle to abort landing
    Q_INVOKABLE void abortLanding(double climbOutAltitude);

    /// Command vichecle to deploy landing gear
    Q_INVOKABLE void landingGearDeploy();

    /// Command vichecle to retract landing gear
    Q_INVOKABLE void landingGearRetract();

    Q_INVOKABLE void startTakeoff();

    Q_INVOKABLE void startMission();

    /// Alter the current mission item on the vehicle
    Q_INVOKABLE void setCurrentMissionSequence(int seq);

    /// Reboot vehicle
    Q_INVOKABLE void rebootVehicle();

    Q_INVOKABLE void sendPlan(QString planFile);
    Q_INVOKABLE void setEstimatorOrigin(const QGeoCoordinate& centerCoord);

    /// Used to check if running current version is equal or higher than the one being compared.
    //  returns 1 if current > compare, 0 if current == compare, -1 if current < compare
    Q_INVOKABLE int versionCompare(const QString& compare) const;
    Q_INVOKABLE int versionCompare(int major, int minor, int patch) const;

    /// Test motor
    ///     @param motor Motor number, 1-based
    ///     @param percent 0-no power, 100-full power
    ///     @param timeoutSec Disabled motor after this amount of time
    Q_INVOKABLE void motorTest(int motor, int percent, int timeoutSecs, bool showError);

    enum PIDTuningTelemetryMode {
        ModeDisabled,
        ModeRateAndAttitude,
        ModeVelocityAndPosition,
        ModeAltitudeAndAirspeed,
    };
    Q_ENUM(PIDTuningTelemetryMode)

    Q_INVOKABLE void setPIDTuningTelemetryMode(PIDTuningTelemetryMode mode);
    
    Q_INVOKABLE void forceArm           ();

    /// Sends PARAM_MAP_RC message to vehicle
    Q_INVOKABLE void sendParamMapRC(const QString& paramName, double scale, double centerValue, int tuningID, double minValue, double maxValue);

    /// Clears all PARAM_MAP_RC settings from vehicle
    Q_INVOKABLE void clearAllParamMapRC(void);

    /// Removes the vehicle from the system
    Q_INVOKABLE void closeVehicle(void) { _vehicleLinkManager->closeVehicle(); }

    /// Trigger camera using MAV_CMD_DO_DIGICAM_CONTROL command
    Q_INVOKABLE void triggerSimpleCamera(void);

    /// Set home from flight map coordinate
    Q_INVOKABLE void doSetHome(const QGeoCoordinate& coord);

    /// Save the joystick enable setting to the settings group
    Q_INVOKABLE void saveJoystickSettings(void);

    Q_INVOKABLE void sendSetupSigning();

    bool    isInitialConnectComplete() const;
    bool    guidedModeSupported     () const;
    bool    pauseVehicleSupported   () const;
    bool    orbitModeSupported      () const;
    bool    roiModeSupported        () const;
    bool    takeoffVehicleSupported () const;
    bool    guidedTakeoffSupported  () const;
    bool    changeHeadingSupported  () const;
    QString gotoFlightMode          () const;
    bool    hasGripper              () const;
    bool haveMRSpeedLimits() const { return _multirotor_speed_limits_available; }
    bool haveFWSpeedLimits() const { return _fixed_wing_airspeed_limits_available; }

    // Property accessors

    QGeoCoordinate coordinate() { return _coordinate; }
    QGeoCoordinate armedPosition    () { return _armedPosition; }

    qreal getInitialGCSPressure() const { return _initialGCSPressure; }
    qreal getInitialGCSTemperature() const { return _initialGCSTemperature; }
    void setInitialGCSPressure(qreal pressure) { _initialGCSPressure = pressure; }
    void setInitialGCSTemperature(qreal temperature) { _initialGCSTemperature = temperature; }

    void updateFlightDistance(double distance);

    bool joystickEnabled            () const;
    void setJoystickEnabled         (bool enabled);
    void sendJoystickDataThreadSafe (float roll, float pitch, float yaw, float thrust, quint16 buttons);

    // Property accesors
    int id() const{ return _id; }
    int compId() const{ return _compID; }
    MAV_AUTOPILOT firmwareType() const { return _firmwareType; }
    MAV_TYPE vehicleType() const { return _vehicleType; }
    QGCMAVLink::VehicleClass_t vehicleClass(void) const { return QGCMAVLink::vehicleClass(_vehicleType); }
    Q_INVOKABLE QString vehicleClassInternalName() const;

    /// Sends a message to the specified link
    /// @return true: message sent, false: Link no longer connected
    bool sendMessageOnLinkThreadSafe(LinkInterface* link, mavlink_message_t message);

    /// Sends the specified messages multiple times to the vehicle in order to attempt to
    /// guarantee that it makes it to the vehicle.
    void sendMessageMultiple(mavlink_message_t message);

    /// Provides access to AutoPilotPlugin for this vehicle.
    AutoPilotPlugin* autopilotPlugin() { return _autopilotPlugin; }

    /// Provides access to the Firmware Plugin for this Vehicle
    FirmwarePlugin* firmwarePlugin() { return _firmwarePlugin; }


    QGeoCoordinate homePosition();

    bool armed              () const{ return _armed; }
    void setArmed           (bool armed, bool showError);
    void setArmedShowError  (bool armed) { setArmed(armed, true); }

    bool flightModeSetAvailable             ();
    QStringList flightModes                 ();
    QString flightMode                      () const;
    void setFlightMode                      (const QString& flightMode);

    bool airship() const;

    /**
     * @brief Send MAV_CMD_DO_GRIPPER command to trigger specified action in the vehicle
     *
     * @param gripperAction Gripper action to trigger
    */

    void setGripperAction(GRIPPER_ACTIONS gripperAction);
    Q_INVOKABLE void sendGripperAction(QGCMAVLink::GRIPPER_OPTIONS gripperOption);

    void pairRX(int rxType, int rxSubType);

    bool fixedWing() const;
    bool multiRotor() const;
    bool vtol() const;
    bool rover() const;
    bool sub() const;

    bool supportsThrottleModeCenterZero () const;
    bool supportsNegativeThrust         ();
    bool supportsRadio                  () const;
    bool supportsJSButton               () const;
    bool supportsMotorInterference      () const;
    bool supportsTerrainFrame           () const;

    void setGuidedMode(bool guidedMode);

    QString prearmError() const { return _prearmError; }
    void setPrearmError(const QString& prearmError);

    QmlObjectListModel* cameraTriggerPoints () { return &_cameraTriggerPoints; }

    //-- Mavlink Logging
    void startMavlinkLog();
    void stopMavlinkLog();

    /// Requests the specified data stream from the vehicle
    ///     @param stream Stream which is being requested
    ///     @param rate Rate at which to send stream in Hz
    ///     @param sendMultiple Send multiple time to guarantee Vehicle reception
    void requestDataStream(MAV_DATA_STREAM stream, uint16_t rate, bool sendMultiple = true);

    // The follow method are used to turn on/off the tracking of settings updates for firmware/vehicle type on offline vehicles.
    void trackFirmwareVehicleTypeChanges(void);
    void stopTrackingFirmwareVehicleTypeChanges(void);

    float           latitude                    () { return static_cast<float>(_coordinate.latitude()); }
    float           longitude                   () { return static_cast<float>(_coordinate.longitude()); }
    int             rcRSSI                      () const{ return _rcRSSI; }
    bool            px4Firmware                 () const { return _firmwareType == MAV_AUTOPILOT_PX4; }
    bool            apmFirmware                 () const { return _firmwareType == MAV_AUTOPILOT_ARDUPILOTMEGA; }
    bool            genericFirmware             () const { return !px4Firmware() && !apmFirmware(); }
    uint            messagesReceived            () const{ return _messagesReceived; }
    uint            messagesSent                () const{ return _messagesSent; }
    uint            messagesLost                () const{ return _messagesLost; }
    bool            flying                      () const { return _flying; }
    bool            landing                     () const { return _landing; }
    bool            guidedMode                  () const;
    bool            inFwdFlight                 () const;
    bool            vtolInFwdFlight             () const { return _vtolInFwdFlight; }
    uint8_t         baseMode                    () const { return _base_mode; }
    uint32_t        customMode                  () const { return _custom_mode; }
    bool            isOfflineEditingVehicle     () const { return _offlineEditingVehicle; }
    QString         brandImageIndoor            () const;
    QString         brandImageOutdoor           () const;
    int             sensorsPresentBits          () const { return static_cast<int>(_onboardControlSensorsPresent); }
    int             sensorsEnabledBits          () const { return static_cast<int>(_onboardControlSensorsEnabled); }
    int             sensorsHealthBits           () const { return static_cast<int>(_onboardControlSensorsHealth); }
    int             sensorsUnhealthyBits        () const { return static_cast<int>(_onboardControlSensorsUnhealthy); }
    QString         missionFlightMode           () const;
    QString         pauseFlightMode             () const;
    QString         rtlFlightMode               () const;
    QString         smartRTLFlightMode          () const;
    bool            supportsSmartRTL            () const;
    QString         landFlightMode              () const;
    QString         takeControlFlightMode       () const;
    QString         followFlightMode            () const;
    QString         motorDetectionFlightMode    () const;
    QString         stabilizedFlightMode        () const;
    double          defaultCruiseSpeed          () const { return _defaultCruiseSpeed; }
    double          defaultHoverSpeed           () const { return _defaultHoverSpeed; }
    QString         firmwareTypeString          () const;
    QString         vehicleTypeString           () const;
    int             telemetryRRSSI              () const{ return _telemetryRRSSI; }
    int             telemetryLRSSI              () const{ return _telemetryLRSSI; }
    unsigned int    telemetryRXErrors           () const{ return _telemetryRXErrors; }
    unsigned int    telemetryFixed              () const{ return _telemetryFixed; }
    unsigned int    telemetryTXBuffer           () const{ return _telemetryTXBuffer; }
    int             telemetryLNoise             () const{ return _telemetryLNoise; }
    int             telemetryRNoise             () const{ return _telemetryRNoise; }
    bool            autoDisarm                  ();
    bool            orbitActive                 () const { return _orbitActive; }
    QGCMapCircle*   orbitMapCircle              () { return &_orbitMapCircle; }
    bool            readyToFlyAvailable         () const{ return _readyToFlyAvailable; }
    bool            readyToFly                  () const{ return _readyToFly; }
    bool            allSensorsHealthy           () const{ return _allSensorsHealthy; }
    QObject*        sysStatusSensorInfo         () { return &_sysStatusSensorInfo; }
    bool            requiresGpsFix              () const { return static_cast<bool>(_onboardControlSensorsPresent & QGCMAVLink::SysStatusSensorGPS); }
    bool            hilMode                     () const { return _base_mode & MAV_MODE_FLAG_HIL_ENABLED; }
    Actuators*      actuators                   () const { return _actuators; }

    /// Get the maximum MAVLink protocol version supported
    /// @return the maximum version
    unsigned        maxProtoVersion         () const { return _maxProtoVersion; }

    bool            mavlinkSigning          () const { return _mavlinkSigning; }

    void startCalibration   (QGCMAVLink::CalibrationType calType);
    void stopCalibration    (bool showError);

    void startUAVCANBusConfig(void);
    void stopUAVCANBusConfig(void);

    FactGroup* vehicleFactGroup             () { return _vehicleFactGroup; }
    FactGroup* gpsFactGroup                 () { return &_gpsFactGroup; }
    FactGroup* gps2FactGroup                () { return &_gps2FactGroup; }
    FactGroup* windFactGroup                () { return &_windFactGroup; }
    FactGroup* vibrationFactGroup           () { return &_vibrationFactGroup; }
    FactGroup* temperatureFactGroup         () { return &_temperatureFactGroup; }
    FactGroup* clockFactGroup               () { return &_clockFactGroup; }
    FactGroup* setpointFactGroup            () { return &_setpointFactGroup; }
    FactGroup* distanceSensorFactGroup      () { return &_distanceSensorFactGroup; }
    FactGroup* localPositionFactGroup       () { return &_localPositionFactGroup; }
    FactGroup* localPositionSetpointFactGroup() { return &_localPositionSetpointFactGroup; }
    FactGroup* escStatusFactGroup           () { return &_escStatusFactGroup; }
    FactGroup* estimatorStatusFactGroup     () { return &_estimatorStatusFactGroup; }
    FactGroup* terrainFactGroup             () { return &_terrainFactGroup; }
    FactGroup* hygrometerFactGroup          () { return &_hygrometerFactGroup; }
    FactGroup* generatorFactGroup           () { return &_generatorFactGroup; }
    FactGroup* efiFactGroup                 () { return &_efiFactGroup; }
    FactGroup* rpmFactGroup                 () { return &_rpmFactGroup; }
    QmlObjectListModel* batteries           () { return &_batteryFactGroupListModel; }

    MissionManager*                 missionManager      () { return _missionManager; }
    GeoFenceManager*                geoFenceManager     () { return _geoFenceManager; }
    RallyPointManager*              rallyPointManager   () { return _rallyPointManager; }
    ParameterManager*               parameterManager    () { return _parameterManager; }
    ParameterManager*               parameterManager    () const { return _parameterManager; }
    VehicleLinkManager*             vehicleLinkManager  () { return _vehicleLinkManager; }
    FTPManager*                     ftpManager          () { return _ftpManager; }
    ComponentInformationManager*    compInfoManager     () { return _componentInformationManager; }
    VehicleObjectAvoidance*         objectAvoidance     () { return _objectAvoidance; }
    Autotune*                       autotune            () const { return _autotune; }
    RemoteIDManager*                remoteIDManager     () { return _remoteIDManager; }

    static void showCommandAckError(const mavlink_command_ack_t& ack);

    /// Sends the specified MAV_CMD to the vehicle. If no Ack is received command will be retried. If a sendMavCommand is already in progress
    /// the command will be queued and sent when the previous command completes.
    ///     @param compId Component to send to.
    ///     @param command MAV_CMD to send
    ///     @param showError true: Display error to user if command failed, false:  no error shown
    /// Signals: mavCommandResult on success or failure
    void sendMavCommand(int compId, MAV_CMD command, bool showError, float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f, float param6 = 0.0f, float param7 = 0.0f);
    void sendMavCommandDelayed(int compId, MAV_CMD command, bool showError, int milliseconds, float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f, float param6 = 0.0f, float param7 = 0.0f);
    void sendMavCommandInt(int compId, MAV_CMD command, MAV_FRAME frame, bool showError, float param1, float param2, float param3, float param4, double param5, double param6, float param7);

    ///
    /// \brief isMavCommandPending
    ///     Query whether the specified MAV_CMD is in queue to be sent or has
    /// already been sent but whose reply has not yet been received and whose
    /// timeout has not yet expired.
    ///
    ///     Or, said another way: if you call `sendMavCommand(compId, command, true, ...)`
    /// will an error be shown because you (or another part of QGC) has already
    /// sent that command?
    ///
    /// \param targetCompId
    /// \param command
    /// \return
    ///
    bool isMavCommandPending(int targetCompId, MAV_CMD command);

    /// Same as sendMavCommand but available from Qml.
    Q_INVOKABLE void sendCommand(int compId, int command, bool showError, double param1 = 0.0, double param2 = 0.0, double param3 = 0.0, double param4 = 0.0, double param5 = 0.0, double param6 = 0.0, double param7 = 0.0);

    typedef enum {
        MavCmdResultCommandResultOnly,          ///< commandResult specifies full success/fail info
        MavCmdResultFailureNoResponseToCommand, ///< No response from vehicle to command
        MavCmdResultFailureDuplicateCommand,    ///< Unable to send command since duplicate is already being waited on for response
    } MavCmdResultFailureCode_t;

    /// Callback for sendMavCommandWithHandler which handles MAV_RESULT_IN_PROGRESS acks
    ///     @param progressHandlerData  Opaque data passed in to sendMavCommand call
    ///     @param ack                  Received COMMAND_ACK
    typedef void (*MavCmdProgressHandler)(void* progressHandlerData, int compId, const mavlink_command_ack_t& ack);

    /// Callback for sendMavCommandWithHandler which handles all acks which are not MAV_RESULT_IN_PROGRESS
    ///     @param resultHandlerData    Opaque data passed in to sendMavCommand call
    ///     @param ack                  Received COMMAND_ACK
    ///     @param failureCode          Failure reason. If not MavCmdResultCommandResultOnly only ack.result == MAV_RESULT_FAILED is valid.
    typedef void (*MavCmdResultHandler)(void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode);

    // Callback info for sendMavCommandWithHandler
    typedef struct MavCmdAckHandlerInfo_s {
        MavCmdResultHandler     resultHandler;          ///> nullptr for no handler
        void*                   resultHandlerData; 
        MavCmdProgressHandler   progressHandler;
        void*                   progressHandlerData;    ///> nullptr for no handler
    } MavCmdAckHandlerInfo_t;

    /// Sends the command and calls the callback with the result
    void sendMavCommandWithHandler(
        const MavCmdAckHandlerInfo_t* ackHandlerInfo,   ///> nullptr to signale no handlers
        int compId, MAV_CMD command, 
        float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f, float param6 = 0.0f, float param7 = 0.0f);

    /// Sends the command and calls the callback with the result
    ///     @param resultHandler    Callback for result, nullptr for no callback
    ///     @param resultHandleData Opaque data passed through callback
    void sendMavCommandIntWithHandler(
        const MavCmdAckHandlerInfo_t* ackHandlerInfo,   ///> nullptr to signale no handlers
        int compId, MAV_CMD command, MAV_FRAME frame, 
        float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, double param5 = 0.0f, double param6 = 0.0f, float param7 = 0.0f);

    /// Sends the command and calls the fallback lambda function in
    /// case the command is MAV_RESULT_UNSUPPORTED
    void sendMavCommandWithLambdaFallback(
        std::function<void()> lambda,
        int compId, MAV_CMD command,
        bool showError,
        float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f, float param6 = 0.0f, float param7 = 0.0f);


    typedef enum {
        RequestMessageNoFailure,
        RequestMessageFailureCommandError,
        RequestMessageFailureCommandNotAcked,
        RequestMessageFailureMessageNotReceived,
        RequestMessageFailureDuplicateCommand,    ///< Unabled to send command since another request message isduplicate is already being waited on for response
    } RequestMessageResultHandlerFailureCode_t;

    /// Callback for requestMessage
    ///     @param resultHandlerData    Opaque data which was passed in to requestMessage call
    ///     @param commandResult        Result from ack to REQUEST_MESSAGE command
    ///     @param failureCode          Failure code
    ///     @param message              Received message which was requested
    typedef void (*RequestMessageResultHandler)(void* resultHandlerData, MAV_RESULT commandResult, RequestMessageResultHandlerFailureCode_t failureCode, const mavlink_message_t& message);

    /// Requests the vehicle to send the specified message. Will retry a number of times.
    ///     @param resultHandler Callback for result
    ///     @param resultHandlerData Opaque data passed back to resultHandler
    void requestMessage(RequestMessageResultHandler resultHandler, void* resultHandlerData, int compId, int messageId, float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f);

    int firmwareMajorVersion() const { return _firmwareMajorVersion; }
    int firmwareMinorVersion() const { return _firmwareMinorVersion; }
    int firmwarePatchVersion() const { return _firmwarePatchVersion; }
    int firmwareVersionType() const { return _firmwareVersionType; }
    int firmwareCustomMajorVersion() const { return _firmwareCustomMajorVersion; }
    int firmwareCustomMinorVersion() const { return _firmwareCustomMinorVersion; }
    int firmwareCustomPatchVersion() const { return _firmwareCustomPatchVersion; }
    int firmwareBoardVendorId() const { return _firmwareBoardVendorId; }
    int firmwareBoardProductId() const { return _firmwareBoardProductId; }
    QString firmwareVersionTypeString() const;
    void setFirmwareVersion(int majorVersion, int minorVersion, int patchVersion, FIRMWARE_VERSION_TYPE versionType = FIRMWARE_VERSION_TYPE_OFFICIAL);
    void setFirmwareCustomVersion(int majorVersion, int minorVersion, int patchVersion);
    static const int versionNotSetValue = -1;

    QString gitHash() const { return _gitHash; }
    quint64 vehicleUID() const { return _uid; }
    QString vehicleUIDStr();

    bool soloFirmware() const { return _soloFirmware; }
    void setSoloFirmware(bool soloFirmware);

    int defaultComponentId() const{ return _defaultComponentId; }

    /// Sets the default component id for an offline editing vehicle
    void setOfflineEditingDefaultComponentId(int defaultComponentId);

    /// @return -1 = Unknown, Number of motors on vehicle
    int motorCount();

    /// @return true: Motors are coaxial like an X8 config, false: Quadcopter for example
    bool coaxialMotors();

    /// @return true: X confiuration, false: Plus configuration
    bool xConfigMotors();

    /// @return Firmware plugin instance data associated with this Vehicle
    class FirmwarePluginInstanceData* firmwarePluginInstanceData() { return _firmwarePluginInstanceData; }

    /// Sets the firmware plugin instance data associated with this Vehicle. This object will be parented to the Vehicle
    /// and destroyed when the vehicle goes away.
    void setFirmwarePluginInstanceData(FirmwarePluginInstanceData* firmwarePluginInstanceData);

    QString vehicleImageOpaque  () const;
    QString vehicleImageOutline () const;

    QVariant                    mainStatusIndicatorContentItem  ();
    const QVariantList&         toolIndicators                  ();
    const QVariantList&         modeIndicators                  ();
    const QVariantList&         staticCameraList                () const;

    bool capabilitiesKnown      () const { return _capabilityBitsKnown; }
    uint64_t capabilityBits     () const { return _capabilityBits; }    // Change signalled by capabilityBitsChanged

    QGCCameraManager*           cameraManager       () { return _cameraManager; }
    QString                     hobbsMeter          ();

    /// The vehicle is responsible for making the initial request for the Plan.
    /// @return: true: initial request is complete, false: initial request is still in progress;
    bool initialPlanRequestComplete() const { return _initialPlanRequestComplete; }

    void forceInitialPlanRequestComplete();

    void _setFlying(bool flying);
    void _setLanding(bool landing);
    void _setHomePosition(QGeoCoordinate& homeCoord);
    void _setMaxProtoVersion(unsigned version);
    void _setMaxProtoVersionFromBothSources();

    /// Vehicle is about to be deleted
    void prepareDelete();

    /// Delete gimbal controller, handy for RequestMessageTest.cc, otherwise gimbal controller message requests will mess with this test
    void deleteGimbalController();

    /// Delete camera manager, just for testing
    void deleteCameraManager();

    quint64     mavlinkSentCount        () const{ return _mavlinkSentCount; }        /// Calculated total number of messages sent to us
    quint64     mavlinkReceivedCount    () const{ return _mavlinkReceivedCount; }    /// Total number of sucessful messages received
    quint64     mavlinkLossCount        () const{ return _mavlinkLossCount; }        /// Total number of lost messages
    float       mavlinkLossPercent      () const{ return _mavlinkLossPercent; }      /// Running loss rate

    bool        isROIEnabled            () const{ return _isROIEnabled; }

    CheckList   checkListState          () { return _checkListState; }
    void        setCheckListState       (CheckList cl)  { _checkListState = cl; emit checkListStateChanged(); }

    double loadProgress                 () const { return _loadProgress; }

    void setEventsMetadata(uint8_t compid, const QString& metadataJsonFileName);
    void setActuatorsMetadata(uint8_t compid, const QString& metadataJsonFileName);

    HealthAndArmingCheckReport* healthAndArmingCheckReport() { return &_healthAndArmingCheckReport; }

    GimbalController* gimbalController  () { return _gimbalController; }

public slots:
    void setVtolInFwdFlight                 (bool vtolInFwdFlight);
    void _offlineFirmwareTypeSettingChanged (QVariant varFirmwareType); // Should only be used by MissionControler to set firmware from Plan file
    void _offlineVehicleTypeSettingChanged  (QVariant varVehicleType);  // Should only be used by MissionController to set vehicle type from Plan file

signals:
    void coordinateChanged              (QGeoCoordinate coordinate);
    void joystickEnabledChanged         (bool enabled);
    void mavlinkMessageReceived         (const mavlink_message_t& message);
    void homePositionChanged            (const QGeoCoordinate& homePosition);
    void armedPositionChanged();
    void armedChanged                   (bool armed);
    void flightModeChanged              (const QString& flightMode);
    void flyingChanged                  (bool flying);
    void landingChanged                 (bool landing);
    void guidedModeChanged              (bool guidedMode);
    void inFwdFlightChanged             ();
    void vtolInFwdFlightChanged         (bool vtolInFwdFlight);
    void prearmErrorChanged             (const QString& prearmError);
    void soloFirmwareChanged            (bool soloFirmware);
    void defaultCruiseSpeedChanged      (double cruiseSpeed);
    void defaultHoverSpeedChanged       (double hoverSpeed);
    void firmwareTypeChanged            ();
    void vehicleTypeChanged             ();
    void cameraManagerChanged           ();
    void hobbsMeterChanged              ();
    void capabilitiesKnownChanged       (bool capabilitiesKnown);
    void initialPlanRequestCompleteChanged(bool initialPlanRequestComplete);
    void capabilityBitsChanged          (uint64_t capabilityBits);
    void toolIndicatorsChanged          ();
    void modeIndicatorsChanged          ();
    void calibrationEventReceived       (int uasid, int componentid, int severity, QSharedPointer<events::parser::ParsedEvent> event);
    void checkListStateChanged          ();
    void longitudeChanged               ();
    void currentConfigChanged           ();
    void rcRSSIChanged                  (int rcRSSI);
    void telemetryRRSSIChanged          (int value);
    void telemetryLRSSIChanged          (int value);
    void telemetryRXErrorsChanged       (unsigned int value);
    void telemetryFixedChanged          (unsigned int value);
    void telemetryTXBufferChanged       (unsigned int value);
    void telemetryLNoiseChanged         (int value);
    void telemetryRNoiseChanged         (int value);
    void autoDisarmChanged              ();
    void flightModesChanged             ();
    void sensorsPresentBitsChanged      (int sensorsPresentBits);
    void sensorsEnabledBitsChanged      (int sensorsEnabledBits);
    void sensorsHealthBitsChanged       (int sensorsHealthBits);
    void sensorsUnhealthyBitsChanged    (int sensorsUnhealthyBits);
    void orbitActiveChanged             (bool orbitActive);
    void readyToFlyAvailableChanged     (bool readyToFlyAvailable);
    void readyToFlyChanged              (bool readyToFy);
    void allSensorsHealthyChanged       (bool allSensorsHealthy);
    void requiresGpsFixChanged          ();
    void haveMRSpeedLimChanged          ();
    void haveFWSpeedLimChanged          ();

    void firmwareVersionChanged         ();
    void firmwareCustomVersionChanged   ();
    void gitHashChanged                 (QString hash);
    void vehicleUIDChanged              ();
    void loadProgressChanged            (float value);

    /// New RC channel values coming from RC_CHANNELS message
    ///     @param channelCount Number of available channels, maxRcChannels max
    ///     @param pwmValues -1 signals channel not available
    void rcChannelsChanged              (int channelCount, int pwmValues[QGCMAVLink::maxRcChannels]);

    /// Remote control RSSI changed  (0% - 100%)
    void remoteControlRSSIChanged       (uint8_t rssi);

    // Mavlink Log Download
    void mavlinkLogData                 (Vehicle* vehicle, uint8_t target_system, uint8_t target_component, uint16_t sequence, uint8_t first_message, QByteArray data, bool acked);

    /// Signalled in response to usage of sendMavCommand
    ///     @param vehicleId        Vehicle which command was sent to
    ///     @param targetComponent  Component which command was sent to
    ///     @param command          Command which was sent
    ///     @param ackResult        MAV_RESULT returned in ack
    ///     @param failureCode      More detailed failure code Vehicle::MavCmdResultFailureCode_t
    void mavCommandResult               (int vehicleId, int targetComponent, int command, int ackResult, int failureCode);

    // MAVlink Serial Data
    void mavlinkSerialControl           (uint8_t device, uint8_t flags, uint16_t timeout, uint32_t baudrate, QByteArray data);

    // MAVLink protocol version
    void requestProtocolVersion         (unsigned version);
    void mavlinkStatusChanged           ();
    void mavlinkSigningChanged          ();

    void isROIEnabledChanged            ();
    void roiCoordChanged                (const QGeoCoordinate& centerCoord);
    void initialConnectComplete         ();

    void sensorsParametersResetAck      (bool success);

    void logEntry                       (uint32_t time_utc, uint32_t size, uint16_t id, uint16_t num_logs, uint16_t last_log_num);
    void logData                        (uint32_t ofs, uint16_t id, uint8_t count, const uint8_t* data);

private slots:
    void _mavlinkMessageReceived            (LinkInterface* link, mavlink_message_t message);
    void _sendMessageMultipleNext           ();
    void _parametersReady                   (bool parametersReady);
    void _remoteControlRSSIChanged          (uint8_t rssi);
    void _handleFlightModeChanged           (const QString& flightMode);
    void _announceArmedChanged              (bool armed);
    void _offlineCruiseSpeedSettingChanged  (QVariant value);
    void _offlineHoverSpeedSettingChanged   (QVariant value);
    void _prearmErrorTimeout                ();
    void _firstMissionLoadComplete          ();
    void _firstGeoFenceLoadComplete         ();
    void _firstRallyPointLoadComplete       ();
    void _sendMavCommandResponseTimeoutCheck();
    void _clearCameraTriggerPoints          ();
    void _updateDistanceHeadingHome         ();
    void _updateMissionItemIndex            ();
    void _updateHeadingToNextWP             ();
    void _updateDistanceHeadingGCS          ();
    void _updateHomepoint                   ();
    void _updateHobbsMeter                  ();
    void _vehicleParamLoaded                (bool ready);
    void _sendQGCTimeToVehicle              ();
    void _mavlinkMessageStatus              (int uasId, uint64_t totalSent, uint64_t totalReceived, uint64_t totalLoss, float lossPercent);
    void _orbitTelemetryTimeout             ();
    void _updateFlightTime                  ();
    void _gotProgressUpdate                 (float progressValue);
    void _doSetHomeTerrainReceived          (bool success, QList<double> heights);
    void _updateAltAboveTerrain             ();
    void _altitudeAboveTerrainReceived      (bool sucess, QList<double> heights);

private:
    void _loadJoystickSettings          ();
    void _activeVehicleChanged          (Vehicle* newActiveVehicle);
    void _captureJoystick               ();
    void _handlePing                    (LinkInterface* link, mavlink_message_t& message);
    void _handleHomePosition            (mavlink_message_t& message);
    void _handleHeartbeat               (mavlink_message_t& message);
    void _handleCurrentMode             (mavlink_message_t& message);
    void _handleRadioStatus             (mavlink_message_t& message);
    void _handleRCChannels              (mavlink_message_t& message);
    void _handleBatteryStatus           (mavlink_message_t& message);
    void _handleSysStatus               (mavlink_message_t& message);
    void _handleExtendedSysState        (mavlink_message_t& message);
    void _handleCommandAck              (mavlink_message_t& message);
    void _handleGpsRawInt               (mavlink_message_t& message);
    void _handleGlobalPositionInt       (mavlink_message_t& message);
    void _handleHighLatency             (mavlink_message_t& message);
    void _handleHighLatency2            (mavlink_message_t& message);
    void _handleOrbitExecutionStatus    (const mavlink_message_t& message);
    void _handleGimbalOrientation       (const mavlink_message_t& message);
    void _handleObstacleDistance        (const mavlink_message_t& message);
    void _handleFenceStatus             (const mavlink_message_t& message);
    void _handleEvent(uint8_t comp_id, std::unique_ptr<events::parser::ParsedEvent> event);
    // ArduPilot dialect messages
#if !defined(QGC_NO_ARDUPILOT_DIALECT)
    void _handleCameraFeedback          (const mavlink_message_t& message);
#endif
    void _handleCameraImageCaptured     (const mavlink_message_t& message);
    void _handleCommandLong             (const mavlink_message_t& message);
    void _missionManagerError           (int errorCode, const QString& errorMsg);
    void _geoFenceManagerError          (int errorCode, const QString& errorMsg);
    void _rallyPointManagerError        (int errorCode, const QString& errorMsg);
    void _say                           (const QString& text);
    QString _vehicleIdSpeech            ();
    void _handleMavlinkLoggingData      (mavlink_message_t& message);
    void _handleMavlinkLoggingDataAcked (mavlink_message_t& message);
    void _ackMavlinkLogData             (uint16_t sequence);
    void _commonInit                    ();
    void _setupAutoDisarmSignalling     ();
    void _setCapabilities               (uint64_t capabilityBits);
    void _updateArmed                   (bool armed);
    bool _apmArmingNotRequired          ();
    void _initializeCsv                 ();
    void _writeCsvLine                  ();
    void _flightTimerStart              ();
    void _flightTimerStop               ();
    void _setMessageInterval            (int messageId, int rate);
    EventHandler& _eventHandler         (uint8_t compid);
    bool setFlightModeCustom            (const QString& flightMode, uint8_t* base_mode, uint32_t* custom_mode);

    static void _rebootCommandResultHandler(void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode);

    int     _id;                    ///< Mavlink system id
    int     _defaultComponentId;
    bool    _offlineEditingVehicle = false; ///< true: This Vehicle is a "disconnected" vehicle for ui use while offline editing

    MAV_AUTOPILOT       _firmwareType;
    MAV_TYPE            _vehicleType;
    FirmwarePlugin*     _firmwarePlugin = nullptr;
    class FirmwarePluginInstanceData*            _firmwarePluginInstanceData = nullptr;
    AutoPilotPlugin*    _autopilotPlugin = nullptr;
    bool                _soloFirmware = false;

    QTimer              _csvLogTimer;
    QFile               _csvLogFile;

    bool            _joystickEnabled = false;
    bool _isActiveVehicle = false;

    QGeoCoordinate  _coordinate;
    QGeoCoordinate  _homePosition;
    QGeoCoordinate  _armedPosition;

    qreal           _initialGCSPressure = 0.;
    qreal           _initialGCSTemperature = 0.;

    int             _rcRSSI = 255;
    double          _rcRSSIstore = 255;
    bool            _flying = false;
    bool            _landing = false;
    bool            _vtolInFwdFlight = false;
    uint32_t        _onboardControlSensorsPresent = 0;
    uint32_t        _onboardControlSensorsEnabled = 0;
    uint32_t        _onboardControlSensorsHealth = 0;
    uint32_t        _onboardControlSensorsUnhealthy = 0;
    bool            _gpsRawIntMessageAvailable              = false;
    bool            _gps2RawMessageAvailable                = false;
    bool            _globalPositionIntMessageAvailable      = false;
    double          _defaultCruiseSpeed = qQNaN();
    double          _defaultHoverSpeed = qQNaN();
    int             _telemetryRRSSI = 0;
    int             _telemetryLRSSI = 0;
    uint32_t        _telemetryRXErrors = 0;
    uint32_t        _telemetryFixed = 0;
    uint32_t        _telemetryTXBuffer = 0;
    int             _telemetryLNoise = 0;
    int             _telemetryRNoise = 0;
    bool            _mavlinkProtocolRequestComplete         = false;
    unsigned        _mavlinkProtocolRequestMaxProtoVersion  = 0;
    unsigned        _maxProtoVersion                        = 0;
    bool            _capabilityBitsKnown                    = false;
    uint64_t        _capabilityBits                         = 0;
    CheckList       _checkListState                         = CheckListNotSetup;
    bool            _readyToFlyAvailable                    = false;
    bool            _readyToFly                             = false;
    bool            _allSensorsHealthy                      = true;
    bool            _mavlinkSigning                         = false;

    SysStatusSensorInfo _sysStatusSensorInfo;

    QGCCameraManager* _cameraManager = nullptr;

    QString             _prearmError;
    QTimer              _prearmErrorTimer;
    static const int    _prearmErrorTimeoutMSecs = 35 * 1000;   ///< Take away prearm error after 35 seconds

    bool                _initialPlanRequestComplete = false;

    ParameterManager*               _parameterManager               = nullptr;
    ComponentInformationManager*    _componentInformationManager    = nullptr;
    VehicleObjectAvoidance*         _objectAvoidance                = nullptr;
    Autotune*                       _autotune                       = nullptr;
    GimbalController*               _gimbalController               = nullptr;

#ifdef QGC_UTM_ADAPTER
    UTMSPVehicle*                    _utmspVehicle                    = nullptr;
#endif

    bool    _armed = false;         ///< true: vehicle is armed
    uint8_t _base_mode = 0;     ///< base_mode from HEARTBEAT
    uint32_t _custom_mode = 0;  ///< custom_mode from HEARTBEAT
    uint32_t _custom_mode_user_intention = 0;  ///< custom_mode_user_intention from CURRENT_MODE
    bool _has_custom_mode_user_intention = false;

    /// Used to store a message being sent by sendMessageMultiple
    typedef struct {
        mavlink_message_t   message;    ///< Message to send multiple times
        int                 retryCount; ///< Number of retries left
    } SendMessageMultipleInfo_t;

    QList<SendMessageMultipleInfo_t> _sendMessageMultipleList;    ///< List of messages being sent multiple times

    static const int _sendMessageMultipleRetries = 5;
    static const int _sendMessageMultipleIntraMessageDelay = 500;

    QTimer  _sendMultipleTimer;
    int     _nextSendMessageMultipleIndex = 0;

    QElapsedTimer                   _flightTimer;
    QTimer                          _flightTimeUpdater;
    TrajectoryPoints*               _trajectoryPoints = nullptr;
    QmlObjectListModel              _cameraTriggerPoints;
    //QMap<QString, ADSBVehicle*>     _trafficVehicleMap;

    bool _allLinksRemovedSent = false; ///< true: allLinkRemoved signal already sent one time

    uint                _messagesReceived = 0;
    uint                _messagesSent = 0;
    uint                _messagesLost = 0;
    uint8_t             _messageSeq = 0;
    uint8_t             _compID = 0;
    bool                _heardFrom = false;

    bool                _isROIEnabled   = false;
    Joystick*           _activeJoystick = nullptr;

    bool _checkLatestStableFWDone = false;
    int _firmwareMajorVersion = versionNotSetValue;
    int _firmwareMinorVersion = versionNotSetValue;
    int _firmwarePatchVersion = versionNotSetValue;
    int _firmwareCustomMajorVersion = versionNotSetValue;
    int _firmwareCustomMinorVersion = versionNotSetValue;
    int _firmwareCustomPatchVersion = versionNotSetValue;
    FIRMWARE_VERSION_TYPE _firmwareVersionType = FIRMWARE_VERSION_TYPE_OFFICIAL;

    // Vendor and Product as reported from the first autopilot version message
    // during the initial connect. They may be zero eg ArduPilot SITL reports 0
    // by default.
    uint16_t       _firmwareBoardVendorId = 0;
    uint16_t       _firmwareBoardProductId = 0;


    QString _gitHash;
    quint64 _uid = 0;

    uint64_t    _mavlinkSentCount       = 0;
    uint64_t    _mavlinkReceivedCount   = 0;
    uint64_t    _mavlinkLossCount       = 0;
    float       _mavlinkLossPercent     = 0.0f;

    float       _loadProgress           = 0.0f;

    QMap<QString, QTime> _noisySpokenPrearmMap; ///< Used to prevent PreArm messages from being spoken too often

    // Orbit status values
    bool            _orbitActive = false;
    QGCMapCircle    _orbitMapCircle;
    QTimer          _orbitTelemetryTimer;
    static const int _orbitTelemetryTimeoutMsecs = 3000; // No telemetry for this amount and orbit will go inactive

    QMap<uint8_t, QSharedPointer<EventHandler>> _events; ///< One protocol handler for each component ID
    HealthAndArmingCheckReport _healthAndArmingCheckReport;

    MAVLinkStreamConfig _mavlinkStreamConfig;

    /// Callback for waitForMavlinkMessage
    ///     @param resultHandleData     Opaque data passed in to waitForMavlinkMessage call
    ///     @param commandResult        Ack result for command send
    ///     @param noReponseFromVehicle true: The vehicle did not responsed to the COMMAND_LONG message
    typedef void (*WaitForMavlinkMessageResultHandler)(void* resultHandlerData, bool noResponsefromVehicle, const mavlink_message_t& message);

    void _waitForMavlinkMessageMessageReceivedHandler(const mavlink_message_t& message);

    // requestMessage handling

    typedef struct RequestMessageInfo {
        Vehicle*                    vehicle             = nullptr;
        int                         compId;
        int                         msgId;
        RequestMessageResultHandler resultHandler       = nullptr;
        void*                       resultHandlerData   = nullptr;
        bool                        commandAckReceived  = false;    // We keep track of the ack/message being received since the order in which this will come in is random
        bool                        messageReceived     = false;    // We only delete the allocated RequestMessageInfo_t when both happen (or the message wait times out)
        QElapsedTimer               messageWaitElapsedTimer;        // Elapsed time since we started waiting for the message to show up
        mavlink_message_t           message;
    } RequestMessageInfo_t;

    QMap<int /* compId */, QMap<int /* msgId */, RequestMessageInfo_t*>> _requestMessageInfoMap; // Map of all request message calls currently waiting on a response

    void _removeRequestMessageInfo(int compId, int msgId);

    static void _requestMessageCmdResultHandler             (void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode);
    static void _requestMessageWaitForMessageResultHandler  (void* resultHandlerData, bool noResponsefromVehicle, const mavlink_message_t& message);

    typedef struct MavCommandListEntry {
        int                     targetCompId        = MAV_COMP_ID_AUTOPILOT1;
        bool                    useCommandInt       = false;
        MAV_CMD                 command;
        MAV_FRAME               frame;
        float                   rgParam1            = 0;
        float                   rgParam2            = 0;
        float                   rgParam3            = 0;
        float                   rgParam4            = 0;
        double                  rgParam5            = 0;
        double                  rgParam6            = 0;
        float                   rgParam7            = 0;
        bool                    showError           = true;
        MavCmdAckHandlerInfo_t  ackHandlerInfo;
        int                     maxTries            = _mavCommandMaxRetryCount;
        int                     tryCount            = 0;
        QElapsedTimer           elapsedTimer;
        int                     ackTimeoutMSecs     = _mavCommandAckTimeoutMSecs;
    } MavCommandListEntry_t;

    QList<MavCommandListEntry_t>    _mavCommandList;
    QTimer                          _mavCommandResponseCheckTimer;
    static const int                _mavCommandMaxRetryCount                = 3;
    static const int                _mavCommandResponseCheckTimeoutMSecs    = 500;
    static const int                _mavCommandAckTimeoutMSecs              = 3000;
    static const int                _mavCommandAckTimeoutMSecsHighLatency   = 120000;

    void _sendMavCommandWorker  (
            bool commandInt, bool showError, 
            const MavCmdAckHandlerInfo_t* ackHandlerInfo,   ///> nullptr to signale no handlers
            int compId, MAV_CMD command, MAV_FRAME frame, 
            float param1, float param2, float param3, float param4, double param5, double param6, float param7);
    void _sendMavCommandFromList(int index);
    int  _findMavCommandListEntryIndex(int targetCompId, MAV_CMD command);
    bool _sendMavCommandShouldRetry(MAV_CMD command);
    bool _commandCanBeDuplicated(MAV_CMD command);

    QMap<uint8_t /* batteryId */, uint8_t /* MAV_BATTERY_CHARGE_STATE_OK */> _lowestBatteryChargeStateAnnouncedMap;

    float _altitudeTuningOffset = qQNaN(); // altitude offset, so the plotted value is around 0

    // these flags are used to determine if the speed change action from fly view should be shown
    bool _multirotor_speed_limits_available = false;
    bool _fixed_wing_airspeed_limits_available = false;

    // FactGroup facts

    const QString _settingsGroup =               QStringLiteral("Vehicle%1");        // %1 replaced with mavlink system id
    const QString _joystickEnabledSettingsKey =  QStringLiteral("JoystickEnabled");

    const QString _vehicleFactGroupName =            QStringLiteral("vehicle");
    const QString _gpsFactGroupName =                QStringLiteral("gps");
    const QString _gps2FactGroupName =               QStringLiteral("gps2");
    const QString _windFactGroupName =               QStringLiteral("wind");
    const QString _vibrationFactGroupName =          QStringLiteral("vibration");
    const QString _temperatureFactGroupName =        QStringLiteral("temperature");
    const QString _clockFactGroupName =              QStringLiteral("clock");
    const QString _setpointFactGroupName =           QStringLiteral("setpoint");
    const QString _distanceSensorFactGroupName =     QStringLiteral("distanceSensor");
    const QString _localPositionFactGroupName =      QStringLiteral("localPosition");
    const QString _localPositionSetpointFactGroupName = QStringLiteral("localPositionSetpoint");
    const QString _escStatusFactGroupName =          QStringLiteral("escStatus");
    const QString _estimatorStatusFactGroupName =    QStringLiteral("estimatorStatus");
    const QString _terrainFactGroupName =            QStringLiteral("terrain");
    const QString _hygrometerFactGroupName =         QStringLiteral("hygrometer");
    const QString _generatorFactGroupName =          QStringLiteral("generator");
    const QString _efiFactGroupName =                QStringLiteral("efi");
    const QString _rpmFactGroupName =                QStringLiteral("rpm");

    VehicleFactGroup*               _vehicleFactGroup;
    VehicleGPSFactGroup             _gpsFactGroup;
    VehicleGPS2FactGroup            _gps2FactGroup;
    VehicleWindFactGroup            _windFactGroup;
    VehicleVibrationFactGroup       _vibrationFactGroup;
    VehicleTemperatureFactGroup     _temperatureFactGroup;
    VehicleClockFactGroup           _clockFactGroup;
    VehicleSetpointFactGroup        _setpointFactGroup;
    VehicleDistanceSensorFactGroup  _distanceSensorFactGroup;
    VehicleLocalPositionFactGroup   _localPositionFactGroup;
    VehicleLocalPositionSetpointFactGroup _localPositionSetpointFactGroup;
    VehicleEscStatusFactGroup       _escStatusFactGroup;
    VehicleEstimatorStatusFactGroup _estimatorStatusFactGroup;
    VehicleHygrometerFactGroup      _hygrometerFactGroup;
    VehicleGeneratorFactGroup       _generatorFactGroup;
    VehicleEFIFactGroup             _efiFactGroup;
    VehicleRPMFactGroup             _rpmFactGroup;
    TerrainFactGroup                _terrainFactGroup;
    QmlObjectListModel              _batteryFactGroupListModel;

    TerrainProtocolHandler* _terrainProtocolHandler = nullptr;

    MissionManager*                 _missionManager             = nullptr;
    GeoFenceManager*                _geoFenceManager            = nullptr;
    RallyPointManager*              _rallyPointManager          = nullptr;
    VehicleLinkManager*             _vehicleLinkManager         = nullptr;
    FTPManager*                     _ftpManager                 = nullptr;
    InitialConnectStateMachine*     _initialConnectStateMachine = nullptr;
    Actuators*                      _actuators                  = nullptr;
    RemoteIDManager*                _remoteIDManager            = nullptr;
    StandardModes*                  _standardModes              = nullptr;

    // Terrain query members, used to get terrain altitude for doSetHome()
    TerrainAtCoordinateQuery*   _currentDoSetHomeTerrainAtCoordinateQuery = nullptr;
    QGeoCoordinate              _doSetHomeCoordinate;

    // Terrain query members, used to get altitude above terrain Fact
    QElapsedTimer               _altitudeAboveTerrQueryTimer;
    TerrainAtCoordinateQuery*   _altitudeAboveTerrTerrainAtCoordinateQuery = nullptr;
    // We use this to limit above terrain altitude queries based on distance and altitude change
    QGeoCoordinate              _altitudeAboveTerrLastCoord;
    float                       _altitudeAboveTerrLastRelAlt = qQNaN();

public:
    int32_t getMessageRate(uint8_t compId, uint16_t msgId);
    void setMessageRate(uint8_t compId, uint16_t msgId, int32_t rate);

signals:
    void mavlinkMsgIntervalsChanged(uint8_t compid, uint16_t msgId, int32_t rate);

private:
    void _handleMessageInterval(const mavlink_message_t& message);

    static void _requestMessageMessageIntervalResultHandler(void* resultHandlerData, MAV_RESULT result, RequestMessageResultHandlerFailureCode_t failureCode, const mavlink_message_t& message);
    void _requestMessageInterval(uint8_t compId, uint16_t msgId);

    static void _setMessageRateCommandResultHandler(void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode);

    typedef QPair<uint8_t, uint16_t> MavCompMsgId;
    QHash<MavCompMsgId, int32_t> _mavlinkMsgIntervals;
    QMultiHash<uint8_t, uint16_t> _unsupportedMessageIds;
    uint16_t _lastSetMsgIntervalMsgId = 0;

/*===========================================================================*/
/*                         ardupilotmega Dialect                             */
/*===========================================================================*/
public:
    Q_INVOKABLE void flashBootloader();

    /// Command vehicle to Enable/Disable Motor Interlock
    Q_INVOKABLE void motorInterlock(bool enable);
/*---------------------------------------------------------------------------*/
/*===========================================================================*/
/*                         CONTROL STATUS HANDLER                            */
/*===========================================================================*/
public:
    Q_INVOKABLE void startTimerRevertAllowTakeover();
    Q_INVOKABLE void requestOperatorControl(bool allowOverride, int requestTimeoutSecs = 0);

private:
    void _handleControlStatus(const mavlink_message_t& message);
    void _handleCommandRequestOperatorControl(const mavlink_command_long_t commandLong);
    static void _requestOperatorControlAckHandler(void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode);

    Q_PROPERTY(uint8_t sysidInControl                        READ sysidInControl                        NOTIFY gcsControlStatusChanged)
    Q_PROPERTY(bool    gcsControlStatusFlags_SystemManager   READ gcsControlStatusFlags_SystemManager   NOTIFY gcsControlStatusChanged)
    Q_PROPERTY(bool    gcsControlStatusFlags_TakeoverAllowed READ gcsControlStatusFlags_TakeoverAllowed NOTIFY gcsControlStatusChanged)
    Q_PROPERTY(bool    firstControlStatusReceived            READ firstControlStatusReceived            NOTIFY gcsControlStatusChanged)
    Q_PROPERTY(int     operatorControlTakeoverTimeoutMsecs   READ operatorControlTakeoverTimeoutMsecs   CONSTANT)
    Q_PROPERTY(int     requestOperatorControlRemainingMsecs  READ requestOperatorControlRemainingMsecs  CONSTANT)
    Q_PROPERTY(bool    sendControlRequestAllowed             READ sendControlRequestAllowed             NOTIFY sendControlRequestAllowedChanged)

    uint8_t sysidInControl() const { return _sysid_in_control; }
    bool    gcsControlStatusFlags_SystemManager() const { return _gcsControlStatusFlags_SystemManager; }
    bool    gcsControlStatusFlags_TakeoverAllowed() const { return _gcsControlStatusFlags_TakeoverAllowed; }
    bool    firstControlStatusReceived() const { return _firstControlStatusReceived; }
    int     operatorControlTakeoverTimeoutMsecs() const;
    int     requestOperatorControlRemainingMsecs() const { return _timerRequestOperatorControl.remainingTime(); }
    bool    sendControlRequestAllowed() const { return _sendControlRequestAllowed; }
    void    requestOperatorControlStartTimer(int requestTimeoutMsecs);
    
    uint8_t _sysid_in_control = 0;
    uint8_t _gcsControlStatusFlags = 0;
    bool    _gcsControlStatusFlags_SystemManager = 0;
    bool    _gcsControlStatusFlags_TakeoverAllowed = 0;
    bool    _firstControlStatusReceived = false;
    QTimer  _timerRevertAllowTakeover;
    QTimer  _timerRequestOperatorControl;
    bool    _sendControlRequestAllowed = true;

signals:
    void gcsControlStatusChanged();
    void requestOperatorControlReceived(int sysIdRequestingControl, int allowTakeover, int requestTimeoutSecs);
    void sendControlRequestAllowedChanged(bool sendControlRequestAllowed);

/*===========================================================================*/
/*                         STATUS TEXT HANDLER                               */
/*===========================================================================*/
private:
    Q_PROPERTY(bool    messageTypeNone    READ messageTypeNone    NOTIFY messageTypeChanged)
    Q_PROPERTY(bool    messageTypeNormal  READ messageTypeNormal  NOTIFY messageTypeChanged)
    Q_PROPERTY(bool    messageTypeWarning READ messageTypeWarning NOTIFY messageTypeChanged)
    Q_PROPERTY(bool    messageTypeError   READ messageTypeError   NOTIFY messageTypeChanged)
    Q_PROPERTY(int     messageCount       READ messageCount       NOTIFY messageCountChanged)
    Q_PROPERTY(QString formattedMessages  READ formattedMessages  NOTIFY formattedMessagesChanged)

    // Q_PROPERTY(StatusTextHandler *statusTextHandler READ statusTextHandler NOTIFY statusTextHandlerChanged)

public:
    Q_INVOKABLE void resetAllMessages();
    Q_INVOKABLE void resetErrorLevelMessages();
    Q_INVOKABLE void clearMessages();

    bool messageTypeNone() const;
    bool messageTypeNormal() const;
    bool messageTypeWarning() const;
    bool messageTypeError() const;
    int messageCount() const;
    QString formattedMessages() const;

    // StatusTextHandler* statusTextHandler() { return m_statusTextHandler; }

signals:
    void textMessageReceived(int sysid, int componentid, int severity, QString text, QString description);

    void messagesReceivedChanged();
    void messagesSentChanged();
    void messagesLostChanged();
    void messageTypeChanged();
    void messageCountChanged();
    void formattedMessagesChanged();
    void newFormattedMessage(QString formattedMessage);

    // void statusTextHandlerChanged();

private slots:
    void _textMessageReceived(MAV_COMPONENT componentid, MAV_SEVERITY severity, QString text, QString description);
    void _errorMessageReceived(QString message);

private:
    void _createStatusTextHandler();

    StatusTextHandler *m_statusTextHandler = nullptr;
/*---------------------------------------------------------------------------*/
/*===========================================================================*/
/*                        Image Protocol Manager                             */
/*===========================================================================*/
private:
    Q_PROPERTY(uint flowImageIndex READ flowImageIndex NOTIFY flowImageIndexChanged)

public:
    uint32_t flowImageIndex() const;

signals:
    void flowImageIndexChanged();

private:
    void _createImageProtocolManager();

    ImageProtocolManager *_imageProtocolManager = nullptr;
/*---------------------------------------------------------------------------*/
/*===========================================================================*/
/*                         MAVLink Log Manager                               */
/*===========================================================================*/
private:
    Q_PROPERTY(MAVLinkLogManager *mavlinkLogManager READ mavlinkLogManager NOTIFY mavlinkLogManagerChanged)

public:
    MAVLinkLogManager *mavlinkLogManager() const;

signals:
    void mavlinkLogManagerChanged();

private:
    void _createMAVLinkLogManager();

    MAVLinkLogManager *_mavlinkLogManager = nullptr;

/*---------------------------------------------------------------------------*/
};
Q_DECLARE_METATYPE(Vehicle::MavCmdResultFailureCode_t)
