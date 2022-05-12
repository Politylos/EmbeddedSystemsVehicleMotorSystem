Readme for motorLib package

 motorLib.h - Defines and Macros for driving motor for EGH456 motor kits.

 Setup:
       1. Please make sure that the pins for the 
          INHA, INLA, INHB, INLB, INHC, INLC, MODE or 
          ENABLE are not reconfigured
       
       2. Please include the following line to your RTOS.cfg file. 
       
		  Clock.timerId = 2;
          
          This line ensures the clock module doesn't use Timer0 
          which is currently used by the motorLib for PWM purposes
          
	   3. Please comment out or delete the PWM_config variable 
	      within the EK_TM4C1294XL.c file 
	   
		   //const PWM_Config PWM_config[] = {
		   // {
		   //     .fxnTablePtr = &PWMTiva_fxnTable,
		   //     .object = &pwmTivaObjects[0],
		   //     .hwAttrs = &pwmTivaHWAttrs[0]
		   // },
		   // {NULL, NULL, NULL}
		   // };
	   
		   This variable is used internally within the TI-RTOS drivers and 
		   is defined within the motorLib library. If not commented or 
		   deleted it will flag a compile error as that symbol is already 
		   defined in the motorLib
		   
		   Example compile error:
		   
		   "symbol "PWM_config" redefined: first defined in "./EK_TM4C1294XL.obj"; redefined in ".../motorlib.lib<motorlib.obj>""
		   
		4. Make sure you inlude motorLib.lib in your linker "File Search Path" settings. 
		   This is similar to including the grlib.lib file
	   

Usage:

      This library has been created simply to drive the lines of the motor safely.
      Firstly you will need to initialise the motor library by calling initMotor().
      
      e.g.   initMotorLib(50);      
     
      		 will initialise the pwm module and set the period of the PWM cycle to
      		 50 microseconds. This determines the frqeuency of the PWM (1/50e-6) = 20Khz
      		 It is recommended to have a PWM frequency > 10KhZ. Feel free to change this 
      		 period but this number determines the Max duty cycle that you can apply which
      		 determines the resolution of your pwm duty 
      		 
      		 	i.e. 50 pwm period = 0 to 50 steps for the duty value
      		 		 10 pwm period = 0 to 10 steps for the duty value
      
      Simply use the updateMotor() function to set the appropriate lines based on
      phase indicated by the hall effect sensors (to be determined by you). The
      Library uses the 6x PWM mode of the motor drive to drive the motor

      e.g.   updateMotor(0, 1, 0)

             Will set the motor lines for the motor phase corresponding to
             Hall effect sensor states - H3=0,H2=1,H1=1 and will load 10 into
             the PWM timer of the correct motor based on the phase.

      You should integrate the updateMotor function into your application. You will also need
      to read the Hall effect sensors via the GPIO lines. It is recommended that you use a 
      GPIO interrupt to sense when a hall effect line has changed.
      
      In order to get the motor to spin to a certain speed you will need to set 
      the Duty cycle of the PWM using the setDuty() function. This will only be applied the 
      next time you call updateMotor().
      
      e.g.   setDuty(25)
      
      	     will set the PWM duty to 25 microseconds (50% Duty Cycle if PWM Period = 50). 
      	     The maximum value is determined by the period of the PWM set from the initialisation 
      	     function. This function checks the maximum duty based on the PWM period and caps the input:
      	      
      	      i.e. if(duty > MAX_DUTY) duty = PWM_MAX_DUTY;
      
	  
Functions:	  
	  
	See motorlib.h for a description of the functions and their corresponding declarations