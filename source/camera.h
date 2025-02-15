/* date = July 5th 2024 1:29 pm */

#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_YAW         -90.0f
#define CAMERA_PITCH        0.0f
#define CAMERA_SENSITIVITY  0.2f
#define WORLD_UP            vector3(0.0f, 1.0f, 0.0f)
#define CAMERA_SPEED        8.0f

typedef enum Camera_Mode {
	CameraMode_Select,
	CameraMode_Fly,
  
  CameraMode_Disabled // NOTE(fz): As in, shouldn't respond to any input
} Camera_Mode;

typedef enum Camera_Movement {
	CameraMovement_Front,
	CameraMovement_Back,
	CameraMovement_Right,
	CameraMovement_Left,
	CameraMovement_Up,
	CameraMovement_Down
} Camera_Movement;

typedef struct Camera {
	Vector3 position;
	Vector3 front;
	Vector3 up;
	Vector3 right;
	f32 yaw;
	f32 pitch;
  
  Camera_Mode mode;
} Camera;

internal Camera camera_init();
internal void print_camera(Camera camera);
internal void camera_update(Camera* camera, f32 delta_time);
internal void camera_mouse_callback(Camera* camera, f64 x_pos, f64 y_pos);
internal void _camera_update(Camera* camera);

#endif //CAMERA_H
