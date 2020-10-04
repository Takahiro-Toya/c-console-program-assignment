#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "general.h"

// Insert other functions here, or include header files
#define DUST_WEIGHT 1
#define SLIME_WEIGHT 5
#define TRASH_WEIGHT 20
#define DUST_MAX 1000
#define SLIME_MAX 10
#define TRASH_MAX 5

// room area definition
double left, right, top, bottom;

// fixed robot and charging station information
double station_x, station_y;
int station_width, station_height;
int robot_width, robot_height;

const int dust_width = 1;
const int dust_height = 1;
const int slime_width = 5;
const int slime_height = 5;
const int trash_width = 11;
const int trash_height = 6;

// declare number of each rabish
int num_trash;
int num_slime;
int num_dust;

// array of location of each rubbish
int tx[5];
int ty[5];
int sx[10];
int sy[10];
int dx[1000];
int dy[1000];

char * station_p;

char * robot_p;


// rubbish chars
char * dust = ".";

char * slime =
" ~~~ "
"~~~~~"
"~~~~~"
"~~~~~"
" ~~~ "
;

char * trash =
"     &     "
"    &&&    "
"   &&&&&   "
"  &&&&&&&  "
" &&&&&&&&& "
"&&&&&&&&&&&"
;


/**
 * Set number of rubbish to drop on the floor
 * Parameters:
 *      type -rubbish type, 'd' for dust, 's' for slime, 't' for trash
 *      num -number of rubbish (specified by first parameter) to drop
 */
void set_num_rubbish(char type, int num){
    if (type == 'd'){
        if (num > DUST_MAX || num < 0){
            num_dust = DUST_MAX;
        } else {
            num_dust = num;
        }
    } else if (type == 's'){
        if (num > SLIME_MAX || num < 0){
            num_slime = SLIME_MAX;
        } else {
            num_slime = num;
        }
    } else if (type == 't'){
        if (num > TRASH_MAX || num < 0){
            num_trash = TRASH_MAX;
        } else {
            num_trash = num;
        }
    }
}


/**
 * Return true if rubbish specified by parameters overwraps other rubbish which is already dropped
 * Parameters:
 *      x, y -x, y coordinate of rubbish to be checked
 *      width, height -width and height of rubbish to be checked
 *      pixels[] -pixel image of rubbish to be checked
 */
bool is_rubbish_overwrapped_by_new_rubbish(int x, int y, int width, int height, char pixels[]){
    if (num_trash == 0 && num_slime == 0 && num_dust == 0){
        return false;
    } else {
        // check if new rubbish overwraps trash
        if (num_trash != 0){
            int t = 0;
            while (t < num_trash){
                if (pixel_collision(x, y, width, height, pixels, tx[t], ty[t], trash_width, trash_height, trash)){
                    return true;
                }
                t++;
            }
        }
        // check if new rubbish overwraps slime
        if (num_slime != 0){
        int s = 0;
            while (s < num_slime){
                if (pixel_collision(x, y, width, height, pixels, sx[s], sy[s], slime_width, slime_height, slime)){
                    return true;
                }
                s++;
            }
        }
        
        if (num_dust != 0){
            // check if new rubbish overwraps dust
            int d = 0;
            while (d < num_dust){
                if (pixel_collision(x, y, width, height, pixels, dx[d], dy[d], dust_width, dust_height, dust)){
                    return true;
                }
                d++;
            }
        }
    }
    return false;
}


/**
 * Drop trash at random location (maximum 5 pieces of trash)
 * Parameters:
 *      x, y -x, y coordinate of vacuum
 */
void drop_trash(double x, double y){
    
    // area
    int max_left = left;
    int max_top = top;
    int max_bottom = bottom - trash_height + 1;
    int max_right = right - trash_width + 1;
    
    // num of trash dropped so far
    int t = 0;
    
    // WHILE 1
    // until the num of the trash dropped so far reach the number of trash required
    while(t < num_trash){
        bool ok = false;
        
        // WHILE 2
        // until the next trash's location is determined without overwrapping
        while(!ok){
            
            // generate random temporary location within the room
            int temp_t_x = rand() % (max_right + 1 - max_left) + max_left;
            int temp_t_y = rand() % (max_bottom + 1 - max_top) + max_top;
            
            // 1. make sure new trash does not overwrap charging station and robot
            if (pixel_collision(temp_t_x, temp_t_y, trash_width, trash_height, trash, x, y, robot_width, robot_height, robot_p) ||
                pixel_collision(temp_t_x, temp_t_y, trash_width, trash_height, trash, station_x, station_y, station_width, station_height, station_p)){
                continue;
            }
            
            if (!is_rubbish_overwrapped_by_new_rubbish(temp_t_x, temp_t_y, trash_width, trash_height, trash)){
                ok = true;
                tx[t] = temp_t_x;
                ty[t] = temp_t_y;
                t++;
            }
        }// end of while 2
    } // end of while 1
} // end of spread_trash()



