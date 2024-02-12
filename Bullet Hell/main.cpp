#include "Headers.h"


#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char** argv) {
	srand(time(NULL));

	QuitingData quiting;
	ColoursData colours;
	
	while (TRUE) {
		quiting.restart = FALSE;

		SDL_setData SDL_set;
		TimeData time;
		PlayerData player;

		if (quiting.skipMenu) {
			player.gameState = PLAYING;
			player.level = quiting.skipToLevel;
			quiting.skipMenu = FALSE;
		}

		EnemiesListData enemiesList;
		BonusesListData bonusesList;

		InitialiseSDL(&SDL_set, &time);
		LoadGraphics(&SDL_set, &player, &enemiesList, &bonusesList);
		SetColours(&colours, &SDL_set);

		while (!quiting.quit && !quiting.restart) {
			if (player.gameState == MENU)
				Menu(&SDL_set, &player, &quiting, &time);
			
			if(player.gameState == PLAYING)
				Game(&SDL_set, &player, &enemiesList, &bonusesList, &colours, &time, &quiting);
			
			if (player.gameState == GAMEOVER)
				GameOver(&SDL_set, &player, &quiting);
		}

		FreeAll(&SDL_set);
		DestroyAllObjects(&enemiesList, &player, &bonusesList);

		if (quiting.quit) {
			return 0;
		}
	}
}


