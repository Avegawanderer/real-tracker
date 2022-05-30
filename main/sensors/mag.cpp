/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of magnetometer functions
    Partially adopted from https://github.com/raul-ortega/u360gts
*/

#include <math.h>

#include "maths.h"
#include "sensors_common.h"
#include "mag_sensor.h"
#include "mag.h"

// List header files of all supported magnetometer sensor drivers here
#include "mag_hmc5883.h"

//----------- Definitions ----------//

//----------- Variables ------------//

// Global list of all supported magnetometers
magSensor_t *magnetometers[] = {
    &mag_hmc5883
};

magSensor_t *Mag::sensor;
SensorAlignment_t Mag::alignemt;
float Mag::offset;
float Mag::magneticDeclination;
float Mag::heading;
bool Mag::isCalibrating;
int16_t Mag::zero[AxisCount];
float Mag::gain[AxisCount];
int16_t Mag::cmin[AxisCount];
int16_t Mag::cmax[AxisCount];


//----------- Externals ------------//

//----------- Prototypes -----------//

//--------- Implementation ---------//


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//  Private functions
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

float Mag::calculateHeading2D(const float *vect)
{
    float Xh = vect[X];
    float Yh = vect[Y];
    float hd = atan2f(Yh, Xh);

    if (hd < 0)
        hd += 2 * M_PIf;

    if (hd > 2 * M_PIf)
        hd -= 2 * M_PIf;

    float hd_deg = rad2degf(hd);
    hd_deg += magneticDeclination + offset;
    return hd_deg;
}


float Mag::calculateHeading3D(const float *vect)
{
    // 3D calculation allows for roll/pitch compensation, and requires actual roll and pitch angles.
    // To be implemented
    return 0;
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//  Public interface
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//


// To be called at startup
// System may use magnetometer data update event for another process timing, such as PID controller.
// Sensor driver should notify system whenever is has new data reading avaliable
// System would call then Mag::Run() and another routines
bool Mag::Init(void (*dataReadyCallback)())
{
    bool isSensorDetected = false;
    magSensor_t *sensorToCheck;
    isCalibrating = false;
    alignemt = CW270_DEG_FLIP;      // default
    offset = 35;                // Just to test
    magneticDeclination = 11.0f;    // Moscow
    for (int i = 0; i < NUM_ELEMENTS(magnetometers, magSensor_t *); i++)
    {
        sensorToCheck = magnetometers[i];
        if (sensorToCheck->detect() == true)
        {
            sensor = sensorToCheck;
            //sensor->init(dataReadyCallback);
            // Load calibration or reset calibration flag - TODO
            // Load other settings - alignment, offset, declination - TODO
            isSensorDetected = true;
            break;
        }
    }
    return isSensorDetected;
}


// Should be run when new reading is ready
void Mag::Run()
{
    int16_t currData[AxisCount] = {0};
    float currDataFlt[AxisCount];
    //sensor->read(currData);
    //alignSensor(currData, currData, alignemt);
    if (!isCalibrating)
    {
        for (uint8_t i = 0; i < AxisCount; i++)
        {
            currDataFlt[i] = currData[i] - zero[i];
            currDataFlt[i] *= gain[i];
        }
        heading = calculateHeading2D(currDataFlt);
    }
    else
    {
        for (uint8_t i = 0; i < AxisCount; i++)
        {
            if (currData[i] < cmin[i])
                cmin[i] = currData[i];
            else if (cmax[i] < currData[i])
                cmax[i] = currData[i];
        }
    }
}


void Mag::StartCalibration()
{
    int16_t currData[AxisCount] = {0};
    //sensor->read(currData);
    //alignSensor(currData, currData, alignemt);
    for (uint8_t i = 0; i < AxisCount; i++)
    {
        cmin[i] = cmax[i] = currData[i];
    }
    isCalibrating = true;
}


void Mag::FinishCalibration()
{
    for (uint8_t i = 0; i < AxisCount; i++)
    {
        zero[i] = (cmax[i] - cmin[i]) / 2;
        gain[i] = 1024.0f / (float)(cmax[i] - cmin[i]);
    }
    isCalibrating = false;
}


void Mag::saveCalibration()
{
    // We need to save 3 values for zero and 3 values for gain
    // TODO
}

//
//SensorAlignment_t Mag::getAlignment()
//{
//    return alignemt;
//}


void Mag::setAlignment(SensorAlignment_t align)
{
    alignemt = align;
    // Save to storage - TODO
}


float Mag::getMagneticDeclination()
{
    return magneticDeclination;
}


void Mag::setMagneticDeclination(float value)
{
    magneticDeclination = value;
    // Save to storage - TODO
}


float Mag::getOffset()
{
    return offset;
}


void Mag::setOffset(float value)
{
    offset = value;
    // Save to storage - TODO
}


float Mag::getHeading()
{
    return heading;
}







