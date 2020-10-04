#!/bin/bash

test_num=0
REPORT_FILE=test_report.txt

echo "Remember to do these two commands before running this script:
export COLUMNS
export LINES
"
echo "Test run starting at $(date)" >>${REPORT_FILE}

#==============================================================================
#	Operations available in the Vacuum Cleaner simulation.
#==============================================================================

function move_vacuum() {
    printf "v${1},${2},${3} "
}

function set_timeout () {
    printf "o${1}"
}

function change_vacuum_weight() {
    printf "w${1} "
}

function change_vacuum_battery() {
    printf "y${1} "
}

function move_vacuum_to_base() {
    printf "b "
}

function move_vacuum_north() {
    printf "i "
}

function move_vacuum_west() {
    printf "j "
}

function move_vacuum_south() {
    printf "k "
}

function move_vacuum_east() {
    printf "l "
}

function drop_dust(){
    printf "d${1},${2} "
}

function drop_slime(){
    printf "s${1},${2} "
}

function drop_trash(){
    printf "t${1},${2} "
}

function pause_resume() {
    printf "p "
}

function quit() {
    printf "q "
}

function reset() {
    printf "r${1},${2},${3} "
}

function show_menu() {
    printf "? "
}

function time_out() {
    printf "o${1} "
}

function change_delay(){
    printf "m${1} "
}

function initial_setup() {
    printf "${1},${2},${3} "
}


