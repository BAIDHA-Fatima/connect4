#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define PLUS_INFINITY 1000000
#define MINUS_INFINITY -1000000
#include <windows.h>


int Z;
// structure to hold a state of the game
typedef struct
{
    int T[6][7]; // the game board
    int valminmax; // the minimax value of the state
    int i, j; // the last move made to reach this state
} etat;

// linked list to hold the successor states of a given state
typedef struct element
{
    etat *state; // a pointer to a state
    struct element *next; // a pointer to the next element
} element;

typedef struct
{
    element *head;
    element *tail;
} liste;

// function to insert a state in a linked list
void inserer(etat *e, liste *l)
{
    element *elem = (element*) malloc(sizeof(element));
    elem->state = e;
    elem->next = NULL;
    if (l->head == NULL)
    {
        l->head = l->tail = elem;
    }
    else
    {
        l->tail->next = elem;
        l->tail = elem;
    }
}
void afficherListe(liste *l)
{
    element *e = l->head;
    while (e != NULL)
    {
        printf("Etat:\n");
        printf("  valminmax = %d\n", e->state->valminmax);
        printf("  i = %d, j = %d\n", e->state->i, e->state->j);
        printf("  T = {\n");
        for (int i = 0; i < 6; i++)
        {
            printf("    {");
            for (int j = 0; j < 7; j++)
            {
                printf("%d", e->state->T[i][j]);
                if (j < 6)
                {
                    printf(", ");
                }
            }
            printf("}\n");
        }
        printf("  }\n");
        e = e->next;
    }
}

// function to generate the successor states of a given state
void generersucc(etat e, liste *l, int player)
{
    for (int j = 0; j < 7; j++)
    {
        int i = emptyPlace(j, e.T);
        if (i != -1)
        {
            e.T[i][j] = player;
            etat* newstate = (etat*) malloc(sizeof(etat));
            copietab(newstate->T, e.T);
            newstate->i = i;
            newstate->j = j;
            newstate->valminmax = 0;

            inserer(newstate, l);
            e.T[i][j] = 0;
        }
    }
}

// function to copy a game board from one array to another
void copietab(int dest[6][7], int src[6][7])
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            dest[i][j] = src[i][j];
        }
    }
}

// function to check if a player has won the game
int isWin(int board[6][7], int player)
{
    // check for horizontal wins
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == player && board[i][j+3] == player)
            {
                return 1;
            }
        }
    }
    // check for vertical wins
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (board[i][j] == player && board[i+1][j] == player && board[i+2][j] == player && board[i+3][j] == player)
            {
                return 1;
            }
        }
    }
    // check for diagonal wins (top left to bottom right)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] == player && board[i+1][j+1] == player && board[i+2][j+2] == player && board[i+3][j+3] == player)
            {
                return 1;
            }
        }
    }

// check for diagonal wins (bottom left to top right)
    for (int i = 3; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (board[i][j] == player && board[i-1][j+1] == player && board[i-2][j+2] == player && board[i-3][j+3] == player)
            {
                return 1;
            }
        }
    }

// no win
    return 0;
}
int isFull(int board[6][7])
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (board[i][j] == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}
int emptyPlace(int j,int T[6][7])
{
    int i;
    if(T[0][j]!=0) return -1; // return -1 when the column is FULL
    for(i=0; i<5; i++)
    {
        if(T[i+1][j]!=0) // this means when i+1 is FULL, we break and return i (i is above i+1)
            break;
    }
    return i;
}





int eval_weights[6][7] =
{
    {3, 4, 5, 7, 5, 4, 3},
    {4, 6, 8, 10, 8, 6, 4},
    {5, 8, 9, 13, 11, 8, 5},
    {5, 8, 11, 13, 11, 8, 5},
    {4, 6, 8, 10, 8, 6, 4},
    {3, 4, 5, 7, 5, 4, 3}
};


int heur(int tab[6][7],int player)
{
    int opplayer=3-player;
    int h=0;
    for(int i=0; i<6; i++)
    {
        for (int j=0; j<7; j++)
        {
            if(tab[i][j]==player) h+=eval_weights[i][j];
            if(tab[i][j]==opplayer) h-=eval_weights[i][j];
        }
    }
    return h;
}
int j=0;
bool compareTab(int tab1[6][7], int tab2[6][7])
{

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (tab1[i][j] != tab2[i][j])
            {
                return false;
            }
        }
    }
    return true;
}






