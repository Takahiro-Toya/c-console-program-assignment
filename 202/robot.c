#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "rubbish.h"
#include "general.h"

// Insert other functions here, or include header files

#define ROBOT_WIDTH 9
#define ROBOT_HEIGHT 9
#define STATION_WIDTH 9
#define STATION_HEIGHT 3
#define BATTERY_MAX 100
#define BATTERY_RETURN 25
#define LOAD_BORDER 45
#define LOAD_MAX 65
#define TIME_SLICE 0.2

// robot movable area
double max_top;
double max_left;
double max_bottom;
double max_right;

// robot current location *use left top location of robot cleaner
double rx;
double ry;
double rdx;
double rdy;
int angle;
int current_battery_level; // battery level
double battery_time; // variable needs to calculate battery level
int battery_start_level;
bool return_base;
int collected_weight;
bool is_charging;

double station_x;
double station_y;

// charging station
char * station =
"#########"
"#########"
"#########"
;

char * robot =
"  @@@@@  "
" @@@@@@@ "
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
" @@@@@@@ "
"  @@@@@  "
;



/**
 *  Set vacuum's movable area based on room width and height
 *  Parameters: do not include wall's coordinates
 *      top -room's top y coordinate
 *      left -room's left x coordinate
 *      right -room's right x coordinate
 *      bottom -room's bottom y coordinate
 */
void set_movable_area(int top, int left, int right, int bottom){
    max_top = top;
    max_left = left;
    max_right = right - ROBOT_WIDTH + 2;
    max_bottom = bottom - ROBOT_HEIGHT + 2;
}

/**
 * Set new vacuum's load weight
 * Parameter:
 *      weight (int):
 *      if the input is more than 65, the program will set 65g
 */
void set_collected_weight(int weight){
    if (weight > LOAD_MAX){
        collected_weight = LOAD_MAX;
    } else {
        collected_weight = weight;
    }
}

/**
 * Set battery time when simulation paused, so that when simulation is resumed,
 * the battery level remains same as before pause.
 */
void set_battery_time(){
    battery_start_level = current_battery_level;
    battery_time = get_current_time();
}

/**
 * Set new battery level for vacuum
 * Parameter:
 *      level (int): integer that you want to set as new battery level
 *      if the input is more than 100, the program will set 100%
 */
void set_battery_level(int level){
    if (level > BATTERY_MAX ){
        current_battery_level = BATTERY_MAX;
    } else {
        current_battery_level = level;
    }
    set_battery_time();
}


/**
 * Return true if vacuum collides to its base
 * Parameters:
 *      x, y -x, y coordinates of vacuum
 */
bool did_robot_collide_to_station(double x, double y){
    return pixel_collision(x, y, ROBOT_WIDTH, ROBOT_HEIGHT, robot, station_x, station_y, STATION_WIDTH, STATION_HEIGHT, station);
}

/**
 * Move robot to the specified position with angle
 * Parameters:
 *      x, y -robot's new location
 *      angle -new angle
 * if the x, y inputs are invalid, this command will be ignored
 */
void move_robot(int x, int y, int ang){
    if (y >= max_top && x >= max_left &&
        x <= max_right && y < max_bottom &&
        !did_robot_collide_to_station(x, y)){
        rx = x;
        ry = y;
        angle = ang % 360;
        rdx = TIME_SLICE * cos(angle * M_PI / 180);
        rdy = TIME_SLICE * sin(angle * M_PI / 180);
    }
}

/**
 * Get vacuum's current battey level
 */
int get_battery_level(){
    return current_battery_level;
}

/**
 * Get vacuum's current location
 * Parameter:
 *      type -'x' or 'y' that you want to get coordinate of vacuum
 *             if 'x' then this returns x coordinate of vacuum
 */
double get_robot_location(char type){
    if (type == 'x'){
        return rx;
    } else {
        return ry;
    }
}

/**
 * Get vacuum's current heading
 */
int get_angle(){
    return angle;
}

/**
 * Get vacuum's current load weight
 */
int get_collected_weight(){
    return collected_weight;
}

/**
 * Get vacuum and base size
 * Parameters:
 *      rs: 'r' for robot, 's' for charging base
 *      wh: 'w' for width, 'h' for height
 *      if you pass 'r', 'w', this function return vacuum's width
 */
int get_robot_size(char rs, char wh){
    if (rs == 'r'){
        if (wh == 'w'){
            return ROBOT_WIDTH;
        } else {
            return ROBOT_HEIGHT;
        }
    } else {
        if (wh == 'w'){
            return STATION_WIDTH;
        } else {
            return STATION_HEIGHT;
        }
    }
}

/**
 *  Return true if the vacuum is currently being charged
 */
bool get_charging_status(){
    return is_charging;
}



/**
 * Returns true if the robot collected more than LOAD_BORDER (default 45g) rubbish
 */
bool check_rubbish_load(){
    return collected_weight > LOAD_BORDER;
}

void calculate_battery_level(){
    double spent = (get_current_time() - battery_time);
    current_battery_level = battery_start_level - (int)spent;
}

/**
 *  Checks if collision occurs between robot and rubbish
 *  if collision is happening at next robot's location, pick up the rubbish
 *  Parameters:
 *      x, y -location of robot (left top)
 */
