# PID & Fuzzy Logic Controller

## Right Edge Following Behaviour + Obstacle Avoidance

Software implementation of PID and Fuzzy Logic Controller (FLC) of two of the most used behaviours in robotics: Right Edge Following (REF) and Obstacle Avoidance (OA). The PID controller is used only for the REF behaviour, while a Fuzzy Logic controller has been developed to perform the two behaviours independently or a combination between them.
When executing the main file, a first screen will be showed asking the user to pick a behavior to run from:
-	REF with PID;
-	REF with FLC;
-	OA with FLC;
-	OA + REF with FLC.
MobileSim can be used to simulate the performance of such behaviors. Nevertheless, a video representing a real simulation with the Pioneer Robot P3-DX can be seen on my website: www.lorenzoparis.it