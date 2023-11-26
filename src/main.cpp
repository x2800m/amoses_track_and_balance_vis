#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <mutex>

#include "../inc/logging.hpp"
#include "../inc/tab.hpp"
#include "../inc/display.hpp"

#pragma comment (lib, "openal32.lib")
#pragma comment (lib, "alut.lib") 

using namespace std;

//-----------------------------------------------------------------------------------------------------------
char run_flag = 0;
char kb_input;
int thread_spawn_status = 0;
pthread_t   tab_update_thread, 
            display_refresh_thread;

//-----------------------------------------------------------------------------------------------------------
void check_thread_spawn(int ret){
    if(ret != 0){
        printf("Error: pthread_create() failed\n");
        exit(EXIT_FAILURE);
    }
}
//-----------------------------------------------------------------------------------------------------------
int main(){
    run_flag = 1;

    //Initialize the HW interfaces. 
    //open_and_write_a_log();   //open a text file and log some data to it
    tab_connect();             //Open the AHRS uart and connect to the AHRS
    display_setup();            //Make the display window

    //Spawn the relevant threads
    check_thread_spawn(pthread_create(&tab_update_thread, NULL, tab_update, NULL));
    check_thread_spawn(pthread_create(&display_refresh_thread, NULL, display_refresh, NULL));


    //Main loop
    while(run_flag == 1){
        usleep(100000);
    }
    
    //Cleanup & exit
    tab_close();           //close the AHRS uart.
    display_close();        //Close the display window.

    return 0;
}