#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

// NETWORK
#define NUM_LOOPS 50000000 //80000000  //2240000 about a half hour running   1500000 //in ms 2000 just to get the weights strong enough,
                      // then increase very high to check none other fire

// Synaptic variables
#define USE            0.03 // Usage of synaptic efficacy in range of 0 to 1
//#define TAU_REC        130  // (ms) recovery time constant of the synapse
#define TAU_IN         1.5  // (ms) Inactive time constant of the synapse
//#define TAU_FACIL      530  // (ms) facilitation time constant
#define TAU_REC      600 //260
#define TAU_FACIL   1950 //1100

// For layer 5 synapses
#define USEL5        0.5 //0.1 // 0.03
#define TAU_REC_L5   570   //580  //720  // 775  // 820 // 280  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_L5 2100 //2500  //2300 //3000  //4000 //1000  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up

// Layer 3
#define USE_L3             0.05
#define TAU_REC_L3         250
#define TAU_FACIL_L3       4000
#define TAU_REC_L3_SENSE   10 // L3 from the senses
#define TAU_FACIL_L3_SENSE 2000 // L3 from the senses

//#define TAU_REC_L3_SENSE   5 // L3 from the senses
//#define TAU_FACIL_L3_SENSE 545 // L3 from the senses
#define TAU_REC_L4_SENSE   10 // L3 from the senses
#define TAU_FACIL_L4_SENSE 2000 // L3 from the senses

#define USE_L4       0.05
#define TAU_REC_L4   250
#define TAU_FACIL_L4 4000

// The variables for the 2 synapses below are for the decider synapses
#define USE_D          0.05
//// left synapse.
#define TAU_REC_DLEFT   250  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_DLEFT 980  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up
////// right synapse.
#define TAU_REC_DRIGHT    820  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_DRIGHT 4000  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up

// The variables for the 3 synapses below are for the side synapses
//// close to wall synapse. This is used by both side synapses and front synapse. Maximum Y (0.156469) at frequency 10 for front sensor
#define TAU_REC_CLOSE   200  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_CLOSE 915  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up

//// medium synapse. Maximum Y (0.156224) at frequency 13
#define TAU_REC_MID    505  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_MID 1455  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up

////// far from wall synapse. Maximum Y (0.156212) at frequency 16
#define TAU_REC_FAR    826  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_FAR 4000  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up

// The variables for the synapse below is for the far front synapse
//// long synapse. Maximum Y (0.1582) at ISI 95
#define TAU_REC_FAR_F    804  // increasing moves maximum to 18Hz and down to 0.119477     INCREASE: LARGER isi, down
#define TAU_FACIL_FAR_F 4000  // increasing moves maximum to 20Hz and up to   0.177624     INCREASE: LARGER isi, up

#define D_ASE 4.2e-8  // ASE for decision
// 160 input (left)
// 4.1 OK one firing at 160
// 4.0e-8 too small

// 200 input (right)
// 4.2e-8 OK, one firing at 200
// 4.1 too small

// in range 160:200isi input outputs 160:200isi
// out of range outputs >=320isi
#define L2_ASE 5.04e-8  //  // 4.98e-8 too low           5.1e-8 too high
#define L3_ASE  1.9e-8  // 1.88e-8: 2small
#define L4_ASE_LEFT  8.5e-9  //8.0e-9  // ASE from decision cell Left 8.3e-9ok   8.35e-9_2high
#define L4_ASE_RIGHT 8.85e-9 //8.35e-9 // ASE from decision cell Right     8.5e-9_2high
#define L4_ASE_SENSE 2.0e-8 //2.10e-8 //2.15e-8 // ASE from sense  2.1e-8_2low   2.2e-8_2HIGH
#define USE_L4       0.05

#define L5_ASE 1e-7 //1e-6
#define L5_ASE_CUT 1.055e-7 //1.02e-7 too low    1.06e-7 too high
//#define L4_ASE 5e-8 //4.82e-8  // mid, starting ASE for learning
// 160 isi input:
// >=4.85e-8             1:1 input:output  160 isi output
//   4.77e-8 -> 4.84e-8  2:1 input:output  320 isi output
// <=4.76e-8             no output

