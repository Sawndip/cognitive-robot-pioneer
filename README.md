# bio-realistic-controller
A deep neural network robot navigation system inspired by the sensory fusion and control structures of biological systems written for the Pioneer 3-DX robot for the Player/Stage[1] system.
Details of the system can be found here: http://ieeexplore.ieee.org/document/6220278/

Laser and sonar sensory information are separated into appropriate hidden clusters and subsequently fused through several layers.

The code allows the robot to infer its place in the environment in a biologically realistic manner by routing information through its self-organizing neural network structure using facilitating dynamic synapses [1]. 

After inferencing, the robot learns unsupervised and online to associate the appropriate motor output for every environment it encounters with an emulated dopamine reinforcement signal [3]. 

[1] http://playerstage.sourceforge.net/
[2] M. V. Tsodyks, K. Pawelzik and H. Markram (1998). "Neural networks with dynamic synapses", Neural Computation, vol. 10, no. 4, pp. 821-835.
[3]  R. E. Suri and W. Schultz (1999). A neural network learns a spatial delayed response task with a dopamine-like reinforcement signal. Neuroscience, 91(3), 871–890.
