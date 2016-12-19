/*
 * pathfinder.c
 *
 * Created: 04-12-2016 23:32:28
 * Author : anant_000
 */ 
/*
	All defined macros here
*/
#define F_CPU 14746500L
/*
	All imports here
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "lcd.h"
#include "analog_to_digital.h"
#include "bothoven_graph.h"
#include "dijkstras_algo.h"
#include "follow_line.h"
#include "mnp_positions.h"
#include "motor_movement.h"

int flag_to_restart = 0;
int curr_node_pos = 0, prev_node_pos 23, next_node_pos;
int num_turn_paths;
int successor_arr[NUM_GRAPH_NODES];

/*
	Start Definitions for buzzer
*/
void buzzer_pin_config (void)
{
	DDRC = DDRC | 0x08;		//Setting PORTC 3 as output
	PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

void buzzer_on (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore | 0x08;
	PORTC = port_restore;
}

void buzzer_off (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore & 0xF7;
	PORTC = port_restore;
}

/*
	End definitions for buzzer
*/


/*
	Start definitons for motion
*/



/*
	Start SHARP, proximity sensors
*/

//MOSFET switch port configuration
void MOSFET_switch_config (void)
{
 DDRH = DDRH | 0x0C; //make PORTH 3 and PORTH 1 pins as output
 PORTH = PORTH & 0xF3; //set PORTH 3 and PORTH 1 pins to 0

 DDRG = DDRG | 0x04; //make PORTG 2 pin as output
 PORTG = PORTG & 0xFB; //set PORTG 2 pin to 0
}

/*
	Initialize ports
*/
void port_init (void)
{
	lcd_port_config();
	buzzer_pin_config();
	motion_pin_config();
	adc_pin_config();
}

/*
	Initialize devices
*/


void init_graph(void){
	init_grid_of_nodes();
	add_both_nodeinfo();
}
void init_devices (void)
{
	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	init_graph();
	init_mnp();
	sei(); //Enables the global interrupts
}

void move_to(int j){
	if(sp_parent[j] == -1 || flag_to_restart == 1){
		return
	}else{
		successor_arr[sp_parent[j]] = j;
		move_to(sp_parent[j]);
		curr_node_pos = j;
		num_turn_paths = path_number(curr_node_pos, prev_node_pos, successor_arr[j]);
		turn_to_path(num_paths);
		if(!handle_obstacle(curr_node_pos, successor_arr[j], num_turn_paths)){
			follow_black_line();
			flag_to_restart = 0;
		}else{
			flag_to_restart = 1;
			return;
		}
	}
}


int main(void)
{    
    int mnp_array[] = {};
    int i = 0, j;
    int destination;
    init_devices();
    while(mnp_array[i] != -1){
    	destination = optimum_node_from_mnp(mnp_array[i]);
    	for(j = 0; j< NUM_GRAPH_NODES; j++)	successor_arr[j] = -1;
    	move_to(destination);
    	if(!flag_to_restart){
    		i++;
    		curr_node_pos = destination;
    	}else{
    		flag_to_restart = 0;
    		continue;
    	}
    }
    return 0;    	
}

