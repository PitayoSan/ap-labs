# Arquitecture and Design

In this document, the decisions regarding the architecture and design of the game will be addressed as concise but explanatory as possible.

The actual requirements of the program can be found in the [README.md](README.md) document, and the reader is encouraged to read that document first in order to correctly interpret the decisions expressed in the present one.

## Structure and Workflow
The overall structure and workflow of the code goes as the following diagram:
```
                           │ 1.
                           │
                      ┌────▼────┐
                      │   a.    │
                      │ main.go │
                      │         │
                      └────┬────┘
                           │
                           │ 2.
                           │
              ┌────────────▼────────────┐
              │           b.            │
     ┌────────┤         game.go         ├────────┐
     │        │                         │        │
     │3.      └┬────────▲─────┬────────▲┘        │
     │         │        │     │        │         │
     │         │        │     │        │         │
     │         │        │     │        │         │
┌────▼────┐    │4.    5.│     │        │         │
│   c.    │    │        │     │        │         │
│ food.go │    │        │     │        │         │
│         │    │        │     │        │         │
└─────────┘   ┌▼────────┴┐    │6.    7.│         │
              │    d.    │    │        │         │
              │ snake.go │    │        │         │
              │          │    │        │         │
              └──────────┘   ┌▼────────┴┐        │8.
                             │    e.    │        │
                             │ enemy.go │        │
                             │          │        │
                             └──────────┘   ┌────▼───┐
                                            │   f.   │
                                            │ hud.go │
                                            │        │
                                            └────────┘
```
_Diagram 1.0: high-level view of the code_

where each box represents a file (named from a. through f.) and each arrow (named from 1. through 8.) expresses communication from one file to another. This in order to express the relative sequence in which the files and their functions are called or executed.  

According to the diagram, the following main/core functions were determined and implemented:

* a. **main.go**:  
  _Responsible for collecting the necessary information form the user as well as setting, instancing, and updating the game._  
  * 1.0. _main()_:
	* sets interface window  
    * executes game object through the supporting library  
  * 1.1. _init()_:  
    * collects and evaluates CLI input  
    * instantiates the game object  
  * 1.2. _Update()_:  
    * updates the game object  
  * 1.3. _Draw()_:  
    * draws the game window  
  * 1.4. _Layout()_:  
    * readjusts window size  
* b. **game.go**:  
  _Core mechanisms of the game, as shown in the diagram. Responsible for in-game operations, such as starting a game, updating assets and game objects, and stopping the game._  
  * 2.0. _NewGame()_:  
    * instantiates game object  
	* generates and tracks food objects  
	* creates main snake and kick-starts its behavior  
	* instantiates enemy snakes and kick-starts their behavior  
	* initializes HUD  
  * 2.1. _GameOver()_:  
    * finishes game due to main snake event  
  * 2.2. _Crashed()_:  
    * finishes game due to crash  
  * 2.3. _Update()_:  
    * tracks the life status of the main snake  
    * updates supporting channels  
    * tracks main snake's feeding and growth processes  
    * tracks enemy snakes' feeding and growth processes  
    * updates food objects' status  
  * 2.4. _Draw()_:  
    * draws background  
    * invokes main snake's drawing process  
    * invokes enemy snakes' drawing processes  
    * invokes hud's drawing process  
    * invokes food objects' drawing processes  
* c. **food.go**:  
  _In charge of generating the food objects as well as drawing them._
  * 3.0. _GenFood()_:  
    * instantiates food object  
	* positions the object in a random point  
  * 3.1. _Update()_:  
	* serves as update method for the supporting library  
	* does nothing at the moment  
  * 3.2. _Draw()_: 
	* draws food object  
* d. **snake.go**:
  _Responsible for creating and managing the main snake object._
  * 4.0. _CreateSnake()_:  
	* instantiates snake object with its assets    
  * 4.1. _Draw()_:  
	* draws head and body of snake according to its positions  
  * 4.2. _UpdatePos()_:  
	* updates position of snake  
  * 4.3. _AddPoint()_:  
	* adds points to the score of the snake and lengthens its body
  * 5.0. _Behavior()_:  
	* manages snake behavior and communicates with the game object 
  * 5.1. _Update()_:  
	* obtains snake direction according to user input  
	* analyzes if snake collided and acts on the game object accordingly 
* e. **enemy.go**:
  _Responsible for creating and managing the enemy snakes objects._  
  * 6.0. _CreateEnemySnake()_:  
    * instantiates enemy snake object with its assets
  * 6.1. _Draw()_:  
	* draws head and body of enemy snake according to its positions  
  * 6.2. _UpdatePos()_:  
	* updates position of enemy snake  
  * 7.0. _Behavior()_:  
	* manages movements by communicating with the game object  
  * 7.1. _Direction()_:  
	* manages the direction of the enemy snake by making random movement choices  
	* detects collisions and acts on the game object accordingly  
* f. **hud.go**:
  _In charge of generating, drawing, and updating the HUD object._  
  * 8.0. _initHud()_:  
    * instantiates the HUD object  
  * 8.1. _Draw()_:  
	* evaluates if game continues  
	* draws the HUD with the appropriate scoring and display texts  

The core functions can access other support functions (not necessarily present in this document) in order to execute the expected functionality.  
Also, the names of the functions present in this document may vary with the actual name of the implemented function, due to slight changes in its functionality and/or the need to have support from support functions as previously stated.  

## Design Choices

* The whole game is encapsulated inside the game object  
	* This decision was necessary due to the used game library, which needed to manipulate the game as a whole in one sole object  
	* Nevertheless, the game object can contain all the necessary structures/objects, as well as execute the necessary functions of those objects, in order to create the desired game  
	* This behavior/pattern can be observed in the diagram 1.0, where the whole game is managed by one entity which is accessed through the main file and method. Additionally, the game entity gives instructions to the other entities and when necessary obtains feedback/information from them  
	* This behavior also let us (even forces us) to use a single instance of the game and map in which we operate all the game object (snakes, food, etc). With this in mind, as previously commented, the objects can give feedback/send information to the centralized game entity (action sort of as clients of the "game server") instead of each of them having a complete copy of it.  
* The enemy snakes' behavior is executed as independent go routines  
	* In order to maintain the desired performance and have the sensation that the enemies run at the same pace as the human player (as well as comply with the given instructions of the project), the behavior of the enemy snakes has to be tracked in separate routines  
	* Each enemy snake tracks its direction, movements, collisions and statuses, and reports the necessary information to the main go routine, which uses that information to have a centralized "perspective" of the game and to track important information, such as the food collisions of all the in-game snakes  
* The main and enemy snakes have many helper/support functions for movements  
	* In order to give a sense of realism and direction on the movements of the snakes, it was necessary to have the appropriate sprites and to track the direction of the movements of the snake at all times  
  * And, in order to do that, we needed to break down the functionality we needed into smaller functions  
  * This allows us to draw the required sprite at just the right moment, and effectively control the graphics of each object  
* The background of the game is static
  * With this type of game and with the used assets/sprites, we thought a static background would benefit the player much more than a dynamic background could.  
  * This because the player can get easily distracted, even dizzy, while trying to track the objects on the screen while the background is moving
* The color palette is limited to grey/black and green
  * We wanted to have simplicity in the colors and appeal to the nostalgia of the players of the original Snake game
