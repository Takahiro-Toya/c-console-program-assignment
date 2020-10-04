#pragma once


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <cab202_graphics.h>

/**
 * Set number of rubbish to drop on the floor
 * Parameters:
 *      type -rubbish type, 'd' for dust, 's' for slime, 't' for trash
 *      num -number of rubbish (specified by first parameter) to drop
 */
void set_num_rubbish(char type, int num);

/**
 * Drop trash at random location (maximum 5 pieces of trash)
 * Parameters:
 *      x, y -x, y coordinate of vacuum
 */
void drop_trash(double x, double y);

/**
 * Drop slime at random location (maximum 10 pieces of trash)
 * Parameters:
 *      x, y -x, y coordinate of vacuum
 */
void drop_slime(double x, double y);

/**
 * Drop dust at random location (maximum 1000 pieces of trash)
 * Parameters:
 *      x, y -x, y coordinate of vacuum
 */
void drop_dust(double x, double y);

/**
 * Draws all pieces of trash. This function is always called after drop_trash()
 */
void draw_trash();

/**
 * Draws all pieces of slime. This function is always called after drop_slime()
 */
void draw_slime();

/**
 * Draws all pieces of dust. This function is always called after drop_dust()
 */
void draw_dust();

/**
 * Update rubbish array after vacuum collecting the rubbish
 * Basically, delete an element from rubbish array after collection
 * Parameters:
 *      index: index of rubbish that vacuum will collect stored in array
 *      type: rubbish type 'd' for dust, 's' for slime, 't' for trash
 */
void update_rubbish_array(int index, char type);

/**
 * Return true if a piece of dust, slime or trash is detected.
 * In other words, return true if vacuum collides to rubbish
 * Parameters:
 *      type -rubbish type
 *      x, y -x, y coordinate of vacuum
 *      index: rubbbish's index
 */
bool is_rubbish_detected(char type, int x, int y, int index);

/**
 * Pick up a piece of rubbish: decrease the number of dust, slime or trash
 * that is stored in num_dust, num_slime or num_trash
 */
void pick_rubbish(char type);

/**
 * Register room width and height before dropping rubbish so that all pieces of rubbish
 * will be dropped within the room
 * Parameters: do not include wall's coordinates
 *      t, b -top and bottom y coordinates of room
 *      l, r -left and right x coordinates of room
 */
void register_room_for_rubbish(int t, int l, int r, int b);

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
                                  int r_w, int r_h, char s_p[], char r_p[]);

/**
 * Get rubbish weight; return weiht of dust or slime or trash
 * Parameters:
 *      type: 'd' for dust, 's' for slime and 't' for trash
 */
int get_rubbish_weight(char type);

/**
 * Get number of rubbish dropped on the floor
 * Parameters:
 *      type: 'd' for dust, 's' for slime and 't' for trash
 */
int get_num_rubbish(char type);

/**
 * Drop a piece of dust at specified position
 * Parameters
 *      x, y -location to drop a piece of dust
 */
void drop_a_piece_of_dust(int x, int y);

/**
 * Drop a piece of slime at specified position
 * Parameters
 *      x, y -location to drop a piece of slime (slime's left top)
 */
void drop_a_piece_of_slime(int x, int y);

/**
 * Drop a piece of trash at specified position
 * Parameters
 *      x, y -location to drop a piece of trash (trash's left top)
 */
void drop_a_piece_of_trash(int x, int y);



