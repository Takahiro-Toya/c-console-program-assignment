#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"
#include "robot.h"
#include "rubbish.h"
#include "general.h"
// Insert other functions here, or include header files

char studentID[] = "n10056513";

// border variables
int one_third;
int two_third;
int w;
int h;
int room_top;
int room_left;
int room_right;
int room_bottom;

// set up variables
double robot_initial_x;
double robot_initial_y;
int battery_time;
double start;
int delay = 20;

// condition variables
int running_time = INT_MAX;
bool paused;
bool is_running;



/**
 / create a room
 */
void draw_border() {
    
    const int ver = '|';
    const int hor = '-';
    const int corner = '+';
    
    // lines
    draw_line(0, 0, w - 1, 0, hor);
    draw_line(0, h - 3, w - 1, h - 3, hor);
    draw_line(0, 0, 0, h - 3, ver);
    draw_line(w - 1, 0, w - 1, h - 3, ver);
    
    // status bar
    draw_line(0, 2, w - 1, 2, hor);
    draw_line(0, 4, w - 1, 4, hor);
    draw_line(one_third - 1, 0, one_third - 1, 4, ver);
    draw_line(two_third - 1, 0, two_third - 1, 4, ver);
    
    // intersections
    draw_char(0, 0, corner);
    draw_char(0, 2, corner);
    draw_char(0, 4, corner);
    draw_char(0, h - 3, corner);
    draw_char(w - 1, 0, corner);
    draw_char(w - 1, 2, corner);
    draw_char(w - 1, 4, corner);
    draw_char(w - 1, h - 3, corner);
    draw_char(one_third - 1, 0, corner);
    draw_char(one_third - 1, 2, corner);
    draw_char(one_third - 1, 4, corner);
    draw_char(two_third - 1, 0, corner);
    draw_char(two_third - 1, 2, corner);
    draw_char(two_third - 1, 4, corner);
}


/**
 * Return minutes elapsed
 */
int get_minutes(){
    int spent = (int)(get_current_time() - start);
    return spent / 60;
}

/**
 * Return second elapsed
 */
int get_seconds(){
    int spent = (int)(get_current_time() - start);
    return spent % 60;
}

/**
 * Fills top of the screen's status bar components
 */
void fill_status_bar(){
    // student number @ r1c1
    draw_formatted_lines((one_third - 10) / 2, 1, "%s", studentID);

    // current angle @ r1c2
    draw_formatted_lines((two_third - one_third - 12) / 2 + one_third, 1, "Heading: %d", get_angle());
    
    // battery level @ r1c3
    if (get_charging_status()){
        draw_string((w - 7) / 2, h - 2, "Docked!");
    }
    draw_formatted_lines((w - two_third - 13) / 2 + two_third, 1, "Battery: %d%%",
                         get_battery_level());
    // timer @ r2c1

    draw_formatted_lines((one_third - 14) / 2, 3, "Elapsed: %02d:%02d", get_minutes(), get_seconds());
    
    // load weight @ r2c2
    draw_formatted_lines((two_third - one_third - 20) / 2 + one_third, 3,
                         "Load weight (g): %d", get_collected_weight());
    
    // rubbish left @ r2c3
    draw_formatted_lines((w - two_third - 19) / 2 + two_third, 3,
                         "Litter (d,s,t): %d,%d,%d", get_num_rubbish('d'), get_num_rubbish('s'), get_num_rubbish('t'));
}




/**
 * Set screen size and define room border coordinates
 * Muust be called before calling draw_border()
 */
void set_screen_size(){
    get_screen_size(&w, &h);
    
    // status bar elements
    one_third = round(w / 3);
    two_third = round(w / 3 * 2);
    
    room_top = 5;
    room_left = 1;
    room_right = w - 2;
    room_bottom = h - 4;
}


/**
 * Draws all components of the simulation
 */
void draw_all(){
    clear_screen();
    draw_border();
    fill_status_bar();
    draw_charge_station((w - get_robot_size('s', 'w')) / 2, room_top);
    draw_trash();
    draw_slime();
    draw_dust();
    draw_robot();
    show_screen();
}

/**
 * Sets up the room with robot
 */
void setup_room() {
    // Insert setup logic here
    set_screen_size();
    set_movable_area(room_top, room_left, room_right, room_bottom);
    register_room_for_rubbish(room_top, room_left, room_right, room_bottom);
    robot_initial_x = (w - get_robot_size('r', 'w')) / 2;
    robot_initial_y = (room_bottom - room_top - get_robot_size('r', 'h')) / 2 + room_top;
    set_up_robot(robot_initial_x, robot_initial_y, 90, 100, 0);
    is_running = true;
    paused = true;
}

/**
 * Sets up rubbish
 */
void set_up_rubbish(){
    set_num_rubbish('d', get_int("How many dust would you like?"));
    set_num_rubbish('s', get_int("How many slime would you like?"));
    set_num_rubbish('t', get_int("How many trash would you like?"));
    drop_trash(robot_initial_x, robot_initial_y);
    drop_slime(robot_initial_x, robot_initial_y);
    drop_dust(robot_initial_x, robot_initial_y);
}


/**
 * Change delay by input
 */
void do_delay_cheat(){
    delay = get_int("New delay (milliseconds)?");
}

/**
 * Set time out by input (initially timeout is set to INT_MAX)
 */
void do_timeout() {
    running_time = get_int("How many seconds shall the program run?");
}

/**
 * Return true if timeout is happened, used as a condition of while loop in the main function
 */
bool timed_out() {
    return get_current_time() - start > running_time;
}

/**
 * Pauses or resumes the simulation, when the simulation is unpaused, this sets new battery time
 * so that the battery level will not be changed after unpause
 */
void do_pause(){
    if (paused) {
        set_battery_time();
    }
    paused = !paused;
}

