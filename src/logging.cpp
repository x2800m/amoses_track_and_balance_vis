#include <iostream>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include "../inc/tab.hpp"
#include "../inc/logging.hpp"

using namespace std;
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------

void write_state_vector(ostream& output_stream){
    //Update the time value;
    /*
    sensor_sv_current.timestamp = time(0);
    output_stream << sensor_sv_current.aircraft_id << ",";
    output_stream << sensor_sv_current.frame_number << ",";
    output_stream << sensor_sv_current.timestamp << ",";
    output_stream << sensor_sv_current.roll << ",";
    output_stream << sensor_sv_current.pitch << ",";
    output_stream << sensor_sv_current.yaw << ",";
    output_stream << sensor_sv_current.gps_alt << ",";
    output_stream << sensor_sv_current.radar_alt << ",";
    output_stream << sensor_sv_current.roll_rate << ",";
    output_stream << sensor_sv_current.pitch_rate << ",";
    output_stream << sensor_sv_current.yaw_rate << ",";
    output_stream << sensor_sv_current.gps_alt_rate << ",";
    output_stream << sensor_sv_current.radar_alt_rate << ",";
    output_stream << sensor_sv_current.gps_lat << ",";
    output_stream << sensor_sv_current.gps_lon << "\n";
    */
}
//-----------------------------------------------------------------------------------------------------------

void open_and_write_a_log(void){
    ofstream log_file_handler;
    int write_count = 0;

        //open the logfile
    log_file_handler.open("log_file_" + to_string(time(0)) + ".txt");
    
    //handle the openfile error
    if(!log_file_handler){
        cout << "ERROR: CANNOT OPEN LOG FILE";
    }

    //write the lines in the log
    /*
    do{
        write_state_vector(log_file_handler);
        write_state_vector(cout);
        usleep(1000 * 100);
        write_count++;
    }while(write_count <= 3);
    */
   
    //close the logfile
    log_file_handler.close();
}
//-----------------------------------------------------------------------------------------------------------