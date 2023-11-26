
#define TAB_UART_PORT          "/dev/ttyUSB0"
#define TAB_BAUD_RATE          B115200
#define TAB_UPDATE_RATE_HZ     30
#define TAB_POLL_DELAY_US      (1 / (float)TAB_UPDATE_RATE_HZ) * 1000000

#define TAB_CONSOLE_DEBUG      0
//-----------------------------------------------------------------------------------------------------------

void *tab_update(void *arg);
int tab_connect(void);
void tab_close(void);