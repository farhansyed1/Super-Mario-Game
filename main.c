/*
Main file that selects which screen (main menu, game or highscore) to display.

2022-12-10
Abhinav Sasikumar
Farhan Syed

*/

#include <stdint.h>
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" // Header file

void *stdout = (void *)0;

uint8_t levelCounter = 0;
int highScores[] = {0, 0, 0};
char highScoreNames[3][4] = {{0, 0, 0, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}};
uint8_t menuHandler = 0;

/* Resets the strings that are displayed. 
*/
void resetDisplayString() {
    display_string(0, "");
    display_string(1, "");
    display_string(2, "");
    display_string(3, "");
    display_update();
}

/*
Resets the game state and sets the level to level 1
*/
void resetGameState() {
    reset_buffer(0);
    jump_state = 1;

    for (int i = 0; i < objectNum; i++)
    {
        for (int j = 0; j < objectParamNum; j++)
        {
            objectArray[i][j] = levelOne[i][j];
        }
    }
    myMario.x = 20;
    myMario.y = 14;
    myMario.screenX = 20;
    myMario.lives = 3;
    myMario.invincible = 0;
    myMario.direction = 0;
    myMario.score = 0;
    levelCounter = 0;
    timeOut = 0;
    myCounter = 0;
    scoreCounter = 0;
}

void showHighscores()
{
    char rankOne[20] = "1. ";
    char rankTwo[20] = "2. ";
    char rankThree[20] = "3. ";
    char scoreOne[5];
    char scoreTwo[5];
    char scoreThree[5];

    // Rank 1
    snprintf(scoreOne, sizeof scoreOne, "%d", highScores[0]);
    strcat(rankOne, highScoreNames[0]);
    strcat(rankOne, " ");
    strcat(rankOne, scoreOne);

    // Rank 2
    snprintf(scoreTwo, sizeof scoreTwo, "%d", highScores[1]);
    strcat(rankTwo, highScoreNames[1]);
    strcat(rankTwo, " ");
    strcat(rankTwo, scoreTwo);

    // Rank 3
    snprintf(scoreThree, sizeof scoreThree, "%d", highScores[2]);
    strcat(rankThree, highScoreNames[2]);
    strcat(rankThree, " ");
    strcat(rankThree, scoreThree);

    // Display highscores
    resetDisplayString();
    display_string(0, "HIGHSCORES");
    display_string(1, rankOne);
    display_string(2, rankTwo);
    display_string(3, rankThree);

    int t = 1;
    while (1)
    {
        int btns = getbtns();
        display_update();

        if (t % 300 == 0)
        {
            if (btns & 0b100)
            {
                resetDisplayString();
                break;
            }
        }
        t++;
    }
}

void insertName()
{
    reset_buffer(1);
    //setMario(0, 0, 0, emptyMario[10][10]);
    display_image(0, world_buffer);
    int btns = getbtns();
    char letters[] = {'A', 'A', 'A'};
    char *underscore = "_";
    int t = 1;
    int index = 0;
    int position = 4;

    // Calculate ranking
    for (int i = 0; i < 3; i++)
    {
        if (myMario.score > highScores[i])
        {
            position = i;
            break;
        }
    }
    // Adjust other rankings
    if (position < 3)
    {
        switch (position)
        {
        case 0:
            if (highScores[0] > 0)
            {
                int copy = highScores[1];
                highScores[1] = highScores[0];
                highScores[2] = copy;
                char nameCopy[4];

                for (int i = 0; i < 4; i++)
                {
                    nameCopy[i] = highScoreNames[1][i];
                    highScoreNames[1][i] = highScoreNames[0][i];
                    highScoreNames[2][i] = nameCopy[i];
                }
            }
            break;
        case 1:
            if (highScores[1] > 0)
            {
                highScores[2] = highScores[1];
                for (int i = 0; i < 4; i++)
                {
                    highScoreNames[2][i] = highScoreNames[1][i];
                }
            }
            break;
        default:
            break;
        }
        highScores[position] = myMario.score;
    }
    // The player can insert 3 initials by cycling through letters.
    while (1)
    {
        if (index == 1)
        {
            underscore = " _";
        }
        else if (index == 2)
        {
            underscore = "  _";
        }
        display_string(0, "Insert initials:");
        display_string(2, letters);
        display_string(3, underscore);
        if (t % 100 == 0)
        {
            if (btns & 0b100)
            { // Button 4, Confirms letter
                if (index <= 2)
                {
                    index++;
                }
            }
            if (btns & 0b010)
            { // Button 3, cycles through letters (to the left in the alphabet)
                if (letters[index] > 65)
                {
                    letters[index]--;
                }
            }
            if (btns & 0b001)
            { // Button 2, cycles through letters (to the right in the alphabet)
                if (letters[index] < 90)
                {
                    letters[index]++;
                }
            }
        }
        // Confirming the name
        if (index > 2)
        {
            if (position < 3)
            {
                for (int i = 0; i < 3; i++)
                {
                    highScoreNames[position][i] = letters[i];
                }
            }
            resetDisplayString();
            break;
        }
        t++;
        btns = getbtns();
        display_update();
    }
}

