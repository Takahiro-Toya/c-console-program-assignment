#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"

// Insert other functions here, or include header files

#define ROBOT_WIDTH 9.0
#define ROBOT_HEIGHT 9.0
#define DUST_WEIGHT 1.0
#define SLIME_WEIGHT 5.0
#define TRASH_WEIGHT 20.0
#define LOAD_BORDER 45.0
#define BATTERY_MAX 100.0

void draw_pixels(int left, int top, int width, int height, char bitmap[], bool space_is_transparent){
    
    for (int j=0; j<height; j++){
        for(int i=0; i<width; i++){
            if (bitmap[i + j * width] != ' '){
                draw_char(left + i, top + j, bitmap[i + j * width]);
            } else if (space_is_transparent == false){
                draw_char(left + i, top + j, ' ');
            } // end of if - else if
        } // end of for(i)
    } // end of for(j)
} // end of draw_pixels

bool pixel_collision(int x0, int y0, int w0, int h0, int x1, int y1, int w1, int h1){
    
    for (int i=x0; i<x0+w0; i++){
        for(int j=y0; j<y0+h0; j++){
            if (i >= x1 &&
                i < x1 + w1 &&
                j >= y1 &&
                j < y1 + h1 &&
                i >= x0 &&
                i < x0 + w0 &&
                j >= y0 &&
                j < y0 + h0 ) {
                return true;
            } // end of if
        } // end of for(i)
    } // end of for(j)
    
    return false;
}


// border variables
int one_third;
int two_third;
int w;
int h;
int room_width;
int room_height;

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

bool return_base;
int battery;



// collected rabish weight
int collected_weight;

// declare number of each rabish
int num_trash;
int num_slime;
int num_dust;

// array of location of each rabbish
int tx[5];
int ty[5];
int sx[10];
int sy[10];
int dx[1000];
int dy[1000];

double start;
int delay;
int time_out;
bool paused = false;
bool is_run;



/*
 / must be called before calling draw_border()
 */
void set_screen_size(){
    get_screen_size(&w, &h);
    
    // status bar elements
    one_third = round(w / 3);
    two_third = round(w / 3 * 2);
    
    // room info
    room_width = w - 2; // deduct border width * 2 (because there are border at both side)
    room_height = h - 8; // deduct status bar size
    
    // assign robot movable area
    max_top = 5; // consider status bar size (fixed size)
    max_left = 1; // consider border width
    max_bottom = h - 2 - ROBOT_HEIGHT;
    max_right = w - ROBOT_WIDTH;
}

/*
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

int battery_time;
char heading[15];
char batterylevel[15];
char elapsed[20];
char load[20];
char litter[30];


/**
 * convert time spent (double) to mm:ss format
 */
void format_timer(){
    int mm = 0;
    int ss = 0;
    int spent = (int)(get_current_time() - start);
    if (spent >= 60){
        mm = spent / 60;
        ss = spent - mm * 60;
    } else {
        ss = spent;
    }
    snprintf(elapsed, 20, "Elapsed: %02d:%02d", mm, ss);
}

void calculate_battery_level(){
    int spent = (int)(get_current_time() - battery_time);
    battery = BATTERY_MAX - spent;
}

/**
 * Fills top of the screen status bar components
 */
void fill_status_bar(){
    // student number @ r1c1
    draw_string(5, 1, "n10056513");
    // current angle @ r1c2
    
    snprintf(heading, 15, "Heading: %d", angle);
    draw_string(one_third + 5, 1, heading);
    // battery level @ r1c3
//    spent = get_current_time() - start;
//    level = (int)(BATTERY_MAX - spent);
//    snprintf(battery, 6, "%d%%", level);
    snprintf(batterylevel, 15, "Battery: %d%%", battery);
    draw_string(two_third + 5, 1, batterylevel);
    // timer @ r2c1
    format_timer();
    draw_string(5, 3, elapsed);
    // load weight @ r2c2
    snprintf(load, 20, "Load weight (g): %d", collected_weight);
    draw_string(one_third + 5, 3, load);
    // rabbish left @ r2c3
    snprintf(litter, 30, "Litter (d,s,t): %d,%d,%d", num_dust, num_slime, num_trash);
    draw_string(two_third + 5, 3, litter);
}

