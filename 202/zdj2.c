#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"

// Global variables
bool game_over = false;
int delay = 10;
int timeout = INT_MAX;
double start_time;

// Hero state
#define HERO_WIDTH 9
#define HERO_HEIGHT 3

double hx, hy;

// Zombie state
#define MAX_ZOMBIES 1000000
#define ZOMBIE_SPEED 0.1
#define ZOMBIE_WIDTH 7
#define ZOMBIE_HEIGHT 5

int num_zombies = 1;

double zx[MAX_ZOMBIES], zy[MAX_ZOMBIES], zdx[MAX_ZOMBIES], zdy[MAX_ZOMBIES];

bool paused = false;

bool collided(
              double x0, double y0, int w0, int h0,
              double x1, double y1, int w1, int h1
              );

/**
 *  Draws a border around the terminal window using '*' symbols.
 */
void draw_border() {
    int w, h;
    get_screen_size(&w, &h);
    const int ch = '*';
    draw_line(0, 0, 0, h - 1, ch);
    draw_line(0, 0, w - 1, 0, ch);
    draw_line(0, h - 1, w - 1, h - 1, ch);
    draw_line(w - 1, 0, w - 1, h - 1, ch);
}

/**
 *  Draw the hero, centred on (hx, hy).
 */
void draw_hero() {
    int left = round(hx) - HERO_WIDTH / 2;
    int right = left + HERO_WIDTH - 1;
    
    int top = round(hy) - HERO_HEIGHT / 2;
    int bottom = top + HERO_HEIGHT - 1;
    
    draw_line(left, top, left, bottom, 'H');
    draw_line(right, top, right, bottom, 'H');
    draw_line(left, round(hy), right, round(hy), 'H');
}

/**
 *  Draw one zombie, indicated by zombie ID i. The object is centred on
 *  (zx[i],zy[i]).
 *
 *  Parameters:
 *      i: An integer which satisfiess 0 <= i < num_zombies.
 */
void draw_zombie(int i) {
    int left = round(zx[i]) - ZOMBIE_WIDTH / 2;
    int right = left + ZOMBIE_WIDTH - 1;
    
    int top = round(zy[i]) - ZOMBIE_HEIGHT / 2;
    int bottom = top + ZOMBIE_HEIGHT - 1;
    
    draw_line(left, top, right, top, 'Z');
    draw_line(right, top, left, bottom, 'Z');
    draw_line(left, bottom, right, bottom, 'Z');
}

/**
 *  Draws all zombies.
 */
void draw_zombies() {
    for (int i = 0; i < num_zombies; i++) {
        draw_zombie(i);
    }
}

/**
 *  Draw all objects in the simulation.
 */
void draw_all() {
    clear_screen();
    draw_border();
    draw_hero();
    draw_zombies();
    show_screen();
}

/**
 *  Set up the hero at the centre of the terminal.
 */
void setup_hero() {
    int w, h;
    get_screen_size(&w, &h);
    hx = w / 2;
    hy = h / 2;
}

/**
 *  Set up a single zombie at a ]
 location.
 *
 *  Parameters:
 *      i: An integer satisfying 0 <= i < num_zombies which indicates the
 *          object to be placed.
 *
 *  Notes:
 *      This function assigns random values to zx[i] and zy[i] so that the
 *  object is guaranteed to be wholly contained within the border. A random
 *  velocity vector is set up in zdx[i] and zdy[i]. The zombie will take
 *  approximately (1.0 / ZOMBIE_SPEED) time slices to travel one screen unit in
 *  the direction specified by the velocity vector.
 */
void setup_zombie(int i) {
    int w, h;
    get_screen_size(&w, &h);
    
    zx[i] = 1 + ZOMBIE_WIDTH / 2  + rand() % (w - 1 - ZOMBIE_WIDTH);
    zy[i] = 1 + ZOMBIE_HEIGHT / 2 + rand() % (h - 1 - ZOMBIE_HEIGHT);
    
    double zombie_dir = rand() * 2 * M_PI / RAND_MAX;
    const double step = ZOMBIE_SPEED;
    
    zdx[i] = step * cos(zombie_dir);
    zdy[i] = step * sin(zombie_dir);
}

/**
 *  Sets up all zombies (0 <= i < num_zombies) so that they are all
 *  wholly contained within the border, and none overlap with the hero.
 */
