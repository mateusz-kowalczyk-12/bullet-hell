#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}


#define TRUE 1
#define FALSE 0
#define TEST_INITIAL_VALUE -1

//game states:
#define MENU 0
#define PLAYING 1
#define GAMEOVER 2

#define LEVELS_NUMBER 3
//those levels are:
#define LEVEL1 0
#define LEVEL2 1
#define LEVEL3 2

#define GAMEOVER_VIEWS_NUMBER 2
//those views are:
#define GAMEOVER_RETRY 0
#define GAMEOVER_MENU 1

#define TIME_SCORE_IMPORTANCE 1/30
#define BONUS_DELAY 0.5 //sec
#define BONUS_LIFE 10

#define MENU_OPTIONS_NUMBER 4
//those are:
#define MENU_LEVEL1 0
#define MENU_LEVEL2 1
#define MENU_LEVEL3 2
#define MENU_QUIT 3

#define GRADES_NUMBER 7
#define GRADE_CHANGE_LIMIT 30
#define GRADE_X 40 //px
#define GRADE_Y 40 //px
#define GRADE_ANIMATION_TEXTURES_NUMBER 2
//those are:
#define BIG_STAR 0
#define SMALL_STAR 1
#define GRADE_ANIMATION_FRAMES_NUMBER 3

#define DIRECTORY_STRING_LENGTH 50
#define NUMBER_STRING_LENGTH 3
#define SCORE_STRING_LENGTH 20

#define DIRECTIONS_NUMBER 4
//those directions are:
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define SCREEN_WIDTH 640 //px
#define SCREEN_HEIGHT 480 //px

#define STRING_PRINTING_MULTIPLIER 4
#define STRING_LINE_HEIGHT 8 //px
#define INTERLINE 4 //px

#define LIFE_BAR_RELATIVE_X 13 //px
#define LIFE_BAR_RELATIVE_Y 32 //px
#define PLAYER_LIFE_BAR_WIDTH_MULTIPLIER 0.25
#define PLAYER_LIFE_BAR_WIDTH_MULTIPLIER 0.25
#define LIFE_BAR_HEIGHT 5 //px

#define NEXT_LEVEL_LIMIT 100 //pts
#define PROGRESS_BAR_WIDTH 100 //px
#define PROGRESS_BAR_HEIGHT 10 //px
#define MAX_HITS_IN_A_ROW_INTERVAL 0.16 //sec
#define BASIC_LIFE_TAKEN_BY_ENEMY 1 //hp

#define LEVEL_WIDTH 4480 //px
#define LEVEL_HEIGHT 3360 //px

#define PLAYER_WIDTH 30 //px
#define PLAYER_HEIGHT 40 //px

#define BULLET_SIZE 6 //px
#define BULLET_JUMP 1 //px

#define BONUS_WIDTH 20 //px
#define BONUS_HEIGHT 24 //px

#define PLAYER_TEXTURES_NUMBER 8
//those are:
#define UP1 0
#define UP2 1
#define RIGHT1 2
#define RIGHT2 3
#define DOWN1 4
#define DOWN2 5
#define LEFT1 6
#define LEFT2 7

#define PURPLEGY_DYNAMIC_SPRITES_NUMBER 2
//those are:
#define UPDOWN 0
#define RIGHTLEFT 1

#define PLAYER_INITIAL_LIFE 2 //hp
#define PLAYER_INVINCIBILITY_TIME 1.5 //sec
#define PLAYER_STEP 8 //px
#define PLAYER_SHOOTING_INTERVAL 0.125 //sec
#define PLAYER_BULLETS_JUMP_INTERVAL 0.025 //sec
#define PLAYER_BULLET_JUMP 4 //px

#define ENEMIES_TYPES_NUMBER 3
//those types are:
#define GREENGY 0
//with:
#define GREENGY_INITIAL_NUMBER 200
#define GREENGY_INITIAL_LIFE 5 //hp
#define GREENGY_SHOOTING_INTERVAL 0.25 //sec
#define GREENGY_BULLETS_JUMP_INTERVAL 0.025 //sec
//also:
#define BLUEGY 1
//with:
#define BLUEGY_INITIAL_NUMBER 200
#define BLUEGY_INITIAL_LIFE 10 //hp
#define BLUEGY_SHOOTING_INTERVAL 0.45 //sec
#define BLUEGY_BULLETS_JUMP_INTERVAL 0.005 //sec
#define BLUEGY_ONE_DIRECTION_TIME_ADDITION 0.125 //sec
#define BLUEGY_INITIAL_ONE_DIRECTION_TIME 0.25 //sec
//also:
#define PURPLEGY 2
//with:
#define PURPLEGY_INITIAL_NUMBER 200
#define PURPLEGY_INITIAL_LIFE 15 //hp
#define PURPLEGY_SHOOTING_INTERVAL 0.75 //sec
#define PURPLEGY_BULLET_JUMP 0.025 //sec
#define PURPLEGY_BULLETS_JUMP_INTERVAL 0.025 //sec
#define PURPLEGY_BULLET_RADIUS_STEP 0.005 //px
#define PURPLEGY_MOVE_INTERVAL 0.05 //sec
#define PURPLEGY_JUMP 1 //px

