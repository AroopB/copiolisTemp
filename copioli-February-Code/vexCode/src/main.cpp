// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// driveLFF              motor         17              
// driveRFF              motor         7               
// driveLFB              motor         6               
// driveRFB              motor         5               
// lift1                motor         15              
// intake1              motor         11              
// intake2              motor         10              
// tilter               motor         14              
// ---- END VEXCODE CONFIGURED DEVICES ----

///////////<change log>///////
/**

2/13/20
  - Lift value +/- 1 triggers tilter to move to value (void automaticTilter)
  - A button toggle 0-200 full, 200-600 half, 600-800 third, 800-1000 quarter, 1000-1200 eigth
  - Started Skills1 function 

1/16/20
  -When toggle is hit, drive goes on hold mode
  -reversed intake and tilter
  -tilter coasts for certain degree values

1/23/20
  - Anish's updated code after Gael
  - RectRed & 
  - Prep for Florin League & Tracey Tournament

**/
///////</change log>///////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include "vex.h"

using namespace vex;

int intake_auto_state = 0;
int intake_auto_vel = 0;
bool intake_speed_toggle_pressed = false;
bool intake_speed_state = false;

controller Controller;
competition Competition;

void reset_drive_encoders(){
  driveLF.resetRotation();
  driveRF.resetRotation();
}

void reset_lift_encoders(){
  lift1.resetRotation();
}

void reset_intake_encoders(){
  intake1.resetRotation();
  intake2.resetRotation();
}

void reset_tilter_encoders(){
  tilter.resetRotation();
}

void reset_encoders(){
  reset_drive_encoders();
  reset_lift_encoders();
  reset_intake_encoders();
  reset_tilter_encoders();
}

void wait(int s){
  vex::task::sleep(s);
}

int sgn(int a){
  if(a != 0) return ((-1*a/a == -1)? 1:-1);
  else return 0;
}

float degrees_to_revs_drive(float degrees){
  return (1/90.0)*(degrees-7);
}

float inches_to_revs_drive(float inches){
  //return inches/(6.29*4);
  return inches*(1.8/24.0);
}

void set_drive(int L, int R, bool speed_toggle){
  if(L == 0)
  {
    if(speed_toggle)
    {
      driveLF.stop(brakeType::hold);
      driveLB.stop(brakeType::hold);
    }
    else
    {
      driveLF.stop(brakeType::coast);
      driveLB.stop(brakeType::coast);
    }
  }
  else
  {
    driveLF.spin(forward, L, velocityUnits::pct);
    driveLB.spin(forward, L, velocityUnits::pct);
  }
  if(R == 0)
  {
    if(speed_toggle)
    {
      driveRF.stop(brakeType::hold);
      driveRB.stop(brakeType::hold);
    }
    else
    {
      driveRF.stop(brakeType::coast);
      driveRB.stop(brakeType::coast);
    }
  }
  else
  {
    driveRF.spin(forward, R, velocityUnits::pct);
    driveRB.spin(forward, R, velocityUnits::pct);
  }
}

void set_drive_auto(int L, int R){
  if(L == 0){
    driveLF.stop(brakeType::coast);
    driveLB.stop(brakeType::coast);
  }
  else{
    driveLF.spin(forward, L, velocityUnits::pct);
    driveLB.spin(forward, L, velocityUnits::pct);
  } 
  if(R == 0){
    driveRF.stop(brakeType::coast);
    driveRB.stop(brakeType::coast);
  } 
  else{
    driveRF.spin(forward, R, velocityUnits::pct);
    driveRB.spin(forward, R, velocityUnits::pct);
  } 
}

void set_lift(int a){
  if(a == 0)
  {
    lift1.stop(brakeType::hold);
  }
  else
  {
    lift1.spin(forward, a, velocityUnits::pct);
  }
}