void setup_zombies() {
    num_zombies = get_int("How many zombies would you like?");
    
    if (num_zombies < 1 || num_zombies > MAX_ZOMBIES) {
        num_zombies = 1;
    }
    
    for (int i = 0; i < num_zombies; i++) {
        do {
            setup_zombie(i);
        }
        while (collided(hx, hy, HERO_WIDTH, HERO_HEIGHT, zx[i], zy[i], ZOMBIE_WIDTH, ZOMBIE_HEIGHT));
    }
}

/**
 *  Set up all objects in the simulation.
 */
void setup(void) {
    start_time = get_current_time();
    setup_hero();
    setup_zombies();
    draw_all();
}

/**
 *  Returns true iff and only if the supplied argument is a hero navigation control.
 */
bool is_hero_ctrl(int ch) {
    return (ch == 'a' || ch == 's' || ch == 'd' || ch == 'w');
}

/**
 *  Update the hero location based on the supplied argument.
 *
 *  Parameters:
 *      ch: a character code which is expected to be one of [asdw].
 *          'a' -> move left
 *          's' -> move down
 *          'd' -> move right
 *          'w' -> move up
 *
 *  Notes:
 *      If the designated move would cause the bounding box of the hero
 *  icon to overlap the border then the move is ignored.
 */
void update_hero(int ch) {
    if (ch == 'a' && hx - HERO_WIDTH / 2 > 1) {
        hx--;
    }
    else if (ch == 'd' && hx + HERO_WIDTH / 2 < screen_width() - 2) {
        hx++;
    }
    else if (ch == 's' && hy + HERO_HEIGHT / 2 < screen_height() - 2) {
        hy++;
    }
    else if (ch == 'w' && hy - HERO_HEIGHT / 2 > 1) {
        hy--;
    }
}

/**
 *  Update the location of a zombie based on its stored velocity.
 *
 *  Parameters:
 *      i: the ID of a zombie, satisfying 0 <= i < num_zombies.
 *
 *  Notes:
 *      If the designated move would cause the bounding box of the zombie
 *  icon to overlap the border then one or both of the velocity components
 *  is negated, as appropriate to cause the zombie to reflect off the wall.
 *  When that happens the zombie does not move.
 */
void update_zombie(int i) {
    // Assume that we have not already collided with the wall.
    // Predict the next screen position of the zombie.
    int new_x = round(zx[i] + zdx[i]) - ZOMBIE_WIDTH / 2;
    int new_y = round(zy[i] + zdy[i]) - ZOMBIE_HEIGHT / 2;
    
    bool bounced = false;
    
    if (new_x == 0 || new_x + ZOMBIE_WIDTH == screen_width()) {
        // horizontal bounce
        zdx[i] = -zdx[i];
        bounced = true;
    }
    
    if (new_y == 0 || new_y + ZOMBIE_HEIGHT == screen_height()) {
        // horizontal bounce
        zdy[i] = -zdy[i];
        bounced = true;
    }
    
    if (!bounced) {
        zx[i] += zdx[i];
        zy[i] += zdy[i];
    }
}

/**
 *  Updates the locatiosn of all zombies based on their stored velocities.
 */
void update_zombies() {
    for (int i = 0; i < num_zombies; i++) {
        update_zombie(i);
    }
}

/**
 *  Returns true if and only if two supplied bounding boxes overlap.
 *
 *  Parameters:
 *      (x0,y0) - real-valued coordinates of the centre of the first bounding box.
 *      (w0,h0) - integer-valued dimensions of the first bounding box.
 *      (x1,y1) - real-valued coordinates of the centre of the second bounding box.
 *      (w1,h1) - integer-valued dimensions of the second bounding box.
 */
bool collided(
              double x0, double y0, int w0, int h0,
              double x1, double y1, int w1, int h1
              ) {
    int left0 = round(x0) - w0 / 2, right0 = left0 + w0 - 1;
    int left1 = round(x1) - w1 / 2, right1 = left1 + w1 - 1;
    
    int top0 = round(y0) - h0 / 2, bottom0 = top0 + h0 - 1;
    int top1 = round(y1) - h1 / 2, bottom1 = top1 + h1 - 1;
    
    if (right1 < left0 || right0 < left1) return false;
    if (bottom1 < top0 || bottom0 < top1) return false;
    
    return true;
}

/**
 *  Resets the simulation.
 */
void reset() {
    setup();
}

/**
 *  Obtain a time-out value from standard input and store it for future use.
 */
void do_timeout() {
    timeout = get_int("How many seconds shall the program run?");
}

/**
 *  Returns true iff the simulation has been running for longer than the
 *  current designated time-out value.
 */
