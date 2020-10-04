#pragma once


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <cab202_graphics.h>

/**
 *  Set vacuum's movable area based on room width and height
 *  Parameters: do not include wall's coordinates
 *      top -room's top y coordinate
 *      left -room's left x coordinate
 *      right -room's right x coordinate
 *      bottom -room's bottom y coordinate
 */
void set_movable_area(int top, int left, int right, int bottom);

/**
 * Set new vacuum's load weight
 * Parameter:
 *      weight (int):
 *      if the input is more than 65, the program will set 65g
 */
void set_collected_weight(int weight);

/**
 * Set battery time when simulation paused, so that when simulation is resumed,
 * the battery level remains same as before pause.
 */
void set_battery_time();

/**
 * Set new battery level for vacuum
 * Parameter:
 *      level (int): integer that you want to set as new battery level
 *      if the input is more than 100, the program will set 100%
 */
void set_battery_level(int level);

/**
 * Move robot to the specified position with angle
 * Parameters:
 *      x, y -robot's new location
 *      angle -new angle
 * if the x, y inputs are invalid, this command will be ignored
 */
void move_robot(int x, int y, int ang);

/**
 * Get vacuum's current battey level
 */
int get_battery_level();

/**
 * Get vacuum's current location
 * Parameter:
 *      type -'x' or 'y' that you want to get coordinate of vacuum
 *             if 'x' then this returns x coordinate of vacuum
 */
double get_robot_location(char type);

/**
 * Get vacuum's current heading
 */
int get_angle();

/**
 * Get vacuum's current load weight
 */
int get_collected_weight();

/**
 * Get vacuum and base size
 * Parameters:
 *      rs: 'r' for robot, 's' for charging base
 *      wh: 'w' for width, 'h' for height
 *      if you pass 'r', 'w', this function return vacuum's width
*/
int get_robot_size(char rs, char wh);

/**
 *  Return true if the vacuum is currently being charged
 */
bool get_charging_status();

/**
 * Force robot to return to charging base
 */
void return_to_base();

/**
 * Return true if vacuum's battery is dead (that is current_battely_leve == 0)
 */
bool is_battery_dead();

/**
 * Tells vacuum that charging process has finished
 */
void finish_charging();

/**
 * Updates vacuum to move new location
 */
void update_robot();

/**
 * Updates vacuum by command input
 * Parameter:
 *      ch: 'i', 'j', 'k' or 'l'
 */
void update_robot_manually(int ch);

/**
 * Draws vacuum
 */
void draw_robot();

/**
 * Draws charging station
 * Parameters:
 *      x -station's x (top left corner)
 *      y -station's y (top left corner)
 */
void draw_charge_station(double x, double y);

/**
 * Set up vacuum with initial location, heading, battery level and load weight
 * Parameters;
 *      x, y -x, y coordinate of vacuum (top left corner)
 *      ang -heading of vacuum
 *      bat -battery level
 *      wei -load weight
 */
void set_up_robot(double x, double y, int ang, int bat, int wei);

