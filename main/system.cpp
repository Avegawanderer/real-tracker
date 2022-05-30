/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of system task
*/

#include "global.h"
#include "config.h"

#include "network.h"
#include "mag.h"
#include "system.h"


//----------- Definitions ----------//

//----------- Variables ------------//

QueueHandle_t System::msgQueue;


bool System::stManualPanControl;
bool System::stManualTiltControl;
bool System::stCalibratingMag;
bool System::stCalibratingPan;
int System::panManualHeading;

struct System::magCalibrationContext_s System::magCalibrationContext;




//----------- Externals ------------//

//----------- Prototypes -----------//

//--------- Implementation ---------//


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//  Private functions
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void System::OnMagDataReady()
{
    // This callback is called from interrupt context.
    BaseType_t xHigherPriorityTaskWoken;
    sysMsg_t sysMsg = {
        .type = sysMsg_MagDataReady
    };
    xQueueSendFromISR(msgQueue, &sysMsg, &xHigherPriorityTaskWoken);
    if( xHigherPriorityTaskWoken )
    {
        portYIELD_FROM_ISR();
    }
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//  Public interface
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//


void System::Run(void *pvParameters)
{
    sysMsg_t msg;

    msgQueue = xQueueCreate(10, sizeof(sysMsg_t));
    if (msgQueue == NULL)
        while(1);

    Mag::Init(OnMagDataReady);
    //PanController::Init();
    //TiltController::Init();

    stManualPanControl = true;      // Just to test
    stManualTiltControl = false;
    stCalibratingMag = false;
    stCalibratingPan = false;
    panManualHeading = 55;      // Just to test

    //PanController::StartTracking();
    //TiltController::StartTracking();

    while (1)
    {
        // Get message from queue
        xQueueReceive(msgQueue, &msg, portMAX_DELAY);

        // Process message
        switch (msg.type)
        {
            case sysMsg_Timetick:
                if (stCalibratingMag)
                {
                    // if Mag::IsCalibrationDone();
                    if ((magCalibrationContext.timer += TICK_PERIOD_MS) >= magCalibrationTimeout * 1000)
                    {
                        stCalibratingMag = false;
                        Mag::FinishCalibration();
                        // Pan controller keeps rotating now. Stop is and start tracking
                        //PanController::Stop();
                        //PanController::StartTracking();       // Target is set manualy or by telemetry
                    }
                }
                else if (stCalibratingPan)
                {
                    if (0) // PanController::IsCalibrationDone();
                    {
                        stCalibratingPan = false;
                        // Pan controller is stopped after self-calibration. Start tracking
                        //PanController::StartTracking();       // Target is set manualy or by telemetry
                    }
                }
                break;

            case sysMsg_MagDataReady:
                Mag::Run();
                // Always update Pan controller with heading. If Pan controller keeps rotating now for mag calibration or 
                // is doing self-calibration, control loop is not called
                //PanController::UpdateSelfHeading(Mag::getHeading());
                break;

            case sysMsg_TargetPositionUpdated:
                // New telemetry has arrived
                if (!stManualPanControl)
                {
                    //PanController::SetHeading(msg.commonRequestData.val1);
                }
                break;

            case sysMsg_SetPanManualMode:
                stManualPanControl = (bool)msg.commonRequestData.value1;
                if (stManualPanControl)
                {
                    if ((!stCalibratingMag) && (!stCalibratingPan))
                    {
                        // Heading target should be updated by UI as separate request
                        //PanController::Stop();
                    }
                }
                else
                {
                    //PanController::StartTracking();
                }
                break;

            case sysMsg_SetPanManualTarget:
                panManualHeading = msg.commonRequestData.value1;
                if (stManualPanControl)
                {
                    if ((!stCalibratingMag) && (!stCalibratingPan))
                    {
                        //PanController::SetHeading(panManualHeading);
                    }
                }
                break;

            case sysMsg_SetTiltManualMode:

                break;

            case sysMsg_SetTiltManualTarget:

                break;

            case sysMsg_CalibrateMag:
                if ((!stCalibratingMag) && (!stCalibratingPan))
                {
                    magCalibrationContext.timer = 0;
                    Mag::StartCalibration();
                    //PanController::Stop();
                    //PanController::Rotate(10);
                    stCalibratingMag = true;
                }
                break;

            case sysMsg_CalibratePanServo:
                if ((!stCalibratingPan) && (!stCalibratingMag))
                {
                    stCalibratingPan = true;
                    //PanController::Stop();
                    //PanController::StartCalibration();
                }
                break;

        }
    }
}


void System::Tick()
{
    sysMsg_t sysMsg = {
        .type = sysMsg_Timetick
    };
    xQueueSend(msgQueue, &sysMsg, portMAX_DELAY);
}






