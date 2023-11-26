#define DISPLAY_UPDATE_RATE_HZ          60                 
#define DISPLAY_UPDATE_DELAY_US         (1 / (float)DISPLAY_UPDATE_RATE_HZ) * 1000000

#define DEG2RAD                         M_PI / 180.0               

#define DISPLAY_X_CENTER                WIN_WIDTH / 2
#define DISPLAY_Y_CENTER                WIN_HEIGHT / 2


#define DISPLAY_NUMBER_OF_MODES         2
//-----------------------------------------------------------------------------------------------------------
enum {
        RECT_X = 20,
        RECT_Y = 20,
        RECT_WIDTH = 10,
        RECT_HEIGHT = 10,

        WIN_X = 0,
        WIN_Y = 0,
        WIN_WIDTH = 1280,
        WIN_HEIGHT = 720,
        WIN_BORDER = 1
};

#define XA_ATOM ((Atom) 4)
#define DB_IMPL_NAME "Xdbe"
//-----------------------------------------------------------------------------------------------------------
void display_setup(void);
void display_close(void);
void *display_refresh(void *arg);


//-----------------------------------------------------------------------------------------------------------