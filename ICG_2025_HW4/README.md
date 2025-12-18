Reference from template of HW3

## How to add a shader pipeline?

### add vertex & fragment shading method
- Same as what we do in HW3
1. write your {shader_name}.vert, {shader_name}.frag in src/shaders folder
2. add {shader_name} to shadingMethod of shader_setup() in src/main.cpp
3. uncomment shader_setup() and comment shader_setup_w_geometry_shader() in setup() in src/main.cpp

### add vertex & geometry & fragment shading method
1. write your {shader_name}.vert, {shader_name}.geom, {shader_name}.frag in src/shaders folder
2. add {shader_name} to shadingMethod of shader_setup() in src/main.cpp
3. comment shader_setup() and uncomment shader_setup_w_geometry_shader() in setup() in src/main.cpp