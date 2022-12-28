#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  game_state_t *res = malloc(sizeof(game_state_t));
  snake_t *snake = malloc(sizeof(snake_t));
  char **board = malloc(sizeof(char) * 8 * 20);
  if (res == NULL || snake == NULL || board == NULL)
	  return NULL;
  res -> num_rows = 18;
  res -> num_snakes = 1;

  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->live = true;
  res->snakes = snake;

  char *t_board[19] = {
	  "####################\n",
	  "#                  #\n",
	  "# d>D    *         #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "#                  #\n",
	  "####################\n"};

  for (int i = 0; i < 18; i++)
  {
	  board[i] = malloc(sizeof(char) * 22);
	  if (board[i] == NULL)
		  return NULL;
	  strcpy(board[i], t_board[i]);
  }
  res->board = board;
  return res;
}

/* Task 2 */
void free_state(game_state_t* state) {
  free(state->snakes);
  for (int i=0; i<state->num_rows; i++)
  {
	free(state->board[i]);  
  }
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i=0; i<state->num_rows; i++)
  {
	  fprintf(fp, "%s", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  char x[] = "wasd";
  for (int i=0; i<sizeof(x)/sizeof(char); i++)
  {
	 if (x[i] == c) return true;
  }

  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  char x[] = "WASDx";
  for (int i=0; i<sizeof(x)/sizeof(char); i++)
  {
	  if (x[i] == c) return true;
  }

  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  char x[] = "WASDwasd^<>vx";
  for (int i=0; i<sizeof(x)/sizeof(char); i++)
  {
	  if (x[i] == c) return true;
  }

  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  switch(c){
	  case '^': 
		  return 'w'; 
		  break;
	  case 'v':
		  return 's';
		  break;
	  case '<':
		  return 'a';
		  break;
	  case '>':
		  return 'd';
		  break;
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
    switch(c){
    case 'W': 
	    return '^';
	    break;
    case 'A': 
	    return '<';
	    break;
    case 'D': 
	    return '>';
	    break;
    case 'S': 
	    return 'v';
	    break;
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  char x[] = "vsS";
  char y[] = "^wW";
  for (int i=0; i<3; i++){
	  if (x[i] == c) return cur_row +1;
	  if (y[i] == c) return cur_row -1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  char x[] = ">dD";
  char y[] = "<aA";
  for (int i = 0; i < 3; i ++ ){
    if (x[i] == c) return cur_col + 1;
    if (y[i] == c) return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  snake_t snake = state->snakes[snum];
  unsigned int row = snake.head_row, col = snake.head_col;
  char head = get_board_at(state, row, col);
  unsigned int new_row = get_next_row(row, head), new_col = get_next_col(col, head);
  
  return get_board_at(state, new_row, new_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  snake_t *snake = &state->snakes[snum];
  unsigned int row = (*snake).head_row, col = (*snake).head_col;
  char head = get_board_at(state, row, col);
  unsigned int new_row = get_next_row(row, head), new_col = get_next_col(col, head);

  (*snake).head_row = new_row, (*snake).head_col = new_col;

  set_board_at(state, row, col, head_to_body(head));
  set_board_at(state, new_row, new_col, head);
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t *snake = &state->snakes[snum];
  unsigned int r = (*snake).tail_row, c = (*snake).tail_col;
  char tail = get_board_at(state, r, c);
  
  unsigned int nr = get_next_row(r, tail), nc = get_next_col(c, tail);
  char new_tail = body_to_tail(get_board_at(state ,nr , nc));  
  (*snake).tail_row = nr, (*snake).tail_col = nc;

  set_board_at(state, r, c, ' ');
  set_board_at(state, nr, nc, new_tail);
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
    for (unsigned int snum = 0; snum < state->num_snakes; snum ++ ){
    snake_t *snake = &(state->snakes[snum]);
    char ne = next_square(state, snum);

    if (ne == ' '){
      update_head(state, snum);
      update_tail(state, snum);
    }
    else if (ne == '*'){
      update_head(state, snum);
      add_food(state);
    }
    else{
      snake->live = false;
      set_board_at(state, snake->head_row, snake->head_col, 'x');
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  FILE *ptr;
  ptr = fopen(filename, "r");
  if (ptr == NULL) return NULL;

  game_state_t *res = malloc(sizeof(game_state_t));
  if (res == NULL) return NULL;

  char str[100010]; 
  char *tep[100010];

  unsigned int cnt = 0;

  while (fgets(str, 100010, ptr) != NULL){
    char *line = malloc(strlen(str) + 1); 
    if (line == NULL) return NULL;
    strcpy(line, str);

    tep[cnt] = line;
    cnt ++ ;
  }
  
  char **board = malloc(sizeof(char) * 8 * cnt);
  for (int i = 0; i < cnt; i ++ ){
    board[i] = tep[i];
  }

  fclose(ptr);

  res->num_rows = cnt;
  res->board = board;

  return res;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  snake_t *snake = &(state->snakes[snum]);
  unsigned int ansR = snake->tail_row, ansC = snake->tail_col;
  char cur = get_board_at(state, ansR, ansC);

  while(!is_head(cur)){
    ansR = get_next_row(ansR, cur), ansC = get_next_col(ansC, cur);
    cur = get_board_at(state, ansR, ansC);
  }

  snake->head_col = ansC, snake->head_row = ansR;

  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  unsigned int r = state->num_rows, cnt = 0;

  for (int i = 0; i < r; i ++ ){
    for (int j = 0; ; j ++ ){
      char c = state->board[i][j];
      if (c == '\0') break;
      if (is_tail(c)) cnt ++;
    }
  }

  snake_t *snake = malloc(sizeof(snake_t) * cnt);
  state->snakes = snake;
  state->num_snakes = cnt;

  cnt = 0;
  for (unsigned int i = 0; i < r; i ++ ){
    for (unsigned int j = 0; ; j ++ ){
      char c = state->board[i][j];
      if (c == '\0') break;
      if (is_tail(c)){
        snake[cnt].tail_row = i;
        snake[cnt].tail_col = j;
        snake[cnt].live = true;
        cnt ++ ;
      }
    }
  }

  for (unsigned int i = 0; i < cnt; i ++ ){
    find_head(state, i);  // BUG: 把 i 写成 cnt...
  }
  return state;}
