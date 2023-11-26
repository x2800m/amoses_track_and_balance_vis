#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <mutex>
#include <algorithm>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xcms.h>

#include "../inc/tab.hpp"
#include "../inc/display.hpp"
#include "../inc/logging.hpp"

//-----------------------------------------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------------------------------------
Display *display;
Window  window;
XEvent  event;
int     screen;
KeySym  key;

GC      app_gc;
Colormap display_colormap; 
XColor  black, white, red, green, orange, gray, deep_sky_blue, chocolate, dummy;
Font font;

unsigned int display_frame_number = 0;
char tmp_disp_string[1024];
char display_mode = 0;

time_t current_time = time(NULL);
struct tm *local_time = localtime(&current_time);
char time_string[64];


extern struct balance_info current_meas;
extern char run_flag;
extern int tab_fd;
//-----------------------------------------------------------------------------------------------------------
void display_draw_bullseye_and_plot_results(void){
    unsigned int bullseye_draw_tracker = 0;
    char display_string[100];

    //Reset line and color paramters
    XSetLineAttributes(display, DefaultGC(display, screen), 1, LineSolid, CapButt, JoinBevel);
    XSetForeground(display, DefaultGC(display,screen), white.pixel);   

    XSetForeground(display, DefaultGC(display,screen), gray.pixel);  
    //Draw the outer clock ring
    do{
        XDrawArc(display, window, DefaultGC(display,screen), 
                    DISPLAY_CLOCK_X - ((DISPLAY_CLOCK_DIAMETER - (bullseye_draw_tracker * DISPLAY_RING_SPACING)) / 2),
                    DISPLAY_CLOCK_Y - ((DISPLAY_CLOCK_DIAMETER - (bullseye_draw_tracker * DISPLAY_RING_SPACING))  / 2),
                    (DISPLAY_CLOCK_DIAMETER - (bullseye_draw_tracker * DISPLAY_RING_SPACING)),
                    (DISPLAY_CLOCK_DIAMETER - (bullseye_draw_tracker * DISPLAY_RING_SPACING)),
                    0,
                    360*64);             
        bullseye_draw_tracker++;
    }while(bullseye_draw_tracker < 10);

    //Draw the radial lines
    bullseye_draw_tracker = 0;
    do{
        XDrawLine(display, window, DefaultGC(display,screen), 
            DISPLAY_CLOCK_X,
            DISPLAY_CLOCK_Y,
            DISPLAY_CLOCK_X + ((DISPLAY_CLOCK_DIAMETER / 2) * sin(DEG2RAD * bullseye_draw_tracker * 30)),
            DISPLAY_CLOCK_Y - ((DISPLAY_CLOCK_DIAMETER / 2) * cos(DEG2RAD * bullseye_draw_tracker * 30)));
        bullseye_draw_tracker++;
    }while(bullseye_draw_tracker < 12);
    XSetForeground(display, DefaultGC(display,screen), white.pixel); 

    //Plot the current vibration levels. 
    XSetForeground(display, DefaultGC(display,screen), red.pixel);  
    XFillArc(display, window, DefaultGC(display,screen), 
            DISPLAY_CLOCK_X + ((DISPLAY_VIBE_POINT_DIAMETER / -2) + (0.5 * DISPLAY_CLOCK_DIAMETER * current_meas.ips * sin(DEG2RAD * current_meas.clock_dir * 30 ))),
            DISPLAY_CLOCK_Y - ((DISPLAY_VIBE_POINT_DIAMETER / 2) + (0.5 * DISPLAY_CLOCK_DIAMETER * current_meas.ips * cos(DEG2RAD * current_meas.clock_dir * 30 ))),
            DISPLAY_VIBE_POINT_DIAMETER,
            DISPLAY_VIBE_POINT_DIAMETER,
            0,
            360*64);    
    XSetForeground(display, DefaultGC(display,screen), white.pixel); 

    //Display the text representation of the data.
    sprintf(display_string, "%.0f RPM  << %.3f IPS @ %.1f >>", current_meas.rpm, current_meas.ips, current_meas.clock_dir);
    XDrawString(display, window, DefaultGC(display,screen),
                    DISPLAY_CHADWICK_TEXT_X,
                    DISPLAY_CHADWICK_TEXT_Y, 
                    display_string, 
                    strlen(display_string) );

    //Display the timestamp
    size_t ret = strftime(time_string, sizeof(time_string), "%c", local_time);
    sprintf(display_string, "%s", time_string);
    XDrawString(display, window, DefaultGC(display,screen),
                    DISPLAY_TIMESTAMP_X,
                    DISPLAY_TIMESTAMP_Y, 
                    display_string, 
                    strlen(display_string) );

}
//-----------------------------------------------------------------------------------------------------------
void display_close(void){
    cout << "Closing the display window\n";
    XDestroyWindow(display, window);        // destroy window
    XCloseDisplay(display);                 // close connection to server
}
//-----------------------------------------------------------------------------------------------------------
void *display_refresh(void *arg){
    char pressedkey = 0;

    while(1){
        XClearWindow(display, window);      //Clear the window.
        //***************************************************************************************************
        switch(display_mode){
            
            case DISPLAY_CLOCK:
                //display_draw_mode_text();
                display_draw_bullseye_and_plot_results();
                break;
            
            default:
                //display_draw_mode_text();
                break;
        }
        
        //***************************************************************************************************
        display_frame_number++; //Track the frame number (maybe display somewhere to show it's running?)
        XFlush(display);        //Write results to the screen.

        while(XPending(display)){
            XNextEvent(display, &event);
            if(event.type == KeyPress){
                //printf( "KeyPress: %x\n", event.xkey.keycode );
                switch(event.xkey.keycode){
                    case 0x18:      // Q to quit
                        run_flag = 0;
                        break;
                    case 0x1E:      // U to select the next channel "UP"

                        break;
                    case 0x28:      // D to select the next channel "DOWN"

                        break;
                    case 0x1F:      // I to zoom in

                        break;
                    case 0x20:      // O to zoom out

                        break;
                    case 0x36:      // C to cage the AHRS (NOT WORKING)
                        //write(ahrs_fd, AHRS_TARE, sizeof(AHRS_TARE));                      // Null the attitude offsets. 
                        break;
                    case  0x3A:     // M to cycle the display mode
                        //display_cycle_mode();
                        break;
                    case 0x09:      // ESC to blank the display
                        run_flag = 0;
                        break;
                    case 0x27:
                        write(tab_fd, "s", 1);          //Toggle Strobe
                        break;
                    default:
                        break;
                }
            }
        }
        usleep(DISPLAY_UPDATE_DELAY_US);         
    }
    pthread_exit(NULL);
}
//-----------------------------------------------------------------------------------------------------------
void display_setup(void){
    XGCValues values;
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        printf("Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);

    // create window
    window = XCreateSimpleWindow(display, RootWindow(display, screen), WIN_X, WIN_Y, WIN_WIDTH, WIN_HEIGHT,
            WIN_BORDER, BlackPixel(display, screen), WhitePixel(display, screen));

    //Make the window fullscreen
    Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    Atom wm_state   = XInternAtom (display, "_NET_WM_STATE", true );
    Atom wm_fullscreen = XInternAtom (display, "_NET_WM_STATE_FULLSCREEN", true );

    XChangeProperty(display, window, wm_state, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&wm_fullscreen, 1);

    XSetWMProtocols(display, window, &del_window, 1);

    XSelectInput(display, window, ExposureMask | KeyPressMask);     // select kind of events we are interested in
    XMapWindow(display, window);                                    // display the window

    app_gc = XCreateGC(display, window, 0, &values);
    display_colormap = DefaultColormap(display, DefaultScreen(display));
    XAllocNamedColor(display, display_colormap, "red", &red, &red);
    XAllocNamedColor(display, display_colormap, "black", &black, &black);
    XAllocNamedColor(display, display_colormap, "green", &green, &green);
    XAllocNamedColor(display, display_colormap, "white", &white, &white);
    XAllocNamedColor(display, display_colormap, "orange", &orange, &orange);
    XAllocNamedColor(display, display_colormap, "dimgray", &gray, &gray);
    XAllocNamedColor(display, display_colormap, "DeepSkyBlue", &deep_sky_blue, &deep_sky_blue);
    XAllocNamedColor(display, display_colormap, "chocolate", &chocolate, &chocolate);

    XSetWindowBackground(display, window, black.pixel);
    XSetBackground(display, DefaultGC(display,screen), black.pixel);
    XSetForeground(display, DefaultGC(display,screen), green.pixel);

    font = XLoadFont(display, "12x24kana");
    XSetFont(display, DefaultGC(display,screen), font);

    XFlush(display);

   return;
}
//-----------------------------------------------------------------------------------------------------------
void display_draw_mode_text(void){
    char display_mode_text_string[30];

    sprintf(display_mode_text_string, "Mode: %d", display_mode);
    XDrawString(display, window, DefaultGC(display,screen),
                    50,
                    90, 
                    display_mode_text_string, 
                    strlen(display_mode_text_string) );

}
//-----------------------------------------------------------------------------------------------------------
void display_cycle_mode(void){
    if(display_mode + 1 < DISPLAY_NUMBER_OF_MODES){
        display_mode ++;
    }else{
        display_mode = 0;
    }
}
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------