int minimax(etat *e, int depth, int player, int alpha, int beta)
{
    // base case: maximum depth or game over
    if (depth == 0 || isFull(e->T) || isWin(e->T, 1) || isWin(e->T, 2))
    {
        return heur(e->T, player);
    }
    // generate the successor states
    liste *successors = (liste*) malloc(sizeof(liste));
    successors->head = successors->tail = NULL;
    generersucc(*e, successors, player);
    // apply minimax to each successor state with alpha-beta pruning
    int minmaxval = (player == 1) ? MINUS_INFINITY : PLUS_INFINITY;
    for (element *elem = successors->head; elem != NULL; elem = elem->next)
    {
        int val = minimax(elem->state, depth-1, 3-player, alpha, beta);

        elem->state->valminmax = val;
        if (player == 1)
        {
            if (val > minmaxval)
            {
                minmaxval = val;
            }
            alpha = (val > alpha) ? val : alpha;
            if (beta < alpha)
            {
                break; // alpha-beta pruning
            }
        }
        else
        {
            if (val < minmaxval)
            {
                minmaxval = val;
            }
            beta = (val < beta) ? val : beta;
            if (beta <alpha)
            {
                break; // alpha-beta pruning
            }
        }
    }

    decisionminimax(e, successors, player, minmaxval);

    // free memory
    for (element *elem = successors->head; elem != NULL; elem = elem->next)
    {
        free(elem->state);
    }
    free(successors);

    // set the minimax value of the state
    e->valminmax = minmaxval;
    return minmaxval;
}

void decisionminimax(etat *e,liste*successors,int player,int val)
{

    liste *suc = (liste*) malloc(sizeof(liste));
    suc->head = suc->tail = NULL;
    generersucc(*e, suc,player);
    for (element *elem = suc->head;
            elem != NULL; elem = elem->next)
    {
        for (element *elem1 = successors->head;
                elem1 != NULL; elem1 = elem1->next)
        {
            if(compareTab(elem->state->T,elem1->state->T)
                    && elem1->state->valminmax==val)
                Z=elem1->state->j;


        }

    }

}

//________________________________________________________________________________________________
int x,q;
// Detect the click location and return the clicked column.
int handleEvents(SDL_Event* e, int winWidth, int* mouseX)
{
    while (SDL_PollEvent(e) != 0)
    {
        // Quit event
        if (e->type == SDL_QUIT)
        {
            return -1;
        }

        // Mouse click event
        if (e->type == SDL_MOUSEBUTTONDOWN)
        {
            if (e->button.button == SDL_BUTTON_LEFT)
            {
                *mouseX = e->button.x;
                if (*mouseX <401 && *mouseX>339)
                {

                    return 0;
                }
                if (*mouseX <450 && *mouseX>401)
                {

                    return 1;
                }
                if (*mouseX <503 && *mouseX>450)
                {

                    return 2 ;
                }
                if (*mouseX <556 && *mouseX>503)
                {

                    return 3;
                }
                if (*mouseX < 605&& *mouseX>556)
                {

                    return 4;
                }
                if (*mouseX <665&& *mouseX>605)
                {

                    return 5;
                }
                if (*mouseX < 722&& *mouseX>665)
                {

                    return 6;
                }
            }
        }
    }
    return -2;
}
int centrecolonneai(int z)
{


    if (z==0)
        return 350;

    if (z==1)
        return 402;

    if (z==2)
        return 455;

    if (z==3)
        return 510;

    if (z==4)
        return 562;

    if (z==5)
        return 615;

    if (z==6)
        return 665;

}
int makeMove(int board[6][7], int col, int player)
{

// check if the column is not full
    if (board[0][col] != 0)
    {
        return -1;
    }
// find the lowest empty slot in the column
    int row = 5;
    while (board[row][col] != 0)
    {
        row--;
    }

// make the move
    board[row][col] = player;

    return row;
}

int centreligne(int i)
{
    if (i==0) return 183;
    if (i==1) return 245;
    if (i==2) return 308 ;
    if (i==3) return 370;
    if (i==4) return 432;
    if (i==5) return 495;
}

bool voiceEffectFinished = false;
void voiceEffectFinishedCallback(int channel)
{
    voiceEffectFinished = true;
}

// Function to display the menu
void afficherMenu()
{
    printf("----- Menu -----\n");
    printf("1. Rules\n");
    printf("2. Play\n");
    printf("3. Exit\n");
    printf("Choisissez une option : ");
}



