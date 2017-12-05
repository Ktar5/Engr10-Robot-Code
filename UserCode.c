// EasycFunctions.c : implementation file
#include "Main.h"

bool doMove = true;
int lowered = -127, raised = 0;

void _ControlRobot(){
    //Raise the servo motor controlling the arm
    SetServo(2, raised);

    //Move to **RED** beacon until we know the
    //Beacon is in front of us
    _MoveToBeacon(0);

    //Turn off beacon by lowering the arm
    SetServo(2, lowered);

    //Wait a little for the servo to slap 'dat
    //Beacon. Then raise the servo
    Wait(350);
    SetServo(2, raised);

    //Wait a little for the arm to thrust back
    //Up. Then refresh strength of IR signal
    Wait(350);
    Read_PD();

    //Check PD_sum against threshold value
    //Execute code in loop until PD_sum is < 5000 
    //PD_sum < 5000 signifies the beacon was
    //turned off
    while(PD_sum > 5000){
        //If a beacon is on and very close,
        //Try to turn beacon off, again, and again
        SetServo(2, lowered); //
        Wait(450);
        SetServo(2, raised);
        Wait(450);
        Read_PD(); //Recheck
    }

    //Reverse away from the beacon for .4 seconds
    //In order to release it from our arm then stop
    SetMotor(8, -127);
    SetMotor(9, 127);
    Wait(400);
    SetMotor(8, 0);
    SetMotor(9, 0);

    //Move to **GREEN** beacon using same method
    //As above for the red beacon, but different frequency
    _MoveToBeacon(1);

    //Lower the arm so that our ultrasonic sensor is pointed
    //Forward and not up (its on our arm)
    SetServo(2, lowered);

    //Scan for a wall using our 
    //_FindNoWall(); method/function
    int dir = _FindNoWall();

    //Check if the direction is forward
    if(dir == 1){
        SetMotor(8, -70);
        SetMotor(9, -70);
        Wait(350);
        SetMotor(8, 127);
        SetMotor(9, -127);
    }else{ //Otherwise, move backward
        SetMotor(8, 70);
        SetMotor(9, 70);
        Wait(350);
        SetMotor(8, -127);
        SetMotor(9, 127);
    }

}

int _FindNoWall() {
	//Start up our ultrasonic sensors
    StartUltrasonic(6, 7); //Butt sensor
    StartUltrasonic(1, 2); //Forward sensor
    //Create an array to store the output of sensors
    int ultrasonic[2];
    //Allow us to use 'doMove = false' to exit while loop
    doMove = true;
    while (doMove == true) { // While doMove is set to true
    	//Rotate the robot slightly
        SetMotor(8, 70);
        SetMotor(9, 70);
        Wait(500);
        SetMotor(8,0);
        SetMotor(9,0);

        //Default our ultrasonic data to -1
        //Explanation later
        ultrasonic[0] = -1;
        ultrasonic[1] = -1;

        ultrasonic[0] = GetUltrasonic(6, 7); //index 0 = butt
        ultrasonic[1] = GetUltrasonic(1, 2); //index 1 = forward

        //Ultrasonic processing is synchronous, and will throw errors when
        //It cannot get a bounce back from any object (no object in range)
        //However, it still runs, and will still return 0-- but
        //After an inconsistent amount of time. Thus, we wait
        //Until our variables get assigned from their default values
        while(ultrasonic[0] == -1 && ultrasonic[1] == -1){
            Wait(200);
        }

        //If butt sensor is 0 or has far away wall, return '0'
        if(ultrasonic[0] > 200 || ultrasonic[0] == 0){
            doMove = false;
            return 0;
        }//Otherwise, if forward sensor is 0 or has far away wall, return '1'
        else if(ultrasonic[1] > 200 || ultrasonic[1] == 0){
            doMove = false;
            return 1;
        }
    }
    return 1;
}

void _MoveToBeacon(int frequency) {
    SetDigitalOutput(10, frequency);
    while (doMove == true) {
        Read_PD();
        find_max();
        _Move();
        //Stop immediately if limit switch is touched
        if(GetDigitalInput(3) == 0){
            doMove = false;
            SetMotor(8, 0);
            SetMotor(9, 0);
        }
    }
    //Reset doMove now that we're out of the loop
    doMove = true;
}

//Custom move function that uses our "doMove" variable to instantly
//Stop any sort of movement and give us some faster times :)
//Only thing added was "doMove = ____" statements
void _Move(){
    int temp, error, steer, speed;
    error=4-max_no;
    steer=error*steer_sensitivity;
    speed=forward_speed;
    if(PD_sum<ambient_level){
        speed = 0;
        steer= -spin_speed;
        doMove= true;
    }
    if(PD_sum>slow_level){
        speed = slow_speed;
        doMove = true;
    }
    if(PD_sum>stop_level){
        speed= 0;
        steer = 0;
        doMove = false;
    }
    temp = limit_pwm(0+steer+speed);
    SetMotor(8, temp);
    temp= limit_pwm(0+steer-speed);
    SetMotor(9, temp);
}

//Print things to the screen
void Print(char string[]) {
    Wait(150);
    PrintToScreen("%s\n", string);
}
