import sys
import re

__example_match__ = '''protocol.h(30) : Typemap for ROLE_INFO info_list[10] (memberin) : %typemap(memberin) SWIGTYPE [ANY]'''
__example_char__ = '''protocol.h(30) : Typemap for char info_list[10] (memberin) : %typemap(memberin) SWIGTYPE [ANY]'''
__example_unsiged_char__ = '''protocol.h(30) : Typemap for unsigned char info_list[10] (memberin) : %typemap(memberin) SWIGTYPE [ANY]'''

def TrueMatch(text):
    return text, True, True, True

def NoneMatch(text):
    return text, None, None, None

def MatchAny():
    def DoFilte(obj):
        return obj
    return DoFilte

def MatchMemberIn(filter_func):
    def DoFilte(obj):
        text, _type_, name, typemap = filter_func(obj)
        result = re.search('''.* : Typemap for (.+) ([a-zA-Z_]\w+\[.*\]) \(memberin\) : %typemap\(memberin\) (.*\[.*\])$''', text)
        if result:
            _type_, name, typemap = result.group(1, 2, 3)
            return text, _type_, name, typemap
        else:
            return NoneMatch(text)
    return DoFilte
    
def FilterCharType(filter_func):
    def DoFilte(obj):
        text, _type_, name, typemap = filter_func(obj)
        if _type_ and 'char' not in _type_.split(' '):
            return text, _type_, name, typemap
        else:
            return NoneMatch(text)
    return DoFilte

def MatchedLines(src, match_func):
    for line in src:
        obj = match_func(TrueMatch(line))
        if obj != NoneMatch(line):
            yield obj
    
def Parse(src_file, dest=sys.stdout):
    results = []
    #match_func = MatchAny()
    #match_func = MatchMemberIn(MatchAny())
    match_func = FilterCharType(MatchMemberIn(MatchAny()))
    with open(src_file) as src:
        for item in MatchedLines(src, match_func):
            print(item)
        
def Usage():
    print('Usage: parse_array.py <input> [<output>]')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        Usage()
    else:
        Parse(*sys.argv[1:])