/**
 * Drop slime at random location (maximum 10 pieces of trash)
 * Parameters:
 *      x, y -x, y coordinate of vacuum
 */
void drop_slime(double x, double y){
    
    // area
    int max_left = left;
    int max_top = top;
    int max_bottom = bottom - slime_height + 1;
    int max_right = right - slime_width + 1;
    
    // num of slime dropped so far
    int s = 0;
    
    // WHILE 1
    // until the num of the trash dropped so far reach the number of trash required
    while(s < num_slime){
        bool ok = false;
        
        // WHILE 2
        // until the next trash's location is determined without overwrapping
        while(!ok){
            
            // generate random temporary location within the room
            int temp_s_x = rand() % (max_right + 1 - max_left) + max_left;
            int temp_s_y = rand() % (max_bottom + 1 - max_top) + max_top;
            
            // 1. make sure new trash does not overwrap charging station and robot
            if (pixel_collision(temp_s_x, temp_s_y, slime_width, slime_height, slime, x, y, robot_width, robot_width, robot_p) ||
                pixel_collision(temp_s_x, temp_s_y, slime_width, slime_height, slime, station_x, station_y, station_width, station_height, station_p)){
                continue;
            }
            
            if (!is_rubbish_overwrapped_by_new_rubbish(temp_s_x, temp_s_y, slime_width, slime_height, slime)){
                ok = true;
                sx[s] = temp_s_x;
                sy[s] = temp_s_y;
                s++;
            }
        }// end of while 2
    } // end of while 1
}







/**
 * Drop dust at random location (maximum 1000 pieces of trash)
 * Parameters:
 *      x, y -x, y coordinate of vacuum
 */
void drop_dust(double x, double y){
    
    // num of slime dropped so far
    int d = 0;
    
    // WHILE 1
    // until the num of the trash dropped so far reach the number of trash required
    while(d < num_dust){
        bool ok = false;
        
        // WHILE 2
        // until the next trash's location is determined without overwrapping
        while(!ok){
            
            // generate random temporary location within the room
            int temp_d_x = rand() % ((int)right + 1 - (int)left) + (int)left;
            int temp_d_y = rand() % ((int)bottom + 1 - (int)top) + (int)top;
            
            // 1. make sure new trash does not overwrap charging station and robot
            if (pixel_collision(temp_d_x, temp_d_y, dust_width, dust_height, dust, x, y, robot_width, robot_height, robot_p) ||
                pixel_collision(temp_d_x, temp_d_y, dust_width, dust_height, dust, station_x, station_y, station_width, station_height, station_p)){
                continue;
            }
            
            if (!is_rubbish_overwrapped_by_new_rubbish(temp_d_x, temp_d_y, dust_width, dust_height, dust)){
                ok = true;
                dx[d] = temp_d_x;
                dy[d] = temp_d_y;
                d++;
            }
        }// end of while 2
    } // end of while 1
}


/**
 * Draws all pieces of trash. This function is always called after drop_trash()
 */
void draw_trash(){
    int count = 0;
    while (count < num_trash){
        draw_pixels(tx[count], ty[count], trash_width, trash_height, trash, true);
        count++;
    }
}

/**
 * Draws all pieces of slime. This function is always called after drop_slime()
 */
void draw_slime(){
    int count = 0;
    while (count < num_slime){
        draw_pixels(sx[count], sy[count], slime_width, slime_height, slime, true);
        count++;
    }
}

/**
 * Draws all pieces of dust. This function is always called after drop_dust()
 */
void draw_dust(){
    int count = 0;
    while (count < num_dust){
        draw_string(dx[count], dy[count], dust);
        count++;
    }
}


/**
 * Update rubbish array after vacuum collecting the rubbish
 * Basically, delete an element from rubbish array after collection
 * Parameters:
 *      index: index of rubbish that vacuum will collect stored in array
 *      type: rubbish type 'd' for dust, 's' for slime, 't' for trash
 */
void update_rubbish_array(int index, char type){
    if (type == 'd'){
        // before ID
        for (int d = 0; d < index; d++){
            dx[d] = dx[d];
            dy[d] = dy[d];
        }
        // after ID
        for (int d = index; d < num_dust; d++){
            dx[d] = dx[d + 1];
            dy[d] = dy[d + 1];
        }
    } else if (type == 's'){
        for (int s = 0; s < index; s++){
            sx[s] = sx[s];
            sy[s] = sy[s];
        }
        for (int s = index; s < num_slime; s++){
            sx[s] = sx[s + 1];
            sy[s] = sy[s + 1];
        }
    } else {
        for (int t = 0; t < index; t++){
            tx[t] = tx[t];
            ty[t] = ty[t];
        }
        for (int t = index; t < num_trash; t++){
            tx[t] = tx[t + 1];
            ty[t] = ty[t + 1];
        }
    }
}

