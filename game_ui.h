/**
 * /file game_ui.h
 * /author Corentin DESTREZ & Valentin GUIBERTEAU
 * /date 13 Apr 2021
 * /brief Source code of the client library
 *
 * This library containts the definition of the function that start the
 * game.
 *
 */

#ifndef GAME_H
#define GAME_H

/**
 * \def GAME_SPEED
 * \brief  number of milli seconds between each ball movement (32000=30fps) 128 000 recommended for playable experience
 *
 */
#define GAME_SPEED 128000

/**
 * \def WIDTH_MAX
 * \brief Screen width
 *
 */
#define WIDTH_MAX 80

/**
 * \def HEIGHT_MAX
 * \brief Screen height
 *
 */
#define HEIGHT_MAX 20

/**
 * \def BRICK_WIDTH
 * \brief Brick width
 *
 */
#define BRICK_WIDTH 2

/**
 * \def SIDE_WALL_WIDTH
 * \brief side walls width from screen border
 *
 */
#define SIDE_WALL_WIDTH 2

/**
 * \def UPPER_WALL_HEIGHT
 * \brief upper wall height from screen border
 *
 */
#define UPPER_WALL_HEIGHT 3

/**
 * \def RACKET_SIZE
 * \brief size of the racket
 *
 */
#define RACKET_SIZE 7

/**
 * \def EMPTY
 * \brief caracter used for void
 *
 */
#define EMPTY ' '

/**
 * \def WALL
 * \brief caracter used for walls
 *
 */
#define WALL 'W'

/**
 * \def BRICK
 * \brief caracter used for bricks
 *
 */
#define BRICK 'B'

/**
 * \def BALL
 * \brief caracter used for the ball
 *
 */
#define BALL 'o'

/**
 * \def RACKET
 * \brief caracter used for the racket
 *
 */
#define RACKET '~'

/**
 * \def ERROR_C
 * \brief caracter used for the error caracter
 *
 */
#define ERROR_C 'R'

/**
 * \def E_TRIGGER
 * \brief caracter used to trigger the death of a ball
 *
 */
#define E_TRIGGER 'E'

/**
 * \struct ball_t
 * \brief Structure containing all ball information.
 *
 * posX and posY are the X and Y coordonates of the ball on the screen
 * moving set if the ball move or not
 * dirX value -1 or 1, describe if the ball is moving to the positive or negative X coordonates
 * dirY value -1 or 1, describe if the ball is moving to the positive or negative Y coordonates
 */
typedef struct ball_t
{
	int posX;
	int posY;
    int moving;
    int dirX;
    int dirY;
}ball_t;

/**
 * \struct brick_t
 * \brief Structure containing all ball information.
 *
 * posX and posY are the X and Y coordonates of the beginning of the brick on the screen
 * width is the width of the racket (2 by default)
 */
typedef struct brick_t
{
	int posX;
	int posY;
	int width;
}brick_t;

/**
 * \struct racket_t
 * \brief Structure containing all racket information.
 *
 * posX and posY are the X and Y coordonates of the racket on the screen
 */
typedef struct racket_t
{
	int posX;
	int posY;
}racket_t;

/**
 * \struct game_t
 * \brief Structure containing all the parameters important to run the game properly.
 *
 * posX and posY are the X and Y coordonates of the ball on the screen
 * moving set if the ball move or not
 * dirX value -1 or 1, describe if the ball is moving to the positive or negative X coordonates
 * dirY value -1 or 1, describe if the ball is moving to the positive or negative Y coordonates
 */
typedef struct game_t
{
	int score;				/*!< variable used to store the score of the player. */
    int end;				/*!< if set to 1 the game ends. */
    int nbBall;				/*!< number of ball (or lives) remaining before the game if over. */
	int bricksRemaining;	/*!< number of bricks left on screen. */
	int fromX;				/*!< horizontal position from where the bricks are placed. */
	int toX;				/*!< horizontal position until which brick are placed. */
	int fromY;				/*!< vertical position from where the bricks are placed. */
	int turbo;				/*!< acts on game speed (the bigger it is the faster the game runs. */
	int cheat;				/*!< activates the cheat (a huge racket taking all the screen avoiding death. */
}game_t;

/**
 * \struct all_t
 * \brief Structure containing different structures send to the different threads to run the game properly.
 */
