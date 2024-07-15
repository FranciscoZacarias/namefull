internal b32 f32_equals(f32 a, f32 b)
{
#if !defined(EPSILON)
	#define EPSILON 0.000001f
#endif
	b32 result = (fabsf(a - b)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(a), fabsf(b))));
	return result;
}

internal Vector2 vector2(f32 x, f32 y) {
	Vector2 result = {x, y};
	return result;
}

internal f32 vector2_distance(Vector2 a, Vector2 b) {
  f32 result = 0.0f;
	f32 dx = b.x - a.x;
	f32 dy = b.y - a.y;
	result = sqrtf(dx*dx + dy*dy);
	return result;
}

internal f32 vector2_distance_signed(Vector2 a, Vector2 b, Vector2 reference) {
  f32 result = 0.0f;
  f32 dx = b.x - a.x;
  f32 dy = b.y - a.y;
  f32 distance = sqrtf(dx*dx + dy*dy);
  
  f32 sign = (reference.x - a.x) * dy - (reference.y - a.y) * dx;
  
  if (sign < 0) {
    result = -distance;
  } else {
    result = distance;
  }
  
  return result;
}

internal Vector3 vector3(f32 x, f32 y, f32 z) {
	Vector3 result = {x, y, z};
	return result;
}

internal Vector3 vector3_from_vector4(Vector4 v) {
	Vector3 result = { v.x, v.y, v.z };
	return result;
}

internal void print_vector3(Vector3 v, const char* label) {
	printf("%sVector3(%.2ff, %.2ff, %.2ff)\n", label, v.x, v.y, v.z);
}

internal Vector3 vector3_add(Vector3 a, Vector3 b) {
	Vector3 result = {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	};
	return result;
}

internal Vector3 sub(Vector3 a, Vector3 b) {
	Vector3 result = {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z
	};
	return result;
}

internal Vector3 vector3_mul(Vector3 a, Vector3 b) {
	Vector3 result = {
		a.x * b.x,
		a.y * b.y,
		a.z * b.z
	};
	return result;
}

internal Vector3 vector3_div(Vector3 a, Vector3 b) {
	Vector3 result = {
		a.x / b.x,
		a.y / b.y,
		a.z / b.z
	};
	return result;
}

internal Vector4 mul_vector4_matrix4(Vector4 v, Matrix4 m) {
	Vector4 result = { 
		m.m0*v.x + m.m4*v.y + m.m8 *v.z + m.m12*v.w,
		m.m1*v.x + m.m5*v.y + m.m9 *v.z + m.m13*v.w,
		m.m2*v.x + m.m6*v.y + m.m10*v.z + m.m14*v.w,
		m.m3*v.x + m.m7*v.y + m.m11*v.z + m.m15*v.w
	};
	return result;
}

internal Vector3 vector3_cross(Vector3 a, Vector3 b) {
	Vector3 result = {
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	};
	return result;
}

internal Vector3 vector3_scale(Vector3 v, f32 scalar) {
	Vector3 result = {
		v.x*scalar,
		v.y*scalar,
		v.z*scalar
	};
	return result;
}

internal Vector3 vector3_scale_xyz(Vector3 v, f32 scale_x, f32 scale_y, f32 scale_z) {
	Vector3 result = {
		v.x * scale_x,
		v.y * scale_y,
		v.z * scale_z
	};
	return result;	
}

internal Vector3 vector3_normalize(Vector3 v) {
	Vector3 result = v;
	f32 length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (length != 0.0f) {
		f32 ilength = 1.0f/length;
		result.x *= ilength;
		result.y *= ilength;
		result.z *= ilength;
	}
	return result;
}

internal Vector3 mul_vector3_matrix4(Vector3 v, Matrix4 m) {
	Vector3 result = {
		m.m0*v.x + m.m4*v.y + m.m8* v.z + m.m12,
		m.m1*v.x + m.m5*v.y + m.m9* v.z + m.m13,
		m.m2*v.x + m.m6*v.y + m.m10*v.z + m.m14
	};
	return result;
}

