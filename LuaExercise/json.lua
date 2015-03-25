
local UTF8Bytes
local UTF8ToUnicode
local UnicodeToJson

local Unmarshal
local UnmarshalString
local UnmarshalNumber
local UnmarshalTable
local UnmarshalDict
local UnmarshalOther
local IsArray
local IsEmpty

local Marshal
local MarshalString
local MarshalNumber
local MarshalArray
local MarshalDict
local MarshalOther


local function tbl2str(t)
  local res = ''
  for k, v in pairs(t) do
    local vstr
    if type(v) == 'table' then
      vstr = tbl2str(v)
    elseif type(v) == 'string' then
      vstr = '"' .. v .. '"'
    else
      vstr = tostring(v)
    end
    res = res ..'"'..k..'"'..':'..vstr ..','
  end
  return '{' .. res:sub(1,#res-1) .. '}'
end

-----------------------------------------------------------------
-- Unicode Help Function
function UTF8Bytes(s)
  local c = string.byte(s:sub(1,1))
  if c < 0xc0 then
    return 1
  elseif c < 0xe0 then
    return 2
  elseif c < 0xf0 then
    return 3
  elseif c < 0xf8 then
    return 4
  elseif c < 0xfc then
    return 5
  else
    return 6
  end
end

function UTF8ToUnicode(s)
  local c = string.byte(s:sub(1,1))
  local nbytes
  if c < 0xc0 then
    nbytes = 1
  elseif c < 0xe0 then
    c = c - 0xc0
    nbytes = 2
  elseif c < 0xf0 then
    c = c - 0xe0
    nbytes = 3
  elseif c < 0xf8 then
    c = c - 0xf0
    nbytes = 4
  elseif c < 0xfc then
    c = c - 0xf8
    nbytes = 5
  else
    c = c - 0xfc
    nbytes = 6
  end
  
  local unicode = 0
  
  for i = nbytes, 2, -1 do
    unicode = unicode + (string.byte(s:sub(i,i)) - 0x80)*(2^(6*(nbytes-i)))
  end
  
  unicode = unicode + c*(2^(6*(nbytes-1)))

  return unicode
end

function UnicodeToJson(unicode)
  if unicode < 0x0080 then
    return string.char(unicode)
  elseif unicode < 0xd800 or (unicode > 0xdfff and unicode <= 0xffff) then
    return [[\u]] .. string.format('%04x', unicode)
  else -- it must be transformed in UTF-16 form, fuck!
    unicode = unicode - 0x10000
    local w1 = 0xd800 + math.floor(unicode / (2^10))
    local w2 = 0xdc00 + math.floor(unicode % (2^10))
    if w1 >= 0xd800 and w1 <= 0xdbff then
      error('impossible!')
    end
    w1 = string.format('%04x', w1)
    w2 = string.format('%04x', w2)
    return [[\u]] .. w1 .. [[\u]] .. w2
  end
end

-----------------------------------------------------------------
-- Transform a lua value to json string
local trynum = 0
function Unmarshal(v)
  trynum = trynum + 1
  if trynum > 36 then
    error(tbl2str(v))
  end
  
  -- Handle Number values
  if type(v) == 'number' then
    return UnmarshalNumber(v)
  
  -- Handle string values
  elseif type(v) == 'string' then
    return UnmarshalString(v)
  
  -- Handle table values
  elseif type(v) == 'table' then
    return UnmarshalTable(v)
  
  -- Handle nil and boolean
  else
    return UnmarshalOther(v)
  end
end

-- Return true if t is array
function IsArray(t)
  for k, v in pairs(t) do
    if type(k) ~= 'number' or math.floor(k) ~= k then 
      return false 
    end
  end
  return true
end

-- Return true if t is enmpty table
function IsEmpty(t)
  for key, val in pairs(t) do
    return false
  end
  return true
end

-- Return json number representation
function UnmarshalNumber(v)
  return string.format('%.16g', v)
end

-- Return json string representation
function UnmarshalString(v)
  
  v = v:gsub('\\', [[\\]])
  
  v = v:gsub('\"', [[\"]])
  v = v:gsub('/', [[\/]])
  v = v:gsub('\b', [[\b]])
  v = v:gsub('\f', [[\f]])
  v = v:gsub('\n', [[\n]])
  v = v:gsub('\r', [[\r]])
  v = v:gsub('\t', [[\t]])
  
  v = v:gsub('\a', [[\u0007]])
  v = v:gsub('\v', [[\u000b]])
  
  local res = {}
  --[=[
  local i = 1
  while i <= #v do
    local utf8bytes = UTF8Bytes(v:sub(i,i))
    local unicode = UTF8ToUnicode(v:sub(i, i+utf8bytes-1))
    table.insert(res, UnicodeToJson(unicode))
    i = i + utf8bytes
  end
  ]=]--
  
  
  return '"' .. table.concat(res) .. '"'
end

-- Return json object or array
function UnmarshalTable(v)
  -- Handle empty tables
  if IsEmpty(v) then
    return '{}'
  end
  -- Handle array tables
  if IsArray(v) then
    local res = {}
    for i = 1, table.maxn(v) do
      trynum = trynum - 1
      table.insert(res, Unmarshal(v[i]))
    end
    return '[' .. table.concat(res, ',') .. ']'
  -- Handle regular tables
  else
    local res = {}
    for key, val in pairs(v) do
      trynum = trynum - 1
      table.insert(res, UnmarshalString(tostring(key)) .. ':' .. Unmarshal(val))
    end
    return '{' .. table.concat(res, ',') .. '}'
  end
end

-- Return null or true or false
function UnmarshalOther(v)
  if type(v) == 'nil' then
    return 'null'
  elseif type(v) == 'boolean' then
    return tostring(v)
  end
end

------------------------------------------------------------------------------
------------------------------------------------------------------------------
------------------------------------------------------------------------------

-- Translate json string to a lua object
function Marshal(s, start)
  -- skip whitespaces
  start = s:find('%S', start)
  local token = s:sub(start, start)
  
  -- handle array
  if token == '[' then
    return MarshalArray(s, start)
  -- handle dict
  elseif token == '{' then
    return MarshalDict(s, start)
  -- handle string
  elseif token == '"' then
    return MarshalString(s, start)
  -- handle number
  elseif token:find('[0-9+-]') then
    return MarshalNumber(s, start)
  -- handle null, true, false
  else
    return MarshalOther(s, start)
  end
  
end

function MarshalString(s, start)
  -- find the next string token '"'
  local _, j = s:find('[^\\]"', start)
  -- return the string object and the index of next part
  s = s:sub(start+1, j-1)
  s = s:gsub('\\\\', '\\')
  s = s:gsub('\\"', '"')
  s = s:gsub('\\/', '/')
  s = s:gsub('\\b', '\b')
  s = s:gsub('\\f', '\f')
  s = s:gsub('\\n', '\n')
  s = s:gsub('\\r', '\r')
  s = s:gsub('\\t', '\t')
  
  local res = {}
  local lastend = 1
  local m, n = s:find('\\u%x%x%x%x')
  while m do
    local unicode = tonumber('0x'..s:sub(m+2,n))
    
    if unicode < 0x0080 then -- it's a ascii char
      table.insert(res, s:sub(lastend, m-1) .. string.char(unicode))
      lastend = n+1
      
    elseif unicode < 0x0800 then -- it'a a two bytes utf-8
      local byte1 = math.floor(unicode/(2^6)) + 0xc0
      local byte2 = unicode%(2^6) + 0x80
      table.insert(res, s:sub(lastend, m-1) .. string.char(byte1) .. string.char(byte2))
      lastend = n+1
      
    else -- it's a three bytes utf-8
      local byte1 = math.floor(unicode/(2^12)) + 0xe0
      local byte2 = math.floor(unicode/(2^6))%(2^6) + 0x80
      local byte3 = unicode%(2^6) + 0x80
      table.insert(res, s:sub(lastend, m-1) .. string.char(byte1) .. string.char(byte2) .. string.char(byte3))
      lastend = n+1
    end
    -- find the next unicode
    m, n = s:find('\\u%x%x%x%x', lastend)
  end
  table.insert(res, s:sub(lastend))
  
  return table.concat(res), j+1
end

function MarshalNumber(s, start)
  local j = s:find('[^0-9.eE+-]', start) or #s+1
  local num = tonumber(s:sub(start, j-1))
  return num, j
end

function MarshalOther(s, start)
  if s:sub(start, start+3) == 'null' then
    return nil, start+4
  elseif s:sub(start, start+3) == 'true' then
    return true, start+4
  elseif s:sub(start, start+4) == 'false' then
    return false, start+5
  end
end

function MarshalArray(s, start)
  local arr = {}
  local val
  local index = 1
  start = s:find('%S', start+1)
  while s:sub(start, start) ~= ']' do
    val, start = Marshal(s, start)
    arr[index] = val
    index = index + 1
    start = s:find('[^%s,]', start)
  end
  return arr, start+1
end

function MarshalDict(s, start)
  local dict = {}
  local key, val
  start = s:find('%S', start+1)
  while s:sub(start, start) ~= '}' do
    key, start = MarshalString(s, start)
    start = s:find('[^%s:]', start)
    val, start = Marshal(s, start)
    dict[key] = val
    start = s:find('[^%s,]', start)
  end
  return dict, start+1
end

return {
  Unmarshal = Unmarshal,
  Marshal = Marshal,
}
