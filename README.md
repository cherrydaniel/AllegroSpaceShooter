# AllegroSpaceShooter (or A.S.S)

This is a simple game made for learning purposes (WIP).

### Libraries used:
* **Allegro5** for drawing on the screen and low-level operations
* **EnTT** for ECS management

### Concepts implemented:
* Multi-threaded asset loader
* Fixed tick game loop
* Reading/writing data files
* Observer pattern
* Entity-Component-System
* Allocation tracker
* Circular queue
* Simple event loop for posting tasks to the main thread
* RAII block duration tracker

### TODO:
* Texture atlas
* Enemy death explosion
* Moving background
* Enemy-Player collision
* Change weapon fire rate from "steps" to "cooldown"
* Fix tick steps so bullets don't miss
* RM uninit - OS will clear resources and window won't stall
* Powerups
* Improve bitmap quality
* Enemy AI
* Multiplayer mode
* Joystick support
* Separate overarching game logic from specific screen handlers
* Remove unneeded crap
* Use protobuf for data file IO (if I really feel like it)
* Music + SFX