void InitialiseSDL(SDL_setData* SDL_set, TimeData* time) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(1);
	}

	//fullscreen mode
	//SDL_set->rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &SDL_set->window, &SDL_set->renderer);
	SDL_set->rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &SDL_set->window, &SDL_set->renderer);
	if (SDL_set->rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(SDL_set->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(SDL_set->renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(SDL_set->window, "Mateusz Kowalczyk's Bullet Hell");

	SDL_set->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	SDL_set->scrtex = SDL_CreateTexture(SDL_set->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	//turning off the visibility of the mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	time->lastTime = SDL_GetTicks();
}

void InitialiseEnemies(EnemiesListData* enemiesList, int level) {
	enemiesList->enemies[level] = (EnemyData*)malloc(enemiesList->enemiesNumber[level] * sizeof(EnemyData));

	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[level]; enemyNumber++) {
		enemiesList->enemies[level][enemyNumber].x = SCREEN_WIDTH / 2 - LEVEL_WIDTH / 2 + PLAYER_WIDTH / 2 + (rand() % (LEVEL_WIDTH - PLAYER_WIDTH));
		enemiesList->enemies[level][enemyNumber].y = SCREEN_HEIGHT / 2 - LEVEL_HEIGHT / 2 + PLAYER_HEIGHT / 2 + (rand() % (LEVEL_HEIGHT - PLAYER_HEIGHT));

		enemiesList->enemies[level][enemyNumber].initX = enemiesList->enemies[level][enemyNumber].x;
		enemiesList->enemies[level][enemyNumber].initY = enemiesList->enemies[level][enemyNumber].y;

		enemiesList->enemies[level][enemyNumber].absX = enemiesList->enemies[level][enemyNumber].initX;
		enemiesList->enemies[level][enemyNumber].absY = enemiesList->enemies[level][enemyNumber].initY;
			
		enemiesList->enemies[level][enemyNumber].life = enemiesList->initialLifes[level];

		enemiesList->enemies[level][enemyNumber].bullets = NULL;
		enemiesList->enemies[level][enemyNumber].bulletsNumber = 0;
		enemiesList->enemies[level][enemyNumber].timeSinceLastShot = 0;

		enemiesList->enemies[level][enemyNumber].timeSinceLastHit = 0;
		enemiesList->enemies[level][enemyNumber].pointsGiven = level + 1;

		enemiesList->enemies[level][enemyNumber].moveDirection = rand() % DIRECTIONS_NUMBER;
		enemiesList->enemies[level][enemyNumber].timeSinceLastMove = 0;

		enemiesList->enemies[level][enemyNumber].animatingTime = 0;

		enemiesList->enemies[level][enemyNumber].shootingYet = FALSE;
		enemiesList->enemies[level][enemyNumber].active = TRUE;
	}
}

void LoadGraphics(SDL_setData* SDL_set, PlayerData* player, EnemiesListData* enemiesList, BonusesListData* bonusesList) {
	SDL_set->charset = SDL_LoadBMP("graphics/cs8x8.bmp");
	if (SDL_set->charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		FreeAll(SDL_set);
		exit(1);
	}
	SDL_SetColorKey(SDL_set->charset, true, 0x000000);

	SDL_set->charsetDark = SDL_LoadBMP("graphics/cs8x8_darkLetters.bmp");
	if (SDL_set->charsetDark == NULL) {
		printf("SDL_LoadBMP(cs8x8_darkLetters.bmp) error: %s\n", SDL_GetError());
		FreeAll(SDL_set);
		exit(1);
	}

	LoadBackgroundGraphics(SDL_set);

	SDL_set->infoBackground = SDL_LoadBMP("graphics/infoBackground.bmp");
	if (SDL_set->infoBackground == NULL)
		LoadingError(SDL_set, "graphics/infoBackground.bmp");

	bonusesList->bonusSprite = SDL_LoadBMP("graphics/bonus.bmp");
	if (bonusesList->bonusSprite == NULL)
		LoadingError(SDL_set, "graphics/bonus.bmp");

	LoadPlayerGraphics(SDL_set, player);
	LoadEnemiesGraphics(SDL_set, enemiesList);
	LoadGradesGraphics(SDL_set);
}

void SetColours(ColoursData* colours, SDL_setData* SDL_set) {
	colours->black = SDL_MapRGB(SDL_set->screen->format, 0x00, 0x00, 0x00);
	colours->green = SDL_MapRGB(SDL_set->screen->format, 0x00, 0xFF, 0x00);
	colours->red = SDL_MapRGB(SDL_set->screen->format, 0xFF, 0x00, 0x00);
	colours->blue = SDL_MapRGB(SDL_set->screen->format, 0x11, 0x11, 0xCC);
}


void LoadEnemiesGraphics(SDL_setData* SDL_set, EnemiesListData* enemiesList) {
	for (int enemyTypeNumber = GREENGY; enemyTypeNumber < ENEMIES_TYPES_NUMBER; enemyTypeNumber++) {
		char enemyDirectory[DIRECTORY_STRING_LENGTH] = "graphics/enemy";
		char bulletDirectory[DIRECTORY_STRING_LENGTH] = "graphics/bullet";
		
		char enemyNumberString[NUMBER_STRING_LENGTH] = "";
		char bulletNumberString[NUMBER_STRING_LENGTH] = "";

		sprintf(enemyNumberString, "%d", enemyTypeNumber);
		strcat(enemyDirectory, enemyNumberString);
		strcat(enemyDirectory, ".bmp");

		sprintf(bulletNumberString, "%d", enemyTypeNumber);
		strcat(bulletDirectory, bulletNumberString);
		strcat(bulletDirectory, ".bmp");

		enemiesList->enemiesSprites[enemyTypeNumber] = SDL_LoadBMP(enemyDirectory);
		if (enemiesList->enemiesSprites[enemyTypeNumber] == NULL)
			LoadingError(SDL_set, enemyDirectory);

		enemiesList->bulletsSprites[enemyTypeNumber] = SDL_LoadBMP(bulletDirectory);
		if (enemiesList->enemiesSprites[enemyTypeNumber] == NULL)
			LoadingError(SDL_set, bulletDirectory);

		for (int destrFrameNumber = 0; destrFrameNumber < 2; destrFrameNumber++) { //now load animations graphics
			char frameNumberString[NUMBER_STRING_LENGTH] = "";
			
			strcpy(enemyDirectory, "graphics/enemy");
			
			strcat(enemyDirectory, enemyNumberString);
			strcat(enemyDirectory, "_destr");
			
			sprintf(frameNumberString, "%d", destrFrameNumber);
			strcat(enemyDirectory, frameNumberString);
			strcat(enemyDirectory, ".bmp");

			enemiesList->enemiesDestructionSprites[enemyTypeNumber * ENEMY_DESTRUCTION_SPRITES_NUMBER + destrFrameNumber] = SDL_LoadBMP(enemyDirectory);
			if (enemiesList->enemiesDestructionSprites[enemyTypeNumber * ENEMY_DESTRUCTION_SPRITES_NUMBER + destrFrameNumber] == NULL)
				LoadingError(SDL_set, enemyDirectory);
		}
	}

	enemiesList->purplegyDynamicSprites[UPDOWN] = SDL_LoadBMP("graphics/enemy2_updown.bmp");
	if (enemiesList->purplegyDynamicSprites[UPDOWN] == NULL)
		LoadingError(SDL_set, "graphics/enemy2_updown.bmp");

	enemiesList->purplegyDynamicSprites[RIGHTLEFT] = SDL_LoadBMP("graphics/enemy2_rightleft.bmp");
	if (enemiesList->purplegyDynamicSprites[UPDOWN] == NULL)
		LoadingError(SDL_set, "graphics/enemy2_rightleft.bmp");
}

void LoadPlayerGraphics(SDL_setData* SDL_set, PlayerData* player) {
	for (int textureNumber = 0; textureNumber < PLAYER_TEXTURES_NUMBER; textureNumber++) {
		char textureDirectory[DIRECTORY_STRING_LENGTH] = "graphics/player_";
		char textureNumberString[NUMBER_STRING_LENGTH] = "";
		char direction[DIRECTORY_STRING_LENGTH] = "";

		if (textureNumber / 2 == UP)
			strcpy(direction, "up_");
		if (textureNumber / 2 == RIGHT)
			strcpy(direction, "right_");
		if (textureNumber / 2 == DOWN)
			strcpy(direction, "down_");
		if (textureNumber / 2 == LEFT)
			strcpy(direction, "left_");
		
		sprintf(textureNumberString, "%d", (textureNumber % 2) + 1);
		strcat(textureDirectory, direction);
		strcat(textureDirectory, textureNumberString);
		strcat(textureDirectory, ".bmp");

		player->playerSprite[textureNumber] = SDL_LoadBMP(textureDirectory);
		if (player->playerSprite[textureNumber] == NULL)
			LoadingError(SDL_set, textureDirectory);
	}

	for (int textureNumber = 0; textureNumber < PLAYER_TEXTURES_NUMBER; textureNumber++) {
		char textureDirectory[DIRECTORY_STRING_LENGTH] = "graphics/player_";
		char textureNumberString[NUMBER_STRING_LENGTH] = "";
		char direction[DIRECTORY_STRING_LENGTH] = "";

		if (textureNumber / 2 == UP)
			strcpy(direction, "up_");
		if (textureNumber / 2 == RIGHT)
			strcpy(direction, "right_");
		if (textureNumber / 2 == DOWN)
			strcpy(direction, "down_");
		if (textureNumber / 2 == LEFT)
			strcpy(direction, "left_");

		sprintf(textureNumberString, "%d", (textureNumber % 2) + 1);
		strcat(textureDirectory, direction);
		strcat(textureDirectory, textureNumberString);
		strcat(textureDirectory, "_inv.bmp");

		player->playerSpriteInv[textureNumber] = SDL_LoadBMP(textureDirectory);
		if (SDL_set->backgrounds[textureNumber] == NULL)
			LoadingError(SDL_set, textureDirectory);
	}

	player->playerBulletSprite = SDL_LoadBMP("graphics/bullet_player.bmp");
	if (player->playerBulletSprite == NULL)
		LoadingError(SDL_set, "graphics/bullet_player.bmp");
}

void LoadBackgroundGraphics(SDL_setData* SDL_set) {
	for (int levelNumber = 0; levelNumber < LEVELS_NUMBER; levelNumber++) {
		char backgroundDirectory[DIRECTORY_STRING_LENGTH] = "graphics/background";
		char levelNumberString[NUMBER_STRING_LENGTH] = "";

		sprintf(levelNumberString, "%d", levelNumber);
		strcat(backgroundDirectory, levelNumberString);
		strcat(backgroundDirectory, ".bmp");

		SDL_set->backgrounds[levelNumber] = SDL_LoadBMP(backgroundDirectory);
		if (SDL_set->backgrounds[levelNumber] == NULL)
			LoadingError(SDL_set, backgroundDirectory);
	}

	for (int menuOptionNumber = 1; menuOptionNumber < MENU_OPTIONS_NUMBER; menuOptionNumber++) {
		char backgroundDirectory[DIRECTORY_STRING_LENGTH] = "graphics/menu_l";
		char levelNumberString[NUMBER_STRING_LENGTH] = "";

		sprintf(levelNumberString, "%d", menuOptionNumber);
		strcat(backgroundDirectory, levelNumberString);
		strcat(backgroundDirectory, ".bmp");

		SDL_set->menuBackgrounds[menuOptionNumber - 1] = SDL_LoadBMP(backgroundDirectory);
		if (SDL_set->menuBackgrounds[menuOptionNumber - 1] == NULL)
			LoadingError(SDL_set, backgroundDirectory);
	}
	SDL_set->menuBackgrounds[MENU_QUIT] = SDL_LoadBMP("graphics/menu_quit.bmp");
	if (SDL_set->menuBackgrounds[MENU_QUIT] == NULL)
		LoadingError(SDL_set, "graphics/menu_quit.bmp");

	SDL_set->gameoverViews[GAMEOVER_RETRY] = SDL_LoadBMP("graphics/gameover_retry.bmp");
	if (SDL_set->gameoverViews[GAMEOVER_RETRY] == NULL)
		LoadingError(SDL_set, "graphics/gameover_retry.bmp");
	SDL_set->gameoverViews[GAMEOVER_MENU] = SDL_LoadBMP("graphics/gameover_menu.bmp");
	if (SDL_set->gameoverViews[GAMEOVER_MENU] == NULL)
		LoadingError(SDL_set, "graphics/gameover_menu.bmp");
}

void LoadGradesGraphics(SDL_setData* SDL_set) {
	for (int gradeNumber = 0; gradeNumber < GRADES_NUMBER; gradeNumber++) {
		char gradeDirectory[DIRECTORY_STRING_LENGTH] = "graphics/grade";

		char gradeNumberString[NUMBER_STRING_LENGTH] = "";

		sprintf(gradeNumberString, "%d", gradeNumber);
		strcat(gradeDirectory, gradeNumberString);
		strcat(gradeDirectory, ".bmp");

		SDL_set->grades[gradeNumber] = SDL_LoadBMP(gradeDirectory);
		if (SDL_set->grades[gradeNumber] == NULL)
			LoadingError(SDL_set, gradeDirectory);
	}

	SDL_set->gradeBackground = SDL_LoadBMP("graphics/grade_background.bmp");
	if (SDL_set->gradeBackground == NULL)
		LoadingError(SDL_set, "graphics/grade_background.bmp");

	SDL_set->gradeAnimationTexture[0] = SDL_LoadBMP("graphics/grade_change0.bmp");
	if (SDL_set->gradeAnimationTexture[0] == NULL)
		LoadingError(SDL_set, "graphics/grade_change0.bmp");
	SDL_set->gradeAnimationTexture[1] = SDL_LoadBMP("graphics/grade_change1.bmp");
	if (SDL_set->gradeAnimationTexture[1] == NULL)
		LoadingError(SDL_set, "graphics/grade_change1.bmp");
}


void LoadingError(SDL_setData* SDL_set, char* directory) {
	printf("SDL_LoadBMP(%s) error: %s\n", directory, SDL_GetError());
	FreeAll(SDL_set);
	exit(1);
}


void Game(SDL_setData* SDL_set, PlayerData* player, EnemiesListData* enemiesList, BonusesListData* bonusesList, ColoursData* colours, TimeData* time, QuitingData* quiting) {
	if (player->newLevel) {
		player->x = SCREEN_WIDTH / 2;
		player->y = SCREEN_HEIGHT / 2;

		DestroyAllObjects(enemiesList, player, bonusesList);

		InitialiseEnemies(enemiesList, player->level);
		player->newLevel = FALSE;
		player->isLevelCreated[player->level] = TRUE;
	}

	UpdateBonuses(bonusesList, *player);
	Shoot(time, enemiesList,* player);
	MoveBullets(enemiesList, *player);
	
	if (player->level == PURPLEGY)
		MovePurplegies(enemiesList);

	RenderFrame(SDL_set, time, colours, player, *enemiesList, *bonusesList);

	CheckCollision(*enemiesList, player, bonusesList);
	DestroyBullets(*enemiesList, player, player->level);
	DestroyEnemies(enemiesList, player->level);


	UpdateTime(time, enemiesList, player, bonusesList);
	UpdateScore(player, *time, quiting);

	HandleEvents(SDL_set, player, enemiesList, *time, quiting, *bonusesList);
}


void Menu(SDL_setData* SDL_set, PlayerData* player, QuitingData* quiting, TimeData* time) {
	DrawSurface(SDL_set->screen, SDL_set->menuBackgrounds[player->menuOption], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	while (SDL_PollEvent(&SDL_set->event)) {
		if (SDL_set->event.type == SDL_KEYDOWN) {
			switch (SDL_set->event.key.keysym.sym) {
			case SDLK_ESCAPE:
				quiting->quit = TRUE;
				break;
			case SDLK_n:
				quiting->restart = TRUE;
				break;
			case SDLK_DOWN:
				player->menuOption = (player->menuOption + 1) % MENU_OPTIONS_NUMBER;
				break;
			case SDLK_UP:
				if (player->menuOption > 0)
					player->menuOption--;
				else
					player->menuOption = MENU_OPTIONS_NUMBER - 1;
				break;
			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				if (player->menuOption == MENU_QUIT) {
					quiting->quit = TRUE;
				}
				else {
					player->level = player->menuOption;
					player->gameState = PLAYING;

					time->lastTime = SDL_GetTicks();
				}
				break;
			}
		}
		if (SDL_set->event.type == SDL_QUIT)
			quiting->quit = 1;
	}
	

	SDL_UpdateTexture(SDL_set->scrtex, NULL, SDL_set->screen->pixels, SDL_set->screen->pitch);
	//SDL_RenderClear(SDL_set->renderer);
	SDL_RenderCopy(SDL_set->renderer, SDL_set->scrtex, NULL, NULL);
	SDL_RenderPresent(SDL_set->renderer);
}

void GameOver(SDL_setData* SDL_set,  PlayerData* player, QuitingData* quiting) {
	DrawSurface(SDL_set->screen, SDL_set->gameoverViews[player->gameOverView], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	
	while (SDL_PollEvent(&SDL_set->event)) {
		if (SDL_set->event.type == SDL_KEYDOWN) {
			switch (SDL_set->event.key.keysym.sym) {
			case SDLK_UP:
			case SDLK_DOWN:
				if (player->gameOverView == GAMEOVER_MENU)
					player->gameOverView = GAMEOVER_RETRY;
				else
					player->gameOverView = GAMEOVER_MENU;
				break;
			case SDLK_n:
				quiting->restart = TRUE;
				break;
			case SDLK_ESCAPE:
				quiting->quit = TRUE;
				break;
			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				if (player->gameOverView == GAMEOVER_MENU) {
					quiting->restart = TRUE;
				}
				else {
					quiting->restart = TRUE;
					quiting->skipMenu = TRUE;
					quiting->skipToLevel = player->level;
				}
				break;
			}
		}
		if (SDL_set->event.type == SDL_QUIT)
			quiting->quit = 1;
	}

	SDL_UpdateTexture(SDL_set->scrtex, NULL, SDL_set->screen->pixels, SDL_set->screen->pitch);
	//SDL_RenderClear(SDL_set->renderer);
	SDL_RenderCopy(SDL_set->renderer, SDL_set->scrtex, NULL, NULL);
	SDL_RenderPresent(SDL_set->renderer);
}

void RenderFrame(SDL_setData* SDL_set, TimeData* time, ColoursData* colours, PlayerData* player, EnemiesListData enemiesList, BonusesListData bonusesList) {
	SDL_FillRect(SDL_set->screen, NULL, colours->black);
	DrawSurface(SDL_set->screen, SDL_set->backgrounds[player->level], player->x, player->y);

	DrawPlayer(SDL_set, player, time, *colours);
	DrawEnemies(SDL_set, enemiesList, player->level, *colours, *time);
	DrawBonuses(SDL_set, bonusesList);

	DrawPlayerBullets(SDL_set, *player);

	DrawInfo(SDL_set, time, colours, *player);
	DrawGrade(SDL_set, player);

	SDL_UpdateTexture(SDL_set->scrtex, NULL, SDL_set->screen->pixels, SDL_set->screen->pitch);
	//SDL_RenderClear(SDL_set->renderer);
	SDL_RenderCopy(SDL_set->renderer, SDL_set->scrtex, NULL, NULL);
	SDL_RenderPresent(SDL_set->renderer);
}

void HandleEvents(SDL_setData* SDL_set, PlayerData* player, EnemiesListData* enemiesList, TimeData time, QuitingData* quiting, BonusesListData bonusesList) {
	while (SDL_PollEvent(&SDL_set->event)) {
		switch (SDL_set->event.type) {
		case SDL_KEYDOWN:
			switch (SDL_set->event.key.keysym.sym) {
			case SDLK_ESCAPE:
				quiting->quit = TRUE;
				break;
			case SDLK_UP:
				if (player->y  + PLAYER_STEP < SCREEN_HEIGHT / 2 + LEVEL_HEIGHT / 2 - PLAYER_HEIGHT / 2) {
					player->direction = UP;
					player->animating = TRUE;
					player->y += PLAYER_STEP;
					MoveEnemiesAndPlayerBullets(enemiesList, player, bonusesList, 'y', '+');
				}
				break;
			case SDLK_DOWN:
				if (player->y - PLAYER_STEP > SCREEN_HEIGHT / 2 - LEVEL_HEIGHT / 2 + PLAYER_HEIGHT / 2) {
					player->direction = DOWN;
					player->animating = TRUE;
					player->y -= PLAYER_STEP;
					MoveEnemiesAndPlayerBullets(enemiesList, player, bonusesList, 'y', '-');
				}
				break;
			case SDLK_RIGHT:
				if (player->x - PLAYER_STEP > SCREEN_WIDTH / 2 - LEVEL_WIDTH / 2 + PLAYER_WIDTH / 2) {
					player->direction = RIGHT;
					player->animating = TRUE;
					player->x -= PLAYER_STEP;
					MoveEnemiesAndPlayerBullets(enemiesList, player, bonusesList, 'x', '-');
				}
				break;
			case SDLK_LEFT:
				if (player->x + PLAYER_STEP < SCREEN_WIDTH / 2 + LEVEL_WIDTH / 2 - PLAYER_WIDTH / 2) {
					player->direction = LEFT;
					player->animating = TRUE;
					player->x += PLAYER_STEP;
					MoveEnemiesAndPlayerBullets(enemiesList, player, bonusesList, 'x', '+');
				}
				break;
			case SDLK_SPACE:
				ShootPlayer(time, player);
				break;
			case SDLK_n:
				quiting->restart = TRUE;
				break;
			}
			break;
		case SDL_KEYUP:
			player->animating = FALSE;
			break;
		case SDL_QUIT:
			quiting->quit = 1;
			break;
		}
	}
}


void DrawEnemies(SDL_setData* SDL_set, EnemiesListData enemiesList, int level, ColoursData colours, TimeData time) {
	for (int enemyNumber = 0; enemyNumber < enemiesList.enemiesNumber[level]; enemyNumber++) {
		SDL_Surface* currentTexture;

		if (enemiesList.enemies[level][enemyNumber].active) {
			currentTexture = enemiesList.enemiesSprites[level];

			if (level == PURPLEGY) {
				if ((int)floor(time.worldTime * 10) % 10 < 5) { //change the frame every 0.5 sec
					if (enemiesList.enemies[level][enemyNumber].moveDirection == UP || enemiesList.enemies[level][enemyNumber].moveDirection == DOWN)
						currentTexture = enemiesList.purplegyDynamicSprites[UPDOWN];
					else
						currentTexture = enemiesList.purplegyDynamicSprites[RIGHTLEFT];
				}
			}
			
			DrawSurface(SDL_set->screen, currentTexture, enemiesList.enemies[level][enemyNumber].x, enemiesList.enemies[level][enemyNumber].y);
			
			int lifeBarX = enemiesList.enemies[level][enemyNumber].x - LIFE_BAR_RELATIVE_X;
			int lifeBarY = enemiesList.enemies[level][enemyNumber].y - LIFE_BAR_RELATIVE_Y;
			int lifeBarWidth = enemiesList.enemies[level][enemyNumber].life * PLAYER_INITIAL_LIFE / enemiesList.initialLifes[level] * PLAYER_LIFE_BAR_WIDTH_MULTIPLIER;
			
			if(lifeBarX >= 0 && lifeBarX <= SCREEN_WIDTH - lifeBarWidth && lifeBarY >=0 && lifeBarY <= SCREEN_HEIGHT - LIFE_BAR_HEIGHT)
				DrawRectangle(SDL_set->screen, lifeBarX, lifeBarY, lifeBarWidth, LIFE_BAR_HEIGHT, colours.black, colours.red);
		}
		else {
			if (enemiesList.enemies[level][enemyNumber].animatingTime > ENEMY_DESTRUCTION_SPRITES_NUMBER * ONE_FRAME_TIME)
				continue;

			if (enemiesList.enemies[level][enemyNumber].animatingTime <= ONE_FRAME_TIME)
				currentTexture = enemiesList.enemiesDestructionSprites[level * ENEMY_DESTRUCTION_SPRITES_NUMBER];
			else
				currentTexture = enemiesList.enemiesDestructionSprites[level * ENEMY_DESTRUCTION_SPRITES_NUMBER + 1];

			DrawSurface(SDL_set->screen, currentTexture, enemiesList.enemies[level][enemyNumber].x, enemiesList.enemies[level][enemyNumber].y);
		}

		DrawBullets(SDL_set, enemiesList, level, enemyNumber);
	}
}

void DrawPlayer(SDL_setData* SDL_set, PlayerData* player, TimeData* time, ColoursData colours) {
	SDL_Surface* currentTexture;
	
	if (player->animating) {
		if (player->timeToNextFrame <= 0) {
			player->timeToNextFrame = ONE_FRAME_TIME;
			player->frame = (player->frame == 1) ? 2 : 1;
		}
	}
	
	if (player->invincible)
		currentTexture = player->playerSpriteInv[player->direction * 2 + player->frame - 1];
	else
		currentTexture = player->playerSprite[player->direction * 2 + player->frame - 1];

	DrawSurface(SDL_set->screen, currentTexture, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	
	DrawRectangle(SDL_set->screen, SCREEN_WIDTH / 2 - LIFE_BAR_RELATIVE_X, SCREEN_HEIGHT / 2 - LIFE_BAR_RELATIVE_Y, player->life * PLAYER_LIFE_BAR_WIDTH_MULTIPLIER, LIFE_BAR_HEIGHT, colours.black, colours.red);

	//sprintf(player->lifeString, "%d", player->life);
	//DrawString(SDL_set->screen, SCREEN_WIDTH / 2 - PLAYER_WIDTH / 3, SCREEN_HEIGHT / 2 - PLAYER_HEIGHT, player->lifeString, SDL_set->charset);
}

void DrawBonuses(SDL_setData* SDL_set, BonusesListData bonusesList) {
	for (int bonusNumber = 0; bonusNumber < bonusesList.bonusesNumber; bonusNumber++) {
		DrawSurface(SDL_set->screen, bonusesList.bonusSprite, bonusesList.bonuses[bonusNumber].x, bonusesList.bonuses[bonusNumber].y);
	}
}

void DrawGrade(SDL_setData* SDL_set, PlayerData* player) {
	if (player->gradeChangeTime > (GRADE_ANIMATION_FRAMES_NUMBER - 1) * ONE_FRAME_TIME)
		DrawSurface(SDL_set->screen, SDL_set->gradeAnimationTexture[BIG_STAR], GRADE_X, GRADE_Y);
	else if (player->gradeChangeTime > (GRADE_ANIMATION_FRAMES_NUMBER - 2) * ONE_FRAME_TIME)
		DrawSurface(SDL_set->screen, SDL_set->gradeAnimationTexture[SMALL_STAR], GRADE_X, GRADE_Y);
	else if (player->gradeChangeTime > (GRADE_ANIMATION_FRAMES_NUMBER - 3) * ONE_FRAME_TIME)
		DrawSurface(SDL_set->screen, SDL_set->gradeAnimationTexture[BIG_STAR], GRADE_X, GRADE_Y);
	else {
		DrawSurface(SDL_set->screen, SDL_set->gradeBackground, GRADE_X, GRADE_Y);
		DrawSurface(SDL_set->screen, SDL_set->grades[player->grade], GRADE_X, GRADE_Y);
	}
}

void DrawBullets(SDL_setData* SDL_set, EnemiesListData enemiesList, int enemyTypeNumber, int enemyNumber) {
	for (int bulletNumber = 0; bulletNumber < enemiesList.enemies[enemyTypeNumber][enemyNumber].bulletsNumber; bulletNumber++) {
		DrawSurface(SDL_set->screen, enemiesList.bulletsSprites[enemyTypeNumber], enemiesList.enemies[enemyTypeNumber][enemyNumber].bullets[bulletNumber].x, enemiesList.enemies[enemyTypeNumber][enemyNumber].bullets[bulletNumber].y);
	}
}

void DrawPlayerBullets(SDL_setData* SDL_set, PlayerData player) {
	for (int bulletNumber = 0; bulletNumber < player.bulletsNumber; bulletNumber++) {
		DrawSurface(SDL_set->screen, player.playerBulletSprite, player.bullets[bulletNumber].x, player.bullets[bulletNumber].y);
	}
}

void MoveEnemiesAndPlayerBullets(EnemiesListData* enemiesList, PlayerData* player, BonusesListData bonusesList, char dimension, char sign) {
	int addition = (sign == '+') ? PLAYER_STEP : -PLAYER_STEP;

	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[player->level]; enemyNumber++) {
		if (dimension == 'x')
			enemiesList->enemies[player->level][enemyNumber].x += addition;
		else
			enemiesList->enemies[player->level][enemyNumber].y += addition;

		for (int bulletNumber = 0; bulletNumber < enemiesList->enemies[player->level][enemyNumber].bulletsNumber; bulletNumber++) {
			if (dimension == 'x')
				enemiesList->enemies[player->level][enemyNumber].bullets[bulletNumber].x += addition;
			else
				enemiesList->enemies[player->level][enemyNumber].bullets[bulletNumber].y += addition;
		}
	}

	for (int playerBulletNumber = 0; playerBulletNumber < player->bulletsNumber; playerBulletNumber++) {
		if (dimension == 'x')
			player->bullets[playerBulletNumber].x += addition;
		else
			player->bullets[playerBulletNumber].y += addition;
	}

	for (int bonusNumber = 0; bonusNumber < bonusesList.bonusesNumber; bonusNumber++) {
		if (dimension == 'x')
			bonusesList.bonuses[bonusNumber].x += addition;
		else
			bonusesList.bonuses[bonusNumber].y += addition;
	}
}

void Shoot(TimeData* time, EnemiesListData* enemiesList, PlayerData player) {
	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[player.level]; enemyNumber++) {
		if (!enemiesList->enemies[player.level][enemyNumber].active)
			continue;
			
		if (abs(enemiesList->enemies[player.level][enemyNumber].x - SCREEN_WIDTH / 2 + PLAYER_WIDTH / 2) <= SCREEN_WIDTH / 2 && abs(enemiesList->enemies[player.level][enemyNumber].y - SCREEN_HEIGHT / 2 - PLAYER_HEIGHT) <= SCREEN_HEIGHT / 2) {
			enemiesList->enemies[player.level][enemyNumber].shootingYet = TRUE;
		}
		if(enemiesList->enemies[player.level][enemyNumber].shootingYet == TRUE) {
			if (enemiesList->enemies[player.level][enemyNumber].timeSinceLastShot >= enemiesList->shootingIntervals[player.level]) {
				enemiesList->enemies[player.level][enemyNumber].timeSinceLastShot = 0;
				if ((int)(time->worldTime) % 20 < 18)
					CreateNewBullet(enemiesList->enemies, player.level, enemyNumber, *time);
			}
		}
	}
}

void ShootPlayer(TimeData time, PlayerData* player) {
	if (player->timeSinceLastShot >= PLAYER_SHOOTING_INTERVAL) {
		player->timeSinceLastShot = 0;

		player->bulletsNumber++;
		BulletData* newBullets = (BulletData*)malloc(player->bulletsNumber * sizeof(BulletData));

		for (int bulletNumber = 0; bulletNumber < player->bulletsNumber - 1; bulletNumber++) {
			newBullets[bulletNumber] = player->bullets[bulletNumber];
		}

		newBullets[player->bulletsNumber - 1].x = SCREEN_WIDTH / 2;
		newBullets[player->bulletsNumber - 1].y = SCREEN_HEIGHT / 2;
		newBullets[player->bulletsNumber - 1].absX = SCREEN_WIDTH + BULLET_SIZE / 2 - player->x;
		newBullets[player->bulletsNumber - 1].absY = SCREEN_HEIGHT + BULLET_SIZE / 2 - player->y;

		newBullets[player->bulletsNumber - 1].active = TRUE;

		newBullets[player->bulletsNumber - 1].shotDirection = player->direction;
		newBullets[player->bulletsNumber - 1].timeSinceLastMove = 0;

		free(player->bullets);
		player->bullets = newBullets;
	}
}

void MoveBullets(EnemiesListData* enemiesList, PlayerData player) {
	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[player.level]; enemyNumber++) {
		for (int bulletNumber = 0; bulletNumber < enemiesList->enemies[player.level][enemyNumber].bulletsNumber; bulletNumber++) {
			if (enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber].timeSinceLastMove >= enemiesList->bulletsJumpIntervals[player.level]) {
				int bulletAbsX = enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber].absX;
				int bulletAbsY = enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber].absY;

				if (bulletAbsX < SCREEN_WIDTH / 2 + LEVEL_WIDTH / 2 - BULLET_SIZE / 2 && bulletAbsX > SCREEN_WIDTH / 2 - LEVEL_WIDTH / 2 + BULLET_SIZE / 2) {
					MoveBulletOfAnEnemy(player.level, &enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber], 'x');
				}
				else {
					enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber].active = FALSE;
				}
				if (bulletAbsY < SCREEN_HEIGHT / 2 + LEVEL_HEIGHT / 2 - BULLET_SIZE / 2 && bulletAbsY > SCREEN_HEIGHT / 2 - LEVEL_HEIGHT / 2 + BULLET_SIZE / 2) {
					MoveBulletOfAnEnemy(player.level, &enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber], 'y');
				}
				else {
					enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber].active = FALSE;
				}

				enemiesList->enemies[player.level][enemyNumber].bullets[bulletNumber].timeSinceLastMove = 0;
			}
		}
	}

	MovePlayerBullets(player);
}

