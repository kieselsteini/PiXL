--[[----------------------------------------------------------------------------

    Famous 16-color palettes for PiXL
    written by Sebastian Steinhauer <s.steinhauer@yahoo.de>

--]]----------------------------------------------------------------------------

local pixl = require('pixl')

local function setcolor(slot, color)
  local r, g, b = string.match(color, '#?(%x%x)(%x%x)(%x%x)')
  pixl.color(slot, tonumber(r, 16), tonumber(g, 16), tonumber(b, 16))
end

local function setpalette(palette)
  -- reset entire palette
  for i = 0, 255 do
    pixl.color(i, 0, 0, 0)
  end
  -- set new palette
  for i, color in pairs(palette) do
    setcolor(i - 1, color)
  end
end

return {
  DawnBringer16 = function()
    setpalette({
      '#140C1C',
      '#442434',
      '#30346D',
      '#4E4A4F',
      '#854C30',
      '#346524',
      '#D04648',
      '#757161',
      '#597DCE',
      '#D27D2C',
      '#8595A1',
      '#6DAA2C',
      '#D2AA99',
      '#6DC2CA',
      '#DAD45E',
      '#DEEED6',
    })
  end,

  pico8 = function()
    setpalette({
      '#000000',
      '#1D2B53',
      '#7E2553',
      '#008751',
      '#AB5236',
      '#5F574F',
      '#C2C3C7',
      '#FFF1E8',
      '#FF004D',
      '#FFA300',
      '#FFEC27',
      '#00E436',
      '#29ADFF',
      '#83769C',
      '#FF77A8',
      '#FFCCAA',
    })
  end,

  Commodore64 = function()
    setpalette({
      '#000000',
      '#ffffff',
      '#880000',
      '#aaffee',
      '#cc44cc',
      '#00cc55',
      '#0000aa',
      '#eeee77',
      '#dd8855',
      '#664400',
      '#ff7777',
      '#333333',
      '#777777',
      '#aaff66',
      '#0088ff',
      '#bbbbbb',
    })
  end,

  IBM_EGA = function()
    setpalette({
      '#000000',
      '#0000AA',
      '#00AA00',
      '#00AAAA',
      '#AA0000',
      '#AA00AA',
      '#AA5500',
      '#AAAAAA',
      '#555555',
      '#5555FF',
      '#55FF55',
      '#55FFFF',
      '#FF5555',
      '#FF55FF',
      '#FFFF55',
      '#FFFFFF',
    })
  end,

  ZXSpectrum = function()
    setpalette({
      '#000000',
      '#0000D7',
      '#D70000',
      '#D700D7',
      '#00D700',
      '#00D7D7',
      '#D7D700',
      '#D7D7D7',
      '#000000',
      '#0000FF',
      '#FF0000',
      '#FF00FF',
      '#00FF00',
      '#00FFFF',
      '#FFFF00',
      '#FFFFFF',
    })
  end,

  IBM_CGA_Low1 = function()
    setpalette({
      '#000000',
      '#00AAAA',
      '#AA00AA',
      '#AAAAAA',
    })
  end,

  IBM_CGA_High1 = function()
    setpalette({
      '#000000',
      '#55FFFF',
      '#FF55FF',
      '#FFFFFF',
    })
  end,

  IBM_CGA_Low2 = function()
    setpalette({
      '#000000',
      '#00AA00',
      '#AA0000',
      '#AA5500',
    })
  end,

  IBM_CGA_High2 = function()
    setpalette({
      '#000000',
      '#55FF55',
      '#FF5555',
      '#FFFF55',
    })
  end,

  GameBoy = function()
    setpalette({
      '#0f380f',
      '#306230',
      '#8bac0f',
      '#9bbc0f',
    })
  end,
}

--[[----------------------------------------------------------------------------

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

--]]----------------------------------------------------------------------------
