// kasbrik.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <termios.h>

#include <string.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include "game_ui.h"
//#include <termios.h>

#define GAME_SPEED 32000
#define WIDTH_MAX 80	// Screen width
#define HEIGHT_MAX 20	// Screen height
/*
#define WIDTH_MAX 80	// Screen width


#define HEIGHT_MAX 20	// Screen height
*/
#define SE 0			// South East direction
#define NE 1			// North East direction
#define NW 2			// North West direction
#define SW 3			// South West direction

#define BRICK_WIDTH 2	// Brick width
#define BRICK 'B'		// caracter used for bricks
#define BALL 'o'		// caracter used for the ball


#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
pthread_mutex_t screenAccess = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consoleAccess = PTHREAD_MUTEX_INITIALIZER;
char screen[WIDTH_MAX][HEIGHT_MAX];

sem_t termAccess ;

typedef struct ball_t
{
	int posX;
	int posY;
    int direction;
    int moving;
    int vert;
    int hor;
}ball_t;

typedef struct brick_t
{
	int posX;
	int posY;
	int width;
}brick_t;

brick_t *bricks;

typedef struct racket_t
{
	int posX;
	int posY;
}racket_t;

typedef struct game_t
{
	int score;
    int end;
    int nbBall;
	int bricksRemaining;
	int bricksBeginning;
	int bricksWidth;
	int fromX;
	int bricksHeight;
	int fromY;
	int turbo;
}game_t;

typedef struct all_t
{
	struct ball_t *all_ball_e;
	struct racket_t *all_racket_e;
	struct game_t *all_game_e;
}all_t;


int placeVoidFrom(int x, int y, int w, int h)
{

	return 0;
}

int placeElementFrom(int x, int y, int w, int h, char e)
{
	for(int py=y;py<h;py++)
	{
		for(int px=x;px<w;px++)
		{
			switch (e)
			{
				case 'V':
					screen[px][py]='V';
					break;

				case BRICK:
					screen[px][py]=BRICK;
					break;

				case 'R':
					screen[px][py]='=';
					break;
				case BALL:
					screen[px][py]=BALL;
					break;
				case 'W':
					screen[px][py]='W';
					break;
				case 'E':
					screen[px][py]='E';
					break;
				default :
					screen[px][py]='W';
			}
		}
	}
	return 0;
}

int placeRacket(int x, int y)
{
	for(int wx=2; wx<WIDTH_MAX-2; wx++)
	{
		screen[wx][y]='R';
	}
	return 0;
}

int placeBrick(int x, int y, game_t *game_e)
{
	int i=game_e->bricksRemaining;
	game_e->bricksRemaining++;
	game_e->bricksBeginning++;
	bricks[i].posX=x;
	bricks[i].posY=y;
	for(int wx=x; wx<x+BRICK_WIDTH; wx++)
	{
		screen[wx][y]=BRICK;
	}
	return 0;
}

int destroyBrick(int x, int y, game_t *game_e)
{
	int i=((x-2)/2)+((WIDTH_MAX-4)/2)*(y-5);
	x=bricks[i].posX;
	y=bricks[i].posY;
	for(int wx=x; wx<x+BRICK_WIDTH; wx++)
	{
		screen[wx][y]='V';
	}
	game_e->bricksRemaining--;
	game_e->score++;
	return 0;
}

int placeBricks(int x, int y, int mx, int my, game_t *game_e)
{
	game_e->bricksWidth=mx-x;
	game_e->fromX=x;
	game_e->bricksHeight=my-y;
	game_e->fromY=y;
	bricks=malloc( ((mx-x)*(my-y)) * sizeof(brick_t));
	for(int py=y;py<my;py++)
	{
		for(int px=x;px<mx;px+=BRICK_WIDTH)
		{
			placeBrick(px, py, game_e);
		}
	}
	return 0;
}