int main(int argc, char *argv[])
{
debut:
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Affichage d'images", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,  1001, 580, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Surface* menuSurface = IMG_Load("menu.jpeg");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
    SDL_FreeSurface(menuSurface);

    SDL_Surface* playeSurface = IMG_Load("play.jpeg");
    SDL_Texture* playeTexture = SDL_CreateTextureFromSurface(renderer, playeSurface);
    SDL_FreeSurface(playeSurface);

    SDL_Surface* ruleSurface = IMG_Load("rules.jpeg");
    SDL_Texture* ruleTexture = SDL_CreateTextureFromSurface(renderer, ruleSurface);
    SDL_FreeSurface(ruleSurface);

    SDL_Surface* exittSurface = IMG_Load("exit.jpeg");
    SDL_Texture* exittTexture = SDL_CreateTextureFromSurface(renderer, exittSurface);
    SDL_FreeSurface(exittSurface);

    SDL_Surface* accSurface = IMG_Load("acceuil.jpeg");
    SDL_Texture* accTexture = SDL_CreateTextureFromSurface(renderer, accSurface);
    SDL_FreeSurface(accSurface);

    SDL_Surface* allSurface = IMG_Load("all_rules.jpeg");
    SDL_Texture* allTexture = SDL_CreateTextureFromSurface(renderer, allSurface);
    SDL_FreeSurface(allSurface);

    // Position and dimensions of the image  menu
    SDL_Rect menuRect;
    menuRect.x = 0;
    menuRect.y = 0;
    menuRect.w = 1002;
    menuRect.h = 580;

    // Position and dimensions of the image to be displayed when clicked.
    SDL_Rect playeRect;
    playeRect.x = 0;
    playeRect.y = 0;
    playeRect.w = 1002;
    playeRect.h = 580;

    SDL_Rect ruleRect;
    ruleRect.x = 0;
    ruleRect.y = 0;
    ruleRect.w = 1002;
    ruleRect.h = 580;

    SDL_Rect exittRect;
    exittRect.x = 0;
    exittRect.y = 0;
    exittRect.w = 1002;
    exittRect.h = 580;

    SDL_Rect accRect;
    accRect.x = 0;
    accRect.y = 0;
    accRect.w = 1002;
    accRect.h = 580;

    SDL_Rect allRect;
    allRect.x = 0;
    allRect.y = 0;
    allRect.w = 1002;
    allRect.h = 580;

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;
    int choice = 0;

    while (!quit)
    {
        SDL_RenderClear(renderer);
        // display menu
        SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect);

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = SDL_TRUE;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = event.button.x;
                    int y = event.button.y;
                    // Checking the click position
                    if (x >= 396 && x <= 637 && y >= 174 && y <= 288)
                    {
                        SDL_RenderCopy(renderer, playeTexture, NULL, &playeRect);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(200); // wait for 200ms
                        choice = 1;  // Play
                        quit = SDL_TRUE;
                    }
                    // Checking the click position
                    else if (x >= 394 && x <= 635 && y >= 312 && y <= 425)
                    {
                        SDL_RenderCopy(renderer, ruleTexture, NULL, &ruleRect);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(200);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(700);

                        // display the image  all_rules
                        SDL_RenderCopy(renderer, allTexture, NULL, &allRect);
                        SDL_RenderPresent(renderer);
                        SDL_Event event;
                        SDL_bool wait_for_click = SDL_TRUE;

                        while (wait_for_click)
                        {
                            while (SDL_PollEvent(&event))
                            {
                                switch (event.type)
                                {
                                case SDL_QUIT:
                                    choice = 3; // Exit
                                    quit = SDL_TRUE;
                                    wait_for_click = SDL_FALSE;
                                    break;
                                case SDL_MOUSEBUTTONDOWN:
                                    choice = 2; // New value for choice
                                    quit = SDL_TRUE;
                                    wait_for_click = SDL_FALSE;
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                    }
                    // Checking the click position
                    else if (x >= 390 && x <= 635 && y >= 453 && y <= 562)
                    {
                        SDL_RenderCopy(renderer, exittTexture, NULL, &exittRect);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(600); //wait for 600ms
                        choice = 3;  // Exit
                        quit = SDL_TRUE;
                    }
                }
                break;
            default:
                break;
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(playeTexture);
    SDL_DestroyTexture(ruleTexture);
    SDL_DestroyTexture(exittTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    // Handle the user's choice

    switch (choice)
    {
    case 1:
        printf("Démarrage du jeu...\n");

        int player = 1; // player 1 starts
        int depth = 7; // depth of the search tree
        int col; // the column where the player makes a move
        etat currentstate; // the current state of the game
        currentstate.valminmax = 0;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 7; j++)
            {
                currentstate.T[i][j] = 0; // initialize the game board to all zeros
            }
        }

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("Failed to initialize SDL: %s\n", SDL_GetError());
            return 1;
        }
// Load image using SDL_image library
        SDL_Surface* image = IMG_Load("game.png");
        if (!image)
        {
            printf("Failed to load image: %s\n", IMG_GetError());
            return 1;
        }
// Create window
        SDL_Window* window = SDL_CreateWindow("Connect 4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1001, 580, SDL_WINDOW_SHOWN);
        if (!window)
        {
            printf("Failed to create window: %s\n", SDL_GetError());
            return 1;
        }

        SDL_Renderer* renderer = SDL_CreateRenderer(window, -5, 0);
        if (!renderer)
        {
            printf("Failed to create renderer: %s\n", SDL_GetError());
            return 1;
        }

        // Create texture from image
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
        SDL_FreeSurface(image);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Render texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Update screen
        SDL_RenderPresent(renderer);

        // Initialize SDL_mixer
        Mix_Init(MIX_INIT_MP3);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        // Load voice sound effect
        Mix_Chunk* voiceEffect = Mix_LoadWAV("audio.wav");
        Mix_Chunk* voiceEffect1 = Mix_LoadWAV("win.wav");
        Mix_Chunk* voiceEffect2 = Mix_LoadWAV("lose.wav");
        Mix_Chunk* voiceEffect3 = Mix_LoadWAV("equa.wav");
        if (!voiceEffect)
        {
            printf("Failed to load voice effect: %s\n", Mix_GetError());
            return 1;
        }
        // Game loop
        while (!isFull(currentstate.T) && !isWin(currentstate.T, 1) && !isWin(currentstate.T, 2))
        {
            // Wait for window close event

            if (player == 1)
            {
                // Player 1's turn
                printf("Player 1's turn\n");

                SDL_Event event;
                int mouseX = 0;
                int colNum = -1;
                bool quit = false;

                // Wait for left mouse button click event and get column number
                while (!quit && colNum == -1  )
                {
                    int eventResult = handleEvents(&event, SDL_QUIT, &mouseX);

                    if (eventResult == -1)
                    {
                        quit = true;
                    }
                    else if (eventResult != -2)
                    {
                        colNum = eventResult;
                    }
                }

                if (quit)
                {
                    break;
                }

                // Make move and render piece image
                int p = makeMove(currentstate.T, colNum, 1);

                int y = centreligne(p);
                int x = centrecolonneai(colNum);

                SDL_Surface* image1 = IMG_Load("pink.png");
                if (!image1)
                {
                    printf("Failed to load image: %s\n", IMG_GetError());
                    return 1;
                }

                SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, image1);
                SDL_FreeSurface(image1);

                SDL_Rect destRect;
                destRect.x = x;
                destRect.y = y;
                destRect.w = image1->w;
                destRect.h = image1->h;

                SDL_RenderCopy(renderer, texture1, NULL, &destRect);
                // Play voice sound effect
                Mix_PlayChannel(-1, voiceEffect, 0);
                SDL_RenderPresent(renderer);

                SDL_DestroyTexture(texture1);

            }
            else
            {
                // AI's turn
                printf("AI's turn\n");

                // Calculate best move using minimax algorithm
                int bestMove = minimax(&currentstate, 7, 2, MINUS_INFINITY, PLUS_INFINITY);
                int p = makeMove(currentstate.T, Z, 2);
                int Y = centreligne(p);
                int X = centrecolonneai(Z);
                printf("z=%d,p=%d",Z,p);
                // Load AI image
                SDL_Surface* image3 = IMG_Load("bleu.png");
                if (!image3)
                {
                    printf("Failed to load image: %s\n", IMG_GetError());
                    return 1;
                }

                // Create texture from AI image
                SDL_Texture* texture3 = SDL_CreateTextureFromSurface(renderer, image3);
                SDL_FreeSurface(image3);

                // Set position and size of AI image
                SDL_Rect destRect;
                destRect.x = X;
                destRect.y = Y;
                destRect.w = image3->w;
                destRect.h = image3->h;

                // Render AI image
                SDL_RenderCopy(renderer, texture3, NULL, &destRect);
                // Play voice sound effect
                Mix_PlayChannel(-1, voiceEffect, 0);
                // Update screen
                SDL_RenderPresent(renderer);

                // Cleanup
                SDL_DestroyTexture(texture3);

                // Switch to player 1's turn

            }
            player = 3-player;
            SDL_Delay(500);
        }

        SDL_Delay(500);


        if (isWin(currentstate.T, 1))
        {
            SDL_Surface* image4 = IMG_Load("win.png");
            SDL_Texture* texture4 = SDL_CreateTextureFromSurface(renderer, image4);

            SDL_FreeSurface(image4);
            SDL_RenderCopy(renderer, texture4, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture4);

            // Play sound asynchronously
            Mix_PlayChannel(-1, voiceEffect1, 0);
        }

        if (isWin(currentstate.T, 1))
        {
            SDL_Surface* image4 = IMG_Load("win.png");
            SDL_Texture* texture4 = SDL_CreateTextureFromSurface(renderer, image4);

            SDL_FreeSurface(image4);
            SDL_RenderCopy(renderer, texture4, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture4);

            // Play sound asynchronously
            Mix_PlayChannel(-1, voiceEffect1, 0);
            SDL_Event event;
            bool voiceEffectFinished = false;
            while (!voiceEffectFinished)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == MIX_CHANNEL_POST)
                    {
                        voiceEffectFinished = true;
                        break;
                    }
                }
                SDL_Delay(100);
            }
