internal Renderer
renderer_init() {
  AssertNoReentry();
  
  MemoryZeroStruct(&GlobalRenderer);
  
  GlobalRenderer.arena = arena_init();
  
  GlobalRenderer.triangles_capacity = Kilobytes(16);
  GlobalRenderer.triangles_data = ArenaPush(GlobalRenderer.arena, Vertex, GlobalRenderer.triangles_capacity*3);
  
  GlobalRenderer.triangles_indices_capacity = GlobalRenderer.triangles_capacity * 3;
  GlobalRenderer.triangles_indices_data = (u32*)ArenaPush(GlobalRenderer.arena, u32, GlobalRenderer.triangles_indices_capacity);
  
  GlobalRenderer.lines_capacity = 3;
  GlobalRenderer.lines_data = ArenaPush(GlobalRenderer.arena, Vertex, GlobalRenderer.lines_capacity*2);
  
  Arena_Temp scratch = scratch_begin(0, 0);
  
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  {
    OS_File vertex_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(MAIN_VS_SHADER));
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
    OS_File fragment_shader_source = os_file_load_entire_file(scratch.arena, StringLiteral(MAIN_FS_SHADER));
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
  
  glCreateVertexArrays(1, &GlobalRenderer.triangles_vao);
  {
    glEnableVertexArrayAttrib(GlobalRenderer.triangles_vao, 0);
    glVertexArrayAttribFormat(GlobalRenderer.triangles_vao, 0, 3, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, position));
    glVertexArrayAttribBinding(GlobalRenderer.triangles_vao, 0, 0);
    
    glEnableVertexArrayAttrib(GlobalRenderer.triangles_vao, 1);
    glVertexArrayAttribFormat(GlobalRenderer.triangles_vao, 1, 4, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, color));
    glVertexArrayAttribBinding(GlobalRenderer.triangles_vao, 1, 0);
    
    glEnableVertexArrayAttrib(GlobalRenderer.triangles_vao, 2);
    glVertexArrayAttribFormat(GlobalRenderer.triangles_vao, 2, 2, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, uv));
    glVertexArrayAttribBinding(GlobalRenderer.triangles_vao, 2, 0);
    
    glEnableVertexArrayAttrib(GlobalRenderer.triangles_vao, 3);
    glVertexArrayAttribFormat(GlobalRenderer.triangles_vao, 3, 3, GL_FLOAT, GL_FALSE, OffsetOfMember(Vertex, normal));
    glVertexArrayAttribBinding(GlobalRenderer.triangles_vao, 3, 0);
    
    glCreateBuffers(1, &GlobalRenderer.triangles_vbo);
    glNamedBufferData(GlobalRenderer.triangles_vbo, sizeof(Vertex) * 3 * Initial_Vertices, NULL, GL_DYNAMIC_DRAW);
    glVertexArrayVertexBuffer(GlobalRenderer.triangles_vao, 0, GlobalRenderer.triangles_vbo, 0, sizeof(Vertex));
    
    glCreateBuffers(1, &GlobalRenderer.triangles_ebo);
    glNamedBufferData(GlobalRenderer.triangles_ebo, sizeof(u32) * Initial_Indices, NULL, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(GlobalRenderer.triangles_vao, GlobalRenderer.triangles_ebo);
  }
  
  glCreateVertexArrays(1, &GlobalRenderer.lines_vao);
  {
    
  }
}