/* Team  11: Embedded System Lab Project
*  Member 1: Ekaterina Bukharova
*  Member 2: Ananya Reginald Frederick
*  Member 3: Adwait Bajpai
*/
#include <stdlib.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "adc.h"
#include "digital.h"
#include "motor.h"

static void blinky(void *pvParameters);
static void move_forward(void);
static void revert1(void);
static void revert2(void);
static void motor_movement(int8_t motor_zero, int8_t motor_one, int8_t motor_two);
static void move_backward(void);
static void Corner_detection(void);
static void target_orientation(void);
static void target_detection(void);
static void detect_target(void);
static void flag_Digital_Revert(void);
static void digital_sensor();
static void digital_sensor_init();
static void init_motor_values(void);

uint32_t count_tracking,count = 0u;
uint32_t ir_IR1,ir_IR2 = 0u;
int8_t motor_zero = 0u;
int8_t motor_one = 0u;
int8_t motor_two = 0u;
int8_t IR_Recv1[100], IR_Recv2[100];
int32_t count_tracking_aclk, count_tracking_clk = 0, target_left,target_right;
int8_t out_of_sight[10] = {1, 1, 1, 1, 1,1, 1, 1, 1, 1};
int8_t flag_DigitalL=0,flag_DigitalR=0;
uint32_t sum_ir1, sum_ir2;
uint32_t target_detected_left, target_detected_right;

typedef enum DM_DIR
{
    MOVE_STRAIGHT = 0, 
	CHANGE_POS11= 1,
	CHANGE_POS12= 3,
	revert1_POS1 = 2,
	revert2_POS2 = 4,
	CHECK_POS,
}DM_MOTORS_DIR;

int8_t target_ir_sensor = 0u;

DM_MOTORS_DIR assign_motor_direction = MOVE_STRAIGHT;

DM_MOTORS_DIR assign_motor_direction_previous = MOVE_STRAIGHT;

int main()
{
    dorobo_init();			//Call dorobo_init() function to initialize HAL, Clocks, Timers etc.
    digital_sensor_init();
	
	xTaskCreate(blinky, "BLINKYTASK", 512, NULL, 2, NULL);	//create blinky task

	vTaskStartScheduler();	//start the freertos scheduler

	return 0;				//should not be reached!
}

static void blinky(void *pvParameters) 
{
	trace_init();
	digital_init();
	motor_init();

	while (1) 
	{
		/*Distance IR sensor Logic*/
		/**************************/
		uint32_t digital_D = 0u;
		led_green_toggle();
		ir_IR1 = adc_get_value(DA_ADC_CHANNEL2);
		ir_IR2 = adc_get_value(DA_ADC_CHANNEL3);
		if((ir_IR2 > 700)&&(ir_IR1 > 700))
		{
		 	 init_motor_values();
		 	motor_movement(motor_zero,motor_one,motor_two);
			assign_motor_direction = CHECK_POS;
		}
		else if((ir_IR1 > 600)&&(ir_IR2<500 ))
		{
		 	 init_motor_values();
		 	motor_movement(motor_zero,motor_one,motor_two);
			assign_motor_direction = CHANGE_POS11;
		}
		else if((ir_IR2 > 500)&&(ir_IR1 < 400))
		{
		 	 init_motor_values();
		 	motor_movement(motor_zero,motor_one,motor_two);
			assign_motor_direction = CHANGE_POS12;
		}
		else
		{
			//do nothing
		}

		switch(assign_motor_direction){
		case MOVE_STRAIGHT:
			move_forward();
			motor_movement(motor_zero,motor_one,motor_two);
			break;
		case CHANGE_POS11:
			 	 init_motor_values();
			 	 do{
			 		ir_IR1 = adc_get_value(DA_ADC_CHANNEL2);
					count++;
					motor_one = 30;
					motor_two = -30;
					motor_movement(motor_zero,motor_one,motor_two);
			 	 }while(ir_IR1 >= 700);

					 do{
						 ir_IR1 = adc_get_value(DA_ADC_CHANNEL2);
						 move_forward();
						 motor_movement(motor_zero,motor_one,motor_two);

					 }while((ir_IR1 > 100)&&(ir_IR1 < 500));
					init_motor_values();
					if(ir_IR1 <100)
					assign_motor_direction = revert1_POS1;
			break;

		case CHANGE_POS12:
					 	 init_motor_values();
					 	 do{
					 		ir_IR2 = adc_get_value(DA_ADC_CHANNEL3);
							count++;
							motor_one = -30;
							motor_two = 30;
							motor_movement(motor_zero,motor_one,motor_two);
					 	 }
					 	 while(ir_IR2 >= 700);
							 do{
								 ir_IR2 = adc_get_value(DA_ADC_CHANNEL3);
								 move_forward();
								 motor_movement(motor_zero,motor_one,motor_two);
							 }while((ir_IR2 > 100)&&(ir_IR2 < 500));
							init_motor_values();
							if(ir_IR2 <100)
							assign_motor_direction = revert2_POS2;
					break;


		case revert1_POS1:
			revert1();
			break;
		case revert2_POS2:
			revert2();
			break;
		case CHECK_POS:
			Corner_detection();
			break;
		}
		/**************************/
		/*Digital sensor Logic*/
		digital_sensor();
		/**************************/
		/*Target detection Logic*/
		detect_target();
		/**************************/
		}
}