function loop () {
	if (( $# >= 1 )) && (( $1 > 0 )) 
	then
		for (( i = 0; i < $1; i++ ))
		do
			printf "!"
		done
	else
		printf "!"
	fi
}

function enter () {
	printf "${1} "
}

#========================================================================
#	Run a single test.
#========================================================================	

function run_test () {
	(( test_num ++ ))
	echo "Test    : ${test_num}"
	echo "Category: ${category}"
	echo "Details : ${details}"
	echo "Expect  : ${expect}"
	
	read -p "Press r to run the test or s to skip..." run_or_skip
	
	if [ "${run_or_skip}" == "r" ]
	then
		echo ${cmd} | ./main
		read -p "Did the test pass or fail (p/f)? " result
		echo "Test ${test_num}: ${result}" >>${REPORT_FILE}
	fi
}


#==============================================================================
category="Initial set up"
details="Vacuum alone in the center of room."
expect="Vacuum is displayed in centre; no rubbish is visible. Status bar shows: student ID 'n10056513' , heading '90', battery '100%' timer, load weight '0', rubbish left '0, 0, 0'"

cmd=$(
    initial_setup 0 0 0
    set_timeout 5
    pause_resume
)

run_test 
#==============================================================================
category="Initial setup"
details="Vacuum and some rubbish"
expect="Vacuum is displayed in centre; 10 dust, 10 slime, & 5 trash visible at the random location. Rubbish is dropped within the room, and not overwrapping vacuum or base Status bar shows: student ID 'n10056513' , heading '90', battery '100%', timer, load weight '0', rubbish left '10, 10, 5'"

cmd=$(
    initial_setup 10 10 5
	set_timeout 10
)

run_test
#==============================================================================
category="Initial setup"
details="Vacuum and maximum rubbish"
expect="Vacuum is displayed in centre; 1000 dust, 10 slime, & 5 trash visible at the random location. Rubbish is dropped within the room, and not overwrapping vacuum or base. Status bar shows: student ID 'n10056513' , heading '90', battery '100%', timer, load weight '0', rubbish left '1000, 10, 5'"

cmd=$(
    initial_setup 1000 10 5
    set_timeout 10
)

run_test
#==============================================================================
category="Dropping rubbish"
details="Dropping some pieces of dust without overwrapping any object"
expect="dropping dust between vacuum and bottom"

(( BOTTOM = LINES - 4, CCENTER = COLUMNS / 2, LCENTER = LINES / 2 ))


cmd=$(
    initial_setup 0 0 0
    set_timeout 5
    for (( i = LCENTER; i < BOTTOM; i += 2 )) ; do drop_dust ${CCENTER} ${i}; done

)

run_test
#==============================================================================
category="Dropping rubbish"
details="Dropping some pieces of slime without overwrapping any object"
expect="dropping slime between vacuum and bottom"

(( X = 10, TOP = 5, BOTTOM = LINES - 4 ))

cmd=$(
initial_setup 0 0 0
for (( i = TOP; i < BOTTOM; i += 6 )) ; do drop_slime ${X} ${i}; done
set_timeout 5
)

run_test
#==============================================================================
category="Dropping rubbish"
details="Dropping some pieces of trash without overwrapping any object"
expect="dropping trash between vacuum and bottom"

(( X = 10, TOP = 5, BOTTOM = LINES - 4 ))

cmd=$(
initial_setup 0 0 0
for (( i = TOP; i < BOTTOM; i += 7 )) ; do drop_trash ${X} ${i}; done
set_timeout 5
)

run_test
#==============================================================================
category="Dropping rubbish"
details="2 pieces of trash at the same location. "
expect="You will only see one trash, but Status bar shows '0, 0, 2'. This is correct behaviour"

(( CCENTER = COLUMNS / 2, LCENTER = LINES / 2, Y = LCENTER + 8))

cmd=$(
    initial_setup 0 0 0
    drop_trash ${CCENTER} ${Y}
    drop_trash ${CCENTER} ${Y}
    set_timeout 5
)

run_test
#==============================================================================
category="Dropping rubbish"
details="Dropping rubbish under vacuum"
expect="1 trash is dropped under vacuum cleaner: Status bar '0, 0, 1'"

(( CCENTER = COLUMNS / 2 - 2, LCENTER = LINES / 2 - 2))

cmd=$(
    initial_setup 0 0 0
    drop_trash ${CCENTER} ${LCENTER}
    set_timeout 5
)

run_test
#==============================================================================
category="Dropping rubbish"
details="Dropping rubbish fail. drops 1 slime outside the room."
expect="Program ignores input. So output: No rubbish visible & status bar is '0, 0, 0'"

(( X = COLUMNS + 2, Y = 0))
cmd=$(
    initial_setup 0 0 0
    drop_slime ${X} ${Y}
    set_timeout 5
)

run_test

#==============================================================================
category="Dropping rubbish"
details="Dropping rubbish fail. Drops 1 slime over the charging base"
expect="Program ignores input. So output: No rubbish visible & status bar is '0, 0, 0'"

(( CCENTER = COLUMNS / 2, Y = 6))

cmd=$(
    initial_setup 0 0 0
    drop_slime ${CCENTER} ${Y}
    set_timeout 5
)

run_test
#==============================================================================
category="Dropping rubbish"
details="Dropping rubbish fail. Drops a piece of rubbish when maximum number of rubbish is already spread"
expect="Program ignores input. So output: No rubbish visible & status bar is '1000, 10, 5'"

(( CCENTER = COLUMNS / 2, Y = 6))

cmd=$(
    initial_setup 1000 10 5
    drop_trash ${CCENTER} ${Y}
    drop_slime 1 10
    drop_dust 1 30
    set_timeout 5
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collecting rubbish"
expect="5 pieces of dust making a line below the vacuum, then the vacuum runs to collect them. After collecting, the statuse bar (row 2 column 3) will be '0, 0 ,0. Also, the status bar (row 2 column 2) will show '5g'."

(( CCENTER = COLUMNS / 2, Y = LINES / 2 + 10 ))

cmd=$(
    initial_setup 0 0 0
    for (( i = Y; i < Y + 10; i += 2 )) ; do drop_dust ${CCENTER} ${i}; done
    loop 100
    pause_resume
    set_timeout 5
)

run_test

#==============================================================================
category="Vacuum behaviour"
details="Collecting rubbish"
expect="3 pieces of slime making a line below the vacuum, then runs the vacuum to collect them. After collecting, the statuse bar (row 2 column 3) will be '0, 0 ,0. Also, the status bar (row 2 column 2) will show '15g'"

(( CCENTER = COLUMNS / 2 - 2, Y = LINES / 2 + 5 ))

cmd=$(
    initial_setup 0 0 0
    for (( i = Y; i <= Y + 12; i += 6 )) ; do drop_slime ${CCENTER} ${i}; done
    loop 100
    pause_resume
    set_timeout 5
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collecting rubbish"
expect="a piece of trash below the vacuum, then runs the vacuum to collect them. After collecting, the statuse bar (row 2 column 3) will be '0, 0 ,0. Also, the status bar (row 2 column 2) will show '15g'."

(( X = COLUMNS / 2 - 2, Y = LINES / 2 + 5))

cmd=$(
    initial_setup 0 0 0
    drop_trash ${X} ${Y};
    loop 100
    pause_resume
    set_timeout 5
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collecting overwrapped rubbish"
expect="You will see only a piece of slime on the screen, but robot will collect two pieces of slime. Status bar: row 2 column 2 -'10g', row 2 column 3 -'0,0,0'"

(( X = COLUMNS / 2 - 2, LCENTER = LINES / 2, Y = LCENTER + 8))

cmd=$(
initial_setup 0 0 0
drop_slime ${X} ${Y}
drop_slime ${X} ${Y}
loop 100
pause_resume
set_timeout 5
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collecting rubbish fail"
expect="drop 1 dust at the left corner but vacuum can not collect"

(( DX = 1, DY = 5, X = 1, Y = 25 ))

cmd=$(
initial_setup 0 0 0
drop_dust ${DX} ${DY}
move_vacuum ${X} ${Y} 270
for (( i = 0; i <= 20; i++ )) ; do move_vacuum_north; done
set_timeout 5
)

run_test

#==============================================================================
category="Vacuum behaviour"
details="Return to base when load weight over 45g"
expect="Vacuum goes back to base after collecting 2 pieces of dust"

(( CCENTER = COLUMNS / 2, Y = LINES / 2 + 10, YY = Y + 10))

cmd=$(
initial_setup 0 0 0
drop_dust ${CCENTER} ${Y}
drop_dust ${CCENTER} ${YY}
change_vacuum_weight 44
loop 100
pause_resume
set_timeout 8
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Return to base when battery below 25"
expect="Initially battery level is set 27 %, so after apporoximately 2 seconds, vacuum goes back to base"

(( X = 1, Y = 5))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 45
change_vacuum_battery 27
pause_resume
set_timeout 10
)

run_test

#==============================================================================
category="Vacuum behaviour"
details="Vacuum does not correct rubbish if the battery level is under 25%"
expect="vacuum goes back to its base without collecting rubbish even though the load is 0"

(( X = 1, Y = 5))

cmd=$(
initial_setup 1000 10 5
change_vacuum_battery 24
move_vacuum ${X} ${Y} 45
pause_resume
set_timeout 10
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Vacuum does not correct rubbish if it is in return to base mode"
expect="vacuum goes back to its base without collecting rubbish even though the load is 0 and battery is over 25%"

(( X = 1, Y = 5))

cmd=$(
initial_setup 1000 10 5
move_vacuum ${X} ${Y} 45
move_vacuum_to_base
pause_resume
set_timeout 10
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="behaviour during Charging"
expect="Battery will get charged to 100%, during this time, word 'Docked!' is displayed at command input area"

(( X = COLUMNS / 2 + 30, Y = LINES - 20))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y}
change_vacuum_battery 10
pause_resume
set_timeout 10
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collision to wall"
expect="vacuum bounce off the top wall & set new direction"

(( X = 5, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 270
pause_resume
set_timeout 5
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collision to wall"
expect="vacuum bounce off the left wall & set new direction"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 180
change_delay 5
pause_resume
set_timeout 5
)

run_test

#==============================================================================
category="Vacuum behaviour"
details="Collision to wall"
expect="vacuum bounce off the right wall & set new direction"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 0
change_delay 5
pause_resume
set_timeout 5
)

run_test
#==============================================================================
category="Vacuum behaviour"
details="Collision to wall"
expect="vacuum bounce off the bottom wall & set new direction"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 90
change_delay 5
pause_resume
set_timeout 4
)

run_test

#==============================================================================
category="Vacuum behaviour"
details="Collision to charging staton"
expect="vacuum bounce off the charging station & set new direction"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 270
change_delay 5
pause_resume
set_timeout 3
)

run_test
#==============================================================================
category="Vacuum cheat"
details="Moving device to top left"
expect="Vacuum appears at the left top, and pause there"

(( LEFT = 1, TOP = 5))

cmd=$(
initial_setup 0 0 0
pause_resume
loop 100
move_vacuum ${LEFT} ${TOP} 34
pause_resume
set_timeout 7
)

run_test
#==============================================================================
category="Vacuum cheat"
details="Moving device with angle 1000 degrees"
expect="Heading will be calculated, and be '280', and pause there"

(( X = COLUMNS / 2, Y = LINES / 2 ))

cmd=$(
initial_setup 0 0 0
pause_resume
loop 100
move_vacuum ${X} ${Y} 1000
pause_resume
set_timeout 7
)

run_test


#==============================================================================
category="Vacuum cheat"
details="Moving device fail"
expect="Trying to move vacuum outside the room. Program ignores this input and nothing will have changed (just simply going straight down to south as default)"

(( X = COLUMNS + 100, Y = LINES + 100))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 225
pause_resume
loop 100
set_timeout 5
)

run_test
#==============================================================================
category="Vacuum cheat"
details="Moving device fail"
expect="Trying to move vacuum over the base. Program ignores this and nothing will have changed"

(( X = COLUMNS / 2, Y = 7))

cmd=$(
initial_setup 0 0 0
pause_resume
loop 100
move_vacuum ${CCENTER} ${Y} 90
set_timeout 5
)

run_test
#==============================================================================
category="Vacuum cheat"
details="change battery level"
expect="battery level starts from 50%, and pause"

cmd=$(
initial_setup 0 0 0
pause_resume
loop 100
change_vacuum_battery 50
pause_resume
set_timeout 6
)

run_test
#==============================================================================
category="Vacuum cheat"
details="Change battery level to 24%, so it needs to go to base"
expect="battery level will be set to 24% after 3 seconds, then goes to base."

(( X = COLUMNS / 2 + 10, Y = LINES / 2 + 30))

cmd=$(
initial_setup 0 0 0
change_delay 10
move_vacuum ${X} ${Y} 45
pause_resume
loop 100
change_vacuum_battery 24
set_timeout 7
)

run_test


#==============================================================================
category="Vacuum behaviour"
details="Return to base by'b' command"
expect="vacuum goes back to base after lurching a bit"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 180
change_delay 10
pause_resume
loop 100
move_vacuum_to_base
set_timeout 4
)

run_test

#==============================================================================
category="Vacuum cheat"
details="change load weight"
expect="after changing load weight to 50 vacuum should head base immediately."

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
change_delay 10
move_vacuum ${X} ${Y} 45
pause_resume
loop 100
change_vacuum_weight 50
set_timeout 7
)

run_test

#==============================================================================
category="Vacuum manual operation"
details="moves vacuum to north"
expect="moves vacuum to north until it reaches charging base"

(( Y = LINES / 2 ))

cmd=$(
initial_setup 0 0 0
for (( i = 0; i < Y - 7; i++ )) ; do move_vacuum_north ; done
set_timeout 3
)

run_test
#==============================================================================
category="Vacuum manual operation"
details="moves vacuum to west"
expect="moves vacuum to west until it reaches wall"

(( X = COLUMNS / 2 ))

cmd=$(
initial_setup 0 0 0
for (( i = 0; i < X - 5; i++ )) ; do move_vacuum_west ; done
set_timeout 8
)

run_test
#==============================================================================
category="Vacuum manual operation"
details="moves vacuum to south"
expect="moves vacuum to south until it reaches wall"

(( Y = LINES / 2 ))

cmd=$(
initial_setup 0 0 0
for (( i = 0; i < LINES - Y - 7; i++ )) ; do move_vacuum_south ; done
set_timeout 3
)

run_test
#==============================================================================
category="Vacuum manual operation"
details="moves vacuum to east"
expect="moves vacuum to east until it reaches wall"

(( X = COLUMNS / 2 ))

cmd=$(
initial_setup 0 0 0
for (( i = 0; i < X - 5; i++ )) ; do move_vacuum_east ; done
set_timeout 8
)

run_test

#==============================================================================
category="Terminate simulation"
details="quit simulation when pressed 'q'"
expect="vacuum lurches a bit, and simulation automatically stops"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
change_delay 10
move_vacuum ${X} ${Y} 290
pause_resume
loop 100
quit
)
run_test

#==============================================================================
category="Terminate simulation"
details="reset simulation when pressed 'r'"
expect="vacuum lurches a bit with maximum rubbish, and simulation resets with no rubbish."

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 1000 10 5
move_vacuum ${X} ${Y} 290
pause_resume
loop 150
reset 0 0 0
set_timeout 3
)
run_test


#==============================================================================
category="Initial set up"
details="Trying to drop more than 1200 dust, 20 slime and 10 trash"
expect="You will only see 1000 dust, 10 slime and 5 trash"


cmd=$(
initial_setup 1200 20 10
set_timeout 10
)
run_test

#==============================================================================
category="Pause"
details="pause simulation"
expect="vacuum lurches a bit, pause for a while: battery level will not be changed after resumed"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 120
pause_resume
loop 100
pause_resume
loop 200
pause_resume
loop 100
#set_timeout 15
quit
)
run_test
#==============================================================================
category="Dealy"
details="Change delay"
expect="vacuum lurches a bit, change delay to make it quicker, and change dealy to make it slower"

(( X = COLUMNS / 2, Y = LINES / 2))

cmd=$(
initial_setup 0 0 0
move_vacuum ${X} ${Y} 290
pause_resume
loop 100
change_delay 5
loop 500
change_delay 300
loop 50
quit
)
run_test