internal Vector3 vector3_rotate_by_axis(Vector3 v, Vector3 axis, f32 radians) {
	// Using Euler-Rodrigues Formula
	// Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula
  
	Vector3 result = v;
  
	// Vector3Normalize(axis);
	f32 length = sqrtf(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
	if (length == 0.0f) length = 1.0f;
	f32 ilength = 1.0f/length;
	axis.x *= ilength;
	axis.y *= ilength;
	axis.z *= ilength;
  
	radians /= 2.0f;
	f32 a = sinf(radians);
	f32 b = axis.x*a;
	f32 c = axis.y*a;
	f32 d = axis.z*a;
	a = cosf(radians);
	Vector3 w = { b, c, d };
  
	// Vector3CrossProduct(w, v)
	Vector3 wv = { w.y*v.z - w.z*v.y, w.z*v.x - w.x*v.z, w.x*v.y - w.y*v.x };
  
	// Vector3CrossProduct(w, wv)
	Vector3 wwv = { w.y*wv.z - w.z*wv.y, w.z*wv.x - w.x*wv.z, w.x*wv.y - w.y*wv.x };
  
	// Vector3Scale(wv, 2*a)
	a *= 2;
	wv.x *= a;
	wv.y *= a;
	wv.z *= a;
  
	// Vector3Scale(wwv, 2)
	wwv.x *= 2;
	wwv.y *= 2;
	wwv.z *= 2;
  
	result.x += wv.x;
	result.y += wv.y;
	result.z += wv.z;
  
	result.x += wwv.x;
	result.y += wwv.y;
	result.z += wwv.z;
  
	return result;
}

internal Vector3 vector3_lerp(Vector3 a, Vector3 b, f32 t) {
	Vector3 result = {
		a.x + t*(b.x - a.x),
		a.y + t*(b.y - a.y),
		a.z + t*(b.z - a.z)
	};
	return result;
}

internal Vector3 vector3_unproject(Vector3 source, Matrix4 projection, Matrix4 view) {
  Vector3 result = { 0 };
  
  // Calculate unprojected matrix (multiply view matrix by projection matrix) and invert it
  Matrix4 matViewProj = {      // MatrixMultiply(view, projection);
    view.m0*projection.m0 + view.m1*projection.m4 + view.m2*projection.m8 + view.m3*projection.m12,
    view.m0*projection.m1 + view.m1*projection.m5 + view.m2*projection.m9 + view.m3*projection.m13,
    view.m0*projection.m2 + view.m1*projection.m6 + view.m2*projection.m10 + view.m3*projection.m14,
    view.m0*projection.m3 + view.m1*projection.m7 + view.m2*projection.m11 + view.m3*projection.m15,
    view.m4*projection.m0 + view.m5*projection.m4 + view.m6*projection.m8 + view.m7*projection.m12,
    view.m4*projection.m1 + view.m5*projection.m5 + view.m6*projection.m9 + view.m7*projection.m13,
    view.m4*projection.m2 + view.m5*projection.m6 + view.m6*projection.m10 + view.m7*projection.m14,
    view.m4*projection.m3 + view.m5*projection.m7 + view.m6*projection.m11 + view.m7*projection.m15,
    view.m8*projection.m0 + view.m9*projection.m4 + view.m10*projection.m8 + view.m11*projection.m12,
    view.m8*projection.m1 + view.m9*projection.m5 + view.m10*projection.m9 + view.m11*projection.m13,
    view.m8*projection.m2 + view.m9*projection.m6 + view.m10*projection.m10 + view.m11*projection.m14,
    view.m8*projection.m3 + view.m9*projection.m7 + view.m10*projection.m11 + view.m11*projection.m15,
    view.m12*projection.m0 + view.m13*projection.m4 + view.m14*projection.m8 + view.m15*projection.m12,
    view.m12*projection.m1 + view.m13*projection.m5 + view.m14*projection.m9 + view.m15*projection.m13,
    view.m12*projection.m2 + view.m13*projection.m6 + view.m14*projection.m10 + view.m15*projection.m14,
    view.m12*projection.m3 + view.m13*projection.m7 + view.m14*projection.m11 + view.m15*projection.m15 };
  
  // Calculate inverted matrix -> MatrixInvert(matViewProj);
  // Cache the matrix values (speed optimization)
  f32 a00 = matViewProj.m0, a01 = matViewProj.m1, a02 = matViewProj.m2, a03 = matViewProj.m3;
  f32 a10 = matViewProj.m4, a11 = matViewProj.m5, a12 = matViewProj.m6, a13 = matViewProj.m7;
  f32 a20 = matViewProj.m8, a21 = matViewProj.m9, a22 = matViewProj.m10, a23 = matViewProj.m11;
  f32 a30 = matViewProj.m12, a31 = matViewProj.m13, a32 = matViewProj.m14, a33 = matViewProj.m15;
  
  f32 b00 = a00*a11 - a01*a10;
  f32 b01 = a00*a12 - a02*a10;
  f32 b02 = a00*a13 - a03*a10;
  f32 b03 = a01*a12 - a02*a11;
  f32 b04 = a01*a13 - a03*a11;
  f32 b05 = a02*a13 - a03*a12;
  f32 b06 = a20*a31 - a21*a30;
  f32 b07 = a20*a32 - a22*a30;
  f32 b08 = a20*a33 - a23*a30;
  f32 b09 = a21*a32 - a22*a31;
  f32 b10 = a21*a33 - a23*a31;
  f32 b11 = a22*a33 - a23*a32;
  
  // Calculate the invert determinant (inlined to avoid double-caching)
  f32 invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);
  
  Matrix4 matViewProjInv = {
    (a11*b11 - a12*b10 + a13*b09)*invDet,
    (-a01*b11 + a02*b10 - a03*b09)*invDet,
    (a31*b05 - a32*b04 + a33*b03)*invDet,
    (-a21*b05 + a22*b04 - a23*b03)*invDet,
    (-a10*b11 + a12*b08 - a13*b07)*invDet,
    (a00*b11 - a02*b08 + a03*b07)*invDet,
    (-a30*b05 + a32*b02 - a33*b01)*invDet,
    (a20*b05 - a22*b02 + a23*b01)*invDet,
    (a10*b10 - a11*b08 + a13*b06)*invDet,
    (-a00*b10 + a01*b08 - a03*b06)*invDet,
    (a30*b04 - a31*b02 + a33*b00)*invDet,
    (-a20*b04 + a21*b02 - a23*b00)*invDet,
    (-a10*b09 + a11*b07 - a12*b06)*invDet,
    (a00*b09 - a01*b07 + a02*b06)*invDet,
    (-a30*b03 + a31*b01 - a32*b00)*invDet,
    (a20*b03 - a21*b01 + a22*b00)*invDet };
  
  // Create quaternion from source point
  Quaternion quat = { source.x, source.y, source.z, 1.0f };
  
  // Multiply quat point by unprojecte matrix
  Quaternion qtransformed = {     // QuaternionTransform(quat, matViewProjInv)
    matViewProjInv.m0*quat.x + matViewProjInv.m4*quat.y + matViewProjInv.m8*quat.z + matViewProjInv.m12*quat.w,
    matViewProjInv.m1*quat.x + matViewProjInv.m5*quat.y + matViewProjInv.m9*quat.z + matViewProjInv.m13*quat.w,
    matViewProjInv.m2*quat.x + matViewProjInv.m6*quat.y + matViewProjInv.m10*quat.z + matViewProjInv.m14*quat.w,
    matViewProjInv.m3*quat.x + matViewProjInv.m7*quat.y + matViewProjInv.m11*quat.z + matViewProjInv.m15*quat.w };
  
  // Normalized world points in vectors
  result.x = qtransformed.x/qtransformed.w;
  result.y = qtransformed.y/qtransformed.w;
  result.z = qtransformed.z/qtransformed.w;
  
  return result;
}

internal f32 vector3_dot(Vector3 a, Vector3 b) {
	f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}

internal f32 vector3_length(Vector3 v) {
	f32 result = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	return result;
}

internal f32 vector3_distance(Vector3 a, Vector3 b) {
	f32 result = 0.0f;
	f32 dx = b.x - a.x;
	f32 dy = b.y - a.y;
	f32 dz = b.z - a.z;
	result = sqrtf(dx*dx + dy*dy + dz*dz);
	return result;
}