static void revert1(void)
{
	if(count > 0)
		{
			move_forward();
			motor_movement(motor_zero,motor_one,motor_two);
			vTaskDelay(350);
			init_motor_values();
			motor_one = -30;
			motor_two = 30;
			count = count*2;
			while(count != 0)
			{
				motor_movement(motor_zero,motor_one,motor_two);
				count--;
			}
			init_motor_values();
			assign_motor_direction = MOVE_STRAIGHT;
		}
}

static void revert2(void)
{
	if(count > 0)
		{
			move_forward();
			motor_movement(motor_zero,motor_one,motor_two);
			vTaskDelay(350);
			init_motor_values();
			motor_one = 30;
			motor_two = -30;
			count = count*2;
			while(count != 0)
			{
				motor_movement(motor_zero,motor_one,motor_two);
				count--;
			}
			init_motor_values();
			assign_motor_direction = MOVE_STRAIGHT;
		}
}

static void move_forward(void){
			motor_one = -60;
			motor_two = 0;
			motor_zero = 60;
}

static void motor_movement(int8_t motor_zero, int8_t motor_one, int8_t motor_two){

	motor_set(DM_MOTOR0, motor_zero);
	motor_set(DM_MOTOR1, motor_one);
	motor_set(DM_MOTOR2, motor_two);
}

static void init_motor_values(void){
	motor_zero = 0;
	motor_one = 0;
	motor_two = 0;
}

static void move_backward(void){
	motor_one = 50;
	motor_two = 0;
	motor_zero = -50;
}

