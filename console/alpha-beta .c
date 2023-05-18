#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define PLUS_INFINITY 1000000
#define MINUS_INFINITY -1000000
#include <windows.h>
int Z;
int nbreNoeuds=0;
void Color(int couleurDuTexte,int couleurDeFond) // fonction d'affichage de couleurs
{
        HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(H,couleurDeFond*16+couleurDuTexte);

}
// structure to hold a state of the game
typedef struct {
    int T[6][7]; // the game board
    int valminmax; // the minimax value of the state
    int i, j; // the last move made to reach this state
} etat;

// linked list to hold the successor states of a given state
typedef struct element {
    etat *state; // a pointer to a state
    struct element *next; // a pointer to the next element
} element;

typedef struct {
    element *head;
    element *tail;
} liste;

// function to insert a state in a linked list
void inserer(etat *e, liste *l) {
    element *elem = (element*) malloc(sizeof(element));
    elem->state = e;
    elem->next = NULL;
    if (l->head == NULL) {
        l->head = l->tail = elem;
    } else {
        l->tail->next = elem;
        l->tail = elem;
    }
}
void afficherListe(liste *l) {
    element *e = l->head;
    while (e != NULL) {
        printf("Etat:\n");
        printf("  valminmax = %d\n", e->state->valminmax);
        printf("  i = %d, j = %d\n", e->state->i, e->state->j);
        printf("  T = {\n");
        for (int i = 0; i < 6; i++) {
            printf("    {");
            for (int j = 0; j < 7; j++) {
                printf("%d", e->state->T[i][j]);
                if (j < 6) {
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
void generersucc(etat e, liste *l, int player) {
    for (int j = 0; j < 7; j++) {
        int i = emptyPlace(j, e.T);
        if (i != -1) {
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
void copietab(int dest[6][7], int src[6][7]) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

// function to check if a player has won the game
int isWin(int board[6][7], int player) {
    // check for horizontal wins
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == player && board[i][j+3] == player) {
                return 1;
            }
        }
    }
    // check for vertical wins
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j] == player && board[i+1][j] == player && board[i+2][j] == player && board[i+3][j] == player) {
                return 1;
            }
        }
    }
    // check for diagonal wins (top left to bottom right)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == player && board[i+1][j+1] == player && board[i+2][j+2] == player && board[i+3][j+3] == player) {
return 1;
}
}
}

// check for diagonal wins (bottom left to top right)
for (int i = 3; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
        if (board[i][j] == player && board[i-1][j+1] == player && board[i-2][j+2] == player && board[i-3][j+3] == player) {
            return 1;
        }
    }
}

