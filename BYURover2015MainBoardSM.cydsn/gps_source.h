/* ========================================
 *
 * Copyright Brigham Young University Mars Rover Team, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Brigham Young University Mars Rover Team.
 *
 * ========================================
*/
#ifndef __GPS_SOURCE_H__
#define __GPS_SOURCE_H__

#include <project.h>
#include <stdbool.h>

#define GPS_MAX_DATA 80

/* tick(void)
* standard tick function
* be sure to call this at least 500 times/second
*/
void gps_tick(void); 

/* isGoodData(void)
* @return - true if data in array is a valid full line of gps data, else false
* ex. of a valid gps line:
* $GPGGA,223851.000,4014.8212,N,11138.8402,W,1,04,2.8,1445.8,M,-16.4,M,,0000*51
*/
bool gps_isGoodData(void);

/* readGpsData(char* array)
* Copies the data from the gps_data_array to your array
* @param array - the array that will be written to
*/
void gps_readGpsData(char* outgoing_array);

/* enableGps(void)
* enables the gps tick function
*/
void gps_enableGps(void);

/* disableGps(void)
* disables the gps tick function
*/
void gps_disableGps(void);

#endif /* __GPS_SOURCE_H__ */

/* [] END OF FILE */