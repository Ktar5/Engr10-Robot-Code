// UserInclude.h : header file
#ifndef USERINCLUDE_H_
#define USERINCLUDE_H_

typedef enum { false, true } bool;

extern bool doMove;
extern int lowered, raised;

void _MoveToBeacon(int);
void _Move();
void _ControlRobot();
int _FindNoWall();
void Print(char[]);

#endif // USERINCLUDE_H_