char * station =
"#########"
"#########"
"#########"
;

void draw_charge_station(){
    draw_pixels(round(w / 2) - 5, 5, 9, 3, station, false);
}

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

// rabbish chars
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




void set_dust_num(int num){
    if (num > 1000 || num < 0){
        num_dust = 1000;
    } else {
        num_dust = num;
    }
}

void set_slime_num(int num){
    if (num > 10 || num < 0){
        num_slime = 10;
    } else {
        num_slime = num;
    }
}

void set_trash_num(int num){
    if (num > 5 || num < 0){
        num_trash = 5;
    } else {
        num_trash = num;
    }
}

/*
 * Return true if objct overwraps trash
 * x -left location
 * y -top location
 * num -number of object to be compared
 * width -compared object's width
 * height -compared object's height
 * obj[] -object to be compared
 */
bool trash_overwrap(int x, int y, int num, int width, int height, char obj[]){
    if (num == 0){
        return false;
    }
    int i = 0;
    while (i < num){
        if (pixel_collision(x, y, width, height, tx[i], ty[i], 11, 9)){
            return true;
        }
        i++;
    }
    
    return false;
}

/* Determines location of trash randomly, and drop trash without overwrapping
 * note: trash width = 11
 *       trash height = 6
 */
void drop_trash(){
    
    // area
    int max_left = 1;
    int max_top = 5;
    int max_bottom = screen_height() - 3 - 6;
    int max_right = screen_width() - 1 - 11;
    
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
            if (pixel_collision(temp_t_x, temp_t_y, 11, 6, rx, ry, 9, 9) || pixel_collision(temp_t_x, temp_t_y, 11, 6, round(w / 2) - 5, 5, 9, 3)){
                continue;
            }
            
            // 2. make sure new trash does not overwrap any other trash
            if(! trash_overwrap(temp_t_x, temp_t_y, t, 11, 9, trash)){
                ok = true;
                tx[t] = temp_t_x;
                ty[t] = temp_t_y;
                t++;
            }
        }// end of while 2
    } // end of while 1
} // end of spread_trash()


/*
 * Draw trash: called after drop_trash()
 */
void draw_trash(){
    int count = 0;
    while (count < num_trash){
        draw_pixels(tx[count], ty[count], 11, 6, trash, false);
        count++;
    }
}



/*
 * Return true if objct overwrap slime
 * x -left location
 * y -top location
 * num -number of object to be compared
 * width -compared object's width
 * height -compared object's height
 * obj[] -object to be compared
 */
bool slime_overwrap(int x, int y, int num, int width, int height, char obj[]){
    if (num == 0){
        return false;
    }
    int i = 0;
    while (i < num){
        if (pixel_collision(x, y, width, height, sx[i], sy[i], 5, 5)){
            return true;
        }
        i++;
    }
    
    return false;
}


/* Determines location of slime randomly, and drop slime without overwrapping
 * note: slime width = 5
 *       slime height = 5
 */
void drop_slime(){
    
    // area
    int max_left = 1;
    int max_top = 5;
    int max_bottom = screen_height() - 3 - 5;
    int max_right = screen_width() - 1 - 5;
    
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
            if (pixel_collision(temp_s_x, temp_s_y, 5, 5, rx, ry, 9, 9) || pixel_collision(temp_s_x, temp_s_y, 5, 5, round(w / 2) - 5, 5, 9, 3)){
                continue;
            }
            
            // 2. make sure new slime does not overwrap trash
            if(trash_overwrap(temp_s_x, temp_s_y, 5, 5, 5, slime)){
                continue;
            }
            
            // 3. make sure new slime does not overwrap any other slim
            if(! slime_overwrap(temp_s_x, temp_s_y, s, 5, 5, slime)){
                ok = true;
                sx[s] = temp_s_x;
                sy[s] = temp_s_y;
                s++;
            }
        }// end of while 2
    } // end of while 1
}

/*
 * Draw slime: called after drop_slim()
 */
void draw_slime(){
    int count = 0;
    while (count < num_slime){
        draw_pixels(sx[count], sy[count], 5, 5, slime, false);
        count++;
    }
}


