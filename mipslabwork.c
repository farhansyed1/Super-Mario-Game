/*
Takes care of all functions such as collisions and movement.
Makes use of interrupts.

2022-12-10
Abhinav Sasikumar
Farhan Syed

*/

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

int scoreCounter = 0;

uint8_t jump_up = 1;
int invincibleCount = 0;

volatile int *portE = (volatile int *)0xbf886110;

/*
Moves obstacles based on mario's movement.
*/
void moveObstacles()
{
    for (int i = 0; i < objectNum; i++)
    {
        if (objectArray[i][5] == 3 && objectArray[i][3] == 1 && objectArray[i][0] == 114)
        {
            break;
        }
        else if (objectArray[i][3])
        {
            clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][4], objectArray[i][7]);
            objectArray[i][0] -= 1;
        }
    }
}

/*
Checks for collisions between mario and obstacles based on cases for each direction

*/
int checkCollisionObstacle(int direction)
{
    switch (direction)
    {
    case 0: // top of block
        for (int j = 1; j < 9; j++)
        { // MARIOS FEET
            if (oled_buffer[myMario.y + 10][myMario.x + j] == 1)
                return 1;
        }
    case 1: // right
        for (int j = 0; j < 10; j++)
        {
            if (oled_buffer[myMario.y + j][myMario.x + 10 + 1] == 1)
            {
                return 1;
            }
        }
        break;
    case 2: // left
        for (int j = 0; j < 10; j++)
        {
            if (oled_buffer[myMario.y + j][myMario.x - 1] == 1)
            {
                return 1;
            }
        }
        break;
    case 3: // bottom of block
        for (int j = 0; j < 10; j++)
        {
            if (oled_buffer[myMario.y - 1][myMario.x + j] == 1)
                return 1;
        }
    default:
        return 0;
    }
    return 0;
}

int collisionCounter = 0;
int timeOut = 0;

/*
Checks for collisions with enemies and coins. Also handles enemy collisions with other obstacles.
*/
void checkCoinEnemyCollision()
{
    for (int i = 0; i < objectNum; i++)
    {

        // Handling enemy collisions
        if (objectArray[i][3] && objectArray[i][5] == 1)
        {
            // Killing an enemy by landing on top of them
            for (int j = 0; j < 10; j++)
            {
                if (myMario.y + 10 == objectArray[i][1] && myMario.x + j == objectArray[i][0])
                {
                    clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][8], objectArray[i][7]);
                    objectArray[i][6] = 1;
                    objectArray[i][3] = 0;
                }
            }
            // Collision with an enemy
            if (myMario.invincible == 0 && objectArray[i][5] == 1 && objectArray[i][3])
            {
                for (int j = 2; j < 10; j++)
                {
                    if (myMario.y + j == objectArray[i][1] &&
                        (myMario.x - 1 == objectArray[i][0] + 8 || myMario.x + 11 == objectArray[i][0]))
                    { // left or right side
                        clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][8], objectArray[i][7]);
                        myMario.lives--;
                        myMario.invincible = 1;
                        invincibleCount = 0;
                        break;
                    }
                }
            }
            // If enemy collides with an object, it will change direction
            if (objectArray[i][9] == 1)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (oled_buffer[objectArray[i][1] + j][objectArray[i][0] - 1] == 1)
                    {
                        objectArray[i][9] = -1;
                        break;
                    }
                }
            }
            else if (objectArray[i][9] == -1)
            { // right collision
                for (int j = 0; j < 8; j++)
                {
                    if (oled_buffer[objectArray[i][1] + j][objectArray[i][0] + 9] == 1)
                    {
                        objectArray[i][9] = 1;
                    }
                    else if (objectArray[i][0] > 118)
                        objectArray[i][9] = 1;
                }
            }
            // Moving an enemy
            clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][4], objectArray[i][7]);
            if (timeOut % 5 == 0 && objectArray[i][0] > 0)
            {
                objectArray[i][0] -= objectArray[i][9];
            }
            timeOut++;
        }
        // Handling coin collisions
        else if (objectArray[i][5] == 2 && objectArray[i][3])
        {
            for (int k = 0; k < 10; k++)
            {
                if (myMario.y + k >= objectArray[i][1] && myMario.y + k <= objectArray[i][1] + objectArray[i][8]) {
                    for (int j = 0; j < 12; j++) {
                        if ((myMario.x + j >= objectArray[i][0] && myMario.x + j <= objectArray[i][0] + objectArray[i][8]) || 
                            (myMario.x - j >= objectArray[i][0] && myMario.x - j <= objectArray[i][0] + objectArray[i][8])) {

                            clear_pixels(objectArray[i][0], objectArray[i][1], objectArray[i][8], objectArray[i][7]);
                            objectArray[i][6] = 1;
                            objectArray[i][3] = 0;
                            if (collisionCounter % 75 == 0)
                            {
                                myMario.score++;
                                collisionCounter = 0;
                            }
                            collisionCounter++;
                        }
                    }
                }
            }
        }
    }
}

/*
Checks whether mario has fallen down in a gap.
*/
void checkBelowFloor()
{
    if (myMario.y > 19)
    {
        clear_pixels(myMario.x, myMario.y, playerSize, playerSize);
        myMario.lives--;
        myMario.x = 10;
        myMario.y = 14;
        myMario.falling = 1;
    }
}

/*
Handles falling movement if mario is not standing on anything
*/
void gravity()
{
    if (!checkCollisionObstacle(0))
    {
        clear_pixels(myMario.x, myMario.y, playerSize, playerSize);
        myMario.y += 1;
    }
}
/*
Changes to level 2
*/
void setLevelTwo()
{
    reset_buffer(0);
    myMario.x = 20;
    myMario.y = 10;
    myMario.screenX = 20;
    myMario.invincible = 0;
    place_text(90, 25, 2);

    for (int i = 0; i < objectNum; i++)
    {
        for (int j = 0; j < objectParamNum; j++)
        {
            objectArray[i][j] = levelTwo[i][j];
        }
    }
}

