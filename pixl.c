////////////////////////////////////////////////////////////////////////////////
//
//  PiXL - a tiny Lua based pixel game engine
//  written by Sebastian Steinhauer <s.steinhauer@yahoo.de>
//
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  Includes
//
////////////////////////////////////////////////////////////////////////////////
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "SDL.h"

#if _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
typedef int socklen_t;
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define closesocket(s) close(s)
#endif // _WIN32


////////////////////////////////////////////////////////////////////////////////
//
//  Defines
//
////////////////////////////////////////////////////////////////////////////////
#define PIXL_MAX_SCREEN_WIDTH   2048
#define PIXL_MAX_SCREEN_HEIGHT  2048

#define PIXL_WINDOW_PADDING     64

enum {
  PIXL_BUTTON_A = 1 << 0,
  PIXL_BUTTON_B = 1 << 1,
  PIXL_BUTTON_X = 1 << 2,
  PIXL_BUTTON_Y = 1 << 3,
  PIXL_BUTTON_UP = 1 << 4,
  PIXL_BUTTON_DOWN = 1 << 5,
  PIXL_BUTTON_LEFT = 1 << 6,
  PIXL_BUTTON_RIGHT = 1 << 7,
  PIXL_BUTTON_START = 1 << 8,
  PIXL_BUTTON_SELECT = 1 << 9
};


////////////////////////////////////////////////////////////////////////////////
//
//  Global Variables
//
////////////////////////////////////////////////////////////////////////////////
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

SOCKET udp = INVALID_SOCKET;

Uint8 screen[PIXL_MAX_SCREEN_WIDTH][PIXL_MAX_SCREEN_HEIGHT];
int screen_width = 0, screen_height = 0;
SDL_Point translation = { 0, 0 };
int clip_xl = 0, clip_yl = 0, clip_xh = 0, clip_yh = 0;

SDL_bool running = SDL_TRUE;
Uint32 seed = 0;

SDL_Point mouse = { 0, 0 };
int buttons_down = 0;
int buttons_pressed = 0;
char textinput[32];

// default color palette
// https://github.com/geoffb/dawnbringer-palettes
static SDL_Color colors[16] = {
  { 0x14, 0x0C, 0x1C, 0xFF },   // Black
  { 0x44, 0x24, 0x34, 0xFF },   // Dark Red
  { 0x30, 0x34, 0x6D, 0xFF },   // Dark Blue
  { 0x4E, 0x4A, 0x4F, 0xFF },   // Dark Gray
  { 0x85, 0x4C, 0x30, 0xFF },   // Brown
  { 0x34, 0x65, 0x24, 0xFF },   // Dark Green
  { 0xD0, 0x46, 0x48, 0xFF },   // Red
  { 0x75, 0x71, 0x61, 0xFF },   // Light Gray
  { 0x59, 0x7D, 0xCE, 0xFF },   // Light Blue
  { 0xD2, 0x7D, 0x2C, 0xFF },   // Orange
  { 0x85, 0x95, 0xA1, 0xFF },   // Blue-Gray
  { 0x6D, 0xAA, 0x2C, 0xFF },   // Light Green
  { 0xD2, 0xAA, 0x99, 0xFF },   // Peach
  { 0x6D, 0xC2, 0xCA, 0xFF },   // Cyan
  { 0xDA, 0xD4, 0x5E, 0xFF },   // Yellow
  { 0xDE, 0xEE, 0xD6, 0xFF }    // White
};

