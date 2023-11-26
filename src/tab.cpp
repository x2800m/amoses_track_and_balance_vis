#include <iostream>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <stdio.h>
#include <string.h> 
#include <fcntl.h>      
#include <errno.h>      
#include <termios.h>    
#include <pthread.h>

#include "../inc/tab.hpp"
#include "../inc/logging.hpp"

//-----------------------------------------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------------------------------------

int tab_fd;
unsigned int tab_frame_number = 0;
char tab_uart_in_buffer[256];
//char ahrs_packet_header[20];
struct termios tab_uart_settings;

balance_info current_meas;
extern int display_frame_number;
//-----------------------------------------------------------------------------------------------------------
/* open the serial port to the AHRS*/
int tab_connect(void){
    cout << "Connecting to the track and balance machine" << endl;

    tab_fd = open(TAB_UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if(tab_fd == -1){
        cout << "ERROR: failure to open TAB machine serial port\n";
	}

    tab_uart_settings.c_cflag &= ~PARENB;  // Clear parity bit, disabling parity (most common)
    tab_uart_settings.c_cflag &= ~CSTOPB;  // Clear stop field, only one stop bit used in communication (most common)
    tab_uart_settings.c_cflag &= ~CSIZE;   // Clear all bits that set the data size 
    tab_uart_settings.c_cflag |= CS8;      // 8 bits per byte (most common)
    tab_uart_settings.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tab_uart_settings.c_cflag |= CREAD | CLOCAL;   // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tab_uart_settings.c_lflag &= ~ICANON;
    tab_uart_settings.c_lflag &= ~ECHO;    // Disable echo
    tab_uart_settings.c_lflag &= ~ECHOE;   // Disable erasure
    tab_uart_settings.c_lflag &= ~ECHONL;  // Disable new-line echo
    tab_uart_settings.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP
    tab_uart_settings.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tab_uart_settings.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tab_uart_settings.c_oflag &= ~OPOST;   // Prevent special interpretation of output bytes (e.g. newline chars)
    tab_uart_settings.c_oflag &= ~ONLCR;   // Prevent conversion of newline to carriage return/line feed

    tab_uart_settings.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tab_uart_settings.c_cc[VMIN] = 0;

    cfsetispeed(&tab_uart_settings, TAB_BAUD_RATE);       // Set in/out baud rate
    cfsetospeed(&tab_uart_settings, TAB_BAUD_RATE);

    if (tcsetattr(tab_fd, TCSANOW, &tab_uart_settings) != 0) {       // Save tty settings, also checking for error
        cout << "Error " + to_string(errno) + " from tcsetattr: " + strerror(errno) + "\n";        
        return 1;
    }

    tcflush(tab_fd, TCIOFLUSH);        //Flush the UART buffer

    //Configure the AHRS
    //write(ahrs_fd, AHRS_FACTORY_RESET, sizeof(AHRS_FACTORY_RESET));
    //write(ahrs_fd, AHRS_ASYNC_OFF, sizeof(AHRS_ASYNC_OFF));              // Stop the AHRS from asyc output
    
    return 0;
}
//-----------------------------------------------------------------------------------------------------------
void tab_close(void){
    cout << "Closing the UART\n";
    close(tab_fd);
}
//-----------------------------------------------------------------------------------------------------------
void *tab_update(void *arg){     //capture AHRS data from the UART and publish it to the global state vector

    while(1){
        memset(tab_uart_in_buffer,0,strlen(tab_uart_in_buffer));      //Clear the UART buffer
        
        //TODO: Delete me
        current_meas.clock_dir = 11;
        current_meas.ips = 1;
        current_meas.rpm = 300;

        /*
        write(tab_fd, AHRS_RPY_REQUEST, sizeof(AHRS_RPY_REQUEST));     //Ask for the roll pitch and yaw packet

        int n = read(ahrs_fd, &ahrs_uart_in_buffer, sizeof(ahrs_uart_in_buffer));   //Read the return from the UART

        sscanf(ahrs_uart_in_buffer, "%[^,],%d,%f,%f,%f\n",  
                                                ahrs_packet_header,
                                                &ahrs_reg_id,
                                                &sensor_sv_current.yaw, 
                                                &sensor_sv_current.pitch, 
                                                &sensor_sv_current.roll);

        if(AHRS_CONSOLE_DEBUG == 1){
            cout << "AHRS: " + to_string(sensor_sv_current.roll) + 
                        ", " + to_string(sensor_sv_current.pitch) +
                        ", " + to_string(sensor_sv_current.yaw) +
                        ", " + to_string(ahrs_frame_number) + 
                        "\n";
        }
        */

        usleep(TAB_POLL_DELAY_US);
        tab_frame_number++;

    }
    pthread_exit(NULL);
}
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------