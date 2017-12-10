# PiXL - Extras
This directory contains additional Lua modules you might find useful when developing a game. If not stated otherwise those modules are public domain.

## palette.lua
Provides additional color palettes.
```lua
local palette = require('palette')
palette.Commodore64()
```

### palette.dawnbringer16()
The default palette of PiXL. See https://github.com/geoffb/dawnbringer-palettes

### palette.pico8()
Set the PICO-8 palette. PICO8 is a fantasy 8-bit console using Lua for scripting. See https://www.lexaloffle.com/pico-8.php

### palette.Commodore64()
Set the 16 colors to the palette of the famous Commodore 64.

### palette.IBM_CGA()
The old IBM PC Color Graphics Adapter palette. If you want to create old MS-DOS games.

### palette.ZXSpectrum()
The 16 colors of Britains famous Sinclair ZX Spectrum home computer.

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
```
