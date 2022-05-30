/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Intermidiate layer between WEB server and the rest of the system
*/

#ifndef __WEBDATA_H__
#define __WEBDATA_H__

#include "global.h"



    void webdata(void);



#ifdef __cplusplus
extern "C" {
#endif

    void webData_OnTextMessage(int clientNum, char *msg, int len);

#ifdef __cplusplus
}   // extern "C"
#endif






#endif // __WEBDATA_H__
