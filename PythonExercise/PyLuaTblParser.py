# -*- coding: utf-8 -*-

import string

#Exception classes

class ParseError(Exception):
    def __init__(self, info):
        self.info = info
    def __str__(self):
        return self.info

class DumpError(ParseError):
    pass
        
class LoadError(ParseError):
    pass

#inter dump function

def _dump(v):
    if v is None:
        return _dumpNone(v)
    elif type(v) is str:
        return _dumpString(v)
    elif type(v) is bool:
        return _dumpBool(v)
    elif type(v) in (int, long, float):
        return _dumpNumber(v)
    elif type(v) in (list, tuple):
        return _dumpList(v)
    elif type(v) is dict:
        return _dumpDict(v)
    else:
        raise DumpError('invalid value type')

def _dumpNone(v):
    return 'nil'

def _dumpString(v):
    v = v.replace('\\', r'\\')
    v = v.replace('\a', r'\a')
    v = v.replace('\b', r'\b')
    v = v.replace('\f', r'\f')
    v = v.replace('\n', r'\n')
    v = v.replace('\r', r'\r')
    v = v.replace('\t', r'\t')
    v = v.replace('\v', r'\v')
    v = v.replace('\"', r'\"')
    v = v.replace('\'', r'\'')
    res = ['"']
    lastend = 0
    for i in range(len(v)):
        if v[i] not in string.printable:
            res.append(v[lastend:i])
            res.append(r'\x')
            res.append('{:02x}'.format(ord(v[i])))
            lastend = i+1
    res.append(v[lastend:])
    res.append('"')
    return ''.join(res)
    
def _dumpBool(v):
    return 'true' if v else 'false'

def _dumpNumber(v):
    return '{:.16g}'.format(v)

def _dumpList(a):
    res = ['{']
    for val in a:
        res.append(_dump(val))
        res.append(',')
    res.append('}')
    return ''.join(res)

def _dumpDict(d):
    res = ['{']
    for key, val in d.items():
        res.append('[')
        res.append(_dump(key))
        res.append(']=')
        res.append(_dump(val))
        res.append(',')
    res.append('}')
    return ''.join(res)


#inner load function

def _skipSpace(s, start = 0):
    for i in range(start, len(s)):
        if s[i] not in string.whitespace:
            return i
    else:
        return len(s)

def _skipComment(s, start = 0):
    assert(s[start:start+2] == '--')
    # when it's a short comment
    if s[start+2] != '[':
        end = s.find('\n', start+2)
        return end+1 if end != -1 else len(s)
    # when it's a long comment
    else:
        i = s.find('[', start+3)
        assert(s[start+3:i] == '='*(i-start-3))
        ctoken = '--' + ']' + '='*(i-start-3) +']'
        j = s.find(ctoken, i+1)
        if j != -1:
            return j + len(ctoken)
        else:
            raise LoadError('unfinished long comment')

def _skipSpaceComment(s, start = 0):
    start = _skipSpace(s, start)
    while s[start:start+2] == '--':
        start = _skipComment(s, start)
        start = _skipSpace(s, start)        
    return start

def _load(s, start = 0):
    start = _skipSpaceComment(s, start)
    if start == len(s):
        raise LoadError('empty expression')
    token = s[start]
    if token in '\'\"[':
        return _loadString(s, start)
    elif token in string.digits + '.+-':
        return _loadNumber(s, start)
    elif token == '{':
        return _loadTable(s, start)
    elif s[start:start+3] == 'nil' and\
    (start+3 == len(s) or s[start+3] not in string.letters+string.digits+'_'):
        return None, start+3
    elif s[start:start+4] == 'true' and\
    (start+4 == len(s) or s[start+4] not in string.letters+string.digits+'_'):
        return True, start+4
    elif s[start:start+5] == 'false' and\
    (start+5 == len(s) or s[start+5] not in string.letters+string.digits+'_'):
        return False, start+5
    else:
        raise LoadError('invalid lua expression')

_escapedict = {
'\\':'\\', 'a':'\a', 'b':'\b', 'f':'\f',
'n':'\n', 'r':'\r', 't':'\t', 'v':'\v',
'"':'"', '\'':'\'', 'u':'u', '/':'/', '\n':'\n'
}

def _getStringBody(s, start):
    assert(s[start] in '\'\"[')
    token = s[start]
    if token in '\'\"':
        for i in range(start+1, len(s)):
            if (s[i] == token and s[i-1] != '\\') or\
            (s[i] == token and s[i-2:i] == '\\\\'):
                return s[start+1:i], i+1
        else:
            raise LoadError('string not closed')
    elif token == '[':
        i = s.find('[', start+1)
        if i == -1 or s[start+1:i] != '='*(i-start-1):
            raise LoadError('not a string')
        ctoken = ']' + '='*(i-start-1) + ']'
        j = s.find(ctoken, i+1)
        if j != -1:
            return s[i+1:j], j+len(ctoken)
        else:
            raise LoadError('string not closed')

