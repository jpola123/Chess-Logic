#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <math.h>   
#include "pieces.h"

/****************************************** */
#define PIN_SDI    19
#define PIN_CS     17
#define PIN_SCK    18
#define PIN_DC     16
#define PIN_nRESET 15

// Uncomment the following #define when 
// you are ready to run Step 3.

// WARNING: The process will take a VERY 
// long time as it compiles and uploads 
// all the image frames into the uploaded 
// binary!  Expect to wait 5 minutes.
//#define ANIMATION

#define CHESS

/****************************************** */
#ifdef ANIMATION
#include "images.h"
#endif
/****************************************** */

uint16_t selected_piece;
uint16_t old_piece;
uint16_t chosen_piece = NULL;
int selected_square[] = {4, 7};
int old_coordinates[] = {4, 7};
int chosen_coordinates[2];
uint8_t board[8][8];
bool move_generation = false;
move_list * white_moves = NULL;
move_list * black_moves = NULL;
move_list * legal_moves = NULL;
bool current_move = false;
bool w_king = false;
bool b_king = false;
bool left_w_rook = false;
bool right_w_rook = false;
bool left_b_rook = false;
bool right_b_rook = false;
bool right_w_castle = false;
bool left_w_castle = false;
bool right_b_castle = false;
bool left_b_castle = false;
int num_of_moves = 0;
int pieces_taken_w = 0;
int pieces_taken_b = 0;
bool en_passant = false;
int en_pass_x;
int en_pass_y;

//false is white, true is black

bool find_legal_move_coord(int, int);


void draw_piece(uint16_t bitmap[], int index, int x_coord, int y_coord){
    bool draw = false;
    draw |= bitmap[index] != 0x0000;
    if(index > 0){
        draw |= bitmap[index-1] != 0x0000;
    } 
    if(index > 30){
        draw |= bitmap[index-30] != 0x0000;
    }
    if(index + 30 < 900){
        draw |= bitmap[index+30] != 0x0000;
    }
    if(index + 1 < 900){
        draw |= bitmap[index+1] != 0x0000;
    }
    if(draw){
        LCD_DrawPoint(x_coord, y_coord, bitmap[index]);
    }
}