void MovePurplegies(EnemiesListData* enemiesList) {
	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[PURPLEGY]; enemyNumber++) {
		if (enemiesList->enemies[PURPLEGY][enemyNumber].timeSinceLastMove >= PURPLEGY_MOVE_INTERVAL) {
			int absX = enemiesList->enemies[PURPLEGY][enemyNumber].absX;
			int absY = enemiesList->enemies[PURPLEGY][enemyNumber].absY;
			
			int x = enemiesList->enemies[PURPLEGY][enemyNumber].absX;
			int y = enemiesList->enemies[PURPLEGY][enemyNumber].absY;

			int xAddition, yAddition;

			SetMovementAddition(enemiesList->enemies[PURPLEGY][enemyNumber].moveDirection, &xAddition, &yAddition, PURPLEGY_JUMP);

			if (absX + xAddition < SCREEN_WIDTH / 2 + LEVEL_WIDTH / 2 - PLAYER_WIDTH / 2 && absX + xAddition > SCREEN_WIDTH / 2 - LEVEL_WIDTH / 2 + PLAYER_WIDTH / 2) {
				enemiesList->enemies[PURPLEGY][enemyNumber].absX += xAddition;
				enemiesList->enemies[PURPLEGY][enemyNumber].x += xAddition;
				
			}
			else {
				enemiesList->enemies[PURPLEGY][enemyNumber].moveDirection = enemiesList->enemies[PURPLEGY][enemyNumber].moveDirection == RIGHT ? LEFT : RIGHT;
			}
			if (absY + yAddition < SCREEN_HEIGHT / 2 + LEVEL_HEIGHT / 2 - PLAYER_HEIGHT / 2 && absY + yAddition > SCREEN_HEIGHT / 2 - LEVEL_HEIGHT / 2 + PLAYER_HEIGHT / 2) {
				enemiesList->enemies[PURPLEGY][enemyNumber].absY += yAddition;
				enemiesList->enemies[PURPLEGY][enemyNumber].y += yAddition;
			}
			else {
				enemiesList->enemies[PURPLEGY][enemyNumber].moveDirection = enemiesList->enemies[PURPLEGY][enemyNumber].moveDirection == UP ? DOWN : UP;
			}

			enemiesList->enemies[PURPLEGY][enemyNumber].timeSinceLastMove = 0;
		}
	}
}

