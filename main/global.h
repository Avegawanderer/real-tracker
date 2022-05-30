#ifndef __GLOBAL_H__
#define __GLOBAL_H__


#include <stdint.h>
#include <stdbool.h>

#include "config.h"

//----------- Definitions ----------//

// Common convenience macro
#define LAST_ELEMENT_ADDR(x, t)     &x[(sizeof(x) / sizeof(t)) - 1]
#define NUM_ELEMENTS(x, t)          (sizeof(x) / sizeof(t))
#define UNUSED_ARG(x)               (void)(x)

typedef enum {
    X,
    Y,
    Z,
    AxisCount
} axis_t;

// Period of system tick
#define TICK_PERIOD_MS      10

#endif // __GLOBAL_H__
