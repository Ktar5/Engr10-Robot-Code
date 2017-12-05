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

    //Wait a little for the servo to slap 'dat beacon
    //Then raise the servo
    Wait(350);
    SetServo(2, raised);

    //Wait a little for the arm to thrust back up
    //Then refresh strength of IR signal
    Wait(350);
    Read_PD();

	//Check PD_sum against threshold value
	//Execute code in loop until PD_sum is < 5000 
	//PD_sum < 5000 signifies the beacon was turned off
    while(PD_sum > 5000){
        //If a beacon is on and very close,
        //Try to turn beacon off, again, and again, and again
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

    //Scan for a wall using our _FindNoWall(); method/function
    int dir = _FindNoWall();

    //Check if the direction is forward
    if(dir == 1){
    	//Move forward
        SetMotor(8, -70);
        SetMotor(9, -70);
        Wait(350);
        SetMotor(8, 127);
        SetMotor(9, -127);
    }else{ //Otherwise, move backward
    	//Move backward
        SetMotor(8, 70);
        SetMotor(9, 70);
        Wait(350);
        SetMotor(8, -127);
        SetMotor(9, 127);
    }

}

int _FindNoWall() {
    StartUltrasonic(6, 7);
    StartUltrasonic(1, 2);
    int ultrasonic[2];
    char snum[50];
    doMove = true;
    while (doMove == true) {
        SetMotor(8, 70);
        SetMotor(9, 70);
        Wait(500);
        SetMotor(8,0);
        SetMotor(9,0);

        ultrasonic[0] = -1;
        ultrasonic[1] = -1;
        //wait turn again
        ultrasonic[0] = GetUltrasonic(6, 7); //Back
        //Wait(10);
        ultrasonic[1] = GetUltrasonic(1, 2); // Front
        //Wait(10);
        while(ultrasonic[0] == -1 && ultrasonic[1] == -1){
            Wait(200);
            sprintf(snum, "Ultrasonic= Front: %d, Back: %d", (int) ultrasonic[1], (int) ultrasonic[0]);
            Print(snum);
        }
        if(ultrasonic[0] > 200 || ultrasonic[0] == 0){
            doMove = false;
            return 0;
        }else if(ultrasonic[1] > 200 || ultrasonic[1] == 0){
            doMove = false;
            return 1;
        }

        //Possibly store last value and check if there has been a sudden increase
        // in the value-- could prevent some stupid shit from happening
    }
    return 1;
}

void _MoveToBeacon(int frequency) {
    SetDigitalOutput(10, frequency);
    while (doMove == true) {
        Read_PD();
        find_max();
        _Move(); //If we stop, stop loop and continue
        //Stop immediately
        if(GetDigitalInput(3) == 0){
            doMove = false;
            SetMotor(8, 0);
            SetMotor(9, 0);
        }
    }
    doMove = true;
}

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

void Print(char string[]) {
    Wait(150);
    PrintToScreen("%s\n", string);
}
