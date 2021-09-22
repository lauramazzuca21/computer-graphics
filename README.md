# computer-graphics
Lab excercises for the Foundations of Computer Graphics exam.  

The library used is OpenGL 4.6 (Compatibility Profile) with Mesa 21.0.3, supported by GLEW 2.1.0.

> The commits in this repo follow the [Gitmoji](https://gitmoji.carloscuesta.me/) guide.

## Lab 00 - 3D Cube
Given the Lab00 setup, modify it to:  
- [x] draw a single cube
- [x] change the color to be applied to each face instead of vertex
- [x] add scaling commands with mouse buttons 

## Lab 01 - 2D Bézier Curves
Given the Lab01 setup, modify it to:
- [x] draw Bézier curve with the de Casteljau algorithm
- [x] drag control points to change their position
- [x] draw Bézier curve with the optimized adaptive subdivision method

## Lab 02 - 2D Lunar Lander Game
- [x] A push from left, D push from right, SPACE thrust
- [x] physics for moving the ship
- [x] particle system for pushes and thrust
- [x] starry background
- [x] win condition: land the lunar module without breaking it

## Lab 03 - 3D Polygonal Meshes Scene Control
Given the Lab03 setup, modify it to:
- [x] compute and store normal vectors of polygonal meshes' vertex
- [x] create a new material
- [x] create Wave Motion in ```v_wave.glsl``` vertex shader
- [x] create Toon Shading in ```v_toon.glsl``` and ```f_toon.glsl```
- [x] add pan horizontal camera navigation (CTRL + mouse wheel up/down)
- [x] enable transformations (rotate - r key, scale - s key, translate - g key) for single objects in the scene wrt either WCS or OCS (pop-up menu to choose which one)
- [x] enable translation of Light object

## Lab 04 - Raytracing
Given the Lab04 setup, modify it to:
- [x] add hard shadow logic
- [x] add soft shadow logic
- [x] add reflective logic

## Lab 05 - Blender mesh modeling and Meshlab
- [x] model a complex object or a scene in Blender  

Use Meshlab for:
- [x] mesh object reconstruction from point clouds (Poisson,MLS, Marching cubes)
- [x] Fill Hole/Mesh Repair tools
- [x] fairing
- [x] decimation
- [x] quality measure tools for surfaces (curvature)

## Lab 06 - Blender rendering - Digital Art
- [x] render a scene with Blender

## Lab 07 - Texture and normal mapping, environmental and refraction
Given the Lab07 setup, modify it with:
- [x] texture mapping 2D on the torus
- [x] texture mapping 2D + Phong shader on the torus
- [x] procedural mapping on torus
- [x] normal mapping
- [x] environment cube mapping: skybox
- [x] environment mapping: object reflection
- [x] Environment mapping: object refraction
- [x] semi-transparent objects