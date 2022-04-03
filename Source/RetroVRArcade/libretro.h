#pragma once

#include <limits.h>

#define RETRO_DEVICE_KEYBOARD     3

#define RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY 9
#define RETRO_ENVIRONMENT_SET_VARIABLES 16
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 27
#define RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY 30 /* Old name, kept for compatibility. */
#define RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY 31

struct retro_game_info
{
	const char* path;       // Path to game, UTF-8 encoded. Usually used as a reference.
							// May be NULL if rom was loaded from stdin or similar.
							// retro_system_info::need_fullpath guaranteed that this path is valid.
	const void* data;       // Memory buffer of loaded game. Will be NULL if need_fullpath was set.
	size_t      size;       // Size of memory buffer.
	const char* meta;       // String of implementation specific meta-data.
};

struct retro_system_info
{
	const char* library_name;
	const char* library_version;
	const char* valid_extensions;
	bool need_fullpath;
	bool block_extract;
};

#ifndef RETRO_CALLCONV
#  if defined(__GNUC__) && defined(__i386__) && !defined(__x86_64__)
#    define RETRO_CALLCONV __attribute__((cdecl))
#  elif defined(_MSC_VER) && defined(_M_X86) && !defined(_M_X64)
#    define RETRO_CALLCONV __cdecl
#  else
#    define RETRO_CALLCONV /* all other platforms only have one calling convention each */
#  endif
#endif

typedef void (RETRO_CALLCONV* retro_log_printf_t)(enum retro_log_level level,
	const char* fmt, ...);

struct retro_log_callback
{
	retro_log_printf_t log;
};

enum retro_log_level
{
	RETRO_LOG_DEBUG = 0,
	RETRO_LOG_INFO,
	RETRO_LOG_WARN,
	RETRO_LOG_ERROR,
	RETRO_LOG_DUMMY = INT_MAX
};

/// <summary>
/// See Retroarch's libretro.h for a complete list of key codes
/// </summary>
enum retro_key {

	// player 1
	RETROK_5 = 53, // player 1 insert coin
	RETROK_UP = 273,
	RETROK_DOWN = 274,
	RETROK_LEFT = 276,
	RETROK_RIGHT = 275,
	RETROK_LCTRL = 306, // P1B1
	RETROK_LALT = 308, // P1B2
	RETROK_SPACE = 32, // P1B3

	// player 2
	RETROK_6 = 54, // player 2 insert coin
	RETROK_r = 114,
	RETROK_f = 102,
	RETROK_d = 100,
	RETROK_g = 103,
	RETROK_a = 97, // P2B1
	RETROK_s = 115, // P2B2
	RETROK_q = 113, // P2B3

	// player 3

	RETROK_i = 105,
	RETROK_k = 107,
	RETROK_j = 106,
	RETROK_l = 108,
	RETROK_RCTRL = 305,
	RETROK_RSHIFT = 303,
	RETROK_RETURN = 13,

	// player 4

	RETROK_KP8 = 264,
	RETROK_KP2 = 258,
	RETROK_KP4 = 260,
	RETROK_KP6 = 262,
	RETROK_KP0 = 256,
	RETROK_KP_PERIOD = 266,
	RETROK_KP_ENTER = 271,

	// misc

	RETROK_TAB = 9,
	RETROK_1 = 49, // player 1 start (in general)
	RETROK_2 = 50,
	RETROK_3 = 51,
	RETROK_4 = 52,
};

bool retro_set_environment(unsigned cmd, void* data);
void video_driver_frame(const void* data, unsigned width, unsigned height, size_t pitch);

typedef bool (*retro_environment_t)(unsigned cmd, void* data);
typedef void (*retro_video_refresh_t)(const void* data, unsigned width, unsigned height, size_t pitch);
typedef void (*retro_audio_sample_t)(int16_t left, int16_t right);
typedef size_t(*retro_audio_sample_batch_t)(const int16_t* data, size_t frames);
typedef void (*retro_input_poll_t)(void);
typedef int16_t(*retro_input_state_t)(unsigned port, unsigned device, unsigned index, unsigned id);

typedef int(*_retro_api_version)(void); // Declare a method to store the DLL method getInvertedBool.
typedef void(*_retro_init)(void); // Declare a method to store the DLL method getIntPlusPlus.
typedef void(*_retro_deinit)(void);
typedef bool(*_retro_load_game)(struct retro_game_info* gameinfo);
typedef void(*_retro_unload_game)(void);
typedef void(*_retro_set_environment)(retro_environment_t);
typedef void(*_retro_set_video_refresh)(retro_video_refresh_t);
typedef void(*_retro_set_audio_sample)(retro_audio_sample_t);
typedef void(*_retro_set_audio_sample_batch)(retro_audio_sample_batch_t);
typedef void(*_retro_set_input_poll)(retro_input_poll_t);
typedef void(*_retro_set_input_state)(retro_input_state_t);
typedef void(*_retro_get_system_info)(struct retro_system_info* info);
typedef void (*_retro_run)();