int moveRacket(struct racket_t *racket_e, char c)
{

	//fflush(stdin);
	for(int wx=racket_e->posX; wx<racket_e->posX+5; wx++)
	{
		screen[wx][racket_e->posY]='V';
	}
	switch (c)
	{
		case 'q':
			if(racket_e->posX>2)
			{
				racket_e->posX-=1;
			}
			break;
		case 'd':
			if(racket_e->posX<WIDTH_MAX-7)
			{
				racket_e->posX+=1;
			}
			break;
		//default :
	}
	for(int wx=racket_e->posX; wx<racket_e->posX+5; wx++)
	{
		screen[wx][racket_e->posY]='R';
	}
	/*for(int wx=2; wx<WIDTH_MAX-2; wx++)
	{
		screen[wx][y]='R';
	}*/
	return 0;
}


int placeBALL(struct ball_t *ball_e)
{
	screen[ball_e->posX][ball_e->posY]=BALL;
	return 0;
}

int collision(struct ball_t *ball_e, struct game_t *game_e)
{
	if(screen[(ball_e->posX)][(ball_e->posY)+1]=='E')
	{
		game_e->nbBall-=1;
		if(game_e->nbBall==0)
		{
			game_e->end=1;
		}
		return 0;
	}

	//if(screen[ball_e->posX+ball_e->hor][(ball_e->posY)+ball_e->vert]!='V'&&screen[ball_e->posX][(ball_e->posY)+ball_e->vert]!='V'&&screen[ball_e->posX+ball_e->hor][(ball_e->posY)!='V'])
	{
		switch(screen[ball_e->posX][(ball_e->posY)+(ball_e->vert)])
		{
			case 'W':
				switch(screen[(ball_e->posX)+ball_e->hor][(ball_e->posY)])
				{
					case 'V':
						ball_e->vert*=-1;
						break;
					case 'W':
						ball_e->vert*=-1;
						ball_e->hor*=-1;
						break;
					default :
						ball_e->direction=5;
						ball_e->vert=1;
						ball_e->hor=0;
				}
				break;
			case BRICK :
				switch(screen[(((ball_e->posX)/2)*2)+ball_e->hor*2][(ball_e->posY)])
				{
					case 'V':
						destroyBrick(ball_e->posX, (ball_e->posY)+ball_e->vert, game_e);
						//destroyBrick(ball_e->posX+ball_e->hor, (ball_e->posY)+ball_e->vert);
						ball_e->vert*=-1;
						break;
					case 'W':
						destroyBrick(ball_e->posX, (ball_e->posY)+ball_e->vert, game_e);
						ball_e->vert*=-1;
						ball_e->hor*=-1;
						break;
					case BRICK :
						destroyBrick(ball_e->posX, (ball_e->posY)+ball_e->vert, game_e);
						//destroyBrick(ball_e->posX+ball_e->hor, (ball_e->posY), game_e);
						ball_e->vert*=-1;
						ball_e->hor*=-1;
						break;
					default :
						ball_e->direction=5;
						ball_e->vert=1;
						ball_e->hor=0;
				}
				break;
			case 'R':
				ball_e->vert*=-1;
				if(screen[ball_e->posX+ball_e->hor][(ball_e->posY)+ball_e->vert]=='W')
						{
							//destroyBrick(ball_e->posX+ball_e->hor, (ball_e->posY)+ball_e->vert);
							ball_e->hor*=-1;
						}
				break;
			case 'V':
				switch(screen[ball_e->posX+ball_e->hor][(ball_e->posY)])
				{
					case 'W':
						ball_e->hor*=-1;
						break;
					case 'V':
						if(screen[ball_e->posX+ball_e->hor][(ball_e->posY)+ball_e->vert]==BRICK)
						{
							destroyBrick(ball_e->posX+ball_e->hor, (ball_e->posY)+ball_e->vert, game_e);
							ball_e->hor*=-1;
						}
						if(screen[ball_e->posX+ball_e->hor][(ball_e->posY)+ball_e->vert]=='W')
						{
							//destroyBrick(ball_e->posX+ball_e->hor, (ball_e->posY)+ball_e->vert);
							ball_e->hor*=-1;
						}
						break;
					case BRICK :
						destroyBrick(ball_e->posX+ball_e->hor, (ball_e->posY), game_e);
						//ball_e->vert*=-1;
						ball_e->hor*=-1;
						break;
					default :
						ball_e->direction=5;
						ball_e->vert=1;
						ball_e->hor=0;
				}
				break;
		}
	}


/*
	switch (ball_e->direction)
	{
		case SE:
			switch (screen[(ball_e->posX)+1][(ball_e->posY)+1])
			{
				case 'W':
					if(screen[(ball_e->posX+1)][(ball_e->posY)+1]=='W'&&screen[(ball_e->posX)][(ball_e->posY+1)]=='R')
					{
						ball_e->direction=2;
					}
					else
					{
						ball_e->direction=3;
					}
					break;

				case 'R':
					ball_e->direction++;
					break;
				case 'B':
					ball_e->direction++;
					destroyBrick(ball_e->posX+1,ball_e->posY+1);
					//screen[(((ball_e->posX+1)/2)*2)][(ball_e->posY)+1]='V';
					//screen[(((ball_e->posX+1)/2)*2)+1][(ball_e->posY)+1]='V';
					game_e->score+=2;


					//((((ball_e->posX+1)%2)*2)+2)
					//screen[(ball_e->posX)+1][(ball_e->posY)+1]='V';
					//screen[(ball_e->posX)+2][(ball_e->posY)+1]='V';
					//screen[(ball_e->posX)+1][(ball_e->posY)+1]='V';
					//screen[((((ball_e->posX-1)/2)*2)+1)][(ball_e->posY)+1]='V';
					break;
				//default:
					//printf("Erreur direction : %d \n", ball_e->direction);
			}
			break;

		case NE:
			switch (screen[(ball_e->posX)+1][(ball_e->posY)-1])
			{
				case 'W':
					if(screen[(ball_e->posX)][(ball_e->posY)-1]=='W'&&screen[(ball_e->posX+1)][(ball_e->posY)]=='W')
					{
						ball_e->direction=3;
					}
					else
					{
						if(screen[(ball_e->posX)][(ball_e->posY)-1]=='W')
					{
						ball_e->direction--;
					}
						else
						{
							ball_e->direction++;
						}
					}
					break;
				case 'B':
					ball_e->direction--;
					destroyBrick(ball_e->posX+1,ball_e->posY-1);
					//screen[(((ball_e->posX+1)/2)*2)][(ball_e->posY)-1]='V';
					//screen[(((ball_e->posX+1)/2)*2)+1][(ball_e->posY)-1]='V';
					game_e->score+=2;
					//screen[(ball_e->posX)+1][(ball_e->posY)-1]='V';
					//screen[(ball_e->posX)+2][(ball_e->posY)-1]='V';
					//screen[((((ball_e->posX+1)/2)*2)+1)][(ball_e->posY)-1]='V';
					break;
				//default:
					//printf("Erreur direction : %d \n", ball_e->direction);
			}
			break;

		case NW:
			switch (screen[(ball_e->posX)-1][(ball_e->posY)-1])
			{
				case 'W':
					if(screen[(ball_e->posX)][(ball_e->posY)-1]=='W'&&screen[(ball_e->posX-1)][(ball_e->posY)]=='W')
					{
						ball_e->direction=0;
					}
					else
					{
						if(screen[(ball_e->posX)][(ball_e->posY)-1]=='W')
						{
							ball_e->direction++;
						}
						else
						{
							ball_e->direction--;
						}
					}
					break;
				case 'B':
					ball_e->direction++;
					destroyBrick(ball_e->posX-1,ball_e->posY-1);
					//screen[(((ball_e->posX-1)/2)*2)][(ball_e->posY)-1]='V';
					//screen[(((ball_e->posX-1)/2)*2)+1][(ball_e->posY)-1]='V';
					game_e->score+=2;
					//screen[(ball_e->posX)-1][(ball_e->posY)-1]='V';
					//screen[(ball_e->posX)][(ball_e->posY)-1]='V';
					//screen[((((ball_e->posX-1)/2)*2)+1)][(ball_e->posY)-1]='V';
					break;
				//default:
					//printf("Erreur direction : %d \n", ball_e->direction);
			}
			break;
		case SW:
			switch (screen[(ball_e->posX)-1][(ball_e->posY)+1])
			{
				case 'W':
					if(screen[(ball_e->posX-1)][(ball_e->posY)+1]=='W'&&screen[(ball_e->posX)][(ball_e->posY+1)]=='R')
					{
						ball_e->direction=1;
					}
					else
					{
						ball_e->direction=0;
					}
					break;

				case 'R':
					ball_e->direction--;
					break;
				case 'B':
					ball_e->direction--;
					destroyBrick(ball_e->posX-1,ball_e->posY+1);
					//screen[(((ball_e->posX-1)/2)*2)][(ball_e->posY)+1]='V';
					//screen[(((ball_e->posX-1)/2)*2)+1][(ball_e->posY)+1]='V';
					game_e->score+=2;
					//screen[((((ball_e->posX-1)/2)*2))][(ball_e->posY)+1]='V';
					//screen[((((ball_e->posX-1)/2)*2)+1)][(ball_e->posY)+1]='V';
					break;
				//default:
					//printf("Erreur direction : %d \n", ball_e->direction);
			}
			break;
		//default:
			//printf("ERREUR COLLISION MAJEURE");
	}*/
	return 0;
}

