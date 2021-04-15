/**
 * \file kassbriik.c
 * \brief code for the game itself.
 * \author Auteur Corentin Destrez, Valentin Guiberteau
 * \version 0.4
 * \date 16 april 2021
 *
 * This file groups all the functions applying to the game itself such as the input drawing functions.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <semaphore.h>
#include <sys/select.h>

/*****************************************************************************/
/*                              CUSTOM LIBRARIES                             */
/*****************************************************************************/
#include "game_ui.h"


char screen[WIDTH_MAX][HEIGHT_MAX];

brick_t *bricks;	/*!< array all the bricks on the screen. */

int startGameUI()
{
	pthread_t thInput;
	pthread_t thDisplay;

	struct termios infos;

 	// recover terminal info
	if (tcgetattr(STDIN_FILENO, &infos) == -1)
	{
	  fprintf(stderr, "Erreur tcgetattr.\n");
	  return (EXIT_FAILURE);
	}
	// switch to no echo mod (we don't wan't to see our key presses)
	infos.c_lflag &= ~ECHO;

	// set new setting to terminal
	if (tcsetattr(STDIN_FILENO, TCSANOW, &infos) == -1)
	{
	  fprintf(stderr, "Erreur tcsetattr.\n");
	  return (EXIT_FAILURE);
	}

	/**
	 * part used to initialise the game with its parameters.
	 */

	game_t game_e;
	game_e.score=0;
	game_e.end=0;
	game_e.nbBall=3;
	game_e.bricksRemaining=0;
	game_e.turbo=1;
	game_e.cheat=0;

	placeElementFrom(0, 0, WIDTH_MAX, HEIGHT_MAX, EMPTY);												/*!< fill the screen with void. */
	placeElementFrom(0, 0, WIDTH_MAX, UPPER_WALL_HEIGHT, WALL);											/*!< place the roof. */
	placeElementFrom(0, 0, SIDE_WALL_WIDTH, HEIGHT_MAX, WALL);											/*!< place the walls. */
	placeElementFrom(WIDTH_MAX-SIDE_WALL_WIDTH,0, WIDTH_MAX, HEIGHT_MAX, WALL);
	placeElementFrom(SIDE_WALL_WIDTH,HEIGHT_MAX-1, WIDTH_MAX-SIDE_WALL_WIDTH, HEIGHT_MAX, E_TRIGGER);	/*!< place the floor. */
	placeBricks(SIDE_WALL_WIDTH, 5, WIDTH_MAX-SIDE_WALL_WIDTH, 9, &game_e);								/*!< place the bricks. */

	/**
	 * place the racket and set its parameters.
	 */
	racket_t racket_e ;
	racket_e.posX=(WIDTH_MAX/2)-2;
	racket_e.posY=HEIGHT_MAX-2;
	moveRacket(&racket_e, '0');		/*!< used to display on the screen the racket. */

	/**
	 * initialisation of the struct ball.
	 */
	ball_t ball_e;
	ball_e.posX=(WIDTH_MAX/2)+1;
	ball_e.posY=HEIGHT_MAX-4;
	ball_e.moving=0;
	ball_e.dirX=1;
	ball_e.dirY=-1;

	/**
	 * initialisation of the struct containing all oher stucts.
	 */
	all_t *all_e = malloc(sizeof(all_t));
	all_e->all_game_e= &game_e;
	all_e->all_ball_e= &ball_e;
	all_e->all_racket_e= &racket_e;

	/**
	 * creation of the two threads dealing with the inputs and the display of the game.
	 */
	pthread_create(&thInput, NULL, fInput, all_e);
	pthread_create(&thDisplay, NULL, fDisplay, all_e);

	system ("/bin/stty raw");	/*!< use system call to make terminal send all keystrokes directly to stdin without waiting for return presses. */

	/**
	 * end of the part used to initialise the game with its parameters.
	 */

	/**
	 * ball position update routine
	 */
	while(game_e.end==0)
	{
		usleep(GAME_SPEED/(game_e.turbo));
		moveBALL(&ball_e, &game_e);
	}

	/**
	 * part to set back the default terminal configuration
	 */
	infos.c_lflag |= ECHO; 										/*!< set back echo mode. */
	if (tcsetattr(STDIN_FILENO, TCSANOW, &infos) == -1) 		/*!< set the changed setting to the terminal. */
	{
	  fprintf(stderr, "Erreur tcsetattr.\n");
	  return (EXIT_FAILURE);
	}
	system ("/bin/stty cooked");								/*!< set back of the tty to cooked (waiting for return to validate input). */

	/**
	 * closure of the threads and end of the program.
	 */
	pthread_join(thInput, NULL);
	pthread_join(thDisplay, NULL);
	return game_e.score;
}

