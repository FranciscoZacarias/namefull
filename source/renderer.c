internal void renderer_init(s32 window_width, s32 window_height) {
  AssertNoReentry();
  
  MemoryZeroStruct(&GRenderer);
  
  GRenderer.arena = arena_init();
  
  GRenderer.vertices_capacity = Kilobytes(64);
  GRenderer.vertices_data  = ArenaPush(GRenderer.arena, Vertex, GRenderer.vertices_capacity);
  GRenderer.vertices_count = 0;
  
  GRenderer.triangles_indices_capacity = Kilobytes(16);
  GRenderer.triangles_indices_data  = (u32*)ArenaPush(GRenderer.arena, u32, GRenderer.triangles_indices_capacity);
  GRenderer.triangles_indices_count = 0;
  
  GRenderer.lines_indices_capacity = Kilobytes(16);
  GRenderer.lines_indices_data  = ArenaPush(GRenderer.arena, u32, GRenderer.lines_indices_capacity);
  GRenderer.lines_indices_count = 0;
  
  GRenderer.textures_capacity = Initial_Textures;
  GRenderer.textures_data  = ArenaPush(GRenderer.arena, u32, GRenderer.textures_capacity);
  GRenderer.textures_count = 0;
  
  Arena_Temp scratch = scratch_begin(0, 0);
  
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(MAIN_VS));
    glShaderSource(vertex_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(vertex_shader);
    s32 success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[1024];
      glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
      printf("Error %d compiling default vertex shader. Log: %s", success, infoLog);
      Assert(0);
    }
  }
  
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    OS_File fragment_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(MAIN_FS));
    glShaderSource(fragment_shader, 1, &fragment_shader_source.data, &(GLint)fragment_shader_source.size);
    glCompileShader(fragment_shader);
    s32 success;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[1024];
      glGetShaderInfoLog(fragment_shader, 1024, NULL, infoLog);
      printf("Error %d compiling default fragment shader. Log: %s", success, infoLog);
      Assert(0);
    }
  }
  
  GRenderer.main_shader = glCreateProgram();
  {
    glAttachShader(GRenderer.main_shader, vertex_shader);
    glAttachShader(GRenderer.main_shader, fragment_shader);
    glLinkProgram(GRenderer.main_shader);
    s32 success;
    glGetProgramiv(GRenderer.main_shader, GL_LINK_STATUS, &success);
    if(!success) {
      char infoLog[1024];
      glGetProgramInfoLog(GRenderer.main_shader, 1024, NULL, infoLog);
      printf("Error %d linking shader program. Log: %s", success, infoLog);
      Assert(0);
    }
  }
  
  glDetachShader(GRenderer.main_shader, vertex_shader);
  glDetachShader(GRenderer.main_shader, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  glCreateVertexArrays(1, &GRenderer.vertices_vao);
  {
    glEnableVertexArrayAttrib (GRenderer.vertices_vao, 0);
    glVertexArrayAttribFormat (GRenderer.vertices_vao, 0, 3, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, position));
    glVertexArrayAttribBinding(GRenderer.vertices_vao, 0, 0);
    
    glEnableVertexArrayAttrib (GRenderer.vertices_vao, 1);
    glVertexArrayAttribFormat (GRenderer.vertices_vao, 1, 4, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, color));
    glVertexArrayAttribBinding(GRenderer.vertices_vao, 1, 0);
    
    glEnableVertexArrayAttrib (GRenderer.vertices_vao, 2);
    glVertexArrayAttribFormat (GRenderer.vertices_vao, 2, 2, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, uv));
    glVertexArrayAttribBinding(GRenderer.vertices_vao, 2, 0);
    
    glEnableVertexArrayAttrib (GRenderer.vertices_vao, 4);
    glVertexArrayAttribFormat (GRenderer.vertices_vao, 4, 3, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, normal));
    glVertexArrayAttribBinding(GRenderer.vertices_vao, 4, 0);

    glEnableVertexArrayAttrib (GRenderer.vertices_vao, 4);
    glVertexArrayAttribFormat (GRenderer.vertices_vao, 4, 1, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, texture));
    glVertexArrayAttribBinding(GRenderer.vertices_vao, 4, 0);
    
    glCreateBuffers(1, &GRenderer.vertices_vbo);
    glNamedBufferData(GRenderer.vertices_vbo, sizeof(Vertex) * Initial_Vertices, NULL, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(GRenderer.vertices_vao, 0, GRenderer.vertices_vbo, 0, sizeof(Vertex));
  }
  
  glCreateBuffers(1, &GRenderer.triangles_ebo);
  glNamedBufferData(GRenderer.triangles_ebo, sizeof(u32) * Initial_Indices, NULL, GL_STATIC_DRAW);
  
  glCreateBuffers(1, &GRenderer.lines_ebo);
  glNamedBufferData(GRenderer.lines_ebo, sizeof(u32) * Initial_Indices, NULL, GL_STATIC_DRAW);
  
  // MSAA
  {
    glGenFramebuffers(1, &GRenderer.msaa_fbo);
    glGenTextures(1, &GRenderer.msaa_texture_color_buffer_multisampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, GRenderer.msaa_texture_color_buffer_multisampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, window_width, window_height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    
    glGenRenderbuffers(1, &GRenderer.msaa_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, GRenderer.msaa_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, window_width, window_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GRenderer.msaa_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, GRenderer.msaa_texture_color_buffer_multisampled, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GRenderer.msaa_rbo);
    
    u32 msaa_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (msaa_fbo_status != GL_FRAMEBUFFER_COMPLETE) {
      printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", msaa_fbo_status);
      Assert(0);
    }
  }
  
  // Post processing and screen texture
  {
    glGenFramebuffers(1, &GRenderer.post_processing_fbo);
    glGenTextures(1, &GRenderer.screen_texture);
    glBindTexture(GL_TEXTURE_2D, GRenderer.screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // attach to intermidiate_fbo
    glBindFramebuffer(GL_FRAMEBUFFER, GRenderer.post_processing_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GRenderer.screen_texture, 0);
    
    u32 postprocessing_fbo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (postprocessing_fbo != GL_FRAMEBUFFER_COMPLETE) {
      printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", postprocessing_fbo);
      Assert(0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  // --- Screen shader
  u32 screen_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(SCREEN_VS));
    glShaderSource(screen_vertex_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(screen_vertex_shader);
    {
      s32 success;
      glGetShaderiv(screen_vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(screen_vertex_shader, 1024, NULL, infoLog);
        printf("Error %d compiling screen vertex shader. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  u32 screen_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(SCREEN_FS));
    glShaderSource(screen_fragment_shader, 1, &vertex_shader_source.data, &(GLint)vertex_shader_source.size);
    glCompileShader(screen_fragment_shader);
    {
      s32 success;
      glGetShaderiv(screen_fragment_shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(screen_fragment_shader, 1024, NULL, infoLog);
        printf("Error %d compiling screen fragment shader. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  GRenderer.screen_shader = glCreateProgram();
  {
    glAttachShader(GRenderer.screen_shader, screen_vertex_shader);
    glAttachShader(GRenderer.screen_shader, screen_fragment_shader);
    glLinkProgram(GRenderer.screen_shader);
    {
      s32 success;
      glGetProgramiv(GRenderer.screen_shader, GL_LINK_STATUS, &success);
      if(!success) {
        char infoLog[1024];
        glGetProgramInfoLog(GRenderer.screen_shader, 1024, NULL, infoLog);
        printf("Error %d linking shader program. Log: %s", success, infoLog);
        Assert(0);
      }
    }
  }
  
  f32 screen_vertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f
  };
  
  glCreateVertexArrays(1, &GRenderer.screen_vao);
  
  glEnableVertexArrayAttrib (GRenderer.screen_vao, 0);
  glVertexArrayAttribFormat (GRenderer.screen_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(GRenderer.screen_vao, 0, 0);
  
  glCreateBuffers(1, &GRenderer.screen_vbo);
  glNamedBufferData(GRenderer.screen_vbo, sizeof(screen_vertices), &screen_vertices, GL_STATIC_DRAW);
  glVertexArrayVertexBuffer(GRenderer.screen_vao, 0, GRenderer.screen_vbo, 0, 2*sizeof(f32));
  
  glUseProgram(GRenderer.main_shader);
  u32 texture_ids[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  renderer_set_array_s32(GRenderer.main_shader, "u_texture", 8, texture_ids);
  glUseProgram(0);
  
  GRenderer.textures_count = 0;

  glCullFace(GL_FRONT);
  glFrontFace(GL_CCW);
  
  scratch_end(&scratch);
}

internal void renderer_draw(Matrix4 view, Matrix4 projection, s32 window_width, s32 window_height) {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GRenderer.msaa_fbo);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  glUseProgram(GRenderer.main_shader);
  renderer_set_uniform_mat4fv(GRenderer.main_shader, "u_model",      matrix4(1.0f));
  renderer_set_uniform_mat4fv(GRenderer.main_shader, "u_view",       view);
  renderer_set_uniform_mat4fv(GRenderer.main_shader, "u_projection", projection);
  
  for (u32 i = 0; i < GRenderer.textures_count; i += 1) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, GRenderer.textures_data[i]);
  }
  
  // Draw to msaa_fbo
  {
    glBindVertexArray(GRenderer.vertices_vao);
    glEnable(GL_CULL_FACE);

    // TODO(fz): We don't need to do NamedBufferSubData all the time, since we don't clear the buffers every frame.

    // Triangles
    glVertexArrayElementBuffer(GRenderer.vertices_vao, GRenderer.triangles_ebo);
    glNamedBufferSubData(GRenderer.vertices_vbo, 0, GRenderer.vertices_count * sizeof(Vertex), GRenderer.vertices_data);
    glNamedBufferSubData(GRenderer.triangles_ebo, 0, GRenderer.triangles_indices_count * sizeof(u32), GRenderer.triangles_indices_data);
    glDrawElements(GL_TRIANGLES, GRenderer.triangles_indices_count, GL_UNSIGNED_INT, NULL);

    // Lines
    glVertexArrayElementBuffer(GRenderer.vertices_vao, GRenderer.lines_ebo);
    glNamedBufferSubData(GRenderer.vertices_vbo, 0, GRenderer.vertices_count * sizeof(Vertex), GRenderer.vertices_data);
    glNamedBufferSubData(GRenderer.lines_ebo, 0, GRenderer.lines_indices_count * sizeof(u32), GRenderer.lines_indices_data);
    glDrawElements(GL_LINES, GRenderer.lines_indices_count, GL_UNSIGNED_INT, NULL);

    glDisable(GL_CULL_FACE);
    glBindVertexArray(0);
  }
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, GRenderer.msaa_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GRenderer.post_processing_fbo);	
  glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  
  glUseProgram(GRenderer.screen_shader);
  glBindVertexArray(GRenderer.screen_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, GRenderer.screen_texture);
  
  renderer_set_uniform_s32(GRenderer.screen_shader, "u_window_width", window_width);
  renderer_set_uniform_s32(GRenderer.screen_shader, "u_window_height", window_height);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  
  glUseProgram(0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

internal void renderer_on_resize(s32 window_width, s32 window_height) {
  // Delete and recreate MSAA framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, GRenderer.msaa_fbo);
  glDeleteTextures(1, &GRenderer.msaa_texture_color_buffer_multisampled);
  glDeleteRenderbuffers(1, &GRenderer.msaa_rbo);
  
  // Regen MSAA buffer
  glGenFramebuffers(1, &GRenderer.msaa_fbo);
  
  glGenTextures(1, &GRenderer.msaa_texture_color_buffer_multisampled);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, GRenderer.msaa_texture_color_buffer_multisampled);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, window_width, window_height, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  
  glGenRenderbuffers(1, &GRenderer.msaa_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, GRenderer.msaa_rbo);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, window_width, window_height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GRenderer.msaa_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, GRenderer.msaa_texture_color_buffer_multisampled, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GRenderer.msaa_rbo);
  
  u32 msaa_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (msaa_fbo_status != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", msaa_fbo_status);
    Assert(0);
  }
  
  // --- Intermidiate fbo for post processing
  glGenFramebuffers(1, &GRenderer.post_processing_fbo);
  glDeleteTextures(1, &GRenderer.screen_texture);
  
  // create color attachment texture
  glGenTextures(1, &GRenderer.screen_texture);
  glBindTexture(GL_TEXTURE_2D, GRenderer.screen_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // attach to intermidiate_fbo
  glBindFramebuffer(GL_FRAMEBUFFER, GRenderer.post_processing_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GRenderer.screen_texture, 0);
  
  u32 postprocessing_fbo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (postprocessing_fbo != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", postprocessing_fbo);
    Assert(0);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

internal f32 renderer_load_color_texture(f32 r, f32 g, f32 b, f32 a) {
  u8 texture_data[4] = {
    (u8)(255.0*r),
    (u8)(255.0*g),
    (u8)(255.0*b),
    (u8)(255.0*a)
  };
  
  u32 texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glBindTexture(GL_TEXTURE_2D, 0);
  
  u32 texture_index = GRenderer.textures_count;
  GRenderer.textures_data[texture_index] = texture_id;
  GRenderer.textures_count += 1;
  
  return (f32)texture_index;
}

internal Model renderer_load_obj(String path) {
  Model result = { 0 };

  tinyobj_attrib_t attrib = { 0 };

  u32 mesh_count          = 0;
  tinyobj_shape_t *meshes = NULL;
  u32 material_count           = 0;
  tinyobj_material_t *materials = NULL;
  
  Arena_Temp scratch = scratch_begin(0, 0);
  OS_File file = os_file_load_entire_file(scratch.arena, path);
  if (file.size == 0) {
    printf("Error loading file %s.", path.str);
    Assert(0);
  }

  s32 tinyobj_result = tinyobj_parse_obj(&attrib, &meshes, &mesh_count, &materials, &material_count, file.data, file.size, TINYOBJ_FLAG_TRIANGULATE);
  if (tinyobj_result != TINYOBJ_SUCCESS) {
    printf("Error on tinyobj_parse_obj.");
    Assert(0);
  }

  result.mesh_count     = mesh_count;
  result.material_count = material_count;
  if (result.material_count == 0) {
      printf("No materials provided, setting one default material for all meshes.\n");
      result.material_count = 1;
  }

  result.arena  = arena_init();

  scratch_end(&scratch);
  return result;
}

internal void renderer_push_triangle(Vertex a, Vertex b, Vertex c) {
  if (GRenderer.triangles_indices_count + 3 > GRenderer.triangles_indices_capacity) {
    printf("Too many triangles indices");
    Assert(0);
  }
  if (GRenderer.vertices_count + 3 > GRenderer.vertices_capacity) {
    printf("Too many vertices");
    Assert(0);
  }

  // TODO(fz): Should try making this a hashtable lookup

  b32 a_exists = false;
  b32 b_exists = false;
  b32 c_exists = false;
  for (u32 i = 0; i < GRenderer.vertices_count; i += 1) {
    Vertex it = GRenderer.vertices_data[i];

    if (!a_exists && MemoryMatch(&it, &a, sizeof(Vertex))) {
      GRenderer.triangles_indices_data[GRenderer.triangles_indices_count] = i;
      GRenderer.triangles_indices_count += 1;
      a_exists = true;
    }
    if (!b_exists && MemoryMatch(&it, &b, sizeof(Vertex))) {
      GRenderer.triangles_indices_data[GRenderer.triangles_indices_count] = i;
      GRenderer.triangles_indices_count += 1;
      b_exists = true;
    }
    if (!c_exists && MemoryMatch(&it, &c, sizeof(Vertex))) {
      GRenderer.triangles_indices_data[GRenderer.triangles_indices_count] = i;
      GRenderer.triangles_indices_count += 1;
      c_exists = true;
    }
    if (a_exists && b_exists && c_exists) {
      break;
    }
  }

  if (!a_exists) {
    GRenderer.vertices_data[GRenderer.vertices_count] = a;
    GRenderer.triangles_indices_data[GRenderer.triangles_indices_count] = GRenderer.vertices_count;
    GRenderer.vertices_count          += 1;
    GRenderer.triangles_indices_count += 1;
  }
  if (!b_exists) {
    GRenderer.vertices_data[GRenderer.vertices_count] = b;
    GRenderer.triangles_indices_data[GRenderer.triangles_indices_count] = GRenderer.vertices_count;
    GRenderer.vertices_count          += 1;
    GRenderer.triangles_indices_count += 1;
  }
  if (!c_exists) {
    GRenderer.vertices_data[GRenderer.vertices_count] = c;
    GRenderer.triangles_indices_data[GRenderer.triangles_indices_count] = GRenderer.vertices_count;
    GRenderer.vertices_count          += 1;
    GRenderer.triangles_indices_count += 1;
  }
}

internal void renderer_push_line(Vector3 a_position, Vector3 b_position, u32 texture) {
  if (GRenderer.lines_indices_count + 3 > GRenderer.lines_indices_capacity) {
    printf("Too many lines indices");
    Assert(0);
  }
  if (GRenderer.vertices_count + 3 > GRenderer.vertices_capacity) {
    printf("Too many vertices");
    Assert(0);
  }

  Vertex a = vertex(a_position, vector4(1.0f, 1.0f, 1.0f, 1.0f), vector2(0.0f, 0.0f), vector3(0.0, 0.0, 0.0), texture);
  Vertex b = vertex(b_position, vector4(1.0f, 1.0f, 1.0f, 1.0f), vector2(0.0f, 0.0f), vector3(0.0, 0.0, 0.0), texture);
  
  b32 a_exists = false;
  b32 b_exists = false;
  for (u32 i = 0; i < GRenderer.vertices_count; i += 1) {
    Vertex it = GRenderer.vertices_data[i];

    if (!a_exists && MemoryMatch(&it, &a, sizeof(Vertex))) {
      GRenderer.lines_indices_data[GRenderer.lines_indices_count] = i;
      GRenderer.lines_indices_count += 1;
      a_exists = true;
    }
    if (!b_exists && MemoryMatch(&it, &b, sizeof(Vertex))) {
      GRenderer.lines_indices_data[GRenderer.lines_indices_count] = i;
      GRenderer.lines_indices_count += 1;
      b_exists = true;
    }
    if (a_exists && b_exists) {
      break;
    }
  }

  if (!a_exists) {
    GRenderer.vertices_data[GRenderer.vertices_count] = a;
    GRenderer.lines_indices_data[GRenderer.lines_indices_count] = GRenderer.vertices_count;
    GRenderer.vertices_count      += 1;
    GRenderer.lines_indices_count += 1;
  }
  if (!b_exists) {
    GRenderer.vertices_data[GRenderer.vertices_count] = b;
    GRenderer.lines_indices_data[GRenderer.lines_indices_count] = GRenderer.vertices_count;
    GRenderer.vertices_count      += 1;
    GRenderer.lines_indices_count += 1;
  }
}

internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Matrix4 mat) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Matrix4 :: Uniform %s not found\n", uniform);
    return;
  }
  glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}

internal void renderer_set_array_s32(u32 program, const char* uniform, s32 count, s32* ptr) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Array[s32] :: Uniform %s not found\n", uniform);
    return;
  }
  glUniform1iv(uniform_location, count, ptr);
}

internal void renderer_set_uniform_s32(u32 program, const char* uniform, s32 s) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("s32 :: Uniform %s not found\n", uniform);
    return;
  }
  glUniform1i(uniform_location, s);
}