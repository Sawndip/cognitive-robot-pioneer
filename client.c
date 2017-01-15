//  pyrimidal neurons.
// synapses L1: in_range:160-200isi out_of_range:>=320isi
//      2s + 160
//sides:
// Close 0:8  => 160:176
// Mid   9:11 => 178:182
// Far  12:20 => 184:200

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libplayerc/playerc.h>
#include <unistd.h>
#include <stdint.h>
#include "client.h"
#define WRITE 2000

playerc_client_t     *client;
playerc_position2d_t *position2d;
playerc_laser_t      *laserp;
playerc_sonar_t      *sonar;
int testing = 0;
int indice;
int neur;
int active_receptors [3] = {-1, -1, -1}; // value of which receptors are active {left, front, right, receptors_changed}
int sensor           [3];                // minimal sensor value in decimeters [left, front, right]
double minSense;
int printLearn       =  1;
int printWhatLearned =  1;
int readSensors      =  1; // the sensors will be read intermittantly
int motor            = -1; // -1:not_moving 0:Forward 1:Left 2:Right 3:ForeLeft 4:ForeRight
int region_L3        = -1; // -1 no decision yet  0:left 1:right
int learning         = -1; // Learning = -1:Not 0:Forward 1:Left 2:Right 3:ForeLeft 4:ForeRight
int connectedL2       [12] = CONNECTED;     // whether L2 neuron is connected
int connectedL3       [36] = CONNECTEDL3;
int choices           [36][3];           // [L3neuron][L4neuron] random choices as to which movements to make 0=done
double ase_L5         [36][3] = ASEL5;
int learned_state     [36] = LEARNED; // learning status for every L4 neuron [region L/R][L4neuron]
                                     // -1=unlearned
                                    // LEARNING_COMPLETE:
                                   // 0: chosen forward
                                  // 1: chosen left
                                 // 2: chosen right
                                // 3: chosen forward-left
                               // 4: chosen forward-right
int dist;
int loop, spike, spikeL2, num;
int printL2         =  1;
int printL3         =  1;
int printL4         =  1;
int printL2inputs   =  1;
int printMotor      =  1;
int printAfter      = -1; //29000; //53000; //49900; //45650;
int motorMove       =  1; // allow the motor to move???
double tempX, tempY, tempZ, tempUse1, Itot, Itot3;
// write to file
FILE *L1, *L2, *L3, *L4, *directPrnt, *conn, *learnPrnt, *dirPrnt;


// Receptor
int isi              [3] = {0, 0, 0};      // the receptor ISIs, incremented every iteration of network loop

// Layer 1
double xL1          [NUM_L1NEURS] = XL1;       // cell
double yL1          [NUM_L1NEURS] = YL1;       // cell
double zL1          [NUM_L1NEURS] = ZL1;       // cell
double Use1L1       [NUM_L1NEURS] = USE1L1;    // cell
int    refractoryL1 [NUM_L1NEURS] = REFRACT_L1;
double voltL1       [NUM_L1NEURS] = VOLTL1;
int    spikeL1      [NUM_L1NEURS];             // output spikes from layer 1
int L1ISIs[8]   = {0,0,0,0,0,0,0,0};

// Layer 2
int    refractoryL2          [MAX_L2NEURS] = REFRACT_L2;
double voltL2                [MAX_L2NEURS] = VOLTL2;
double xL2      [MAX_L2SYNAP][MAX_L2NEURS] = XL2;    // synapse, cell
double yL2      [MAX_L2SYNAP][MAX_L2NEURS] = YL2;    // synapse, cell
double zL2      [MAX_L2SYNAP][MAX_L2NEURS] = ZL2;    // synapse, cell
double Use1L2   [MAX_L2SYNAP][MAX_L2NEURS] = USE1L2; // synapse, cell
int    L2ISIs                [MAX_L2NEURS] = ISIL2;

// Decision cells
int    decisionISI           = 0;
int    spikeL4Dec;                         // spike into layer 4 from decision neurons
double decisionX         [2] = {1.0, 1.0}; // synapse
double decisionY         [2] = {0.0, 0.0}; // synapse
double decisionZ         [2] = {0.0, 0.0}; // synapse
double decisionUse1      [2] = {0.0, 0.0}; // synapse
double decisionVolt      [2] = {0.0, 0.0}; // neuron
int    decisionRefractory[2] = {0,   0};   // neuron

// Direction facing towards, parallel or away from wall
int isiDirection  = 0;
int dirISIS  [3]  = {0, 0, 0};        // ISIs for the 3 direction cells
int isiDirVec[3]  = {160, 180, 200}; // vector of isis for direction
int direction     = 0; // 0:towards  1:parallel  2:away
double xDir          [3] = XDIR;       // synapse, cell
double yDir          [3] = YDIR;       // synapse, cell
double zDir          [3] = ZDIR;       // synapse, cell
double Use1Dir       [3] = USE1DIR;    // synapse, cell
int    refractoryDir [3] = REFRACT_DIR;
double voltDir       [3] = VOLTDIR;
int    spikeDir; // spikes from direction cells into L3
double ItotDir       [3];

// Layer 3
double xL3faci    = 1.0;
double yL3faci    = 0.0;
double zL3faci    = 0.0;
double Use1L3faci = 0.0;
double xL3para    = 1.0;
double yL3para    = 0.0;
double zL3para    = 0.0;
double Use1L3para = 0.0;
double xL3away    = 1.0;
double yL3away    = 0.0;
double zL3away    = 0.0;
double Use1L3away = 0.0;
double xL3          [MAX_L3NEURS] = XL3;
double yL3          [MAX_L3NEURS] = YL3;
double zL3          [MAX_L3NEURS] = ZL3;
double Use1L3       [MAX_L3NEURS] = USE1L3;
double voltL3       [MAX_L3NEURS] = VOLTL3;
int    refractoryL3 [MAX_L3NEURS] = REFRACT_L3;
int    L3ISI        [MAX_L3NEURS] = ISIL3;

// Layer 4
double xL4Left     = 1.0;
double xL4Right    = 1.0;
double yL4Left     = 0.0;
double yL4Right    = 0.0;
double zL4Left     = 0.0;
double zL4Right    = 0.0;
double Use1L4Left  = 0.0;
double Use1L4Right = 0.0;
double ItotL4       [MAX_L4SYNAP];
double xL4          [MAX_L4NEURS] = XL4;
double yL4          [MAX_L4NEURS] = YL4;
double zL4          [MAX_L4NEURS] = ZL4;
double Use1L4       [MAX_L4NEURS] = USE1L4;
double voltL4       [MAX_L4NEURS] = VOLTL4;
int    refractoryL4 [MAX_L4NEURS] = REFRACT_L4;
int    L4ISI        [MAX_L4NEURS] = ISIL4;

// Layer 5
double xL5    [MAX_L5SYNAP][MAX_L5NEURS] = XL5;
double yL5    [MAX_L5SYNAP][MAX_L5NEURS] = YL5;
double zL5    [MAX_L5SYNAP][MAX_L5NEURS] = ZL5;
double ItotL5              [MAX_L5NEURS] = ITOTL5;
double voltL5              [MAX_L5NEURS] = VOLTL5;
int    refractoryL5        [MAX_L5NEURS] = REFRACT_L5;
int    L5ISI               [MAX_L5NEURS] = ISIL5;


