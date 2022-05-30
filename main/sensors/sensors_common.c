/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Common functions for sensors
    Partially adopted from https://github.com/raul-ortega/u360gts
*/

#include <string.h>

#include "maths.h"
#include "sensors_common.h"


//------------ Definitions ----------//

//------------ Variables ------------//

//------------ Externals ------------//

//------------ Prototypes -----------//

//--------- Implementation ----------//


/**
    @brief Align sensors (swap axis)

    @param  src: vector of x, y, z axis
    @param  dest: vector of x, y, z axis, aligned
    @param  alignment: type of conversion
    @return none
*/
void alignSensor(const int16_t *src, int16_t *dest, SensorAlignment_t alignment)
{
    static uint16_t swap[3];
    memcpy(swap, src, sizeof(swap));
    switch (alignment)
    {
        default:
        case CW0_DEG:
            dest[X] = swap[X];
            dest[Y] = swap[Y];
            dest[Z] = swap[Z];
            break;
        case CW90_DEG:
            dest[X] = swap[Y];
            dest[Y] = -swap[X];
            dest[Z] = swap[Z];
            break;
        case CW180_DEG:
            dest[X] = -swap[X];
            dest[Y] = -swap[Y];
            dest[Z] = swap[Z];
            break;
        case CW270_DEG:
            dest[X] = -swap[Y];
            dest[Y] = swap[X];
            dest[Z] = swap[Z];
            break;
        case CW0_DEG_FLIP:
            dest[X] = -swap[X];
            dest[Y] = swap[Y];
            dest[Z] = -swap[Z];
            break;
        case CW90_DEG_FLIP:
            dest[X] = swap[Y];
            dest[Y] = swap[X];
            dest[Z] = -swap[Z];
            break;
        case CW180_DEG_FLIP:
            dest[X] = swap[X];
            dest[Y] = -swap[Y];
            dest[Z] = -swap[Z];
            break;
        case CW270_DEG_FLIP:
            dest[X] = -swap[Y];
            dest[Y] = -swap[X];
            dest[Z] = -swap[Z];
            break;
    }
}


/**
    @brief Convert radians to degree

    @param  rad: angle, rad
    @return angle, degree
*/
float rad2degf(float rad)
{
    return rad * 180.0f / M_PIf;
}