void draw_square(uint16_t drawn_piece, int board_x, int board_y, bool selected){
    u16 board_color = selected ? RED : (board_y + board_x) % 2 ? 0x9264 : 0xeed2;
    board_color = (board_y < 0 || board_y > 7) ? BLACK : board_color;
    LCD_DrawFillRectangle(board_x * 30, 40 + board_y * 30, (board_x + 1) * 30, 40 + (board_y + 1) * 30, board_color);
    for(int k = 0; k < 30; k++){
        for(int l = 0; l < 30; l++){
            int index = k + l * 30;
            int x_coord = board_x * 30 + k;
            int y_coord = board_y * 30 + l + 40;
            if(drawn_piece == WHITE_PAWN){
                draw_piece(white_pawn, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_BISHOP){
                draw_piece(white_bishop, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_KING){
                draw_piece(white_king, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_QUEEN){
                draw_piece(white_queen, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_KNIGHT){
                draw_piece(white_knight, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_ROOK){
                draw_piece(white_rook, index, x_coord, y_coord);
            }
            if(drawn_piece == BLACK_PAWN){
                draw_piece(black_pawn, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_BISHOP){
                draw_piece(black_bishop, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_KING){
                draw_piece(black_king, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_QUEEN){
                draw_piece(black_queen, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_KNIGHT){
                draw_piece(black_knight, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_ROOK){
                draw_piece(black_rook, index, x_coord, y_coord);
            }
        }
    }
    if(legal_moves != NULL && find_legal_move_coord(board_x, board_y)){
        LCD_Circle(board_x * 30 + 15, board_y * 30 + 55, 5, true, GRAY);
    } 
}

void draw_captured(uint16_t drawn_piece, float board_x, float board_y, bool selected){
    for(int k = 0; k < 30; k++){
        for(int l = 0; l < 30; l++){
            int index = k + l * 30;
            int x_coord = board_x * 30 + k;
            int y_coord = board_y * 30 + l + 40;
            if(drawn_piece == WHITE_PAWN){
                draw_piece(white_pawn, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_BISHOP){
                draw_piece(white_bishop, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_KING){
                draw_piece(white_king, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_QUEEN){
                draw_piece(white_queen, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_KNIGHT){
                draw_piece(white_knight, index, x_coord, y_coord);
            }
            else if(drawn_piece == WHITE_ROOK){
                draw_piece(white_rook, index, x_coord, y_coord);
            }
            if(drawn_piece == BLACK_PAWN){
                draw_piece(black_pawn, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_BISHOP){
                draw_piece(black_bishop, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_KING){
                draw_piece(black_king, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_QUEEN){
                draw_piece(black_queen, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_KNIGHT){
                draw_piece(black_knight, index, x_coord, y_coord);
            }
            else if(drawn_piece == BLACK_ROOK){
                draw_piece(black_rook, index, x_coord, y_coord);
            }
        }
    }
    if(legal_moves != NULL && find_legal_move_coord(board_x, board_y)){
        LCD_Circle(board_x * 30 + 15, board_y * 30 + 55, 5, true, GRAY);
    } 
}

void draw_board(uint8_t board[8][8]){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            draw_square(board[i][j], j, i, false);
        }
    }    
}

void draw_legal_moves(){
    move_list ** head = &legal_moves;
    while((*head) != NULL){
        if(((*head)->last_move) != NULL){
            draw_square(board[(*head)->y_coord][(*head)->x_coord], (*head)->x_coord, (*head)->y_coord, false);
        }
        head = &((*head)->next_move);
    }
}

bool check_legal_move(int move_x, int move_y){
    uint8_t temp[8][8];
    memcpy(temp, board, sizeof(temp));
    temp[chosen_coordinates[1]][chosen_coordinates[0]] = 0;
    temp[move_y][move_x] = chosen_piece;
    if(en_passant){
        temp[chosen_coordinates[1]][move_x] = 0;
    }
    uint8_t king_id = current_move ? 8 : 2;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            uint8_t attacking_piece = temp[i][j];
            if(attacking_piece != 0  && ((current_move && attacking_piece < 7)|| (!current_move && attacking_piece > 6))){
                //simulate the moves. If the king of the opposing color is in the path of attack, return false.
                if(attacking_piece == WHITE_PAWN){
                    if(temp[i-1][j+1] == king_id || temp[i-1][j-1] == king_id){
                        return false;
                    }
                }
                if(attacking_piece == WHITE_BISHOP || attacking_piece == WHITE_QUEEN || attacking_piece == WHITE_KING || attacking_piece == BLACK_BISHOP || attacking_piece == BLACK_QUEEN || attacking_piece == BLACK_KING){
                    int max_range = attacking_piece == WHITE_KING || attacking_piece == BLACK_KING ? 2 : 8;
                    for(int k = 1; k < max_range; k++){
                        if(k + i < 8 && k + j < 8){
                            if(temp[k+i][k+j] == king_id){
                                return false;
                            }
                            else if((temp[k+i][k+j] != 0)){
                                break;
                            }
                        } 
                    }
                    for(int k = 1; k < max_range; k++){
                        if(i-k >= 0 && k + j < 8){
                            if(temp[i-k][j+k] == king_id){
                                return false;
                            }
                            else if((temp[i-k][k+j] != 0)){
                                break;
                            }
                        } 
                    }
                    for(int k = 1; k < max_range; k++){
                        if(i+k < 8 && j - k >= 0){
                            if(temp[i+k][j-k] == king_id){
                                return false;
                            }
                            else if((temp[i+k][j-k] != 0)){
                                break;
                            }
                        } 
                    }
                    for(int k = 1; k < max_range; k++){
                        if(i-k >= 0 && j - k >= 0){
                            if(temp[i-k][j-k] == king_id){
                                return false;
                            }
                            else if((temp[i-k][j-k] != 0)){
                                break;
                            }
                        } 
                    }
                }
                if(attacking_piece == WHITE_ROOK || attacking_piece == WHITE_QUEEN || attacking_piece == WHITE_KING || attacking_piece == BLACK_ROOK || attacking_piece == BLACK_QUEEN || attacking_piece == BLACK_KING){
                    int max_range = attacking_piece == WHITE_KING || attacking_piece == BLACK_KING ? 2 : 8;
                    for(int k = 1; k < max_range; k++){
                        if(k + i < 8){
                            if(temp[k+i][j] == king_id){
                                return false;
                            }
                            else if((temp[k+i][j] != 0)){
                                break;
                            }
                        } 
                    }
                    for(int k = 1; k < max_range; k++){
                        if(k + j < 8){
                            if(temp[i][j+k] == king_id){
                                return false;
                            }
                            else if((temp[i][j+k] != 0)){
                                break;
                            }
                        } 
                    }
                    for(int k = 1; k < max_range; k++){
                        if(j - k >= 0){
                            if(temp[i][j-k] == king_id){
                                return false;
                            }
                            else if((temp[i][j-k] != 0)){
                                break;
                            }
                        } 
                    }
                    for(int k = 1; k < max_range; k++){
                        if(i-k >= 0){
                            if(temp[i-k][j] == king_id){
                                return false;
                            }
                            else if((temp[i-k][j] != 0)){
                                break;
                            }
                        } 
                    }
                }
                if(attacking_piece == WHITE_KNIGHT || attacking_piece == BLACK_KNIGHT){
                    //down 1 right 2
                    if(j + 2 < 8 && i + 1 < 8){
                        if(temp[i+1][j+2] == king_id){
                            return false;
                        }
                    }
                    //down 2 right 1
                    if(j + 1 < 8 && i + 2 < 8){
                        if(temp[i+2][j+1] == king_id){
                            return false;
                        }
                    }
                    //down 1 left 2
                    if(j - 2 >= 0 && i + 1 < 8){
                        if(temp[i+1][j-2] == king_id){
                            return false;
                        }
                    }
                    //down 2 left 1
                    if(j - 1 >= 0 && i + 2 < 8){
                        if(temp[i+2][j-1] == king_id){
                            return false;
                        }
                    }
                    //up 1 right 2
                    if(j + 2 < 8 && i - 1 >= 0){
                        if(temp[i-1][j+2] == king_id){
                            return false;
                        }
                    }
                    //up 2 right 1
                    if(j + 1 < 8 && i - 2 >= 0){
                        if(temp[i-2][j+1] == king_id){
                            return false;
                        }
                    }
                    //up 1 left 2
                    if(j - 2 >= 0 && i - 1 >= 0){
                        if(temp[i-1][j-2] == king_id){
                            return false;
                        }
                    }
                    //up 2 left 1
                    if(j - 1 >= 0 && i - 2 >= 0){
                        if(temp[i-2][j-1] == king_id){
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

//delete the linked list
void delete_list(int list_type){
    move_list ** head = list_type == 0 ? &legal_moves : list_type == 1 ? &white_moves : &black_moves;
    move_list * current = *head;
    move_list * next;
    while((current) != NULL){
        next = current->next_move;
        free(current);
        current = next;
    }
    *head = NULL;
}


void clear_legal_moves(){
    num_of_moves = 0;
    move_list ** head = &legal_moves;
    move_list * current = *head;
    move_list * next;
    while((current) != NULL){
        next = current->next_move;
        free(current);
        current = next;
    }
    *head = NULL;
    draw_board(board);
}
bool find_legal_move(){
    move_list ** head = &legal_moves;
    while(*head != NULL){
        if((*head)->x_coord == selected_square[0] && (*head)->y_coord == selected_square[1]){
            return true;
        }
        head = &(*head)->next_move;
    }
    return false;
}

bool find_legal_move_coord(int x, int y){
    move_list ** head = &legal_moves;
    while((*head) != NULL){
        if((*head)->x_coord == x && (*head)->y_coord == y){
            return true;
        }
        head = &((*head)->next_move);
    }
    return false;
}

//add a node to the linked list of moves. List Type: 0: legal moves, 1: white_moves, 2: black_moves
void add_move(uint16_t piece_id, uint16_t x_coord, uint16_t y_coord, uint8_t list_type){
    if(check_legal_move(x_coord, y_coord)){
        num_of_moves++;
        move_list ** head = list_type == 0 ? &legal_moves : list_type == 1 ? &white_moves : &black_moves;
        move_list ** prev = NULL;
        while((*head) != NULL){
            prev = head;
            head = &(*head)->next_move;
        }
        (*head) = (move_list *)malloc(sizeof(move_list));
        (*head)->last_move = (*prev);
        (*head)->pieceId = piece_id;
        (*head)->x_coord = x_coord;
        (*head)->y_coord = y_coord;
        (*head)->next_move = NULL;
    }
}

void legal_move_generator(bool checkmate_assessment){
    if(!current_move){
        if(chosen_piece == WHITE_PAWN){
            if(chosen_coordinates[1] == 6 && board[5][chosen_coordinates[0]] == 0){
                if(board[chosen_coordinates[1] - 2][chosen_coordinates[0]] == 0){
                    add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] - 2, 0);
                }
            }
            if(chosen_coordinates[1] > 0){
                if(board[chosen_coordinates[1] - 1][chosen_coordinates[0]] == 0){
                    add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] - 1, 0);
                }
                if(chosen_coordinates[0] == 0){
                    if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] + 1] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] - 1, 0);
                    }
                }
                else if(chosen_coordinates[0] == 7){
                    if(board[chosen_coordinates[1]- 1][chosen_coordinates[0] - 1] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] - 1, 0);
                    }
                }
                else{
                    if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] - 1] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] - 1, 0);
                    }
                    if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] + 1] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] - 1, 0);
                    }
                }
            }
            if(en_passant){
                if(chosen_coordinates[1] == en_pass_y){
                    if(chosen_coordinates[0] - 1 == en_pass_x){
                        add_move(chosen_piece, en_pass_x, chosen_coordinates[1] - 1, 0);
                    }
                    else if(chosen_coordinates[0] + 1 == en_pass_x){
                        add_move(chosen_piece, en_pass_x, chosen_coordinates[1] - 1, 0);
                    }
                }
            }
        }
        if(chosen_piece == WHITE_BISHOP || chosen_piece == WHITE_QUEEN || chosen_piece == WHITE_KING){
            //methodology: go in one direction and then break once we hit an obstacle
            int max_range = chosen_piece == WHITE_KING ? 2 : 8;
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] + i < 8 && chosen_coordinates[1] + i < 8){
                    if(board[chosen_coordinates[1] + i][chosen_coordinates[0] + i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] + i, 0);
                    }
                    else if(board[chosen_coordinates[1] + i][chosen_coordinates[0] + i] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] + i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] - i >= 0 && chosen_coordinates[1] + i < 8){
                    if(board[chosen_coordinates[1] + i][chosen_coordinates[0] - i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] + i, 0);
                    }
                    else if(board[chosen_coordinates[1] + i][chosen_coordinates[0] - i] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] + i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] + i < 8 && chosen_coordinates[1] - i >= 0){
                    if(board[chosen_coordinates[1] - i][chosen_coordinates[0] + i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] - i, 0);
                    }
                    else if(board[chosen_coordinates[1] - i][chosen_coordinates[0] + i] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] - i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] - i >= 0 && chosen_coordinates[1] - i >= 0){
                    if(board[chosen_coordinates[1] - i][chosen_coordinates[0] - i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] - i, 0);
                    }
                    else if(board[chosen_coordinates[1] - i][chosen_coordinates[0] - i] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] - i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
        }
        if(chosen_piece == WHITE_KNIGHT){
            //just hard code check these, no point in doing extra work
            //down 1 right 2
            if(chosen_coordinates[0] + 2 < 8 && chosen_coordinates[1] + 1 < 8){
                if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 2] == 0 || board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 2] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] + 2, chosen_coordinates[1] + 1, 0);
                }
            }
            //down 2 right 1
            if(chosen_coordinates[0] + 1 < 8 && chosen_coordinates[1] + 2 < 8){
                if(board[chosen_coordinates[1] + 2][chosen_coordinates[0] + 1] == 0 || board[chosen_coordinates[1] + 2][chosen_coordinates[0] + 1] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] + 2, 0);
                }
            }
            //down 1 left 2
            if(chosen_coordinates[0] - 2 >= 0 && chosen_coordinates[1] + 1 < 8){
                if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] - 2] == 0 || board[chosen_coordinates[1] + 1][chosen_coordinates[0] - 2] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] - 2, chosen_coordinates[1] + 1, 0);
                }
            }
            //down 2 left 1
            if(chosen_coordinates[0] - 1 >= 0 && chosen_coordinates[1] + 2 < 8){
                if(board[chosen_coordinates[1] + 2][chosen_coordinates[0] - 1] == 0 || board[chosen_coordinates[1] + 2][chosen_coordinates[0] - 1] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] + 2, 0);
                }
            }
            //up 1 right 2
            if(chosen_coordinates[0] + 2 < 8 && chosen_coordinates[1] - 1 >= 0){
                if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] + 2] == 0 || board[chosen_coordinates[1] - 1][chosen_coordinates[0] + 2] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] + 2, chosen_coordinates[1] - 1, 0);
                }
            }
            //up 2 right 1
            if(chosen_coordinates[0] + 1 < 8 && chosen_coordinates[1] - 2 >= 0){
                if(board[chosen_coordinates[1] - 2][chosen_coordinates[0] + 1] == 0 || board[chosen_coordinates[1] - 2][chosen_coordinates[0] + 1] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] - 2, 0);
                }
            }
            //up 1 left 2
            if(chosen_coordinates[0] - 2 >= 0 && chosen_coordinates[1] - 1 >= 0){
                if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] - 2] == 0 || board[chosen_coordinates[1] - 1][chosen_coordinates[0] - 2] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] - 2, chosen_coordinates[1] - 1, 0);
                }
            }
            //up 2 left 1
            if(chosen_coordinates[0] - 1 >= 0 && chosen_coordinates[1] - 2 >= 0){
                if(board[chosen_coordinates[1] - 2][chosen_coordinates[0] - 1] == 0 || board[chosen_coordinates[1] - 2][chosen_coordinates[0] - 1] > 6){
                    add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] - 2, 0);
                }
            }
        }
        if(chosen_piece == WHITE_ROOK || chosen_piece == WHITE_QUEEN || chosen_piece == WHITE_KING){
            int max_range = chosen_piece == WHITE_KING ? 2 : 8;
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] + i < 8){
                    if(board[chosen_coordinates[1]][chosen_coordinates[0] + i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1], 0);
                    }
                    else if(board[chosen_coordinates[1]][chosen_coordinates[0] + i] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1], 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[1] + i < 8){
                    if(board[chosen_coordinates[1] + i][chosen_coordinates[0]] == 0){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] + i, 0);
                    }
                    else if(board[chosen_coordinates[1] + i][chosen_coordinates[0]] > 6){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] + i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] - i >= 0){
                    if(board[chosen_coordinates[1]][chosen_coordinates[0] - i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1], 0);
                    }
                    else if(board[chosen_coordinates[1]][chosen_coordinates[0] - i] > 6){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1], 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[1] - i >= 0){
                    if(board[chosen_coordinates[1] - i][chosen_coordinates[0]] == 0){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] - i, 0);
                    }
                    else if(board[chosen_coordinates[1] - i][chosen_coordinates[0]] > 6){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] - i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
        }
        if(chosen_piece == WHITE_KING && !w_king && !right_w_rook){
            if(board[7][6] == 0 && board[7][5] == 0){
                if(check_legal_move(6, 7) && check_legal_move(5, 7) && check_legal_move(4, 7)){
                    add_move(chosen_piece, 6, 7, 0);
                    right_w_castle = true;
                }
                else{
                    right_w_castle = false;
                }
            }
            else {
                right_w_castle = false;
            }
        }
        else {
            right_w_castle = false;
        }
        if(chosen_piece == WHITE_KING && !w_king && !left_w_rook){
            if(board[7][3] == 0 && board[7][2] == 0 && board[7][1] == 0){
                if(check_legal_move(3, 7) && check_legal_move(2, 7) && check_legal_move(4, 7)){
                    add_move(chosen_piece, 2, 7, 0);
                    left_w_castle = true;
                }
                else{
                    left_w_castle = false;
                }
            }
            else {
                left_w_castle = false;
            }
        }
        else {
            right_w_castle = false;
        }
    }
    else{
        if(chosen_piece == BLACK_PAWN){
            if(chosen_coordinates[1] == 1 && board[2][chosen_coordinates[0]] == 0){
                if(board[chosen_coordinates[1] + 2][chosen_coordinates[0]] == 0){
                    add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] + 2, 0);
                }
            }
            if(chosen_coordinates[1] < 7){
                if(board[chosen_coordinates[1] + 1][chosen_coordinates[0]] == 0){
                    add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] + 1, 0);
                }
                if(chosen_coordinates[0] == 0){
                    if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 1] < 7 && board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 1] != 0){
                        add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] + 1, 0);
                    }
                }
                else if(chosen_coordinates[0] == 7){
                    if(board[chosen_coordinates[1]+ 1][chosen_coordinates[0] - 1] < 7 && board[chosen_coordinates[1] + 1][chosen_coordinates[0] - 1] != 0){
                        add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] + 1, 0);
                    }
                }
                else{
                    if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] - 1] < 7 && board[chosen_coordinates[1] + 1][chosen_coordinates[0] - 1] != 0){
                        add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] + 1, 0);
                    }
                    if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 1] < 7 && board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 1] != 0){
                        add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] + 1, 0);
                    }
                }
            }
            if(en_passant){
                if(chosen_coordinates[1] == en_pass_y){
                    if(chosen_coordinates[0] - 1 == en_pass_x){
                        add_move(chosen_piece, en_pass_x, chosen_coordinates[1] + 1, 0);
                    }
                    else if(chosen_coordinates[0] + 1 == en_pass_x){
                        add_move(chosen_piece, en_pass_x, chosen_coordinates[1] + 1, 0);
                    }
                }
            }
        }
        if(chosen_piece == BLACK_BISHOP || chosen_piece == BLACK_QUEEN || chosen_piece == BLACK_KING){
            //methodology: go in one direction and then break once we hit an obstacle
            int max_range = chosen_piece == BLACK_KING ? 2 : 8;
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] + i < 8 && chosen_coordinates[1] + i < 8){
                    if(board[chosen_coordinates[1] + i][chosen_coordinates[0] + i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] + i, 0);
                    }
                    else if(board[chosen_coordinates[1] + i][chosen_coordinates[0] + i] < 7){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] + i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] - i >= 0 && chosen_coordinates[1] + i < 8){
                    if(board[chosen_coordinates[1] + i][chosen_coordinates[0] - i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] + i, 0);
                    }
                    else if(board[chosen_coordinates[1] + i][chosen_coordinates[0] - i] < 7){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] + i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] + i < 8 && chosen_coordinates[1] - i >= 0){
                    if(board[chosen_coordinates[1] - i][chosen_coordinates[0] + i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] - i, 0);
                    }
                    else if(board[chosen_coordinates[1] - i][chosen_coordinates[0] + i] < 7){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1] - i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] - i >= 0 && chosen_coordinates[1] - i >= 0){
                    if(board[chosen_coordinates[1] - i][chosen_coordinates[0] - i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] - i, 0);
                    }
                    else if(board[chosen_coordinates[1] - i][chosen_coordinates[0] - i] < 7){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1] - i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
        }
        if(chosen_piece == BLACK_KNIGHT){
            //just hard code check these, no point in doing extra work
            //down 1 right 2
            if(chosen_coordinates[0] + 2 < 8 && chosen_coordinates[1] + 1 < 8){
                if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] + 2] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] + 2, chosen_coordinates[1] + 1, 0);
                }
            }
            //down 2 right 1
            if(chosen_coordinates[0] + 1 < 8 && chosen_coordinates[1] + 2 < 8){
                if(board[chosen_coordinates[1] + 2][chosen_coordinates[0] + 1] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] + 2, 0);
                }
            }
            //down 1 left 2
            if(chosen_coordinates[0] - 2 >= 0 && chosen_coordinates[1] + 1 < 8){
                if(board[chosen_coordinates[1] + 1][chosen_coordinates[0] - 2] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] - 2, chosen_coordinates[1] + 1, 0);
                }
            }
            //down 2 left 1
            if(chosen_coordinates[0] - 1 >= 0 && chosen_coordinates[1] + 2 < 8){
                if(board[chosen_coordinates[1] + 2][chosen_coordinates[0] - 1] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] + 2, 0);
                }
            }
            //up 1 right 2
            if(chosen_coordinates[0] + 2 < 8 && chosen_coordinates[1] - 1 >= 0){
                if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] + 2] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] + 2, chosen_coordinates[1] - 1, 0);
                }
            }
            //up 2 right 1
            if(chosen_coordinates[0] + 1 < 8 && chosen_coordinates[1] - 2 >= 0){
                if(board[chosen_coordinates[1] - 2][chosen_coordinates[0] + 1] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] + 1, chosen_coordinates[1] - 2, 0);
                }
            }
            //up 1 left 2
            if(chosen_coordinates[0] - 2 >= 0 && chosen_coordinates[1] - 1 >= 0){
                if(board[chosen_coordinates[1] - 1][chosen_coordinates[0] - 2] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] - 2, chosen_coordinates[1] - 1, 0);
                }
            }
            //up 2 left 1
            if(chosen_coordinates[0] - 1 >= 0 && chosen_coordinates[1] - 2 >= 0){
                if(board[chosen_coordinates[1] - 2][chosen_coordinates[0] - 1] < 7){
                    add_move(chosen_piece, chosen_coordinates[0] - 1, chosen_coordinates[1] - 2, 0);
                }
            }
        }
        if(chosen_piece == BLACK_ROOK || chosen_piece == BLACK_QUEEN || chosen_piece == BLACK_KING){
            int max_range = chosen_piece == BLACK_KING ? 2 : 8;
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] + i < 8){
                    if(board[chosen_coordinates[1]][chosen_coordinates[0] + i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1], 0);
                    }
                    else if(board[chosen_coordinates[1]][chosen_coordinates[0] + i] < 7){
                        add_move(chosen_piece, chosen_coordinates[0] + i, chosen_coordinates[1], 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[1] + i < 8){
                    if(board[chosen_coordinates[1] + i][chosen_coordinates[0]] == 0){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] + i, 0);
                    }
                    else if(board[chosen_coordinates[1] + i][chosen_coordinates[0]] < 7){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] + i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[0] - i >= 0){
                    if(board[chosen_coordinates[1]][chosen_coordinates[0] - i] == 0){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1], 0);
                    }
                    else if(board[chosen_coordinates[1]][chosen_coordinates[0] - i] < 7){
                        add_move(chosen_piece, chosen_coordinates[0] - i, chosen_coordinates[1], 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
            for(int i = 1; i < max_range; i++){
                if(chosen_coordinates[1] - i >= 0){
                    if(board[chosen_coordinates[1] - i][chosen_coordinates[0]] == 0){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] - i, 0);
                    }
                    else if(board[chosen_coordinates[1] - i][chosen_coordinates[0]] < 7){
                        add_move(chosen_piece, chosen_coordinates[0], chosen_coordinates[1] - i, 0);
                        break;
                    }
                    else{
                        break;
                    }
                }
            }
        }
        if(chosen_piece == BLACK_KING && !b_king && !right_b_rook){
            if(board[0][6] == 0 && board[0][5] == 0){
                if(check_legal_move(6, 0) && check_legal_move(5, 0) && check_legal_move(4, 0)){
                    add_move(chosen_piece, 6, 0, 0);
                    right_b_castle = true;
                }
                else{
                    right_b_castle = false;
                }
            }
            else {
                right_b_castle = false;
            }
        }
        else {
            right_b_castle = false;
        }
        if(chosen_piece == BLACK_KING && !b_king && !left_b_rook){
            if(board[0][3] == 0 && board[0][2] == 0 && board[0][1] == 0){
                if(check_legal_move(3, 0) && check_legal_move(2, 0) && check_legal_move(4, 0)){
                    add_move(chosen_piece, 2, 0, 0);
                    left_b_castle = true;
                }
                else{
                    left_b_castle = false;
                }
            }
            else {
                left_b_castle = false;
            }
        }
        else {
            right_b_castle = false;
        }
    }
    if(!checkmate_assessment){
        draw_legal_moves();
    }
}