void MoveBulletOfAnEnemy(int enemyTypeNumber, BulletData* bullet, char coordinate) {
	int xAddition, yAddition;

	SetMovementAddition(bullet->shotDirection, &xAddition, &yAddition, BULLET_JUMP);

	if (enemyTypeNumber == GREENGY) {
		
		bullet->distanceTraveled += PURPLEGY_BULLET_JUMP;
		bullet->radius += PURPLEGY_BULLET_RADIUS_STEP;

		if (bullet->shotDirection == RIGHT || bullet->shotDirection == LEFT) {
			bullet->x += xAddition;
			bullet->y += sin(bullet->trigonometricalMultiplier * bullet->distanceTraveled);
			bullet->absX += xAddition;
			bullet->absY += sin(bullet->trigonometricalMultiplier * bullet->distanceTraveled);
		}
		else {
			bullet->x += sin(bullet->trigonometricalMultiplier * bullet->distanceTraveled);
			bullet->y += yAddition;
			bullet->absX += sin(bullet->trigonometricalMultiplier * bullet->distanceTraveled);
			bullet->absY += yAddition;
		}
	}

	if(enemyTypeNumber == BLUEGY)
		switch (coordinate) {
		case 'x':
			bullet->absX += xAddition;
			bullet->x += xAddition;
			break;
		case 'y':
			bullet->absY += yAddition;
			bullet->y += yAddition;
		}
	
	if (enemyTypeNumber == BLUEGY) {
		if (bullet->timeSinceLastTurn >= bullet->oneDirectionTime) {
			bullet->shotDirection = (bullet->shotDirection + 1) % DIRECTIONS_NUMBER;
			bullet->timeSinceLastTurn = 0;
			bullet->oneDirectionTime += BLUEGY_ONE_DIRECTION_TIME_ADDITION;
		}
	}

	if (enemyTypeNumber == PURPLEGY) {
		bullet->distanceTraveled += PURPLEGY_BULLET_JUMP;
		bullet->radius += PURPLEGY_BULLET_RADIUS_STEP;

		bullet->x += sin(bullet->distanceTraveled) *bullet->radius;
		bullet->y += cos(bullet->distanceTraveled) *bullet->radius;
		bullet->absX += sin(bullet->distanceTraveled) * bullet->radius;
		bullet->absY += cos(bullet->distanceTraveled) * bullet->radius;
	}
}