internal f32 vector3_angle(Vector3 a, Vector3 b) {
	f32 result = 0.0f;
	Vector3 cross = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
	f32 len = sqrtf(cross.x*cross.x + cross.y*cross.y + cross.z*cross.z);
	f32 dot = (a.x*b.x + a.y*b.y + a.z*b.z);
	result  = atan2f(len, dot);
	return result;
}

internal Vector4 vector4(f32 x, f32 y, f32 z, f32 w) {
	Vector4 result = {x, y, z, w};
	return result;
}

internal Vector4 vector4_from_vector3(Vector3 v) {
	Vector4 result = {v.x, v.y, v.z};
	return result;
}

internal Vector4 vector4_add(Vector4 a, Vector4 b) {
	Vector4 result = {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w
	};
	return result;
}

internal Vector4 vector4_sub(Vector4 a, Vector4 b) {
	Vector4 result = {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	};
	return result;
}

internal Vector4 vector4_mul(Vector4 a, Vector4 b) {
	Vector4 result = {
		a.x * b.x,
		a.y * b.y,
		a.z * b.z,
		a.w * b.w
	};
	return result;
}

internal Vector4 vector4_div(Vector4 a, Vector4 b) {
	Vector4 result = {
		a.x / b.x,
		a.y / b.y,
		a.z / b.z,
		a.w / b.w
	};
	return result;
}

internal Vector4 vector4_scale(Vector4 v, f32 scalar) {
	Vector4 result = {
		v.x*scalar,
		v.y*scalar,
		v.z*scalar,
		v.w*scalar
	};
	return result;
}

internal Vector4 vector4_normalize(Vector4 v) {
	Vector4 result = v;
	f32 length = sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
	if (length > 0) {
    f32 ilength = 1.0f/length;
    result.x = v.x*ilength;
    result.y = v.y*ilength;
    result.z = v.z*ilength;
    result.w = v.w*ilength;
	}
	return result;
}

internal Vector4 vector4_lerp(Vector4 a, Vector4 b, f32 t) {
	Vector4 result = {
		a.x + t*(b.x - a.x),
		a.y + t*(b.y - a.y),
		a.z + t*(b.z - a.z),
		a.w + t*(b.w - a.w)
	};
	return result;
}

internal f32 vector4_dot(Vector4 a, Vector4 b) {
	f32 result = (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
	return result;
}

internal f32 vector4_length(Vector4 v) {
	f32 result = sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));
	return result;
}

internal f32 vector4_distance(Vector4 a, Vector4 b) {
	f32 result = 0.0f;
	f32 dx = a.x - b.x;
	f32 dy = a.y - b.y;
	f32 dz = a.z - b.z;
	f32 dw = a.w - b.w;
	result = sqrtf(dx*dx + dy*dy + dz*dz + dw*dw);
	return result;
}

//////////////////////////////////////////////
// Matrix4 f32
internal Matrix4 matrix4(f32 diag) {
	Matrix4 result = {
		diag, 0.0f, 0.0f, 0.0f,
		0.0f, diag, 0.0f, 0.0f,
		0.0f, 0.0f, diag, 0.0f,
		0.0f, 0.0f, 0.0f, diag };
  
	return result;
}

internal Matrix4 matrix4_add(Matrix4 left, Matrix4 right) {
	Matrix4 result = { 0 };
	result.m0  = left.m0  + right.m0;
	result.m1  = left.m1  + right.m1;
	result.m2  = left.m2  + right.m2;
	result.m3  = left.m3  + right.m3;
	result.m4  = left.m4  + right.m4;
	result.m5  = left.m5  + right.m5;
	result.m6  = left.m6  + right.m6;
	result.m7  = left.m7  + right.m7;
	result.m8  = left.m8  + right.m8;
	result.m9  = left.m9  + right.m9;
	result.m10 = left.m10 + right.m10;
	result.m11 = left.m11 + right.m11;
	result.m12 = left.m12 + right.m12;
	result.m13 = left.m13 + right.m13;
	result.m14 = left.m14 + right.m14;
	result.m15 = left.m15 + right.m15;
	return result;
}

internal Matrix4 matrix4_sub(Matrix4 left, Matrix4 right) {
	Matrix4 result = { 0 };
	result.m0  = left.m0  - right.m0;
	result.m1  = left.m1  - right.m1;
	result.m2  = left.m2  - right.m2;
	result.m3  = left.m3  - right.m3;
	result.m4  = left.m4  - right.m4;
	result.m5  = left.m5  - right.m5;
	result.m6  = left.m6  - right.m6;
	result.m7  = left.m7  - right.m7;
	result.m8  = left.m8  - right.m8;
	result.m9  = left.m9  - right.m9;
	result.m10 = left.m10 - right.m10;
	result.m11 = left.m11 - right.m11;
	result.m12 = left.m12 - right.m12;
	result.m13 = left.m13 - right.m13;
	result.m14 = left.m14 - right.m14;
	result.m15 = left.m15 - right.m15;
	return result;
}

internal Matrix4 matrix4_mul(Matrix4 left, Matrix4 right) {
	Matrix4 result = { 0 };
	result.m0  = left.m0 *right.m0 + left.m1 *right.m4 + left.m2 *right.m8  + left.m3 *right.m12;
	result.m1  = left.m0 *right.m1 + left.m1 *right.m5 + left.m2 *right.m9  + left.m3 *right.m13;
	result.m2  = left.m0 *right.m2 + left.m1 *right.m6 + left.m2 *right.m10 + left.m3 *right.m14;
	result.m3  = left.m0 *right.m3 + left.m1 *right.m7 + left.m2 *right.m11 + left.m3 *right.m15;
	result.m4  = left.m4 *right.m0 + left.m5 *right.m4 + left.m6 *right.m8  + left.m7 *right.m12;
	result.m5  = left.m4 *right.m1 + left.m5 *right.m5 + left.m6 *right.m9  + left.m7 *right.m13;
	result.m6  = left.m4 *right.m2 + left.m5 *right.m6 + left.m6 *right.m10 + left.m7 *right.m14;
	result.m7  = left.m4 *right.m3 + left.m5 *right.m7 + left.m6 *right.m11 + left.m7 *right.m15;
	result.m8  = left.m8 *right.m0 + left.m9 *right.m4 + left.m10*right.m8  + left.m11*right.m12;
	result.m9  = left.m8 *right.m1 + left.m9 *right.m5 + left.m10*right.m9  + left.m11*right.m13;
	result.m10 = left.m8 *right.m2 + left.m9 *right.m6 + left.m10*right.m10 + left.m11*right.m14;
	result.m11 = left.m8 *right.m3 + left.m9 *right.m7 + left.m10*right.m11 + left.m11*right.m15;
	result.m12 = left.m12*right.m0 + left.m13*right.m4 + left.m14*right.m8  + left.m15*right.m12;
	result.m13 = left.m12*right.m1 + left.m13*right.m5 + left.m14*right.m9  + left.m15*right.m13;
	result.m14 = left.m12*right.m2 + left.m13*right.m6 + left.m14*right.m10 + left.m15*right.m14;
	result.m15 = left.m12*right.m3 + left.m13*right.m7 + left.m14*right.m11 + left.m15*right.m15;
	return result;
}

