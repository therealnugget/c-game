This is a simple game I wrote in C. I kept the scope relatively small because I didn't use a game engine - just adding collision, physics, rendering, and simple combat behaviour.

A few interesting features about this project:
-  batch rendering
  .  the entire vertex buffer (which contains every sprite in the scene) is sent to the GPU once per frame instead of calling multiple draw calls, which is more efficient than using multiple batches and is practical seeing as this is a 2D project.
-  used openGL to render everything
  .  every sprite in the scene is rendered as a quad; the project is limited in that quads cannot be rotated.
  .  the same vertex and fragment shader are shared across every quad in the scene, textureIDs were used to distinguish between world-space and screen-space sprites.
-  64-bit layer-collision matrix
  .  collision events can be handled in behaviours independently of the physics system, which was a problem I realized unity had, which is that in unity, if you want a collider belonging to one layer to ignore a collider belonging to another in terms of physics responses, the two collider's collision event cannot be detected within a behaviour.
-  dictionaries were created from scratch using a custom hash function, so that the most unique hashes can be outputted for the specific requirements of the project (i.e. whichever strings that I happen to need converted to hashes).


WASD to move, space to attack. The executable for the game is located in "c-game/x64/Debug/cGame.exe".
