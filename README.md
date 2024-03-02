* A project following lab5 from "ECE319K: Introduction to Embedded systems" by Dr.Valvano
* This project was implemented on STM32F103C8T6 kit to design a simple traffic light system for 2 traffic ways : South and West, and a way for pedestrians.
* This project used 3 color LEDs for 2 traffic ways (Red, Green, Yellow) and 2 color LEDs for pedestrians light (Red, White).
* The priority sequence for the lights are: South West Walk. Whenever there is a car (or a person) coming, the sensors (buttons in this project) is on and the system will turn on Green light on one way, redlight on others.
* In Case of 2 or more sensors are working, the lights will follow the priority sequence listed above. 
* The time for the Red lights, Yellow lights and Green lights are 0.5s, 5s and 10s perspectively. The time is dislayed on an LCD screen.
* The main part of the code doesn't used any available library. GPIOs, Interrupt, Systick,.. are coded by declaring registers address and implemented on them (Reading datasheet :>).
* Only the code for the LCD is from other libraries. Link to the github res: https://github.com/weewStack/STM32F1-Tutorial
