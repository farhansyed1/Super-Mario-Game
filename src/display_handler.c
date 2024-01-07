/*
Takes care of displaying different objects on the OLED screen.

2022-12-10
Abhinav Sasikumar
Farhan Syed

*/
#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

const int OLED_COLS = 128;
const int OLED_ROWS = 32;
#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

uint8_t oled_buffer[32][128];


/*  Displays strings on the screen. There are 4 rows available.

*/
void display_string(int line, char *s)
{
    int i;
    if (line < 0 || line >= 4)
        return;
    if (!s)
        return;

    for (i = 0; i < 16; i++)
        if (*s)
        {
            textbuffer[line][i] = *s;
            s++;
        }
        else
            textbuffer[line][i] = ' ';
}

/*  TODO

*/
uint8_t spi_send_recv(uint8_t data)
{
    while (!(SPI2STAT & 0x08))
        ;
    SPI2BUF = data;
    while (!(SPI2STAT & 1))
        ;
    return SPI2BUF;
}

/*  Resets the screen to either all black or all white pixels

*/
void reset_buffer(int blackOrWhite)
{
    if (blackOrWhite == 1)
    {
        for (int i = 0; i < OLED_ROWS; i++)
        {
            for (int j = 0; j < OLED_COLS; j++)
            {
                oled_buffer[i][j] = 1;
            }
        }
    }
    else
    {
        for (int i = 0; i < OLED_ROWS; i++)
        {
            for (int j = 0; j < OLED_COLS; j++)
            {
                oled_buffer[i][j] = 0;
            }
        }
    }
}

/*  Clears specific pixels that are associated to a sprite object.

*/
void clear_pixels(int x, int y, int columns, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            oled_buffer[i + y][j + x] = 0;
        }
    }
}

/*  Sets Marios different display states (standing, jumping or walking)

*/
void setMario(int direction, int x, int y, uint8_t mario[10][10])
{
    if (direction == 0)
    {
        for (int i = 0; i < playerSize; i++)
        {
            for (int j = 0; j < playerSize; j++)
            {
                oled_buffer[i + y][j + x] = mario[i][j];
            }
        }
    }
    else
    {
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                oled_buffer[i + y][j + x] = mario[i][9 - j];
            }
        }
    }
}

/*  Places Mario on the screen.

*/
void place_mario(int x, int y)
{
    if (myMario.walking == 1)
    {
        setMario(myMario.direction, x, y, playerwalk);
    }
    else
    {
        if (myMario.falling == 1)
        {
            setMario(myMario.direction, x, y, playerjump);
        }
        else
        {
            setMario(myMario.direction, x, y, player);
        }
    }
}

/*  Places the level name on the bottom right corner of the screen.

*/
void place_text(int x, int y, int level)
{
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (level == 1)
            {
                oled_buffer[i + y][j + x] = level1Text[i][j];
            }
            else
            {
                oled_buffer[i + y][j + x] = level2Text[i][j];
            }
        }
    }
}

/*  Used to place an object when an object is entering from the right edge of the screen.

*/
void place_object_rightEdge(int x, int y, int columnsToDisplay, int typeOfObject)
{

    switch (typeOfObject)
    {
    case 0: // BLOCK
        for (int i = 0; i < obstacleSize; i++)
        { // rows
            for (int j = 0; j < columnsToDisplay; j++)
            { // columns
                oled_buffer[i + y][j + x] = block[i][j];
            }
        }
        break;
    case 1: // BOMB
        for (int i = 0; i < obstacleSize; i++)
        {
            for (int j = 0; j < columnsToDisplay; j++)
            {
                oled_buffer[i + y][j + x] = bomb[i][j];
            }
        }
        break;
    case 2: // COIN
        for (int i = 0; i < obstacleSize; i++)
        {
            for (int j = 0; j < columnsToDisplay; j++)
            {
                oled_buffer[i + y][j + x] = coin[i][j];
            }
        }
        break;
    case 3: // FLAG
        for (int i = 0; i < flagHeight; i++)
        {
            for (int j = 0; j < columnsToDisplay; j++)
            {
                oled_buffer[i + y][j + x] = flag[i][j];
            }
        }
        break;
    case 4: // HOLE
        for (int i = 0; i < 1; i++)
        {
            for (int j = 0; j < columnsToDisplay; j++)
            {
                oled_buffer[i + y][j + x] = hole[i][j];
            }
        }
        break;
    }
}

