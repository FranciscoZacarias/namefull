#include "main.h"

#define Color_White vec4f32(1.0f, 1.0f, 1.0f, 1.0f)

int 
main(int argc, char** argv) {
  os_init();
  Thread_Context main_thread_context;
  thread_context_init_and_attach(&main_thread_context);
  
  program_init();
  renderer_init(GlobalProgram.window_width, GlobalProgram.window_height);
  
  u32 texture_red   = renderer_load_color_texture(1.0, 0.0, 0.0, 1.0);
  u32 texture_green = renderer_load_color_texture(0.0, 1.0, 0.0, 1.0);
  u32 texture_blue  = renderer_load_color_texture(0.0, 0.0, 1.0, 1.0);
  
  renderer_push_line(vec3f32(-8.0f,  0.0f,  0.0f),  vec3f32(8.0f, 0.0f, 0.0f), texture_blue);
  renderer_push_line(vec3f32( 0.0f, -8.0f,  0.0f),  vec3f32(0.0f, 8.0f, 0.0f), texture_green);
  renderer_push_line(vec3f32( 0.0f,  0.0f, -8.0f),  vec3f32(0.0f, 0.0f, 8.0f), texture_blue);
  
  // renderer_push_quad(vec3f32(2.0f, 2.0f, -2.0f), Color_White, 5.0f, 5.0f, texture_green);
  // renderer_push_quad(vec3f32(-7.0f, 2.0f, -2.0f), Color_White, 5.0f, 5.0f, texture_blue);
  
  while (GlobalProgram.is_running) {
    program_tick();
    
    renderer_draw(GlobalProgram.view, GlobalProgram.projection, GlobalProgram.window_width, GlobalProgram.window_height);
    
    glfwSwapBuffers(GlobalProgram.window);
  }
  
  return 0;
}

internal void
program_tick() {
  glfwPollEvents();
  
  GlobalProgram.view = look_at_mat4f32(GlobalProgram.camera.position, add_vec3f32(GlobalProgram.camera.position, GlobalProgram.camera.front), GlobalProgram.camera.up);
  GlobalProgram.projection = perspective_mat4f32(Radians(45), GlobalProgram.window_width, GlobalProgram.window_height, GlobalProgram.near_plane, GlobalProgram.far_plane);
  
  camera_update(&GlobalProgram.camera, GlobalProgram.input_state, GlobalProgram.delta_time);
  
  GlobalProgram.current_time = glfwGetTime();
  GlobalProgram.delta_time   = GlobalProgram.current_time - GlobalProgram.last_time;;
  GlobalProgram.last_time    = GlobalProgram.current_time;
  
  if (GlobalProgram.camera.mode == CameraMode_Select) {
    f32 mouse_x_ndc = (2.0f * GlobalProgram.input_state.mouse_current.screen_space_x) / GlobalProgram.window_width - 1.0f;
    f32 mouse_y_ndc = 1.0f - (2.0f * GlobalProgram.input_state.mouse_current.screen_space_y) / GlobalProgram.window_height;
    
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(mouse_x_ndc, mouse_y_ndc, 1.0f), GlobalProgram.projection, GlobalProgram.view);
    GlobalProgram.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(GlobalProgram.camera.position.x, GlobalProgram.camera.position.y, GlobalProgram.camera.position.z)));
  } else {
    GlobalProgram.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
  
  if (glfwWindowShouldClose(GlobalProgram.window) || input_is_key_pressed(KeyboardKey_ESCAPE)) {
    GlobalProgram.is_running = false;
  }
}

internal void 
program_init() {
  AssertNoReentry();
  MemoryZeroStruct(&GlobalProgram);
  
  GlobalProgram.window_width  = 1280;
  GlobalProgram.window_height = 720;
  
  GlobalProgram.view       = mat4f32(1.0f);
  GlobalProgram.projection = mat4f32(1.0f);
  
  GlobalProgram.current_time = 0.0f;
  GlobalProgram.delta_time   = 0.0f;
  GlobalProgram.last_time    = 0.0f;
  
  GlobalProgram.near_plane = 0.1;
  GlobalProgram.far_plane  = 100.0f;
  GlobalProgram.is_running = true;
  
  GlobalProgram.camera      = camera_init();
  input_init(&GlobalProgram.input_state, GlobalProgram.window_width, GlobalProgram.window_height);
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GlobalProgram.window = glfwCreateWindow(GlobalProgram.window_width, GlobalProgram.window_height, APP_NAME, NULL, NULL);
  if (GlobalProgram.window == NULL) {
    printf("Failed to create GLFW window");
    Assert(0); 
  }
	
  glfwMakeContextCurrent(GlobalProgram.window);
  glfwSwapInterval(0);
  
	glfwSetFramebufferSizeCallback(GlobalProgram.window, framebuffer_size_callback);
  glfwSetKeyCallback(GlobalProgram.window,             keyboard_callback);
	glfwSetCursorPosCallback(GlobalProgram.window,       mouse_cursor_callback);
  glfwSetMouseButtonCallback(GlobalProgram.window,     mouse_button_callback);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    Assert(0);
  }
}

internal void 
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  GlobalProgram.window_width  = width;
  GlobalProgram.window_height = height;
}

internal void 
keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
  switch (key) {
    case 256: key = 0x1B; break; // ESC
    case 258: key = 0x09; break; // TAB
    case 341: key = 0xA2; break; // LEFT CONTROL
    case 261: key = 0x2E; break; // DELETE
    case 340: key = 0xA0; break; // LEFT SHIFT
    case 290: key = 0x70; break; // F1
    case 291: key = 0x71; break; // F2
    case 292: key = 0x72; break; // F3
    case 293: key = 0x73; break; // F4
  }
  
  if (key >= 0 && key <= KEYBOARD_STATE_SIZE) {
    b32 is_pressed = (action != GLFW_RELEASE);
    input_process_keyboard_key(key, is_pressed);
  }
}

internal void 
mouse_cursor_callback(GLFWwindow* window, f64 x_position, f64 y_position) {
  input_process_mouse_cursor(x_position, y_position);
}

internal void 
mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods) {
  b32 is_pressed = (action != GLFW_RELEASE);
  
  if (button == MouseButton_Right && is_pressed) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  if (button < 3) {
    input_process_mouse_button(button, is_pressed);
  }
}