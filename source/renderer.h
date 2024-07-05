/* date = July 5th 2024 2:45 pm */

#ifndef RENDERER_H
#define RENDERER_H

#define MSAA_SAMPLES 8

#define MAIN_VS_SHADER   "D:\\work\\noname\\src\\shaders\\default_vertex_shader.glsl"
#define MAIN_FS_SHADER "D:\\work\\noname\\src\\shaders\\default_fragment_shader.glsl"
#define SCREEN_VS_SHADER    "D:\\work\\noname\\src\\shaders\\screen_vertex_shader.glsl"
#define SCREEN_FS_SHADER  "D:\\work\\noname\\src\\shaders\\screen_fragment_shader.glsl"

typedef struct Vertex {
  Vec3f32 position;
  Vec4f32 color;
  Vec2f32 uv;
  Vec3f32 normal;
  u32 texture;
}

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
  u32  triangles_count;
  u32  triangles_capacity;
  u32* triangles_indices_data;
  u32  triangles_indices_count;
  u32  triangles_indices_capacity;
  
  Vertex* lines_data;
  u32 lines_count;
  u32 lines_capacity;
  
  u32 textures[8];
  
} Renderer;

internal Renderer renderer_init(MainWindow* window);

#endif //RENDERER_H