void drive_forward(int dist, int vel){
  float d = inches_to_revs_drive(dist);
  driveLB.startRotateFor(directionType::fwd, d, rotationUnits::rev, vel, velocityUnits::pct); 
  driveLB.startRotateFor(directionType::fwd, d, rotationUnits::rev, vel, velocityUnits::pct);
  driveLB.startRotateFor(directionType::fwd, d, rotationUnits::rev, vel, velocityUnits::pct);
  driveRF.rotateFor(directionType::fwd, d, rotationUnits::rev, vel, velocityUnits::pct);
  set_drive(0, 0, false);
  //drive_hold();
}

void set_intake(int a){
  if(a == 0)
  {
    intake1.stop(brakeType::hold);
    intake2.stop(brakeType::hold);
  }
  else
  {
    intake1.spin(forward, -a, velocityUnits::pct);
    intake2.spin(forward, -a, velocityUnits::pct);
  }
}

void set_tilter(int a, bool skills=false){
  if(a == 0)
  {
    
    float tilter_pos = tilter.position(degrees);
    if(skills)
    {
      tilter.stop(brakeType::coast);
    }
    else
    {
      if(tilter_pos >= -307)
      {
        tilter.stop(brakeType::coast);
      }
      else tilter.stop(brakeType::hold);
    }
  }
    
  else tilter.spin(forward, -a, velocityUnits::pct);
}

void stop_motors(){
  set_drive(0, 0, false);
  set_intake(0);
  set_tilter(0);
  set_lift(0);
}

bool hold_on = false;
bool drive_pressed = false;
void drive_train(int x, int y, bool speed_toggle){
  if(speed_toggle)
  {
    if(!drive_pressed)
    {
      hold_on = !hold_on;
      drive_pressed = true;
    }
  }
  else
  {
    drive_pressed = false;
  }
  if(abs(x) < 3) x = 0;
  if(abs(y) < 3) y = 0;
  int left = y+x;
  int right = y-x;
  set_drive(left, right, hold_on);
}


void intake(int up, int down, bool speed_toggle){
  if(speed_toggle)
  {
    if(!intake_speed_toggle_pressed)
    {
      intake_speed_state = !intake_speed_state;
      intake_speed_toggle_pressed = true;
    }
  }
  else
  {
    intake_speed_toggle_pressed = false;
  }
  int vel = 0;
  if(up) vel = -100;
  else if(down) vel = 100;
  else vel = 0;
  if(intake_speed_state){
     vel /= 5;
  }
  set_intake(vel);
}

bool lift_speed_toggle_pressed = false;
bool lift_speed_state = false;
void lift(int a, bool speed_toggle){
  if(speed_toggle)
  {
    if(!lift_speed_toggle_pressed)
    {
      lift_speed_state = !lift_speed_state;
      lift_speed_toggle_pressed = true;
    }
  }
  else
  {
    lift_speed_toggle_pressed = false;
  }
  if(abs(a) < 3) a = 0;
  if(lift_speed_state) a /= 4;
  set_lift(a);
}

bool tilter_speed_toggle_pressed = false;
bool tilter_speed_state = false;
void tilt(bool up, bool down, bool speed_toggle){
   // -990 >> infinity: quarter speed
  if(speed_toggle)
  {
    if(!tilter_speed_toggle_pressed)
    {
      tilter_speed_state = !tilter_speed_state;
      tilter_speed_toggle_pressed = true;
    }
  }
  else
  {
    tilter_speed_toggle_pressed = false;
  }
  int vel = 0;
  if(up) vel = 100;
  else if(down) vel = -100;
  else vel = 0;
  if(tilter_speed_state)
  {
    if((tilter.rotation(rotationUnits::deg) < -200) && (tilter.rotation(rotationUnits::deg) >= -600)){
      if(vel>0){
        vel /= 2;
      }
      else{
        vel /= 2;
      }
    }
    else if((tilter.rotation(rotationUnits::deg) < -600) && (tilter.rotation(rotationUnits::deg) >= -800)){
      if(vel>0){
        vel /= 3;
      }
      else{
        vel /= 3;
      }
    }
    else if((tilter.rotation(rotationUnits::deg) < -800) && (tilter.rotation(rotationUnits::deg) >= -1000)){
      if(vel>0){
        vel /= 4;
      }
      else{
        vel /=4;
      }
    }
    else if((tilter.rotation(rotationUnits::deg) < -1000) && (tilter.rotation(rotationUnits::deg) >= -2000)){
      if(vel>0){
        vel /= 8;
      }
      else{
        vel /= 8;
      }
    }
    else{
      if(vel>0){
        vel /= 1;
      }
      else{
        vel /=1;
      }
    }   
  }// vel /= 8;
  set_tilter(vel);
}


