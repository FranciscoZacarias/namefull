/* date = July 5th 2024 2:45 pm */

#ifndef RENDERER_H
#define RENDERER_H

#define MSAA_SAMPLES 8

#define MAIN_VS "D:\\work\\noname\\src\\shaders\\default_vertex_shader.glsl"
#define MAIN_FS "D:\\work\\noname\\src\\shaders\\default_fragment_shader.glsl"
#define SCREEN_VS "D:\\work\\noname\\src\\shaders\\screen_vertex_shader.glsl"
#define SCREEN_FS "D:\\work\\noname\\src\\shaders\\screen_fragment_shader.glsl"

#define Initial_Vertices 1024
#define Initial_Lines    3
#define Initial_Indices  1024
#define Initial_Textures 8

typedef struct Vertex {
  Vec3f32 position;
  Vec4f32 color;
  Vec2f32 uv;
  Vec3f32 normal;
  u32 texture;
} Vertex;

typedef struct Renderer {
  
  // Default program
  u32 main_shader;
  u32 triangles_vao;
  u32 triangles_vbo;
  u32 triangles_ebo;
  
  u32 lines_vao;
  u32 lines_vbo;
  
  // Offscreen 
  u32 msaa_fbo;
  u32 msaa_rbo;
  u32 msaa_texture_color_buffer_multisampled;
  u32 post_processing_fbo;
  
  // Screen programm
  u32 screen_texture;
  u32 screen_shader;
  u32 screen_vao;
  u32 screen_vbo;
  
  // Data
  Arena* arena;
  
  Vertex* triangles_data;
  u32     triangles_count;
  u32     triangles_capacity;
  u32* triangles_indices_data;
  u32  triangles_indices_count;
  u32  triangles_indices_capacity;
  
  Vertex* lines_data;
  u32 lines_count;
  u32 lines_capacity;
  
  u32 textures_data[Initial_Textures];
  u32 textures_count;
  
} Renderer;

Renderer GlobalRenderer;

internal void renderer_init(s32 window_width, s32 window_height);
internal void renderer_draw(Mat4f32 view, Mat4f32 projection, s32 window_width, s32 window_height);
internal void renderer_on_resize(s32 window_width, s32 window_height);

internal u32 renderer_load_color_texture(f32 r, f32 g, f32 b, f32 a);

internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);
internal void renderer_set_array_s32(u32 program, const char* uniform, s32 count, s32* ptr);
internal void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s);

#endif //RENDERER_H