/**
 * Resets simulation
 */
void do_reset(){
    paused = true;
    clear_screen();
    setup_room();
    draw_border();
    fill_status_bar();
    draw_charge_station((w - 9) / 2, room_top);
    draw_robot();
    show_screen();
    set_up_rubbish();
    start = get_current_time();
}

/**
 * drop a piece of rubbish at the gived coordinates
 * Parameters:
 *      type -type of rubbish ('d', 's' or 't')
 */
void do_rubbish_cheat(char type){
    int x_coord = get_int("x coordinate?");
    int y_coord = get_int("y coordinate?");
    
    if (type == 'd'){
        drop_a_piece_of_dust(x_coord, y_coord);
    } else if (type == 's'){
        drop_a_piece_of_slime(x_coord, y_coord);
    } else {
        drop_a_piece_of_trash(x_coord, y_coord);
    }
}

/**
 * move robot at given coordinates with angle
 */
void do_robot_cheat(){
    int x_coord = get_int("New x coordinate?");
    int y_coord = get_int("New y coordinate?");
    int n_angle = get_int("New angle?");
    move_robot(x_coord, y_coord, n_angle);
}

/**
 * Change robot's load weight
 */
void do_weight_cheat(){
    set_collected_weight(get_int("New load weight {0,...,65} ?"));
}

/**
 * Change robot's battery level
 */
void do_battery_cheat(){
    set_battery_level(get_int("New battery level {0,...,100} ?"));
}

/**
 * The function when battery level reaches '0%' to show that the simulation is over
 */
void battery_dead(){
    clear_screen();
    draw_all();
    draw_formatted_lines((w - 40) / 2, (room_bottom - room_top - 4) / 2 + room_top,
                         "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "!!! Vacuum Cleaner ran out of charge !!!\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "!                                      !\n"
                         "!     Press 'q' to quit simulation.    !\n"
                         "!     Press 'r' to reset simulation.   !\n"
                         "!                                      !\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         );
    show_screen();
    bool q_or_r = false;
    while (!q_or_r){
        int ch = get_char();
        if (ch == 'q'){
            q_or_r = true;
            is_running = false;
        } else if (ch == 'r'){
            q_or_r = true;
            do_reset();
        }
    }
}

/**
 * Return true if the input character is for operation
 * Parameters:
 *      ch -input character
 */
bool is_input_correct(int ch){
    return ch == 'b' || ch == 'd' || ch == 'm' ||
    ch == 'o' || ch == 'p' || ch == 'r' ||
    ch == 's' || ch == 't' || ch == 'v' ||
    ch == 'w' || ch == 'y' || ch == '?';
}

/**
 * Return true if the input character is for robot cheat (moving vacuum)
 * Parameters:
 *      ch -input character
 */
bool is_robot_cheat_input_correct(int ch){
    return ch == 'i' || ch == 'j' || ch == 'k' || ch == 'l';
}

/**
 *  Dialog which displays a help screen.
 */
void do_help() {
    clear_screen();
    draw_formatted_lines(0, 0,
                         "!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "!!! VacuSim !!!\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "\n"
                         "b: immediately return to base\n"
                         "i, j, k, l: push device 1 unit North, West, South, or East\n"
                         "d, s, t: litter cheat - drop litter\n"
                         "m: change the millisecond loop delay\n"
                         "r: reset\n"
                         "o: specify time-out (seconds) after which the program terminates\n"
                         "p: pause or resume vacuum movement\n"
                         "q: quit\n"
                         "v: valuum cheat 1 - override the position of the vacuum\n"
                         "w: vacuum cheat 2 - override the load of the vacuum (grams)\n"
                         "y: vacuum cheat 3 - override battery level\n"
                         "?: display this help screen\n\n\n"
                         "Press a key to return to the game."
                         );
    show_screen();
    
    while (get_char() > 0) {}
    while (!timed_out() && wait_char() <= 0) {}
}


/**
 *  Do operation based on input character (command)
 *  Parameters:
 *      ch -a command character
 */
void do_operation(int ch) {
    
    if (ch == 'b') {
        return_to_base();
    } else if (ch == 'd' || ch == 's' || ch == 't') {
        do_rubbish_cheat(ch);
    } else if (ch == 'm') {
        do_delay_cheat();
    } else if (ch == 'o') {
        do_timeout();
    } else if (ch == 'p') {
        do_pause();
    } else if (ch == 'r') {
        do_reset();
    } else if (ch == 'v') {
        do_robot_cheat();
    } else if (ch == 'w') {
        do_weight_cheat();
    } else if (ch == 'y') {
        do_battery_cheat();
    } else if (ch == '?') {
        do_help();
    }
    set_battery_time();
}

/**
 * The loop function to control robot behaviour and handle input
 */
void loop() {
    // Insert loop logic here.
    int ch = get_char();
    
    if (ch == 'q'){
        is_running = false;
        return;
    } else if (is_battery_dead()){
        battery_dead();
    } else if (is_input_correct(ch)) {
        do_operation(ch);
    } else if (paused && is_robot_cheat_input_correct(ch)){
        update_robot_manually(ch);
    } else if (!paused) {
        update_robot();
        if (get_charging_status()){
            set_collected_weight(0);
            for (int i = get_battery_level(); i < 100; i++){
                timer_pause(30);
                set_battery_level(i + 1);
                draw_all();
            }
            finish_charging();
        }
    }
    draw_all();
    
    
}



int main() {
    srand(get_current_time());
    setup_screen();
    setup_room();
    set_up_rubbish();
    start = get_current_time();
    draw_all();
    show_screen();

    while (is_running && !timed_out()){
        loop();
        timer_pause( /* Insert delay expression here. */ delay);
    }
    
    return 0;
}