internal Matrix4 matrix4_translate(f32 x, f32 y, f32 z) {
	Matrix4 result = {
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

internal Matrix4 matrix4_rotate_axis(Vector3 axis, f32 radians) {
	Matrix4 result = { 0 };
  
	f32 x = axis.x;
	f32 y = axis.y;
	f32 z = axis.z;
	f32 lengthSquared = x*x + y*y + z*z;
  
	if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f)) {
		f32 ilength = 1.0f/sqrtf(lengthSquared);
		x *= ilength;
		y *= ilength;
		z *= ilength;
	}
  
	f32 sinres = sinf(radians);
	f32 cosres = cosf(radians);
	f32 t = 1.0f - cosres;
  
	result.m0 = x*x*t + cosres;
	result.m1 = y*x*t + z*sinres;
	result.m2 = z*x*t - y*sinres;
	result.m3 = 0.0f;
  
	result.m4 = x*y*t - z*sinres;
	result.m5 = y*y*t + cosres;
	result.m6 = z*y*t + x*sinres;
	result.m7 = 0.0f;
  
	result.m8 = x*z*t + y*sinres;
	result.m9 = y*z*t - x*sinres;
	result.m10 = z*z*t + cosres;
	result.m11 = 0.0f;
  
	result.m12 = 0.0f;
	result.m13 = 0.0f;
	result.m14 = 0.0f;
	result.m15 = 1.0f;
  
	return result;
}

internal Matrix4 matrix4_rotate_x(f32 radians) {
  Matrix4 result = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  f32 cosres = cosf(radians);
  f32 sinres = sinf(radians);
  result.m5  = cosres;
  result.m6  = sinres;
  result.m9  = -sinres;
  result.m10 = cosres;
  return result;
}

internal Matrix4 matrix4_rotate_y(f32 radians) {
  Matrix4 result = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  f32 cosres = cosf(radians);
  f32 sinres = sinf(radians);
  result.m0 = cosres;
  result.m2 = -sinres;
  result.m8 = sinres;
  result.m10 = cosres;
  return result;
}

internal Matrix4 matrix4_rotate_z(f32 radians) {
  Matrix4 result = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  f32 cosres = cosf(radians);
  f32 sinres = sinf(radians);
  result.m0 = cosres;
  result.m1 = sinres;
  result.m4 = -sinres;
  result.m5 = cosres;
  return result;
}

internal Matrix4 matrix4_rotate_xyz(Vector3 radians) {
	Matrix4 result = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
  
	f32 cosz = cosf(-radians.z);
	f32 sinz = sinf(-radians.z);
	f32 cosy = cosf(-radians.y);
	f32 siny = sinf(-radians.y);
	f32 cosx = cosf(-radians.x);
	f32 sinx = sinf(-radians.x);
  
	result.m0 = cosz*cosy;
	result.m1 = (cosz*siny*sinx) - (sinz*cosx);
	result.m2 = (cosz*siny*cosx) + (sinz*sinx);
  
	result.m4 = sinz*cosy;
	result.m5 = (sinz*siny*sinx) + (cosz*cosx);
	result.m6 = (sinz*siny*cosx) - (cosz*sinx);
  
	result.m8 = -siny;
	result.m9 = cosy*sinx;
	result.m10= cosy*cosx;
  
	return result;
}

internal Matrix4 matrix4_rotate_zyx(Vector3 radians) {
	Matrix4 result = { 0 };
  
	f32 cz = cosf(radians.z);
	f32 sz = sinf(radians.z);
	f32 cy = cosf(radians.y);
	f32 sy = sinf(radians.y);
	f32 cx = cosf(radians.x);
	f32 sx = sinf(radians.x);
  
	result.m0 = cz*cy;
	result.m4 = cz*sy*sx - cx*sz;
	result.m8 = sz*sx + cz*cx*sy;
	result.m12 = 0;
  
	result.m1 = cy*sz;
	result.m5 = cz*cx + sz*sy*sx;
	result.m9 = cx*sz*sy - cz*sx;
	result.m13 = 0;
  
	result.m2 = -sy;
	result.m6 = cy*sx;
	result.m10 = cy*cx;
	result.m14 = 0;
  
	result.m3 = 0;
	result.m7 = 0;
	result.m11 = 0;
	result.m15 = 1;
  
	return result;
}

internal Matrix4 matrix4_transpose(Matrix4 m) {
	Matrix4 result = { 0 };
	result.m0  = m.m0;
	result.m1  = m.m4;
	result.m2  = m.m8;
	result.m3  = m.m12;
	result.m4  = m.m1;
	result.m5  = m.m5;
	result.m6  = m.m9;
	result.m7  = m.m13;
	result.m8  = m.m2;
	result.m9  = m.m6;
	result.m10 = m.m10;
	result.m11 = m.m14;
	result.m12 = m.m3;
	result.m13 = m.m7;
	result.m14 = m.m11;
	result.m15 = m.m15;
	return result;
}