// Clean up SDL mixer resources
            Mix_FreeChunk(voiceEffect1);
            Mix_CloseAudio();
            Mix_Quit();


        }
        else if (isWin(currentstate.T, 2))
        {
            SDL_Surface* image5 = IMG_Load("lose.png");
            SDL_Texture* texture5 = SDL_CreateTextureFromSurface(renderer, image5);

            SDL_FreeSurface(image5);
            SDL_RenderCopy(renderer, texture5, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture5);

            // Play sound asynchronously
            Mix_PlayChannel(-1, voiceEffect2, 0);
            // Wait for the voice effect to finish playing
            SDL_Event event;
            bool voiceEffectFinished = false;
            while (!voiceEffectFinished)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == MIX_CHANNEL_POST)
                    {
                        voiceEffectFinished = true;
                        break;
                    }
                }
                SDL_Delay(100);
            }


// Clean up SDL mixer resources
            Mix_FreeChunk(voiceEffect1);
            Mix_CloseAudio();
            Mix_Quit();


        }
        else if (isFull(currentstate.T))
        {
            SDL_Surface* image6 = IMG_Load("tie.png");
            SDL_Texture* texture6 = SDL_CreateTextureFromSurface(renderer, image6);

            SDL_FreeSurface(image6);
            SDL_RenderCopy(renderer, texture6, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture6);
            Mix_PlayChannel(-1, voiceEffect3, 0);
            SDL_Event event;
            bool voiceEffectFinished = false;
            while (!voiceEffectFinished)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == MIX_CHANNEL_POST)
                    {
                        voiceEffectFinished = true;
                        break;
                    }
                }
                SDL_Delay(100);
            }