/*  Used to place an object when an object is disappearing from the left edge of the screen.

*/
void place_object_leftEdge(int x, int y, int columnsToDisplay, int typeOfObject)
{
    switch (typeOfObject)
    {
    case 0: // BLOCK
        for (int j = columnsToDisplay; j < obstacleSize; j++)
        {
            for (int i = 0; i < obstacleSize; i++)
            {
                oled_buffer[y + i][x + j - columnsToDisplay] = block[i][j];
            }
        }
        break;
    case 1: // BOMB
        for (int j = columnsToDisplay; j < obstacleSize; j++)
        {
            for (int i = 0; i < obstacleSize; i++)
            {
                oled_buffer[i + y][j + x - columnsToDisplay] = bomb[i][j];
            }
        }
        break;
    case 2: // COIN
        for (int j = columnsToDisplay; j < 8; j++)
        {
            for (int i = 0; i < 8; i++)
            {
                oled_buffer[i + y][j + x - columnsToDisplay] = coin[i][j];
            }
        }
        break;
    case 3: // FLAG
        for (int j = columnsToDisplay; j < 11; j++)
        {
            for (int i = 0; i < 20; i++)
            {
                oled_buffer[i + y][j + x - columnsToDisplay] = flag[i][j];
            }
        }
        break;
    case 4: // HOLE
        for (int j = columnsToDisplay; j < 14; j++)
        {
            for (int i = 0; i < 1; i++)
            {
                oled_buffer[i + y][j + x - columnsToDisplay] = hole[i][j];
            }
        }
        break;
    default:
        break;
    }
}

/*  Draws the floor that Mario stands on.

*/
void set_floor()
{
    for (int i = 0; i < 128; i++)
    {
        oled_buffer[24][i] = 1;
    }
}

/* Converts bit map (2d array) into byte array and writes it to the world buffer.

 */
void setbytearray(uint8_t buf[OLED_ROWS][OLED_COLS])
{
    int m = 0;
    for (int n = 0; n < 4; n++)
    {
        for (int i = 0; i < OLED_COLS; i++)
        {
            int k = 0;
            int l = 0;
            for (int j = n * 8; j < n * 8 + 8; j++)
            {
                l += (1 << k) * buf[j][i];
                k++;
                if (k == 8)
                { // after we have read 8 bits;
                    world_buffer[m] = l;
                    m++;
                    l = 0;
                    k = 0;
                }
            }
        }
    }
}

/* Spawns all the objects  bit map (2d array) into byte array and writes it to the world buffer.

 */
void spawnObjects()
{
    for (int i = 0; i < objectNum; i++)
    {
        if (objectArray[i][6])
        { // If the object is dead
            objectArray[i][3] = 0;
        }
        else if (myMario.screenX > objectArray[i][2] && objectArray[i][0] >= 0)
        {
            objectArray[i][3] = 1; // Appear on screen
        }

        if (objectArray[i][3])
        {
            if (objectArray[i][4] < objectArray[i][8])
            { // Appearing from right edge
                clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][4], objectArray[i][7]);
                objectArray[i][4] = OLED_COLS - objectArray[i][0];
                place_object_rightEdge(objectArray[i][0], objectArray[i][1], objectArray[i][4], objectArray[i][5]);
            }
            else if (objectArray[i][0] < 1)
            { // Appearing from left edge
                clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][10], objectArray[i][7]);
                objectArray[i][10]++;
                if (objectArray[i][5] == 4)
                { // Special case for holes - they disappear instantly
                    objectArray[i][3] = 0;
                    objectArray[i][6] = 1;
                    continue;
                }
                if (objectArray[i][10] >= objectArray[i][8])
                { // When the last column has disappeared
                    objectArray[i][3] = 0;
                    objectArray[i][6] = 1;
                    continue;
                }
                place_object_leftEdge(0, objectArray[i][1], objectArray[i][10], objectArray[i][5]);
            }
            else
            { // Object is in the middle of the screen.
                clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][4], objectArray[i][7]);
                objectArray[i][4] = objectArray[i][8];
                place_object_rightEdge(objectArray[i][0], objectArray[i][1], objectArray[i][4], objectArray[i][5]);
            }
        }
    }
}
// The following code was provided from the lab code

/* TODO

 */
void display_image(int x, uint8_t *buf) {

    int i, j;
    if (x == 1) {
        setbytearray(marioscreen);
    }
    else {
        setbytearray(oled_buffer);
    }
    for (i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0 & 0xF);
        spi_send_recv(0x10 | ((0 >> 4) & 0xF));

        DISPLAY_CHANGE_TO_DATA_MODE;

        for (j = 0; j < 128; j++)
        {
            spi_send_recv(~buf[i * 128 + j]);
        }
    }
}

/* A delay function.

 */
void quicksleep(int cyc)
{
    int i;
    for (i = cyc; i > 0; i--)
        ;
}

/* Initializations for the display buffer.

 */
void display_init(void)
{
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    spi_send_recv(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);

    spi_send_recv(0xAF);
}

/* Updates the display screen. Used when displaying strings.

 */
void display_update(void)
{
    int i, j, k;
    int c;
    for (i = 0; i < 4; i++)
    {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for (j = 0; j < 16; j++)
        {
            c = textbuffer[i][j];
            if (c & 0x80)
                continue;

            for (k = 0; k < 8; k++)
                spi_send_recv(font[c * 8 + k]);
        }
    }
}


