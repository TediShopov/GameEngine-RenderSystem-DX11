

#   📝 Overview

This project is a real-time rendering demo built with **DirectX 11 / C++**.

It simulates an immersive rocky ocean environment where the player pilots a ship capable of destroying procedural terrain. It combines work from three separate university modules, showcasing a range of computer graphics techniques. The implementation covers both foundational and intermediate graphics concepts,

The repository showcases my work in low-level graphics programming and shader-driven rendering pipelines.
It began as part of a university computer graphics module, but has since evolved into a larger project that integrates additional features developed both for coursework and personal exploration.

Here you can see the full overview of this [project](https://tedishopov.github.io/ComputerGraphicsApp.html).

For a full overview of my projects, please visit my [portfolio website](https://tedishopov.github.io/).

<img src="./Images/CGBanner.gif" alt="CG GerstnerWaveControl" />

#   🛠️ Features

*   Procedural terrain generation with tessellation
*   Gerstner wave simulation for realistic water surfaces
*   Cascaded shadow maps for large-scale outdoor scenes
*   Post-processing pipeline (bloom, blur, distortion)
*   Debug UI with ImGui for tweaking parameters in real time


#   ⚙️ Tech Stack

*   Language: **C++**
*   Graphics API: **DirectX 11**
*   Shaders: **HLSL** (vertex, pixel, compute, tessellation)
*   UI: **ImGui**
*   Physics: **Bullet 3D** (optional, for rigid body support)
*   Debug: **RenderDoc** (optional, for rigid body support)


<details>
  <summary><b>📂 Detailed File Map</b></summary>

### Entry Point
- `Scene.h/.cpp` - Central scene manager combining Asset and Render systems, with responsibility for ship instance setup and behavior.

### Sub-Systems
- `AssetSystem.h/.cpp` - loading and indexing of static scene assets
- `RenderSystem.h/.cpp` - handles all shaders and render passes

### Utilities
- `Transform.h/.cpp` - transform hierarchy
- `RenderItemCollection.h/.cpp` - per-shader collection & setup
- `ShaderParameter.h/.cpp` - helpers for initializing shader parameters

### Destructible Terrain - pipeline
1) **Terrain generation & extraction**  
   `Terrain.h/.cpp` - fBM/extra-noise terrain, destroyed-mask sampling, BFS peak extraction (`extractPeakTerrainSubregion`), mesh building.

2) **Procedural cutting/shattering**  
   `ProceduralDestruction.h/.cpp` - `cutMeshOpen` / `cutMeshClosed`, triangle-fan caps, `radialPlaneCutsRandom`.

3) **Gameplay glue (raycast → extract → cut/close → shatter → physics)**  
   `DestructableTerrainPeaks.h/.cpp` - `fireProjectileAt`: raycast to terrain, extract peak, close against plane, shatter, spawn Bullet bodies.

4) **Rendering & mask update**  
   `TerrainTesselationShader.h/.cpp` - shader bindings, CPU staging texture + SRV, `markRegionDestructed`.


</details>

<details>
  <summary><b>🔬 Technical Breakdown</b></summary>


## Water Simulation - Gerstner Waves
*   Implemented a physically-inspired Gerstner Wave model in the domain shader to deform a tessellated water plane.
*   Supports multi-layered waves with editable parameters: steepness, wavelength, speed, and direction.
*   Accurate vertex normal reconstruction was done using derived tangents and bitangents per wave layer to ensure proper lighting and shading.
<!-- <img src="./CG_GerstnerWaveControl.gif" alt="Terrain">-->

<!--<img src="https://s14.gifyu.com/images/bNMhr.gif" alt="CG GerstnerWaveControl" />-->
<img src="/Images/bNMhr.gif" alt="CG GerstnerWaveControl" />


## Parallel Split Shadow Maps (PSSM)
*   Integrated Parallel Split Shadow Mapping (PSSM) for high-quality directional shadows with 3 cascade levels (near, mid, far).
*   Each cascade is calculated using frustum slicing and orthographic projection aligned with the light direction.
*   Light positions and properties fully configurable at runtime via GUI.
<img src="./Images/bNM7j.gif" alt="Parallel-Split Shadow Maps" >
 

## Compute-shader Buoyancy 

*   The boat is physically represented by a rectangular Bullet3D body synced with the visible mesh.
*   A grid of points is sampled across the bottom hull surface each frame.
*   A compute shader evaluates each point using gradient descent to find the corresponding wave height (Gerstner waves require solving for vertical displacement, as they map 3D inputs to 3D outputs).
*   The vertical distance between each hull point and wave surface is used to estimate displaced fluid volume and generate upward buoyant forces.


 <img src="./Images/bNMhY.gif" alt="Buoyancy">


## Screen-Space Reflection (SSR)
Realistic water surface reflections were achieved using a screen-space ray marching algorithm:
*   Ray-marching in screen space with Digital Differential Analyzer (DDA) stepping to prevent over-sampling.
*   Fallback to a cubemap-based skybox when rays leave the view frustum, avoiding visual artifacts at screen edges.
*   Exposed fidelity controls (max steps, step size, reflection opacity) via ImGui interface for real-time tweaking and performance balancing.

<img src="./Images/bNM7R.gif" alt="Scree-Space Reflections" >
<!-- <img src="./CG_ScreenSpaceReflections.gif" alt="Terrain"> -->

## Procedural Terrain With Procedural Destruction
#### Terrain Generation
*   Used fractal Brownian motion (fBM) to procedurally generate rocky peaks emerging from the ocean.
*   GPU terrain is recalculated every frame using dynamic tessellation and vertex displacement.
*   A parallel CPU mesh is generated once and partially recalculated only when geometry is altered (e.g., via cannon fire), enabling accurate ray-triangle intersections and collisions.


<p>
 <img src="./Images/CG_Terrain.png" alt="Terrain" width="45%" >
 <img src="./Images/CG_TerrainNormals.png" alt="Terrain Normals"   width="45%" >
</p>

 <img src="./Images/CG_Tesselation.png" alt="Terrain Normals" width="50%" >

#### Destruction System
*   When terrain is hit by a cannon, the affected mesh region is extracted and converted into a destructible fragment set.
*   A sequence of randomly positioned and oriented splitting planes intersect the geometry and fragment it into jagged, rock-like chunks.
*   Each internal face is automatically inverted and retextured to distinguish the interior from the original surface.

<p>
 <img src="./Images/CG_ProceduralDestruction1.png" alt="Terrain" width="45%" >
 <img src="./Images/CG_ProceduralDestruction2.png" alt="Terrain Normals"   width="45%" >
</p>

## Retro Underwater Effect 
Achieved by combining various post-processing techniques:
*   Edge Blur: Vignette-based Gaussian blur applied only to screen edges using dynamic radial masks.
*   Water Distortion: Ripple effects applied via trigonometric distortion of UVs in the pixel shader.
*   Color Tinting & Magnification: Simulated depth distortion with a center-screen magnifier using aspect-corrected circular masks.
<!-- <img src="./CG_RetroWaterDistortion.gif" alt="Terrain"> -->
<img src="./Images/bNM70.gif" alt="Retro Water Distortion" >

<!--

## Dynamic Tesselation
*   Edge-based tesselation adjusts the subdivision level based on distance to the camera, optimizing detail vs. performance.

## Material Shading - Displacement + Normal Mapping
Rocks and props utilize displacement mapping driven by grayscale height maps, combined with normal mapping for high detail on low-poly meshes.
-->




</details>





