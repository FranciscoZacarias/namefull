/* date = July 5th 2024 2:45 pm */

#ifndef RENDERER_H
#define RENDERER_H

#define MSAA_SAMPLES 8

#define MAIN_VS   "D:\\work\\namefull\\source\\shaders\\vs_main.glsl"
#define MAIN_FS   "D:\\work\\namefull\\source\\shaders\\fs_main.glsl"
#define SCREEN_VS "D:\\work\\namefull\\source\\shaders\\vs_screen.glsl"
#define SCREEN_FS "D:\\work\\namefull\\source\\shaders\\fs_screen.glsl"

#define Initial_Vertices 1024
#define Initial_Lines    3
#define Initial_Indices  1024
#define Initial_Textures 8

typedef struct Vertex {
  Vec3f32 position;
  Vec4f32 color;
  Vec2f32 uv;
  u32 texture;
} Vertex;

#define vertex(p,c,u,t) (Vertex){p,c,u,t}

typedef struct Renderer {
  
  u32 main_shader;
  
  u32 vertex_vao;
  u32 vertex_vbo;
  
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
  
  // Vertex data
  Vertex* vertex_data;
  u32     vertex_count;
  u32     vertex_capacity;
  
  // Indices data
  u32  triangles_ebo;
  u32* triangles_indices_data;
  u32  triangles_indices_count;
  u32  triangles_indices_capacity;
  
  // Lines data
  u32  lines_ebo;
  u32* lines_indices_data;
  u32  lines_indices_count;
  u32  lines_indices_capacity;
  
  u32 textures_data[Initial_Textures];
  u32 textures_count;
  
} Renderer;

Renderer GlobalRenderer;

internal void renderer_init(s32 window_width, s32 window_height);
internal void renderer_draw(Mat4f32 view, Mat4f32 projection, s32 window_width, s32 window_height);
internal void renderer_on_resize(s32 window_width, s32 window_height);

internal u32 renderer_load_color_texture(f32 r, f32 g, f32 b, f32 a);

internal void renderer_push_quad(Vec3f32 bot_left_point, Vec4f32 color, f32 width, f32 height, u32 texture);
internal void renderer_push_triangle(Vertex a, Vertex b, Vertex c, u32 texture);
internal void renderer_push_line(Vec3f32 a_position, Vec3f32 b_position, u32 texture);
internal u32  renderer_push_vertex(Vertex v);
internal void renderer_push_triangle_index(u32 index);
internal void renderer_push_line_index(u32 index);

internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);
internal void renderer_set_array_s32(u32 program, const char* uniform, s32 count, s32* ptr);
internal void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s);

#endif //RENDERER_H
