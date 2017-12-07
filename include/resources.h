#pragma once

#ifdef _DEBUG
#define SHADER_PATH(x) "../shaders/" ## x
#else
#define SHADER_PATH(x) "../shaders/" ## x
#endif