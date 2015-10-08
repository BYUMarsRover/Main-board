/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "Scheduler.h"

#define TIMEOUT_CNT 50000

volatile uint16 timeoutCounter;
volatile bool recievedMessage = false;
bool messageStillNotRecievedError = false;

void initTodoList(ToDoList* todo)
{
    todo->gimbalPan   = 1500;
    todo->gimbalTilt  = 1500;
    todo->leftWheels  = 1500;
    todo->rightWheels = 1500;
}

void initReport(Report* report)
{
    report->gimbalPan   = 1500;
    report->gimbalTilt  = 1500;
    memset(report->gpsData, 0, CHAR_ARRAY_MAX_SIZE);
    report->leftWheels  = 1500;
    report->rightWheels = 1500;
    report->healthStatus.systemState = STARTUP;
}

/*//static void gps_printArray(Scheduler* sch)
//{
//    printf("Printing gps data array:\r\n");
//    uint8_t i = 0;
//    for(i = 0; i < GPS_MAX_DATA; i++)
//    {
//        printf("%c",sch->gpsIncoming[i]);    
//    }
//    printf("\r\n");
//    return;
//}*/

void sendPacket(uint16_t id, uint8_t* payload, uint8_t payloadSize)
{
    wiznetWriteUdpFrame(&id, 1);
    wiznetWriteUdpFrame(payload, payloadSize);
    wiznetSend();
}


void sendAllPacketsForReport()
{
    sendPacket(HEALTH_HEARTBEAT,healthStatusPayload, sizeof(healthStatusPayload));
    sendPacket(ROVER_STATUS, roverStatusPayload, sizeof(roverStatusPayload));
    sendPacket(IMU_SAMPLE, imuPayload, sizeof(imuPayload));
    sendPacket(DRIVE_SETPOINTS, drivePayload, sizeof(drivePayload));
}

//How packets are sent from main to Basestation
//Heartbeat:
// 

enum taskSchedulerStates {init_taskScheduler, read, update, read_sensors, report, wait, reset} taskSchedulerState;


void taskScheduler(Scheduler* sch, volatile uint8_t* testSignal)
{
    //State Actions
    switch (taskSchedulerState) 
    {
        case init_taskScheduler:
        //printf("init state");
        initTodoList(&(sch->todo));
        initReport(&(sch->report));
        
        timeoutCounter = 0;
//        gps_enableGps();
            
        break;
        case read:
        sch->report.healthStatus.systemState = NORMAL;
        
        //Take top Package off of Wiznet Buffer
       /* sch->todo.leftWheels = testSignal[0]; 
        //printf("Test Signal 0 value read: %u", testSignal[0]);
        sch->todo.rightWheels = testSignal[1];
        sch->todo.gimbalPan = testSignal[2];
        sch->todo.gimbalTilt = testSignal[3];*/
        //Open it up
        //Parse and save all pieces of data into ToDo list struct
        
        
        
        wiznet_parse(&(sch->todo), testSignal);
        
        //printf("Left wheelie: %u \n\r", sch->todo.leftWheels);
        //printf("Right Wheelie: %u \n\r", sch->todo.leftWheels);
        //printf("Gimbal Peter Pan: %u \n\r", sch->todo.gimbalPan);
        //printf("Gimbal Tilt-a-Whirl: %u \n\r", sch->todo.gimbalTilt);
        
        
        break;
        
        case update:
        
        //Write var to pwm components
        Wheels_1_WriteCompare(sch->todo.leftWheels);
        Wheels_2_WriteCompare(sch->todo.rightWheels);
        Gimbal_WriteCompare1(sch->todo.gimbalPan);
        Gimbal_WriteCompare2(sch->todo.gimbalTilt);
        
        
        //Switch camera
        break;
       
        //read incomming GPS and IMU data
        case read_sensors:
            if(gps_isGoodData())
            {
                gps_readGpsData(sch->report.gpsData);
                //gps_printArray(sch);
            }
            
            //sch->report.gpsData[5] = 'G';
        break;
        
        case report:
            //wiznetWriteUdpFrame((uint8_t*)sch->report.gpsData, sizeof(sch->report.gpsData));
            //wiznetSend();
            
            sendPacket(GPS_MESSAGE_ID, (uint8_t*)sch->report.gpsData, sizeof(sch->report.gpsData));
        break;
        case wait:
       
        break;
        case reset:
            
        break;
        
        
    }    
       
    //Transitions
    switch(taskSchedulerState)
    {
        case init_taskScheduler:
            taskSchedulerState = wait;
        break;
        case read:
            taskSchedulerState = update;
        break;
        case update:
            taskSchedulerState = read_sensors;
        break;
        case read_sensors:
            taskSchedulerState = report;
        break;
        case report:
            timeoutCounter = 0;
            taskSchedulerState = wait;
        break;
        case wait:
            
            //taskSchedulerState = read; /* uncomment to bypass the wait state */
            
            if(WIZ_INT_Read() == 0)
            {
           
                timeoutCounter = 0;
                taskSchedulerState = read;
            }
            else if(timeoutCounter >= TIMEOUT_CNT)
            {
                timeoutCounter = 0;
               // printf("!");
                sch->todo.leftWheels = 1500;
                sch->todo.rightWheels = 1500;
                sch->todo.gimbalPan = 1500;
                sch->todo.gimbalTilt = 1500;
                sch->report.healthStatus.systemState = FAILSAFE;
                taskSchedulerState = reset;   
            }
            else
            {
                //printf("i"); 
                timeoutCounter++;
                taskSchedulerState = wait;
            }
        break;
        case reset:
            if(WIZ_INT_Read() == 0)
            {
                
                
                taskSchedulerState = read;
            }
            else
            {
            Wheels_1_WriteCompare(sch->todo.leftWheels);
            Wheels_2_WriteCompare(sch->todo.rightWheels);
            Gimbal_WriteCompare1(sch->todo.gimbalPan);
            Gimbal_WriteCompare2(sch->todo.gimbalTilt);
            
            taskSchedulerState = reset;   
            }
        break;
        
    }
    
}

/* [] END OF FILE */
