#pragma once

#ifdef _DEBUG
#define SHADER_PATH(x) "../shaders/" ## x
#define ASSET_PATH(x) "../assets/" ## x
#define CONFIG_PATH(x) "../config/" ## x
#define RENDER_PATH(x) "../render/" ## x
#else
#define SHADER_PATH(x) "../shaders/" ## x
#define ASSET_PATH(x) "../assets/" ## x
#define CONFIG_PATH(x) "../config/" ## x
#define RENDER_PATH(x) "../render/" ## x
#endif