void MovePlayerBullets(PlayerData player) {
	for (int bulletNumber = 0; bulletNumber < player.bulletsNumber; bulletNumber++) {
		if (player.bullets[bulletNumber].timeSinceLastMove >= PLAYER_BULLETS_JUMP_INTERVAL) {
			int xAddition, yAddition;

			SetMovementAddition(player.bullets[bulletNumber].shotDirection, &xAddition, &yAddition, PLAYER_BULLET_JUMP);

			//printf("%d %d\n%d %d\n------\n", player.bullets[bulletNumber].absX, player.bullets[bulletNumber].absY, player.bullets[bulletNumber].x, player.bullets[bulletNumber].y);

			if (player.bullets[bulletNumber].absX < SCREEN_WIDTH / 2 + LEVEL_WIDTH / 2 - BULLET_SIZE / 2 && player.bullets[bulletNumber].absX > SCREEN_WIDTH / 2 - LEVEL_WIDTH / 2 + BULLET_SIZE / 2) {
				player.bullets[bulletNumber].absX += xAddition;
				player.bullets[bulletNumber].x += xAddition;
			}
			else {
				player.bullets[bulletNumber].active = FALSE;
			}
			if (player.bullets[bulletNumber].absY < SCREEN_HEIGHT / 2 + LEVEL_HEIGHT / 2 - BULLET_SIZE / 2 && player.bullets[bulletNumber].absY > SCREEN_HEIGHT / 2 - LEVEL_HEIGHT / 2 + BULLET_SIZE / 2) {
				player.bullets[bulletNumber].absY += yAddition;
				player.bullets[bulletNumber].y += yAddition;
			}
			else {
				player.bullets[bulletNumber].active = FALSE;
			}

			player.bullets[bulletNumber].timeSinceLastMove = 0;
		}
	}
}

