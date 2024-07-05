/* date = July 5th 2024 11:21 am */

#ifndef MAIN_H
#define MAIN_H

#define APP_NAME "namefull"
#define ENABLE_HOTLOAD_VARIABLES 1
#define ENABLE_ASSERT 1

//~ Opengl(Glad) and GLFW Includes
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

//~ FLayer
#include "f_includes.h"

//~ Third-party headers
#include "external/stb_truetype.h"
#include "external/stb_image.h"

//~ *.h
#include "input.h"
#include "camera.h"

//~ Third-party source
#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

//~ *.c
#include "input.c"
#include "camera.c"

#endif //MAIN_H