// 200 isi input:
//  4.84e-8   1:1 input:output  200 isi output
//  4.81e-8 -> 4.83e-8   2:1 input:output  400 isi output
//  <=4.80e-8    no output

// LIF Neuron variables
//#define R_MEM_BASE 1e+9                       //  (Ohms 1000 * 10^6) Membrane resistence
#define R_MEM_BASE_INTERNEURON 1000000000     //  1 GigaO (GigaOhms) => 1000 * 10^6 (10^9) Membrane resistence for bipolar interneurons
#define R_MEM_BASE_PYRAMIDAL   100000000      //  100 MO  (MegaOhms) => 100 * 10^6 (10^8) Membrane resistence for pyramidal neurons
#define VTH        0.025                      //  (mV) Voltage firing threshold
#define VREST      0.000                      //  (mV)
#define TAU_CONST_INTERNEURON -0.01666666666666666643537 // see below
// TAU_MEM   = 60;    //  (ms) Neuron membrane time constant
// Ts        = 1;    // size of delta T for Euler calculations
// TAU_CONST = (Ts / (-1 * tau_mem)  => (1/(-1 * 60))  => -0.01666666666666666643537
#define TAU_CONST_PYRAMIDAL -0.025 // see below
// TAU_MEM   = 40;    //  (ms) Neuron membrane time constant
// Ts        = 1;    // size of delta T for Euler calculations
// TAU_CONST = (Ts / (-1 * tau_mem)  => (1/(-1 * 60))  => -0.025

// Layer 1
#define USE_L1       0.05
#define L1_ASE_SIDE      6.3173e-9  // 6.3172e-9_2low   6.3175e-9_2high
#define L1_ASE_FRONT     6.32e-9  //  6.325e-9 ok
#define L1_ASE_FRONT_FAR 6.32e-9 //   6.325e-9 ok
#define NUM_L1NEURS  8
#define XL1          {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}
#define YL1          {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define ZL1          {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define USE1L1       {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define VOLTL1       {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define REFRACT_L1   {0,0,0,0,0,0,0,0}
// short  neuron ISIs 59-> 79
// middle neuron ISIs 82-> 87
// long   neuron ISIs 90->111
#define ISIS         {59, 61, 64, 66, 69, 72, 74, 77, 79, 82, 85, 87, 90, 92, 95, 98, 100, 103, 105, 108, 111}// check against incremented ISI for spike

//     FRONT*10 +SIDE         00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22
//#define INDEX_SENSE2L1_LEFT  { 0, 2, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 3, 5}
#define INDEX_SENSE2L1_RIGHT { 6, 8,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 7, 9,11}

#define LOW_MAX     899 // side 2, sensors 2 and 5
#define MID_MIN     900
#define MID_MAX    1049
#define HIGH_MIN   1050
#define HIGH_MAX   2000
#define FRONT_HIGH_MIN  750
#define FRONT_HIGH_MAX 2000

// Layer 2
#define MAX_L2NEURS  12 // maximum number of neurons in each region of layer 2
#define MAX_L2SYNAP  2 // maximum number of synapses for each neuron in layer 2
#define XL2          {{1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}, {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}}
#define YL2          {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}}
#define ZL2          {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}}
#define USE1L2       {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}}
#define SPIKEL2      {{0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0}}
#define VOLTL2       {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define REFRACT_L2   {0,0,0,0,0,0,0,0,0,0,0,0}
#define ISIL2        {0,0,0,0,0,0,0,0,0,0,0,0}
#define L1_L2_COMBOS {{-1,-1,-1,-1,-1,-1,-1,-1,-1}, {-1,-1,-1,-1,-1,-1,-1,-1,-1}}

// Layer 3
#define MAX_L3NEURS 36 // maximum number of neurons in layer 3
#define MAX_L3SYNAP  2 // maximum number of synapses for each neuron in layer 3
#define XL3         {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}
#define YL3         {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define ZL3         {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define USE1L3      {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define SPIKEL3     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define VOLTL3      {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define REFRACT_L3  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define ISIL3       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