internal Matrix4 matrix4_scale(f32 x, f32 y, f32 z) {
	Matrix4 result = {
		x,    0.0f, 0.0f, 0.0f,
		0.0f, y,    0.0f, 0.0f,
		0.0f, 0.0f, z,    0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return result;
}

internal Matrix4 matrix4_frustum(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane) {
	Matrix4 result = { 0 };
  
	f32 rl = (f32)(right - left);
	f32 tb = (f32)(top - bottom);
	f32 fn = (f32)(far_plane - near_plane);
  
	result.m0 = ((f32)near_plane*2.0f)/rl;
	result.m1 = 0.0f;
	result.m2 = 0.0f;
	result.m3 = 0.0f;
  
	result.m4 = 0.0f;
	result.m5 = ((f32)near_plane*2.0f)/tb;
	result.m6 = 0.0f;
	result.m7 = 0.0f;
  
	result.m8 = ((f32)right + (f32)left)/rl;
	result.m9 = ((f32)top + (f32)bottom)/tb;
	result.m10 = -((f32)far_plane + (f32)near_plane)/fn;
	result.m11 = -1.0f;
  
	result.m12 = 0.0f;
	result.m13 = 0.0f;
	result.m14 = -((f32)far_plane*(f32)near_plane*2.0f)/fn;
	result.m15 = 0.0f;
  
	return result;
}

internal Matrix4 matrix4_perspective(f64 fovy, f64 window_width, f64 window_height, f64 near_plane, f64 far_plane) {
	Matrix4 result = { 0 };
  
	f64 top = near_plane*tan(fovy*0.5);
	f64 bottom = -top;
	f64 right = top*(window_width/window_height);
	f64 left = -right;
  
	// MatrixFrustum(-right, right, -top, top, near, far);
	f32 rl = (f32)(right - left);
	f32 tb = (f32)(top - bottom);
	f32 fn = (f32)(far_plane - near_plane);
  
	result.m0 = ((f32)near_plane*2.0f)/rl;
	result.m5 = ((f32)near_plane*2.0f)/tb;
	result.m8 = ((f32)right + (f32)left)/rl;
	result.m9 = ((f32)top + (f32)bottom)/tb;
	result.m10 = -((f32)far_plane + (f32)near_plane)/fn;
	result.m11 = -1.0f;
	result.m14 = -((f32)far_plane*(f32)near_plane*2.0f)/fn;
  
	return result;
}

internal Matrix4 matrix4_ortographic(f64 left, f64 right, f64 bottom, f64 top, f64 near_plane, f64 far_plane) {
	Matrix4 result = { 0 };
  
	f32 rl = (f32)(right - left);
	f32 tb = (f32)(top - bottom);
	f32 fn = (f32)(far_plane - near_plane);
  
	result.m0 = 2.0f/rl;
	result.m1 = 0.0f;
	result.m2 = 0.0f;
	result.m3 = 0.0f;
	result.m4 = 0.0f;
	result.m5 = 2.0f/tb;
	result.m6 = 0.0f;
	result.m7 = 0.0f;
	result.m8 = 0.0f;
	result.m9 = 0.0f;
	result.m10 = -2.0f/fn;
	result.m11 = 0.0f;
	result.m12 = -((f32)left + (f32)right)/rl;
	result.m13 = -((f32)top + (f32)bottom)/tb;
	result.m14 = -((f32)far_plane + (f32)near_plane)/fn;
	result.m15 = 1.0f;
  
	return result;
}

internal Matrix4 matrix4_look_at(Vector3 eye, Vector3 target, Vector3 up) {
	Matrix4 result = { 0 };
  
	f32 length = 0.0f;
	f32 ilength = 0.0f;
  
	// Vector3Subtract(eye, target)
	Vector3 vz = { eye.x - target.x, eye.y - target.y, eye.z - target.z };
  
	// Vector3Normalize(vz)
	Vector3 v = vz;
	length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (length == 0.0f) length = 1.0f;
	ilength = 1.0f/length;
	vz.x *= ilength;
	vz.y *= ilength;
	vz.z *= ilength;
  
	// Vector3CrossProduct(up, vz)
	Vector3 vx = { up.y*vz.z - up.z*vz.y, up.z*vz.x - up.x*vz.z, up.x*vz.y - up.y*vz.x };
  
	// Vector3Normalize(x)
	v = vx;
	length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	if (length == 0.0f) length = 1.0f;
	ilength = 1.0f/length;
	vx.x *= ilength;
	vx.y *= ilength;
	vx.z *= ilength;
  
	// Vector3CrossProduct(vz, vx)
	Vector3 vy = { vz.y*vx.z - vz.z*vx.y, vz.z*vx.x - vz.x*vx.z, vz.x*vx.y - vz.y*vx.x };
  
	result.m0 = vx.x;
	result.m1 = vy.x;
	result.m2 = vz.x;
	result.m3 = 0.0f;
	result.m4 = vx.y;
	result.m5 = vy.y;
	result.m6 = vz.y;
	result.m7 = 0.0f;
	result.m8 = vx.z;
	result.m9 = vy.z;
	result.m10 = vz.z;
	result.m11 = 0.0f;
	result.m12 = -(vx.x*eye.x + vx.y*eye.y + vx.z*eye.z);
	result.m13 = -(vy.x*eye.x + vy.y*eye.y + vy.z*eye.z);
	result.m14 = -(vz.x*eye.x + vz.y*eye.y + vz.z*eye.z);
	result.m15 = 1.0f;
  
	return result;
}

internal Matrix4 matrix_from_quaternion(Quaternion q) {
	Matrix4 result = matrix4_identity();

	f32 a2 = q.x*q.x;
	f32 b2 = q.y*q.y;
	f32 c2 = q.z*q.z;
	f32 ac = q.x*q.z;
	f32 ab = q.x*q.y;
	f32 bc = q.y*q.z;
	f32 ad = q.w*q.x;
	f32 bd = q.w*q.y;
	f32 cd = q.w*q.z;

	result.m0 = 1 - 2*(b2 + c2);
	result.m1 = 2*(ab + cd);
	result.m2 = 2*(ac - bd);

	result.m4 = 2*(ab - cd);
	result.m5 = 1 - 2*(a2 + c2);
	result.m6 = 2*(bc + ad);

	result.m8 = 2*(ac + bd);
	result.m9 = 2*(bc - ad);
	result.m10 = 1 - 2*(a2 + b2);

	return result;
}

internal Transform transform_from_matrix4(Matrix4 mat) {
	Transform result = { 0 };

	// Extract translation.
	result.translation.x = mat.m12;
	result.translation.y = mat.m13;
	result.translation.z = mat.m14;

	// Extract upper-left for determinant computation
	f32 a = mat.m0;
	f32 b = mat.m4;
	f32 c = mat.m8;
	f32 d = mat.m1;
	f32 e = mat.m5;
	f32 f = mat.m9;
	f32 g = mat.m2;
	f32 h = mat.m6;
	f32 i = mat.m10;
	f32 A = e*i - f*h;
	f32 B = f*g - d*i;
	f32 C = d*h - e*g;

	// Extract scale
	f32 det = a*A + b*B + c*C;
	Vector3 abc = vector3(a, b, c);
	Vector3 def = vector3(d, e, f);
	Vector3 ghi = vector3(g, h, i);

	f32 scalex = vector3_length(abc);
	f32 scaley = vector3_length(def);
	f32 scalez = vector3_length(ghi);
	Vector3 s  = vector3(scalex, scaley, scalez);
	if (det < 0) {
		s = vector3(-s.x, -s.y, -s.z);
	}

	result.scale = s;

	// Remove scale from the matrix if it is not close to zero
	Matrix4 clone = mat;
	if (!FloatEquals(det, 0)) {
		clone.m0 /= s.x;
		clone.m5 /= s.y;
		clone.m10 /= s.z;
		result.rotation = quaternion_from_matrix(clone);
	}	else {
		result.rotation = quaternion_identity();
	}
}

internal Quaternion quaternion_add(Quaternion q1, Quaternion q2) {
	Quaternion result = {q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w};
	return result;
}

internal Quaternion quaternion_add_value(Quaternion q, f32 value) {
	Quaternion result = {q.x + value, q.y + value, q.z + value, q.w + value};
	return result;
}

internal Quaternion quaternion_subtract(Quaternion q1, Quaternion q2) {
	Quaternion result = {q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w};
	return result;
}

internal Quaternion quaternion_subtract_value(Quaternion q, f32 value) {
	Quaternion result = {q.x - value, q.y - value, q.z - value, q.w - value};
	return result;
}

internal f32 quaternion_length(Quaternion q) {
	f32 result = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	return result;
}

internal Quaternion quaternion_normalize(Quaternion q) {
	Quaternion result = { 0 };

	f32 length = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	if (length == 0.0f) {
		length = 1.0f;
	}
	f32 ilength = 1.0f/length;

	result.x = q.x*ilength;
	result.y = q.y*ilength;
	result.z = q.z*ilength;
	result.w = q.w*ilength;

	return result;
}

internal Quaternion quaternion_invert(Quaternion q) {
	Quaternion result = q;
	f32 lengthSq = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;

	if (lengthSq != 0.0f) {
		f32 invLength = 1.0f/lengthSq;
		result.x *= -invLength;
		result.y *= -invLength;
		result.z *= -invLength;
		result.w *= invLength;
	}
	return result;
}

internal Quaternion quaternion_multiply(Quaternion q1, Quaternion q2) {
	Quaternion result = { 0 };
	f32 qax = q1.x, qay = q1.y, qaz = q1.z, qaw = q1.w;
	f32 qbx = q2.x, qby = q2.y, qbz = q2.z, qbw = q2.w;

	result.x = qax*qbw + qaw*qbx + qay*qbz - qaz*qby;
	result.y = qay*qbw + qaw*qby + qaz*qbx - qax*qbz;
	result.z = qaz*qbw + qaw*qbz + qax*qby - qay*qbx;
	result.w = qaw*qbw - qax*qbx - qay*qby - qaz*qbz;

	return result;
}

internal Quaternion quaternion_scale(Quaternion q, f32 scalar) {
	Quaternion result = { 0 };

	result.x = q.x*scalar;
	result.y = q.y*scalar;
	result.z = q.z*scalar;
	result.w = q.w*scalar;

	return result;
}

internal Quaternion quaternion_divide(Quaternion q1, Quaternion q2) {
	Quaternion result = { q1.x/q2.x, q1.y/q2.y, q1.z/q2.z, q1.w/q2.w };
	return result;
}

internal Quaternion quaternion_lerp(Quaternion q1, Quaternion q2, f32 amount) {
	Quaternion result = { 0 };

	result.x = q1.x + amount*(q2.x - q1.x);
	result.y = q1.y + amount*(q2.y - q1.y);
	result.z = q1.z + amount*(q2.z - q1.z);
	result.w = q1.w + amount*(q2.w - q1.w);

	return result;
}

internal Quaternion quaternion_nlerp(Quaternion q1, Quaternion q2, f32 amount) {
	Quaternion result = { 0 };

	// QuaternionLerp(q1, q2, amount)
	result.x = q1.x + amount*(q2.x - q1.x);
	result.y = q1.y + amount*(q2.y - q1.y);
	result.z = q1.z + amount*(q2.z - q1.z);
	result.w = q1.w + amount*(q2.w - q1.w);

	// QuaternionNormalize(q);
	Quaternion q = result;
	f32 length = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	if (length == 0.0f) {
		length = 1.0f;
	}
	f32 ilength = 1.0f/length;

	result.x = q.x*ilength;
	result.y = q.y*ilength;
	result.z = q.z*ilength;
	result.w = q.w*ilength;

	return result;
}

internal Quaternion quaternion_slerp(Quaternion q1, Quaternion q2, f32 amount) {
	Quaternion result = { 0 };

#if !defined(EPSILON)
	#define EPSILON 0.000001f
#endif

	f32 cosHalfTheta = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;

	if (cosHalfTheta < 0) {
		q2.x = -q2.x; q2.y = -q2.y; q2.z = -q2.z; q2.w = -q2.w;
		cosHalfTheta = -cosHalfTheta;
	}

	if (fabsf(cosHalfTheta) >= 1.0f) {
		result = q1;
	} else if (cosHalfTheta > 0.95f) {
		result = quaternion_nlerp(q1, q2, amount);
	} else {
		f32 halfTheta = acosf(cosHalfTheta);
		f32 sinHalfTheta = sqrtf(1.0f - cosHalfTheta*cosHalfTheta);

		if (fabsf(sinHalfTheta) < EPSILON) {
			result.x = (q1.x*0.5f + q2.x*0.5f);
			result.y = (q1.y*0.5f + q2.y*0.5f);
			result.z = (q1.z*0.5f + q2.z*0.5f);
			result.w = (q1.w*0.5f + q2.w*0.5f);
		} else {
			f32 ratioA = sinf((1 - amount)*halfTheta)/sinHalfTheta;
			f32 ratioB = sinf(amount*halfTheta)/sinHalfTheta;
			result.x = (q1.x*ratioA + q2.x*ratioB);
			result.y = (q1.y*ratioA + q2.y*ratioB);
			result.z = (q1.z*ratioA + q2.z*ratioB);
			result.w = (q1.w*ratioA + q2.w*ratioB);
		}
	}

	return result;
}

internal Quaternion quaternion_cubic_hermit_spline(Quaternion q1, Quaternion outTangent1, Quaternion q2, Quaternion inTangent2, f32 t) {
	// Calculate quaternion cubic spline interpolation using Cubic Hermite Spline algorithm
	// as described in the GLTF 2.0 specification: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#interpolation-cubic
	f32 t2 = t*t;
	f32 t3 = t2*t;
	f32 h00 = 2*t3 - 3*t2 + 1;
	f32 h10 = t3 - 2*t2 + t;
	f32 h01 = -2*t3 + 3*t2;
	f32 h11 = t3 - t2;

	Quaternion p0 = quaternion_scale(q1, h00);
	Quaternion m0 = quaternion_scale(outTangent1, h10);
	Quaternion p1 = quaternion_scale(q2, h01);
	Quaternion m1 = quaternion_scale(inTangent2, h11);
	Quaternion result = { 0 };

	result = quaternion_add(p0, m0);
	result = quaternion_add(result, p1);
	result = quaternion_add(result, m1);
	result = quaternion_normalize(result);

	return result;
}

internal Quaternion quaternion_from_vector3_to_vector3(Vector3 from, Vector3 to) {
	Quaternion result = { 0 };

	f32 cos2Theta = (from.x*to.x + from.y*to.y + from.z*to.z);    // Vector3DotProduct(from, to)
	Vector3 cross = { from.y*to.z - from.z*to.y, from.z*to.x - from.x*to.z, from.x*to.y - from.y*to.x }; // Vector3CrossProduct(from, to)

	result.x = cross.x;
	result.y = cross.y;
	result.z = cross.z;
	result.w = 1.0f + cos2Theta;

	// QuaternionNormalize(q);
	// NOTE: Normalize to essentially nlerp the original and identity to 0.5
	Quaternion q = result;
	f32 length = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	if (length == 0.0f) {
		length = 1.0f;
	}
	f32 ilength = 1.0f/length;

	result.x = q.x*ilength;
	result.y = q.y*ilength;
	result.z = q.z*ilength;
	result.w = q.w*ilength;

	return result;
}

internal Quaternion quaternion_from_matrix(Matrix4 mat) {
	Quaternion result = { 0 };

	f32 fourWSquaredMinus1 = mat.m0  + mat.m5 + mat.m10;
	f32 fourXSquaredMinus1 = mat.m0  - mat.m5 - mat.m10;
	f32 fourYSquaredMinus1 = mat.m5  - mat.m0 - mat.m10;
	f32 fourZSquaredMinus1 = mat.m10 - mat.m0 - mat.m5;

	int biggestIndex = 0;
	f32 fourBiggestSquaredMinus1 = fourWSquaredMinus1;

	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}

	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}

	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	f32 biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.0f)*0.5f;
	f32 mult = 0.25f/biggestVal;

	switch (biggestIndex) {
		case 0: {
			result.w = biggestVal;
			result.x = (mat.m6 - mat.m9)*mult;
			result.y = (mat.m8 - mat.m2)*mult;
			result.z = (mat.m1 - mat.m4)*mult;
		} break;
		case 1: {
			result.x = biggestVal;
			result.w = (mat.m6 - mat.m9)*mult;
			result.y = (mat.m1 + mat.m4)*mult;
			result.z = (mat.m8 + mat.m2)*mult;
		} break;
		case 2: {
			result.y = biggestVal;
			result.w = (mat.m8 - mat.m2)*mult;
			result.x = (mat.m1 + mat.m4)*mult;
			result.z = (mat.m6 + mat.m9)*mult;
		} break;
		case 3: {
			result.z = biggestVal;
			result.w = (mat.m1 - mat.m4)*mult;
			result.x = (mat.m8 + mat.m2)*mult;
			result.y = (mat.m6 + mat.m9)*mult;
		} break;
	}

	return result;
}

