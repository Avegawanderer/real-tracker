/**
    Project: Real Tracker
    Module:
    Author:	avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of system task
*/

#pragma once

#include "global.h"

//----------- Externals ------------//

//----------- Definitions ----------//

// System message types
typedef enum {

    sysMsg_Timetick,

    sysMsg_MagDataReady,
    sysMsg_TargetPositionUpdated,

    sysMsg_SetPanManualMode,
    sysMsg_SetPanManualTarget,

    sysMsg_SetTiltManualMode,
    sysMsg_SetTiltManualTarget,

    sysMsg_CalibrateMag,
    sysMsg_CalibratePanServo,



} SystemMessageType;

// Message data structure
typedef struct {
    SystemMessageType type;
    struct {
        int value1;
    } commonRequestData;
} sysMsg_t;

//----------- Variables ------------//

//----------- Prototypes -----------//

//--------- Implementation ---------//

class System {

public:

    static void Run(void *pvParameters);
    static void Tick();

    static QueueHandle_t msgQueue;

    // System state variables. Read-only, do not modify 
    static bool stManualPanControl;
    static bool stManualTiltControl;
    static bool stCalibratingMag;
    static bool stCalibratingPan;
    static int panManualHeading;

private:

    static const uint32_t magCalibrationTimeout = 10;   // seconds
    static struct magCalibrationContext_s {
        uint32_t timer;
    } magCalibrationContext;

    static void OnMagDataReady();

protected:

};


//----------- Prototypes -----------//


