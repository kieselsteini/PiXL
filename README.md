# PiXL - a tiny Lua pixel game engine
> “Make things as simple as possible, but not simpler.” - *Albert Einstein*

PiXL is a tiny pixel game engine that's great for retro games or game-jams.

[![Build Status](https://travis-ci.org/kieselsteini/PiXL.svg?branch=master)](https://travis-ci.org/kieselsteini/PiXL)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/kieselsteini/PiXL)](https://ci.appveyor.com/project/kieselsteini/pixl)

## Features
- newest Lua 5.3 with all features
- pure software rendered screen
- 16 color palette
- variable resolution (up to 2048x2048)
- simple and advanced drawing functions
- customizeable font glyphs
- sprite/images are simple Lua strings
- controller support
- WAD file archive loading

## Non-Features
- image loading / saving ... sprites are Lua strings!
- advanced modern OpenGL/Direct3D rendering

## Planned Features
- chiptune sound engine with different waveform generators

## Goals
- **a "ready to go" package**: just ship PiXL.exe / SDL2.dll / LICENSES.txt and your game.lua
- **portability** - written in simple C using Lua 5.3 and SDL2 as the only dependencies
- **simple API** - a handful of very powerful commands to customize nearly every aspect of PiXL

## License
I believe in simple and free licensing terms. So this is free and unencumbered software released into the *public domain*. Please note that Lua 5.3 is *MIT* and SDL2 *zlib* licensed.
```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
```

# Documentation

## Callbacks

### update(dt)
PiXL runs its own event-loop to process input and screen rendering to host operating system. To update the game logic and rendering the game screen ```update(dt)``` will be called regularely with *dt* giving the delta time in seconds since the last call of ```udpate()```.

This means that PiXL runs theoretically with an uncapped framerate. The internal renderer respects *vsync* so on most modern PC this will result in a *60.0 FPS*  framerate.

```lua
function update(dt)
  pixl.clear()
  pixl.print(15, 0, 0, string.format('dt: %f - FPS: %f', dt, 1.0 / fps))
end
```

## Video Settings

### pixl.color(slot[r, g, b])
PiXL provides a 16 color palette. The default palette is the famous Dawnbringer 16 color palette (https://github.com/geoffb/dawnbringer-palettes). In the *extra* directory is the *palette.lua* which provides different also very common 16 color palettes ready to use for you.

```lua
pixl.color(0, 0x00, 0x00, 0x00) -- set color 0 to RGB #000000
pixl.color(1, 0x00, 0x00, 0xAA) -- set color 1 to RGB #0000AA

local r, g, b = pixl.color(1) -- the RGB values for color 1
```

### pixl.resolution([width, height])
> **HINT:** Setting a new resolution is slow. You should not use this function regularely (e.g. in the ```update()``` call.

> **HINT:** Setting a new resolution will reset the clipping area to the whole screen.

```lua
pixl.resolution(256, 240) -- set resolution to 256x240 (NES screen size)

local width, height = pixl.resolution() -- get current resolution (this is a quick call)
```

### pixl.translate([x, y])
```lua
pixl.translate(-10, -10) -- set translation to -10,-10
pixl.point(1, 10, 10) -- this pixel will now appear on 0,0

local x, y = pixl.translate() -- get current translation
```

### pixl.clip([x1, y1, x2, y2])
Restrict drawing inside the area of *x1*, *y1* to *x2*, *y2*.
```lua
pixl.clip(10, 10, 50, 50) -- restrict drawing to the area of 10,10-50,50

local w, h = pixl.resolution()
pixl.clip(0, 0, w - 1, h - 1) -- reset clipping to the whole screen

local x1, y1, x2, y2 = pixl.clip() -- get current clipping area
```

### pixl.glyph(ch[, bitdata])
The font used by ```pixl.print()``` is composed out of 8x8 pixel glyphs. A glyph is represented by a 8 byte string where every byte is one row of the glyph. Every bit set in a row byte will result in a pixel on the screen.
```lua
pixl.glyph(string.byte('A'), '\x0C\x1E\x33\x33\x3F\x33\x33\x00') -- set for the 'A' a new bitpattern

local bitpattern = pixl.glyph(string.byte('@')) -- get bitpattern for '@'
```

## Primitive Drawing Routines

### pixl.clear([color])
> **HINT:** ```pixl.clear()``` ignores translation and clipping!
```lua
pixl.clear() -- clear screen to color 0 (this is default)
pixl.clear(5) -- clear screen to color 5
```

### pixl.point([color, ] x, y)
> **HINT:** Getting a pixel from screen ignores translation and clipping.
```lua
pixl.point(10, 100, 100) -- set the pixel at 100,100 to color 10

local color = pixl.point(50, 50) -- get the current pixel color at 50,50
```

### pixl.rect(color, x1, y1, x2, y2[, fill])
```lua
pixl.rect(1, 10, 10, 20, 20) -- draw a single pixel rectangle from 10,10 to 20,20
pixl.rect(2, 20, 20, 30, 30, true) -- draw a filled rectangle from 20,20 to 30,30
```

### pixl.circle(color, x, y, radius[, fill])
```lua
pixl.circle(1, 10, 10, 5) -- draw a single pixel circle at 10,10 with radius 5
pixl.circle(2, 20, 20, 5, true) -- draw a filled circle at 20,20
```

## Advanced Drawing Routines

### pixl.print(color, x, y, text)
Prints the *text* at *x*, *y* with a 8x8 pixel font. See ```pixl.glyph()``` how to modify the font.
```lua
pixl.print(1, 0, 0, 'Hello World!')
pixl.print(2, 0, 8, string.format('Score: %d', score))
```

### pixl.sprite(x, y, width, height, data[, transparent_color])
Images and sprites are represented as simple Lua strings containing hexadecimal values for every color. This way you can simply include all sprites directly into the source code and modify them (e.g. recoloring) with the default Lua string functionalities. It doesn't matter if you write lowecase or uppercase hex letters.

```lua
local x, y = pixl.mouse()
pixl.sprite(x, y, 4, 4, '0123456789ABCDEF') -- draw a 4x4 image containing the whole color palette
```

## Input Functions
PiXL assumes a XBox360 controller and automatically maps every connected controller to this button layout. Following buttons are recognized:

- **A**
  - controller: A
  - mouse: left click
  - keyboard: ","
- **B**
  - controller: B
  - mouse: right click
  - keyboard: "."
- **X**
  - controller: X
  - keyboard: "k"
- **Y**
  - controller: Y
  - keyboard: "l"
- **UP**
  - controller: dpad up
  - keyboard: "w", arrow up
- **DOWN**
  - controller: dpad down
  - keyboard: "s", arrow down
- **LEFT**
  - controller: dpad left
  - keyboard: "a", arrow left
- **RIGHT**
  - controller: dpad right
  - keyboard: "d", arrow right
- **START**
  - controller: start
  - keyboard: return
- **SELECT**
  - controller: home
  - keyboard: space

### pixl.btn(button)
```lua
local down_A = pixl.btn('A') -- returns true if button 'A' is currently down
```

### pixl.btnp(button)
```lua
local pressed_A = pixl.btnp('A') -- returns true if button 'A' was pressed once since last frame
```

### pixl.mouse()
> **HINT:** The returned mouse position is translated as well. See ```pixl.translate()```
```lua
local x, y = pixl.mouse() -- returns the position of the mouse cursor
```

### pixl.inkey()
```lua
local utf8 = pixl.inkey() -- returns an UTF-8 string of the last pressed keyboard key
```

## Random Number Generator
These random number generator functions behave like the ```math.randomseed()``` and ```math.random()``` functions of Lua itself. In contrast to the default number generator this behaves identical on every platform. This can be useful if you need to reproduce the same random events across different systems (e.g. replay functionality or networking).

PiXL uses the fast and relatively "random" pseudo random number generator *xorshift* (https://en.wikipedia.org/wiki/Xorshift).

### pixl.randomseed([seed])
```lua
pixl.randomseed(47) -- set a new random seed
pixl.randomseed(os.time()) -- set a more "random" seed :)

local seed = pixl.randomseed() -- get current seed
```

### pixl.random([low[, up]])
```lua
local r = pixl.random() -- get a random value between [0,1)

local r = pixl.random(6) -- get a random value between [1,6]

local r = pixl.random(10, 20) -- get random value between [10,20]
```

## Network Support
PiXL provides a simple networking API for sending and receiving UDP packages. Especially for games UDP has many advantages (and a *big* disadvantage as well...see the hint), such as small footprint, fast delivery, connectionless and an idea of packets (in contrast to TCP which is a stream).

Please note that networking is a complex task and even harder to debug. It is entirely out of scope of this small manual to teach you how to use UDP properly.

The UDP socket created by PiXL will be set to no-blocking to simplify the ```recv()``` implementation. But also ```send()``` profits from this design decision as the send calls will return immediately.

> **HINT:** UDP is an unreliable protocol, so packets may arrive out of order or even not at all. You have to provide some mechanism to ensure that everything is fine.

### pixl.bind([port])
Bind the UDP socket to a specific port. This is useful for the server part. If you don't specify a port, a random free port will be used.

If the socket was already bound, it will be closed and a new socket is created.
```lua
pixl.bind(12345) -- bind to the port 12345, this is useful for servers

pixl.bind() -- unbind and use random free port, useful for clients
```

### pixl.send(address, port, data)
```lua
local addr = pixl.resolve('remote-host.org')
pixl.send(addr, 12345, 'This is my message!')
```

### pixl.recv()
```lua
local addr, port, data = pixl.recv() -- this functions returns nil if no packet is available
```

### pixl.resolve(hostname)
Resolves the given *hostname* to an IPv4 address which will be returned as a number. If the *hostname* could not be resolved, it returns *nil* and a error message.
> **HINT:** This function could take some to time to complete as it might require to resolve the hostname with external DNS.
```lua
local addr = pixl.resolve('my-host.org') -- returns the IPv4 address of the given hostname

local addr = assert(pixl.resolve('127.0.0.1'))
```

## Miscellaneous Functions

### pixl.quit()
This function will eventually quit the running game.
```lua
pixl.quit() -- end the event-loop
```

### pixl.title([title])
```lua
pixl.title('My awesome game!') -- set the window title

local title = pixl.title() -- get current window title
```

### pixl.clipboard([text])
You can modify the clipboard using this functions. On Windows systems this function resembles ```Ctrl+C``` and ```Ctrl+V``` shortcuts.
```lua
pixl.clipboard('This is now in the clipboard') -- set clipboard content

local text = pixl.clipboard() -- get text from clipboard (nil if empty)
```

### pixl.time()
```lua
local time = pixl.time() -- return the passed time sice start in seconds
```

## WAD Archive Support

### pixl.openwad(filename)
Opens a DOOM WAD archive and adds the lumps to the virtual file system.
```lua
assert(pixl.openwad('mygame.wad'))
```

### pixl.loadfile(filename)
Loads the file with *filename*. PiXL tries to load the file in the current directory and then it tries to find the *filename* in the loaded WAD archives.
```lua
local data = pixl.loadfile('assets.lua') -- load the assets.lua file from current directory or WAD
```