/**
 * Return true if a piece of dust, slime or trash is detected.
 * In other words, return true if vacuum collides to rubbish
 * Parameters:
 *      type -rubbish type
 *      x, y -x, y coordinate of vacuum
 *      index: rubbbish's index
 */
bool is_rubbish_detected(char type, int x, int y, int index){
    if (type == 'd'){
        return pixel_collision(x, y, robot_width, robot_height, robot_p, dx[index], dy[index], dust_width, dust_height, dust);
    } else if (type == 's'){
        return pixel_collision(x, y, robot_width, robot_height, robot_p, sx[index], sy[index], slime_width, slime_height, slime);
    } else {
        return pixel_collision(x, y, robot_width, robot_height, robot_p, tx[index], ty[index], trash_width, trash_height, trash);
    }
}

/**
 * Pick up a piece of rubbish: decrease the number of dust, slime or trash
 * that is stored in num_dust, num_slime or num_trash
 */
void pick_rubbish(char type){
    if (type == 'd'){
        num_dust -= 1;
    } else if (type == 's'){
        num_slime -= 1;
    } else {
        num_trash -= 1;
    }
}

/**
 * Register room width and height before dropping rubbish so that all pieces of rubbish
 * will be dropped within the room
 * Parameters: do not include wall's coordinates
 *      t, b -top and bottom y coordinates of room
 *      l, r -left and right x coordinates of room
 */
void register_room_for_rubbish(int t, int l, int r, int b){
    left = l;
    right = r;
    top = t;
    bottom = b;
}

/**
 * Register vacuum and charging base's size and location so that rubbish will not be dropped
 * over or under the vacuum cleaner and charging station
 * Parameters:
 *      s_x, s_y -station's x and y coordinate
 *      s_w, s_h -station's width and height
 *      r_w, r_h -vacuum's width and height
 *      s_p[] -station's pixel image
 *      r_p[] -vacuum's pixel image
 */
void register_objects_for_rubbish(double s_x, double s_y, int s_w, int s_h,
                     int r_w, int r_h, char s_p[], char r_p[]){
    station_x = s_x;
    station_y = s_y;
    station_width = s_w;
    station_height = s_h;
    robot_width = r_w;
    robot_height = r_h;
    station_p = s_p;
    robot_p = r_p;
}

/**
 * Get rubbish weight; return weiht of dust or slime or trash
 * Parameters:
 *      type: 'd' for dust, 's' for slime and 't' for trash
 */
int get_rubbish_weight(char type){
    if (type == 'd'){
        return DUST_WEIGHT;
    } else if (type == 's'){
        return SLIME_WEIGHT;
    } else {
        return TRASH_WEIGHT;
    }
}

/**
 * Get number of rubbish dropped on the floor
 * Parameters:
 *      type: 'd' for dust, 's' for slime and 't' for trash
 */
int get_num_rubbish(char type){
    if (type == 'd'){
        return num_dust;
    } else if (type == 's'){
        return num_slime;
    } else {
        return num_trash;
    }
}

/**
 * Drop a piece of dust at specified position
 * Parameters
 *      x, y -location to drop a piece of dust
 */
void drop_a_piece_of_dust(int x, int y){
    if (num_dust < DUST_MAX &&
        x >= left &&
        x <= right &&
        y >= top &&
        y <= bottom &&
        ! pixel_collision(x, y, dust_width, dust_height, dust, station_x, station_y, station_width, station_height, station_p)){
        dx[num_dust] = x;
        dy[num_dust] = y;
        num_dust++;
    }
}

/**
 * Drop a piece of slime at specified position
 * Parameters
 *      x, y -location to drop a piece of slime (slime's left top)
 */
void drop_a_piece_of_slime(int x, int y){
    if (num_slime < SLIME_MAX &&
        x >= left &&
        x <= right - slime_width + 1 &&
        y >= top &&
        y <= bottom - slime_height + 1 &&
        ! pixel_collision(x, y, slime_width, slime_height, slime, station_x, station_y, station_width, station_height, station_p)){
        sx[num_slime] = x;
        sy[num_slime] = y;
        num_slime++;
    }
}

/**
 * Drop a piece of trash at specified position
 * Parameters
 *      x, y -location to drop a piece of trash (trash's left top)
 */
void drop_a_piece_of_trash(int x, int y){
    if (num_trash < TRASH_MAX &&
        x >= left &&
        x <= right - trash_width + 1 &&
        y >= top &&
        y <= bottom - trash_height + 1 &&
        ! pixel_collision(x, y, trash_width, trash_height, trash, station_x, station_y, station_width, station_height, station_p)){
        tx[num_trash] = x;
        ty[num_trash] = y;
        num_trash++;
    }
}

