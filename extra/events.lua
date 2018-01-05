--[[----------------------------------------------------------------------------

    Simple event dispatcher for objects
    written by Sebastian Steinhauer <s.steinhauer@yahoo.de>

--]]----------------------------------------------------------------------------

local Object = require('object') -- needs object.lua from extra

return Object({
  __init = function(self)
    self.listeners = {}
  end,

  listen = function(self, message, object, fn)
    fn = fn or object[message]
    assert(type(fn) == 'function')
    local listeners = self.listeners[message] or setmetatable({}, { __mode = 'kv' })
    listeners[object] = fn
    self.listeners[message] = listeners
  end,

  send = function(self, message, ...)
    local listeners = self.listeners[message]
    if listeners then
      for object, fn in pairs(listeners) do
        fn(object, ...)
      end
    end
  end,

  ignore = function(self, message, object)
    local listeners = self.listeners[message]
    if listeners then
      listeners[object] = nil
      if not next(listeners) then
        self.listeners[message] = nil
      end
    end
  end,

  ignore_all = function(self, object)
    for message, listeners in pairs(self.listeners) do
      self:ignore(message, object)
    end
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