typedef struct all_t
{
	struct ball_t *all_ball_e;		/*!< structure containing a pointer to a ball_t structure. */
	struct racket_t *all_racket_e;	/*!< structure containing a pointer to a racket_t structure. */
	struct game_t *all_game_e;		/*!< structure containing a pointer to a game_t structure. */
}all_t;

/**
 * \fn void placeElementFrom(int x, int y, int w, int h, char e)
 * \brief place every element possible in the game to set the base screen (or debug purposes).
 *
 * \param x base horizontal position.
 * \param y base vertical position.
 * \param w end horizontal position.
 * \param h end vertical position.
 */
void placeElementFrom(int x, int y, int w, int h, char e);

/**
 * \fn void placeHugeRacket()
 * \brief place a huge racket avoiding death.
 *
 */
void placeHugeRacket();

/**
 * \fn void removeHugeRacket()
 * \brief remove the huge racket avoiding death.
 *
 */
void removeHugeRacket();

/**
 * \fn void placeBrick(int x, int y, game_t *game_e)
 * \brief place one brick beginning at the x and y coordonates indicated.
 *
 * \param x base horizontal position of the brick.
 * \param y base vertical position of the brick.
 * \param game_e structure containing all game info.
 */
void placeBrick(int x, int y, game_t *game_e);

/**
 * \fn void destroyBrick(int x, int y, game_t *game_e)
 * \brief destroy the brick begining at the x and y coordonates indicated.
 *
 * \param x base horizontal position of the brick.
 * \param y base vertical position of the brick.
 * \param game_e structure containing all game info.
 */
void destroyBrick(int x, int y, game_t *game_e);

/**
 * \fn void placeBricks(int x, int y, int mx, int my, game_t *game_e)
 * \brief place brick beginning at the x and y coordonates indicated to the mx and my coordonates indicated.
 *
 * \param x base horizontal position.
 * \param y base vertical position.
 * \param mx end horizontal position.
 * \param my end vertical position.
 * \param game_e structure containing all game info.
 */
void placeBricks(int x, int y, int mx, int my, game_t *game_e);

/**
 * \fn void moveRacket(racket_t *racket_e, char c)
 * \brief move the racket accordingly to the input of the user q for left d for right.
 *
 * \param ball_e structure containing all ball info.
 * \param c input received from the keyboard.
 */
void moveRacket(racket_t *racket_e, char c);

/**
 * \fn void collision(ball_t *ball_e, game_t *game_e)
 * \brief deals with all the collisions in the game.
 *
 * \param game_e structure containing all game info.
 * \param ball_e structure containing all ball info.
 */
void collision(ball_t *ball_e, game_t *game_e);

/**
 * \fn void moveBALL(ball_t *ball_e, game_t *game_e)
 * \brief move the ball after the collision treatment.
 *
 * \param game_e structure containing all game info.
 * \param ball_e structure containing all ball info.
 */
void moveBALL(ball_t *ball_e, game_t *game_e);

/**
 * \fn void drawScreen(game_t *game_e)
 * \brief draws the caracter on the user screen accordingly to the array "screen" content.
 *
 * \param game_e structure containing all game info.
 */
void drawScreen(game_t *game_e);

/**
 * \fn void *fDisplay(void *arg)
 * \brief function used in a thread to deal with the display the game on the user's screen.
 *
 */
void *fDisplay(void *arg);

/**
 * \fn void *fDisplay(void *arg)
 * I couldn't find the original site for kbhit and getch definition but they can be found on :
 * https://forum.df2.ru/lofiversion/index.php/t16583-50.html
 */
int kbhit();

/**
 * \fn int getch()
 * I couldn't find the original site for kbhit and getch definition but they can be found on :
 * https://forum.df2.ru/lofiversion/index.php/t16583-50.html
 */
int getch();

/**
 * \fn void *fInput(void *arg)
 * \brief function used in a thread to deal with the input of the game.
 *
 */
void *fInput(void *arg);

/**
 * \fn int startGameUI()
 * \ Brief Start the game's UI
 *
 * Start the game UI to let the player play the game after that the server ask it.
 * Then return the player's score.
 *
 * \return The player's score at the end of the game.
 *
 */
int startGameUI();

#endif
