/* ========================================
 *
 * Copyright BRIGHAM YOUNG UNIVERSITY, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF BYU.
 *
 * ========================================
*/
#ifndef INFOREPORTS_H
#define INFOREPORTS_H

#include <project.h>
#include "roverlink10.h"

    
#define CHAR_ARRAY_MAX_SIZE 80
#define HEALTH_PAYLOAD_SIZE 7
#define ROVER_STATUS_PAYLOAD_SIZE 4
#define IMU_PAYLOAD_SIZE 18
#define DRIVE_PAYLOAD_SIZE 8

//Message IDs
#define GPS_MESSAGE_ID 0x01C8
    
/* Internal Structs */
typedef struct todolist
{
    uint16 leftWheels;
    uint16 rightWheels;
    uint16 gimbalPan;
    uint16 gimbalTilt;
} ToDoList;

typedef struct report
{
    uint16 leftWheels;
    uint16 rightWheels;
    uint16 gimbalPan;
    uint16 gimbalTilt;
    char   gpsData[CHAR_ARRAY_MAX_SIZE];
    roverlink_sys_health_t healthStatus;
    roverlink_rover_status_t roverStatus;
    //roverlink_video_t videoStatus;
    
} Report;

/* Packets */
uint8_t healthStatusPayload[HEALTH_PAYLOAD_SIZE];
uint8_t roverStatusPayload[ROVER_STATUS_PAYLOAD_SIZE];
uint8_t imuPayload[IMU_PAYLOAD_SIZE];
uint8_t drivePayload[DRIVE_PAYLOAD_SIZE];

#endif
/* [] END OF FILE */