/*
*		Left side	  |	    Right side             |        Perpendicular Obstacle
*                	  |                            |
*  |\                 |                 /|         |  
*  |  \               |               /  |         |   
*  |    \ (obstacle)  |             /    |         |   ____________________________ (obstacle)
*  |   _  \           |           /  _   | (Wall)  |			    _
*  |  |_|   \         |         /   |_|  |         | 			   |_|
*  | (bot)    \       |       /    (bot) |         |			  (bot)
*  |            \     |     /            |         |
*                     |                            |
*/
static void Corner_detection(void)
{
	//setting count 0 so that it doesnt interfere with turns
	count=0;
	//moves back
	move_backward();
	motor_movement(motor_zero, motor_one,motor_two);
	vTaskDelay(100);
	init_motor_values();
	motor_movement(motor_zero, motor_one,motor_two);
	//moves anticlockwise in both cases
	motor_one = 30;
	motor_two = -30;
	motor_movement(motor_zero,motor_one,motor_two);
	vTaskDelay(200);
	init_motor_values();
	motor_movement(motor_zero, motor_one,motor_two);
	//moves forward to check the ir sensors
	move_forward();
	motor_movement(motor_zero,motor_one,motor_two);
	vTaskDelay(150);
	init_motor_values();
	motor_movement(motor_zero, motor_one,motor_two);
	vTaskDelay(100);
	ir_IR1 = adc_get_value(DA_ADC_CHANNEL2);
	ir_IR2 = adc_get_value(DA_ADC_CHANNEL3);
	if((ir_IR1>500)&&(ir_IR2>500))
	{
		move_backward();
		motor_movement(motor_zero, motor_one,motor_two);
		vTaskDelay(500);
		init_motor_values();
		motor_movement(motor_zero, motor_one,motor_two);
		motor_one = -30;
		motor_two = 30;
		motor_movement(motor_zero,motor_one,motor_two);
		vTaskDelay(300);
		init_motor_values();
		motor_movement(motor_zero, motor_one,motor_two);
	}
	else
	{
		init_motor_values();
		motor_movement(motor_zero, motor_one,motor_two);
		//Rotate anti clockwissw
		motor_one = 30;
		motor_two = -30;
		motor_movement(motor_zero,motor_one,motor_two);
		vTaskDelay(200);
		init_motor_values();
		motor_movement(motor_zero, motor_one,motor_two);
		//move forward
		move_forward();
		motor_movement(motor_zero, motor_one,motor_two);
		vTaskDelay(200);
		init_motor_values();
		motor_movement(motor_zero,motor_one,motor_two);
		//re orient
		motor_one = -30;
		motor_two = 30;
		motor_movement(motor_zero,motor_one,motor_two);
		vTaskDelay(250);
		init_motor_values();
		motor_movement(motor_zero,motor_one,motor_two);
	}
	assign_motor_direction = MOVE_STRAIGHT;

}
// function which implements target detection logic
static void target_orientation(void)
{
	int32_t count_local = 0;
	int32_t resultant_direction = 0;
	count_tracking_clk = 0;
	count_tracking_aclk = 0;
	target_detection();
	init_motor_values();
	motor_movement(motor_zero,motor_one,motor_two);
	//	determine clockwise angle
	vTaskDelay(5);

	do
	{
		vTaskDelay(100);		
		target_right = 0;
		target_left = 0;
		motor_one = -30;
		motor_two = 30;
		motor_movement(motor_zero,motor_one,motor_two);
		target_detection();
		
		count_tracking_clk++;
	}while(target_left <= 9 );
	count_local = count_tracking_clk;

	init_motor_values();
	motor_movement(motor_zero,motor_one,motor_two);

//		Reorient
	while(count_local != 0)
	{
		target_detection();
		
		motor_one = 30;
		motor_two = -30;
		motor_movement(motor_zero,motor_one,motor_two);
		
		count_local--;
		target_detection();
	}
	init_motor_values();
	motor_movement(motor_zero,motor_one,motor_two);	
	do
	{
		vTaskDelay(100);
		
		motor_one = 30;
		motor_two = -30;
		motor_movement(motor_zero,motor_one,motor_two);
		
		count_tracking_aclk++;
		target_detection();
	}while(target_right <= 9);
	count_local = count_tracking_aclk;
	
//		Reorient
	while(count_local != 0)
	{
		target_detection();
		
		init_motor_values();
		motor_one = -30;
		motor_two = 30;
		motor_movement(motor_zero,motor_one,motor_two);
		
		count_local--;
		target_detection();
	}
	resultant_direction = count_tracking_aclk - count_tracking_clk;
	init_motor_values();
	motor_movement(motor_zero,motor_one,motor_two);

	
	if(resultant_direction > 0)
	{
		// move anti clockwise
		do
		{
			target_detection();
	
			motor_one = 30;
			motor_two = -30;
			motor_movement(motor_zero,motor_one,motor_two);
			resultant_direction--;
	
			target_detection();
		}while(resultant_direction >= 0);
	}
	else
	{
		// move clockwise
		do
		{
			target_detection();
			motor_one = -30;
			motor_two = 30;
			motor_movement(motor_zero,motor_one,motor_two);
			resultant_direction++;

			target_detection();
		}while(resultant_direction <= 0);
	}


	init_motor_values();
	motor_movement(motor_zero,motor_one,motor_two);
	assign_motor_direction = MOVE_STRAIGHT;
	count_tracking_clk = 0;
	count_tracking_aclk = 0;
	move_forward();
	motor_movement(motor_zero,motor_one,motor_two);
	vTaskDelay(200);

}
// function which implements alternate target detection logic
static void target_detection()
{
	int8_t i = 0 ;
	target_right = 0;
	target_left = 0;
    for(i=0; i<10 ;i++)
    {
	IR_Recv1[i] = digital_get_pin(DD_PIN_PF10); // left IR sensor
	tracef("\n Current value of array PF10 %d ",IR_Recv1[i]);
	IR_Recv2[i] = digital_get_pin(DD_PIN_PB3); // right IR sensor
	tracef("\n Current value of array PB3 %d ",IR_Recv2[i]);
    if(IR_Recv1[i] == out_of_sight[i])
    {
    	target_left++;
    }
    if(IR_Recv2[i] == out_of_sight[i])
    {
    	target_right++;
    }
    }
}

