This is a simple game I wrote in C. I kept the scope relatively small because I didn't use a game engine - just adding collision, physics, rendering, and simple combat behaviour.

A few interesting features about this project:
-  Batch rendering
  .  The entire vertex buffer (which contains every sprite in the scene) is sent to the GPU once per frame instead of calling multiple draw calls, which is more efficient than using multiple batches and is practical seeing as this is a 2D project.
-  Used openGL to render everything
  .  Every sprite in the scene is rendered as a quad; the project is limited in that quads cannot be rotated.
  .  The same vertex and fragment shader are shared across every quad in the scene, textureIDs were used to distinguish between world-space and screen-space sprites.
-  Texture atlasing was implemented to drastically increase the number of textures able to be shown on the screen at once, instead of being confined to the usual default 32 texture slots on modern-day machines
-  64-bit layer-collision matrix
  .  Collision events can be handled in behaviours independently of the physics system, which was a problem I realized unity had, which is that in unity, if you want a collider belonging to one layer to ignore a collider belonging to another in terms of physics responses, the two collider's collision event cannot be detected within a behaviour.
-  Dictionaries were created from scratch using a custom hash function, so that the most unique hashes can be outputted for the specific requirements of the project (i.e. whichever strings that I happen to need converted to hashes).
-  A static spatial partitioning grid is used to sort entities into cells. The cell size of the grid is determined by the largest collider in the scene and is updated every time a new entity with a collider is created in the scene. The game is limited in this way, in that if an arbitrarily large collider (compared to the size of most of the other colliders in the scene), if such an arbitrarily sized collider, is created in the scene, the grid cell size will become so large that an arbitrarily large number of entities will fit into one cell, hence the time complexity will approach that of a naive solution, where all entities are sorted into one list. Of course the worst case scenario will actually be slightly slower, since the rest of the cells in the grid (the empty linked lists) will still need to be probed through

WASD to move, space to attack. The executable for the game is located in "c-game/x64/Debug/cGame.exe".