internal Quaternion quaternion_from_axis_angle(Vector3 axis, f32 angle) {
	Quaternion result = { 0.0f, 0.0f, 0.0f, 1.0f };
	f32 axisLength = sqrtf(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);

	if (axisLength != 0.0f) {
		angle *= 0.5f;

		f32 length = 0.0f;
		f32 ilength = 0.0f;

		// Vector3Normalize(axis)
		length = axisLength;
		if (length == 0.0f) {
			length = 1.0f;
		}
		ilength = 1.0f/length;
		axis.x *= ilength;
		axis.y *= ilength;
		axis.z *= ilength;

		f32 sinres = sinf(angle);
		f32 cosres = cosf(angle);

		result.x = axis.x*sinres;
		result.y = axis.y*sinres;
		result.z = axis.z*sinres;
		result.w = cosres;

		// QuaternionNormalize(q);
		Quaternion q = result;
		length = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
		if (length == 0.0f) {
			length = 1.0f;
		}
		ilength = 1.0f/length;
		result.x = q.x*ilength;
		result.y = q.y*ilength;
		result.z = q.z*ilength;
		result.w = q.w*ilength;
	}

	return result;
}

internal void axis_angle_from_quaternion(Quaternion q, Vector3 *axis, f32 *angle) {
	if (fabsf(q.w) > 1.0f) {
		f32 length = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
		if (length == 0.0f) {
			length = 1.0f;
		}

		f32 ilength = 1.0f/length;
		q.x = q.x*ilength;
		q.y = q.y*ilength;
		q.z = q.z*ilength;
		q.w = q.w*ilength;
	}

	Vector3 resAxis = { 0.0f, 0.0f, 0.0f };
	f32 resAngle = 2.0f*acosf(q.w);
	f32 den = sqrtf(1.0f - q.w*q.w);

#if !defined(EPSILON)
	#define EPSILON 0.000001f
#endif

	if (den > EPSILON) {
		resAxis.x = q.x/den;
		resAxis.y = q.y/den;
		resAxis.z = q.z/den;
	} else {
		// This occurs when the angle is zero.
		// Not a problem: just set an arbitrary normalized axis.
		resAxis.x = 1.0f;
	}

	*axis  = resAxis;
	*angle = resAngle;
}