static void digital_sensor_init()
{
	digital_configure_pin(DD_PIN_PE1, DD_CFG_INPUT_PULLUP);
	digital_configure_pin(DD_PIN_PE0, DD_CFG_INPUT_PULLUP);
}
static void digital_sensor()
{
	//Calculation to see if the bot is having any of the digital sensor pressed
	int8_t switch_left[5], switch_right[5],sum_r=0,sum_l=0;
	for(int i=0;i<5;i++)
	{
		switch_left[i]=digital_get_pin(DD_PIN_PE0);
		switch_right[i]=digital_get_pin(DD_PIN_PE1);
	}
	for(int i=0;i<5;i++)
	{
		sum_l=sum_l+switch_left[i];
		sum_r=sum_r+switch_right[i];
	}
	// Controlling the bot in case any or both the switches are pressed
	if((sum_r<2)&&(sum_l<2))
	{
		count=0;
			//moves back
			move_backward();
			motor_movement(motor_zero, motor_one,motor_two);
			vTaskDelay(100);
			// init motor values
			init_motor_values();
			motor_movement(motor_zero, motor_one,motor_two);
			//moves anticlockwise in both cases
			motor_one = 30;
			motor_two = -30;
			motor_movement(motor_zero,motor_one,motor_two);
			vTaskDelay(250);
			// init motor values
			init_motor_values();
			motor_movement(motor_zero, motor_one,motor_two);
			//moves forward to check the ir sensors
			move_forward();
			motor_movement(motor_zero,motor_one,motor_two);
			vTaskDelay(150);
			init_motor_values();
			motor_movement(motor_zero, motor_one,motor_two);
			vTaskDelay(100);
			ir_IR1 = adc_get_value(DA_ADC_CHANNEL2);
			ir_IR2 = adc_get_value(DA_ADC_CHANNEL3);
			if((ir_IR1>400)&&(ir_IR2>400))
			{
				move_backward();
				motor_movement(motor_zero, motor_one,motor_two);
				vTaskDelay(500);
				init_motor_values();
				motor_movement(motor_zero, motor_one,motor_two);
				motor_one = -30;
				motor_two = 30;
				motor_movement(motor_zero,motor_one,motor_two);
				vTaskDelay(350);
				init_motor_values();
				motor_movement(motor_zero, motor_one,motor_two);
			}
			else
			{
				init_motor_values();
				motor_movement(motor_zero, motor_one,motor_two);
				motor_one = 30;
				motor_two = -30;
				motor_movement(motor_zero,motor_one,motor_two);
				vTaskDelay(200);
				init_motor_values();
				motor_movement(motor_zero, motor_one,motor_two);
				move_forward();
				motor_movement(motor_zero, motor_one,motor_two);
				vTaskDelay(200);
				init_motor_values();
				motor_movement(motor_zero,motor_one,motor_two);
				motor_one = -30;
				motor_two = 30;
				motor_movement(motor_zero,motor_one,motor_two);
				vTaskDelay(250);
				init_motor_values();
				motor_movement(motor_zero,motor_one,motor_two);
			}
			assign_motor_direction = MOVE_STRAIGHT;
	}
	else if(sum_l<2)
	{
		move_backward();
		motor_movement(motor_zero, motor_one, motor_two);
		vTaskDelay(100);
		init_motor_values();
		motor_movement(motor_zero,motor_one,motor_two);
		motor_one = -50;
		motor_two = 50;
		motor_movement(motor_zero,motor_one,motor_two);
		vTaskDelay(150);
		init_motor_values();
		motor_movement(motor_zero,motor_one,motor_two);
		
	}
	else if(sum_r<2)
	{
		move_backward();
		motor_movement(motor_zero, motor_one, motor_two);
		vTaskDelay(100);
		init_motor_values();
		motor_movement(motor_zero,motor_one,motor_two);
		motor_one = 50;
		motor_two = -50;
		motor_movement(motor_zero,motor_one,motor_two);
		vTaskDelay(150);
		init_motor_values();
		motor_movement(motor_zero,motor_one,motor_two);
	}
	else{
		// do nothing
	}
}

// function which implements alternate target detection logic
static void detect_target(){

	target_detected_left = 0;
	target_detected_right = 0;


		for(int8_t i=0; i<10 ;i++)
	    {
		IR_Recv1[i] = digital_get_pin(DD_PIN_PF10); // right IR sensor
		IR_Recv2[i] = digital_get_pin(DD_PIN_PB3); // left IR sensor

	    if(IR_Recv1[i] == out_of_sight[i])
	    {
	    	target_detected_left++;
	    }
	    if(IR_Recv2[i] == out_of_sight[i])
	    {
	    	target_detected_right++;
	    }
	    }

	    if((target_detected_left <6) || (target_detected_right <6))
	    {
	    	target_orientation();
	    }
}