#define ONE_FRAME_TIME 0.25 //sec
#define ENEMY_DESTRUCTION_SPRITES_NUMBER 2


typedef struct QuitingData {
	char quit = FALSE;
	char restart = FALSE;

	char skipMenu = FALSE;
	int skipToLevel = TEST_INITIAL_VALUE;
} QuitingData;

typedef struct SDL_setData {
	int rc;
	SDL_Event event;
	
	SDL_Surface* screen = NULL;
	SDL_Surface* charset = NULL;
	SDL_Surface* charsetDark = NULL;
	
	SDL_Surface* backgrounds[LEVELS_NUMBER] = { NULL };
	SDL_Surface* infoBackground = NULL;
	SDL_Surface* menuBackgrounds[MENU_OPTIONS_NUMBER] = { NULL };
	SDL_Surface* gameoverViews[GAMEOVER_VIEWS_NUMBER] = { NULL };
	
	SDL_Surface* grades[GRADES_NUMBER] = { NULL };
	SDL_Surface* gradeBackground = NULL;
	SDL_Surface* gradeAnimationTexture[GRADE_ANIMATION_TEXTURES_NUMBER] = { NULL };
	
	SDL_Texture* scrtex = NULL;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
} SDL_setData;

typedef struct TimeData {
	int lastTime = 0;
	double worldTime = 0;
} TimeData;

typedef struct ColoursData {
	int black, green, red, blue;
} ColoursData;

typedef struct BulletData {
	float x, y;
	float absX, absY;

	float distanceTraveled;
	float radius;

	float trigonometricalMultiplier;

	char active;

	float timeSinceLastMove;
	float timeSinceLastTurn;
	float oneDirectionTime;
	char shotDirection;
} BulletData;

typedef struct PlayerData {
	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT / 2;
	
	SDL_Surface* playerSprite[PLAYER_TEXTURES_NUMBER], * playerSpriteInv[PLAYER_TEXTURES_NUMBER];
	SDL_Surface* playerBulletSprite;

	BulletData* bullets = NULL;
	int bulletsNumber = 0;
	float timeSinceLastShot = 0;

	char animating = FALSE;
	char direction = LEFT;
	char frame = 1;
	float timeToNextFrame = ONE_FRAME_TIME;

	char invincible = FALSE;
	double invincibilityTimeLeft = 0;

	int life = PLAYER_INITIAL_LIFE;
	
	int level = 0;
	char newLevel = TRUE;

	char isLevelCreated[LEVELS_NUMBER] = { FALSE, FALSE, FALSE };

	char gameState = MENU;
	int menuOption = MENU_LEVEL1;
	int gameOverView = GAMEOVER_RETRY;

	int killingPoints = 0;
	int score = 0;

	int grade = 0;
	float gradeChangeTime = 0;
} PlayerData;

typedef struct EnemyData {
	int x, y;
	int initX, initY;
	int absX, absY;

	BulletData* bullets;
	int bulletsNumber;
	float timeSinceLastShot;

	char moveDirection;
	float timeSinceLastMove;

	char shootingYet;
	char active;

	float timeSinceLastHit;
	int pointsGiven;

	float animatingTime;

	int life;
} EnemyData;

typedef struct BonusData {
	int x, y;

	char active;
} BonusData;

typedef struct BonusesListData {
	BonusData* bonuses = NULL;
	int bonusesNumber = 0;

	float timeSinceLastBonus = 0;
	
	SDL_Surface* bonusSprite;
};

typedef struct EnemiesListData {
	EnemyData* enemies[ENEMIES_TYPES_NUMBER];
	int enemiesNumber[ENEMIES_TYPES_NUMBER] = {
		GREENGY_INITIAL_NUMBER, BLUEGY_INITIAL_NUMBER, PURPLEGY_INITIAL_NUMBER
	};

	SDL_Surface* enemiesSprites[ENEMIES_TYPES_NUMBER];
	SDL_Surface* enemiesDestructionSprites[ENEMIES_TYPES_NUMBER * ENEMY_DESTRUCTION_SPRITES_NUMBER];
	SDL_Surface* purplegyDynamicSprites[PURPLEGY_DYNAMIC_SPRITES_NUMBER];

	SDL_Surface* bulletsSprites[ENEMIES_TYPES_NUMBER];

	int initialLifes[ENEMIES_TYPES_NUMBER] = {
		GREENGY_INITIAL_LIFE, BLUEGY_INITIAL_LIFE, PURPLEGY_INITIAL_LIFE
	};

	float shootingIntervals[ENEMIES_TYPES_NUMBER] = {
		GREENGY_SHOOTING_INTERVAL, BLUEGY_SHOOTING_INTERVAL, PURPLEGY_SHOOTING_INTERVAL
	};
	float bulletsJumpIntervals[ENEMIES_TYPES_NUMBER] = {
		GREENGY_BULLETS_JUMP_INTERVAL, BLUEGY_BULLETS_JUMP_INTERVAL, PURPLEGY_BULLETS_JUMP_INTERVAL
	};
} EnemiesListData;