void CheckCollision(EnemiesListData enemiesList, PlayerData* player, BonusesListData* bonusesList) {
	for (int enemyNumber = 0; enemyNumber < enemiesList.enemiesNumber[player->level]; enemyNumber++) {
		CheckIfPlayerHit(enemiesList, player, enemyNumber);
		
		if (enemiesList.enemies[player->level][enemyNumber].active)
			CheckIfEnemyHit(enemiesList, player, enemyNumber);
	}

	CheckIfBonusCollected(player, bonusesList);
}

void CheckIfPlayerHit(EnemiesListData enemiesList, PlayerData* player, int enemyNumber) {
	for (int bulletNumber = 0; bulletNumber < enemiesList.enemies[player->level][enemyNumber].bulletsNumber; bulletNumber++) {
		int bulletX = enemiesList.enemies[player->level][enemyNumber].bullets[bulletNumber].x;
		int bulletY = enemiesList.enemies[player->level][enemyNumber].bullets[bulletNumber].y;

		if ((bulletX > (SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2 - BULLET_SIZE / 2) && bulletX < (SCREEN_WIDTH / 2 + PLAYER_WIDTH / 2 + BULLET_SIZE / 2))
			&& (bulletY >= (SCREEN_HEIGHT / 2 - PLAYER_HEIGHT / 2 - BULLET_SIZE / 2) && bulletY <= (SCREEN_HEIGHT / 2 + PLAYER_HEIGHT / 2 + BULLET_SIZE / 2))) {
			if (!player->invincible) {
				if (player->life - BASIC_LIFE_TAKEN_BY_ENEMY >= 0)
					player->life -= BASIC_LIFE_TAKEN_BY_ENEMY;
				else
					player->life = 0;
				
				enemiesList.enemies[player->level][enemyNumber].bullets[bulletNumber].active = FALSE;
				player->invincible = TRUE;
				player->invincibilityTimeLeft = PLAYER_INVINCIBILITY_TIME;
				
			}
		}
	}
}

void CheckIfEnemyHit(EnemiesListData enemiesList, PlayerData* player, int enemyNumber) {
	for (int playerBulletNumber = 0; playerBulletNumber < player->bulletsNumber; playerBulletNumber++) {
		int playerBulletX = player->bullets[playerBulletNumber].absX;
		int playerBulletY = player->bullets[playerBulletNumber].absY;

		int enemyAbsX = enemiesList.enemies[player->level][enemyNumber].absX;
		int enemyAbsY = enemiesList.enemies[player->level][enemyNumber].absY;

		if ((playerBulletX > enemyAbsX - PLAYER_WIDTH / 2 + BULLET_SIZE / 2 && playerBulletX < enemyAbsX + PLAYER_WIDTH / 2 + BULLET_SIZE / 2)
			&& (playerBulletY > enemyAbsY - PLAYER_HEIGHT / 2 + BULLET_SIZE / 2 && playerBulletY < enemyAbsY + PLAYER_HEIGHT / 2 + BULLET_SIZE / 2)) {
			player->bullets[playerBulletNumber].active = FALSE;

			if (enemiesList.enemies[player->level][enemyNumber].life > 1)
				enemiesList.enemies[player->level][enemyNumber].life--;
			else {
				enemiesList.enemies[player->level][enemyNumber].active = FALSE;
			}

			if (enemiesList.enemies[player->level][enemyNumber].timeSinceLastHit <= MAX_HITS_IN_A_ROW_INTERVAL)
				enemiesList.enemies[player->level][enemyNumber].pointsGiven++;
			else
				enemiesList.enemies[player->level][enemyNumber].pointsGiven = player->level + 1;

			enemiesList.enemies[player->level][enemyNumber].timeSinceLastHit = 0;

			player->killingPoints += enemiesList.enemies[player->level][enemyNumber].pointsGiven;
		}
	}
}

void CheckIfBonusCollected(PlayerData* player, BonusesListData* bonusesList) {
	for (int bonusNumber = 0; bonusNumber < bonusesList->bonusesNumber; bonusNumber++) {
		int bonusX = bonusesList->bonuses[bonusNumber].x;
		int bonusY = bonusesList->bonuses[bonusNumber].y;

		if ((bonusX > (SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2 - BONUS_WIDTH / 2) && bonusX < (SCREEN_WIDTH / 2 + PLAYER_WIDTH / 2 + BONUS_WIDTH / 2))
			&& (bonusY >= (SCREEN_HEIGHT / 2 - PLAYER_HEIGHT / 2 - BONUS_HEIGHT / 2) && bonusY <= (SCREEN_HEIGHT / 2 + PLAYER_HEIGHT / 2 + BONUS_HEIGHT / 2))) {
			if (player->life < PLAYER_INITIAL_LIFE) {
				if (player->life + BONUS_LIFE <= PLAYER_INITIAL_LIFE)
					player->life += BONUS_LIFE;
				else
					player->life = PLAYER_INITIAL_LIFE;

				bonusesList->bonuses[bonusNumber].active = FALSE;
			}
		}
	}
}

void UpdateTime(TimeData* time, EnemiesListData* enemiesList, PlayerData* player, BonusesListData* bonusesList) {
	double timeNow = SDL_GetTicks();

	double delta = (timeNow - time->lastTime) * 0.001;
	time->lastTime = timeNow;
	
	time->worldTime += delta;

	player->timeSinceLastShot += delta;

	if (player->animating)
		player->timeToNextFrame -= delta;

	for (int bulletNumber = 0; bulletNumber < player->bulletsNumber; bulletNumber++) {
		player->bullets[bulletNumber].timeSinceLastMove += delta;
	}

	if (player->invincibilityTimeLeft > 0)
		player->invincibilityTimeLeft -= delta;
	else
		player->invincible = FALSE;

	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[player->level]; enemyNumber++) {
		enemiesList->enemies[player->level][enemyNumber].timeSinceLastShot += delta;
		enemiesList->enemies[player->level][enemyNumber].timeSinceLastHit += delta;

		if (player->level == PURPLEGY)
			enemiesList->enemies[PURPLEGY][enemyNumber].timeSinceLastMove += delta;

		if (!enemiesList->enemies[player->level][enemyNumber].active)
			enemiesList->enemies[player->level][enemyNumber].animatingTime += delta;
			
		for (int bulletNumber = 0; bulletNumber < enemiesList->enemies[player->level][enemyNumber].bulletsNumber; bulletNumber++) {
			enemiesList->enemies[player->level][enemyNumber].bullets[bulletNumber].timeSinceLastMove += delta;
			enemiesList->enemies[player->level][enemyNumber].bullets[bulletNumber].timeSinceLastTurn += delta;
		}
	}

	if (player->gradeChangeTime > 0)
		player->gradeChangeTime -= delta;

	bonusesList->timeSinceLastBonus += delta;
}