def _loadString(s, start):
    token = s[start]
    if token == '[':
        i = s.find('[', start+1)
        if i == -1 or s[start+1:i] != '='*(i-start-1):
            raise LoadError('not a string')
        ctoken = ']' + '='*(i-start-1) + ']'
        j = s.find(ctoken, i+1)
        if j != -1:
            return s[i+1:j], j+len(ctoken)
        else:
            raise LoadError('string not closed')
    else:
        res = []
        i = start+1
        while s[i] != '"':
            if s[i] == '\\':
                ec = s[i+1]
                if ec in _escapedict:
                    res.append(_escapedict[ec])
                    i = i+2
                elif ec == 'x':
                    try:
                        res.append(chr(int(s[i+2:i+4], 16)))
                        i = i+4
                    except ValueError:
                        raise LoadError('invalid escape sequence')
                elif ec.isdigit():
                    if s[i+2].isdigit() and s[i+3].isdigit():
                        nend = i + 4
                    elif s[i+2].isdigit():
                        nend = i + 3
                    else:
                        nend = i + 2
                    try:
                        res.append(chr(int(s[i+1:nend])))
                        i = nend
                    except ValueError:
                        raise LoadError('invalid escape sequence')
                else:
                    raise LoadError('redundant escape in string')
            else:
                res.append(s[i])
                i = i+1
        return ''.join(res), i+1
 
def _getNumberBody(s, start):
    for i in range(start, len(s)):
        if s[i] not in string.hexdigits + '+-.eExXpP':
            end = i
            break
    else:
        end = len(s)
    j = s.find('--', start, end)
    return (s[start:end], end) if j == -1 else (s[start:j], j)

def _loadNumber(s, start):
    body, end = _getNumberBody(s, start)
    if s[start:start+2] not in ('0x', '0X'):
        if '.' in body or 'e' in body or 'E' in body:
            return float(body), end
        else:
            return int(body), end
    else:
        body = body.lower()
        mainbody, sep, expBody = body.partition('p')
        intbody, sep, fracbody = mainbody.partition('.')
        res = int(intbody, 16)
        if fracbody != '':
            res = res + int(fracbody, 16)*(16**(-len(fracbody)))
        if expBody != '':
            res = res*(2**int(expBody))
        return res, end


def _loadTable(s, start):
    assert(s[0] == '{')
    resDict = {}
    resList = []
    start = _skipSpaceComment(s, start+1)
    if start == len(s):
        raise LoadError('incomplete lua table')
    
    while s[start] != '}':
        try:
            val, start = _load(s, start)
            resList.append(val)
        except LoadError:
            if s[start] == '[':
                key, start = _load(s, start+1)
                start = _skipSpaceComment(s, start)
                assert(s[start] == ']')
                start = _skipSpaceComment(s, start+1)
                assert(s[start] == '=')
                val, start = _load(s, start+1)
                if val is not None:
                    resDict[key] = val
            else:
                end = s.find('=', start)
                key = s[start:end]
                key = key.strip()
                val, start = _load(s, end+1)
                if val is not None:
                    resDict[key] = val
        start = _skipSpaceComment(s, start)
        if s[start] == ',' or s[start] == ';':
            start = _skipSpaceComment(s, start+1)

    if not resDict and not resList:
        return {}, start+1
    
    if resDict:
        for i in range(len(resList)):
            resDict[i+1] = resList[i]
        return resDict, start+1
    else:
        return resList, start+1

def _deepCopy(v):
    if type(v) in (list, tuple):
        res = [None]*len(v)
        for i in range(len(res)):
            res[i] = _deepCopy(v[i])
        if type(v) is tuple:
            return tuple(res)
        else:
            return res
            
    elif type(v) is dict:
        res = {}
        for key in v.keys():
            res[key] = _deepCopy(v[key])
        return res
        
    else:
        return v


class PyLuaTblParser:
    """Lua table Parser"""
    def __init__(self):
        self._data = None
    
    def load(self, s):
        self._data, shit = _load(s)
    
    def dump(self):
        return _dump(self._data)
    
    def loadLuaTable(self, f):
        with open(f) as fobj:
            self.load(fobj.read())
    
    def dumpLuaTable(self, f):
        with open(f, 'w') as fobj:
            fobj.write(self.dump())
    
    def loadDict(self, d):
        d = _deepCopy(d)
        self._data = {key:val for key, val in d.items() if type(key) in (str, int, long, float)}
        
    
    def dumpDict(self):
        return _deepCopy(self._data)
    

