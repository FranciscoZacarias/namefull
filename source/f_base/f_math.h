
#ifndef F_MATH_H
#define F_MATH_H

#define PI 3.14159265358979323846f
#define EPSILON 0.000001f

#define Degrees(r) (r * (180 / PI))
#define Radians(d) (d * (PI / 180))

typedef struct Vector2 {
	union {
		f32 data[2];
		struct {
			f32 x;
			f32 y;
		};
	};
} Vector2;
#define vector2(x,y) (Vector2){x,y}

typedef struct Vector3 {
  union {
    f32 data[3];
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
  };
} Vector3;
#define vector3(x,y,z) (Vector3){x,y,z}

typedef struct Vector4 {
  union {
    f32 data[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} Vector4;
#define vector4(x,y,z,w) (Vector4){x,y,z,w}

typedef struct Matrix4 {
  union {
    f32 data[4][4];
    struct {
      f32 m0, m4, m8,  m12,
      m1, m5, m9,  m13,
      m2, m6, m10, m14,
      m3, m7, m11, m15;
    };
  };
} Matrix4;
#define matrix4(diag) (Matrix4) {diag,0.0f,0.0f,0.0f,0.0f,diag,0.0f,0.0f,0.0f,0.0f,diag,0.0f,0.0f,0.0f,0.0f,diag}

typedef struct Quaternion {
  union {
    f32 data[4];
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
  };
} Quaternion;
#define quaternion(x,y,z,w) (Quaternion){x,y,z,w}
#define quaternion_identity() quaternion(0.0f, 0.0f, 0.0f, 1.0f);

typedef struct Transform {
  Vector3 translation;
  Quaternion rotation;
  Vector3 scale;
} Transform;
#define transform(t,r,s) (Transform){t,r,s}

typedef struct Rayf32 {
  Vector3 point;
  Vector3 direction;
} Rayf32;
#define rayf32(point, direction) (Rayf32){point,direction}

internal f32 f32_clamp(f32 value, f32 min, f32 max);
internal f32 f32_lerp(f32 start, f32 end, f32 amount);
internal f32 f32_normalize(f32 value, f32 start, f32 end);
internal f32 f32_remap(f32 value, f32 inputStart, f32 inputEnd, f32 outputStart, f32 outputEnd);
internal f32 f32_wrap(f32 value, f32 min, f32 max);
internal b32 f32_equals(f32 a, f32 b);

internal f32 vector2_distance(Vector2 a, Vector2 b);
internal f32 vector2_distance_signed(Vector2 a, Vector2 b, Vector2 reference);

internal Vector3 vector3_from_vector4(Vector4 v); /* Discards the w value */
internal void print_vector3(Vector3 v, const char* label);

internal Vector3 vector3_add(Vector3 a, Vector3 b);
internal Vector3 sub(Vector3 a, Vector3 b);
internal Vector3 vector3_mul(Vector3 a, Vector3 b);
internal Vector3 vector3_div(Vector3 a, Vector3 b);
internal Vector3 mul_vector3_matrix4(Vector3 v, Matrix4 m);

internal Vector3 vector3_cross(Vector3 a, Vector3 b);
internal Vector3 vector3_scale(Vector3 v, f32 scalar);
internal Vector3 vector3_scale_xyz(Vector3 v, f32 scale_x, f32 scale_y, f32 scale_z);
internal Vector3 vector3_normalize(Vector3 v);
internal Vector3 vector3_rotate_by_axis(Vector3 v, Vector3 axis, f32 angle);
internal Vector3 vector3_lerp(Vector3 a, Vector3 b, f32 t);
internal Vector3 vector3_unproject(Vector3 source, Matrix4 projection, Matrix4 view);
internal Vector3 mul_vector3_matrix4(Vector3 v, Matrix4 m);

internal f32 vector3_dot(Vector3 a, Vector3 b);
internal f32 vector3_length(Vector3 v);
internal f32 vector3_distance(Vector3 a, Vector3 b);
internal f32 vector3_angle(Vector3 a, Vector3 b);

internal Vector4 vector4_from_vector3(Vector3 v);

internal Vector4 vector4_add(Vector4 a, Vector4 b);
internal Vector4 vector4_sub(Vector4 a, Vector4 b);
internal Vector4 vector4_mul(Vector4 a, Vector4 b);
internal Vector4 vector4_div(Vector4 a, Vector4 b);
internal Vector4 mul_vector4_matrix4(Vector4 v, Matrix4 m);

internal Vector4 vector4_scale(Vector4 v, f32 scalar);
internal Vector4 vector4_normalize(Vector4 v);
internal Vector4 vector4_lerp(Vector4 a, Vector4 b, f32 t);

internal f32 vector4_dot(Vector4 a, Vector4 b);
internal f32 vector4_length(Vector4 v);
internal f32 vector4_distance(Vector4 a, Vector4 b);

internal Matrix4 matrix4_add(Matrix4 left, Matrix4 right);
internal Matrix4 matrix4_sub(Matrix4 left, Matrix4 right); /* Apply the left matrix to the right matrix*/
internal Matrix4 matrix4_mul(Matrix4 left, Matrix4 right); 

internal Matrix4 matrix4_translate(f32 x, f32 y, f32 z);
internal Matrix4 matrix4_rotate_axis(Vector3 axis, f32 radians);
internal Matrix4 matrix4_rotate_x(f32 radians);
internal Matrix4 matrix4_rotate_y(f32 radians);
internal Matrix4 matrix4_rotate_z(f32 radians);
internal Matrix4 matrix4_rotate_xyz(Vector3 radians);
internal Matrix4 matrix4_rotate_zyx(Vector3 radians);

internal Matrix4 matrix4_transpose(Matrix4 m);
internal Matrix4 matrix4_scale(f32 x, f32 y, f32 z);
internal Matrix4 matrix4_frustum(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
internal Matrix4 matrix4_perspective(f64 fovy, f64 window_width, f64 window_height, f64 near_plane, f64 far_plane);
internal Matrix4 matrix4_ortographic(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane);
internal Matrix4 matrix4_look_at(Vector3 eye, Vector3 target, Vector3 up);
internal Matrix4 matrix_from_quaternion(Quaternion q);
internal Transform transform_from_matrix4(Matrix4 mat);

internal Quaternion quaternion_add(Quaternion q1, Quaternion q2);
internal Quaternion quaternion_add_value(Quaternion q, f32 value);
internal Quaternion quaternion_subtract(Quaternion q1, Quaternion q2);
internal Quaternion quaternion_subtract_value(Quaternion q, f32 value);
internal f32        quaternion_length(Quaternion q);
internal Quaternion quaternion_normalize(Quaternion q);
internal Quaternion quaternion_invert(Quaternion q);
internal Quaternion quaternion_multiply(Quaternion q1, Quaternion q2);
internal Quaternion quaternion_scale(Quaternion q, f32 scalar);
internal Quaternion quaternion_divide(Quaternion q1, Quaternion q2);
internal Quaternion quaternion_lerp(Quaternion q1, Quaternion q2, f32 amount);
internal Quaternion quaternion_nlerp(Quaternion q1, Quaternion q2, f32 amount);
internal Quaternion quaternion_slerp(Quaternion q1, Quaternion q2, f32 amount);
internal Quaternion quaternion_cubic_hermit_spline(Quaternion q1, Quaternion outTangent1, Quaternion q2, Quaternion inTangent2, f32 t);
internal Quaternion quaternion_from_vector3_to_vector3(Vector3 from, Vector3 to);
internal Quaternion quaternion_from_matrix(Matrix4 mat);
internal Quaternion quaternion_from_axis_angle(Vector3 axis, f32 angle);
internal void       axis_angle_from_quaternion(Quaternion q, Vector3 *axis, f32 *angle);
internal Quaternion quaternion_from_euler(f32 pitch, f32 yaw, f32 roll);
internal void       euler_from_quaternion(Quaternion q, f32* pitch, f32* yaw, f32* roll);
internal Quaternion quaternion_mul_matric4(Quaternion q, Matrix4 mat);
internal b32        quaternion_equals(Quaternion p, Quaternion q);

internal b32 is_vector_inside_rectangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c);
internal Vector3 intersect_ray_with_plane(Rayf32 line, Vector3 point1, Vector3 point2, Vector3 point3);

#endif // F_MATH_H