/*
Sets the levelarray to empty.
*/
void setEmptyArray()
{
    for (int i = 0; i < objectNum; i++)
    {
        for (int j = 0; j < objectParamNum; j++)
        {
            objectArray[i][j] = emptyArray[i][j];
        }
    }
}

int myCounter = 0;

/*
Contains logic for moving mario.
*/
void moveMario(int direction)
{
    switch (direction)
    {
    case -1: // FALLING DOWN
        if (!checkCollisionObstacle(0))
        {
            clear_pixels(myMario.x, myMario.y, playerSize, playerSize);

            myMario.y += 1;
            myMario.falling = 1;
        }
        break;
    case 0: // UP
        if (checkCollisionObstacle(3) || myMario.y < 1)
        {
            break;
        }
        clear_pixels(myMario.x, myMario.y, playerSize, playerSize);
        myMario.y -= 1;
        myMario.falling = 1;
        break;
    case 1:
        // LEFT
        if (myMario.x > 0)
        {
            if (!checkCollisionObstacle(2))
            {
                clear_pixels(myMario.x, myMario.y, playerSize, playerSize);

                myMario.x -= 1;
                myMario.screenX -= 1;
                myMario.walking = 1;
                myMario.direction = 1;
            }
        }
        break;
    case 2:
        // RIGHT
        if (objectArray[objectNum-1][3] && objectArray[objectNum-1][0] == 114)
        {
            if (checkCollisionObstacle(1) && myMario.x >= 105)
            {
                if (levelCounter == 0)
                {
                    setLevelTwo();
                    levelCounter++;
                }
                else if (levelCounter == 1)
                {
                    scoreCounter /= 100;
                    scoreCounter = 500 - scoreCounter;
                    myMario.score += scoreCounter;
                    setEmptyArray();
                    levelCounter++;
                }
                break;
            }
            clear_pixels(myMario.x, myMario.y, playerSize, playerSize);

            myMario.x += 1;
            myMario.walking = 1;
            myMario.screenX += 1;
            myMario.direction = 0;
        }
        else if (myMario.x >= 48)
        {
            if (!checkCollisionObstacle(1))
            {
                myMario.screenX += 1;
                myMario.walking = 1;
                myMario.direction = 0;
                moveObstacles();
            }
        }
        else
        {
            if (!checkCollisionObstacle(1))
            {
                clear_pixels(myMario.x, myMario.y, playerSize, playerSize);
                myMario.x += 1;
                myMario.walking = 1;
                myMario.screenX += 1;
                myMario.direction = 0;
            }
        }
    }
}

/*
Handles jumps, mario jumps up 12 blocks and falls 12 blocks.
*/
void handle_jump()
{
    if (myMario.y<23 | myMario.y> 24 && jump_state == 0)
    {
        gravity();
    }

    if (jump_state == 1)
    {
        if (jump_up < 12)
        {
            myMario.falling = 1;
            moveMario(0);
            jump_up++;
        }
        if (jump_up >= 12 && jump_up < 24)
        {
            myMario.falling = 1;
            moveMario(-1);
            jump_up++;
        }
        if (jump_up == 24)
        {
            myMario.falling = 1;
            jump_state = 0;
            jump_up = 0;
        }
    }
}

/*
Displays number of lives remaining on the LEDs
*/
void showLives()
{
    switch (myMario.lives)
    {
    case 3:
        *portE = 7;
        break;
    case 2:
        *portE = 3;
        break;
    case 1:
        *portE = 1;
        break;
    case 0:
        *portE = 0;
        break;
    }
}

/* Interrupt Service Routine. This is where all the buttons presses are handled and the respective movement called.  */
void user_isr(void)
{
    int buttonValue = getbtns();
    if (IFS(0) & 0x100)
    {
        scoreCounter++;
        IFS(0) &= ~0x100; // resetting the 8th bit

        if (buttonValue & 0b100)
        { // button 4
            if (jump_state == 0)
            {
                jump_state = 1;
                jump_up = 0;
            }
        }
        if (buttonValue & 0b010)
        { // button 3
            moveMario(1);
        }
        if (buttonValue & 0b001)
        { // button 2
            moveMario(2);
        }

        if (myMario.invincible == 1)
        {
            invincibleCount++;
            if (invincibleCount % 200 == 0)
            {
                myMario.invincible = 0;
            }
        }
        handle_jump();
        checkCoinEnemyCollision();

        showLives();
        checkBelowFloor();
        place_mario(myMario.x, myMario.y);
        set_floor();
        spawnObjects();
    }
}

/* Lab-specific initialization goes here. Initializes interrupts. */
volatile int *trisE = (volatile int *)0xbf886100;

void labinit(void)
{
    *trisE &= 0xffffff00;
    TRISD |= 0x00fe0; /// 0000 0000 1111 1110 0000
    TRISFSET = 0x2;

    IFS(0) &= ~0x100;

    T2CON = 0x70;                 // 256 prescaling, 0111 0000 bits 6-4      bit 1 is 0 for internal clock
    PR2 = (80000000 / 256) / 100; // 80 mil / 256 / 10 = 31250, interrupts 10 times per second
    TMR2 = 0;

    T2CON |= 0x8000; //  bit 15 = turning timer on

    IEC(0) |= 0x100; // 8th bit = 1,  0001 0000 0000
    IPC(2) |= 0x1f;  // 0001 1111, last 5 bits should be 1

    enable_interrupt();
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    display_image(0, world_buffer);
    myMario.walking = 0;
    myMario.falling = 0;
}