# PiXL - Extras
This directory contains additional Lua modules you might find useful when developing a game. If not stated otherwise those modules are public domain.

## palette.lua
Provides additional color palettes.
```lua
local palette = require('palette')
palette.Commodore64()
```

### palette.DawnBringer16()
The default palette of PiXL. See https://github.com/geoffb/dawnbringer-palettes

### palette.pico8()
Set the PICO-8 palette. PICO8 is a fantasy 8-bit console using Lua for scripting. See https://www.lexaloffle.com/pico-8.php

### palette.Commodore64()
Set the 16 colors to the palette of the famous Commodore 64.

### palette.IBM_EGA()
The old IBM PC EGA 16 color palette. If you want to create old MS-DOS games.

### palette.ZXSpectrum()
The 16 colors of Britains famous Sinclair ZX Spectrum home computer.

### palette.IBM_CGA_Low1()
### palette.IBM_CGA_High1()
### palette.IBM_CGA_Low2()
### palette.IBM_CGA_High2()
The 4 color palette modes that IBM CGA graphics mode could provide. For really old MS-DOS games :smile:.

### palette.GameBoy()
Sets the first 4 colors to the greenish 4 shades of the original Nintendo GameBoy. Other colors will be set to black.

## params.lua
A very simple commandline argument parser. PiXL sets like ```lua``` all arguments to the global variable ```arg```.

```lua
local params = require('params')
```

### params.has(argument)
Checks if *argument* is present in the command line arguments.
```lua
if params.has('-debug') then
  -- enable debug mode
end
```

### params.arg(argument)
Returns the command line argument after *argument*. Useful if you want to check args like ```-file additional.lua```.

```lua
local file = params.arg('-file')
if file then
  -- load additional file
  loadfile(file)
end
```

## object.lua
A very minimalistic Object for prototype based programming.

```lua
local Object = require('object')

-- create new Player object
local Player = Object({
  x = 0,
  y = 0,

  move = function(self, dx, dy)
    self.x = self.x + dx
    self.y = self.y + dy
  end,
})

-- create Player instances
local Player1 = Player()
local Player2 = Player()

Player1:move(-10, 5)
Player2:move(20, -7)

print(Player1.x, Player1.y) --> -10  5
print(Player2.x, Player2.y) -->  20 -7

-- create an object with "constructor" method
local List = Object({
  __init = function(self)
    -- this init function will called when a new object is created
    self.list = {}
  end,
})
```

## threads.lua
A simple module to program with coroutines and waits. You can use the functions directly from the module or create a separate instance.
```lua
local threads = require('threads')

local my_threads = threads()
print(my_threads.active())
```

### thread.start(fn)
Starts a new thread. The given function will be called with *wait* which is function to wait a certain amount of seconds.
It returns a function to "stop" the coroutine (it will not be resumed anymore).
```lua
local stop_it = threads.start(function(wait)
  -- do some stuff
  wait(5.0) -- wait for 5s
  -- do some other stuff
  wait(0.1) -- wait for 0.1s
end)

-- now let's stop the thread
stop_it()
```

### thread.update(dt)
This should be called regularely with the *dt* counting the time in seconds since the last *thread.update()* call.
You typically can call this inside the global *update(dt)* function which will be called by PiXL.
```lua
function update(dt)
  -- we update also our timed threads
  threads.update(dt)
end
```

### threads.active()
Returns *true* if there are any running threads.
```lua
if threads.active() then
  -- still some threads running
end
```