void drive_dist(float dist, int vel){
  reset_drive_encoders();
  dist = inches_to_revs_drive(dist);
  driveLF.rotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct, false);
  driveRF.rotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct, false);
  driveLF.rotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct, false);
  driveRF.rotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct, false);  
  //wait(5);
  set_drive(0, 0, true);
}

void drive_dist_alt(float dist, int vel){
  reset_drive_encoders();
  dist = inches_to_revs_drive(dist);
  while(std::abs(driveLF.rotation(rotationUnits::rev)) < std::abs(dist))
  {
    set_drive_auto(sgn(dist)*vel, sgn(dist)*vel);
  }
  set_drive_auto(0,0);
  //wait(5, timeUnits::msec);
  //set_drive(0, 0, true);;
}

void drive_turn(float dist, int vel){
  reset_drive_encoders();
  dist = degrees_to_revs_drive(dist);
  if(dist < 0)
  {
    driveLF.rotateFor(directionType::fwd, -dist, rotationUnits::rev, vel, velocityUnits::pct, false);
    driveRF.rotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct, false);
  }
  else
  {
    driveLF.rotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct, false);
    driveRF.rotateFor(directionType::fwd, -dist, rotationUnits::rev, vel, velocityUnits::pct, false);
  }
  wait(5);
  set_drive(0, 0, false);
}

void drive_turn_alt(float dist, int vel){
  reset_drive_encoders();
  dist = degrees_to_revs_drive(dist);
  if(dist < 0)
  {
    while(std::abs(driveLF.rotation(rotationUnits::rev)) < std::abs(dist))
    {
      set_drive_auto(-vel, vel);
    }
  }
  else
  {
    while(std::abs(driveRF.rotation(rotationUnits::rev)) < std::abs(dist))
    {
      set_drive_auto(vel, -vel);
    }
  }
  set_drive_auto(0,0);
  //wait(5, timeUnits::msec);
  //set_drive(0, 0, true);;
}

void drive_time(int t, int vel){
  set_drive_auto(vel, vel);
  wait(t);
  set_drive_auto(0,0);
}

void lift_hold(){
  lift1.stop(brakeType::hold);
}

void move_lift(float dist, int vel){
  lift1.startRotateFor(directionType::fwd, dist, rotationUnits::rev, vel, velocityUnits::pct);
  set_lift(0);
  lift_hold();
}

int intake_macro(){
  float time = Brain.timer(timeUnits::msec);
  while(Brain.timer(timeUnits::msec)-time < 15000)
  {
    set_intake(intake_auto_state*intake_auto_vel);
    task::sleep(5);
  }
  return (0);
}

void intake_auto(int state, int vel){
  intake_auto_state = state;
  intake_auto_vel = vel;
}

void tilter_hold(){
  tilter.stop(brakeType::hold);
}

void move_tilter(float degrees, int vel){
  tilter.rotateFor(directionType::fwd, degrees, rotationUnits::deg, vel, velocityUnits::pct);
  set_tilter(0);
  tilter_hold();
}

void move_tilter_time(int power, int sec){
  tilter.spin(directionType::fwd, power, velocityUnits::pct);
  task::sleep(sec);
  tilter.spin(directionType::fwd, 0, velocityUnits::pct);
}

void pre_auton(){
  reset_encoders();
}

