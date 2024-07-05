#include "main.h"

typedef struct Window {
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
  
  Input_State input_state;
  
  Vec3f32 raycast;
  b32 is_running;
} Window;

Window MainWindow;
internal void window_init();
internal void window_tick();

// Glfw 
internal void framebuffer_size_callback(GLFWwindow* window, int width, int height);
internal void keyboard_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
internal void mouse_cursor_callback(GLFWwindow* window, f64 x_position, f64 y_position);
internal void mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods);

int 
main(int argc, char** argv) {
  os_init();
  Thread_Context main_thread_context;
  thread_context_init_and_attach(&main_thread_context);
  
  window_init();
  
  while (MainWindow.is_running) {
    window_tick();
    
    glfwSwapBuffers(MainWindow.window);
  }
  
  return 0;
}

internal void
window_tick() {
  glfwPollEvents();
  
  MainWindow.view = look_at_mat4f32(MainWindow.camera.position, add_vec3f32(MainWindow.camera.position, MainWindow.camera.front), MainWindow.camera.up);
  MainWindow.projection = perspective_mat4f32(Radians(45), MainWindow.window_width, MainWindow.window_height, MainWindow.near_plane, MainWindow.far_plane);
  
  camera_update(&MainWindow.camera, MainWindow.input_state, MainWindow.delta_time);
  
  MainWindow.current_time = glfwGetTime();
  MainWindow.delta_time   = MainWindow.current_time - MainWindow.last_time;;
  MainWindow.last_time    = MainWindow.current_time;
  
  if (MainWindow.camera.mode == CameraMode_Select) {
    f32 mouse_x_ndc = (2.0f * MainWindow.input_state.mouse_current.screen_space_x) / MainWindow.window_width - 1.0f;
    f32 mouse_y_ndc = 1.0f - (2.0f * MainWindow.input_state.mouse_current.screen_space_y) / MainWindow.window_height;
    
    Vec3f32 unproject_mouse = unproject_vec3f32(vec3f32(mouse_x_ndc, mouse_y_ndc, 1.0f), MainWindow.projection, MainWindow.view);
    MainWindow.raycast = normalize_vec3f32(sub_vec3f32(vec3f32(unproject_mouse.x, unproject_mouse.y, unproject_mouse.z), vec3f32(MainWindow.camera.position.x, MainWindow.camera.position.y, MainWindow.camera.position.z)));
  } else {
    MainWindow.raycast = vec3f32(F32_MAX, F32_MAX, F32_MAX);
  }
  
  if (glfwWindowShouldClose(MainWindow.window) || input_is_key_pressed(KeyboardKey_ESCAPE)) {
    MainWindow.is_running = false;
  }
}

internal void 
window_init() {
  AssertNoReentry();
  MemoryZeroStruct(&MainWindow);
  
  MainWindow.window_width  = 1280;
  MainWindow.window_height = 720;
  
  MainWindow.view       = mat4f32(1.0f);
  MainWindow.projection = mat4f32(1.0f);
  
  MainWindow.current_time = 0.0f;
  MainWindow.delta_time   = 0.0f;
  MainWindow.last_time    = 0.0f;
  
  MainWindow.near_plane = 0.1;
  MainWindow.far_plane  = 100.0f;
  MainWindow.is_running = true;
  
  MainWindow.camera      = camera_init();
  input_init(&MainWindow.input_state, MainWindow.window_width, MainWindow.window_height);
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  MainWindow.window = glfwCreateWindow(MainWindow.window_width, MainWindow.window_height, APP_NAME, NULL, NULL);
  if (MainWindow.window == NULL) {
    printf("Failed to create GLFW window");
    Assert(0); 
  }
	
  glfwMakeContextCurrent(MainWindow.window);
  glfwSwapInterval(0);
  
	glfwSetFramebufferSizeCallback(MainWindow.window, framebuffer_size_callback);
  glfwSetKeyCallback(MainWindow.window,             keyboard_callback);
	glfwSetCursorPosCallback(MainWindow.window,       mouse_cursor_callback);
  glfwSetMouseButtonCallback(MainWindow.window,     mouse_button_callback);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    Assert(0);
  }
}

internal void 
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  MainWindow.window_width  = width;
  MainWindow.window_height = height;
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