int moveBALL(struct ball_t *ball_e, struct game_t *game_e)
{
	if(ball_e->moving==1)
	{
		screen[ball_e->posX][ball_e->posY]='V';
		int res=collision(ball_e, game_e);


		ball_e->posY=ball_e->posY+ball_e->vert;
		ball_e->posX=ball_e->posX+ball_e->hor;
		/*if(ball_e->vert==1)
		{

			ball_e->posY=ball_e->posY+1;
			if(ball_e->hor==1)
			{
				ball_e->posX=ball_e->posX+1;
			}
			if(ball_e->hor==-1)
			{
				ball_e->posX=ball_e->posX-1;
			}
		}
		if(ball_e->vert==-1)
		{

			ball_e->posY=ball_e->posY-1;
			if(ball_e->hor==1)
			{
				ball_e->posX=ball_e->posX+1;
			}
			if(ball_e->hor==-1)
			{
				ball_e->posX=ball_e->posX-1;
			}
		}
*/

/*
		switch (ball_e->direction)
		{
			case SE:
				ball_e->posX=ball_e->posX+1;
				ball_e->posY=ball_e->posY+1;
				ball_e->vert=1;
				ball_e->hor=1;
				break;

			case NE:
				ball_e->posX=ball_e->posX+1;
				ball_e->posY=ball_e->posY-1;
				ball_e->vert=1;
				ball_e->hor=-1;
				break;

			case NW:
				ball_e->posX=ball_e->posX-1;
				ball_e->posY=ball_e->posY-1;
				ball_e->vert=-1;
				ball_e->hor=-1;
				break;
			case SW:
				ball_e->posX=ball_e->posX-1;
				ball_e->posY=ball_e->posY+1;
				ball_e->vert=-1;
				ball_e->hor=1;
				break;
			default:
				ball_e->posX=ball_e->posX-0;
				ball_e->posY=ball_e->posY+5;
				ball_e->vert=1;
				ball_e->hor=0;
		}*/
	}
	screen[ball_e->posX][ball_e->posY]=BALL;
	return 0;
}


