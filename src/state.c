#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  game_state_t* ptr_to_game_state = (game_state_t*)malloc(sizeof(game_state_t));
  if(ptr_to_game_state == NULL){
    printf("Error: malloc failed\n");
  }
  unsigned int num_rows = 18;
  unsigned int num_snakes = 1;
  ptr_to_game_state -> num_snakes = num_snakes;
  ptr_to_game_state -> num_rows = num_rows;

  char** board = (char**)malloc(num_rows * sizeof(char*));
  ptr_to_game_state -> board = (char**)malloc(ptr_to_game_state -> num_rows * sizeof(char*));
  
  for(int i = 0; i < num_rows; i++){
    ptr_to_game_state -> board[i] = (char*)malloc((unsigned int)20 * sizeof(char));
    for(int j = 0; j < 20; j++){
      if(i == 0 || i == num_rows - 1 || j == 0 || j == 19){
        ptr_to_game_state -> board[i][j] = '#';
      }else{
        ptr_to_game_state -> board[i][j] = ' ';
      }
    }
  }
  set_board_at(ptr_to_game_state, 2, 9, '*');
  set_board_at(ptr_to_game_state, 2, 2, 'd');
  set_board_at(ptr_to_game_state, 2, 3, '>');
  set_board_at(ptr_to_game_state, 2, 4, 'D');


  snake_t* snake_array = (snake_t*)malloc(ptr_to_game_state -> num_snakes * sizeof(snake_t));
  snake_t snake_default;
  snake_default.tail_row = 2;
  snake_default.tail_col = 2;
  snake_default.head_row = 2;
  snake_default.head_col = 4;
  snake_default.live     = true;
  snake_array[0]         = snake_default;
  ptr_to_game_state -> snakes = snake_array;
  // strcpy(ptr_to_game_state -> board, board);
  
  return ptr_to_game_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  free(state -> snakes);
  for(int i = 0; i < state -> num_rows; i++){
    free(state -> board[i]);
  }
  free(state -> board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  // fp = fopen("output.txt", "w");
  // fprintf(fp, "%s");
  // for(int i = 0; i < state -> num_rows; i++){
  //   for(int j = 0; j < 20; j++){
  //     fprintf(fp, "%c", state -> board[i][j]);
  //   }
  //   fprintf(fp, "\n");
  //   // fprintf(fp, "%s\n", state -> board[i]);
  // }
  for(int i = 0; i < state -> num_rows; i++){
    fprintf(fp, "%s\n", state -> board[i]);
  }
  return;
}
/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if(c == 'w' || c == 'a' || c == 's' || c == 'd')
    return true;
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')
    return true;
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  bool is_tail_or_head = is_tail(c) || is_head(c);
  bool is_body = (c == '^' || c == '<' || c == 'v' || c == '>');
  return is_tail_or_head || is_body;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  char res = ' ';
  switch(c){
    case '^':
      res = 'w';
      break;

    case '<':
      res = 'a';
      break;

    case 'v':
      res = 's';
      break;

    case '>':
      res = 'd';
      break;

    default:
      break;
  }
  return res;
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  char res = ' ';
  switch(c){
    case 'W':
      res = '^';
      break;

    case 'A':
      res = '<';
      break;

    case 'S':
      res = 'v';
      break;

    case 'D':
      res = '>';
      break;
    
    default:
      break;
  }
  return res;
}


