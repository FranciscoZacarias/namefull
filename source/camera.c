internal Camera camera_init() {
  AssertNoReentry();
  
	Camera camera;
	camera.position = vector3(0.0f, 0.0f, 3.0f);
	camera.front    = vector3(0.0f, 0.0f, 0.0f);
	camera.up       = vector3(0.0f, 0.0f, 0.0f);
	camera.right    = vector3(0.0f, 0.0f, 0.0f);
	camera.pitch    = -0.0f;
	camera.yaw      = -90.0f;
  camera.mode     = CameraMode_Select;
  
  _camera_update(&camera);
  return camera;
}

internal void print_camera(Camera camera) {
  print_vector3(camera.position, "camera.position = ");
  print_vector3(camera.front,    "camera.front    = ");
  print_vector3(camera.up,       "camera.up       = ");
  print_vector3(camera.right,    "camera.right    = ");
  printf("camera.pitch    = %.2ff;\ncamera.yaw      = %.2ff;\n-------------\n", camera.pitch, camera.yaw);
}

internal void camera_update(Camera* camera, f32 delta_time) {
  local_persist b32 was_right_mouse_button_down = 0;
  
  if (input_is_button_down(MouseButton_Right)) {
    if (!was_right_mouse_button_down) {
			// Reset the previous mouse position to the current position
			InputState.mouse_previous.screen_space_x = InputState.mouse_current.screen_space_x;
			InputState.mouse_previous.screen_space_y = InputState.mouse_current.screen_space_y;
			was_right_mouse_button_down = 1;
		}
    
    camera->mode = CameraMode_Fly;
    f32 camera_speed = (CAMERA_SPEED * delta_time);
    
    if (input_is_key_down(KeyboardKey_W)) {
      Vector3 delta = vector3_scale(camera->front, camera_speed);
      camera->position = vector3_add(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_S)) {
      Vector3 delta = vector3_scale(camera->front, camera_speed);
      camera->position = sub(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_A)) {
      Vector3 cross = vector3_cross(camera->front, camera->up);
      Vector3 delta = vector3_scale(cross, camera_speed);
      camera->position = sub(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_D)) {
      Vector3 cross = vector3_cross(camera->front, camera->up);
      Vector3 delta = vector3_scale(cross, camera_speed);
      camera->position = vector3_add(camera->position, delta);
    }
    if (input_is_key_down(KeyboardKey_Q)) {
      camera->position.y -= camera_speed;
    }
    if (input_is_key_down(KeyboardKey_E)) {
      camera->position.y += camera_speed;
    }
    
    f32 x_offset = InputState.mouse_current.screen_space_x  - InputState.mouse_previous.screen_space_x;
    f32 y_offset = InputState.mouse_previous.screen_space_y - InputState.mouse_current.screen_space_y;
    
    camera->yaw   += (x_offset * CAMERA_SENSITIVITY);
    camera->pitch += (y_offset * CAMERA_SENSITIVITY);
    
    if (camera->pitch >  89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
    
    _camera_update(camera);
  } else {
    camera->mode = CameraMode_Select;
    was_right_mouse_button_down = 0;
  }
}

internal void _camera_update(Camera* camera) {
  Vector3 front = vector3(cos(Radians(camera->yaw)) * cos(Radians(camera->pitch)),sin(Radians(camera->pitch)),sin(Radians(camera->yaw)) * cos(Radians(camera->pitch)));
  
  camera->front = vector3_normalize(front);
  Vector3 right = vector3_cross(camera->front, WORLD_UP);
  camera->right = vector3_normalize(right);
  Vector3 up    = vector3_cross(camera->right, camera->front);
  camera->up    = vector3_normalize(up);
}