// Clean up SDL mixer resources
            Mix_FreeChunk(voiceEffect1);
            Mix_CloseAudio();
            Mix_Quit();
        }

// Delay to allow time for the sound to play
        SDL_Delay(500);



// Cleanup
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        break;
    case 2:
        printf("voila les regles du jeu");
        // Chargement de l'image


        goto debut;


        break;
    case 3:
        printf("Au revoir !\n");
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("Failed to initialize SDL: %s\n", SDL_GetError());
            return 1;
        }
// Load image using SDL_image library
        SDL_Surface* image7 = IMG_Load("acceuil.png");
        if (!image7)
        {
            printf("Failed to load image: %s\n", IMG_GetError());
            return 1;
        }
        // Create window
        SDL_Window* window1 = SDL_CreateWindow("Connect 4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1001, 580, SDL_WINDOW_SHOWN);
        if (!window)
        {
            printf("Failed to create window: %s\n", SDL_GetError());
            return 1;
        }

        SDL_Renderer* renderer7 = SDL_CreateRenderer(window1, -5, 0);
        if (!renderer7)
        {
            printf("Failed to create renderer: %s\n", SDL_GetError());
            return 1;
        }

        // Create texture from image
        SDL_Texture* texture7 = SDL_CreateTextureFromSurface(renderer7, image7);
        SDL_FreeSurface(image7);

        // Clear screen
        SDL_SetRenderDrawColor(renderer7, 255, 255, 255, 255);
        SDL_RenderClear(renderer7);

        // Render texture
        SDL_RenderCopy(renderer7, texture7, NULL, NULL);

        // Update screen
        SDL_RenderPresent(renderer7);
        SDL_Delay(800);

        break;
    default:
        printf("Option invalide. Veuillez choisir une option valide.\n");
        break;
    }

    return 0;
}