void init_spi_lcd() {
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_DC, GPIO_FUNC_SIO);
    gpio_set_function(PIN_nRESET, GPIO_FUNC_SIO);

    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_nRESET, GPIO_OUT);

    gpio_put(PIN_CS, 1); // CS high
    gpio_put(PIN_DC, 0); // DC low
    gpio_put(PIN_nRESET, 1); // nRESET high

    // initialize SPI1 with 48 MHz clock
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);
    spi_init(spi0, 100 * 1000 * 1000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

bool stalemate(){
    delete_list(0);
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            chosen_piece = board[i][j];
            chosen_coordinates[0] = j;
            chosen_coordinates[1] = i;
            if(chosen_piece != 0 && chosen_piece < 7 && !current_move){
                legal_move_generator(true);
                if(num_of_moves != 0){
                    return false;
                    num_of_moves = 0;
                }
            }
            else if(chosen_piece > 6 && current_move){
                legal_move_generator(true);
                if(num_of_moves != 0){
                    return false;
                    num_of_moves = 0;
                }
            }
        }
    }
    return true;
}

bool checkmate(){
    if(!stalemate()){
        return false;
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            chosen_piece = board[i][j];
            chosen_coordinates[0] = j;
            chosen_coordinates[1] = i;
            if(chosen_piece == WHITE_KING && !current_move){
                if(check_legal_move(j, i)){
                    return false;
                }
            }
            if(chosen_piece == BLACK_KING && current_move){
                if(check_legal_move(j, i)){
                    return false;
                }
            }
        }
    }
    return true;
}

