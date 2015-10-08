/*
 * wiznet_parse.c
 *
 *  Created on: Jan 31, 2015
 *      Author: cramsted joint with Jeff Nybo, Steve and Marshall
 */
#include "wiznet_parse.h"

//This variable needs to be implemented in the main code base. 
//It should be the variable that the recieved wiznet data is copied to when RX goes high
//volatile uint8_t RX[16] = {0x0, 0x1, 0xC, 0x8, 0x0, 0x0, 0x0, 0x4, 0x0, 0x3, 0xE, 0x8, 0x0, 0x0, 0x0, 0x0};
						// 0	1	 2	  3    4    5    6    7    8    9    10   11   12   13   14   15
// Steve's Note: Here is what the (WIZNET) payload looks like:
//  Byte:   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
//  Value:  C0  A8  1   al  ph  pl  zh  zl  0th 1st 2nd 3rd etc
//  Means:  |Src Ip Addr  |Src Port|#bytes| Actual Payload for the rest of the packet
//  For reference, the Python Payload Line: [0x01,0xC8,(mainSeqCount&0xFF),DRIVE_LEN,(gimbal[0]>>8)&0xFF,gimbal[0]&0xFF,(gimbal[1]>>8)&0xFF,gimbal[1]&0xFF]
//  WHAT THIS MEANS FOR YOU! : The High and Low bytes for the gimbal are 12,13 for X, 14,15 for Y, as indexed from the RX[] array
//  Here is a real (WIZNET) packet:
//  Byte:   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
//  Value:  C0  A8  01  01  E4  F0  00  08  01  C8  7A  05  02  89  02  06
//  Means: |Src Ip Addr   |SrcPort|#bytes | See Aligned Below:
//                                          From Basestation to Main Controller Board
//                                              Write Drive Message ID
//                                                  Packet Sequence #
//                                                      #bytes remaining in this message
//                                                          X Axis High Byte
//                                                              X Axis Low Byte
//                                                                  Y Axis High Byte
//                                                                      Y Axis Low Byte
//  Note: The X and Y Axis values are between -1000 and +1000, in this case, X is 649, Y is 518
//parses incoming packets
void wiznet_parse(ToDoList* todo, uint8_t* RX){

    int16_t left_y =  ((int16_t)(((RX[12]<<8) & 0xFF00) | (RX[13] & 0x00FF) ) );
    int16_t right_y = ((int16_t)(((RX[14]<<8) & 0xFF00) | (RX[15] & 0x00FF) ) );
    
    uint16_t lw = (uint16_t)(((left_y)>>1)+1500);
    uint16_t rw = (uint16_t)(((right_y)>>1)+1500);
    
    
    
    // boundary checking:
    if (lw > 2000) todo->leftWheels = 2000;
    else if (lw < 1000) todo->leftWheels = 1000;
    else todo->leftWheels = lw;
    
    if (rw > 2000) todo->rightWheels = 2000;
    else if (rw < 1000) todo->rightWheels = 1000;
    else todo->rightWheels = rw;
    // done with boundary check
    
    
   // int16_t peter =        ((int16_t)(((RX[16]<<8) & 0xFF00) | RX[17]) / 2) + 1500;
   //int16_t tilt_a_whirl = ((int16_t)(((RX[18]<<8) & 0xFF00) | RX[19]) / 2) + 1500; 
    
    
    uint16 temp1;
    uint16 temp2;
    int16_t t;
    
    temp1 = (RX[18] << 8) & 0xFF00;
    temp2 = 0x00FF & (RX[19]);
    t = (int16_t)(temp1 | temp2);
    
    int16_t p = ((int16_t)(((RX[16]<<8) & 0xFF00) | (RX[17] & 0x00FF)));
    //int16_t t = ((int16_t)(((RX[18]<<8) & 0xFF00) | (RX[19] & 0x00FF)));
    
    //uint16_t peter = (uint16_t)(((p)>>1)+1500);
    //uint16_t tilt_a_whirl = (uint16_t)(((t)>>1)+1500);
    
    //boundary check
//    if(peter > 2000) peter = 1500;
//    if(peter < 1000) peter = 1500;
//    
//    if(tilt_a_whirl > 1900) tilt_a_whirl = 1500;
//    if(tilt_a_whirl < 1000) tilt_a_whirl = 1500;
    //end boundary check
    
    //boundary check
    if(p > 1010) p = 0;
    if(p < -1010) p = 0;
    
    if(t > 1010) t = 0;
    if(t < -1010) t = 0;
    
    //GIMBAL PAN
    if(p > 950)
    {
        todo->gimbalPan += PAN_INCREMENT_AMMOUNT;
    }
    else if(p < -950)
    {
        todo->gimbalPan -= PAN_INCREMENT_AMMOUNT;
    }
    else
    {
        todo->gimbalPan = todo->gimbalPan;
    }
   
    
    //Try this: perhaps set the threshold highter
    
    //GIMBAL TILT
    if(t > 950)
    {
        todo->gimbalTilt += TILT_INCREMENT_AMMOUNT;
    }
    else if(t < -10)
    {
        todo->gimbalTilt -= TILT_INCREMENT_AMMOUNT;
    }
    else 
    {
        todo->gimbalTilt = todo->gimbalTilt;
    }
    
   
	    //todo->gimbalPan = 	(uint16_t) peter;
    
	    //todo->gimbalTilt = 	(uint16_t) tilt_a_whirl;
    
	//printf("wiznet_parse is finished");
}