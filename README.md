# Suspension Solution

Implementation of raycast suspension using UE5's Chaos physics. Requires AsyncTickPhysics Plugin.

>Note: work in progress: to be refactored

Starting point for creating vehicles from scratch.
Each suspension component has the following parameters:

* Length 
>  
* Stiffness 
  > *Dictates how bouncy the suspension is*
* Damping
  >*Prevents vehicle from springing indefinitely. Recommended value is Stiffness / 10*

A suspension component is a raycast. It casts down from the starting point until it touches the ground. The compression value is then calculated.

0 = Fully extended |
1 = Fully compressed

Below is footage from my racing video game, using these principles as a starting base. 

[![Example Video](https://img.youtube.com/vi/875aIVPxfV0/0.jpg)](https://www.youtube.com/watch?v=875aIVPxfV0)

[![Example Video](https://img.youtube.com/vi/IM-Gb9Q90YU/3.jpg)](https://www.youtube.com/watch?v=IM-Gb9Q90YU)