int drawScreen(struct game_t *game_e)
{
	system("clear");
	//game_e->bricksRemaining=0;
	for(int y=0;y<HEIGHT_MAX;y++)
	{
		for(int x=0;x<WIDTH_MAX;x++)
		{
			switch (screen[x][y])
			{
				case BRICK:
					printf("B");
					//game_e->bricksRemaining+=1;
					break;
				case 'V':
					printf(" ");
					break;

				case 'R':
					printf("~");
					break;
				case 'o':
					printf("o");
					break;
				case 'E':
					printf(" ");
					break;
				default :
					printf("W");
			}
		}
		//printf("\n");
	}
	printf("SCORE : %d BRIQUES RESTANTES : %d \n", game_e->score, game_e->bricksRemaining);
	printf("beginning : %d \n",game_e->bricksBeginning);
	fflush(stdout);
	if(game_e->bricksRemaining==0)
	{
		game_e->end=1;
	}
	//return game_e->bricksRemaining;
	return 0;
}
void *fDisplay(void *arg)
{
	int rB=1;
	all_t *all_e = (all_t *)arg;
	while(all_e->all_game_e->end==0)
	{
		//usleep(500000);
		usleep(64000);
		rB=drawScreen(all_e->all_game_e);
		CHECK(rB,"Erreur lors de l'affichage a l'ecran");
		printf("affichage : %d    ", screen[all_e->all_ball_e->posX][((all_e->all_ball_e->posY)+1)]);
		printf("vert : %d   ", all_e->all_ball_e->vert);
		printf("hor : %d   ", all_e->all_ball_e->hor);
		printf("posX : %d   ", all_e->all_ball_e->posX);
		printf("posY : %d   \n", all_e->all_ball_e->posY);
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
	int c;
	//ball_t *ball_e = (ball_t *)arg;
	all_t *all_e = (all_t *)arg;
	//(all_e->all_game_e->end==0&&c!='c')
	while(all_e->all_game_e->end==0)
	{
		//usleep(16000);

		//pthread_mutex_lock(&consoleAccess);
		//sem_wait(&termAccess);
		//fflush(stdin);
		if (kbhit())
		{
        /* do some work */
			c=getch();
			if(c=='c')
			{
				all_e->all_game_e->end=1;
				break;
			}
			if(c=='b')
			{
				all_e->all_game_e->nbBall-=1;
				all_e->all_ball_e->posX=(WIDTH_MAX/2)-1;
				all_e->all_ball_e->posY=HEIGHT_MAX-10;
			}
			if(c=='o')
			{
				if(all_e->all_game_e->turbo==1)
				{
					all_e->all_game_e->turbo=100;
				}
				else
				{
					all_e->all_game_e->turbo=1;
				}
			}
			if(c=='p')
			{
				if(all_e->all_ball_e->moving==0)
				{
					all_e->all_ball_e->moving=1;
				}
				else
				{
					all_e->all_ball_e->moving=0;
				}
			}
			else
			{
				//pthread_mutex_lock(&screenAccess);
				moveRacket(all_e->all_racket_e, c);
				//pthread_mutex_unlock(&screenAccess);
			}
		}
		//c = getchar();
		//fread(&c, sizeof(c),1,stdin);
		//fflush(stdin);
		//rB=drawScreen(all_e->all_game_e);


		//system ("/bin/stty cooked");
		//sem_post(&termAccess);
		//pthread_mutex_unlock(&consoleAccess);
		//fflush(stdin);

		// use system call to set terminal behaviour to more normal behaviour
	}

	pthread_exit(EXIT_SUCCESS);
}



int startGameUI()
{
	void * ret;
	pthread_t th;

	struct termios infos;

	if (tcgetattr(STDIN_FILENO, &infos) == -1) // on récupère les infos du terminal
	{
	  fprintf(stderr, "Erreur tcgetattr.\n");
	  return (EXIT_FAILURE);
	}
	infos.c_lflag &= ~ECHO; // on le passe en mode non-echo
	if (tcsetattr(STDIN_FILENO, TCSANOW, &infos) == -1) // on set les infos du terminal
	{
	  fprintf(stderr, "Erreur tcsetattr.\n");
	  return (EXIT_FAILURE);
	}






	sem_init(&termAccess,PTHREAD_PROCESS_SHARED,1);
	/*
	// INIT
	*/
	system("clear");
	int res;

	res=placeElementFrom(0,0, WIDTH_MAX,3, 'W');
	res=placeElementFrom(0,0, 5, HEIGHT_MAX, 'W');
	res=placeElementFrom(WIDTH_MAX-2,0, WIDTH_MAX, HEIGHT_MAX, 'W');

	CHECK(res,"Erreur lors du placement de l'element : mur");

	res=placeElementFrom(2,3, WIDTH_MAX-2, HEIGHT_MAX, 'V');
	CHECK(res,"Erreur lors du placement de l'element : void");
	struct racket_t racket_e = { (WIDTH_MAX/2)-2,HEIGHT_MAX-2 };
	res=moveRacket(&racket_e, '0');
	res=placeRacket((WIDTH_MAX/2)-2,HEIGHT_MAX-3);
	CHECK(res,"Erreur lors du placement de l'element : raquette");
	//res=placeElementFrom(2, 5, WIDTH_MAX-2, 9, 'B');
	//res=placeElementFrom(3, HEIGHT_MAX-4, 6, HEIGHT_MAX-3, 'B');

	game_t game_e = { 0 ,0,1,0};
	game_e.turbo=1;
	game_e.bricksRemaining=0;
	//game_e.bricksRemaining;
	placeBricks(2, 5, WIDTH_MAX-2, 9, &game_e);
	CHECK(res,"Erreur lors du placement de l'element : BRICK");
	res=placeElementFrom(2,HEIGHT_MAX-1, WIDTH_MAX-2, HEIGHT_MAX, 'E');
	struct ball_t ball_e3 = { WIDTH_MAX-6 ,14,2,1 };
	ball_t ball_e = { (WIDTH_MAX/2)-8 ,HEIGHT_MAX-11,0,1,1,1};
	struct ball_t ball_e2 = { (WIDTH_MAX/2)-3 ,HEIGHT_MAX-10,1,1 };
	struct ball_t ball_e4 = { (WIDTH_MAX/2)-6 ,HEIGHT_MAX-6,3,1 };
	//res=drawScreen(&game_e
	res=placeBALL(&ball_e);
	CHECK(res,"Erreur lors du placement de l'element : BALLe");


	all_t *all_e = malloc(sizeof(all_t));
	all_e->all_game_e= &game_e;
	all_e->all_ball_e= &ball_e;
	all_e->all_racket_e= &racket_e;

	pthread_create(&th, NULL, fInput, all_e);
	pthread_create(&th, NULL, fDisplay, all_e);
	int rB=1;
	// use system call to make terminal send all keystrokes directly to stdin
	system ("/bin/stty raw");

	/*
	// routine affichage
	*/
	while(rB!=0&& game_e.end==0)
	{
		//usleep(500000);
		usleep(GAME_SPEED/(game_e.turbo));
		//usleep(100);
		//pthread_mutex_lock(&screenAccess);
		moveBALL(&ball_e, &game_e);
		//moveBALL(&ball_e2, &game_e);
		//moveBALL(&ball_e3, &game_e);
		//moveBALL(&ball_e4, &game_e);
		//pthread_mutex_unlock(&screenAccess);
		//pthread_mutex_lock(&consoleAccess);
		//system ("/bin/stty cooked");
		//sem_wait(&termAccess);
		//rB=drawScreen(&game_e);
		CHECK(rB,"Erreur lors de l'affichage a l'ecran");
		//sem_post(&termAccess);
		//system ("/bin/stty raw");
		//pthread_mutex_unlock(&consoleAccess);
	}
	infos.c_lflag |= ECHO; // on le passe en mode non-echo
	if (tcsetattr(STDIN_FILENO, TCSANOW, &infos) == -1) // on set les infos du terminal
	{
	  fprintf(stderr, "Erreur tcsetattr.\n");
	  return (EXIT_FAILURE);
	}
	system ("/bin/stty cooked");
	game_e.end=1;
	pthread_join(th, &ret);
	sem_destroy(&termAccess);
	return game_e.score;
}
