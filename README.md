
Instructions for executing and playing game:

The executable for the game is located in "c-game/x64/Debug/cGame.exe". Alternatively, you can open "cGame.sln" in microsoft visual studio and run the game by pressing "f5". The controls are WASD to move, space to attack.

This is a simple game I wrote in C. I kept the scope relatively small because I didn't use a game engine - just adding collision, physics, rendering, animation, and simple combat behaviour.

A few interesting features about this project:
-  **Batch rendering**<br/>
    - The entire vertex buffer (which contains every sprite in the scene) is sent to the GPU once per frame instead of calling multiple draw calls, which is more efficient than using multiple batches and is practical seeing as this is a 2D project.
-  **Used openGL to render everything**<br/>
    -  Every sprite in the scene is rendered as a quad; the project is limited in that a feature has not been added to allow quads to rotate.
    -  The same vertex and fragment shader are shared across every quad in the scene, textureIDs were used to distinguish between world-space and screen-space sprites.
-  **Texture atlasing was implemented**<br/>
    -  Drastically increases the number of textures able to be shown on the screen at once, instead of being confined to the usual default 32 texture slots on modern-day machines.
-  **64-bit layer-collision matrix**<br/>
    -  Collision events can be handled in behaviours independently of the physics system. This was a problem I realized unity had, which is that in unity, if you want a collider belonging to one layer to ignore a collider belonging to another in terms of physics responses, the two collider's collision event cannot be detected within a behaviour.
-  **Dictionaries were created from scratch**<br/>
    -  I used a custom hash function, so that the most unique hashes can be outputted for the specific requirements of the project (i.e. whichever strings that I happen to need converted to hashes).
-  **A static spatial partitioning grid is used to sort entities into cells for the broad phase collision detection.**<br/>
    -  The cell size of the grid is determined by the largest collider in the scene and is updated every time a new entity with a collider is created in the scene. The game is limited in this way, in that if an arbitrarily large collider (compared to the size of most of the other colliders in the scene), if such an arbitrarily sized collider is created in the scene, the grid cell size will become so large that an arbitrarily large number of entities will fit into one cell, hence the time complexity will approach that of a naive solution, where all entities are sorted into one list and each entity is checked for collisions against one another. Of course the worst case scenario will actually be slightly slower, since the rest of the cells in the grid (the empty linked lists) will still need to be probed through. The project was created this way instead of the traditional approach - choosing an appropriate cell size before runtime, and if an entity is too big sorting it into multiple grid cells - since simplicity and practicality (it was assumed that arbitrarily large entities would not be created) of the project were of high importance in the early stages of the project.
-  **An AABB collision method is used in the physics system**<br/>
    -  The project is limited by this, because separating axis theorem was not used. Hence, colliders cannot be rotated and only square-square collsions are simulated.
-  **Verlet integration was used for physics simulation**<br/>
-  **The physics simulation will check if an entity in the scene has moved before it checks collisions, i.e. if no entities have moved since the last frame, physics will not be simulated in the current frame.**
-  **An "earthquake" coefficient was added to the shader file to denote how much random noise should be added to each vertex, if the shader effect is needed.**
