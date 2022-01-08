# doom-nukem

3D FPS engine everything in C, no hardware acceleration.

* [Frameworks](#frameworks)
* [Constraints](#contraints)
* [Features](#features)
* [Installation](#installation)

## Frameworks

* SDL2 for drawing pixels on screen.
* SDL2_ttf for text rendering.
* SDL2_mixer for sound/music.

## Constraints

Amongst multiple constraints, here is a non exhaustive list of the restrictions we had to follow:

* C is mandatory
* Computing everything on the CPU. No hardware acceleration.
* Rendering has to be done from scratch. No ready made libraries.
* Any crash or memory leak would mean the project has failed.

## Features

### Editing objects

* Controls:
  * Main menu: ````Q````
  * Movement: ````w, a, s, d, space, shift````
  * Toggle mouse in and out of window so you can use menus and move camera: ````tab````
  * Toggle noclip: ````N````
  * Toggle wireframe mode: ````Z````
  * Select/unselect all faces: ````O````
  * Activate door: ````E````
  * Spray: ````T````
  
* By default select faces by clicking them. Cancel selection by clicking skybox or unselect specific faces by clicking them again.
* On the left menu you can change to vertex selection mode. It works in the same way.
* Faces can be triangles or quads. They are automatically optimized when you load obj. Change mode from menu.

### Editing textures
* When faces are selected you can edit textures by clicking UV editor from menu. There you can move vertices by dragging them.
* Controls: ````arrow keys, -, =````
* When two faces UV overlaps it causes problems with baking and sprays. Fix this by selecting overlapping UVs and clicking fix uv overlap. This copies overlapping face and the texture it covers.

### Editing lights
* Go to light editor from level editor menu.
* World brightness adds light to everywhere. This does not affect bloom.
* After adding a light click it to select and edit settings.
* If shadows dont look right got to level editor menu and click ````cull static````
* Use ````raytrace lights```` if you want to quickly see what bake result looks like. This will be slow if you use point lights. (Reduce render resolution before using)
* Once you are done editing, ````bake lights```` to reduce rendertime significantly. Dont bake if UV overlaps on purpose.

### Post processing
* Smooth pixel setting adds gradient inside each pixel when render resolution is less than 100%.
* Bloom limit adjusts amount of light needed for bloom to be rendered.
* SSAO light bias adjusts amount of light needed to cancel SSAO effect.
* Rest is self explanatory. Try rendering only bloom etc. with checkboxes to see what settings do.

### Optimizing
* Reduce render distance. Add fog if you see disappearing faces in the level.
* Reduce amount of triangles.
  * Quads are as fast to render as triangles.
  * The engine will automatically optimize to quads if two triangles make a symmetrical quad when loading object.
  * See what faces are rendered as quads with wireframe quad visualize toggle.
* Reduce render resolution live by pressing ````.```` and ````,```` keys or with slider in main menu settings.

### Adding enemies
* Go to game settings form level editor.
* Animations are saved as series of objects that are interpolated in runtime.
* Edit 3d models and textures by following example structure in embed directory.

### Other
* Game ends when player moves to win position change it in level editor game settings.
* Main menu animation loop can be edited in level editor game settings.

## Installation
* Clone repo
* Run ````make````
* Run ````dnukem````
