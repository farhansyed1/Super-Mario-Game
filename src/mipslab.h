/* mipslab.h
   Header file for all files.

   2022-12-10
   Abhinav Sasikumar
   Farhan Syed 

/* Declare display-related functions from display_handler.c */
void display_image(int x, uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
void place_mario(int x, int y);
void clear_pixels(int x, int y, int columns, int rows);
void labinit(void);
void spawnObjects();
int checkCollisionObstacle(int direction);
void set_floor();

struct marioAttributes
{
   int x;
   int y;
   int screenX;
   uint8_t direction;
   uint8_t walking;
   uint8_t falling;
   uint8_t lives;
   uint8_t invincible;
   int score;
};

struct marioAttributes myMario;

/* Declare lab-related functions from mipslabfunc.c */
void labwork(void);
void quicksleep(int cyc);
void handle_jump();
void place_text(int x, int y, int level);
void reset_buffer(int blackOrWhite);

void setMario(int direction, int x, int y, uint8_t mario[10][10]);

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug(volatile int *const addr);


/* Declare bitmap array containing font */
extern const uint8_t const font[128 * 8];
/* Declare text buffer for display output */
extern char textbuffer[4][16];
/* Declare bitmap array containing icon */
extern uint8_t player[10][10];
extern uint8_t bomb[8][8];
extern uint8_t world_buffer[512]; // 128 *32 /8 = 512 bytes
extern uint8_t block[8][8];
extern uint8_t coin[8][8];

extern uint8_t flag[20][12];
extern uint8_t oled_buffer[32][128];
extern uint8_t hole[1][14];

extern int objectArray[30][11];
extern int levelOne[30][11];
extern int levelTwo[30][11];
extern int emptyArray[30][11];

extern uint8_t jump_state;
extern uint8_t emptyMario[10][10];
extern uint8_t playerwalk[10][10];
extern uint8_t playerjump[10][10];

extern uint8_t level1Text[7][32];
extern uint8_t level2Text[7][32];
extern uint8_t marioscreen[32][128];

extern uint8_t levelCounter;
extern int myCounter;
extern int timeOut;
extern int scoreCounter;
extern volatile int *portE;

extern uint8_t const playerSize;
extern uint8_t const obstacleSize;
extern uint8_t const flagHeight;
extern uint8_t const flagWidth;
extern uint8_t const screenWidth;
extern uint8_t const screenHeight;
extern uint8_t const holewidth;
extern uint8_t const objectNum;
extern uint8_t const objectParamNum;
extern uint8_t const levelTextHeight;
extern uint8_t const levelTextWidth;

void delay(int);
int getbtns(void);
int getsw(void);
void enable_interrupt(void);