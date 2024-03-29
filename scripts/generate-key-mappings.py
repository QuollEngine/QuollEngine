def glfw(k):
    upper_k = f'{k}'.upper()
    return f'getKeyboardKeyFromGlfw(GLFW_KEY_{upper_k})'

def print_map(key, glfw_key):
    print('{"', f'KEY_{key}', '", ', glfw(glfw_key), '},', sep='')

print('// letters')
letters = [chr(i).upper() for i in range(ord('a'), ord('z') + 1)]
for k in letters:
    print_map(k, k)

print()
print('// digits')
digits = [i for i in range(0, 10)]
for k in digits:
    print_map(k, k)

print()
print('// special')
special_chars = [
 'COMMA',
 'MINUS',
 'PERIOD',
 'SLASH',
 'SEMICOLON',
 'EQUAL',
 'LEFT_BRACKET',
 'BACKSLASH',
 'RIGHT_BRACKET',
 'GRAVE_ACCENT'
]

for k in special_chars:
    print_map(k, k)

commands = [
    'SPACE',
    'ESCAPE',
    'ENTER',
    'TAB',
    'BACKSPACE',
    'INSERT',
    'DELETE',
    'RIGHT',
    'LEFT',
    'DOWN',
    'UP',
    'PAGE_UP',
    'PAGE_DOWN',
    'HOME',
    'END',
    'CAPS_LOCK',
    'SCROLL_LOCK',
    'NUM_LOCK',
    'PRINT_SCREEN',
    'PAUSE',
    'LEFT_SHIFT',
    'LEFT_CONTROL',
    'LEFT_ALT',
    'LEFT_SUPER',
    'RIGHT_SHIFT',
    'RIGHT_CONTROL',
    'RIGHT_ALT',
    'RIGHT_SUPER',
]
   
print()
print('// command keys')
for k in commands:
    print_map(k, k)