/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if(c == 'v' || c == 's' || c == 'S')
    return cur_row + 1;
  else if(c == '^' || c == 'w' || c == 'W')
    return cur_row - 1;
  else
    return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if(c == '>' || c == 'd' || c == 'D')
    return cur_col + 1;
  else if(c == '<' || c == 'a' || c == 'A')
    return cur_col - 1;
  else 
    return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // snake_t snake_temp = state -> snakes[snum];
  // head_row, head_col
  unsigned int temp_head_row = state -> snakes[snum].head_row;
  unsigned int temp_head_col = state -> snakes[snum].head_col;
  // char** temp_board          = state -> board;
  char temp_head             = state -> board[temp_head_row][temp_head_col];
  unsigned int next_head_row = get_next_row(temp_head_row, temp_head);
  unsigned int next_head_col = get_next_col(temp_head_col, temp_head);
  // printf("%d %d %d %c %d %d %d\n", snum, temp_head_row, temp_head_col, temp_head, next_head_row, next_head_col, state -> board[next_head_row][next_head_col]);
  return state -> board[next_head_row][next_head_col];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // snake_t snake_temp = state -> snakes[snum];
  unsigned int temp_head_row = state -> snakes[snum].head_row;
  unsigned int temp_head_col = state -> snakes[snum].head_col;
  char temp_head             = state -> board[temp_head_row][temp_head_col];
  unsigned int next_head_row = get_next_row(temp_head_row, temp_head);
  unsigned int next_head_col = get_next_col(temp_head_col, temp_head);
  state -> snakes[snum].head_row = next_head_row;
  state -> snakes[snum].head_col = next_head_col;
  state -> board[next_head_row][next_head_col] = temp_head;
  state -> board[temp_head_row][temp_head_col] = head_to_body(temp_head);
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int temp_tail_row = state -> snakes[snum].tail_row;
  unsigned int temp_tail_col = state -> snakes[snum].tail_col;
  char temp_tail             = state -> board[temp_tail_row][temp_tail_col];
  unsigned int next_tail_row = get_next_row(temp_tail_row, temp_tail);
  unsigned int next_tail_col = get_next_col(temp_tail_col, temp_tail);
  state -> snakes[snum].tail_row = next_tail_row;
  state -> snakes[snum].tail_col = next_tail_col;
  // printf("%d %d\n", state -> snakes[snum].tail_row, state -> snakes[snum].tail_col);
  char next_pos = state -> board[next_tail_row][next_tail_col];
  state -> board[next_tail_row][next_tail_col] = body_to_tail(next_pos);
  state -> board[temp_tail_row][temp_tail_col] = ' ';
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for(unsigned int i = 0; i < state -> num_snakes; i++){
    snake_t snake_temp = state -> snakes[i];
    if(!snake_temp.live) continue;
    char next_head = next_square(state, i);
    if(next_head == '*'){
      update_head(state, i);
      add_food(state);
    }else if(next_head == ' '){
      update_head(state, i);
      update_tail(state, i);
    }else{
      state -> board[snake_temp.head_row][snake_temp.head_col] = 'x';
      state -> snakes[i].live = false;
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
    int buffer_size = 64;
    char *buffer = (char *)malloc(buffer_size * sizeof(char));
    if(!buffer){
        fprintf(stderr, "内存分配失败\n");
        exit(1);
    }
    int pos = 0;
    while(1){
        if(!fgets(buffer + pos, buffer_size - pos, fp)){
            break;
        }
        char *newline = strchr(buffer + pos, '\n');
        if(newline){
            *newline = '\n';
            *(newline + 1) = '\0';
            break;
        }
        pos = strlen(buffer);
        if(pos == buffer_size - 1){
            buffer_size += buffer_size;
            char *new_buffer = realloc(buffer, buffer_size);
            if(!new_buffer){
                free(buffer);
                fprintf(stderr, "内存分配失败\n");
                exit(1);
            }
            buffer = new_buffer;
            // printf("%d ", buffer_size);
        }
    }
    // printf(" | ");
    return buffer;
}


/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  if(fp == NULL){
    return NULL;
  }
  game_state_t* ptr_to_game_state = (game_state_t*)malloc(sizeof(game_state_t));
  ptr_to_game_state -> snakes = (snake_t*)malloc(1000 * sizeof(snake_t));
  
  // read in huge rows
  int buffer_size = 64;
  ptr_to_game_state -> board = (char**)malloc(buffer_size * sizeof(char*));
  int i = 0;
  for(i = 0; !feof(fp); i++){
    char* temp = read_line(fp);
    unsigned int length = (unsigned int)strlen(temp);
    if(i >= buffer_size){
      buffer_size *= 2;
      ptr_to_game_state -> board = (char**)realloc(ptr_to_game_state -> board, buffer_size * sizeof(char*));
    }
    ptr_to_game_state -> board[i] = (char*)malloc(length * sizeof(char));
    memset(ptr_to_game_state -> board[i], '\0', length);
    for(int j = 0; j < length && temp[j] !='\n'; j++){
      ptr_to_game_state -> board[i][j] = temp[j];
    }
  }
  
  ptr_to_game_state -> num_rows = (unsigned int)i - 1;
  ptr_to_game_state -> num_snakes = 0;
  return ptr_to_game_state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake_temp = state -> snakes[snum];
  unsigned int cur_row = snake_temp.tail_row;
  unsigned int cur_col = snake_temp.tail_col;
  char temp_tail = state -> board[cur_row][cur_col];
  char temp_part = temp_tail;
  // printf("%c", temp_part);
  while(!is_head(temp_part)){
    unsigned int next_row = get_next_row(cur_row, temp_part);
    unsigned int next_col = get_next_col(cur_col, temp_part);
    cur_row = next_row;
    cur_col = next_col;
    temp_part = state -> board[cur_row][cur_col];
    // printf("%c ", temp_part);
  }
  state -> snakes[snum].head_row = cur_row;
  state -> snakes[snum].head_col = cur_col;
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  unsigned int rows = state -> num_rows;
  unsigned int snakes = 0;
  state -> snakes = (snake_t*)malloc(1000 * sizeof(snake_t));
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < strlen(state -> board[i]); j++){
      // printf("%d %d\n", i, j);
      if(is_tail(state -> board[i][j])){
        // state -> snakes[snakes] = 
        //   (snake_t){.tail_row = i, .tail_col = j, .live = true};
        state -> snakes[snakes].tail_row = (unsigned int)i;
        state -> snakes[snakes].tail_col = (unsigned int)j;
        state -> snakes[snakes].live     = true;
        find_head(state, snakes);
        snakes++;
      }
    }
  }
  state -> num_snakes = snakes;
  return state;
}
