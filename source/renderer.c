internal void
renderer_init(s32 window_width, s32 window_height) {
  AssertNoReentry();
  
  MemoryZeroStruct(&GlobalRenderer);
  
  GlobalRenderer.arena = arena_init();
  
  GlobalRenderer.vertex_capacity = Kilobytes(64);
  GlobalRenderer.vertex_data  = ArenaPush(GlobalRenderer.arena, Vertex, GlobalRenderer.vertex_capacity);
  
  GlobalRenderer.triangles_indices_capacity = Kilobytes(16);
  GlobalRenderer.triangles_indices_data   = (u32*)ArenaPush(GlobalRenderer.arena, u32, GlobalRenderer.triangles_indices_capacity);
  
  GlobalRenderer.lines_indices_capacity = Kilobytes(16);
  GlobalRenderer.lines_indices_data     =  ArenaPush(GlobalRenderer.arena, u32, GlobalRenderer.lines_indices_capacity);
  
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
  
  GlobalRenderer.main_shader = glCreateProgram();
  {
    glAttachShader(GlobalRenderer.main_shader, vertex_shader);
    glAttachShader(GlobalRenderer.main_shader, fragment_shader);
    glLinkProgram(GlobalRenderer.main_shader);
    s32 success;
    glGetProgramiv(GlobalRenderer.main_shader, GL_LINK_STATUS, &success);
    if(!success) {
      char infoLog[1024];
      glGetProgramInfoLog(GlobalRenderer.main_shader, 1024, NULL, infoLog);
      printf("Error %d linking shader program. Log: %s", success, infoLog);
      Assert(0);
    }
  }
  
  glDetachShader(GlobalRenderer.main_shader, vertex_shader);
  glDetachShader(GlobalRenderer.main_shader, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  glCreateVertexArrays(1, &GlobalRenderer.vertex_vao);
  {
    glEnableVertexArrayAttrib (GlobalRenderer.vertex_vao, 0);
    glVertexArrayAttribFormat (GlobalRenderer.vertex_vao, 0, 3, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, position));
    glVertexArrayAttribBinding(GlobalRenderer.vertex_vao, 0, 0);
    
    glEnableVertexArrayAttrib (GlobalRenderer.vertex_vao, 1);
    glVertexArrayAttribFormat (GlobalRenderer.vertex_vao, 1, 4, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, color));
    glVertexArrayAttribBinding(GlobalRenderer.vertex_vao, 1, 0);
    
    glEnableVertexArrayAttrib (GlobalRenderer.vertex_vao, 2);
    glVertexArrayAttribFormat (GlobalRenderer.vertex_vao, 2, 2, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, uv));
    glVertexArrayAttribBinding(GlobalRenderer.vertex_vao, 2, 0);
    
    glEnableVertexArrayAttrib (GlobalRenderer.vertex_vao, 3);
    glVertexArrayAttribFormat (GlobalRenderer.vertex_vao, 3, 1, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, texture));
    glVertexArrayAttribBinding(GlobalRenderer.vertex_vao, 3, 0);
    
    glCreateBuffers(1, &GlobalRenderer.vertex_vbo);
    glNamedBufferData(GlobalRenderer.vertex_vbo, sizeof(Vertex) * Initial_Vertices, NULL, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(GlobalRenderer.vertex_vao, 0, GlobalRenderer.vertex_vbo, 0, sizeof(Vertex));
  }
  
  glCreateBuffers(1, &GlobalRenderer.triangles_ebo);
  glNamedBufferData(GlobalRenderer.triangles_ebo, sizeof(u32) * Initial_Indices, NULL, GL_STATIC_DRAW);
  
  glCreateBuffers(1, &GlobalRenderer.lines_ebo);
  glNamedBufferData(GlobalRenderer.lines_ebo, sizeof(u32) * Initial_Indices, NULL, GL_STATIC_DRAW);
  
  // MSAA
  {
    glGenFramebuffers(1, &GlobalRenderer.msaa_fbo);
    glGenTextures(1, &GlobalRenderer.msaa_texture_color_buffer_multisampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, GlobalRenderer.msaa_texture_color_buffer_multisampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, window_width, window_height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    
    glGenRenderbuffers(1, &GlobalRenderer.msaa_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, GlobalRenderer.msaa_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, window_width, window_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GlobalRenderer.msaa_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, GlobalRenderer.msaa_texture_color_buffer_multisampled, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GlobalRenderer.msaa_rbo);
    
    u32 msaa_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (msaa_fbo_status != GL_FRAMEBUFFER_COMPLETE) {
      printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", msaa_fbo_status);
      Assert(0);
    }
  }
  
  // Post processing and screen texture
  {
    glGenFramebuffers(1, &GlobalRenderer.post_processing_fbo);
    glGenTextures(1, &GlobalRenderer.screen_texture);
    glBindTexture(GL_TEXTURE_2D, GlobalRenderer.screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // attach to intermidiate_fbo
    glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderer.post_processing_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GlobalRenderer.screen_texture, 0);
    
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
  
  GlobalRenderer.screen_shader = glCreateProgram();
  {
    glAttachShader(GlobalRenderer.screen_shader, screen_vertex_shader);
    glAttachShader(GlobalRenderer.screen_shader, screen_fragment_shader);
    glLinkProgram(GlobalRenderer.screen_shader);
    {
      s32 success;
      glGetProgramiv(GlobalRenderer.screen_shader, GL_LINK_STATUS, &success);
      if(!success) {
        char infoLog[1024];
        glGetProgramInfoLog(GlobalRenderer.screen_shader, 1024, NULL, infoLog);
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
  
  glCreateVertexArrays(1, &GlobalRenderer.screen_vao);
  
  glEnableVertexArrayAttrib (GlobalRenderer.screen_vao, 0);
  glVertexArrayAttribFormat (GlobalRenderer.screen_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(GlobalRenderer.screen_vao, 0, 0);
  
  glCreateBuffers(1, &GlobalRenderer.screen_vbo);
  glNamedBufferData(GlobalRenderer.screen_vbo, sizeof(screen_vertices), &screen_vertices, GL_STATIC_DRAW);
  glVertexArrayVertexBuffer(GlobalRenderer.screen_vao, 0, GlobalRenderer.screen_vbo, 0, 2*sizeof(f32));
  
  glUseProgram(GlobalRenderer.main_shader);
  u32 texture_ids[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  renderer_set_array_s32(GlobalRenderer.main_shader, "u_texture", 8, texture_ids);
  glUseProgram(0);
  
  GlobalRenderer.textures_count = 0;
  
  scratch_end(&scratch);
}

internal void
renderer_draw(Mat4f32 view, Mat4f32 projection, s32 window_width, s32 window_height) {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GlobalRenderer.msaa_fbo);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  glUseProgram(GlobalRenderer.main_shader);
  renderer_set_uniform_mat4fv(GlobalRenderer.main_shader, "u_model",      mat4f32(1.0f));
  renderer_set_uniform_mat4fv(GlobalRenderer.main_shader, "u_view",       view);
  renderer_set_uniform_mat4fv(GlobalRenderer.main_shader, "u_projection", projection);
  
  for (u32 i = 0; i < GlobalRenderer.textures_count; i += 1) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, GlobalRenderer.textures_data[i]);
  }
  
  // Draw to msaa_fbo
  {
    // TODO(fz): We don't need to do NamedBufferSubData all the time, since we don't clear the buffers every frame.
    
    // Triangles
    // glBindVertexArray(GlobalRenderer.vertex_vao);
    // glNamedBufferSubData(GlobalRenderer.vertex_vbo, 0, GlobalRenderer.vertex_count * sizeof(Vertex), GlobalRenderer.vertex_data);
    // glNamedBufferSubData(GlobalRenderer.vertex_ebo, 0, GlobalRenderer.triangles_indices_count * sizeof(u32), GlobalRenderer.triangles_indices_data);
    // glDrawElements(GL_TRIANGLES, GlobalRenderer.triangles_indices_count, GL_UNSIGNED_INT, NULL);
    
    // Lines
    glBindVertexArray(GlobalRenderer.vertex_vao);
    glVertexArrayElementBuffer(GlobalRenderer.vertex_vao, GlobalRenderer.lines_ebo);
    glNamedBufferSubData(GlobalRenderer.vertex_vbo, 0, GlobalRenderer.vertex_count * sizeof(Vertex), GlobalRenderer.vertex_data);
    glNamedBufferSubData(GlobalRenderer.lines_ebo, 0, GlobalRenderer.lines_indices_count * sizeof(u32), GlobalRenderer.lines_indices_data);
    glDrawElements(GL_LINES, GlobalRenderer.lines_indices_count, GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
  }
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, GlobalRenderer.msaa_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GlobalRenderer.post_processing_fbo);	
  glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  
  glUseProgram(GlobalRenderer.screen_shader);
  glBindVertexArray(GlobalRenderer.screen_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, GlobalRenderer.screen_texture);
  
  renderer_set_uniform_s32(GlobalRenderer.screen_shader, "u_window_width", window_width);
  renderer_set_uniform_s32(GlobalRenderer.screen_shader, "u_window_height", window_height);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  
  glUseProgram(0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

internal void
renderer_on_resize(s32 window_width, s32 window_height) {
  // Delete and recreate MSAA framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderer.msaa_fbo);
  glDeleteTextures(1, &GlobalRenderer.msaa_texture_color_buffer_multisampled);
  glDeleteRenderbuffers(1, &GlobalRenderer.msaa_rbo);
  
  // Regen MSAA buffer
  glGenFramebuffers(1, &GlobalRenderer.msaa_fbo);
  
  glGenTextures(1, &GlobalRenderer.msaa_texture_color_buffer_multisampled);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, GlobalRenderer.msaa_texture_color_buffer_multisampled);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, window_width, window_height, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  
  glGenRenderbuffers(1, &GlobalRenderer.msaa_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, GlobalRenderer.msaa_rbo);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, window_width, window_height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GlobalRenderer.msaa_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, GlobalRenderer.msaa_texture_color_buffer_multisampled, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GlobalRenderer.msaa_rbo);
  
  u32 msaa_fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (msaa_fbo_status != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", msaa_fbo_status);
    Assert(0);
  }
  
  // --- Intermidiate fbo for post processing
  glGenFramebuffers(1, &GlobalRenderer.post_processing_fbo);
  glDeleteTextures(1, &GlobalRenderer.screen_texture);
  
  // create color attachment texture
  glGenTextures(1, &GlobalRenderer.screen_texture);
  glBindTexture(GL_TEXTURE_2D, GlobalRenderer.screen_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // attach to intermidiate_fbo
  glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderer.post_processing_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GlobalRenderer.screen_texture, 0);
  
  u32 postprocessing_fbo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (postprocessing_fbo != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR::GL_FRAMEBUFFER:: Render Buffer Object is not complete. Value: %u. ", postprocessing_fbo);
    Assert(0);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

internal u32
renderer_load_color_texture(f32 r, f32 g, f32 b, f32 a) {
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
  
  u32 texture_index = GlobalRenderer.textures_count;
  GlobalRenderer.textures_data[texture_index] = texture_id;
  GlobalRenderer.textures_count += 1;
  
  return texture_index;
}

internal void
renderer_push_line(Vec3f32 a_position, Vec3f32 b_position, u32 texture) {
  if (GlobalRenderer.lines_indices_count + 1 > GlobalRenderer.lines_indices_capacity) {
    printf("Error :: Renderer :: Too many lines!");
    Assert(0);
  }
  
  Vertex a = {
    a_position,
    vec4f32(1.0f, 1.0f, 1.0f, 1.0f),
    vec2f32(0.0f, 0.0f),
    texture
  };
  
  Vertex b = {
    b_position,
    vec4f32(1.0f, 1.0f, 1.0f, 1.0f),
    vec2f32(0.0f, 0.0f),
    texture
  };
  
  u32 aidx = renderer_push_vertex(a);
  renderer_push_line_index(aidx);
  u32 bidx = renderer_push_vertex(b);
  renderer_push_line_index(bidx);
}

internal void
renderer_push_triangle(Vertex a, Vertex b, Vertex c, u32 texture) {
  if (GlobalRenderer.triangles_indices_count + 1 > GlobalRenderer.triangles_indices_capacity) {
    printf("Error :: Renderer :: Too many triangles!");
    Assert(0);
  }
  
  u32 aidx = renderer_push_vertex(a);
  renderer_push_triangle_index(aidx);
  u32 bidx = renderer_push_vertex(b);
  renderer_push_triangle_index(bidx);
  u32 cidx = renderer_push_vertex(c);
  renderer_push_triangle_index(cidx);
}

internal u32
renderer_push_vertex(Vertex v) {
  if (GlobalRenderer.vertex_count + 1 > GlobalRenderer.vertex_capacity) {
    printf("Too many vertices");
    Assert(0);
  }
  
  u32 index = U32_MAX;
  
  // TODO(fz): Maybe we can try this but with a hash table?
  for (u32 i = 0; i < GlobalRenderer.vertex_count; i += 1) {
    if (MemoryMatch(&v, &GlobalRenderer.vertex_data[i], sizeof(Vertex))) {
      index = true;
      break;
    }
  }
  
  if (index == U32_MAX) {
    index = GlobalRenderer.vertex_count;
    GlobalRenderer.vertex_data[GlobalRenderer.vertex_count] = v;
    GlobalRenderer.vertex_count += 1;
  }
  
  return index;
}

internal void
renderer_push_triangle_index(u32 index) {
  if (GlobalRenderer.triangles_indices_count + 1 > GlobalRenderer.triangles_indices_capacity) {
    printf("Too many indices");
    Assert(0);
  }
  GlobalRenderer.triangles_indices_data[GlobalRenderer.triangles_indices_count] = index;
  GlobalRenderer.triangles_indices_count += 1;
}

internal void
renderer_push_line_index(u32 index) {
  if (GlobalRenderer.lines_indices_count + 1 > GlobalRenderer.lines_indices_capacity) {
    printf("Too many indices");
    Assert(0);
  }
  GlobalRenderer.lines_indices_data[GlobalRenderer.lines_indices_count] = index;
  GlobalRenderer.lines_indices_count += 1;
}

internal void
renderer_push_quad(Vec3f32 bot_left_point, Vec4f32 color, f32 width, f32 height, u32 texture) {
  Vec3f32 a = bot_left_point;
  Vec3f32 b = vec3f32(a.x+width, a.y,        a.z);
  Vec3f32 c = vec3f32(a.x+width, a.y+height, a.z);
  Vec3f32 d = vec3f32(a.x,       a.y+height, a.z);
  
  Vertex va = vertex(a, color, vec2f32(0.0f, 0.0f), texture);
  Vertex vb = vertex(b, color, vec2f32(1.0f, 0.0f), texture);
  Vertex vc = vertex(c, color, vec2f32(1.0f, 1.0f), texture);
  Vertex vd = vertex(d, color, vec2f32(0.0f, 1.0f), texture);
  
  renderer_push_triangle(va, vb, vc, texture);
  renderer_push_triangle(va, vc, vd, texture);
}

internal void
renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Mat4f32 :: Uniform %s not found\n", uniform);
    return;
  }
  glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}

internal void
renderer_set_array_s32(u32 program, const char* uniform, s32 count, s32* ptr) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Array[s32] :: Uniform %s not found\n", uniform);
    return;
  }
  glUniform1iv(uniform_location, count, ptr);
}

internal void
renderer_set_uniform_s32(u32 program, const char* uniform, s32 s) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("s32 :: Uniform %s not found\n", uniform);
    return;
  }
  glUniform1i(uniform_location, s);
}