// basic ASCII font (bit encoded)
Uint8 font[128][8] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0000 (nul)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0001
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0002
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0003
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0004
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0005
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0006
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0007
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0008
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0009
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000A
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000B
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000C
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000D
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000E
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000F
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0010
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0011
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0012
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0013
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0014
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0015
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0016
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0017
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0018
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0019
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001A
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001B
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001C
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001D
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001E
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001F
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0020 (space)
  { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00 },   // U+0021 (!)
  { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0022 (")
  { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00 },   // U+0023 (#)
  { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00 },   // U+0024 ($)
  { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00 },   // U+0025 (%)
  { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00 },   // U+0026 (&)
  { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0027 (')
  { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00 },   // U+0028 (()
  { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00 },   // U+0029 ())
  { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00 },   // U+002A (*)
  { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00 },   // U+002B (+)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06 },   // U+002C (,)
  { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00 },   // U+002D (-)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00 },   // U+002E (.)
  { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00 },   // U+002F (/)
  { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00 },   // U+0030 (0)
  { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00 },   // U+0031 (1)
  { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00 },   // U+0032 (2)
  { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00 },   // U+0033 (3)
  { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00 },   // U+0034 (4)
  { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00 },   // U+0035 (5)
  { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00 },   // U+0036 (6)
  { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00 },   // U+0037 (7)
  { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00 },   // U+0038 (8)
  { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00 },   // U+0039 (9)
  { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00 },   // U+003A (:)
  { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06 },   // U+003B (//)
  { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00 },   // U+003C (<)
  { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00 },   // U+003D (=)
  { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00 },   // U+003E (>)
  { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00 },   // U+003F (?)
  { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00 },   // U+0040 (@)
  { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00 },   // U+0041 (A)
  { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00 },   // U+0042 (B)
  { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00 },   // U+0043 (C)
  { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00 },   // U+0044 (D)
  { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00 },   // U+0045 (E)
  { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00 },   // U+0046 (F)
  { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00 },   // U+0047 (G)
  { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00 },   // U+0048 (H)
  { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 },   // U+0049 (I)
  { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00 },   // U+004A (J)
  { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00 },   // U+004B (K)
  { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00 },   // U+004C (L)
  { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00 },   // U+004D (M)
  { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00 },   // U+004E (N)
  { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00 },   // U+004F (O)
  { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00 },   // U+0050 (P)
  { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00 },   // U+0051 (Q)
  { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00 },   // U+0052 (R)
  { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00 },   // U+0053 (S)
  { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 },   // U+0054 (T)
  { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00 },   // U+0055 (U)
  { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00 },   // U+0056 (V)
  { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00 },   // U+0057 (W)
  { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00 },   // U+0058 (X)
  { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00 },   // U+0059 (Y)
  { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00 },   // U+005A (Z)
  { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00 },   // U+005B ([)
  { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00 },   // U+005C (\)
  { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00 },   // U+005D (])
  { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00 },   // U+005E (^)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF },   // U+005F (_)
  { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0060 (`)
  { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00 },   // U+0061 (a)
  { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00 },   // U+0062 (b)
  { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00 },   // U+0063 (c)
  { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00 },   // U+0064 (d)
  { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00 },   // U+0065 (e)
  { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00 },   // U+0066 (f)
  { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F },   // U+0067 (g)
  { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00 },   // U+0068 (h)
  { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 },   // U+0069 (i)
  { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E },   // U+006A (j)
  { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00 },   // U+006B (k)
  { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 },   // U+006C (l)
  { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00 },   // U+006D (m)
  { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00 },   // U+006E (n)
  { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00 },   // U+006F (o)
  { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F },   // U+0070 (p)
  { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78 },   // U+0071 (q)
  { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00 },   // U+0072 (r)
  { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00 },   // U+0073 (s)
  { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00 },   // U+0074 (t)
  { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00 },   // U+0075 (u)
  { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00 },   // U+0076 (v)
  { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00 },   // U+0077 (w)
  { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00 },   // U+0078 (x)
  { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F },   // U+0079 (y)
  { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00 },   // U+007A (z)
  { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00 },   // U+007B ({)
  { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00 },   // U+007C (|)
  { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00 },   // U+007D (})
  { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+007E (~)
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }    // U+007F
};

// map string to number values (string color mapping)
static const Uint8 sprite_color_map[128] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10,
  11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};


////////////////////////////////////////////////////////////////////////////////
//
//  Helpers
//
////////////////////////////////////////////////////////////////////////////////
static void pixl_set_resolution(lua_State *L, int width, int height, double aspect) {
  SDL_DisplayMode mode;

  if (texture) SDL_DestroyTexture(texture);
  screen_width = screen_height = 0;
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL) luaL_error(L, "SDL_CreateTexture() failed: %s", SDL_GetError());
  screen_width = width; screen_height = height;
  clip_xl = 0; clip_yl = 0; clip_xh = width; clip_yh = height;

  if (aspect) height = (int)((1.0 / aspect) * (double)width);
  if (SDL_RenderSetLogicalSize(renderer, width, height)) luaL_error(L, "SDL_RenderSetLogicalSize() failed: %s", SDL_GetError());

  if (SDL_GetDesktopDisplayMode(0, &mode) == 0) {
    int factorx = (mode.w - PIXL_WINDOW_PADDING) / width;
    int factory = (mode.h - PIXL_WINDOW_PADDING) / height;
    int factor = factory > factorx ? factorx : factory;
    SDL_SetWindowSize(window, width * factor, height * factor);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  }
}

static void pixl_open_controllers(lua_State *L) {
  SDL_GameController *controller;
  int i;

  for (i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      controller = SDL_GameControllerOpen(i);
      if (controller == NULL) luaL_error(L, "SDL_GameControllerOpen() failed: %s", SDL_GetError());
    }
  }
}

static void pixl_pset(Uint8 color, int x, int y) {
  x += translation.x;
  y += translation.y;
  if ((x >= 0) && (x < screen_width) && (y >= 0) && (y < screen_height)) {
    if ((x >= clip_xl) && (x <= clip_xh) && (y >= clip_yl) && (y <= clip_yh)) {
      screen[x][y] = color;
    }
  }
}

static Uint8 pixl_pget(int x, int y) {
  if ((x >= 0) && (x < screen_width) && (y >= 0) && (y < screen_height)) {
    return screen[x][y] & 15;
  }
  return 0;
}

#define pixl_swap(T, a, b) do { T __tmp__ = a; a = b; b = __tmp__; } while(0)


////////////////////////////////////////////////////////////////////////////////
//
//  Video Settings
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_f_color(lua_State *L) {
  SDL_Color *color;
  int slot = (int)luaL_checkinteger(L, 1);
  luaL_argcheck(L, (slot >= 0) && (slot < 16), 1, "invalid color slot");
  color = &colors[slot];
  switch (lua_gettop(L)) {
    case 1:
      lua_pushinteger(L, color->r);
      lua_pushinteger(L, color->g);
      lua_pushinteger(L, color->b);
      return 3;
    case 4:
      color->r = (Uint8)luaL_checknumber(L, 2);
      color->g = (Uint8)luaL_checknumber(L, 3);
      color->b = (Uint8)luaL_checknumber(L, 4);
      return 0;
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}

static int pixl_f_resolution(lua_State *L) {
  int width, height;
  double aspect;
  switch (lua_gettop(L)) {
    case 0:
      lua_pushinteger(L, screen_width);
      lua_pushinteger(L, screen_height);
      return 2;
    case 2:
    case 3:
      width = (int)luaL_checkinteger(L, 1);
      height = (int)luaL_checkinteger(L, 2);
      aspect = (double)luaL_optnumber(L, 3, 0.0);
      luaL_argcheck(L, (width > 0) && (width < PIXL_MAX_SCREEN_HEIGHT), 1, "invalid width");
      luaL_argcheck(L, (height > 0) && (height < PIXL_MAX_SCREEN_HEIGHT), 2, "invalid height");
      pixl_set_resolution(L, width, height, aspect);
      return 0;
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}

static int pixl_f_translate(lua_State *L) {
  switch (lua_gettop(L)) {
    case 0:
      lua_pushinteger(L, translation.x);
      lua_pushinteger(L, translation.y);
      return 2;
    case 2:
      translation.x = (int)luaL_checknumber(L, 1);
      translation.y = (int)luaL_checknumber(L, 2);
      return 0;
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}

static int pixl_f_clip(lua_State *L) {
  switch (lua_gettop(L)) {
    case 0:
      lua_pushinteger(L, clip_xl);
      lua_pushinteger(L, clip_yl);
      lua_pushinteger(L, clip_xh);
      lua_pushinteger(L, clip_yh);
      return 4;
    case 4: {
      int xl = (int)luaL_checknumber(L, 1);
      int yl = (int)luaL_checknumber(L, 2);
      int xh = (int)luaL_checknumber(L, 3);
      int yh = (int)luaL_checknumber(L, 4);

      if (xl > xh) pixl_swap(int, xl, xh);
      if (yl > yh) pixl_swap(int, yl, yh);

      clip_xl = xl; clip_xh = xh;
      clip_yl = yl; clip_yh = yh;

      return 0;
    }
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}

static int pixl_f_glyph(lua_State *L) {
  int ch = (int)luaL_checkinteger(L, 1);
  luaL_argcheck(L, (ch >= 0) && (ch < 128), 1, "invalid glyph");
  switch (lua_gettop(L)) {
    case 1:
      lua_pushlstring(L, (const char*)&font[ch][0], 8);
      return 1;
    case 2: {
      int i;
      size_t length;
      const Uint8 *data = (const Uint8*)luaL_checklstring(L, 2, &length);
      luaL_argcheck(L, length == 8, 2, "invalid glyph data length");
      for (i = 0; i < 8; ++i) font[ch][i] = *data++;
      return 0;
    }
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}


////////////////////////////////////////////////////////////////////////////////
//
//  Primitive Drawing Routines
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_f_clear(lua_State *L) {
  Uint8 color = (Uint8)luaL_optinteger(L, 1, 0);
  SDL_memset(screen, color, sizeof(screen));
  return 0;
}

static int pixl_f_point(lua_State *L) {
  Uint8 color;
  int x, y;
  switch (lua_gettop(L)) {
    case 2:
      x = (int)luaL_checknumber(L, 1);
      y = (int)luaL_checknumber(L, 2);
      lua_pushinteger(L, pixl_pget(x, y));
      return 1;
    case 3:
      color = (Uint8)luaL_checkinteger(L, 1);
      x = (int)luaL_checknumber(L, 2);
      y = (int)luaL_checknumber(L, 3);
      pixl_pset(color, x, y);
      return 0;
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}

static int pixl_f_rect(lua_State *L) {
  int x, y;
  Uint8 color = (Uint8)luaL_checkinteger(L, 1);
  int x1 = (int)luaL_checknumber(L, 2);
  int y1 = (int)luaL_checknumber(L, 3);
  int x2 = (int)luaL_checknumber(L, 4);
  int y2 = (int)luaL_checknumber(L, 5);
  int fill = lua_toboolean(L, 6);

  if (x1 > x2) pixl_swap(int, x1, x2);
  if (y1 > y2) pixl_swap(int, y1, y2);

  if (fill) {
    for (x = x1; x <= x2; ++x) {
      for (y = y1; y <= y2; ++y) {
        pixl_pset(color, x, y);
      }
    }
  } else {
    for (x = x1; x <= x2; ++x) {
      pixl_pset(color, x, y1);
      pixl_pset(color, x, y2);
    }
    for (y = y1 + 1; y <= y2 - 1; ++y) {
      pixl_pset(color, x1, y);
      pixl_pset(color, x2, y);
    }
  }
  return 0;
}

static int pixl_f_circle(lua_State *L) {
  int rx, ry, dist;
  Uint8 color = (Uint8)luaL_checkinteger(L, 1);
  int x = (int)luaL_checknumber(L, 2);
  int y = (int)luaL_checknumber(L, 3);
  int radius = (int)luaL_checknumber(L, 4);
  int r0sq = lua_toboolean(L, 5) ? 0 : (radius - 1) * (radius - 1);
  int r1sq = radius * radius;

  for (rx = -radius; rx <= radius; ++rx) {
    for (ry = -radius; ry <= radius; ++ry) {
      dist = (rx * rx) + (ry * ry);
      if ((dist >= r0sq) && (dist <= r1sq)) pixl_pset(color, x + rx, y + ry);
    }
  }
  return 0;
}

static int pixl_f_line(lua_State *L) {
  Uint8 color = (Uint8)luaL_checkinteger(L, 1);
  int x1 = (int)luaL_checknumber(L, 2);
  int y1 = (int)luaL_checknumber(L, 3);
  int x2 = (int)luaL_checknumber(L, 4);
  int y2 = (int)luaL_checknumber(L, 5);
  int dx = SDL_abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
  int dy = SDL_abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, e2;
  for (;;) {
    pixl_pset(color, x1, y1);
    if (x1 == x2 && y1 == y2) break;
    e2 = err;
    if (e2 > -dx) { err -= dy; x1 += sx; }
    if (e2 < dy) { err += dx; y1 += sy; }
  }
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Advanced Drawing Routines
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_f_print(lua_State *L) {
  int px, py, mask;
  Uint8 color = (Uint8)luaL_checkinteger(L, 1);
  int x = (int)luaL_checknumber(L, 2);
  int y = (int)luaL_checknumber(L, 3);
  const char *text = luaL_checkstring(L, 4);

  for (; *text; ++text, x += 8) {
    for (py = 0; py < 8; ++py) {
      mask = font[*text & 127][py];
      for (px = 0; px < 8; ++px) {
        if (mask & (1 << px)) pixl_pset(color, x + px, y + py);
      }
    }
  }
  return 0;
}

static int pixl_f_sprite(lua_State *L) {
  int px, py;
  size_t length;
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *data = luaL_checklstring(L, 5, &length);
  Uint8 transparent = (Uint8)luaL_optinteger(L, 6, 16);
  luaL_argcheck(L, (int)length == w * h, 5, "invalid sprite data length");
  for (py = 0; py < h; ++py) {
    for (px = 0; px < w; ++px) {
      Uint8 color = sprite_color_map[(*data++) & 127];
      if (color != transparent) pixl_pset(color, x + px, y + py);
    }
  }
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Input Functions
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_check_button_mask(lua_State *L) {
  static const char *buttons[] = { "A", "B", "X", "Y", "UP", "DOWN", "LEFT", "RIGHT", "START", "SELECT", NULL };
  static const int masks[] = { PIXL_BUTTON_A, PIXL_BUTTON_B, PIXL_BUTTON_X, PIXL_BUTTON_Y, PIXL_BUTTON_UP, PIXL_BUTTON_DOWN, PIXL_BUTTON_LEFT, PIXL_BUTTON_RIGHT, PIXL_BUTTON_START, PIXL_BUTTON_SELECT };
  return masks[luaL_checkoption(L, 1, NULL, buttons)];
}

static int pixl_f_btn(lua_State *L) {
  int mask = pixl_check_button_mask(L);
  lua_pushboolean(L, buttons_down & mask);
  return 1;
}

static int pixl_f_btnp(lua_State *L) {
  int mask = pixl_check_button_mask(L);
  lua_pushboolean(L, buttons_pressed & mask);
  return 1;
}

static int pixl_f_mouse(lua_State *L) {
  lua_pushinteger(L, mouse.x - translation.x);
  lua_pushinteger(L, mouse.y - translation.y);
  return 2;
}

static int pixl_f_inkey(lua_State *L) {
  lua_pushstring(L, textinput);
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Random Number Generator
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_f_randomseed(lua_State *L) {
  switch (lua_gettop(L)) {
    case 0:
      lua_pushinteger(L, seed);
      return 1;
    case 1:
      seed = (Uint32)luaL_checkinteger(L, 1);
      return 0;
    default:
      return luaL_error(L, "wrong number of arguments");
  }
}

static int pixl_f_random(lua_State *L) {
  double r;
  lua_Integer low, up;

  // perform xorshift32
  if (seed == 0) seed = 314159265;
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  r = (double)seed / 4294967296.0;

  switch (lua_gettop(L)) {
    case 0:
      lua_pushnumber(L, (lua_Number)r);
      return 1;
    case 1:
      low = 1;
      up = luaL_checkinteger(L, 1);
      break;
    case 2:
      low = luaL_checkinteger(L, 1);
      up = luaL_checkinteger(L, 2);
      break;
    default:
      return  luaL_error(L, "wrong number of arguments");
  }
  luaL_argcheck(L, low <= up, 1, "interval is empty");
  luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1, "interval too large");
  r *= (double)(up - low) + 1.0;
  lua_pushinteger(L, (lua_Integer)r + low);
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Networking Functions
//
////////////////////////////////////////////////////////////////////////////////
static void pixl_create_udp_socket(lua_State *L) {
  if (udp == INVALID_SOCKET) {
    udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp == INVALID_SOCKET) luaL_error(L, "cannot create UDP socket");
    #if _WIN32
      DWORD nonBlocking = 1;
      if (ioctlsocket(udp, FIONBIO, &nonBlocking) != 0) luaL_error(L, "failed to set UDP socket non-blocking");
    #else
      if (fcntl(udp, F_SETFL, O_NONBLOCK, 1) == -1) luaL_error(L, "failed to set UDP socket non-blocking");
    #endif // _WIN32
  }
}

static int pixl_f_bind(lua_State *L) {
  struct sockaddr_in address;
  Uint16 port = (Uint16)luaL_optinteger(L, 1, 0);

  SDL_zero(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (port > 0) {
    if (udp != INVALID_SOCKET) {
      closesocket(udp);
      udp = INVALID_SOCKET;
    }
  }

  pixl_create_udp_socket(L);
  if (bind(udp, (const struct sockaddr*)&address, sizeof(address)) < 0) luaL_error(L, "cannot bind UDP socket");
  return 0;
}

static int pixl_f_send(lua_State *L) {
  struct sockaddr_in address;
  size_t length;
  int sent_bytes;
  Uint32 ip = (Uint32)luaL_checkinteger(L, 1);
  Uint16 port = (Uint16)luaL_checkinteger(L, 2);
  const char *data = luaL_checklstring(L, 3, &length);

  SDL_zero(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(ip);
  address.sin_port = htons(port);

  pixl_create_udp_socket(L);
  sent_bytes = sendto(udp, data, (int)length, 0, (const struct sockaddr*)&address, sizeof(address));
  if (sent_bytes != (int)length) luaL_error(L, "cannot send data");
  return 0;
}

static int pixl_f_recv(lua_State *L) {
  struct sockaddr_in address;
  socklen_t socklen;
  int received_bytes;
  char data[1024 * 32];

  pixl_create_udp_socket(L);
  socklen = sizeof(address);
  received_bytes = recvfrom(udp, data, sizeof(data), 0, (struct sockaddr*)&address, &socklen);
  if (received_bytes > 0) {
    lua_pushinteger(L, (lua_Integer)ntohl(address.sin_addr.s_addr));
    lua_pushinteger(L, (lua_Integer)ntohs(address.sin_port));
    lua_pushlstring(L, data, received_bytes);
    return 3;
  }
  return 0;
}

static int pixl_f_resolve(lua_State *L) {
  struct addrinfo hints, *result;
  const char *hostname = luaL_checkstring(L, 1);
  SDL_zero(hints);
  hints.ai_family = AF_INET;
  if (getaddrinfo(hostname, NULL, &hints, &result) == 0) {
    if (result) {
      lua_pushinteger(L, ntohl(((struct sockaddr_in*)(result->ai_addr))->sin_addr.s_addr));
      freeaddrinfo(result);
      return 1;
    }
  }
  lua_pushnil(L);
  lua_pushfstring(L, "cannot resolve " LUA_QS, hostname);
  return 2;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Miscellaneous Functions
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_f_quit(lua_State *L) {
  (void)L;
  running = SDL_FALSE;
  return 0;
}

static int pixl_f_title(lua_State *L) {
  const char *title = luaL_optstring(L, 1, NULL);
  if (title) {
    SDL_SetWindowTitle(window, title);
    return 0;
  } else {
    title = SDL_GetWindowTitle(window);
    lua_pushstring(L, title);
    return 1;
  }
}

static int pixl_f_clipboard(lua_State *L) {
  const char *text = luaL_optstring(L, 1, NULL);
  if (text) {
    (void)SDL_SetClipboardText(text);
    return 0;
  } else {
    text = SDL_GetClipboardText();
    lua_pushstring(L, text);
    return 1;
  }
}

static int pixl_f_time(lua_State *L) {
  lua_Number ticks = (lua_Number)SDL_GetTicks();
  lua_pushnumber(L, ticks / 1000.0);
  return 1;
}

static int pixl_f_cursor(lua_State *L) {
  if (lua_gettop(L) > 0) SDL_ShowCursor((int)lua_toboolean(L, 1));
  lua_pushboolean(L, SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE);
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Lua Module Definition
//
////////////////////////////////////////////////////////////////////////////////
static const luaL_Reg pixl_funcs[] = {
  { "color", pixl_f_color },
  { "resolution", pixl_f_resolution },
  { "translate", pixl_f_translate },
  { "clip", pixl_f_clip },
  { "glyph", pixl_f_glyph },

  { "clear", pixl_f_clear },
  { "point", pixl_f_point },
  { "rect", pixl_f_rect },
  { "circle", pixl_f_circle },
  { "line", pixl_f_line },

  { "print", pixl_f_print },
  { "sprite", pixl_f_sprite },

  { "btn", pixl_f_btn },
  { "btnp", pixl_f_btnp },
  { "mouse", pixl_f_mouse },
  { "inkey", pixl_f_inkey },

  { "randomseed", pixl_f_randomseed },
  { "random", pixl_f_random },

  { "bind", pixl_f_bind },
  { "send", pixl_f_send },
  { "recv", pixl_f_recv },
  { "resolve", pixl_f_resolve },

  { "quit", pixl_f_quit },
  { "title", pixl_f_title },
  { "clipboard", pixl_f_clipboard },
  { "time", pixl_f_time },
  { "cursor", pixl_f_cursor },

  { NULL, NULL }
};

static int pixl_open(lua_State *L) {
  luaL_newlib(L, pixl_funcs);

  lua_pushstring(L, "Sebastian Steinhauer <s.steinhauer@yahoo.de>");
  lua_setfield(L, -2, "_AUTHOR");

  lua_pushinteger(L, 50);
  lua_setfield(L, -2, "_VERSION");

  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Event Loop
//
////////////////////////////////////////////////////////////////////////////////
static void pixl_render_screen(lua_State *L) {
  if (SDL_SetRenderDrawColor(renderer, colors[0].r, colors[0].g, colors[0].b, 255)) luaL_error(L, "SDL_SetRenderDrawColor() failed: %s", SDL_GetError());
  if (SDL_RenderClear(renderer)) luaL_error(L, "SDL_RenderClear() failed: %s", SDL_GetError());

  if (texture != NULL) {
    SDL_Color *color;
    Uint8 *pixels, *p;
    int x, y, pitch;

    if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch)) luaL_error(L, "SDL_LockTexture() failed: %s", SDL_GetError());
    for (y = 0; y < screen_height; ++y) {
      p = pixels + (y * pitch);
      for (x = 0; x < screen_width; ++x) {
        color = &colors[screen[x][y] & 15];
        *p++ = 255; *p++ = color->b; *p++ = color->g; *p++ = color->r;
      }
    }
    SDL_UnlockTexture(texture);
    if (SDL_RenderCopy(renderer, texture, NULL, NULL)) luaL_error(L, "SDL_RenderCopy() failed: %s", SDL_GetError());
  }

  SDL_RenderPresent(renderer);
}

static void pixl_update_buttons(int mask, int down) {
  if (down) {
    buttons_down |= mask;
    buttons_pressed |= mask;
  } else {
    buttons_down &= ~mask;
  }
}

static void pixl_handle_mouse(int button, int down) {
  int mask;
  switch (button) {
    case SDL_BUTTON_LEFT: mask = PIXL_BUTTON_A; break;
    case SDL_BUTTON_RIGHT: mask = PIXL_BUTTON_B; break;
    default: return;
  }
  pixl_update_buttons(mask, down);
}

static void pixl_handle_controller(int button, int down) {
  int mask;
  switch (button) {
    case SDL_CONTROLLER_BUTTON_A: mask = PIXL_BUTTON_A; break;
    case SDL_CONTROLLER_BUTTON_B: mask = PIXL_BUTTON_B; break;
    case SDL_CONTROLLER_BUTTON_X: mask = PIXL_BUTTON_X; break;
    case SDL_CONTROLLER_BUTTON_Y: mask = PIXL_BUTTON_Y; break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: mask = PIXL_BUTTON_UP; break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: mask = PIXL_BUTTON_DOWN; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: mask = PIXL_BUTTON_LEFT; break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: mask = PIXL_BUTTON_RIGHT; break;
    case SDL_CONTROLLER_BUTTON_START: mask = PIXL_BUTTON_START; break;
    case SDL_CONTROLLER_BUTTON_GUIDE: mask = PIXL_BUTTON_SELECT; break;
    default: return;
  }
  pixl_update_buttons(mask, down);
}

static void pixl_handle_keyboard(SDL_Keycode key, int down) {
  int mask;
  switch (key) {
    case SDLK_COMMA: mask = PIXL_BUTTON_A; break;
    case SDLK_PERIOD: mask = PIXL_BUTTON_B; break;
    case SDLK_k: mask = PIXL_BUTTON_X; break;
    case SDLK_l: mask = PIXL_BUTTON_Y; break;
    case SDLK_UP: case SDLK_w: mask = PIXL_BUTTON_UP; break;
    case SDLK_DOWN: case SDLK_s: mask = PIXL_BUTTON_DOWN; break;
    case SDLK_LEFT: case SDLK_a: mask = PIXL_BUTTON_LEFT; break;
    case SDLK_RIGHT: case SDLK_d: mask = PIXL_BUTTON_RIGHT; break;
    case SDLK_RETURN: mask = PIXL_BUTTON_START; break;
    case SDLK_SPACE: mask = PIXL_BUTTON_SELECT; break;
    default: return;
  }
  pixl_update_buttons(mask, down);
}

static void pixl_handle_SDL_event(lua_State *L, const SDL_Event *ev) {
  switch (ev->type) {
    case SDL_QUIT:
      running = SDL_FALSE;
      break;
    case SDL_MOUSEMOTION:
      mouse.x = ev->motion.x;
      mouse.y = ev->motion.y;
      break;
    case SDL_MOUSEBUTTONDOWN:
      pixl_handle_mouse(ev->button.button, SDL_TRUE);
      break;
    case SDL_MOUSEBUTTONUP:
      pixl_handle_mouse(ev->button.button, SDL_FALSE);
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      pixl_handle_controller(ev->cbutton.button, SDL_TRUE);
      break;
    case SDL_CONTROLLERBUTTONUP:
      pixl_handle_controller(ev->cbutton.button, SDL_FALSE);
      break;
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
      pixl_open_controllers(L);
      break;
    case SDL_KEYDOWN:
      pixl_handle_keyboard(ev->key.keysym.sym, SDL_TRUE);
      break;
    case SDL_KEYUP:
      pixl_handle_keyboard(ev->key.keysym.sym, SDL_FALSE);
      break;
    case SDL_TEXTINPUT:
      SDL_strlcpy(textinput, ev->text.text, sizeof(textinput));
      break;
  }
}

static void pixl_run_event_loop(lua_State *L) {
  Uint32 last_tick, current_tick, delta_ticks;
  SDL_Event ev;

  last_tick = SDL_GetTicks();
  while (running) {
    buttons_pressed = 0;
    textinput[0] = 0;
    while (SDL_PollEvent(&ev)) pixl_handle_SDL_event(L, &ev);

    current_tick = SDL_GetTicks();
    delta_ticks = current_tick - last_tick;
    last_tick = current_tick;

    if (lua_getglobal(L, "update") == LUA_TFUNCTION) {
      lua_pushnumber(L, (lua_Number)delta_ticks / 1000.0);
      lua_call(L, 1, 0);
    } else {
      lua_pop(L, 1);
    }

    pixl_render_screen(L);
  }
}


////////////////////////////////////////////////////////////////////////////////
//
//  Init & Shutdown routines
//
////////////////////////////////////////////////////////////////////////////////
static int pixl_init(lua_State *L) {
  #if _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR) luaL_error(L, "WSAStartup() failed!");
  #endif // _WIN32

  if (SDL_Init(SDL_INIT_EVERYTHING)) luaL_error(L, "SDL_Init() failed: %s", SDL_GetError());
  window = SDL_CreateWindow("PiXL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 240, SDL_WINDOW_RESIZABLE);
  if (window == NULL) luaL_error(L, "SDL_CreateWindow() failed: %s", SDL_GetError());
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) luaL_error(L, "SDL_CreateRenderer() failed: %s", SDL_GetError());
  SDL_StartTextInput();
  pixl_set_resolution(L, 256, 240, 0.0);
  pixl_open_controllers(L);

  if (luaL_loadfile(L, "game.lua") != LUA_OK) lua_error(L);
  lua_call(L, 0, 0);

  pixl_run_event_loop(L);
  return 0;
}

static void pixl_shutdown() {
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();

  if (udp != INVALID_SOCKET) closesocket(udp);
  #if _WIN32
    WSACleanup();
  #endif // _WIN32
}

static void pixl_register_arg(lua_State *L, int argc, char **argv) {
  int i;
  lua_newtable(L);
  for (i = 0; i < argc; ++i) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
  }
  lua_setglobal(L, "arg");
}

int main(int argc, char **argv) {
  lua_State *L = luaL_newstate();
  pixl_register_arg(L, argc, argv);
  luaL_openlibs(L);
  luaL_requiref(L, "pixl", pixl_open, 0);
  lua_getglobal(L, "debug");
  lua_getfield(L, -1, "traceback");
  lua_remove(L, -2);
  lua_pushcfunction(L, pixl_init);
  if (lua_pcall(L, 0, 0, -2) != LUA_OK) {
    const char *msg = luaL_gsub(L, lua_tostring(L, -1), "\t", "  ");
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "PiXL Panic", msg, window);
  }
  lua_close(L);
  pixl_shutdown();
  return 0;
}