void DrawInfo(SDL_setData* SDL_set, TimeData* time, ColoursData* colours, PlayerData player) {
	char text[128];

	DrawSurface(SDL_set->screen, SDL_set->infoBackground, SCREEN_WIDTH / 2, 0);

	sprintf(text, "Time from start: %.1lf s. Press SPACE to shoot", time->worldTime);
	DrawString(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER, INTERLINE, text, SDL_set->charsetDark);

	sprintf(text, "Esc - exit, n - new game, \032\031\030\033 - movement");
	DrawString(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER, STRING_LINE_HEIGHT + 2 * INTERLINE, text, SDL_set->charsetDark);

	sprintf(text, "Score: %d", player.score);
	DrawString(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER, 2 * STRING_LINE_HEIGHT + 3 * INTERLINE, text, SDL_set->charsetDark);

	if (player.level < 2) {
		sprintf(text, "Progress to next lvl:");
		DrawString(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER - PROGRESS_BAR_WIDTH + 14, 3 * STRING_LINE_HEIGHT + 4 * INTERLINE, text, SDL_set->charsetDark);
		DrawRectangle(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER + PROGRESS_BAR_WIDTH - 14, 3 * STRING_LINE_HEIGHT + 4 * INTERLINE, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, colours->black, colours->black);

		int progressBarGreen = (player.killingPoints - player.level * NEXT_LEVEL_LIMIT) * PROGRESS_BAR_WIDTH / NEXT_LEVEL_LIMIT;

		DrawRectangle(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER + PROGRESS_BAR_WIDTH - 14, 3 * STRING_LINE_HEIGHT + 4 * INTERLINE, progressBarGreen, PROGRESS_BAR_HEIGHT, colours->black, colours->green);
	}
	else {
		sprintf(text, "Finale!");
		DrawString(SDL_set->screen, SDL_set->screen->w / 2 - strlen(text) * STRING_PRINTING_MULTIPLIER, 3 * STRING_LINE_HEIGHT + 4 * INTERLINE, text, SDL_set->charsetDark);
	}
}


void CreateNewBullet(EnemyData** enemies, int enemyTypeNumber, int enemyNumber, TimeData time) {
	enemies[enemyTypeNumber][enemyNumber].bulletsNumber++;
	BulletData* newBullets = (BulletData*)malloc(enemies[enemyTypeNumber][enemyNumber].bulletsNumber * sizeof(BulletData));

	for (int bulletNumber = 0; bulletNumber < enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1; bulletNumber++) {
		newBullets[bulletNumber] = enemies[enemyTypeNumber][enemyNumber].bullets[bulletNumber];
	}

	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].x = enemies[enemyTypeNumber][enemyNumber].x;
	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].y = enemies[enemyTypeNumber][enemyNumber].y;

	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].absX = enemies[enemyTypeNumber][enemyNumber].absX;
	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].absY = enemies[enemyTypeNumber][enemyNumber].absY;

	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].distanceTraveled = 0;
	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].radius = 0;

	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].active = TRUE;

	if (enemyTypeNumber == BLUEGY)
		newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].shotDirection = UP;
	else
		newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].shotDirection = rand() % DIRECTIONS_NUMBER;

	if ((int)(time.worldTime) % 10 < 9)
		newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].trigonometricalMultiplier = 2;
	else
		newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].trigonometricalMultiplier = 10;

	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].timeSinceLastMove = 0;
	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].timeSinceLastTurn = 0;
	newBullets[enemies[enemyTypeNumber][enemyNumber].bulletsNumber - 1].oneDirectionTime = BLUEGY_INITIAL_ONE_DIRECTION_TIME;

	free(enemies[enemyTypeNumber][enemyNumber].bullets);
	enemies[enemyTypeNumber][enemyNumber].bullets = newBullets;
}

void DestroyBullets(EnemiesListData enemiesList, PlayerData* player, int level) {
	for (int enemyNumber = 0; enemyNumber < enemiesList.enemiesNumber[level]; enemyNumber++) {
		int inactiveBulletsNumber = 0;

		for (int bulletNumber = 0; bulletNumber < enemiesList.enemies[level][enemyNumber].bulletsNumber; bulletNumber++) {
			if (!enemiesList.enemies[level][enemyNumber].bullets[bulletNumber].active)
				inactiveBulletsNumber++;
		}

		BulletData* newBullets = (BulletData*)malloc((enemiesList.enemies[level][enemyNumber].bulletsNumber - inactiveBulletsNumber) * sizeof(BulletData));

		int bulletNewNumber = 0;

		for (int bulletNumber = 0; bulletNumber < enemiesList.enemies[level][enemyNumber].bulletsNumber; bulletNumber++) {
			if (enemiesList.enemies[level][enemyNumber].bullets[bulletNumber].active) {
				newBullets[bulletNewNumber] = enemiesList.enemies[level][enemyNumber].bullets[bulletNumber];
				bulletNewNumber++;
			}
		}

		free(enemiesList.enemies[level][enemyNumber].bullets);
		enemiesList.enemies[level][enemyNumber].bullets = newBullets;
		enemiesList.enemies[level][enemyNumber].bulletsNumber -= inactiveBulletsNumber;
	}

	DestroyPlayerBullets(player);
}

void DestroyPlayerBullets(PlayerData* player) {
	int inactiveBulletsNumber = 0;

	for (int bulletNumber = 0; bulletNumber < player->bulletsNumber; bulletNumber++) {
		if (!player->bullets[bulletNumber].active)
			inactiveBulletsNumber++;
	}

	BulletData* newBullets = (BulletData*)malloc((player->bulletsNumber - inactiveBulletsNumber) * sizeof(BulletData));

	int bulletNewNumber = 0;

	for (int bulletNumber = 0; bulletNumber < player->bulletsNumber; bulletNumber++) {
		if (player->bullets[bulletNumber].active) {
			newBullets[bulletNewNumber] = player->bullets[bulletNumber];
			bulletNewNumber++;
		}
	}

	free(player->bullets);
	player->bullets = newBullets;
	player->bulletsNumber -= inactiveBulletsNumber;
}

void DestroyEnemies(EnemiesListData* enemiesList, int level) {
	int enemiesToDestroyNumber = 0;

	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[level]; enemyNumber++) {
		if (!enemiesList->enemies[level][enemyNumber].active && enemiesList->enemies[level][enemyNumber].bulletsNumber == 0 && enemiesList->enemies[level][enemyNumber].animatingTime >= ENEMY_DESTRUCTION_SPRITES_NUMBER * ONE_FRAME_TIME) {
			enemiesToDestroyNumber++;
		}
	}

	EnemyData* newEnemies = (EnemyData*)malloc((enemiesList->enemiesNumber[level] - enemiesToDestroyNumber) * sizeof(EnemyData));

	int enemyNewNumber = 0;

	for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[level]; enemyNumber++) {
		if (enemiesList->enemies[level][enemyNumber].active || enemiesList->enemies[level][enemyNumber].bulletsNumber > 0 || enemiesList->enemies[level][enemyNumber].animatingTime < ENEMY_DESTRUCTION_SPRITES_NUMBER * ONE_FRAME_TIME) {
			newEnemies[enemyNewNumber] = enemiesList->enemies[level][enemyNumber];
			enemyNewNumber++;
		}
		else {
			free(enemiesList->enemies[level][enemyNumber].bullets);
		}
	}

	free(enemiesList->enemies[level]);
	enemiesList->enemies[level] = newEnemies;
	enemiesList->enemiesNumber[level] -= enemiesToDestroyNumber;
}

