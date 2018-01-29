
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define PRESS_EVENT 120
#define MENU_LINES 4
#define MENU_ITEMS 7
#define VOLUME 0
#define BALANCE 1
#define INPUTS_NUMBER 4
#define MAX_DELAY_RELAY 40
#define TEMBRO_MENU_LINES 4

#define EXIT 0
#define DELAY_IN_TURN_ON 1
#define BYE 2
#define VOLUME 3
#define SET_BALANCE 4
#define INPUTS 5
#define OUTPUT_OFF 6
#define BACK 7
#define INPUTS_SELECTOR_ITEM 8
#define BALANCE_ITEM 9
#define TEMBRO_ITEM 10
#define DELAY_ITEM 11
#define ITEM_1 112
#define ITEM_2  13
#define INPUT_1 14
#define INPUT_2 15
#define INPUT_3 16
#define USB 17
#define SECONDS 18
#define INPUTS_SELECTOR 19
#define TEMBRO 20
#define GET_BACK 21
#define TURN_ON 22
#define TUNE 23
#define TURN_OFF 24
#define TURNED_ON 25
#define TURNED_OFF 26
#define RETURN 27
#define LOW_FREQ 28
#define HIGH_FREQ 29
#define TEMBRO_ADJUST 30
#define MINUS_6DB 31
#define MINUST_4DB 32
#define MINUS_2DB 33
#define ZERO_DB 34
#define PLUS_2DB 35
#define PLUS_4DB 36
#define PLUS_6DB 37

int volume = 0;
byte tembro = 136;
bool tembroOn = 0;
byte relayDelayVal = 0;
int balance = 0;

int volumeL = 0;
int volumeR = 0;  
unsigned int counter = 0;
bool menu_event = 0;

long encoder_position = 0;
int menu_position;
bool selector_event = 0;
bool balance_event = 0;
bool tembro_event = 0;

bool relay_delay_event = 0;
bool item1_event = 0;
bool item2_event = 0;
bool tune_tembro_event = 0;
bool lf_event = 0;
bool hf_event = 0;

bool balanceActivated = 0;
bool selectorActivated = 0;
bool tembroActivated = 0;

long last_encoder = 0;
byte selector = 1;

