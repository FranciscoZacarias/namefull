#include "main.h"

int main(int argc, char** argv) {
  os_init();
  Thread_Context main_thread_context;
  thread_context_init_and_attach(&main_thread_context);
  
  program_init();
  renderer_init(GProgram.window_width, GProgram.window_height);
  
  f32 texture_red   = renderer_load_color_texture(1.0, 0.0, 0.0, 1.0);
  f32 texture_green = renderer_load_color_texture(0.0, 1.0, 0.0, 1.0);
  f32 texture_blue  = renderer_load_color_texture(0.0, 0.0, 1.0, 1.0);
  f32 texture_yell  = renderer_load_color_texture(1.0, 1.0, 0.0, 1.0);
  
  renderer_push_line(vec3f32(-8.0f,  0.0f,  0.0f), vec3f32(8.0f, 0.0f, 0.0f), texture_red);
  renderer_push_line(vec3f32( 0.0f, -8.0f,  0.0f), vec3f32(0.0f, 8.0f, 0.0f), texture_green);
  renderer_push_line(vec3f32( 0.0f,  0.0f, -8.0f), vec3f32(0.0f, 0.0f, 8.0f), texture_blue);

  while (GProgram.is_running) {
    program_tick();

    renderer_draw(GProgram.view, GProgram.projection, GProgram.window_width, GProgram.window_height);

    glfwSwapBuffers(GProgram.window);
  }
  
  return 0;
}

internal void
program_tick() {
  input_update(); 
  glfwPollEvents();
  
  GProgram.view = look_at_mat4f32(GProgram.camera.position, add_vec3f32(GProgram.camera.position, GProgram.camera.front), GProgram.camera.up);
  GProgram.projection = perspective_mat4f32(Radians(45), GProgram.window_width, GProgram.window_height, GProgram.near_plane, GProgram.far_plane);
  
  camera_update(&GProgram.camera, GProgram.delta_time);
  
  GProgram.current_time = glfwGetTime();
  GProgram.delta_time   = GProgram.current_time - GProgram.last_time;;
  GProgram.last_time    = GProgram.current_time;
  
  if (GProgram.camera.mode == CameraMode_Select) {
    f32 mouse_x_ndc = (2.0f * InputState.mouse_current.screen_space_x) / GProgram.window_width - 1.0f;
    f32 mouse_y_ndc = 1.0f - (2.0f * InputState.mouse_current.screen_space_y) / GProgram.window_height;
    
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(mouse_x_ndc, mouse_y_ndc, 1.0f), GProgram.projection, GProgram.view);
    GProgram.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(GProgram.camera.position.x, GProgram.camera.position.y, GProgram.camera.position.z)));
  } else {
    GProgram.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
  
  if (glfwWindowShouldClose(GProgram.window) || input_is_key_pressed(KeyboardKey_ESCAPE)) {
    GProgram.is_running = false;
  }
}

internal void 
program_init() {
  AssertNoReentry();
  MemoryZeroStruct(&GProgram);
  
  GProgram.window_width  = 1280;
  GProgram.window_height = 720;
  
  GProgram.view       = mat4f32(1.0f);
  GProgram.projection = mat4f32(1.0f);
  
  GProgram.current_time = 0.0f;
  GProgram.delta_time   = 0.0f;
  GProgram.last_time    = 0.0f;
  
  GProgram.near_plane = 0.1;
  GProgram.far_plane  = 100.0f;
  GProgram.is_running = true;
  
  GProgram.camera      = camera_init();
  input_init(GProgram.window_width, GProgram.window_height);
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GProgram.window = glfwCreateWindow(GProgram.window_width, GProgram.window_height, APP_NAME, NULL, NULL);
  if (GProgram.window == NULL) {
    printf("Failed to create GLFW window");
    Assert(0); 
  }
	
  glfwMakeContextCurrent(GProgram.window);
  glfwSwapInterval(0);
  
	glfwSetFramebufferSizeCallback(GProgram.window, framebuffer_size_callback);
  glfwSetKeyCallback(GProgram.window,             keyboard_callback);
	glfwSetCursorPosCallback(GProgram.window,       mouse_cursor_callback);
  glfwSetMouseButtonCallback(GProgram.window,     mouse_button_callback);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    Assert(0);
  }
}

internal void 
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  GProgram.window_width  = width;
  GProgram.window_height = height;
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