void DestroyAllObjects(EnemiesListData* enemiesList, PlayerData* player, BonusesListData* bonusesList) {
	for (int levelNumber = LEVEL1; levelNumber < LEVELS_NUMBER; levelNumber++) {
		if (player->isLevelCreated[levelNumber]) {
			for (int bulletNumber = 0; bulletNumber < player->bulletsNumber; bulletNumber++) {
				player->bullets[bulletNumber].active = FALSE;
			}

			for (int enemyNumber = 0; enemyNumber < enemiesList->enemiesNumber[levelNumber]; enemyNumber++) {
				for (int bulletNumber = 0; bulletNumber < enemiesList->enemies[levelNumber][enemyNumber].bulletsNumber; bulletNumber++) {
					enemiesList->enemies[levelNumber][enemyNumber].bullets[bulletNumber].active = FALSE;
				}

				DestroyBullets(*enemiesList, player, levelNumber);

				enemiesList->enemies[levelNumber][enemyNumber].active = 0;
				enemiesList->enemies[levelNumber][enemyNumber].animatingTime = (ENEMY_DESTRUCTION_SPRITES_NUMBER + 1) * ONE_FRAME_TIME;
			}

			DestroyEnemies(enemiesList, levelNumber);

			for (int bonusNumber = 0; bonusNumber < bonusesList->bonusesNumber; bonusNumber++) {
				bonusesList->bonuses[bonusNumber].active = FALSE;
			}
			bonusesList->timeSinceLastBonus = TEST_INITIAL_VALUE;

			UpdateBonuses(bonusesList, *player);


			player->isLevelCreated[levelNumber] = FALSE;
		}
	}
}

void SetMovementAddition(char direction, int* x_addition, int* y_addition, int jump) {
	switch (direction) {
	case UP:
		*x_addition = 0;
		*y_addition = -jump;
		break;
	case RIGHT:
		*x_addition = jump;
		*y_addition = 0;
		break;
	case DOWN:
		*x_addition = 0;
		*y_addition = jump;
		break;
	case LEFT:
		*x_addition = -jump;
		*y_addition = 0;
		break;
	}
}

void UpdateBonuses(BonusesListData* bonusesList, PlayerData player) {
	int inactiveBonusesNumber = 0;

	for (int bonusNumber = 0; bonusNumber < bonusesList->bonusesNumber; bonusNumber++) {
		if (!bonusesList->bonuses[bonusNumber].active)
			inactiveBonusesNumber++;
	}

	BonusData* newBonuses = (BonusData*)malloc((bonusesList->bonusesNumber - inactiveBonusesNumber) * sizeof(BonusData));

	int bonusNewNumber = 0;

	for (int bonusNumber = 0; bonusNumber < bonusesList->bonusesNumber; bonusNumber++) {
		if (bonusesList->bonuses[bonusNumber].active) {
			newBonuses[bonusNewNumber] = bonusesList->bonuses[bonusNumber];
			bonusNewNumber++;
		}
	}

	free(bonusesList->bonuses);
	bonusesList->bonuses = newBonuses;
	bonusesList->bonusesNumber -= inactiveBonusesNumber;
	
	if (bonusesList->timeSinceLastBonus < BONUS_DELAY)
		return;

	bonusesList->bonusesNumber++;
	newBonuses = (BonusData*)malloc(bonusesList->bonusesNumber * sizeof(BonusData));

	for (int bonusNumber = 0; bonusNumber < bonusesList->bonusesNumber - 1; bonusNumber++) {
		newBonuses[bonusNumber] = bonusesList->bonuses[bonusNumber];
	}

	newBonuses[bonusesList->bonusesNumber - 1].active = TRUE;
	newBonuses[bonusesList->bonusesNumber - 1].x = SCREEN_WIDTH / 2 - LEVEL_WIDTH / 2 + BONUS_WIDTH / 2 + (rand() % (LEVEL_WIDTH - BONUS_WIDTH)) - SCREEN_WIDTH / 2 + player.x;
	newBonuses[bonusesList->bonusesNumber - 1].y = SCREEN_HEIGHT / 2 - LEVEL_HEIGHT / 2 + BONUS_HEIGHT / 2 + (rand() % (LEVEL_HEIGHT - BONUS_HEIGHT)) - SCREEN_HEIGHT / 2 + player.y;

	free(bonusesList->bonuses);
	bonusesList->bonuses = newBonuses;
	bonusesList->timeSinceLastBonus = 0;

}

void UpdateScore(PlayerData* player, TimeData time, QuitingData* quiting) {
	player->score = (player->killingPoints + player->life) * time.worldTime * TIME_SCORE_IMPORTANCE;
	
	if (player->life == 0)
		player->gameState = GAMEOVER;

	if (player->killingPoints >= (player->level + 1) * NEXT_LEVEL_LIMIT) {
		if (player->level < 2) {
			player->level++;
			player->newLevel = TRUE;
		}
	}

	if (player->score >= (player->grade + 1)*(player->grade + 1) * GRADE_CHANGE_LIMIT)
		if (player->grade < GRADES_NUMBER - 1) {
			player->grade++;
			player->gradeChangeTime = GRADE_ANIMATION_FRAMES_NUMBER * ONE_FRAME_TIME;
		}
}


void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	}
}

void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
}

void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	}
}

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++) {
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	}
}


void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
}


void FreeAll(SDL_setData* SDL_set) {
	if (SDL_set->screen != NULL)
		SDL_FreeSurface(SDL_set->screen);
	if (SDL_set->charset != NULL)
		SDL_FreeSurface(SDL_set->charset);
	if (SDL_set->charsetDark != NULL)
		SDL_FreeSurface(SDL_set->charsetDark);

	for (int backgroundNumber = LEVEL1; backgroundNumber < LEVELS_NUMBER; backgroundNumber++) {
		if((SDL_set->backgrounds[backgroundNumber] != NULL))
			SDL_FreeSurface(SDL_set->backgrounds[backgroundNumber]);
	}
	if (SDL_set->infoBackground != NULL) {
		SDL_FreeSurface(SDL_set->infoBackground);
	}
	for (int menuBackgroundNumber = MENU_LEVEL1; menuBackgroundNumber < MENU_OPTIONS_NUMBER; menuBackgroundNumber++) {
		if ((SDL_set->menuBackgrounds[menuBackgroundNumber] != NULL))
			SDL_FreeSurface(SDL_set->menuBackgrounds[menuBackgroundNumber]);
	}
	for (int gameoverViewNumber = GAMEOVER_RETRY; gameoverViewNumber < GAMEOVER_VIEWS_NUMBER; gameoverViewNumber++) {
		if ((SDL_set->gameoverViews[gameoverViewNumber] != NULL))
			SDL_FreeSurface(SDL_set->gameoverViews[gameoverViewNumber]);
	}

	for (int gradeNumber = 0; gradeNumber < GRADES_NUMBER; gradeNumber++) {
		if ((SDL_set->grades[gradeNumber] != NULL))
			SDL_FreeSurface(SDL_set->grades[gradeNumber]);
	}
	if (SDL_set->gradeBackground != NULL) {
		SDL_FreeSurface(SDL_set->gradeBackground);
	}
	for (int gradeAnimationNumber = BIG_STAR; gradeAnimationNumber < GRADE_ANIMATION_TEXTURES_NUMBER; gradeAnimationNumber++) {
		if ((SDL_set->gradeAnimationTexture[gradeAnimationNumber] != NULL))
			SDL_FreeSurface(SDL_set->gradeAnimationTexture[gradeAnimationNumber]);
	}

	if(SDL_set->scrtex != NULL)
		SDL_DestroyTexture(SDL_set->scrtex);
	if(SDL_set->window != NULL)
		SDL_DestroyWindow(SDL_set->window);
	if(SDL_set->renderer != NULL)
		SDL_DestroyRenderer(SDL_set->renderer);

	SDL_Quit();
}