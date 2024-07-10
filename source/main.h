/* date = July 5th 2024 11:21 am */

#ifndef MAIN_H
#define MAIN_H

#define APP_NAME "namefull"
#define ENABLE_HOTLOAD_VARIABLES 1
#define ENABLE_ASSERT 1

//~ Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//~ FLayer
#include "f_includes.h"

//~ Third-party headers
#include "external/stb_truetype.h"
#include "external/stb_image.h"

//~ *.h
#include "input.h"
#include "camera.h"
#include "renderer.h"

//~ Third-party source
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

//~ *.c
#include "input.c"
#include "camera.c"
#include "renderer.c"

//~ Program

typedef struct Program {
  GLFWwindow* window;
  s32 window_width;
  s32 window_height;
  
  Camera camera;
  
  Mat4f32 view;
  Mat4f32 projection;
  
  f64 current_time;
  f64 delta_time;
  f64 last_time;
  
  f32 near_plane;
  f32 far_plane;
  
  Vec3f32 raycast;
  b32 is_running;
} Program;

Program GlobalProgram;

internal void program_init();
internal void program_tick();

// Glfw 
internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
internal void mouse_cursor_callback(GLFWwindow* window, f64 x_position, f64 y_position);
internal void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods);

#endif //MAIN_H