void placeElementFrom(int x, int y, int w, int h, char e)
{
	int py;
	int px;
	for(py=y;py<h;py++)
	{
		for(px=x;px<w;px++)
		{
			switch (e)
			{
				case EMPTY:
					screen[px][py]=EMPTY;
					break;
				case BRICK:
					screen[px][py]=BRICK;
					break;
				case RACKET:
					screen[px][py]=RACKET;
					break;
				case BALL:
					screen[px][py]=BALL;
					break;
				case WALL:
					screen[px][py]=WALL;
					break;
				case E_TRIGGER:
					screen[px][py]=E_TRIGGER;
					break;
				default :
					screen[px][py]=ERROR_C;
			}
		}
	}
}

void placeHugeRacket()
{
	int wx;
	for(wx=SIDE_WALL_WIDTH; wx<WIDTH_MAX-SIDE_WALL_WIDTH; wx++)
	{
		screen[wx][HEIGHT_MAX-3]=RACKET;
	}
}

void removeHugeRacket()
{
	int wx;
	for(wx=SIDE_WALL_WIDTH; wx<WIDTH_MAX-SIDE_WALL_WIDTH; wx++)
	{
		screen[wx][HEIGHT_MAX-3]=EMPTY;
	}
}

void placeBrick(int x, int y, game_t *game_e)
{
	int wx;
	int i=game_e->bricksRemaining++;
	bricks[i].posX=x;
	bricks[i].posY=y;
	for(wx=x; wx<x+BRICK_WIDTH; wx++)
	{
		screen[wx][y]=BRICK;
	}
}

void destroyBrick(int x, int y, game_t *game_e)
{
	int wx;
	int i=((x-(SIDE_WALL_WIDTH))/2)+((game_e->toX-game_e->fromX)/2)*(y-(game_e->fromY));
	x=bricks[i].posX;
	y=bricks[i].posY;
	for(wx=x; wx<x+BRICK_WIDTH; wx++)
	{
		screen[wx][y]=EMPTY;
	}
	game_e->bricksRemaining--;
	game_e->score++;
}

void placeBricks(int x, int y, int mx, int my, game_t *game_e)
{
	int py;
	int px;
	game_e->fromX=x;
	game_e->toX=mx;
	game_e->fromY=y;
	bricks=malloc( ((mx-x)*(my-y)) * sizeof(brick_t));
	for(py=y;py<my;py++)
	{
		for(px=x;px<mx;px+=BRICK_WIDTH)
		{
			placeBrick(px, py, game_e);
		}
	}
}

void moveRacket(racket_t *racket_e, char c)
{
	int wx;
	// erase the actual racket
	for(wx=racket_e->posX; wx<racket_e->posX+RACKET_SIZE; wx++)
	{
		screen[wx][racket_e->posY]=EMPTY;
	}
	switch (c)
	{
		case 'q':
			if(racket_e->posX>SIDE_WALL_WIDTH)
			{
				racket_e->posX--;
			}
			break;
		case 'd':
			if(racket_e->posX<(WIDTH_MAX-(RACKET_SIZE+SIDE_WALL_WIDTH)))
			{
				racket_e->posX++;
			}
			break;
		default :
			// other input are dealt with in "fInput()" so nothing to treat here.
			break;
	}
	// place the new racket position on the screen
	for(wx=racket_e->posX; wx<racket_e->posX+RACKET_SIZE; wx++)
	{
		screen[wx][racket_e->posY]=RACKET;
	}
}