bool timed_out() {
    return get_current_time() - start_time >= timeout;
}

/**
 *  Action taken when a zombie has collided with the hero.
 *  Displays a message, waits for a response from standard input
 *  (or simulation time-out), then resets the simulation.
 */
void do_collided() {
    clear_screen();
    
    const char *message[] = {
        "You're zombie food!",
        "Press any key to restart..."
    };
    
    const int rows = 2;
    
    for (int i = 0; i < rows; i++) {
        int len = strlen(message[i]);
        int x = (screen_width() - len) / 2;
        int y = i + (screen_height() - rows) / 2;
        draw_formatted(x, y, message[i]);
    }
    
    show_screen();
    
    timer_pause(1000);
    while (get_char() > 0) {}
    while (!timed_out() && wait_char() <= 0) {}
    
    reset();
}

/**
 *  Tests to see if any zombie has collided with the hero. If so,
 *  do_collided is called to restart the simulation.
 */
void check_collision() {
    for (int i = 0; i < num_zombies; i++) {
        if (collided(hx, hy, HERO_WIDTH, HERO_HEIGHT, zx[i], zy[i], ZOMBIE_WIDTH, ZOMBIE_HEIGHT)) {
            do_collided();
            return;
        }
    }
}

/**
 *  A dialogue which allows the position and velocity of a designated zombie to
 *  be manually overridden.
 */
void do_zombie_cheat() {
    int zombie_id = get_int("Zombie Id?");
    int x = get_int("x?");
    int y = get_int("y?");
    int angle = get_int("degrees?");
    
    if (zombie_id >= 0 && zombie_id < num_zombies) {
        zx[zombie_id] = x;
        zy[zombie_id] = y;
        zdx[zombie_id] = ZOMBIE_SPEED * cos(angle * M_PI / 180);
        zdy[zombie_id] = ZOMBIE_SPEED * sin(angle * M_PI / 180);
    }
}

/**
 *  A dialogue which allows the position of the hero to be specified directly.
 */
void do_hero_cheat() {
    hx = get_int("Hero x?");
    hy = get_int("Hero y?");
}

/**
 *  Toggles zombie motion between paused and not-paused.
 */
void do_pause() {
    paused = !paused;
}

/**
 *  Dialogue which allows the loop delay to be manually overridden.
 */
void do_delay_cheat() {
    delay = get_int("New delay (milliseconds)?");
    
    if (delay < 0) delay = 10;
}

/**
 *  Dialog which displays a help screen.
 */
void do_help() {
    clear_screen();
    draw_formatted_lines(0, 0,
                         "!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "!!! ZombieDash Junior !!!\n"
                         "!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         "\n"
                         "a, s, d, w: Move hero left, down, right, up respectively\n"
                         "r: restart\n"
                         "t: time out for a user specified number of milliseconds\n"
                         "z: zombie cheat - override the position and direction of a zombie\n"
                         "h: hero cheat - override the position of the hero\n"
                         "p: pause or resume zombie movement\n"
                         "m: change the millisecond loop delay\n"
                         "q: quit\n"
                         "?: display this help screen\n\n\n"
                         "Press a key to return to the game."
                         );
    show_screen();
    
    while (get_char() > 0) {}
    while (!timed_out() && wait_char() <= 0) {}
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
    if (ch == 'r') {
        reset();
    }
    else if (is_hero_ctrl(ch)) {
        update_hero(ch);
    }
    else if (ch == 't') {
        do_timeout();
    }
    else if (ch == 'z') {
        do_zombie_cheat();
    }
    else if (ch == 'h') {
        do_hero_cheat();
    }
    else if (ch == 'p') {
        do_pause();
    }
    else if (ch == 'm') {
        do_delay_cheat();
    }
    else if (ch == '?') {
        do_help();
    }
}

/**
 *  The loop function: called once per time slice to run the simulation.
 */
void loop() {
    int ch = get_char();
    
    if (ch == 'q') {
        game_over = true;
        return;
    }
    else if (ch >= ' ') {
        do_operation(ch);
    }
    else if (!paused) {
        update_zombies();
    }
    
    check_collision();
    
    draw_all();
}

/**
 *  Program start and event loop.
 */
int main(void) {
    
    // srand(9139532); // For initial testing
    srand(get_current_time()); // For production
    
    setup_screen();
    setup();
    
    while (!game_over && !timed_out()) {
        loop();
        timer_pause(delay);
    }
    
    return 0;
}