void deposit_cubes(){
  //move_tilter_time(-100, 550);

  set_intake(-60); //Outake right quick
  wait(400);
  set_intake(0); //stop
  wait(200);
  //tilter.rotateTo(-500, rotationUnits::deg, true);
  move_tilter_time(-60, 1000);

  
  set_intake(-5);
  move_tilter_time(-30, 2000);
  set_intake(0);
  drive_forward(-12, 30);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void rectRed(){ //222222222222222222222222222222
  reset_lift_encoders(); reset_drive_encoders(); reset_tilter_encoders(); set_drive(0, 0, false);
  
  //drive_forward(6, 100);
    //task::sleep(200);
    //drive_time(500, 100);
    //drive_time(400, -100);

  task::sleep(100);
  //drive_time(200, -50);
 
  drive_time(500, 80); //drive fwd half sec
  task::sleep(300);

  drive_time(550, -100); //drive bck half sec
  move_tilter(-700, 100); //move_tilter_fwd
  move_tilter(700, 100); //End of deploy //move_tilter_back
  task::sleep(650);

  drive_forward(11, 100); //Collect 4 flat
  set_intake(80); //Turn intake on
  drive_forward(33, 50); //Collect 4 flat
  task::sleep(100);
  
  drive_forward(-25, 100); // drive back before turn
  task::sleep(250);
  
  drive_turn_alt(-55, 35);
  task::sleep(150);
  
  drive_time(1200, 70);
  task::sleep(150);
  
  drive_forward(-10, 90); // go back before final turn
  task::sleep(100);
  
  drive_turn_alt(-19, 30);

  drive_forward(15,30); // drive to goal

  move_tilter(-500, 50); //move_tilter_fwd

  set_intake(-20);

  move_tilter_time(25, 500);

  drive_forward(-10,50); // drive back
}

void OLD_squareRed8Cube(){  //1212121212121212121212
  reset_lift_encoders();
  reset_drive_encoders();
  reset_tilter_encoders();
  //task t (intake_macro);
  set_intake(-100);//intake_auto(-1, 100);
  move_tilter(600, 100); //move_tilter_up
  move_tilter(-550, 100); //End of deploy //move_tilter_back
  task::sleep(700);


  //drive_forward(11, 75);
  
  
  

  drive_forward(44, 60); //Collect 4 flat
  task::sleep(300);

  set_intake(0);//intake_auto(0,0);

  drive_turn_alt(24, 30); // turn right a bit

  task::sleep(200);

  drive_forward(-30, 95);//Drive back (CENTER N)
  
  task::sleep(500);

  drive_turn_alt(-15, 30); // turn left to correct
  
  task::sleep(100);

  drive_forward(5, 60);
  set_intake(-100);//intake_auto(-1, 100);
  drive_forward(25, 60); //Collect 4 stacked
  set_intake(0);//intake_auto(0,0);

  task::sleep(200);

  drive_turn_alt(110, 30); // turn right a bit

  task::sleep(350);

  drive_time(2500, 80);//drive_forward(40, 70);//Drive back
  task::sleep(200);
  //########################################
  //task::sleep(10000);//master WAIT block
  //########################################

  set_drive(0, 0, true);
  set_tilter(100); //Final Tilter
  task::sleep(900);
  set_tilter(35);
  task::sleep(1600);

  set_drive(-30,-30, true);
  task::sleep(2000);
  set_drive(0,0, true);

}

void squareRed(){ //11111111111111111111111111111111 (ctrl f purposes)
  reset_lift_encoders(); reset_drive_encoders(); reset_tilter_encoders();  
  set_drive(0, 0, false);

  move_tilter(-700, 100); //move_tilter_up
  move_tilter(650, 100); //End of deploy //move_tilter_back
  task::sleep(400);
  set_lift(-40);
  wait(300);
  set_lift(0);

  set_intake(100);
  drive_forward(45, 30); //Collect 4 flat
  task::sleep(100);

  set_intake(0);//intake_auto(0,0);

  set_intake(40);
  drive_forward(-30, 100); //back up
  task::sleep(200);
  set_intake(0);

  drive_turn_alt(95, 30);

  task::sleep(100);


  drive_time(600, 70);
  drive_time(200, -40);
  set_drive(0, 0, false); //check holding
  //task::sleep(300);

  // set_tilter(30); //Final Tilter
  // task::sleep(4500);
  // set_drive(-30,-30, true);
  // task::sleep(2000);
  // set_drive(0,0, true);
  deposit_cubes();
}

void OLD_rectRed(){
  
  reset_lift_encoders();
  reset_drive_encoders();
  reset_tilter_encoders();
  set_drive(0, 0, false);
  
  //drive_forward(6, 100);
  //task::sleep(200);
  //drive_time(500, 100);
  //drive_time(400, -100);
  task::sleep(500);
  //drive_time(200, -50);
  drive_time(500, 100);
  task::sleep(150);
  drive_time(550, -100);
  move_tilter(-700, 100); //move_tilter_up
  move_tilter(650, 100); //End of deploy //move_tilter_back
  task::sleep(650);
  set_intake(80);
  drive_forward(44, 25); //Collect 4 flat
  task::sleep(100);
  drive_forward(-25, 40);
  task::sleep(100);
  drive_turn_alt(-55, 30);
  task::sleep(150);
  drive_time(500, 90);
  task::sleep(150);
  drive_forward(10, -90);
  task::sleep(100);
  drive_turn_alt(-22, 30);

}

void OLD_squareBlue2(){ //5 cube 55555555555555555555555555555555555
  reset_lift_encoders();
  reset_drive_encoders();
  reset_tilter_encoders();
  //task t (intake_macro);
  
  task::sleep(1000);
  //intake_auto(-1, 100);
  task::sleep(50);
  
  set_drive(0, 0, true);;
  move_tilter(700, 100); //move_tilter_up
  move_tilter(-650, 100); //End of deploy //move_tilter_back
  set_intake(-100);
  drive_forward(41, 35); //Collect 4 flat
  task::sleep(100);

  set_intake(0);//intake_auto(0,0);

  drive_forward(-27, 35); //back up
  task::sleep(100);

  drive_turn_alt(-75, 30);

  task::sleep(100);

  drive_time(900, 40);


  //########################################
  //task::sleep(10000);//master WAIT block
  //########################################

  set_drive(0, 0, true);;
  set_intake(20);
  task::sleep(200);
  set_intake(0);

  set_tilter(30); //Final Tilter
  task::sleep(4200);
  set_drive(-30,-30, true);
  task::sleep(2000);
  set_drive(0, 0, true);;
}

void OLD_squareBlue(){ //3333333333333333333333333333    8 cube
  reset_lift_encoders();
  reset_drive_encoders();
  //task t (intake_macro);
  set_intake(-100);//intake_auto(-1, 100);
  move_tilter(600, 100); //move_tilter_up
  move_tilter(-550, 100); //End of deploy //move_tilter_back
  task::sleep(500);

  //drive_forward(11, 100);
  
  set_drive(0, 0, true);;
  

  drive_forward(44, 60); //Collect 4 flat
  task::sleep(300);

  set_intake(0);//intake_auto(0,0);

  drive_turn_alt(-12.5, 30); // turn right a bit

  task::sleep(100);

  drive_forward(-35, 100);//Drive back (CENTER N)
  
  task::sleep(500);

  drive_turn_alt(22, 30); // turn left to correct
  
  task::sleep(100);

  drive_forward(5, 60);
  set_intake(-100);//intake_auto(-1, 100);
  drive_forward(25, 60); //Collect 4 stacked
  set_intake(0);//intake_auto(0,0);

  task::sleep(200);
  set_intake(-100);
  drive_turn_alt(-75, 30); // turn right a bit
  set_intake(0);

  task::sleep(350);

  drive_time(1600, 100);//drive_forward(40, 70);//Drive back

  drive_time(500, 40);
  task::sleep(200);
  //########################################
  //task::sleep(10000);//master WAIT block
  //########################################

  set_drive(0, 0, true);;
  set_tilter(100); //Final Tilter
  task::sleep(900);
  set_tilter(35);
  task::sleep(1600);

  set_drive(-30,-30, true);
  task::sleep(2000);
  set_drive(0, 0, true);;
}

void OLD_skillsAuton(){
  reset_lift_encoders();
  reset_drive_encoders();
  //task t (intake_macro);
  set_intake(-100);//intake_auto(-1, 100);
  move_tilter(600, 100); //move_tilter_up
  move_tilter(-550, 100); //End of deploy //move_tilter_back
  task::sleep(500);

  //drive_forward(11, 100);
  
  set_drive(0, 0, true);;
  

  drive_forward(50, 60); //Collect 4 flat

  task::sleep(300);

  drive_turn_alt(75, 30);

  task::sleep(100);

  drive_forward(50, 60);

  task::sleep(100);

  drive_turn_alt(75, 30);



  task::sleep(100);
  drive_forward(50, 60);
  task::sleep(100);
  drive_turn_alt(-40, 30);
  task::sleep(100);
  drive_forward(50, 60);

  set_drive(0, 0, true);;
  set_intake(20);
  set_tilter(100); //Final Tilter
  task::sleep(900);
  set_tilter(35);
  task::sleep(1600);
  set_tilter(-80);
  task::sleep(900);
  set_tilter(0);
  
  drive_forward(-50, 60);
  drive_turn_alt(-40, 30);
  drive_forward(-30, 60);

}

void OLD_NewsquareRed8Cube(){  //1212121212121212121212
  reset_lift_encoders();
  reset_drive_encoders();
  reset_tilter_encoders();
  //task t (intake_macro);
  set_intake(100);//intake_auto(-1, 100);
  move_tilter(600, 100); //move_tilter_up
  move_tilter(-550, 100); //End of deploy //move_tilter_back
  task::sleep(500);
  set_intake(-100);

  //drive_forward(11, 75);
  
  
  

  drive_forward(44, 100); //Collect 4 flat
  
  task::sleep(500);


  drive_turn_alt(-7, 30);

  set_intake(0);//intake_auto(0,0);

  
  task::sleep(200);
  drive_forward(-39, 100);
  //intake_auto(-1, 100);

  task::sleep(400);
  drive_turn_alt(22.5, 20);
  task::sleep(200);
  drive_forward(7, 50); //Collect 4 stacked
  set_intake(-100);
  drive_forward(33, 60);
  set_intake(0);//intake_auto(0,0);

  task::sleep(200);

  drive_forward(-5, 50);
  task::sleep(200);
  set_intake(-50);
  drive_turn_alt(100, 20); // turn right a bit
  set_intake(0);

  task::sleep(350);

  drive_time(1100, 90);//drive_forward(40, 70);//Drive back
  task::sleep(200);
  //########################################
  //task::sleep(10000);//master WAIT block
  //########################################

  set_intake(20);
  set_drive(0, 0, true);;
  set_tilter(100); //Final Tilter
  task::sleep(900);
  set_tilter(35);
  task::sleep(1600);

  set_drive(-30,-30, true);
  task::sleep(2000);
  set_drive(0, 0, true);;

}

void skills1(){
  reset_lift_encoders(); reset_drive_encoders(); reset_tilter_encoders();  
  set_drive(0, 0, false);

  move_tilter(-700, 100); //move_tilter_up
  move_tilter(650, 100); //End of deploy //move_tilter_back
  task::sleep(800);
  set_lift(-40);
  wait(300);
  set_lift(0);

  set_intake(100);
  drive_forward(45, 30); //Collect 4 flat
  task::sleep(100);

  set_intake(0);//intake_auto(0,0);

  set_intake(40);
  drive_forward(-27, 100); //back up
  task::sleep(200);
  set_intake(0);

  drive_turn_alt(95, 30);

  task::sleep(500);


  drive_time(900, 70);
  task::sleep(300);
  drive_time(200, -25);
  set_drive(0, 0, false); //check holding
  //task::sleep(300);

  // set_tilter(30); //Final Tilter
  // task::sleep(4500);
  // set_drive(-30,-30, true);
  // task::sleep(2000);
  // set_drive(0,0, true);
  deposit_cubes();
}

void skills2() //runs after 5 cube //ssssssssssssssssssssssssssssssssssssss
{
  reset_lift_encoders(); reset_drive_encoders(); reset_tilter_encoders();  
  move_tilter_time(100, 1000);

  drive_forward(-13, 30); //backups and turn to align with row of blocks
  drive_turn_alt(-25, 30);
  task::sleep(300);
  drive_forward(-15, 30); //backups and turn to align with row of blocks
  task::sleep(300);
  drive_turn_alt(-35, 30);
  task::sleep(300);
  drive_time(1000, -30);
  task::sleep(300);


  set_intake(100); //go forward and collect two cubes and back up
  drive_forward(24, 30);
  task::sleep(300);
  drive_forward(-15, 60);
  drive_time(1500, -40);
  set_intake(0);
  task::sleep(300);
  drive_forward(30, 50);

  task::sleep(300); // turn to align with first tower and drive forward
  drive_turn_alt(62, 30);
  task::sleep(300);
  drive_forward(23, 30);
  set_tilter(0, true);
  task::sleep(300); //lift the lift and drop cube, then lower lift
  set_lift(100);
  wait(2000);
  set_lift(0);
  drive_forward(13, 30);
  set_intake(-50);
  wait(1000);
  set_intake(0);
  drive_forward(-20, 30);


  wait(200); //last tower
  drive_turn_alt(-59, 30);
  //drive_forward(30, 30);
  drive_time(1000, 60);
  set_intake(-50);
  wait(1000);
  set_intake(0);
  drive_forward(-20, 30);

  

}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void autonomous(void)//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
{
  //squareRed(); // 5 cube
  //squareRed8Cube(); // 8 cube
  //squareBlue(); //8 cube
  //squareBlue2(); //5 cube
  //rectRed();
  //rectBlue();
  //skillsAuton();
  //NewsquareRed8Cube();
  skills1();
  skills2();
  //deposit_cubes();
}

void lcdFeedback(void){
  //LCD feedback:
  //Each readout is divided by its gear ratio to translate from
  //motor rotation to arm/claw rotation
  Brain.Screen.clearLine(0,color::black);
  Brain.Screen.clearLine(1,color::black);
  Brain.Screen.setCursor(1,0);
  Brain.Screen.print("Tilter rotation: %f degrees",tilter.rotation(rotationUnits::deg));
  Brain.Screen.setCursor(2,0);
  Brain.Screen.print("Lift rotation: %f degrees",lift1.rotation(rotationUnits::deg));
  Brain.Screen.render(); //push data to the LCD all at once to prevent image flickering
}

void automaticOutake(void){
  int trigger = -700;
  int speed = -80;

  if(tilter.rotation(rotationUnits::deg) < (trigger)){
    set_intake(speed); //Turn intake on
  }
}

void automaticTilter(){
  int liftTrigger = 700; //degrees
  int speed = 80;
  int tilterTarget = -800;

  if((lift1.rotation(rotationUnits::deg) == (liftTrigger))){
    tilter.rotateFor(directionType::fwd, tilterTarget, rotationUnits::deg, speed, velocityUnits::pct);
  }
}

void usercontrol( void )
{
  stop_motors();
  tilter.resetPosition();
  while(true)
  {
    drive_train(Controller.Axis4.position(percentUnits::pct), Controller.Axis3.position(percentUnits::pct), Controller.ButtonA.pressing());
    lift(Controller.Axis2.position(percentUnits::pct), Controller.ButtonA.pressing());
    intake(Controller.ButtonL1.pressing(), Controller.ButtonL2.pressing(), Controller.ButtonA.pressing());
    tilt(Controller.ButtonR1.pressing(), Controller.ButtonR2.pressing(), Controller.ButtonA.pressing());
    //automaticTilter();
    lcdFeedback();
    wait(5);
  }
}

int main()
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  pre_auton();
  //////////////////////////////////////////////////////
  Competition.autonomous( autonomous );
  Competition.drivercontrol( usercontrol );
  stop_motors();
}