void detect_rubbish(int x, int y){
    
    for (int d = 0; d < get_num_rubbish('d'); d++){
        if (is_rubbish_detected('d', x, y, d)){
            if (check_rubbish_load()){return;}
            pick_rubbish('d');
            collected_weight += get_rubbish_weight('d');
            update_rubbish_array(d, 'd');
        }
    }
    for (int s = 0; s < get_num_rubbish('s'); s++){
        if (is_rubbish_detected('s', x, y, s)){
            if (check_rubbish_load()){return;}
            pick_rubbish('s');
            collected_weight += get_rubbish_weight('s');
            update_rubbish_array(s, 's');
        }
    }
    for (int t = 0; t < get_num_rubbish('t'); t++){
        if (is_rubbish_detected('t', x, y, t)){
            if (check_rubbish_load()){return;}
            pick_rubbish('t');
            collected_weight += get_rubbish_weight('t');
            update_rubbish_array(t, 't');
        }
    }
}




/**
 * Force robot to return to charging base
 */
void return_to_base(){
    
    return_base = true;
    
    double rxc = rx + ROBOT_WIDTH / 2;
    double st_x_c = station_x + STATION_WIDTH / 2;
    double st_y = max_top + 1;
    double t1 = ry - st_y;
    double t2 = st_x_c - rxc;
    double radian = atan2(t2, t1);
    
    angle = 270 + (int)(radian * 180 / M_PI);
    double angle_as_radian = angle * M_PI / 180;
    rdx = TIME_SLICE * cos(angle_as_radian);
    rdy = TIME_SLICE * sin(angle_as_radian);
    
}


/**
 * Generates new random angle between 30 ~ 60 degrees
 */
double next_angle(){
    double next = rand() % (60 + 1 - 30) + 30;
    return next;
}

/**
 * Determines if the robot should swivel left or right
 *      -determined by if the rand() generate even or odd number
 */
int next_swing_direction(){
    int next = rand() % (10 + 1 - 1) + 1;
    return next;
}

/**
 * Set dx and dy based on angle
 */
void set_dx_dy(){
    double radian = angle * M_PI / 180;
    rdx = TIME_SLICE * cos(radian);
    rdy = TIME_SLICE * sin(radian);
}

/**
 * Sets new direction for robot when it hits an obstacle
 */
void set_new_direction(){
    
    // generate random between 30 to 60
    double random_angle = next_angle();
    
    // decide swing left or right
    int randomLR = next_swing_direction();
    if (randomLR % 2 == 0){
        random_angle = -random_angle;
    }
    
    // set new angle
    int temp = angle + 180 + random_angle;
    
    // mod 360 for the case temp is less that 0 or greater thatn 360 degree
    angle = temp % 360;
    
    set_dx_dy();
}

/**
 * Return true if vacuum's battery is dead (that is current_battely_leve == 0)
 */
bool is_battery_dead(){
    return current_battery_level <= 0;
}

/**
 * Tells vacuum that charging process has finished
 */
void finish_charging(){
    return_base = false;
    is_charging = false;
}

/**
 * Let vacuum bounce off the object
 */
void bounce(){
    set_new_direction();
    timer_pause(100);
}


/**
 * Updates vacuum to move new location
 */
void update_robot(){

    bool bounced = false;
    
    calculate_battery_level();
    
    // next robot locations
    double new_x = rx + rdx;
    double new_y = ry + rdy;

    if ((check_rubbish_load() || current_battery_level < BATTERY_RETURN) && !return_base){
        return_to_base();
    }

    if (((new_x <= max_left || new_x >= max_right) && !return_base) ||
        ((new_y <= max_top || new_y >= max_bottom) && !return_base) ||
        did_robot_collide_to_station(new_x, new_y)){
        if (return_base){
            is_charging = true;
        }
        bounced = true;
        bounce();
    }

    if (!bounced){
        rx += rdx;
        ry += rdy;
    }
    
    if (! return_base){
        detect_rubbish(rx, ry);
    }
}


/**
 * Updates vacuum by command input
 * Parameter:
 *      ch: 'i', 'j', 'k' or 'l'
 */
void update_robot_manually(int ch){
    
    return_base = false;
    
    // head north
    if (ch == 'i' && !(ry - 1 < max_top) && !did_robot_collide_to_station(rx, ry - 1)) {
        ry--;
    }
    // head west
    else if (ch == 'j' && !(rx - 1 < max_left) && !did_robot_collide_to_station(rx - 1, ry)) {
        rx--;
    }
    // head south
    else if (ch == 'k' && !(ry + 1 >= max_bottom) && !did_robot_collide_to_station(rx, ry + 1)) {
        ry++;
    }
    // head east
    else if (ch == 'l' && !(rx + 1 >= max_right) && !did_robot_collide_to_station(rx + 1, ry)) {
        rx++;
    }
    
    if (current_battery_level >= BATTERY_RETURN){
        detect_rubbish(rx, ry);
    }
}



/**
 * Draws vacuum
 */
void draw_robot(){
    draw_pixels(rx, ry, ROBOT_WIDTH, ROBOT_HEIGHT, robot, true);
}

/**
 * Draws charging station
 * Parameters:
 *      x -station's x (top left corner)
 *      y -station's y (top left corner)
 */
void draw_charge_station(double x, double y){
    station_x = x;
    station_y = y;
    draw_pixels(x, y, STATION_WIDTH, STATION_HEIGHT, station, true);
}

/**
 * Set up vacuum with initial location, heading, battery level and load weight
 * Parameters;
 *      x, y -x, y coordinate of vacuum (top left corner)
 *      ang -heading of vacuum
 *      bat -battery level
 *      wei -load weight
 */
void set_up_robot(double x, double y, int ang, int bat, int wei){
    rx = x;
    ry = y;
    angle = ang;
    set_dx_dy();
    current_battery_level = bat;
    battery_start_level = current_battery_level;
    collected_weight = wei;
    return_base = false;
    register_objects_for_rubbish((screen_width() - ROBOT_WIDTH) / 2, max_top, STATION_WIDTH, STATION_HEIGHT, ROBOT_WIDTH, ROBOT_HEIGHT, station, robot);
    set_battery_time();
}