// no win
return 0;
}
 int isFull(int board[6][7]) {
      for (int i = 0; i < 6; i++) {
         for (int j = 0; j < 7; j++) {
            if (board[i][j] == 0) {
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
    for(i=0;i<5;i++){
        if(T[i+1][j]!=0) // this means when i+1 is FULL, we break and return i (i is above i+1)
            break;}
    return i;
}

int makeMove(int board[6][7], int col, int player) {
// check if the column is valid
if (col < 0 || col > 6) {
return 0;
}
// check if the column is not full
if (board[0][col] != 0) {
return 0;
}
// find the lowest empty slot in the column
int row = 5;
while (board[row][col] != 0) {
row--;
}
// make the move
board[row][col] = player;
return 1;
}


// function to print the board
void printBoard(int board[6][7]) {
for (int i = 0; i < 6; i++) {
for (int j = 0; j < 7; j++) {
if(board[i][j]==1){
    Color(11,11);
    printf("%d ", board[i][j]);
    Color(15,0);
    printf(" ");
}
if(board[i][j]==2){
    Color(18,0);
    printf("%d ", board[i][j]);
    Color(15,0);
     printf(" ");
}
if(board[i][j]==0){
    printf("%d ", board[i][j]);
     printf(" ");

}
}
printf("\n");
printf("\n");
}
printf("\n");
printf("\n");
}

int eval_weights[6][7] = {
    {3, 4, 5, 7, 5, 4, 3},
    {4, 6, 8, 10, 8, 6, 4},
    {5, 8, 11, 13, 11, 8, 5},
    {5, 8, 11, 13, 11, 8, 5},
    {4, 6, 8, 10, 8, 6, 4},
    {3, 4, 5, 7, 5, 4, 3}};


int heur(int tab[6][7],int player){
int opplayer=3-player;
int h=0;
for(int i=0;i<6;i++)
{  for (int j=0;j<7;j++){
       if(tab[i][j]==player) h+=eval_weights[i][j];
       if(tab[i][j]==opplayer) h-=eval_weights[i][j];
   }
}
return h;
}
int j=0;
bool compareTab(int tab1[6][7], int tab2[6][7]) {

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            if (tab1[i][j] != tab2[i][j]) {
                return false;
            }
        }
    }
    return true;
}





int minimax(etat *e, int depth, int player, int alpha, int beta) {
    // base case: maximum depth or game over
    if (depth == 0 || isFull(e->T) || isWin(e->T, 1) || isWin(e->T, 2)) {
        return heur(e->T, player);
    }
    // generate the successor states
    liste *successors = (liste*) malloc(sizeof(liste));
    successors->head = successors->tail = NULL;
    generersucc(*e, successors, player);
    // apply minimax to each successor state with alpha-beta pruning
    int minmaxval = (player == 1) ? MINUS_INFINITY : PLUS_INFINITY;
    for (element *elem = successors->head; elem != NULL; elem = elem->next) {
        int val = minimax(elem->state, depth-1, 3-player, alpha, beta);
        nbreNoeuds++;
        elem->state->valminmax = val;
        if (player == 1) {
            if (val > minmaxval) {
                minmaxval = val;
            }
            alpha = (val > alpha) ? val : alpha;
            if (beta < alpha) {
                break; // alpha-beta pruning
            }
        } else {
            if (val < minmaxval) {
                minmaxval = val;
            }
            beta = (val < beta) ? val : beta;
            if (beta <alpha) {
                break; // alpha-beta pruning
            }
        }
    }

    decisionminimax(e, successors, player, minmaxval);

    // free memory
    for (element *elem = successors->head; elem != NULL; elem = elem->next) {
        free(elem->state);
    }
    free(successors);

    // set the minimax value of the state
    e->valminmax = minmaxval;
    return minmaxval;
}

void decisionminimax(etat *e,liste*successors,int player,int val){

    liste *suc = (liste*) malloc(sizeof(liste));
    suc->head = suc->tail = NULL;
    generersucc(*e, suc,player);
    for (element *elem = suc->head; elem != NULL; elem = elem->next){
         for (element *elem1 = successors->head; elem1 != NULL; elem1 = elem1->next){
            if(compareTab(elem->state->T,elem1->state->T) && elem1->state->valminmax==val)
                Z=elem1->state->j;


         }

    }

}
int main() {
    int player = 1; // player 1 starts
    int depth =7; // depth of the search tree
    int col; // the column where the player makes a move
    etat currentstate; // the current state of the game
    currentstate.valminmax = 0;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            currentstate.T[i][j] = 0; // initialize the game board to all zeros
        }
    }
    while (!isFull(currentstate.T) && !isWin(currentstate.T, 1) && !isWin(currentstate.T, 2)) {
        printf("Player %d's turn\n", player);
        if (player == 1) {
            printf("Enter the column where you want to make a move (0-6): ");
            scanf("%d", &col);
            while (!makeMove(currentstate.T, col, player)) {
                printf("Invalid move. Enter another column (0-6): ");
                scanf("%d", &col);
            }
        } else {

            int bestmove =minimax(&currentstate, depth, 2, MINUS_INFINITY,PLUS_INFINITY);
            makeMove(currentstate.T, Z, player);
            printf("Computer chose column %d\n", Z);
            printf("nnbreNoeuds=%d\n",nbreNoeuds);

        }
        // switch to the other player
        player = 3 - player;
        // print the current state of the game
        printBoard(currentstate.T);
    }
    // print the winner of the game or a tie
    if (isWin(currentstate.T, 1)) {
        printf("Player 1 wins!\n");
    } else if (isWin(currentstate.T, 2)) {
        printf("Player 2 wins!\n");
    } else {
        printf("It's a tie!\n");
    }
    return 0;
}