// Layer 4
#define MAX_L4NEURS 36 // maximum number of neurons in layer 3
#define MAX_L4SYNAP  2 // maximum number of synapses for each neuron in layer 3
#define XL4         {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}
#define YL4         {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define ZL4         {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define USE1L4      {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define SPIKEL4     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define VOLTL4      {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#define REFRACT_L4  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define ISIL4       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}


// direction
#define MAX_DIRECTION_NEURS 36
#define DIRECTIONS  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define XDIR        {1.0,1.0,1.0}
#define YDIR        {0.0,0.0,0.0}
#define ZDIR        {0.0,0.0,0.0}
#define USE1DIR     {0.0,0.0,0.0}
#define VOLTDIR     {0.0,0.0,0.0}
#define REFRACT_DIR {0,0,0}

// Layer 5
#define MAX_L5NEURS 3 // maximum number of neurons in each region of layer 3 (18^2)
#define MAX_L5SYNAP 36
#define XL5         {{1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}, {1.0,1.0,1.0}}
#define YL5         {{0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}}
#define ZL5         {{0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}}
#define USE1L5      {{0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0}}
#define VOLTL5      { 0.0,0.0,0.0}
#define ITOTL5      { 0.0,0.0,0.0}
#define REFRACT_L5  { 0,  0,  0}
#define SPIKEL5     { 0,  0,  0}
#define ISIL5       { 0,  0,  0}
#define ASEL5       {{L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}, {L5_ASE,L5_ASE,L5_ASE}}

// Learning values
#define TURN_CUT_OFF   4.5e-9 //1e-9
#define MAX_ASE_UPDATE 1e-8 // max update = 0.27e-8
#define MIN_ASE        4.81e-8 //  Minimum ASE before cutting synapse
#define MAX_ASE        4.9e-8
// learning status for every L5 neuron [region L/R][L4neuron]
                                                    // -1=unlearned
                                                   // LEARNING_COMPLETE:
                                                  // 0: chosen forward
                                                 // 1: chosen left
                                                // 2: chosen right
                                               // 3: chosen forward-left
                                              // 4: chosen forward-right
#define GAMMAH        0.99
#define LEARNED       {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
#define CONNECTEDL3   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define VS            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
//#define RS            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define CONNECTED     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define NUMDIRECTIONS { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}
//#define NUMMVS        {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}}
#define VALUE         {{3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}}

#define FAST 0.2
#define FASTR 0.14
//#define FASTLEFT  0.15
//#define FASTRIGHT 0.3
//#define FASTFORELEFT 0.16
//#define FASTFORERIGHT 0.24
#define SLOW 0.35 // 0.2 too fast;  0.15 off +-?
#define STOP 0.0

#define NOMOVE   -1
#define FORE      0
#define LEFT      1
#define RIGHT     2
#define FORELEFT  3
#define FORERIGHT 4

#define READING_COUNTER 4 //3
#define NUMSECONDS_MOVE 3

bool learnMove(int L4neuron, int motorNeuron);
bool get_changed(int L3neuron);
int get_closest_sonar();
double get_closest_sonar_back();
int get_direction();
double get_reward(int L5n, int lastSonar);
//double get_reward(int L5n, int lastSonar, int dist);
//double get_reward_forward();
bool moveRobot(int side, int front, int sonar, int learning); // returns if moved
void learningDone(int L3neuron);
int  get_choice();
void too_far();
void set_sensors();
void learn(int L3Neuron, int L4Neuron, int lpy);
void motors(int direction, double speed);
void set_choices();
double get_laser_left();
double get_laser_front();
double get_laser_right();
void computeL2(int l2cell, int inputSpike1, int inputSpike2, double L4weight, int followSide);
void computeL3L4(int neuron, double L4weight, int direct, int followSide);
//long long timeval_diff(struct timeval *difference, struct timeval *end_time, struct timeval *start_time);
void computeL5();
void reinitialize_variables();

#endif // CLIENT_H_INCLUDED