void titleScreen()
{
    display_image(1, world_buffer);
    quicksleep(20000000);
}

void gameOverScreen()
{
    display_image(2, world_buffer);
}

void startMenu()
{
    int btns = getbtns();
    int t = 1;
    int u = 1;
    resetDisplayString();

    while (1)
    {
        resetGameState();
        btns = getbtns();
        if (menuHandler == 0)
        { // Play game
            if (t % 70 == 0)
            {
                if (btns & 0b100)
                {
                    display_string(1, "");
                    display_string(2, "");
                    display_update();
                    while (1)
                    {
                        place_text(90, 25, 1);
                        display_image(0, world_buffer);
                        while (levelCounter < 2 && myMario.lives > 0)
                        {
                            labwork(); // Game loop
                        }
                        break;
                    }
                    if (myMario.lives > 0)
                    {
                        insertName();
                        showHighscores();
                    }
                    else
                    {
                        resetDisplayString();
                        display_string(1,"  GAME OVER");
                        display_update();
                        quicksleep(10000000);
                        
                        resetDisplayString();                        
                    }
                }
                display_string(1, "* PLAY GAME");
                display_string(2, "  HIGHSCORES");
                display_update();

                if (btns & 0b001)
                {
                    menuHandler++;
                }
            }
        }
        if (menuHandler == 1)
        { // Highscores
            if (u % 70 == 0)
            {
                if (btns & 0b010)
                {
                    menuHandler--;
                }
                if ((btns & 0b100))
                {
                    resetDisplayString();
                    while (1)
                    {
                        showHighscores();
                        menuHandler = 1;
                        break;
                    }
                }
            }
            display_string(1, "  PLAY GAME");
            display_string(2, "* HIGHSCORES");
            display_update();
        }
        t++;
        u++;
    }
}

int main(void)
{
    /*
  This will set the peripheral bus clock to the same frequency
  as the sysclock. That means 80 MHz, when the microcontroller
  is running at 80 MHz. Changed 2017, as recommended by Axel.
*/
    SYSKEY = 0xAA996655; /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA; /* Unlock OSCCON, step 2 */
    while (OSCCON & (1 << 21))
        ;                 /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
    while (OSCCON & (1 << 21))
        ;         /* Wait until PBDIV ready */
    SYSKEY = 0x0; /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    /* SPI2STAT bit SPIROV = 0; */
    SPI2STATCLR = 0x40;
    /* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
    /* SPI2CON bit MSTEN = 1; */
    SPI2CONSET = 0x20;
    /* SPI2CON bit ON = 1; */
    SPI2CONSET = 0x8000;

    resetGameState();

    labinit();
    display_init();
    titleScreen();

    startMenu(0);

    return 0;
}