internal Quaternion quaternion_from_euler(f32 pitch, f32 yaw, f32 roll) {
	Quaternion result = { 0 };

	f32 x0 = cosf(pitch*0.5f);
	f32 x1 = sinf(pitch*0.5f);
	f32 y0 = cosf(yaw*0.5f);
	f32 y1 = sinf(yaw*0.5f);
	f32 z0 = cosf(roll*0.5f);
	f32 z1 = sinf(roll*0.5f);

	result.x = x1*y0*z0 - x0*y1*z1;
	result.y = x0*y1*z0 + x1*y0*z1;
	result.z = x0*y0*z1 - x1*y1*z0;
	result.w = x0*y0*z0 + x1*y1*z1;

	return result;
}

internal void euler_from_quaternion(Quaternion q, f32* pitch, f32* yaw, f32* roll) {
	// Roll (x-axis rotation)
	f32 x0 = 2.0f*(q.w*q.x + q.y*q.z);
	f32 x1 = 1.0f - 2.0f*(q.x*q.x + q.y*q.y);
	*pitch = atan2f(x0, x1);

	// Pitch (y-axis rotation)
	f32 y0 = 2.0f*(q.w*q.y - q.z*q.x);
	y0 = y0 > 1.0f ? 1.0f : y0;
	y0 = y0 < -1.0f ? -1.0f : y0;
	*yaw = asinf(y0);

	// Yaw (z-axis rotation)
	f32 z0 = 2.0f*(q.w*q.z + q.x*q.y);
	f32 z1 = 1.0f - 2.0f*(q.y*q.y + q.z*q.z);
	*roll = atan2f(z0, z1);
}

