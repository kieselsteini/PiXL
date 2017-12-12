--------------------------------------------------------------------------------
--  PiXL boot code
--------------------------------------------------------------------------------
local pixl = require('pixl')

--------------------------------------------------------------------------------
--  DOOM WAD archive
--------------------------------------------------------------------------------
local wad_lumps = {}

function pixl.openwad(filename)
  local fp, err = io.open(filename, 'rb')
  if not fp then
    return nil, err
  end
  local magic, numlumps, tableofs = string.unpack('< c4 i4 i4', fp:read(12))
  assert(fp:seek('set', tableofs))
  for i = 1, numlumps do
    local offset, length, name = string.unpack('< i4 i4 c8', fp:read(16))
    name = string.match(name, '[^\0]+')
    wad_lumps[name] = { fp = fp, offset = offset, length = length }
  end
  return true
end

function pixl.loadfile(filename)
  local fp = io.open(filename, 'rb')
  local length
  if fp then
    length = 'a'
  else
    local name = string.upper(string.match(filename, '[^%.]+'))
    local lump = wad_lumps[name]
    if lump then
      fp = lump.fp
      length = lump.length
      assert(fp:seek('set', lump.offset))
    else
      return nil, string.format("file '%s' not found", filename)
    end
  end
  return fp:read(length)
end


--------------------------------------------------------------------------------
--  Module Loader
--------------------------------------------------------------------------------
table.insert(package.searchers, 2, function(modulename)
  local code = pixl.loadfile(modulename .. '.lua')
  if code then
    return load(code)
  else
    return string.format("\n\tno virtual module '%s'", modulename)
  end
end)


--------------------------------------------------------------------------------
--  load the game
--------------------------------------------------------------------------------
for i = 1, 9 do
  pixl.openwad(string.format('game%d.wad', i))
end
assert(load(assert(pixl.loadfile('game.lua')), 'game.lua'))()