/*
 * Return true if dust overwrap dust, only used to check if dust overwrap other dust
 * x -left location
 * y -top location
 */
bool dust_overwrap(int x, int y, int num){
    if (num == 0){
        return false;
    }
    int i = 0;
    while (i < num){
        if (pixel_collision(x, y, 1, 1, dx[i], dy[i], 1, 1)){
            return true;
        }
        i++;
    }
    
    return false;
}


/* Determines location of dust randomly, and drop dust without overwrapping
 * note: trash width = 1
 *       trash height = 1
 */
void drop_dust(){
    // area
    int max_left = 1;
    int max_top = 5;
    int max_bottom = screen_height() - 3 - 1;
    int max_right = screen_width() - 1 - 1;
    
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
            int temp_d_x = rand() % (max_right + 1 - max_left) + max_left;
            int temp_d_y = rand() % (max_bottom + 1 - max_top) + max_top;
            
            // 1. make sure new trash does not overwrap charging station and robot
            if (pixel_collision(temp_d_x, temp_d_y, 1, 1, rx, ry, 9, 9) || pixel_collision(temp_d_x, temp_d_y, 1, 1, round(w / 2) - 5, 5, 9, 3)){
                continue;
            }
            
            // 2. make sure new slime does not overwrap trash
            if(trash_overwrap(temp_d_x, temp_d_y, 5, 1, 1, dust)){
                continue;
            }
            
            // 3. make sure new slime does not overwrap any other slime
            if(slime_overwrap(temp_d_x, temp_d_y, 10, 1, 1, dust)){
                continue;
            }
            
            // 4. make sure new dust does not overwrap any other dust
            if (! dust_overwrap(temp_d_x, temp_d_y, d)){
                ok = true;
                dx[d] = temp_d_x;
                dy[d] = temp_d_y;
                d++;
            }
        }// end of while 2
    } // end of while 1
}


/*
 * Draw dust: called after drop_dust()
 */
void draw_dust(){
    int count = 0;
    while (count < num_dust){
        draw_string(dx[count], dy[count], dust);
        count++;
    }
}


/**
 * update rabbish array such as dx, dy, sx, sy, tx, ty,
 * delete element identified by id and type
 *  Parameters:
 *      x -x location of rabbish to be deleted
 *      y -y location of rabbish to be deleted
 *      type -rabbish type: 'd', 's', or 't';
 */
void update_rabbish_array(int id, char type){
    if (type == 'd'){
        // before ID
        for (int d = 0; d < id; d++){
            dx[d] = dx[d];
            dy[d] = dy[d];
        }
        // after ID
        for (int d = id; d < num_dust; d++){
            dx[d] = dx[d + 1];
            dy[d] = dy[d + 1];
        }
    } else if (type == 's'){
        for (int s = 0; s < id; s++){
            sx[s] = sx[s];
            sy[s] = sy[s];
        }
        for (int s = id; s < num_slime; s++){
            sx[s] = sx[s + 1];
            sy[s] = sy[s + 1];
        }
    } else if (type == 't'){
        for (int t = 0; t < id; t++){
            tx[t] = tx[t];
            ty[t] = ty[t];
        }
        for (int t = id; t < num_trash; t++){
            tx[t] = tx[t + 1];
            ty[t] = ty[t + 1];
        }
    } else {
        
    }
}


bool check_rabbish_load(){
    return collected_weight > LOAD_BORDER;
}

/**
 *  check if collision occurs between robot and rabbish
 *  if collision is happening at next robot's location, pick up the rabbish
 *
 */
void detect_rabbish(int x, int y){
    
    for (int d = 0; d < num_dust; d++){
        if (pixel_collision(x, y, ROBOT_WIDTH, ROBOT_HEIGHT,
                            dx[d], dy[d], 1, 1)){
            if (check_rabbish_load()){return;}
            num_dust -= 1;
            collected_weight += DUST_WEIGHT;
            update_rabbish_array(d, 'd');

        }
    }
    for (int s = 0; s < num_slime; s++){
        if (pixel_collision(x, y, ROBOT_WIDTH, ROBOT_HEIGHT,
                            sx[s], sy[s], 5, 5)){
            if (check_rabbish_load()){return;}
            check_rabbish_load();
            num_slime -= 1;
            collected_weight += SLIME_WEIGHT;
            update_rabbish_array(s, 's');

        }
    }
    for (int t = 0; t < num_trash; t++){
        if (pixel_collision(x, y, ROBOT_WIDTH, ROBOT_HEIGHT,
                            tx[t], ty[t], 11, 6)){
            if (check_rabbish_load()){return;}
            check_rabbish_load();
            num_trash -= 1;
            collected_weight += TRASH_WEIGHT;
            update_rabbish_array(t, 't');

        }
    }
}


