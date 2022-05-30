/**
    Project: Real Tracker
    Module:
    Author:	avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of magnetometer functions
    Partially adopted from https://github.com/raul-ortega/u360gts
*/

#pragma once

#include "global.h"
#include "sensors_common.h"
#include "mag_sensor.h"

//----------- Externals ------------//

//----------- Variables ------------//

//----------- Definitions ----------//

//----------- Prototypes -----------//

//--------- Implementation ---------//

class Mag {

public:

    // To be called at startup
    static bool Init(void (*dataReadyCallback)());
    
    // Periodic call
    static void Run();

    // To be called by system-level controller
    static void StartCalibration();
    static void FinishCalibration();
    // Save calibration to non-volatile storage
    static void saveCalibration();

    // Get and set values
    //static SensorAlignment_t getAlignment();
    static void setAlignment(SensorAlignment_t align);
    static float getMagneticDeclination();
    static void setMagneticDeclination(float value);
    static float getOffset();
    static void setOffset(float value);
    static float getHeading();

private:

    static magSensor_t *sensor;
    static SensorAlignment_t alignemt;
    static float offset;
    static float magneticDeclination;
    static float heading;

    static bool isCalibrating;
    static int16_t zero[AxisCount];
    static float gain[AxisCount];
    static int16_t cmin[AxisCount];
    static int16_t cmax[AxisCount];

    static float calculateHeading2D(const float *vect);
    static float calculateHeading3D(const float *vect);

protected:

};


//----------- Prototypes -----------//


