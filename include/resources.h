#pragma once

#ifdef _DEBUG
#define SHADER_PATH(x) "../shaders/" ## x
#define ASSET_PATH(x) "../assets" ## x
#else
#define SHADER_PATH(x) "../shaders/" ## x
#define ASSET_PATH(x) "../assets" ## x
#endif