void return_to_base(){
    return_base = true;
    double rxc = rx + 4;
    double st_x_c = round(w / 2) - 1;
    double st_y = 5;
    double t1 = ry - st_y;
    double t2 = st_x_c - rxc;
    double radian = atan2(t2, t1);
    angle = 270 + (int)(radian * 180 / M_PI);
    double angle_as_radian = angle * M_PI / 180;
    rdx = cos(angle_as_radian);
    rdy = sin(angle_as_radian);
    
}

double next_angle(){
    double next = rand() % (60 + 1 - 30) + 30;
    return next;
}

int next_swing_direction(){
    int next = rand() % (10 + 1 - 1) + 1;
    return next;
}


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
    
    // modulo calcuration in case the angle is less than 0 or greater than 360
    if (temp < 0){
        angle = temp + 360;
    } else if (temp >= 360){
        angle = temp - 360;
    } else {
        angle = temp;
    }

    // set new direction
    double radian = angle * M_PI / 180;
    rdx = cos(radian);
    rdy = sin(radian);
}


/**
 * Moves robot automatically
 */
void update_robot(){
    
    double new_x = rx + rdx;
    double new_y = ry + rdy;

    
    if (collected_weight >= LOAD_BORDER && !return_base){
        return_to_base();
    }
    if (battery <= 25 && !return_base){
        return_to_base();
    }
    
    bool bounced = false;
    
    // return_base must be false
    if (((new_x <= max_left || new_x >= max_right) && !return_base) ||
        ((new_y <= max_top || new_y >= max_bottom) && !return_base)){
        set_new_direction();
        timer_pause(100);
        bounced = true;
    }
//    if ((new_y <= max_top || new_y >= max_bottom ) && !return_base){
//        set_new_direction();
//        timer_pause(100);
//        bounced = true;
//    }
    
    if (pixel_collision(new_x, new_y, ROBOT_WIDTH, ROBOT_HEIGHT,
                        round(w / 2) - 5, 5, 9, 3)){
        if (return_base){
            collected_weight = 0;
            draw_string(two_third + 5, 1, batterylevel);
            timer_pause(battery * 3 / 100);
            battery = 100;
            battery_time = get_current_time();
            return_base = false;
            
        }
        set_new_direction();
        timer_pause(100);
        bounced = true;
    }
    
    if (!bounced){
        rx += rdx;
        ry += rdy;
    }
    
    detect_rabbish(rx, ry);
}


/**
 * Updates robot location manually according to command input
 * Parameter:
 *      ch must be 'i', 'j', 'k' or 'l'
 *          -confirmed by is_robot_cheat_input_correct(int ch)
 */
void update_robot_manually(int ch){
    
    // return_base must be false, so after command input, the robot sets new direction
    // towards the charging station
    return_base = false;
    
    // head north
    if (ch == 'i' && !(ry - 1 <= max_top) && !pixel_collision(rx, ry - 1, ROBOT_WIDTH, ROBOT_HEIGHT,round(w / 2) - 5, 5, 9, 3)) {
        ry--;
        detect_rabbish(rx, ry);
    }
    // head west
    else if (ch == 'j' && !(rx - 1 <= max_left) && !pixel_collision(rx - 1, ry, ROBOT_WIDTH, ROBOT_HEIGHT,round(w / 2) - 5, 5, 9, 3)) {
        rx--;
        detect_rabbish(rx, ry);
    }
    // head south
    else if (ch == 'k' && !(ry + 1 >= max_bottom) && !pixel_collision(rx, ry + 1, ROBOT_WIDTH, ROBOT_HEIGHT,round(w / 2) - 5, 5, 9, 3)) {
        ry++;
        detect_rabbish(rx, ry);
    }
    // head east
    else if (ch == 'l' && !(rx + 1 >= max_right) && !pixel_collision(rx + 1, ry, ROBOT_WIDTH, ROBOT_HEIGHT,round(w / 2) - 5, 5, 9, 3)) {
        rx++;
        detect_rabbish(rx, ry);
    }
}

