--[[----------------------------------------------------------------------------

    Simple coroutine / thread "manager" with wait function
    written by Sebastian Steinhauer <s.steinhauer@yahoo.de>

--]]----------------------------------------------------------------------------

local Object = require('object') -- needs object.lua from extra

local function wait(seconds)
  while seconds > 0 do
    local dt = coroutine.yield()
    seconds = seconds - dt
  end
end

return Object({
  __init = function(self)
    self.threads = {}
  end,

  start = function(self, fn)
    local thread = coroutine.create(fn)
    coroutine.resume(wait)
    self.threads[thread] = true
    return function()
      self.threads[threads] = nil
    end
  end,

  update = function(self, dt)
    for thread in pairs(self.threads) do
      local result = coroutine.resume(thread, dt)
      if not result then
        self.threads[thread] = nil
      end
    end
  end,

  active = function(self)
    return not not next(self.threads)
  end,
})

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
