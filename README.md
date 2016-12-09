# `TUNNEL RACING`
======

### [Shenli Yuan](shenliy@stanford.edu) 

## Mission

Move around and avoid getting hit by the blocks flying at you. You win if your score is larger than 0 when the time is up; you lose if your score drop down to 0 before the time is up. Each time you get hit by a regular block, the score will be deducted by 5, and each time you hit a "siren block", the score will add 5. "siren block" is the one rotating and changing colors, you will also hear siren coming from the location of this very block. 

## Instruction
* Move with arrow keys.

* [1] or [2] choosing difficulties: Hard/Harder

* [s] to start or resume.

* [p] to pause.

* [r] to reset (This is not shown in the game UI)

## Features

* [Software Interface] The background music has binaural effect, you will hear music moving around your head. The siren block includes binaural and doppler effects. There will be both visual and audio effect when hit by a block. The game is developed using [openFrameworks](http://openframeworks.cc/). The binaural effect is created using [ofxhoa library](https://github.com/CICM/ofxHoa). Doppler effect is created using delayA from [Synthesis ToolKit](https://ccrma.stanford.edu/software/stk/).

* [Hardware Interface] Actuated infinity mirror is able to create the illusion of a tunnel. What's better is that the tunnel is not static; instead, you will see the tunnul tilting towards different directions. The color of tunnel is created by an RGB LED strip; the RGB channels are controlled with PWM and MOSFETs as switches. The tunnel is formed by the reflections of a two-way mirror and a regular mirror; and the "tilting" is created by two servo motors mounted inside the box along with string-pulley system. An Arduino Uno was used for controlling this infinity mirror as well as communicating with the game. 

* [Integration] The tunnel created by the infinity mirror is synchronized with the tunnel in the game. The tilt towards the same direction. The colors of tunnels in both interfaces are also synchoronized. Please check [this video](https://youtu.be/y8aDIHktlng) for more details. 


=====
## Milestone 1 update
Code based on Assignment 4. Milestone 1 includes a change of field of view when hit by block. The fov moves closer and then gradually moves back. This creates a more genuine feeling of "getting hit" and also allows a little more time to react and recover from the hit.

====
## Milestone 2 update
* Include binaural effect for the sound source.
* Include a "siren" object (with binaural and doppler effect). When this particular object gets hit, the score will increase instead of decreasing like all other objects.
* The color of objects on the screen are changing gradually instead of previous random colors
* The color of the tunnel also changes gradually instead of previous white tunnel.  
* Background color includes a gradient instead of previous black color.


## Pictures
*Infinity Mirror*
![Inifinity Mirror](screenshots/Mirror1.jpg?raw=true "Inifinity Mirror")
![Inifinity Mirror](screenshots/Mirror2.jpg?raw=true "Inifinity Mirror")
![Inifinity Mirror](screenshots/Mirror3.jpg?raw=true "Inifinity Mirror")
*Start Screen*
![Start Screen](screenshots/Img1.png?raw=true "Start Screen")
*Game Screen*
![Game Screen](screenshots/Img2.png?raw=true "Game Screen")
*Pause Screen*
![Pause Screen](screenshots/Img3.png?raw=true "Pause Screen")
*Result Screen*
![Result Screen](screenshots/Img4.png?raw=true "Result Screen")