int main()
{
  char typed;
  // debugging output data
  int printReceptor   =  1;
  int printDirection  =  1;
  int printL1         =  1;

  // find wall variable
  int wall_found = 0;
  int neur; //, num; //, rOLD; //, num; // loop only used for network loop. neur can be re-used in any loop
  int initialized  = 0; // client not initialized yet

  // Receptor
  bool sensors_changed;

  // DECISION REGION
  int decisionISI2spike    [2] = {160, 200}; // {left, right};
  int decISI[3]    = {0, 0, 0};

  //  f=fopen("output/laser_readings.m", "w");
  conn       = fopen("/home/eric/Desktop/output/connections.txt",  "w");
  L1         = fopen("/home/eric/Desktop/output/Layer1.txt",       "w");
  L2         = fopen("/home/eric/Desktop/output/Layer2.txt",       "w");
  L3         = fopen("/home/eric/Desktop/output/Layer3.txt",       "w");
  L4         = fopen("/home/eric/Desktop/output/Layer4.txt",       "w");
  learnPrnt  = fopen("/home/eric/Desktop/output/learn.txt",        "w");
  directPrnt = fopen("/home/eric/Desktop/output/direction.txt",    "w");
  dirPrnt    = fopen("/home/eric/Desktop/output/direction2L3.txt", "w");
  // Create a client object and connect to the server; the server must be running on "localhost" at port 6665
  //  client = playerc_client_create(NULL, "192.168.1.176", 6665);
  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
  {
    printf("client creation failed \n");
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  // Create a position2d proxy (device id "position2d:0") and susbscribe in read/write mode
  position2d = playerc_position2d_create(client, 0);
  if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE))
  {
    printf("position2d subscription failed \n");
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  // Create a laser proxy and susbscribe in read/write mode
  laserp = playerc_laser_create(client, 0);
//  if (playerc_laser_subscribe(laserp, PLAYERC_OPEN_MODE) != 0)
  if (playerc_laser_subscribe(laserp, PLAYER_OPEN_MODE))
  {
    printf("laser failed \n");
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  // Create a sonar proxy and susbscribe in read/write mode
  sonar = playerc_sonar_create(client, 0);
  if (playerc_sonar_subscribe(sonar, PLAYERC_OPEN_MODE))
  {
    printf("sonar failed \n");
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }


  playerc_position2d_enable(position2d, 1); // Enable the robots motors
  printf("waiting 5 seconds for initialization... ");
  fflush(stdout); // flush the standard output before sleeping
  sleep(5); // wait 5 seconds */
  playerc_client_read(client); // Wait for new data from server
  playerc_client_read(client); // Wait for new data from server
  playerc_client_read(client); // Wait for new data from server

  do
  {
    playerc_client_read(client); // Wait for new data from server
    if (NULL != laserp->point)
      initialized = 1;
    else
      printf("\nlaser pointer == NULL \n");
  } while (initialized == 0);
  printf("done!!! \n");

  // initialize sensor data
  playerc_client_read(client);         // Wait for new data from server



printf("OK \n");



  // First thing we need to do is find the wall
  while (wall_found == 0) // wall is not found
  {
    playerc_client_read(client);         // Wait for new data from server

    // get closest reading on the left
    minSense = laserp->ranges[181];
    for(neur=183; neur<=360; neur=neur+2)
      if (minSense > laserp->ranges[neur])
        minSense = laserp->ranges[neur];

    if (minSense < 1.2) // wall found
    {
      motors(NOMOVE, STOP);
      region_L3  = 0; // follow on left side
    }

    // see if distance on the right is closer than the left
    for(neur=0; neur<181; neur=neur+2)
      if (minSense > laserp->ranges[neur]) // closer on the right
        if (laserp->ranges[neur] < 1.2) // wall found
        {
          motors(NOMOVE, STOP);
          region_L3  = 1; // follow on right side
          break;          // exit for loop
        }

    if (region_L3 > -1)
    {
      motor = -1;      // motor is not moving
      wall_found = 1;  // found the wall, leave while loop
      if (region_L3==0)
        printf("\nfound the wall, folllowing on the left!\n\n");
      else
        printf("\nfound the wall, folllowing on the right!\n\n");
    }
    else if (motor == -1) // // haven't found the wall, robot is not moving
      motors(FORE, SLOW);
  } // end while (wall_found == 0) // wall is not found




  set_choices(); // set up starting random values for choices


//// FOR TESTING. DELETE WHEN DONE!!!!!!!!!!!!!!!!
//  // connect everything
//  for (num = 0; num<12; num++)
//  {
//    connectedL2[num] = 1;
//    connectedL3[num] = 1;
//    //learned_state[num] = 0;
//  }
//  for (num = 12; num<36; num++)
//  {
//    connectedL3[num] = 1;
//    //learned_state[num] = 0;
//  }
//
//printf("\nReady to enter loop\n");


  // NETWORK LOOP STARTS HERE!!!!!! --------------------------------------------------------------
  for (loop=0; loop<NUM_LOOPS; loop++)
  {

    if ((loop % 500001) == 0)
    {
      neur = motor;
      motors(NOMOVE, STOP); // stop
      do
      {
        printf("%i minutes, press t, r, l or q. ", (int)(loop / 60000));
        fflush(stdout); // flush the standard output before sleeping
        typed = getchar();

        if (typed == 't')
        {
          testing = 1;
        }
        if (typed == 'r')
        {
          if (region_L3==0) // was following on left
          {
            region_L3 = 1;        // change region
            printf("turning to follow on right... \n");
            playerc_position2d_set_cmd_vel(position2d,  STOP, 0.0,  0.1, 1); // turn left
            do
            {
              playerc_client_read(client); // update data
              neur = 0;
              for (num=1; num<16; num++)
                if (sonar->scan[num] < sonar->scan[neur])
                  neur = num;
            } while((neur!=8) && (neur!=7));
            motors(NOMOVE, STOP); // stop
            readSensors = 1;
            fprintf(conn,       "%ims, turning around to follow wall on right \n", loop);
            fprintf(L1,         "%ims, turning around to follow wall on right \n", loop);
            fprintf(L2,         "%ims, turning around to follow wall on right \n", loop);
            fprintf(L3,         "%ims, turning around to follow wall on right \n", loop);
            fprintf(learnPrnt,  "%ims, turning around to follow wall on right \n", loop);
            fprintf(directPrnt, "%ims, turning around to follow wall on right \n", loop);
            neur = -1;
          }
          else if (region_L3==0)
            printf("still following on right... \n");
        }
        else if (typed == 'l')
        {
          if (region_L3==0)
            printf("still following on left... \n");
          else
          {
            region_L3 = 0;        // change region
            printf("turning to follow on left... \n");
            playerc_position2d_set_cmd_vel(position2d,  STOP, 0.0,  -0.1, 1); // turn right
            do
            {
              playerc_client_read(client); // update data
              neur = 0;
              for (num=1; num<16; num++)
                if (sonar->scan[num] < sonar->scan[neur])
                  neur = num;
            } while((neur!=0) && (neur!=15));
            motors(NOMOVE, STOP); // stop
            readSensors = 1;
            fprintf(conn,       "%ims, turning around to follow wall on left \n", loop);
            fprintf(L1,         "%ims, turning around to follow wall on left \n", loop);
            fprintf(L2,         "%ims, turning around to follow wall on left \n", loop);
            fprintf(L3,         "%ims, turning around to follow wall on left \n", loop);
            fprintf(learnPrnt,  "%ims, turning around to follow wall on left \n", loop);
            fprintf(directPrnt, "%ims, turning around to follow wall on left \n", loop);
            neur = -1;
          }
        }
        else if (typed == 'q')
        {
          fclose(conn);
          fclose(L1);
          fclose(L2);
          fclose(L3);
          fclose(L4);
          fclose(learnPrnt);
          fclose(directPrnt);
          return 0;
        }
      } while ((typed != 'r') && (typed != 'l') && (typed != 'q'));
      if (neur>-1)
        motors(neur, SLOW); // continue
    }


    ItotL5[0] = 0.0;
    ItotL5[1] = 0.0;
    ItotL5[2] = 0.0;



    // Sensor Input
    //      2s + 160
    //       0:8  => 160:176
    //       9:11 => 178:182
    //      12:20 => 184:200
    if (readSensors == 1)  // time to read the sensors
    {
      sensors_changed = false;
      do
      {
        playerc_client_read(client);  // Wait for new data from server

        // Sensor 0: 60 readings (1 every degree)
        sensor[0] = ((2 * ((int)(get_laser_left() * 10))) + 160); // convert closest value to decimeter and then to ISI
        if (sensor[0] < 177)            // close     receptor ISIs 160->176
        {
          if (active_receptors[0] != 0)
            sensors_changed   = true;
        }
        else if (sensor[0] < 183)       // middle    receptor ISIs 178->182
        {
          if (active_receptors[0] != 1)
            sensors_changed   = true;
        }
        else if (active_receptors[0] != 2) // far receptor ISIs 184->200
          sensors_changed     = true;

        // Sensor 1: 60 readings (1 every degree)
        sensor[1] = (( 2 * ((int)(get_laser_front() * 10))) + 160);// convert closest value to decimeter and then to ISI
        if (sensor[1] < 177)            // close     receptor ISIs 160->176
        {
          if (active_receptors[1] != 3)
            sensors_changed   = true;
        }
        else if (active_receptors[1] != 4) // far       receptor ISIs 178->200
          sensors_changed     = true;

        // Sensor 2: 60 readings (1 every degree)
        sensor[2] = (( 2 * ((int)(get_laser_right() * 10))) + 160);// convert closest value to decimeter and then to ISI
        if (sensor[2] < 177)            // close     receptor ISIs 160->176
        {
          if (active_receptors[2] != 5)
            sensors_changed = true;
        }
        else if (sensor[2] < 183)       // middle    receptor ISIs 178->182
        {
          if (active_receptors[2] != 6)
            sensors_changed = true;
        }
        else if (active_receptors[2] != 7) // far receptor ISIs 184->200
          sensors_changed   = true;

        // facing direction
        num = get_direction();
        if (num != direction) // 0:towards  1:parallel  2:facing
          sensors_changed = true;

        if (motor<0)
          sensors_changed = true;
        if (sonar->scan[get_closest_sonar()] > 1.9) // too far
          if (learned_state[(num + 33)] == -1)
            sensors_changed = true;
      } while (sensors_changed == false);

      // OK, sensors changed. Stop robot, then update sensors
      motors(NOMOVE, STOP);         //Stop robot
      sleep(2);                     // make sure it has fully stopped
      playerc_client_read(client);  // Wait for new data from server

      // update sensors
      // Sensor 0: 60 readings (1 every degree)
      sensor[0] = ((2 * ((int)(get_laser_left() * 10))) + 160); // convert closest value to decimeter and then to ISI
      if (sensor[0] < 177)          // close     receptor ISIs 160->176
        active_receptors[0] = 0;    // too close receptor will be active
      else if (sensor[0] < 183)     // middle    receptor ISIs 178->182
        active_receptors[0] = 1;    // target receptor will be active
      else                          // far receptor ISIs 184->200
      {
        active_receptors[0] = 2;    // far away receptor will be active
        if (sensor[0] > 200)
          sensor[0] = 200;
      }

      // Sensor 1: 60 readings (1 every degree)
      sensor[1] = (( 2 * ((int)(get_laser_front() * 10))) + 160);// convert closest value to decimeter and then to ISI
      if (sensor[1] < 177)               // close     receptor ISIs 160->176
          active_receptors[1] = 3;       // too close receptor will be active
      else                               // far       receptor ISIs 178->200
      {
        active_receptors[1] = 4;         // far away receptor will be active
        if (sensor[1] > 200)
          sensor[1] = 200;
      }

      // Sensor 2: 60 readings (1 every degree)
      sensor[2] = (( 2 * ((int)(get_laser_right() * 10))) + 160);// convert closest value to decimeter and then to ISI
      if (sensor[2] < 177)            // close     receptor ISIs 160->176
          active_receptors[2] = 5;    // too close receptor will be active
      else if (sensor[2] < 183)       // middle    receptor ISIs 178->182
          active_receptors[2] = 6;    // target receptor will be active
      else                            // far receptor ISIs 184->200
      {
        active_receptors[2] = 7;    // far away receptor will be active
        if (sensor[2] > 200)
          sensor[2] = 200;
      }

      direction = get_direction();   // 0:towards  1:parallel  2:facing

      // Add neuron to the left side?
      indice = (active_receptors[0] * 2);
      if (active_receptors[1]==4)
        ++indice;
      if (connectedL2[indice] == 0) // need to make connection on left L2 side
      {
        connectedL2[indice] = 1;   // connect this L2 neuron
        fprintf(conn, "connecting layer 2 neuron %i \n", indice);
      }
      neur = (indice * 3) + direction;
      if (connectedL3[neur] == 0)// need to make connection on left L3 side
      {
        connectedL3[neur] = 1;   // connect this L3 neuron
        fprintf(conn, "connecting layer 3 neuron %i \n", neur);
      }

      // Add neuron to the right side?
      indice = ((active_receptors[2] - 2) * 2);
      if (active_receptors[1]==4)
        ++indice;
      if (connectedL2[indice] == 0) // need to make connection on right side
      {
        connectedL2[indice] = 1;   // connect this L2 neuron
        fprintf(conn, "connecting layer 2 neuron %i \n", indice);
      }
      neur = (indice * 3) + direction;
      if (connectedL3[neur] == 0)// need to make connection on left L3 side
      {
        connectedL3[neur] = 1;   // connect this L3 neuron
        fprintf(conn, "connecting layer 3 neuron %i \n", neur);
      }

      // gone too far???
      if ((sensor[(region_L3*2)] == 200) && (sensor[1] == 200)) // too far
        if (motor > -1)
          too_far();

        readSensors = 0;    // stop reading sensors
    }  // end if (readSensors == 1)  // time to read the sensors








    // Layer 1 ***************************************************************************************
    // LEFT
    spike = 0;
    isi[0]++;                // left isi
    if (isi[0] >= sensor[0]) // time to spike
    {
      isi[0] = 0;            // reset isi
      spike = 1;
      if (printReceptor == 1)
        if (loop > printAfter)
          fprintf(L1, "at %ims: spike from left receptor at %i ISI. \n", loop, sensor[0]);
    }

    // close neuron 0
    // Excitatory synapse (TS=1, so is removed, as there's no point wasting processor cycles to multiply by 1)
    tempX    = xL1[0]    + (        zL1[0] / TAU_REC_CLOSE)    - ((Use1L1[0] * xL1[0]) * spike);  // spike [synapse][neuron]
    tempY    = yL1[0]    + (-1 *    yL1[0] / TAU_IN)           + ((Use1L1[0] * xL1[0]) * spike);
    tempZ    = zL1[0]    + (        yL1[0] / TAU_IN)           - (zL1[0] / TAU_REC_CLOSE);
    tempUse1 = Use1L1[0] + (-1 *(Use1L1[0] / TAU_FACIL_CLOSE)) + (USE_L1 * (1 - Use1L1[0]) * spike);
    xL1[0]    = tempX;
    yL1[0]    = tempY;
    zL1[0]    = tempZ;
    Use1L1[0] = tempUse1;
    if (refractoryL1[0] < 1)
    {
      voltL1[0] = voltL1[0] + (TAU_CONST_INTERNEURON * (voltL1[0] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltL1[0] < 0.0)
        voltL1[0] = 0.0;
    }
    else
      refractoryL1[0]--;
    if (voltL1[0] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from close  left neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[0]) );
        L1ISIs[0]       = loop;
      }
      voltL1[0]       = 0.0;
      spikeL1[0] = 1;
      refractoryL1[0] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[0] = 0;

    // medium neuron 1
    tempX    = xL1[1]    + (         zL1[1] / TAU_REC_MID)    - ((Use1L1[1] * xL1[1]) * spike);  // spike [synapse][neuron]
    tempY    = yL1[1]    + (-1 *     yL1[1] / TAU_IN)         + ((Use1L1[1] * xL1[1]) * spike);
    tempZ    = zL1[1]    + (         yL1[1] / TAU_IN)         - (zL1[1] / TAU_REC_MID);
    tempUse1 = Use1L1[1] + (-1 * (Use1L1[1] / TAU_FACIL_MID)) + (USE_L1 * (1 - Use1L1[1]) * spike);
    xL1[1]    = tempX;
    yL1[1]    = tempY;
    zL1[1]    = tempZ;
    Use1L1[1] = tempUse1;
    if (refractoryL1[1] < 1)
    {
      voltL1[1] = voltL1[1] + (TAU_CONST_INTERNEURON * (voltL1[1] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltL1[1] < 0.0)
        voltL1[1] = 0.0;
    }
    else
      refractoryL1[1]--;
    if (voltL1[1] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from mid distance left  neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[1]) );
        L1ISIs[1] = loop;
      }
      voltL1[1]  = 0.0;
      spikeL1[1] = 1;
      refractoryL1[1] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[1] = 0;

    // long neuron 2
    tempX    = xL1[2]    + (        zL1[2] / TAU_REC_FAR)    - ((Use1L1[2] * xL1[2]) * spike);  // spike [synapse][neuron]
    tempY    = yL1[2]    + (-1 *    yL1[2] / TAU_IN)       + ((Use1L1[2] * xL1[2]) * spike);
    tempZ    = zL1[2]    + (        yL1[2] / TAU_IN)       - (zL1[2] / TAU_REC_FAR);
    tempUse1 = Use1L1[2] + (-1 *(Use1L1[2] / TAU_FACIL_FAR)) + (USE_L1 * (1 - Use1L1[2]) * spike);
    xL1[2]    = tempX;
    yL1[2]    = tempY;
    zL1[2]    = tempZ;
    Use1L1[2] = tempUse1;
    if (refractoryL1[2] < 1)
    {
      voltL1[2] = voltL1[2] + (TAU_CONST_INTERNEURON * (voltL1[2] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltL1[2] < 0.0)
        voltL1[2] = 0.0;
    }
    else
      refractoryL1[2]--;
    if (voltL1[2] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from far distance left  neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[2]) );
        L1ISIs[2]       = loop;
      }
      voltL1[2]       = 0.0;
      spikeL1[2]      = 1;
      refractoryL1[2] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[2] = 0;


    // FRONT
    spike = 0;
    isi[1]++;  // front isi
    if (isi[1] >= sensor[1]) // time to spike
    {
      isi[1] = 0;            // reset isi
      spike = 1;
      if (printReceptor == 1)
        if (loop > printAfter)
          fprintf(L1, "at %ims: spike from front receptor at %i ISI. \n", loop, sensor[1]);
    }

    // close neuron 3
    // Excitatory synapse (TS=1, so is removed, as there's no point wasting processor cycles to multiply by 1)
    tempX     = xL1[3]    + (        zL1[3] / TAU_REC_CLOSE)    - ((Use1L1[3] * xL1[3])     * spike);  // spike [synapse][neuron]
    tempY     = yL1[3]    + (-1 *    yL1[3] / TAU_IN)           + ((Use1L1[3] * xL1[3])     * spike);
    tempZ     = zL1[3]    + (        yL1[3] / TAU_IN)           - (zL1[3] / TAU_REC_CLOSE);
    tempUse1  = Use1L1[3] + (-1 *(Use1L1[3] / TAU_FACIL_CLOSE)) + (USE_L1 * (1 - Use1L1[3]) * spike);
    xL1[3]    = tempX;
    yL1[3]    = tempY;
    zL1[3]    = tempZ;
    Use1L1[3] = tempUse1;
    if (refractoryL1[3] < 1)
    {
      voltL1[3] = voltL1[3] + (TAU_CONST_INTERNEURON * (voltL1[3] - (R_MEM_BASE_INTERNEURON * (L1_ASE_FRONT * tempY))));
      if (voltL1[3] < 0.0)
          voltL1[3] = 0.0;
    }
    else
      refractoryL1[3]--;
    if (voltL1[3] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from close  front neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[3]) );
        L1ISIs[3]       = loop;
      }
      voltL1[3]       = 0.0;
      spikeL1[3]      = 1;
      refractoryL1[3] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[3] = 0;


    // far neuron 2
    tempX    = xL1[4]    + (        zL1[4] / TAU_REC_FAR_F)    - ((Use1L1[4] * xL1[4])     * spike);  // spike [synapse][neuron]
    tempY    = yL1[4]    + (-1 *    yL1[4] / TAU_IN)           + ((Use1L1[4] * xL1[4])     * spike);
    tempZ    = zL1[4]    + (        yL1[4] / TAU_IN)           - (zL1[4] / TAU_REC_FAR_F);
    tempUse1 = Use1L1[4] + (-1 *(Use1L1[4] / TAU_FACIL_FAR_F)) + (USE_L1 * (1 - Use1L1[4]) * spike);
    xL1[4]    = tempX;
    yL1[4]    = tempY;
    zL1[4]    = tempZ;
    Use1L1[4] = tempUse1;
    if (refractoryL1[4] < 1)
    {
      voltL1[4] = voltL1[4] + (TAU_CONST_INTERNEURON * (voltL1[4] - (R_MEM_BASE_INTERNEURON * (L1_ASE_FRONT_FAR * tempY))));
      if (voltL1[4] < 0.0)
          voltL1[4] = 0.0;
    }
    else
      refractoryL1[4]--;
    if (voltL1[4] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from far distance front neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[4]) );
        L1ISIs[4]       = loop;
      }
      voltL1[4]       = 0.0;
      spikeL1[4]      = 1;
      refractoryL1[4] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[4] = 0;



    // RIGHT
    spike = 0;
    isi[2]++;  // right isi
    if (isi[2] >= sensor[2]) // time to spike
    {
      isi[2] = 0;            // reset isi
      spike  = 1;
      if (printReceptor == 1)
        if (loop > printAfter)
          fprintf(L1, "at %ims: spike from right receptor at %i ISI. \n", loop, sensor[2]);
    }

    // short neuron 0
    // Excitatory synapse (TS=1, so is removed, as there's no point wasting processor cycles to multiply by 1)
    tempX    = xL1[5]    + (        zL1[5] / TAU_REC_CLOSE)    - ((Use1L1[5] * xL1[5]) * spike);  // spike [synapse][neuron]
    tempY    = yL1[5]    + (-1 *    yL1[5] / TAU_IN)       + ((Use1L1[5] * xL1[5]) * spike);
    tempZ    = zL1[5]    + (        yL1[5] / TAU_IN)       - (zL1[5] / TAU_REC_CLOSE);
    tempUse1 = Use1L1[5] + (-1 *(Use1L1[5] / TAU_FACIL_CLOSE)) + (USE_L1 * (1 - Use1L1[5]) * spike);
    xL1[5]    = tempX;
    yL1[5]    = tempY;
    zL1[5]    = tempZ;
    Use1L1[5] = tempUse1;
    if (refractoryL1[5] < 1)
    {
      voltL1[5] = voltL1[5] + (TAU_CONST_INTERNEURON * (voltL1[5] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltL1[5] < 0.0)
        voltL1[5] = 0.0;
    }
    else
      refractoryL1[5]--;
    if (voltL1[5] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from close  right neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[5]) );
        L1ISIs[5]       = loop;
      }
      voltL1[5]       = 0.0;
      spikeL1[5]      = 1;
      refractoryL1[5] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[5] = 0;


    // medium neuron 1
    tempX    = xL1[6]    + (        zL1[6] / TAU_REC_MID)    - ((Use1L1[6] * xL1[6]) * spike);  // spike [synapse][neuron]
    tempY    = yL1[6]    + (-1 *    yL1[6] / TAU_IN)         + ((Use1L1[6] * xL1[6]) * spike);
    tempZ    = zL1[6]    + (        yL1[6] / TAU_IN)         - (zL1[6] / TAU_REC_MID);
    tempUse1 = Use1L1[6] + (-1 *(Use1L1[6] / TAU_FACIL_MID)) + (USE_L1 * (1 - Use1L1[6]) * spike);
    xL1[6]    = tempX;
    yL1[6]    = tempY;
    zL1[6]    = tempZ;
    Use1L1[6] = tempUse1;
    if (refractoryL1[6] < 1)
    {
      voltL1[6] = voltL1[6] + (TAU_CONST_INTERNEURON * (voltL1[6] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltL1[6] < 0.0)
        voltL1[6] = 0.0;
    }
    else
      refractoryL1[6]--;
    if (voltL1[6] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from mid distance right neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[6]) );
        L1ISIs[6]       = loop;
      }
      voltL1[6]       = 0.0;
      spikeL1[6]      = 1;
      refractoryL1[6] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[6] = 0;


    // long neuron 2
    tempX    = xL1[7]    + (        zL1[7] / TAU_REC_FAR)    - ((Use1L1[7] * xL1[7]) * spike);  // spike [synapse][neuron]
    tempY    = yL1[7]    + (-1 *    yL1[7] / TAU_IN)         + ((Use1L1[7] * xL1[7]) * spike);
    tempZ    = zL1[7]    + (        yL1[7] / TAU_IN)         - (zL1[7] / TAU_REC_FAR);
    tempUse1 = Use1L1[7] + (-1 *(Use1L1[7] / TAU_FACIL_FAR)) + (USE_L1 * (1 - Use1L1[7]) * spike);
    xL1[7]    = tempX;
    yL1[7]    = tempY;
    zL1[7]    = tempZ;
    Use1L1[7] = tempUse1;
    if (refractoryL1[7] < 1)
    {
      voltL1[7] = voltL1[7] + (TAU_CONST_INTERNEURON * (voltL1[7] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltL1[7] < 0.0)
        voltL1[7] = 0.0;
    }
    else
      refractoryL1[7]--;
    if (voltL1[7] >= VTH)
    {
      if (printL1 == 1)
      {
        if (loop > printAfter)
          fprintf(L1, "%ims: spike from far distance right neuron L1 at %i ISI. \n", loop, (loop - L1ISIs[7]) );
        L1ISIs[7]       = loop;
      }
      voltL1[7]       = 0.0;
      spikeL1[7]      = 1;
      refractoryL1[7] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeL1[7] = 0;










    //******************************************************************************************
    //******************************************************************************************
    // DIRECTION *******************************************************************************
    // towards  => 160
    // parallel => 180
    // away     => 200
    isiDirection++;                // left isi
    if (isiDirection >= isiDirVec[direction]) // time to spike
    {
      isiDirection = 0;            // reset isi
      spike = 1;
      if (printDirection == 1)
        fprintf(dirPrnt, "%ims: spike into directions at %i ISI. \n", loop, isiDirVec[direction]);
    }
    else
      spike = 0;


    // towards neuron 0
    tempX    = xDir[0]    + (        zDir[0] / TAU_REC_CLOSE)    - ((Use1Dir[0] * xDir[0]) * spike);  // spike [synapse][neuron]
    tempY    = yDir[0]    + (-1 *    yDir[0] / TAU_IN)           + ((Use1Dir[0] * xDir[0]) * spike);
    tempZ    = zDir[0]    + (        yDir[0] / TAU_IN)           - (zDir[0] / TAU_REC_CLOSE);
    tempUse1 = Use1Dir[0] + (-1 *(Use1Dir[0] / TAU_FACIL_CLOSE)) + (USE_L1 * (1 - Use1Dir[0]) * spike);
    xDir[0]    = tempX;
    yDir[0]    = tempY;
    zDir[0]    = tempZ;
    Use1Dir[0] = tempUse1;
    if (refractoryDir[0] < 1)
    {
      voltDir[0] = voltDir[0] + (TAU_CONST_INTERNEURON * (voltDir[0] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltDir[0] < 0.0)
        voltDir[0] = 0.0;
    }
    else
      refractoryDir[0]--;
    if (voltDir[0] >= VTH)
    {
      if (printDirection == 1)
      {
        fprintf(dirPrnt, "%ims: spike from towards neuron into L3 at %i ISI. \n", loop, (loop - dirISIS[0]) );
        dirISIS[0]       = loop;
      }
      voltDir      [0] = 0.0;
      spikeDir         = 1;
      refractoryDir[0] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeDir = 0;

    // synapse from facing wall direction to L3
    tempX      = xL3faci    + (     zL3faci    / TAU_REC_L3)    - ((Use1L3faci * xL3faci) * spikeDir);  // spike [L3neuron][synapse][time]
    tempY      = yL3faci    + (-1 * yL3faci    / TAU_IN)        + ((Use1L3faci * xL3faci) * spikeDir);
    tempZ      = zL3faci    + (     yL3faci    / TAU_IN)        - (zL3faci / TAU_REC_L3);
    tempUse1   = Use1L3faci + (-1 *(Use1L3faci / TAU_FACIL_L3)) + (USE_L3 * (1 - Use1L3faci) * spikeDir);
    ItotDir[0] = (L3_ASE * tempY);
    xL3faci    = tempX;
    yL3faci    = tempY;
    zL3faci    = tempZ;
    Use1L3faci = tempUse1;
    if (spikeDir==1)
      fprintf(L3, "  %ims: spike from facing wall direction to L3 ItotDir[0]. \n", loop);

    // parallel neuron 1
    tempX    = xDir[1]    + (        zDir[1] / TAU_REC_MID)    - ((Use1Dir[1] * xDir[1])    * spike);  // spike [synapse][neuron]
    tempY    = yDir[1]    + (-1 *    yDir[1] / TAU_IN)         + ((Use1Dir[1] * xDir[1])    * spike);
    tempZ    = zDir[1]    + (        yDir[1] / TAU_IN)         - (zDir[1] / TAU_REC_MID);
    tempUse1 = Use1Dir[1] + (-1 *(Use1Dir[1] / TAU_FACIL_MID)) + (USE_L1 * (1 - Use1Dir[1]) * spike);
    xDir[1]    = tempX;
    yDir[1]    = tempY;
    zDir[1]    = tempZ;
    Use1Dir[1] = tempUse1;
    if (refractoryDir[1] < 1)
    {
      voltDir[1] = voltDir[1] + (TAU_CONST_INTERNEURON * (voltDir[1] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltDir[1] < 0.0)
        voltDir[1] = 0.0;
    }
    else
      refractoryDir[1]--;
    if (voltDir[1] >= VTH)
    {
      if (printDirection == 1)
      {
        fprintf(dirPrnt, "%ims: spike from parallel neuron into L3 at %i ISI. \n", loop, (loop - dirISIS[1]) );
        dirISIS[1] = loop;
      }
      voltDir      [1] = 0.0;
      spikeDir         = 1;
      refractoryDir[1] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeDir = 0;

    // synapse from parallel to wall direction to L3
    tempX      = xL3para    + (     zL3para    / TAU_REC_L3)    - ((Use1L3para * xL3para)    * spikeDir);  // spike [L3neuron][synapse][time]
    tempY      = yL3para    + (-1 * yL3para    / TAU_IN)        + ((Use1L3para * xL3para)    * spikeDir);
    tempZ      = zL3para    + (     yL3para    / TAU_IN)        - (zL3para / TAU_REC_L3);
    tempUse1   = Use1L3para + (-1 *(Use1L3para / TAU_FACIL_L3)) + (USE_L3 * (1 - Use1L3para) * spikeDir);
    ItotDir[1] = (L3_ASE * tempY);
    xL3para    = tempX;
    yL3para    = tempY;
    zL3para    = tempZ;
    Use1L3para = tempUse1;
    if (spikeDir==1)
      fprintf(L3, "  %ims: spike from parallel to wall direction to L3 ItotDir[1]. \n", loop);

    // away neuron 2
    tempX    = xDir[2]    + (        zDir[2] / TAU_REC_FAR)    - ((Use1Dir[2] * xDir[2])    * spike);  // spike [synapse][neuron]
    tempY    = yDir[2]    + (-1 *    yDir[2] / TAU_IN)         + ((Use1Dir[2] * xDir[2])    * spike);
    tempZ    = zDir[2]    + (        yDir[2] / TAU_IN)         - (zDir[2] / TAU_REC_FAR);
    tempUse1 = Use1Dir[2] + (-1 *(Use1Dir[2] / TAU_FACIL_FAR)) + (USE_L1 * (1 - Use1Dir[2]) * spike);
    xDir[2]    = tempX;
    yDir[2]    = tempY;
    zDir[2]    = tempZ;
    Use1Dir[2] = tempUse1;
    if (refractoryDir[2] < 1)
    {
      voltDir[2] = voltDir[2] + (TAU_CONST_INTERNEURON * (voltDir[2] - (R_MEM_BASE_INTERNEURON * (L1_ASE_SIDE * tempY))));
      if (voltDir[2] < 0.0)
        voltDir[2] = 0.0;
    }
    else
      refractoryDir[2]--;
    if (voltDir[2] >= VTH)
    {
      if (printDirection == 1)
      {
        fprintf(dirPrnt, "%ims: spike from away neuron into L3 at %i ISI. \n", loop, (loop - dirISIS[2]) );
        dirISIS[2]       = loop;
      }
      voltDir      [2] = 0.0;
      spikeDir         = 1;
      refractoryDir[2] = 5; // 2ms absolute refractory period, 3ms relative
    }// end if  volt(moderate) >= Vth
    else
      spikeDir = 0;

    // synapse from away from wall direction to L3
    tempX      = xL3away    + (     zL3away    / TAU_REC_L3)    - ((Use1L3away * xL3away) * spikeDir);  // spike [L3neuron][synapse][time]
    tempY      = yL3away    + (-1 * yL3away    / TAU_IN)        + ((Use1L3away * xL3away) * spikeDir);
    tempZ      = zL3away    + (     yL3away    / TAU_IN)        - (zL3away / TAU_REC_L3);
    tempUse1   = Use1L3away + (-1 *(Use1L3away / TAU_FACIL_L3)) + (USE_L3 * (1 - Use1L3away) * spikeDir);
    ItotDir[2] = (L3_ASE * tempY);
    xL3away    = tempX;
    yL3away    = tempY;
    zL3away    = tempZ;
    Use1L3away = tempUse1;
    if (spikeDir==1)
      fprintf(L3, "  %ims: spike from away from wall direction to L3 ItotDir[2]. \n", loop);



    //******************************************************************************************
    //******************************************************************************************
    // INTERNAL DECISION:
    decisionISI++; // increase isi
    if (decisionISI >= decisionISI2spike[region_L3]) // time to spike
    {
      decisionISI = 0;            // reset isi
      spike = 1;
    }
    else
      spike = 0;

    // INTERNAL DECISION: 0.FOLLOW ON LEFT
    // Excitatory synapse (TS=1, so is removed, as there's no point wasting processor cycles to multiply by 1)
    tempX    = decisionX[0]    + (     decisionZ[0]    / TAU_REC_DLEFT)    - ((decisionUse1[0] * decisionX[0]) * spike);
    tempY    = decisionY[0]    + (-1 * decisionY[0]    / TAU_IN)           + ((decisionUse1[0] * decisionX[0]) * spike);
    tempZ    = decisionZ[0]    + (     decisionY[0]    / TAU_IN)           - (decisionZ[0] / TAU_REC_DLEFT);
    tempUse1 = decisionUse1[0] + (-1 *(decisionUse1[0] / TAU_FACIL_DLEFT)) + (USE_D * (1 - decisionUse1[0])    * spike);
    decisionX[0]    = tempX;
    decisionY[0]    = tempY;
    decisionZ[0]    = tempZ;
    decisionUse1[0] = tempUse1;
    if (decisionRefractory[0] < 1)
    {
      decisionVolt[0] = decisionVolt[0] + (TAU_CONST_PYRAMIDAL * (decisionVolt[0] - (R_MEM_BASE_PYRAMIDAL * (D_ASE * tempY))));
      if (decisionVolt[0] < 0.0)
        decisionVolt[0] = 0.0;
    }
    else
      decisionRefractory[0]--;
    if (decisionVolt[0] >= VTH)
    {
      spikeL4Dec            = 1; // spikeL4
      decisionVolt[0]       = 0.0;
      decisionRefractory[0] = 5; // 2ms absolute refractory period, 3ms relative
      if (printL4==1)
      {
        fprintf(L4, "%ims: spike to L4 follow wall on left side region at %i ISI. \n", loop, (loop - decISI[0]));
        decISI[0] = loop;
      }
    }// end if  volt(moderate) >= Vth
    else
      spikeL4Dec = 0; // no spikeL4

    // synapses from left decision neuron to L4
    tempX      = xL4Left    + (     zL4Left    / TAU_REC_L4)    - ((Use1L4Left * xL4Left) * spikeL4Dec);  // spike [L4Decineuron][synapse][time]
    tempY      = yL4Left    + (-1 * yL4Left    / TAU_IN)        + ((Use1L4Left * xL4Left) * spikeL4Dec);
    tempZ      = zL4Left    + (     yL4Left    / TAU_IN)        - (zL4Left / TAU_REC_L4);
    tempUse1   = Use1L4Left + (-1 *(Use1L4Left / TAU_FACIL_L4)) + (USE_L4 * (1 - Use1L4Left) * spikeL4Dec);
    ItotL4[0]  = (L4_ASE_LEFT * tempY);
    xL4Left    = tempX;
    yL4Left    = tempY;
    zL4Left    = tempZ;
    Use1L4Left = tempUse1;


    // INTERNAL DECISION: 1.FOLLOW ON RIGHT
    tempX    = decisionX[1]    + (        decisionZ[1]  / TAU_REC_DRIGHT)    - ((decisionUse1[1] * decisionX[1]) * spike);  // spike [synapse][neuron]
    tempY    = decisionY[1]    + (-1 *    decisionY[1]  / TAU_IN)            + ((decisionUse1[1] * decisionX[1]) * spike);
    tempZ    = decisionZ[1]    + (        decisionY[1]  / TAU_IN)            - (decisionZ[1] / TAU_REC_DRIGHT);
    tempUse1 = decisionUse1[1] + (-1 *(decisionUse1[1]  / TAU_FACIL_DRIGHT)) + (USE_D * (1 - decisionUse1[1])    * spike);
    decisionX[1]    = tempX;
    decisionY[1]    = tempY;
    decisionZ[1]    = tempZ;
    decisionUse1[1] = tempUse1;
    if (decisionRefractory[1] < 1)
    {
      decisionVolt[1] = decisionVolt[1] + (TAU_CONST_PYRAMIDAL * (decisionVolt[1] - (R_MEM_BASE_PYRAMIDAL * (D_ASE * tempY))));
      if (decisionVolt[1] < 0.0)
        decisionVolt[1] = 0.0;
    }
    else
      decisionRefractory[1]--;
    if (decisionVolt[1] >= VTH)
    {
      spikeL4Dec = 1; // spikeL4
      decisionVolt[1]       = 0.0;
      decisionRefractory[1] = 5; // 2ms absolute refractory period, 3ms relative
      if (printL4==1)
      {
        if (loop > printAfter)
          fprintf(L4, "%ims: spike to L4 follow wall on right side region at %i ISI. \n", loop, (loop - decISI[1]));
        decISI[1] = loop;
      }
    }// end if  volt(moderate) >= Vth
    else
      spikeL4Dec = 0; // no spikeL4

    // synapses from right decision neuron to L4
    tempX       = xL4Right    + (     zL4Right    / TAU_REC_L4)    - ((Use1L4Right * xL4Right) * spikeL4Dec);  // spike [L4Decineuron][synapse][time]
    tempY       = yL4Right    + (-1 * yL4Right    / TAU_IN)        + ((Use1L4Right * xL4Right) * spikeL4Dec);
    tempZ       = zL4Right    + (     yL4Right    / TAU_IN)        - (zL4Right / TAU_REC_L4);
    tempUse1    = Use1L4Right + (-1 *(Use1L4Right / TAU_FACIL_L4)) + (USE_L4 * (1 - Use1L4Right) * spikeL4Dec);
    ItotL4[1]   = (L4_ASE_RIGHT * tempY);
    xL4Right    = tempX;
    yL4Right    = tempY;
    zL4Right    = tempZ;
    Use1L4Right = tempUse1;


    // LAYERS 2->4!!!!
    //*******************************************************************
    // Left Cortex
    if (connectedL2[ 0] == 1)  // short (close) left, short (close) front
      computeL2(0, spikeL1[0], spikeL1[3], L4_ASE_LEFT, 0);
    if (connectedL2[ 1] == 1)  // short (close) left, long (target) front
      computeL2(1, spikeL1[0], spikeL1[4], L4_ASE_LEFT, 0);
    if (connectedL2[ 2] == 1)  // short (close) left, long (target) front
      computeL2(2, spikeL1[1], spikeL1[3], L4_ASE_LEFT, 0);
    if (connectedL2[ 3] == 1)  // short (close) left, long (target) front
      computeL2(3, spikeL1[1], spikeL1[4], L4_ASE_LEFT, 0);
    if (connectedL2[ 4] == 1)  // short (close) left, long (target) front
      computeL2(4, spikeL1[2], spikeL1[3], L4_ASE_LEFT, 0);
    if (connectedL2[ 5] == 1)  // short (close) left, long (target) front
      computeL2(5, spikeL1[2], spikeL1[4], L4_ASE_LEFT, 0);
    // Right Cortex
    if (connectedL2[ 6] == 1)  // short (close) left, long (target) front
      computeL2(6, spikeL1[5], spikeL1[3], L4_ASE_RIGHT, 1);
    if (connectedL2[ 7] == 1)  // short (close) left, long (target) front
      computeL2(7, spikeL1[5], spikeL1[4], L4_ASE_RIGHT, 1);
    if (connectedL2[ 8] == 1)  // short (close) left, long (target) front
      computeL2(8, spikeL1[6], spikeL1[3], L4_ASE_RIGHT, 1);
    if (connectedL2[ 9] == 1)  // short (close) left, long (target) front
      computeL2(9, spikeL1[6], spikeL1[4], L4_ASE_RIGHT, 1);
    if (connectedL2[10] == 1)  // short (close) left, long (target) front
      computeL2(10, spikeL1[7], spikeL1[3], L4_ASE_RIGHT, 1);
    if (connectedL2[11] == 1)  // short (close) left, long (target) front
      computeL2(11, spikeL1[7], spikeL1[4], L4_ASE_RIGHT, 1);
    //*******************************************************************


    // LAYER 5 Motor Cortex!!!!
    //*******************************************************************
    computeL5();


  } // end network loop "for (loop=0; loop<NUM_LOOPS; loop++)"


fclose(conn);
fclose(L1);
fclose(L2);
fclose(L3);
fclose(L4);
fclose(learnPrnt);
fclose(directPrnt);
fclose(dirPrnt);
printf("\n\n");
  return 0;
}


void computeL2(int l2cell, int inputSpike1, int inputSpike2, double L4weight, int followSide)
{
  // synapse 1 side
  // Excitatory synapse (TS=1, so is removed, as there's no point wasting processor cycles to multiply by 1)
  tempX    = xL2[0][l2cell]    + (        zL2[0][l2cell] / TAU_REC)    - ((Use1L2[0][l2cell] * xL2[0][l2cell]) * inputSpike1);  // spike [L1neuron]
  tempY    = yL2[0][l2cell]    + (-1 *    yL2[0][l2cell] / TAU_IN)     + ((Use1L2[0][l2cell] * xL2[0][l2cell]) * inputSpike1);
  tempZ    = zL2[0][l2cell]    + (        yL2[0][l2cell] / TAU_IN)     - (zL2[0][l2cell] / TAU_REC);
  tempUse1 = Use1L2[0][l2cell] + (-1 *(Use1L2[0][l2cell] / TAU_FACIL)) + (USE * (1 - Use1L2[0][l2cell])   * inputSpike1);
  Itot = (L2_ASE * tempY);
  xL2[0][l2cell]    = tempX;
  yL2[0][l2cell]    = tempY;
  zL2[0][l2cell]    = tempZ;
  Use1L2[0][l2cell] = tempUse1;
  // synapse 2 front
  tempX    = xL2[1][l2cell]    + (        zL2[1][l2cell] / TAU_REC)    - ((Use1L2[1][l2cell] * xL2[1][l2cell])  * inputSpike2);
  tempY    = yL2[1][l2cell]    + (-1 *    yL2[1][l2cell] / TAU_IN)     + ((Use1L2[1][l2cell] * xL2[1][l2cell])  * inputSpike2);
  tempZ    = zL2[1][l2cell]    + (        yL2[1][l2cell] / TAU_IN)     - (zL2[1][l2cell] / TAU_REC);
  tempUse1 = Use1L2[1][l2cell] + (-1 *(Use1L2[1][l2cell] / TAU_FACIL)) + (USE * (1 - Use1L2[1][l2cell])    * inputSpike2);
  Itot += (L2_ASE * tempY);
  xL2[1][l2cell]    = tempX;
  yL2[1][l2cell]    = tempY;
  zL2[1][l2cell]    = tempZ;
  Use1L2[1][l2cell] = tempUse1;

  if (printL2inputs==1)
  {
    if (inputSpike1==1)
      fprintf(L2, "%ims: spike into L2 cell %i from the side. \n", loop, l2cell);
    if (inputSpike2==1)
      fprintf(L2, "%ims: spike into L2 cell %i from the front. \n", loop, l2cell);
  }

  if (refractoryL2[l2cell] < 1)
    voltL2[l2cell] = voltL2[l2cell] + (TAU_CONST_PYRAMIDAL * (voltL2[l2cell] - (R_MEM_BASE_PYRAMIDAL * Itot)));
  else
  {
    if (refractoryL2[l2cell] < 4) //relative refractory period
      {
      if ((inputSpike1 + inputSpike2) >= 1) // there is a spike
        {
        if (refractoryL2[l2cell] == 3) // relative refractory
          voltL2[l2cell] = voltL2[l2cell] + (TAU_CONST_PYRAMIDAL * (voltL2[l2cell] - (R_MEM_BASE_PYRAMIDAL * Itot))) -0.012;
        else if (refractoryL2[l2cell] == 2) // relative refractory
          voltL2[l2cell] = voltL2[l2cell] + (TAU_CONST_PYRAMIDAL * (voltL2[l2cell] - (R_MEM_BASE_PYRAMIDAL * Itot))) -0.008;
        else
          voltL2[l2cell] = voltL2[l2cell] + (TAU_CONST_PYRAMIDAL * (voltL2[l2cell] - (R_MEM_BASE_PYRAMIDAL * Itot))) -0.004;
        }
      else if (voltL2[l2cell] > 0.0)
        voltL2[l2cell] = voltL2[l2cell] + (TAU_CONST_PYRAMIDAL * (voltL2[l2cell] - (R_MEM_BASE_PYRAMIDAL * Itot))); // let it leak
      }
    refractoryL2[l2cell]--;
    if (voltL2[l2cell] < 0.0)
      voltL2[l2cell] = 0.0;
  }
  if (voltL2[l2cell] >= VTH)
  {
    if (printL2==1)
    {
      if (loop > printAfter)
        fprintf(L2, "%ims: spike from L2 cell %i at %i ISI. \n", loop, l2cell, (loop - L2ISIs[l2cell]));
      L2ISIs[l2cell] = loop;
    }
    voltL2[l2cell]       = 0.0;
    refractoryL2[l2cell] = 5; // 2ms absolute refractory period, 3ms relative
    spikeL2 = 1; // spike Layer 3
  }// end if  volt(moderate) >= Vth
  else
    spikeL2 = 0;


  // Layer 3
  // Are layers 3&4 connected? If so, compute
  num  = l2cell * 3;
  if (connectedL3[num]==1)  // 1st cell from this L2 neuron
    computeL3L4(num, L4weight, 0, followSide);

  num = num + 1;
  if (connectedL3[num]==1)  // 2nd cell from this L2 neuron
    computeL3L4(num, L4weight, 1, followSide);

  num = num + 1;
  if (connectedL3[num]==1) // 3rd cell from this L2 neuron
    computeL3L4(num, L4weight, 2, followSide);
} // end of function computeL2


void computeL3L4(int neuron, double L4weight, int direct, int followSide)
{
  tempX    = xL3[neuron]    + (     zL3[neuron]    / TAU_REC_L3)    - ((Use1L3[neuron] * xL3[neuron]) * spikeL2);  // spike [L3neuron][synapse][time]
  tempY    = yL3[neuron]    + (-1 * yL3[neuron]    / TAU_IN)        + ((Use1L3[neuron] * xL3[neuron]) * spikeL2);
  tempZ    = zL3[neuron]    + (     yL3[neuron]    / TAU_IN)        - (zL3[neuron] / TAU_REC_L3);
  tempUse1 = Use1L3[neuron] + (-1 *(Use1L3[neuron] / TAU_FACIL_L3)) + (USE_L3 * (1 - Use1L3[neuron]) * spikeL2);
  Itot3    = (ItotDir[direct] + (L3_ASE * tempY));
  xL3[neuron] = tempX;
  yL3[neuron] = tempY;
  zL3[neuron] = tempZ;
  Use1L3[neuron] = tempUse1;
  // neuron
  if (refractoryL3[neuron] < 1)
    voltL3[neuron] = voltL3[neuron] + (TAU_CONST_PYRAMIDAL * (voltL3[neuron] - (R_MEM_BASE_PYRAMIDAL * Itot3)));
  else
    refractoryL3[neuron]--;
  if (spikeL2==1)
    fprintf(L3, "  %ims: spike from L2 to L3 cell %i. \n", loop, neuron);
  if (testing==1)
  {
    // Add neuron to the right side?
    indice = ((active_receptors[2] - 2) * 2);
    if (active_receptors[1]==4)
      ++indice;
    indice = (indice * 3) + direction;
    if (neuron==indice)
      fprintf(L3, "%g\n",voltL3[neuron]);
  }


  if (voltL3[neuron] >= VTH)
  {
    if (printL3==1)
    {
      fprintf(L3, "%ims: spike from L3 cell %i at %i ISI. \n", loop, neuron, (loop - L3ISI[neuron]));
      L3ISI[neuron]     = loop;
    }
    voltL3[neuron]       = 0.0;
    refractoryL3[neuron] = 6; // 2ms refractory period
    spike = 1;
  }
  else
    spike = 0;

  // Layer 4
  // synapse from sensor
  tempX    = xL4[neuron]    + (     zL4[neuron]    / TAU_REC_L4_SENSE)    - ((Use1L4[neuron] * xL4[neuron]) * spike);  // spike [L4neuron][synapse][time]
  tempY    = yL4[neuron]    + (-1 * yL4[neuron]    / TAU_IN)              + ((Use1L4[neuron] * xL4[neuron]) * spike);
  tempZ    = zL4[neuron]    + (     yL4[neuron]    / TAU_IN)              - (zL4[neuron] / TAU_REC_L4_SENSE);
  tempUse1 = Use1L4[neuron] + (-1 *(Use1L4[neuron] / TAU_FACIL_L4_SENSE)) + (USE_L4 * (1 - Use1L4[neuron]) * spike);
  Itot     = ItotL4[followSide] + (L4_ASE_SENSE * tempY);
  xL4[neuron] = tempX;
  yL4[neuron] = tempY;
  zL4[neuron] = tempZ;
  Use1L4[neuron]= tempUse1;
  // neuron
  if (refractoryL4[neuron] < 1)
    voltL4[neuron] = voltL4[neuron] + (TAU_CONST_PYRAMIDAL * (voltL4[neuron] - (R_MEM_BASE_PYRAMIDAL * Itot)));
  else
    refractoryL4[neuron]--;

  if (spike==1)
    fprintf(L4, "%ims: spike into L4 cell %i. \n", loop, neuron);

  if (voltL4[neuron] >= VTH)
  {
    if (printL4==1)
    {
      fprintf(L4, "%ims: spike from L4 cell %i at %i ISI. \n", loop, neuron, (loop - L4ISI[neuron]));
      L4ISI[neuron]     = loop;
    }
    voltL4[neuron]       = 0.0;
    refractoryL4[neuron] = 6; // 2ms refractory period
    spike = 1;
  }// end if (voltL3[neuron] >= VTH)
  else
    spike = 0;


  // Layer 5
  if (learned_state[neuron]==-1)
  {
    if (spike == 1)
      learn(neuron, get_choice(neuron), loop);  // get direction and learn
  }
  else
  {
    // Layer 5 forward synapse
    if ((learned_state[neuron] == 0 ) || (learned_state[neuron] >  2 )) // forward or forward turn
    {
      tempX = xL5[neuron][0] + (     zL5[neuron][0] / TAU_REC_L5)  - ((USEL5 * xL5[neuron][0]) * spike);  // spike [L5neuron][synapse][time]
      tempY = yL5[neuron][0] + (-1 * yL5[neuron][0] / TAU_IN)      + ((USEL5 * xL5[neuron][0]) * spike);
      tempZ = zL5[neuron][0] + (     yL5[neuron][0] / TAU_IN)      - (zL5[neuron][0] / TAU_REC_L5);
      xL5[neuron][0] = tempX;
      yL5[neuron][0] = tempY;
      zL5[neuron][0] = tempZ;
      ItotL5[0]     += (ase_L5[neuron][0] * tempY);
      if (printMotor == 1)
        if (spike == 1)
          fprintf(learnPrnt, "%ims: spike into motor neuron 0 from L4 cell %i. \n", loop, neuron);
    }


    // Layer 5 side synapse
    if (learned_state[neuron] > 0 )
    {
      neur  = learned_state[neuron];
      if (neur>2)
        neur = neur - 2;

      tempX = xL5[neuron][neur] + (     zL5[neuron][neur] / TAU_REC_L5)  - ((USEL5 * xL5[neuron][neur]) * spike);  // spike [L5neuron][synapse][time]
      tempY = yL5[neuron][neur] + (-1 * yL5[neuron][neur] / TAU_IN)      + ((USEL5 * xL5[neuron][neur]) * spike);
      tempZ = zL5[neuron][neur] + (     yL5[neuron][neur] / TAU_IN)      - (zL5[neuron][neur] / TAU_REC_L5);
      xL5[neuron][neur] = tempX;
      yL5[neuron][neur] = tempY;
      zL5[neuron][neur] = tempZ;
      ItotL5[neur]     += (ase_L5[neuron][neur] * tempY);
      if (printMotor == 1)
        if (spike == 1)
          fprintf(learnPrnt, "%ims: spike into motor neuron %i from L4 cell %i. \n", loop, neur, neuron);
    }
  }
} // end void computeL3L4(int neuron, double L4weight)


// This function computes the Motor Cortex region of the network
void computeL5()
{
  int direct = -1;

  // Layer 5. Itots already computed. Just add them to the neurons...
  // Forward neuron
  if (refractoryL5[0] < 1)
    voltL5[0] = voltL5[0] + (TAU_CONST_PYRAMIDAL * (voltL5[0] - (R_MEM_BASE_PYRAMIDAL * ItotL5[0])));
  else
    refractoryL5[0]--;

  if (voltL5[0] >= VTH)
  {
    if (printMotor == 1)
    {
        fprintf(learnPrnt, "%ims: spike from forward neuron at %i ISI. \n", loop, (loop - L5ISI[0]) );
        L5ISI[0] = loop;
    }
    //reinitialize_variables();
    if (learning == -1)      // robot not learning
      if (motorMove==1)
        direct = 0;

    readSensors = 1; // read the sensors
  }// end if  volt(moderate) >= Vth


  // Turn Left neuron
  if (refractoryL5[1] < 1)
    voltL5[1] = voltL5[1] + (TAU_CONST_PYRAMIDAL * (voltL5[1] - (R_MEM_BASE_PYRAMIDAL * ItotL5[1])));
  else
    refractoryL5[1]--;

  if (voltL5[1] >= VTH)
  {
    if (printMotor == 1)
    {
        fprintf(learnPrnt, "%ims: spike from left neuron at %i ISI. \n", loop, (loop - L5ISI[1]) );
        L5ISI[1] = loop;
    }
    //reinitialize_variables();
    if (learning == -1)      // robot not learning
      if (motorMove==1)
      {
        if (direct==-1)
        {
          motors(LEFT, FAST);
          if (printLearn==1)
            fprintf(learnPrnt, "%ims: turning left \n", loop);
        }
        else
        {
          motors(FORELEFT, FASTR);
          if (printLearn==1)
            fprintf(learnPrnt, "%ims: moving forward-left \n", loop);
        }
      }
    readSensors = 1; // read the sensors
    direct = 1;
  }// end if  volt(moderate) >= Vth


  // Turn Right neuron
  if (refractoryL5[2] < 1)
    voltL5[2] = voltL5[2] + (TAU_CONST_PYRAMIDAL * (voltL5[2] - (R_MEM_BASE_PYRAMIDAL * ItotL5[2])));
  else
    refractoryL5[2]--;

  if (voltL5[2] >= VTH)
  {
    if (printMotor == 1)
    {
        fprintf(learnPrnt, "%ims: spike from right neuron at %i ISI. \n", loop, (loop - L5ISI[2]) );
        L5ISI[2] = loop;
    }
    //reinitialize_variables();
    if (learning == -1)       // robot not learning
      if (motorMove==1)
      {
        if (direct==-1)
        {
          motors(RIGHT, FAST);
          if (printLearn==1)
            fprintf(learnPrnt, "%ims: turning right \n", loop);
        }
        else
        {
          motors(FORERIGHT, FASTR);
          if (printLearn==1)
            fprintf(learnPrnt, "%ims: moving forward-right \n", loop);
        }
      }
    readSensors = 1; // read the sensors
    direct = 2;
  }// end if  volt(moderate) >= Vth

  if (direct==0)
  {
    motors(FORE, FAST);
    if (printLearn==1)
      fprintf(learnPrnt, "%ims: moving forward \n", loop);
  }

  if (direct!=-1)
    reinitialize_variables();

}


double get_laser_left()
{
  double closestLeft = laserp->ranges[241]; // Sense 0: 241:299 in half degrees
  if (closestLeft > laserp->ranges[243]) closestLeft = laserp->ranges[243];
  if (closestLeft > laserp->ranges[245]) closestLeft = laserp->ranges[245];
  if (closestLeft > laserp->ranges[247]) closestLeft = laserp->ranges[247];
  if (closestLeft > laserp->ranges[249]) closestLeft = laserp->ranges[249];
  if (closestLeft > laserp->ranges[251]) closestLeft = laserp->ranges[251];
  if (closestLeft > laserp->ranges[253]) closestLeft = laserp->ranges[253];
  if (closestLeft > laserp->ranges[255]) closestLeft = laserp->ranges[255];
  if (closestLeft > laserp->ranges[257]) closestLeft = laserp->ranges[257];
  if (closestLeft > laserp->ranges[259]) closestLeft = laserp->ranges[259];
  if (closestLeft > laserp->ranges[261]) closestLeft = laserp->ranges[261];
  if (closestLeft > laserp->ranges[263]) closestLeft = laserp->ranges[263];
  if (closestLeft > laserp->ranges[265]) closestLeft = laserp->ranges[265];
  if (closestLeft > laserp->ranges[267]) closestLeft = laserp->ranges[267];
  if (closestLeft > laserp->ranges[269]) closestLeft = laserp->ranges[269];
  if (closestLeft > laserp->ranges[271]) closestLeft = laserp->ranges[271];
  if (closestLeft > laserp->ranges[273]) closestLeft = laserp->ranges[273];
  if (closestLeft > laserp->ranges[275]) closestLeft = laserp->ranges[275];
  if (closestLeft > laserp->ranges[277]) closestLeft = laserp->ranges[277];
  if (closestLeft > laserp->ranges[279]) closestLeft = laserp->ranges[279];
  if (closestLeft > laserp->ranges[281]) closestLeft = laserp->ranges[281];
  if (closestLeft > laserp->ranges[283]) closestLeft = laserp->ranges[283];
  if (closestLeft > laserp->ranges[285]) closestLeft = laserp->ranges[285];
  if (closestLeft > laserp->ranges[287]) closestLeft = laserp->ranges[287];
  if (closestLeft > laserp->ranges[289]) closestLeft = laserp->ranges[289];
  if (closestLeft > laserp->ranges[291]) closestLeft = laserp->ranges[291];
  if (closestLeft > laserp->ranges[293]) closestLeft = laserp->ranges[293];
  if (closestLeft > laserp->ranges[295]) closestLeft = laserp->ranges[295];
  if (closestLeft > laserp->ranges[297]) closestLeft = laserp->ranges[297];
  if (closestLeft > laserp->ranges[299]) closestLeft = laserp->ranges[299];
  if (closestLeft > laserp->ranges[301]) closestLeft = laserp->ranges[301];
  if (closestLeft > laserp->ranges[303]) closestLeft = laserp->ranges[303];
  if (closestLeft > laserp->ranges[305]) closestLeft = laserp->ranges[305];
  if (closestLeft > laserp->ranges[307]) closestLeft = laserp->ranges[307];
  if (closestLeft > laserp->ranges[309]) closestLeft = laserp->ranges[309];
  if (closestLeft > laserp->ranges[311]) closestLeft = laserp->ranges[311];
  if (closestLeft > laserp->ranges[313]) closestLeft = laserp->ranges[313];
  if (closestLeft > laserp->ranges[315]) closestLeft = laserp->ranges[315];
  if (closestLeft > laserp->ranges[317]) closestLeft = laserp->ranges[317];
  if (closestLeft > laserp->ranges[319]) closestLeft = laserp->ranges[319];
  if (closestLeft > laserp->ranges[321]) closestLeft = laserp->ranges[321];
  if (closestLeft > laserp->ranges[323]) closestLeft = laserp->ranges[323];
  if (closestLeft > laserp->ranges[325]) closestLeft = laserp->ranges[325];
  if (closestLeft > laserp->ranges[327]) closestLeft = laserp->ranges[327];
  if (closestLeft > laserp->ranges[329]) closestLeft = laserp->ranges[329];
  if (closestLeft > laserp->ranges[331]) closestLeft = laserp->ranges[331];
  if (closestLeft > laserp->ranges[333]) closestLeft = laserp->ranges[333];
  if (closestLeft > laserp->ranges[335]) closestLeft = laserp->ranges[335];
  if (closestLeft > laserp->ranges[337]) closestLeft = laserp->ranges[337];
  if (closestLeft > laserp->ranges[339]) closestLeft = laserp->ranges[339];
  if (closestLeft > laserp->ranges[341]) closestLeft = laserp->ranges[341];
  if (closestLeft > laserp->ranges[343]) closestLeft = laserp->ranges[343];
  if (closestLeft > laserp->ranges[345]) closestLeft = laserp->ranges[345];
  if (closestLeft > laserp->ranges[347]) closestLeft = laserp->ranges[347];
  if (closestLeft > laserp->ranges[349]) closestLeft = laserp->ranges[349];
  if (closestLeft > laserp->ranges[351]) closestLeft = laserp->ranges[351];
  if (closestLeft > laserp->ranges[353]) closestLeft = laserp->ranges[353];
  if (closestLeft > laserp->ranges[355]) closestLeft = laserp->ranges[355];
  if (closestLeft > laserp->ranges[357]) closestLeft = laserp->ranges[357];
  if (closestLeft > laserp->ranges[359]) closestLeft = laserp->ranges[359];
  if (closestLeft > laserp->ranges[360]) closestLeft = laserp->ranges[360];

  return closestLeft;
}


double get_laser_front()
{
  double closestFront = laserp->ranges[121]; // Sense 1: 121:239 in half degrees
  if (closestFront > laserp->ranges[123]) closestFront = laserp->ranges[123];
  if (closestFront > laserp->ranges[125]) closestFront = laserp->ranges[125];
  if (closestFront > laserp->ranges[127]) closestFront = laserp->ranges[127];
  if (closestFront > laserp->ranges[129]) closestFront = laserp->ranges[129];
  if (closestFront > laserp->ranges[131]) closestFront = laserp->ranges[131];
  if (closestFront > laserp->ranges[133]) closestFront = laserp->ranges[133];
  if (closestFront > laserp->ranges[135]) closestFront = laserp->ranges[135];
  if (closestFront > laserp->ranges[137]) closestFront = laserp->ranges[137];
  if (closestFront > laserp->ranges[139]) closestFront = laserp->ranges[139];
  if (closestFront > laserp->ranges[141]) closestFront = laserp->ranges[141];
  if (closestFront > laserp->ranges[143]) closestFront = laserp->ranges[143];
  if (closestFront > laserp->ranges[145]) closestFront = laserp->ranges[145];
  if (closestFront > laserp->ranges[147]) closestFront = laserp->ranges[147];
  if (closestFront > laserp->ranges[149]) closestFront = laserp->ranges[149];
  if (closestFront > laserp->ranges[151]) closestFront = laserp->ranges[151];
  if (closestFront > laserp->ranges[153]) closestFront = laserp->ranges[153];
  if (closestFront > laserp->ranges[155]) closestFront = laserp->ranges[155];
  if (closestFront > laserp->ranges[157]) closestFront = laserp->ranges[157];
  if (closestFront > laserp->ranges[159]) closestFront = laserp->ranges[159];
  if (closestFront > laserp->ranges[161]) closestFront = laserp->ranges[161];
  if (closestFront > laserp->ranges[163]) closestFront = laserp->ranges[163];
  if (closestFront > laserp->ranges[165]) closestFront = laserp->ranges[165];
  if (closestFront > laserp->ranges[167]) closestFront = laserp->ranges[167];
  if (closestFront > laserp->ranges[169]) closestFront = laserp->ranges[169];
  if (closestFront > laserp->ranges[171]) closestFront = laserp->ranges[171];
  if (closestFront > laserp->ranges[173]) closestFront = laserp->ranges[173];
  if (closestFront > laserp->ranges[175]) closestFront = laserp->ranges[175];
  if (closestFront > laserp->ranges[177]) closestFront = laserp->ranges[177];
  if (closestFront > laserp->ranges[179]) closestFront = laserp->ranges[179];
  if (closestFront > laserp->ranges[181]) closestFront = laserp->ranges[181];
  if (closestFront > laserp->ranges[183]) closestFront = laserp->ranges[183];
  if (closestFront > laserp->ranges[185]) closestFront = laserp->ranges[185];
  if (closestFront > laserp->ranges[187]) closestFront = laserp->ranges[187];
  if (closestFront > laserp->ranges[189]) closestFront = laserp->ranges[189];
  if (closestFront > laserp->ranges[191]) closestFront = laserp->ranges[191];
  if (closestFront > laserp->ranges[193]) closestFront = laserp->ranges[193];
  if (closestFront > laserp->ranges[195]) closestFront = laserp->ranges[195];
  if (closestFront > laserp->ranges[197]) closestFront = laserp->ranges[197];
  if (closestFront > laserp->ranges[199]) closestFront = laserp->ranges[199];
  if (closestFront > laserp->ranges[201]) closestFront = laserp->ranges[201];
  if (closestFront > laserp->ranges[203]) closestFront = laserp->ranges[203];
  if (closestFront > laserp->ranges[205]) closestFront = laserp->ranges[205];
  if (closestFront > laserp->ranges[207]) closestFront = laserp->ranges[207];
  if (closestFront > laserp->ranges[209]) closestFront = laserp->ranges[209];
  if (closestFront > laserp->ranges[211]) closestFront = laserp->ranges[211];
  if (closestFront > laserp->ranges[213]) closestFront = laserp->ranges[213];
  if (closestFront > laserp->ranges[215]) closestFront = laserp->ranges[215];
  if (closestFront > laserp->ranges[217]) closestFront = laserp->ranges[217];
  if (closestFront > laserp->ranges[219]) closestFront = laserp->ranges[219];
  if (closestFront > laserp->ranges[221]) closestFront = laserp->ranges[221];
  if (closestFront > laserp->ranges[223]) closestFront = laserp->ranges[223];
  if (closestFront > laserp->ranges[225]) closestFront = laserp->ranges[225];
  if (closestFront > laserp->ranges[227]) closestFront = laserp->ranges[227];
  if (closestFront > laserp->ranges[229]) closestFront = laserp->ranges[229];
  if (closestFront > laserp->ranges[231]) closestFront = laserp->ranges[231];
  if (closestFront > laserp->ranges[233]) closestFront = laserp->ranges[233];
  if (closestFront > laserp->ranges[235]) closestFront = laserp->ranges[235];
  if (closestFront > laserp->ranges[237]) closestFront = laserp->ranges[237];
  if (closestFront > laserp->ranges[239]) closestFront = laserp->ranges[239];

  return closestFront;
}


double get_laser_right()
{
  double closestRight = laserp->ranges[0]; // Sense 2: 61:119
  if (closestRight > laserp->ranges[ 1]) closestRight = laserp->ranges[ 1];
  if (closestRight > laserp->ranges[ 3]) closestRight = laserp->ranges[ 3];
  if (closestRight > laserp->ranges[ 5]) closestRight = laserp->ranges[ 5];
  if (closestRight > laserp->ranges[ 7]) closestRight = laserp->ranges[ 7];
  if (closestRight > laserp->ranges[ 9]) closestRight = laserp->ranges[ 9];
  if (closestRight > laserp->ranges[11]) closestRight = laserp->ranges[11];
  if (closestRight > laserp->ranges[13]) closestRight = laserp->ranges[13];
  if (closestRight > laserp->ranges[15]) closestRight = laserp->ranges[15];
  if (closestRight > laserp->ranges[17]) closestRight = laserp->ranges[17];
  if (closestRight > laserp->ranges[19]) closestRight = laserp->ranges[19];
  if (closestRight > laserp->ranges[21]) closestRight = laserp->ranges[21];
  if (closestRight > laserp->ranges[23]) closestRight = laserp->ranges[23];
  if (closestRight > laserp->ranges[25]) closestRight = laserp->ranges[25];
  if (closestRight > laserp->ranges[27]) closestRight = laserp->ranges[27];
  if (closestRight > laserp->ranges[29]) closestRight = laserp->ranges[29];
  if (closestRight > laserp->ranges[31]) closestRight = laserp->ranges[31];
  if (closestRight > laserp->ranges[33]) closestRight = laserp->ranges[33];
  if (closestRight > laserp->ranges[35]) closestRight = laserp->ranges[35];
  if (closestRight > laserp->ranges[37]) closestRight = laserp->ranges[37];
  if (closestRight > laserp->ranges[39]) closestRight = laserp->ranges[39];
  if (closestRight > laserp->ranges[41]) closestRight = laserp->ranges[41];
  if (closestRight > laserp->ranges[43]) closestRight = laserp->ranges[43];
  if (closestRight > laserp->ranges[45]) closestRight = laserp->ranges[45];
  if (closestRight > laserp->ranges[47]) closestRight = laserp->ranges[47];
  if (closestRight > laserp->ranges[49]) closestRight = laserp->ranges[49];
  if (closestRight > laserp->ranges[51]) closestRight = laserp->ranges[51];
  if (closestRight > laserp->ranges[53]) closestRight = laserp->ranges[53];
  if (closestRight > laserp->ranges[55]) closestRight = laserp->ranges[55];
  if (closestRight > laserp->ranges[57]) closestRight = laserp->ranges[57];
  if (closestRight > laserp->ranges[59]) closestRight = laserp->ranges[59];
  if (closestRight > laserp->ranges[61]) closestRight = laserp->ranges[61];
  if (closestRight > laserp->ranges[63]) closestRight = laserp->ranges[63];
  if (closestRight > laserp->ranges[65]) closestRight = laserp->ranges[65];
  if (closestRight > laserp->ranges[67]) closestRight = laserp->ranges[67];
  if (closestRight > laserp->ranges[69]) closestRight = laserp->ranges[69];
  if (closestRight > laserp->ranges[71]) closestRight = laserp->ranges[71];
  if (closestRight > laserp->ranges[73]) closestRight = laserp->ranges[73];
  if (closestRight > laserp->ranges[75]) closestRight = laserp->ranges[75];
  if (closestRight > laserp->ranges[77]) closestRight = laserp->ranges[77];
  if (closestRight > laserp->ranges[79]) closestRight = laserp->ranges[79];
  if (closestRight > laserp->ranges[81]) closestRight = laserp->ranges[81];
  if (closestRight > laserp->ranges[83]) closestRight = laserp->ranges[83];
  if (closestRight > laserp->ranges[85]) closestRight = laserp->ranges[85];
  if (closestRight > laserp->ranges[87]) closestRight = laserp->ranges[87];
  if (closestRight > laserp->ranges[89]) closestRight = laserp->ranges[89];
  if (closestRight > laserp->ranges[91]) closestRight = laserp->ranges[91];
  if (closestRight > laserp->ranges[93]) closestRight = laserp->ranges[93];
  if (closestRight > laserp->ranges[95]) closestRight = laserp->ranges[95];
  if (closestRight > laserp->ranges[97]) closestRight = laserp->ranges[97];
  if (closestRight > laserp->ranges[99]) closestRight = laserp->ranges[99];
  if (closestRight > laserp->ranges[101]) closestRight = laserp->ranges[101];
  if (closestRight > laserp->ranges[103]) closestRight = laserp->ranges[103];
  if (closestRight > laserp->ranges[105]) closestRight = laserp->ranges[105];
  if (closestRight > laserp->ranges[107]) closestRight = laserp->ranges[107];
  if (closestRight > laserp->ranges[109]) closestRight = laserp->ranges[109];
  if (closestRight > laserp->ranges[111]) closestRight = laserp->ranges[111];
  if (closestRight > laserp->ranges[113]) closestRight = laserp->ranges[113];
  if (closestRight > laserp->ranges[115]) closestRight = laserp->ranges[115];
  if (closestRight > laserp->ranges[117]) closestRight = laserp->ranges[117];
  if (closestRight > laserp->ranges[119]) closestRight = laserp->ranges[119];

  return closestRight;
}


// get robot direction
int get_direction()
{
  int closest=0, closest2=0, index=1;

  for (; index<16; index++)
    if (sonar->scan[index] < sonar->scan[closest])
      closest = index;

    for (index=1; index<16; index++)
      if (sonar->scan[index] < sonar->scan[closest2])
        if (index != closest)
          closest2 = index;

  if (region_L3 == 0) // following wall on the left
  {
    if (((closest == 0) || (closest == 15)) && ((closest2 == 0) || (closest2 == 15)))
      return 1; // parallell
    else if (closest < 8) // distance from 0
      return 0; // facing towards wall
    else
      return 2; // facing away from wall
  }
  else //  (region_L3 == 1)
  {
    if (((closest == 7) || (closest == 8)) && ((closest2 == 7) || (closest2 == 8)))
      return 1; // parallell
    else if (closest < 8) // distance from 0
      return 0; // facing towards wall
    else
      return 2; // facing away from wall
  }
}


void motors(int direction, double speed)
{
  playerc_position2d_set_cmd_vel(position2d, STOP, 0.0, STOP, 1);  // stop robot
  motor = direction; // direction of the motor
  switch (direction)
  {
    case FORE:       playerc_position2d_set_cmd_vel(position2d, speed, 0.0,   STOP, 1); break; // move forward
    case LEFT:       playerc_position2d_set_cmd_vel(position2d,  STOP, 0.0,  speed, 1); break; // turn left
    case RIGHT:      playerc_position2d_set_cmd_vel(position2d,  STOP, 0.0, -speed, 1); break; // turn right
    case FORELEFT:   playerc_position2d_set_cmd_vel(position2d, speed, 0.0,  speed, 1); break; // forward-left
    case FORERIGHT:  playerc_position2d_set_cmd_vel(position2d, speed, 0.0, -speed, 1); break; // forward-right
  }
}








// make a choice for the next move
int get_choice(int L4neuron)
{
  if (choices[L4neuron][0]   > choices[L4neuron][1])
  {
    if (choices[L4neuron][0] > choices[L4neuron][2])
      return 0;
    else
      return 2;
  }

  if (choices[L4neuron][1] > choices[L4neuron][2])
    return 1;
  else
    return 2;
}


bool learnMove(int L4neuron, int motorNeuron)
{
  int side, front, sonar;

  if (get_changed(L4neuron)) // states changed
  {
    if (printLearn==1) fprintf(learnPrnt, "Environment changed. Exiting learning sequence. \n");
    return false;
  }

  if (region_L3==0)
    side  = (( 2 * ((int)(get_laser_left()  * 10))) + 160); // convert closest value to decimeter and then to ISI
  else
    side  = (( 2 * ((int)(get_laser_right()  * 10))) + 160); // convert closest value to decimeter and then to ISI
  front = (( 2 * ((int)(get_laser_front() * 10))) + 160); // convert closest value to decimeter and then to ISI
  sonar = get_closest_sonar();
  if (moveRobot(side, front, sonar, motorNeuron))
    return true;

  if (printLearn==1) fprintf(learnPrnt, "Cannot move robot. Exiting learning sequence. \n");
  return false;
}



// returns rewards in range of 0->1.0
// forward is distance,
double get_reward(int L5n, int lastSonar)               // reward [now, t+1, t+2, t+3]
{
  int closestIndx; // sensor reading index
  double minDist, minSide, rewardDirection;

  playerc_client_read(client);          // Wait for new data from server

  //get min sensor readings
  minDist = get_laser_front();                                     // from -30 to +30 degrees
  minSide  = (region_L3==0) ? get_laser_left() : get_laser_right(); // -90 to -30 or 30 to 90 depending on region_L3

  if (L5n==0) // forward movement   Returns 0 -> 0.9
  {
    if (printLearn==1) fprintf(learnPrnt, "    returning min[front, side]:[%g, %g] \n", minDist, minSide);

    // get minimum proximity
    if (minDist > minSide)
      minDist = minSide;

    minDist = (minDist>=0.900) ? 1.0 : (0.1 + minDist);
    return minDist;
  }

  if (dist==-1)
  {
    if ((minDist<0.9) || (minSide<0.9)) // close to the wall
      dist = 0;
    else if (minSide>1.199)             // far from wall
      dist = 1;
    else
      dist = 2;
  }

  // rewardDirection - learn acording to direction
  closestIndx = get_closest_sonar();
  if (region_L3 == 0) // following wall on the left
  {
    if (dist == 0) // close to the wall
    {                                  // target is 14
      switch (closestIndx)
      {
        case 0:  rewardDirection = 5.0; break;
        case 1:  rewardDirection = 4.0; break;
        case 2:  rewardDirection = 3.0; break;
        case 3:  rewardDirection = 2.0; break;
        case 4:  rewardDirection = 1.0; break;
        case 5:  rewardDirection = 0.0; break;
        case 6:  rewardDirection = 0.0; break;
        case 7:  rewardDirection = 1.0; break;
        case 8:  rewardDirection = 2.0; break;
        case 9:  rewardDirection = 3.0; break;
        case 10: rewardDirection = 4.0; break;
        case 11: rewardDirection = 5.0; break;
        case 12: rewardDirection = 6.0; break;
        case 13: rewardDirection = 7.0; break;
        case 14: rewardDirection = 7.0; break;
        case 15: rewardDirection = 6.0;
      }
    } // end if close to wall
    else if (dist == 1)                    // far from wall
    {                                          // target is 1
      switch (closestIndx)
      {
        case 0:  rewardDirection = 6.0; break;
        case 1:  rewardDirection = 7.0; break;
        case 2:  rewardDirection = 7.0; break;
        case 3:  rewardDirection = 6.0; break;
        case 4:  rewardDirection = 5.0; break;
        case 5:  rewardDirection = 4.0; break;
        case 6:  rewardDirection = 3.0; break;
        case 7:  rewardDirection = 2.0; break;
        case 8:  rewardDirection = 1.0; break;
        case 9:  rewardDirection = 0.0; break;
        case 10: rewardDirection = 0.0; break;
        case 11: rewardDirection = 1.0; break;
        case 12: rewardDirection = 2.0; break;
        case 13: rewardDirection = 3.0; break;
        case 14: rewardDirection = 4.0; break;
        case 15: rewardDirection = 5.0;
      }
    } // end else if (minSide>1.199)                    // far from wall
    else // in target range
    {
      switch (closestIndx)
      {
        case 1:  rewardDirection = 6.0; break;
        case 2:  rewardDirection = 5.0; break;
        case 3:  rewardDirection = 4.0; break;
        case 4:  rewardDirection = 3.0; break;
        case 5:  rewardDirection = 2.0; break;
        case 6:  rewardDirection = 1.0; break;
        case 7:  rewardDirection = 0.0; break;
        case 8:  rewardDirection = 0.0; break;
        case 9:  rewardDirection = 1.0; break;
        case 10: rewardDirection = 2.0; break;
        case 11: rewardDirection = 3.0; break;
        case 12: rewardDirection = 4.0; break;
        case 13: rewardDirection = 5.0; break;
        case 14: rewardDirection = 6.0; break;
        case 15:
        case  0: if (lastSonar==-1) // did not move, doing rewardPre
                 {
                   if (get_direction()==1) // parallel
                     rewardDirection = 7.0;
                   else
                     rewardDirection = 6.0;
                 }
                 else if (lastSonar==closestIndx)
                   rewardDirection = 7.0;
                 else
                   rewardDirection = 6.0;
      }
    }
  }
  else // following wall on right side
  {
    if (dist == 0) // close to the wall // close to the wall
    {                                        // target is 9
      switch (closestIndx)
      {
        case 0:  rewardDirection = 1.0; break;
        case 1:  rewardDirection = 0.0; break;
        case 2:  rewardDirection = 0.0; break;
        case 3:  rewardDirection = 1.0; break;
        case 4:  rewardDirection = 2.0; break;
        case 5:  rewardDirection = 3.0; break;
        case 6:  rewardDirection = 4.0; break;
        case 7:  rewardDirection = 5.0; break;
        case 8:  rewardDirection = 6.0; break;
        case 9:  rewardDirection = 7.0; break;
        case 10: rewardDirection = 7.0; break;
        case 11: rewardDirection = 6.0; break;
        case 12: rewardDirection = 5.0; break;
        case 13: rewardDirection = 4.0; break;
        case 14: rewardDirection = 3.0; break;
        case 15: rewardDirection = 2.0;
      }
    } // end if close to wall
    else if (dist == 1)                 // far from wall
    {                                       // target is 6
      switch (closestIndx)
      {
        case 0:  rewardDirection = 2.0; break;
        case 1:  rewardDirection = 3.0; break;
        case 2:  rewardDirection = 4.0; break;
        case 3:  rewardDirection = 5.0; break;
        case 4:  rewardDirection = 6.0; break;
        case 5:  rewardDirection = 7.0; break;
        case 6:  rewardDirection = 7.0; break;
        case 7:  rewardDirection = 6.0; break;
        case 8:  rewardDirection = 5.0; break;
        case 9:  rewardDirection = 4.0; break;
        case 10: rewardDirection = 3.0; break;
        case 11: rewardDirection = 2.0; break;
        case 12: rewardDirection = 1.0; break;
        case 13: rewardDirection = 0.0; break;
        case 14: rewardDirection = 0.0; break;
        case 15: rewardDirection = 1.0;
      }
    } // end else if far from wall
    else // in target range
    {
      switch (closestIndx)
      {
        case 0:  rewardDirection = 0.0; break;
        case 1:  rewardDirection = 1.0; break;
        case 2:  rewardDirection = 2.0; break;
        case 3:  rewardDirection = 3.0; break;
        case 4:  rewardDirection = 4.0; break;
        case 5:  rewardDirection = 5.0; break;
        case 6:  rewardDirection = 6.0; break;
        case 7:
        case 8:  if (lastSonar==-1) // did not move, doing rewardPre
                 {
                   if (get_direction()==1) // parallel
                     rewardDirection = 7.0;
                   else
                     rewardDirection = 6.0;
                 }
                 else if (lastSonar==closestIndx)
                   rewardDirection = 7.0;
                 else
                   rewardDirection = 6.0;
                 break;
        case 9:  rewardDirection = 6.0; break;
        case 10: rewardDirection = 5.0; break;
        case 11: rewardDirection = 4.0; break;
        case 12: rewardDirection = 3.0; break;
        case 13: rewardDirection = 2.0; break;
        case 14: rewardDirection = 1.0; break;
        case 15: rewardDirection = 0.0;
      }
    }
  } // end else // following wall on right side
  rewardDirection /= 7.0;
  if (printLearn==1) fprintf(learnPrnt, "    closestSonar:%i,  returning rewardDirection:%g\n", closestIndx, rewardDirection);

  return rewardDirection; // return reward
} // end function


// returns false if sensor states changed, true if not changed
bool get_changed(int L4neuron)
{ // + 0, 1 2
  double side, front;
  int dr;
  int reg = region_L3 * 2;
  int valueclose  [2] = {0, 5};
  int valuetarget [2] = {1, 6};
  int valuefar    [2] = {2, 7};

  playerc_client_read(client); // Wait for new data from server
  side  = (region_L3==0) ? get_laser_left() : get_laser_right();
  front = get_laser_front();
  dr    = get_direction();        // get direction

  if (side < 0.9)            // close     receptor ISIs 160->176
  {
    if (active_receptors[reg] != valueclose[region_L3])
    {
fprintf(learnPrnt, "\n2063: (active_receptors[%i] != valueclose[%i])  (%i != %i) \n", reg, region_L3, active_receptors[reg], valueclose[region_L3]);
      return true;
    }

    // we know side is close, see if front is close
    if (front < 0.9)             // front is close
    {
      if (region_L3==1)
        dr += 18;
      if (dr != L4neuron)
      {
fprintf(learnPrnt, "\n2074:  (%i != %i) \n", dr, L4neuron);
        return true;     // front and side is low, how about direction...
      }
    }
    else                         // front is far
    {
      if (region_L3==0)
        dr +=  3;
      else
        dr += 21;

      if (dr != L4neuron)
      {
fprintf(learnPrnt, "\n2087:  (%i != %i) \n", dr, L4neuron);
        return true;     // front and side is low, how about direction...
      }
    }
  }
  else if (side < 1.2)       // middle    receptor ISIs 178->182
  {
    if (active_receptors[reg] != valuetarget[region_L3])
    {
fprintf(learnPrnt, "\n2096: (active_receptors[%i] != valuetarget[%i])  (%i != %i) \n", reg, region_L3, active_receptors[reg], valuetarget[region_L3]);
      return true;
    }

    // we know side is mid, see if front is close
    if (front < 0.9)      // front is close
    {
      if (region_L3==0)
        dr +=  6;
      else
        dr += 24;

      if (dr != L4neuron)
      {
fprintf(learnPrnt, "\n2110:  (%i != %i) \n", dr, L4neuron);
        return true;     // front and side is low, how about direction...
      }
    }
    else                  // front is far
    {
      if (region_L3==0)
        dr +=  9;
      else
        dr += 27;

      if (dr != L4neuron)
      {
fprintf(learnPrnt, "\n2123:  (%i != %i) \n", dr, L4neuron);
        return true;     // front and side is low, how about direction...
      }
    }
  }
  else // side is far away
  {
    if (active_receptors[reg] != valuefar[region_L3]) // far receptor ISIs 184->200
    {
fprintf(learnPrnt, "\n2132: (active_receptors[%i] != valuefar[%i])  (%i != %i) \n", reg, region_L3, active_receptors[reg], valuefar[region_L3]);
      return true;
    }

    // we know side is far, see if front is close
    if (front < 0.9)      // front is close
    {
      if (region_L3==0)
        dr += 12;
      else
        dr += 30;

      if (dr != L4neuron)
      {
fprintf(learnPrnt, "\n2146:  (%i != %i) \n", dr, L4neuron);
        return true;     // front and side is low, how about direction...
      }
    }
    else                  // front is far
    {
      if (region_L3==0)
        dr += 15;
      else
        dr += 33;

      if (dr != L4neuron)
      {
fprintf(learnPrnt, "\n2159:  (%i != %i) \n", dr, L4neuron);
        return true;     // front and side is low, how about direction...
      }
    }
  }

  if (front < 0.9)            // close     receptor ISIs 160->176
  {
    if (active_receptors[1] != 3)
    {
fprintf(learnPrnt, "\n2169:  active_receptors[1] == %i \n", active_receptors[1]);
      return true;     // front and side is low, how about direction...
    }
  }
  else //if (front >= 177)
    if (active_receptors[1] != 4) // far       receptor ISIs 178->200
      {
fprintf(learnPrnt, "\n2176:  active_receptors[1] == %i \n", active_receptors[1]);
        return true;     // front and side is low, how about direction...
      }

  return false;
}


// These are learning functions **********************************************
// returns if moved
bool moveRobot(int side, int front, int sonar, int learning)
{

  if (((side < 167) || (front < 167)) && ((sonar < 9) || (sonar == 15))) // too close
  {
    printf("Too close to wall, moving... "); fflush(stdout);
    if (region_L3==0)
    {
      if ((sonar > 1) && (sonar < 8))
        playerc_position2d_set_cmd_vel(position2d, -SLOW, 0.0, -SLOW, 1); // back-left
      else
        playerc_position2d_set_cmd_vel(position2d, -SLOW, 0.0,  SLOW, 1); // back-right

      sleep(2); // wait 2 seconds
      playerc_position2d_set_cmd_vel(position2d, STOP, 0.0,  -SLOW, 1); // turn right
    }
    else
    {
      if (sonar < 7)
        playerc_position2d_set_cmd_vel(position2d, -SLOW, 0.0,  SLOW, 1); // back-right
      else //if ((sonar == 7) || (sonar == 8))
        playerc_position2d_set_cmd_vel(position2d, -SLOW, 0.0, -SLOW, 1); // back-left

      sleep(2); // wait 2 seconds
      playerc_position2d_set_cmd_vel(position2d, STOP, 0.0,  SLOW, 1); // turn left
    }

    sleep(1); // wait 2 seconds
    motors(NOMOVE, STOP);
    //sleep(2); // wait 2 seconds
    //motors(NOMOVE, STOP);
    printf("done \n");

    return false;
  }
  else if ((side > 200) && (front > 200)) // too far
  {
    too_far();
    if (printLearn==1) fprintf(learnPrnt, "Too far from Wall. ");
    return false;
  }

  switch (learning)
  {
    case  0:  playerc_position2d_set_cmd_vel(position2d, SLOW, 0.0,  STOP, 1); break; // move forward
    case  1:  playerc_position2d_set_cmd_vel(position2d, STOP, 0.0,  SLOW, 1); break; // turn left
    case  2:  playerc_position2d_set_cmd_vel(position2d, STOP, 0.0, -SLOW, 1); break; // turn right
    //case  3:  playerc_position2d_set_cmd_vel(position2d, SLOW, 0.0,  SLOW, 1); break; // forward-left
    //case  4:  playerc_position2d_set_cmd_vel(position2d, SLOW, 0.0, -SLOW, 1); break; // forward-right
    default:  if (printLearn==1)
                fprintf(learnPrnt, "Learning is set to < 0. ");
              return false;
  }
  sleep(1); // wait 1 second
  motors(NOMOVE, STOP);
  sleep(1); // wait 1 second
  //playerc_client_read(client); // Wait for new data from server

  return true;
}



// too far from wall
void too_far()
{
  int sri, lear;
  double front, side;
  bool found = false;
  int min [2]   = {121,  1};
  int max [2]   = {360, 239};

  if (region_L3==0)
    lear = 15 + direction;
  else  // (region_L3==1)
    lear = 33 + direction;

  playerc_client_read(client);          // Wait for new data from server
  sri   = get_closest_sonar();

  if (learned_state[lear] == -1) // haven't learned what to do when too far away
  {
    motors(NOMOVE, STOP);
    printf("Too far from wall, turning...   "); fflush(stdout);
    if (region_L3==0)
    {
      playerc_position2d_set_cmd_vel(position2d,  0.1, 0.0,  0.1, 1); // forward-left
      do
      {
        playerc_client_read(client);          // Wait for new data from server
        sri   = get_closest_sonar();
        front = get_laser_front();
        side  = get_laser_left();
      } while ((sri != 1) && (sri != 2) && (front > 1.8) && (side > 1.8));

    }
    else
    { // when too far, turn in approprate direction
      //if ((sri==15) || ((sri<) &&)

      playerc_position2d_set_cmd_vel(position2d,  0.1, 0.0, -0.1, 1); // forward-right
      do
      {
        playerc_client_read(client);          // Wait for new data from server
        sri   = get_closest_sonar();
        front = get_laser_front();
        side  = get_laser_right();
      } while ((sri != 5) && (sri != 6) && (front > 1.8) && (side > 1.8));

    }
    motors(NOMOVE, STOP);
    if (sonar->scan[sri] > 1.999)
    {
      printf("moving towards wall... "); fflush(stdout);
      playerc_position2d_set_cmd_vel(position2d, 0.15, 0.0,   STOP, 1); // move forward
      while (!found)
      {
        playerc_client_read(client); // Wait for new data from server
        for(sri=min[region_L3]; sri<=max[region_L3]; sri=sri+2)
          if (laserp->ranges[sri] < 1.5)
          {
            found = true;
            break;
          }
      }
      motors(NOMOVE, STOP);
    }
    printf("done \n");
    set_sensors();
  }

} // end too_far()


// returns the closest sonar index
int get_closest_sonar()
{
  int sri;
  int closestIndx = 0;
  playerc_client_read(client);                      // Wait for new data from server
  for (sri=1; sri<16; sri++)
    if (sonar->scan[sri] < sonar->scan[closestIndx])
      closestIndx = sri;

  return closestIndx;
}


// returns the closest sonar reading at the back
double get_closest_sonar_back()
{
  int sri;
  double closest;
  playerc_client_read(client);                      // Wait for new data from server

  closest = sonar->scan[9];
  for (sri=10; sri<15; sri++)
    if (closest > sonar->scan[sri])
      closest   = sonar->scan[sri];

  if (region_L3==0)
  {
    if (closest > sonar->scan[15])
      closest   = sonar->scan[15];
  }
  else
  {
    if (closest > sonar->scan[8])
      closest   = sonar->scan[8];
  }

  return closest;
}

// set up starting random values for choices
void set_choices()
{
  time_t t1;
  int l3neur, l4neur, stopLoop;
  (void) time(&t1);
  srand48((long) t1);

// for (l3neur=0; l3neur<12; l3neur++)  // for each L3 neuron
  for (l3neur=0; l3neur<=35; l3neur++)  // for each L3 neuron
  {
    for (l4neur=0; l4neur<3; l4neur++) // for each L4 motor neuron
    {
      if (choices[l3neur][l4neur] == 0)
      {
        do                               // make sure this is different from other weights
        {
          choices[l3neur][l4neur] = (int)(lrand48()/400000000);
          stopLoop=1;
          if ((choices[l3neur][l4neur]<1) || (choices[l3neur][l4neur]>3))
            stopLoop=0;
          else
          {
            for (indice=0; indice<l4neur; indice++)
              if (choices[l3neur][l4neur] == choices[l3neur][indice])
                stopLoop = 0;
          }
        } while (stopLoop==0);
      } // end if (choices[l3neur][l4neur] == 0)
    }// end for l4neur
  } // end for l3neur
}

// update sensor data
void set_sensors()
{
  int sri;
  sleep(1); // wait a second
  playerc_client_read(client); // Wait for new data from server

  // Left ISI
  minSense = laserp->ranges[241];
  for(sri=243; sri<=299; sri=sri+2)
    if (minSense > laserp->ranges[sri])
      minSense =   laserp->ranges[sri];
  sensor[0] = (minSense > 2.000) ? 200 : (( 2 * ((int)(minSense * 10))) + 160);// convert minSense value to decimeter and then to ISI
  // front ISI
  minSense = laserp->ranges[121]; // get minSense reading at the front
  for(sri=123; sri<=239; sri=sri+2)
    if (minSense > laserp->ranges[sri])
      minSense = laserp->ranges[sri];
  sensor[1] = (minSense > 2.000) ? 200 : (( 2 * ((int)(minSense * 10))) + 160);// convert minSense value to decimeter and then to ISI
  // Right ISI
  minSense = laserp->ranges[61];
  for(sri=63; sri<=119; sri=sri+2)
    if (minSense > laserp->ranges[sri])
      minSense =   laserp->ranges[sri];
  sensor[2] = (minSense > 2.000) ? 200 : (( 2 * ((int)(minSense * 10))) + 160);// convert minSense value to decimeter and then to ISI
}



void learn(int L4neuron, int L5neuron, int lpy)
{
  double V, Vnew, td, rewardPre, reward;
  int sonarIndex;
  motors(NOMOVE, STOP); // stop robot
  // 1st learning sequence
  dist = -1;
  if (printLearn==1) fprintf(learnPrnt, "%i: rewardPre: ", lpy);
  rewardPre = get_reward(L5neuron, -1);  // returns   0.0->1.0
  sonarIndex = get_closest_sonar();
  if (learnMove(L4neuron, L5neuron)) // robot has moved and is ready to learn
  {
    choices[L4neuron][L5neuron] = 0;
    if (printLearn==1) fprintf(learnPrnt, "%i: reward:    ", lpy); // print reward
    reward = get_reward(L5neuron, sonarIndex);                                 // new reward after movement
    V      = 1.98;                         //(3 * rewardPre);      // long term value
    Vnew   = 5 * reward - 3 * rewardPre;                           // prediction 2 steps forward
    td     = reward + (GAMMAH * Vnew) - V;                         // temporal difference
    if (printLearn==1) fprintf(learnPrnt, "learn:1, %i, td=%g, reward=%g, Vnew=%g, V=%g, changing ase_L5[%i][%i] from %g to ", choices[L4neuron][L5neuron], td, reward, Vnew, V, L4neuron, L5neuron, ase_L5[L4neuron][L5neuron]);
    ase_L5[L4neuron][L5neuron] = ase_L5[L4neuron][L5neuron] + (MAX_ASE_UPDATE * td);
    if (printLearn==1) fprintf(learnPrnt, "%g \n", ase_L5[L4neuron][L5neuron]);
    sonarIndex = get_closest_sonar();

    // 2nd learning sequence
    if (learnMove(L4neuron, L5neuron)) // robot has moved and is ready to learn
    {
      rewardPre = reward;
      if (printLearn==1) fprintf(learnPrnt, "reward: "); // print reward
      reward = get_reward(L5neuron, sonarIndex);                     // new reward after movement
      V      = Vnew;                                     // long term value
      Vnew   = reward + reward - rewardPre;              // prediction 1 step forward
      td     = reward + (GAMMAH * Vnew) - V;             // temporal difference
      if (printLearn==1) fprintf(learnPrnt, "learn:2, %i: td=%g, reward=%g, Vnew=%g, V=%g, changing ase_L5[%i][%i] from %g to ", choices[L4neuron][L5neuron], td, reward, Vnew, V, L4neuron, L5neuron, ase_L5[L4neuron][L5neuron]);
      ase_L5[L4neuron][L5neuron] = ase_L5[L4neuron][L5neuron] + (MAX_ASE_UPDATE * td);
      if (printLearn==1) fprintf(learnPrnt, "%g \n", ase_L5[L4neuron][L5neuron]);
      sonarIndex = get_closest_sonar();

      // 3rd learning sequence
      if (learnMove(L4neuron, L5neuron)) // robot has moved and is ready to learn
      {
        if (printLearn==1) fprintf(learnPrnt, "reward: "); // print reward
        reward = get_reward(L5neuron, sonarIndex);                     // new reward after movement
        V      = Vnew;                                     // long term value
        Vnew   = 0.0;                                      // no prediction this time
        td     = reward + (GAMMAH * Vnew) - V;             // temporal difference
        if (printLearn==1) fprintf(learnPrnt, "learn:3, %i: td=%g, reward=%g, Vnew=%g, V=%g, changing ase_L5[%i][%i] from %g to ", choices[L4neuron][L5neuron], td, reward, Vnew, V, L4neuron, L5neuron, ase_L5[L4neuron][L5neuron]);
        ase_L5[L4neuron][L5neuron] = ase_L5[L4neuron][L5neuron] + (MAX_ASE_UPDATE * td);
        if (printLearn==1) fprintf(learnPrnt, "%g \n", ase_L5[L4neuron][L5neuron]);
      }
    }
    // see if learning is complete
    if ((choices[L4neuron][0] + choices[L4neuron][1] + choices[L4neuron][2]) == 0)
      learningDone(L4neuron);      // if done with all connections, cut weakest link
    reinitialize_variables();
  } // end if (get_changed(L4neuron)) // states unchanged

  readSensors =  1;
  learning    = -1;
} // end function



// learning is done for one of the connections
// find min weight and cut
void learningDone(int L4neuron)
{
  short int fore = 0;

  // start with forward
  if (ase_L5[L4neuron][0] < L5_ASE)
    ase_L5[L4neuron][0] = 0.0;
  else
    fore = 1;


  // get the strongest turn
  if (ase_L5[L4neuron][1] > ase_L5[L4neuron][2]) // left stronger than right
  {
    if ((ase_L5[L4neuron][1] - ase_L5[L4neuron][2]) < TURN_CUT_OFF) // difference too small for turn
    {
      if (fore==1) // connection to forward cell
      {
        ase_L5[L4neuron][1] = 0.0;     // cut connection
        learned_state[L4neuron] =  0;  // will move forward
      }
      else  // no connection to forward cell
      {
        learned_state[L4neuron] =  1;  // keep left connection
        fprintf(learnPrnt, "%ims, all connections cut, keeping strongest link, left connection from L4 cell %i \n", loop, L4neuron);
      }
    }
    else // difference is enough for turn
      learned_state[L4neuron] = (fore==0) ? 1 : 3; // will move forward-left or left

    ase_L5[L4neuron][2] = 0.0; // cut weakest link
  }
  else //   if (ase_L5[L4neuron][1] <= ase_L5[L4neuron][2]) // right stronger than left
  {
    if ((ase_L5[L4neuron][2] - ase_L5[L4neuron][1]) < TURN_CUT_OFF) // difference too small for turn
    {
      if (fore==1) // connection to forward cell
      {
        ase_L5[L4neuron][2]     = 0.0; // cut connection
        learned_state[L4neuron] = 0;  // will move forward
      }
      else  // no connection to forward cell
      {
        learned_state[L4neuron] =  2;  // keep right connection
        fprintf(learnPrnt, "%ims, all connections cut, keeping strongest link, right connection from L4 cell %i \n", loop, L4neuron);
      }
    }
    else // difference is enough for turn
      learned_state[L4neuron] = (fore==0) ? 2 : 4; // will move forward-right or right

    ase_L5[L4neuron][1] = 0.0; // cut weakest link
  }

  if (printWhatLearned==1)
    fprintf(learnPrnt, "Layer 4 cell %i learned %i \n", L4neuron, learned_state[L4neuron]);
}


void reinitialize_variables()
{
  int neur;

  // Decision cells
  //decisionISI           = 0;
  //decisionX         [0] = 1.0; // synapse
  //decisionY         [0] = 0.0; // synapse
  //decisionZ         [0] = 0.0; // synapse
  //decisionUse1      [0] = 0.0; // synapse
  decisionVolt      [0] = 0.0; // neuron
  decisionRefractory[0] = 0;   // neuron
  //decisionX         [1] = 1.0; // synapse
  //decisionY         [1] = 0.0; // synapse
  //decisionZ         [1] = 0.0; // synapse
  //decisionUse1      [1] = 0.0; // synapse
  decisionVolt      [1] = 0.0; // neuron
  decisionRefractory[1] = 0;   // neuron
  // Layer 3
  xL3faci             = 1.0; // FACING
  yL3faci             = 0.0;
  zL3faci             = 0.0;
  Use1L3faci          = 0.0;
  xL3para             = 1.0; // PARALLEL
  yL3para             = 0.0;
  zL3para             = 0.0;
  Use1L3para          = 0.0;
  xL3away             = 1.0; // AWAY
  yL3away             = 0.0;
  zL3away             = 0.0;
  Use1L3away          = 0.0;
  // Layer 4
  xL4Left             = 1.0;
  xL4Right            = 1.0;
  yL4Left             = 0.0;
  yL4Right            = 0.0;
  zL4Left             = 0.0;
  zL4Right            = 0.0;
  Use1L4Left          = 0.0;
  Use1L4Right         = 0.0;
  ///isiDirection        = 0;

  for (neur=0; neur<36; neur++) // 0 -> 7
  {
    // Layer 2
    if (neur < 12)
    {
      // Layer 1
      if (neur < 8)
      {
        // Direction facing towards, parallel or away from wall
        if (neur < 3)
        {
           //isi           [neur] = 0;
           //xDir          [neur] = 1.0;
           //yDir          [neur] = 0.0;
           //zDir          [neur] = 0.0;
           //Use1Dir       [neur] = 0.0;
           voltDir       [neur] = 0.0;
           //refractoryDir [neur] = 0;
        }

        xL1          [neur] = 1.0;  // cell
        yL1          [neur] = 0.0;  // cell
        zL1          [neur] = 0.0;  // cell
        Use1L1       [neur] = 0.0;  // cell
        voltL1       [neur] = 0.0;  // cell
        refractoryL1 [neur] = 0;    // cell
      }

      xL2       [0][neur] = 1.0;  // synapse, cell
      yL2       [0][neur] = 0.0;  // synapse, cell
      zL2       [0][neur] = 0.0;  // synapse, cell
      Use1L2    [0][neur] = 0.0;  // synapse, cell
      xL2       [1][neur] = 1.0;  // synapse, cell
      yL2       [1][neur] = 0.0;  // synapse, cell
      zL2       [1][neur] = 0.0;  // synapse, cell
      Use1L2    [1][neur] = 0.0;  // synapse, cell
      voltL2       [neur] = 0.0;  // cell
      refractoryL2 [neur] = 0;    // cell
    }

    // Layer 3
    xL3          [neur] = 1.0;
    yL3          [neur] = 0.0;
    zL3          [neur] = 0.0;
    Use1L3       [neur] = 0.0;
    voltL3       [neur] = 0.0;
    refractoryL3 [neur] = 0;

    // Layer 4
    xL4          [neur] = 1.0;
    yL4          [neur] = 0.0;
    zL4          [neur] = 0.0;
    Use1L4       [neur] = 0.0;
    voltL4       [neur] = 0.0;
    refractoryL4 [neur] = 0;

    // Layer 5
    xL5    [neur][0] = 1.0;
    xL5    [neur][1] = 1.0;
    xL5    [neur][2] = 1.0;
    yL5    [neur][0] = 0.0;
    yL5    [neur][1] = 0.0;
    yL5    [neur][2] = 0.0;
    zL5    [neur][0] = 0.0;
    zL5    [neur][1] = 0.0;
    zL5    [neur][2] = 0.0;
    voltL5       [0] = 0.0;
    voltL5       [1] = 0.0;
    voltL5       [2] = 0.0;
    refractoryL5 [0] = 0;
    refractoryL5 [1] = 0;
    refractoryL5 [2] = 0;
  }

}