internal Quaternion quaternion_mul_matric4(Quaternion q, Matrix4 mat) {
	Quaternion result = { 0 };

	result.x = mat.m0*q.x + mat.m4*q.y + mat.m8*q.z + mat.m12*q.w;
	result.y = mat.m1*q.x + mat.m5*q.y + mat.m9*q.z + mat.m13*q.w;
	result.z = mat.m2*q.x + mat.m6*q.y + mat.m10*q.z + mat.m14*q.w;
	result.w = mat.m3*q.x + mat.m7*q.y + mat.m11*q.z + mat.m15*q.w;

	return result;
}

internal b32 quaternion_equals(Quaternion p, Quaternion q) {
	#if !defined(EPSILON)
    #define EPSILON 0.000001f
#endif

	b32 result = (((fabsf(p.x - q.x)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.x), fabsf(q.x))))) &&
								((fabsf(p.y - q.y)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.y), fabsf(q.y))))) &&
								((fabsf(p.z - q.z)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.z), fabsf(q.z))))) &&
								((fabsf(p.w - q.w)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.w), fabsf(q.w)))))) ||
								(((fabsf(p.x + q.x)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.x), fabsf(q.x))))) &&
								((fabsf(p.y + q.y)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.y), fabsf(q.y))))) &&
								((fabsf(p.z + q.z)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.z), fabsf(q.z))))) &&
								((fabsf(p.w + q.w)) <= (EPSILON*fmaxf(1.0f, fmaxf(fabsf(p.w), fabsf(q.w))))));

	return result;
}

internal f32 clampf32(f32 value, f32 min, f32 max) {
	f32 result = value;
	if (result < min) {
		result = min;
	} else if (result > max) {
		result = max;
	}
	return result;
}

internal f32 lerpf32(f32 start, f32 end, f32 t) {
	f32 result = start + t * (end - start);
	return result;
}

internal b32 is_vector_inside_rectangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c) {
	b32 result = 0;
  
	Vector3 ab = sub(a, b);
	Vector3 bc = sub(b, c);
	Vector3 ap = sub(a, p);
	Vector3 bp = sub(b, p);
  
	f32 abap = vector3_dot(ab, ap);
	f32 abab = vector3_dot(ab, ab);
	f32 bcbp = vector3_dot(bc, bp);
	f32 bcbc = vector3_dot(bc, bc);
  
	if (0 <= abap && abap <= abab && 0 <= bcbp && bcbp <= bcbc) {
		result = 1;
	}
  
	return result;
}

internal Vector3 intersect_ray_with_plane(Rayf32 ray, Vector3 point1, Vector3 point2, Vector3 point3) {
	Vector3 result   = vector3(F32_MAX, F32_MAX, F32_MAX);
	Vector3 plane_v1 = vector3(point2.x-point1.x, point2.y-point1.y, point2.z-point1.z);
	Vector3 plane_v2 = vector3(point3.x-point1.x, point3.y-point1.y, point3.z-point1.z);
	Vector3 plane_normal = vector3(plane_v1.y*plane_v2.z - plane_v1.z*plane_v2.y,
																 plane_v1.z*plane_v2.x - plane_v1.x*plane_v2.z,
																 plane_v1.x*plane_v2.y - plane_v1.y*plane_v2.x);
	f32 dot = vector3_dot(ray.direction, plane_normal);
  
	// If the dot product is close to zero, the ray is parallel to the plane
	if (fabs(dot) < 0.000001f) {
    return result;
	}
  
	// Calculate the vector from a point on the ray to a point on the plane
	Vector3 rayToPlane = vector3(point1.x-ray.point.x, point1.y-ray.point.y, point1.z-ray.point.z);
  
	// Calculate the distance along the ray to the intersection point
	f32 t = (rayToPlane.x*plane_normal.x + rayToPlane.y*plane_normal.y + rayToPlane.z*plane_normal.z) / dot;
  
	// Calculate the intersection point
	result = vector3(ray.point.x + t * ray.direction.x,
                   ray.point.y + t * ray.direction.y,
                   ray.point.z + t * ray.direction.z);
  
	return result;
}