void collision(ball_t *ball_e, game_t *game_e)
{
	/**
	 * this section deals with the ball hiting th bottom of the screen
	 */
	if(screen[(ball_e->posX)][(ball_e->posY)+1]=='E')
	{
		game_e->nbBall--;
		if(game_e->nbBall==0)
		{
			game_e->end=1;
			return;
		}
		game_e->score/=2;
		ball_e->posX=(WIDTH_MAX/2)+1;
		ball_e->posY=HEIGHT_MAX-4;
		ball_e->dirY=-1;
		ball_e->dirX=1;
		ball_e->moving=0;
		return;
	}

	/**
	 * heart of the collision detection
	 */
	char diagonal_detection = screen[ball_e->posX+ball_e->dirX][ball_e->posY+ball_e->dirY];
	char vertical_detection = screen[ball_e->posX][ball_e->posY+ball_e->dirY];
	char horizontal_detection = screen[ball_e->posX+ball_e->dirX][ball_e->posY];

	if(diagonal_detection!=EMPTY || vertical_detection!=EMPTY || horizontal_detection!=EMPTY )
	{
		switch(vertical_detection)
		{
			case WALL:
				switch(horizontal_detection)
				{
					case EMPTY:
						ball_e->dirY*=-1;
						break;
					case WALL:
						ball_e->dirY*=-1;
						ball_e->dirX*=-1;
						break;
					default :
						ball_e->dirY=1;
						ball_e->dirX=0;
				}
				break;
			case BRICK :
				switch(screen[((ball_e->posX/2)*2)+ball_e->dirX*2][ball_e->posY])			/*!< potential detection on the pair component of the brick hit. */
				{
					case EMPTY:
						destroyBrick(ball_e->posX, ball_e->posY+ball_e->dirY, game_e);
						ball_e->dirY*=-1;
						break;
					case WALL:
						destroyBrick(ball_e->posX, ball_e->posY+ball_e->dirY, game_e);
						ball_e->dirY*=-1;
						ball_e->dirX*=-1;
						break;
					case BRICK :
						destroyBrick(ball_e->posX, ball_e->posY+ball_e->dirY, game_e);
						ball_e->dirY*=-1;
						ball_e->dirX*=-1;
						break;
					default :
						ball_e->dirY=1;
						ball_e->dirX=0;
				}
				break;
			case RACKET:
				ball_e->dirY*=-1;
				if(diagonal_detection==WALL)
						{
							ball_e->dirX*=-1;
						}
				break;
			case EMPTY:
				switch(horizontal_detection)
				{
					case WALL:
						ball_e->dirX*=-1;
						break;
					case EMPTY:
						if(diagonal_detection==BRICK)
						{
							destroyBrick(ball_e->posX+ball_e->dirX, ball_e->posY+ball_e->dirY, game_e);
							ball_e->dirX*=-1;
						}
						if(diagonal_detection==WALL)
						{
							ball_e->dirX*=-1;
						}
						break;
					case BRICK :
						destroyBrick(ball_e->posX+ball_e->dirX, ball_e->posY, game_e);
						ball_e->dirX*=-1;
						break;
					default :
						ball_e->dirY=1;
						ball_e->dirX=0;
				}
				break;
		}
	}
}

void moveBALL(ball_t *ball_e, game_t *game_e)
{
	if(ball_e->moving==1)
	{
		screen[ball_e->posX][ball_e->posY]=EMPTY;	/*!< empty the last position of the ball on the screen. */
		collision(ball_e, game_e);
		ball_e->posY=ball_e->posY+ball_e->dirY;
		ball_e->posX=ball_e->posX+ball_e->dirX;
	}
	screen[ball_e->posX][ball_e->posY]=BALL;		/*!< draws the ball on it's new position on the screen. */
}

void drawScreen(game_t *game_e)
{
	int y;
	int x;
	char xy;
	system("clear");
	for(y=0;y<HEIGHT_MAX;y++)
	{
		for(x=0;x<WIDTH_MAX;x++)
		{
			xy=screen[x][y];
			if(xy=='E')
			{
				printf(" ");
			}
			else
			{
				printf("%c", xy);
			}
		}
	}

	/**
	 * draws the bottom of the screen with different information for the user.
	 */
	int i;
	for(i=0; i<WIDTH_MAX; i++)
	{
		printf("=");
	}
	// draws the next line (the half line containing info)
	for(i=0; i<23; i++)
	{
		printf("=");
	}
	printf(" SCORE: %d | BRICKS: %d | BALLS: %d ", game_e->score, game_e->bricksRemaining, game_e->nbBall);
	for(i=0; i<21; i++)
	{
		printf("=");
	}
	fflush(stdout);
	if(game_e->bricksRemaining==0)
	{
		game_e->end=1;
	}
}

void *fDisplay(void *arg)
{
	all_t *all_e = (all_t *)arg;
	while(all_e->all_game_e->end==0)
	{
		usleep(64000);
		drawScreen(all_e->all_game_e);
	}
	pthread_exit(EXIT_SUCCESS);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}
int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

void *fInput(void *arg)
{
	char c;
	all_t *all_e = (all_t *)arg;
	while(all_e->all_game_e->end==0)
	{
		if (kbhit())
		{
			c=getch();
			switch(c)
			{
				case 'c':
					all_e->all_game_e->end=1;
					break;
				case 'b':
					all_e->all_game_e->nbBall-=1;
					break;
				case 'o':
					if(all_e->all_game_e->turbo==1)
					{
						all_e->all_game_e->turbo=100;
					}
					else
					{
						all_e->all_game_e->turbo=1;
					}
					break;
				case 'p':
					if(all_e->all_ball_e->moving==0)
					{
						all_e->all_ball_e->moving=1;
					}
					else
					{
						all_e->all_ball_e->moving=0;
					}
					break;
				case 'i':
					if(all_e->all_game_e->cheat==0)
					{
						all_e->all_game_e->cheat=1;
						placeHugeRacket();
					}
					else
					{
						all_e->all_game_e->cheat=0;
						removeHugeRacket();
					}
					break;
				default :
					moveRacket(all_e->all_racket_e, c);
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
