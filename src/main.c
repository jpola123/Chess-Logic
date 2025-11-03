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
int selected_square[] = {4, 7};
int old_coordinates[] = {4, 7};
uint8_t board[8][8];

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
    LCD_DrawFillRectangle(board_x * 30, 40 + board_y * 30, (board_x + 1) * 30, 40 + (board_y + 1) * 30, board_color);
    for(int k = 0; k < 30; k++){
        for(int l = 0; l < 30; l++){
            int index = k + l * 30;
            int x_coord = board_x * 30 + k;
            int y_coord = board_y * 30 + l + 40;
            if(drawn_piece != 0){
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
    } 
}

void draw_board(uint8_t board[8][8]){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            u16 board_color = (i + j) % 2 ? 0x9264 : 0xeed2;
            uint16_t drawn_piece = board[i][j];
            LCD_DrawFillRectangle(j * 30, 40 + i * 30, (j + 1) * 30, 40 + (i + 1) * 30, board_color);
            for(int k = 0; k < 30; k++){
                for(int l = 0; l < 30; l++){
                    int index = k + l * 30;
                    int x_coord = j * 30 + k;
                    int y_coord = i * 30 + l + 40;
                    if(drawn_piece != 0){
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
            }
                
        }
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

void gpio_isr(){
    if(gpio_get_irq_event_mask(10) == 0x8){
        gpio_acknowledge_irq(10, 0x8);
        if(selected_square[1] > 0){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[1]--;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
    }
    else if(gpio_get_irq_event_mask(11) == 0x8){
        gpio_acknowledge_irq(11, 0x8);
        if(selected_square[1] < 7){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[1]++;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
    }
    else if(gpio_get_irq_event_mask(9) == 0x8){
        gpio_acknowledge_irq(9, 0x8);
        if(selected_square[0] > 0){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[0]--;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
    }
    else if(gpio_get_irq_event_mask(12) == 0x8){
        gpio_acknowledge_irq(12, 0x8);
        if(selected_square[0] < 7){
            old_coordinates[1] = selected_square[1];
            old_coordinates[0] = selected_square[0];
            selected_square[0]++;
            old_piece = board[old_coordinates[1]][old_coordinates[0]];
            draw_square(old_piece, old_coordinates[0], old_coordinates[1], false);
        }
        
    }
    else if(gpio_get_irq_event_mask(13) == 0x8){
        gpio_acknowledge_irq(13, 0x8);
    }
    selected_piece = board[selected_square[1]][selected_square[0]];
    draw_square(selected_piece, selected_square[0], selected_square[1], true);    
}


void init_gpio() {
    gpio_init(21);
    gpio_init(26);
    gpio_init_mask(0x1F << 9);
    gpio_add_raw_irq_handler_masked(0x1F<<9, gpio_isr);
    gpio_set_irq_enabled(9, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(10, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(11, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(12, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(13, GPIO_IRQ_EDGE_RISE, true);
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

    #ifndef ANIMATION
    #ifndef CHESS
    #define N_BODIES   3      // Number of bodies in the simulation
    #define G          12.0f  // Gravitational constant
    #define DT         0.01f // Simulation time step
    #define SOFTENING  5.0f   // Prevents extreme forces at close range

    // Colors as per the 16-bit RGB565 specification.
    #define BLACK      0x0000
    #define RED        0xF800
    #define LIME       0x07E0   // brighter green
    #define BLUE       0x001F

    // Make things easier to keep track of for each "body".
    typedef struct {
        float x, y, vx, vy, mass;
        uint16_t color;
    } Body;

    // Clear everything so we start from scratch
    LCD_Clear(BLACK);

    // Initialize all bodies in a compact list
    Body bodies[N_BODIES] = {
        { .x=120.0f, .y=100.0f, .vx= 1.2f, .vy= 0.5f, .mass=20.0f, .color=RED  },
        { .x=180.0f, .y=250.0f, .vx=-0.8f, .vy=-1.0f, .mass=25.0f, .color=LIME },
        { .x= 60.0f, .y=250.0f, .vx= 0.5f, .vy= 0.9f, .mass=30.0f, .color=BLUE }
    };

    // Infinite Animation Loop
    while(1) {
        // Calculate accelerations and update velocities
        for (int i = 0; i < N_BODIES; i++) {
            float total_accel_x = 0.0f;
            float total_accel_y = 0.0f;

            for (int j = 0; j < N_BODIES; j++) {
                if (i == j) continue;

                float dx = bodies[j].x - bodies[i].x;
                float dy = bodies[j].y - bodies[i].y;
                // d^2 = dx^2 + dy^2 (+ a fake softening factor to avoid collisions)
                float dist_sq = dx * dx + dy * dy + SOFTENING;
                // Newton's law of gravitation: F = G * m1 * m2 / d^2
                float inv_dist_cubed = 1.0f / (dist_sq * sqrtf(dist_sq));
                
                // Acceleration = Force / mass, but we multiply by mass to get the force directly
                // so we can use it to update velocity directly.
                total_accel_x += dx * inv_dist_cubed * bodies[j].mass * G;
                total_accel_y += dy * inv_dist_cubed * bodies[j].mass * G;
            }
            bodies[i].vx += total_accel_x * DT;
            bodies[i].vy += total_accel_y * DT;
        }
        
        // Update positions and draw each body
        for (int i = 0; i < N_BODIES; i++) {
            // new position = old position + velocity * time step
            bodies[i].x += bodies[i].vx * DT;
            bodies[i].y += bodies[i].vy * DT;

            // Wrap around screen edges
            if (bodies[i].x < 0)    bodies[i].x += 240;
            if (bodies[i].x >= 240) bodies[i].x -= 240;
            if (bodies[i].y < 0)    bodies[i].y += 320;
            if (bodies[i].y >= 320) bodies[i].y -= 320;

            LCD_DrawPoint((uint16_t)bodies[i].x, (uint16_t)bodies[i].y, bodies[i].color);
        }

        // Slow it WAY down so we can see the planets interact with each other!
        sleep_ms(1);
    }
    #endif
    #endif
    
    /*
        Now, for some more fun!

        Uncomment the ANIMATION #define at the top of main.c 
        to run this section.
        
        We've converted a popular GIF into a series of images, 
        and stored each of those frames in its own C array.  
        Look at the lab for the script we wrote to do this.

        This is an example of how you can draw a very large picture, 
        but notice how slow the animation is, even at 100 MHz.  
        The LCD_DrawPicture function is not really intended for such 
        large images, but it will be very helpful for smaller ones, 
        like scary monsters and nice sprites in a game.
    */ 

    #ifndef CHESS
    Picture* frame_pic = NULL;
    int frame_index = 0;
    while (1) { // Loop forever
        // Get the next frame from the array
        frame_pic = load_image(mystery_frames[frame_index]);
    
        if (frame_pic) {
            // Draw the frame to the top-left corner of the screen
            LCD_DrawPicture(0, 0, frame_pic);
            
            // Free the Picture struct (not the pixel data)
            free_image(frame_pic);
        }
    
        // Move to the next frame, looping back to the start
        frame_index++;
        if (frame_index >= mystery_frame_count) {
            frame_index = 0;
        }
    
        // Add a small delay to control animation speed
        sleep_ms(1); // Adjust delay as needed
    }
    #endif

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
    
    
    bool vertical_dir = false;
    bool horizontal_dir = false;
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