void draw_robot(){
    draw_pixels(rx, ry, ROBOT_WIDTH, ROBOT_HEIGHT, robot, false);
}

void draw_all(){
    clear_screen();
    
    draw_border();
    fill_status_bar();
    draw_charge_station();
    draw_trash();
    draw_slime();
    draw_dust();
    draw_robot();
    
    show_screen();
}


void setup() {
    // Insert setup logic here
    set_screen_size();
    battery = 100;
    delay = 200;
    collected_weight = 0;
    return_base = false;
    is_run = true;
    rx = round(room_width / 2) - round(ROBOT_WIDTH / 2) + 1;
    ry = round(room_height / 2) - round(ROBOT_HEIGHT / 2) + 2;
    // go down
    angle = 90;
    rdx = 0.0;
    rdy = 1.0;
    set_trash_num(5);
    set_slime_num(10);
    set_dust_num(1000);
    drop_trash();
    drop_slime();
    drop_dust();
    // draw room, status bar and a robot cleaner at the center of the room
    draw_all();
    
}


bool timed_out() {
    return get_current_time() - start > time_out;
}

/*
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

void do_delay_cheat(){
    delay = get_int("New delay (milliseconds)?");
    
    if (delay < 0) {delay = 200;}
}

void do_timeout() {
    time_out = get_int("How many seconds shall the program run?");
}



void do_pause(){
    paused = !paused;
}


bool is_input_correct(int ch){
    return ch == 'b' || ch == 'd' || ch == 'm' ||
            ch == 'o' || ch == 'p' || ch == 'r' ||
            ch == 's' || ch == 't' || ch == 'v' ||
            ch == 'w' || ch == 'y' || ch == '?';
}

bool is_robot_cheat_input_correct(int ch){
    return ch == 'i' || ch == 'j' || ch == 'k' || ch == 'l';
}
/**
 *  Jump table which chooses the action (if any) which corresponds to a
 *  supplied command character.
 *
 *  Parameters:
 *      ch: a command character. Currently recognised comands are listed in
 *          the do_help function.
 */
void do_operation(int ch) {
    if (ch == 'b') {
        // stop cleaning
        return_to_base();
        // return to base
        
    } else if (ch == 'd') {
        // drop a piece of dust
        
    } else if (ch == 'm') {
        // set milliseconds delay
        do_delay_cheat();
    } else if (ch == 'o') {
        // set time out (measured in seconds)
        do_timeout();
    } else if (ch == 'p') {
        // stop/ run device
        do_pause();
    } else if (ch == 'r') {
        // reset simulation
        
    } else if (ch == 's') {
        // drop a piece of slime
        
    } else if (ch == 't') {
        // drop a piece of trash
        
    } else if (ch == 'v') {
        // move the device to a new location and specify a new direction
        
    } else if (ch == 'w') {
        // change the amount
        
    } else if (ch == 'y') {
        // change battery level
        
    } else if (ch == '?') {
        // display help screen
        do_help();
    }
}

/**
 *  The loop function: called once per time slice to run the simulation.
 */
void loop() {
    // Insert loop logic here.
    
    int ch = get_char();
    
    if (ch == 'q'){
        is_run = false;
        return;
    } else if (is_input_correct(ch)) {
        do_operation(ch);
    } else if (!paused) {
        if (is_robot_cheat_input_correct(ch)){
            update_robot_manually(ch);
        } else {
            update_robot();
        }
        calculate_battery_level();
    }
    draw_all();

}

int main() {
    srand(get_current_time());
    setup_screen();
    setup();
    show_screen();
    start = get_current_time();
    battery_time = get_current_time();
    int i = 0;
    while (is_run || !timed_out()){///* Insert termination conditions here */ i < 1000) {
        
        loop();
        i++;
        timer_pause( /* Insert delay expression here. */ delay);
    }
    
    return 0;
}