void gpio_isr(){
    if(gpio_get_irq_event_mask(10) == GPIO_IRQ_LEVEL_HIGH){
        gpio_acknowledge_irq(10, GPIO_IRQ_LEVEL_HIGH);
        if(selected_square[1] > 0){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[1]--;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
    }
    else if(gpio_get_irq_event_mask(11) == GPIO_IRQ_LEVEL_HIGH){
        gpio_acknowledge_irq(11, GPIO_IRQ_LEVEL_HIGH);
        if(selected_square[1] < 7){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[1]++;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
    }
    else if(gpio_get_irq_event_mask(9) == GPIO_IRQ_LEVEL_HIGH){
        gpio_acknowledge_irq(9, GPIO_IRQ_LEVEL_HIGH);
        if(selected_square[0] > 0){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[0]--;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
    }
    else if(gpio_get_irq_event_mask(12) == GPIO_IRQ_LEVEL_HIGH){
        gpio_acknowledge_irq(12, GPIO_IRQ_LEVEL_HIGH);
        if(selected_square[0] < 7){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[0]++;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
        
    }
    else if(gpio_get_irq_event_mask(13) == GPIO_IRQ_LEVEL_HIGH){
        gpio_acknowledge_irq(13, GPIO_IRQ_LEVEL_HIGH);
        // if move_generation is not on, then we need to select a piece to generate moves for. 
        if((!move_generation)){
            if(selected_piece != 0){
                move_generation = true;
                chosen_piece = board[selected_square[1]][selected_square[0]];
                chosen_coordinates[0] = selected_square[0];
                chosen_coordinates[1] = selected_square[1];
                legal_move_generator(false);
            }
        }
        else{
            if(selected_square[1] != chosen_coordinates[1] || selected_square[0] != chosen_coordinates[0]){
                //if the selected square is one of the legal moves
                if(find_legal_move()){
                    int list_type = current_move ? 3 : 2;
                    move_generation = false;
                    if(selected_square[0] == en_pass_x){
                        if(!current_move && selected_square[1] + 1 == en_pass_y && selected_piece == WHITE_PAWN && en_passant){
                            draw_captured(board[en_pass_y][en_pass_x], pieces_taken_w * 0.3, 8, false);
                            pieces_taken_w++;
                            board[en_pass_y][en_pass_x] = 0;
                        }
                        else if(current_move && selected_square[1] - 1 == en_pass_y && selected_piece == BLACK_PAWN && en_passant){
                            draw_captured(board[en_pass_y][en_pass_x], pieces_taken_b * 0.3, -1, false);
                            pieces_taken_b++;
                            board[en_pass_y][en_pass_x] = 0;
                        }
                    }
                    if(!current_move){
                        w_king = (chosen_piece == WHITE_KING) || w_king;
                        right_w_rook = (chosen_piece == WHITE_ROOK && chosen_coordinates[0] == 7) || right_w_rook;
                        left_w_rook = (chosen_piece == WHITE_ROOK && chosen_coordinates[0] == 1) || left_w_rook;
                        en_passant = chosen_piece == WHITE_PAWN && chosen_coordinates[1] - selected_square[1] == 2;
                    }
                    else if(current_move){
                        b_king = (chosen_piece == BLACK_KING) || b_king;
                        right_b_rook = (chosen_piece == BLACK_ROOK && chosen_coordinates[0] == 7) || right_b_rook;
                        left_b_rook = (chosen_piece == BLACK_ROOK && chosen_coordinates[0] == 1) || left_b_rook;
                        en_passant = chosen_piece == BLACK_PAWN && chosen_coordinates[1] - selected_square[1] == -2;
                    }
                    if(en_passant){
                        en_pass_x = selected_square[0];
                        en_pass_y = selected_square[1];
                    }
                    if((right_w_castle || left_w_castle) && !current_move){
                        if(selected_square[0] == 6){
                            board[7][5] = WHITE_ROOK;
                            board[7][7] = 0;
                            right_w_castle = false;
                            left_w_castle = false;
                        }
                        else if(selected_square[0] == 2){
                            board[7][3] = WHITE_ROOK;
                            board[7][0] = 0;
                            left_w_castle = false;
                            right_w_castle = false;
                        }
                    }
                    if((right_b_castle || left_b_castle) && current_move){
                        if(selected_square[0] == 6){
                            board[0][5] = BLACK_ROOK;
                            board[0][7] = 0;
                            right_b_castle = false;
                            left_b_castle = false;
                        }
                        else if(selected_square[0] == 2){
                            board[0][3] = BLACK_ROOK;
                            board[0][0] = 0;
                            left_b_castle = false;
                            right_b_castle = false;
                        }                        
                    }
                    if(board[selected_square[1]][selected_square[0]] != 0){
                        if(current_move){
                            draw_captured(board[selected_square[1]][selected_square[0]], pieces_taken_b * 0.3, -1, false);
                            pieces_taken_b++;
                        }
                        else{
                            draw_captured(board[selected_square[1]][selected_square[0]], pieces_taken_w * 0.3, 8, false);
                            pieces_taken_w++;
                        }
                    }
                    board[selected_square[1]][selected_square[0]] = chosen_piece;
                    board[chosen_coordinates[1]][chosen_coordinates[0]] = 0; 
                    current_move = !current_move;
                    add_move(chosen_piece, selected_square[0], selected_square[1], list_type);
                    clear_legal_moves();
                    if(checkmate()){
                        if(current_move){
                            LCD_DrawString(0, 0, WHITE, BLACK, "White Wins", 12, false);
                        }
                        else{
                            LCD_DrawString(0, 0, WHITE, BLACK, "Black Wins", 12, false);
                        }
                    }
                    else if(stalemate()){
                        LCD_DrawString(0, 0, WHITE, BLACK, "TIE NO ONE WINS!", 12, false);
                    }
                    delete_list(0);
                }
                //generate for a different piece. 
                else if(selected_piece != 0){
                    clear_legal_moves();
                    move_generation = true;
                    chosen_piece = board[selected_square[1]][selected_square[0]];
                    chosen_coordinates[0] = selected_square[0];
                    chosen_coordinates[1] = selected_square[1];
                    legal_move_generator(false);
                }
                //else, just clear the legal moves
                else{
                    move_generation = false;
                    clear_legal_moves();
                }

                
            }
        }
    }
    selected_piece = board[selected_square[1]][selected_square[0]];
    draw_square(selected_piece, selected_square[0], selected_square[1], true);   
    busy_wait_ms(250);
}


void init_gpio() {
    gpio_init(21);
    gpio_init(26);
    gpio_init_mask(0x1F << 9);
    gpio_add_raw_irq_handler_masked(0x1F<<9, gpio_isr);
    gpio_set_irq_enabled(9, GPIO_IRQ_LEVEL_HIGH, true);
    gpio_set_irq_enabled(10, GPIO_IRQ_LEVEL_HIGH, true);
    gpio_set_irq_enabled(11, GPIO_IRQ_LEVEL_HIGH, true);
    gpio_set_irq_enabled(12, GPIO_IRQ_LEVEL_HIGH, true);
    gpio_set_irq_enabled(13, GPIO_IRQ_LEVEL_HIGH, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
}



Picture* load_image(const char* image_data);
void free_image(Picture* pic);


int main() {
    stdio_init_all();
    init_gpio();

    init_spi_lcd();

    LCD_Setup();
    LCD_Clear(0x0000); // Clear the screen to black

    #ifdef CHESS
    bool initial_draw = 1;
    
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(i == 1 || i == 6){
                board[i][j] = i == 1 ? BLACK_PAWN : i == 6 ? WHITE_PAWN : 0;
            }
            else if(j == 0 || j == 7){
                board[i][j] = i == 0 ? BLACK_ROOK : i == 7 ? WHITE_ROOK : 0;
            }
            else if(j == 1 || j == 6){
                board[i][j] = i == 0 ? BLACK_KNIGHT : i == 7 ? WHITE_KNIGHT : 0;
            }
            else if(j == 2 || j == 5){
                board[i][j] = i == 0 ? BLACK_BISHOP : i == 7 ? WHITE_BISHOP : 0;
            }
            else if(j == 3){
                board[i][j] = i == 0 ? BLACK_QUEEN : i == 7 ? WHITE_QUEEN : 0;
            }
            else if(j == 4){
                board[i][j] = i == 0 ? BLACK_KING : i == 7 ? WHITE_KING : 0;
            }
            else{
                board[i][j] = 0;
            }
        }
    }
    
    selected_piece = board[selected_square[1]][selected_square[0]];
    
    while(1){
        if(initial_draw){
            initial_draw = 0;
            draw_board(board);
            draw_square(board[7][4], 4, 7, true);
        }   
    }
    #endif

    for(;;);
}