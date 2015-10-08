/* ========================================
 * gps_uart.c
 * author: Marshall Garey
 * date created: Feb 2015
 * This file contains various functions that allow you to properly use the gps
 * GPS shield pinout:
 * 1 - Gnd, 2- Vin, 3 - Rx, 4 - Tx, 5 - Gnd, 6 - Directive
 * 
 * ========================================= */
#include "gps_source.h"
#include <ctype.h>

static volatile enum gps_states { init, dollar, letter_g1, letter_p,
    letter_g2, letter_g3, letter_a, comma, goodData, wait } gps_currentState;

static volatile char gps_data[GPS_MAX_DATA];
static volatile bool gps_enable = false;

volatile static bool gps_good_data = true;
volatile static bool gps_dataWasRead = false;

inline static void gps_resetCurrState();
static void gps_clearDataArray();
static void gps_printArray();
static bool gps_isValidChar(char);

void gps_tick()
{
    if (gps_enable == true) // only output if gps_enable signal is true
    {
       // printf("S\n");
        static char myChar = NULL; // static so compiler doesn't have to keep increasing stack size upon each call
        // poll Uart Rx buffer - if it's empty, return; otherwise, get the character,
        // flush the Rx buffer, and process the data:
        myChar = GPS_UART_UartGetChar();
        //GPS_UART_SpiUartClearRxBuffer();
        //printf("%c",myChar); // print out the received character (for debugging)
        //return; // For debug purpose only
        
        if (myChar == NULL)
            return;
        else
        {
            // ===================== perform isr state machine tasks ======================== //
        	// Start in "dollar" state 
        	// If dataArray is writeable, check for the string "$GPGGA,"
        	// If we get that string, clear the array and then
        	// fill up the data array until end of line.
        	// Then lock the array so it can't be written to any more
            static uint8_t gpsArrayIndex = 0;
            // Check for valid data:
            if (gps_isValidChar(myChar))
            {
                switch(gps_currentState)
                {	
                case init:
            	    gps_clearDataArray(); // initialize data array ($ signs)
                    gps_resetCurrState(); // init state 
                    gpsArrayIndex = 0;
                    break;
                case dollar:
                    if (myChar == '$') {
                        gps_currentState = letter_g1;
                    }
                    break;
                case letter_g1:
                    if (myChar == 'G') {
                        gps_currentState = letter_p; 
                    } else {
                        gps_resetCurrState();
                    }
                    break;
                case letter_p:
                    if (myChar == 'P') {
                        gps_currentState = letter_g2;   
                    } else {
                        gps_resetCurrState();
                    }
                    break;
                case letter_g2:
                    if (myChar == 'G') {
                        gps_currentState = letter_g3;
                    } else {
                        gps_resetCurrState();
                    }
                    break;
                case letter_g3:
                    if (myChar == 'G') {
                        gps_currentState = letter_a;
                    } else {
                        gps_resetCurrState();
                    }
                    break;
                case letter_a:
                    if (myChar == 'A') {
                        gps_currentState = comma;
            			// I'm not clearing the array to decrease utilization
                        //gps_clearDataArray(); // at this point we know that it's the correct line, so clear the array
            		} else {
                        gps_resetCurrState();  
                    }
                    break;
                case comma:
                    if (myChar == ',') {
                        gps_currentState = goodData;
            			gpsArrayIndex = 0; // reset data array index
                        gps_dataWasRead = false;
                    } else { // we shouldn't need this line, but just in case...
                        gps_resetCurrState();
                    }
                    break;
                case goodData:
            		// add data to buffer:
            		gps_data[gpsArrayIndex] = myChar;
            		gpsArrayIndex++;
            		// next state logic:
                   // printf("D\n");
                    if (myChar == '$') {
                        gps_printArray();
                        gps_good_data = true;
                        gps_currentState = wait;
                        //printf("N\n"); 
            	    }
                    break;
                case wait:
                    //printf("W\n"); 
                    if (gps_dataWasRead == true) {
                        gps_good_data = false;
                        gps_resetCurrState();
                        //printf("R\n"); 
                        gps_dataWasRead = false;
                        //gps_clearDataArray();
                    }
                    break;
                default:
            		gps_resetCurrState();
                    break;
                }
            }
            // ============================ End ISR tasks =================================== //
        }
    }
    return;
}


bool gps_isGoodData()
{
    return gps_good_data;    
}

inline static void gps_resetCurrState()
{
    gps_currentState = dollar;
    gps_dataWasRead = false;
    return;
}

static void gps_clearDataArray()
{
    uint8_t i = 0;
    while (i < GPS_MAX_DATA)
    {
        gps_data[i] = '$';
        i++;
    }
    return;
}

static void gps_printArray()
{
   // printf("Printing gps data array:\r\n");
    uint8_t i = 0;
    for(i = 0; i < GPS_MAX_DATA; i++)
    {
        //printf("%c",gps_data[i]);    
    }
   // printf("\r\n");
    return;
}


static bool gps_isValidChar(char myChar)
{
    if (isalpha(myChar) || isdigit(myChar) || (myChar == ',') ||
        (myChar == '$') || (myChar == '.') || (myChar == '*'))
    {
        return true;    
    }
    else return false;
}

void gps_readGpsData(char* outgoing_array)
{
    memcpy(outgoing_array, gps_data, GPS_MAX_DATA);
    gps_dataWasRead = true;
}

void gps_enableGps()
{
    gps_enable = true;
}

void gps_disableGps()
{
    gps_enable = false;
}

/* [] END OF FILE */
