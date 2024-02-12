#include "Definitions.h"


//initialising functions:

	//----
	void InitialiseSDL(SDL_setData* SDL_set, TimeData* time);

	//allocates initial memory, sets initial positions and lifes of the enemies
	void InitialiseEnemies(EnemiesListData* enemiesList, int level);

	//also sets the color key
	void LoadGraphics(SDL_setData* SDL_set, PlayerData* player, EnemiesListData* enemyList, BonusesListData* bonusesList);

	//sets RGB values for the colours from the Colours structure
	void SetColours(ColoursData* colours, SDL_setData* SDL_set);


//initialising subfunctions:

	//----
	void LoadEnemiesGraphics(SDL_setData* SDL_set, EnemiesListData* enemiesList);

	//----
	void LoadPlayerGraphics(SDL_setData* SDL_set, PlayerData* player);

	//----
	void LoadBackgroundGraphics(SDL_setData* SDL_set);

	//---
	void LoadGradesGraphics(SDL_setData* SDL_set);

//initializing mini-functions:

	//prints an error, clears the memory and ends the program:
	void LoadingError(SDL_setData* SDL_set, char* directory);


//game function - dispatches everything to proceed with the game:
	void Game(SDL_setData* SDL_set, PlayerData* player, EnemiesListData* enemiesList, BonusesListData* bonusesList, ColoursData* colours, TimeData* time, QuitingData* quiting);


//in-game functions:

	//displays the menu and handles the menu actions:
	void Menu(SDL_setData* SDL_set, PlayerData* player, QuitingData* quiting, TimeData* time);

	//displays the gameover menu and handles the gameover menu actions:
	void GameOver(SDL_setData* SDL_set, PlayerData* player, QuitingData* quiting);

	//renders a single screen
	void RenderFrame(SDL_setData* SDL_set, TimeData* time, ColoursData* colours, PlayerData* player, EnemiesListData enemiesList, BonusesListData bonusesList);

	//handles events (if there were any)
	void HandleEvents(SDL_setData* SDL_set, PlayerData* player, EnemiesListData* enemiesList, TimeData time, QuitingData* quiting, BonusesListData bonusesList);


//in-game subfunctions:

	//----
	void DrawEnemies(SDL_setData* SDL_set, EnemiesListData enemiesList, int level, ColoursData colours, TimeData time);

	//----
	void DrawPlayer(SDL_setData* SDL_set, PlayerData* player, TimeData* time, ColoursData colours);

	//----
	void DrawBonuses(SDL_setData* SDL_set, BonusesListData bonusesList);

	//----
	void DrawGrade(SDL_setData* SDL_set, PlayerData* player);

	//draws the bullets of the given enemy
	void DrawBullets(SDL_setData* SDL_set, EnemiesListData enemiesList, int enemyTypeNumber, int enemyNumber);

	//----
	void DrawPlayerBullets(SDL_setData* SDL_set, PlayerData player);

	//moves enemies, their bullets, player 's bullets and bonuses when the player moves
	void MoveEnemiesAndPlayerBullets(EnemiesListData* enemiesList, PlayerData* player, BonusesListData bonusesList, char dimension, char sign);

	//checks if a shot should be execute and applies shooting
	void Shoot(TimeData* time, EnemiesListData* enemiesList, PlayerData player);

	//checking if a shot can be executed by the player and applies shooting
	void ShootPlayer(TimeData time, PlayerData* player);

	//dispatches moving of specific bullets when the set time passes
	void MoveBullets(EnemiesListData* enemiesList, PlayerData player);

	//randomly moves Purplegies
	void MovePurplegies(EnemiesListData* enemyList);

	//moves a specific bullet
	void MoveBulletOfAnEnemy(int enemyTypeNumber, BulletData* bullet, char coordinate);

	//moves player bullets when a set time passes
	void MovePlayerBullets(PlayerData player);
	
	//dispatches checking collidions of the bullets and sprites
	void CheckCollision(EnemiesListData enemiesList, PlayerData* player, BonusesListData* bonusesList);

	//checks if the player was hit
	void CheckIfPlayerHit(EnemiesListData enemiesList, PlayerData* player, int enemyNumber);

	//checks if any enemy was hit
	void CheckIfEnemyHit(EnemiesListData enemiesList, PlayerData* player, int enemyNumber);

	//checks if the player touched any bonus
	void CheckIfBonusCollected(PlayerData* player, BonusesListData* bonusesList);
	
	//counts time
	void UpdateTime(TimeData* time, EnemiesListData* enemiesList, PlayerData* player, BonusesListData* bonusesList);
	
	//draws the info box with the game data and the instructions
	void DrawInfo(SDL_setData* SDL_set, TimeData* time, ColoursData* colours, PlayerData player);


//in-game minifunctions:

	//alocates new memory and sets the data for the new bullet
	void CreateNewBullet(EnemyData** enemies, int eneyTypeNumber, int enemyNumber, TimeData time);

	//frees the memory allocated for the inactive bullets
	void DestroyBullets(EnemiesListData enemies, PlayerData* player, int level);

	//frees the memory allocated for the inactive bullets of the player
	void DestroyPlayerBullets(PlayerData* player);

	//frees the memory allocated for the inactive bullets of the player
	void DestroyEnemies(EnemiesListData* enemiesList, int level);

	//frees the memory of all enemies, bullets and bonuses before loading a new level
	void DestroyAllObjects(EnemiesListData* enemiesList, PlayerData* player, BonusesListData* bonusesList);

	//specifies the direction for the bullet movements
	void SetMovementAddition(char direction, int* x_addition, int* y_addition, int jump);

	//creates a new bonus after a certain time and frees the memory of the inactive ones
	void UpdateBonuses(BonusesListData* bonusesList, PlayerData player);

	//updates player's score and checks if a new level is achieved
	void UpdateScore(PlayerData* player, TimeData time, QuitingData* quiting);


//drawing subfunctions:

	//draws a text txt on surface screen, starting from the point (x, y)
	//charset is a 128x128 bitmap containing character images
	void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);

	//draws a surface sprite on a surface screen in point (x, y)
	//(x, y) is the center of sprite on screen
	void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);

	//draws a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
	void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);

	//draws a rectangle of size l by k
	void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);


//drawing mini-functions:

	//draws a single pixel
	void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);


//end-of-the-game functions:

	//frees all the surfaces, textures, ...
	//charsetToo (logical value) tells if the charset should also be freed
	void FreeAll(SDL_setData* SDL_set);