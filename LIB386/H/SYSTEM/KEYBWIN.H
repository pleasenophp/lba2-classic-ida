//──────────────────────────────────────────────────────────────────────────
#ifndef	LIB_SYSTEM_KEYBWIN
#define LIB_SYSTEM_KEYBWIN

#include <SDL_keycode.h>

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
extern	"C"	{
#endif

//──────────────────────────────────────────────────────────────────────────
extern	S32		AsciiMode		;
extern	S32		Key			;

//──────────────────────────────────────────────────────────────────────────
extern	U8		TabKeys[]		;

//──────────────────────────────────────────────────────────────────────────
//extern	WINDOW_PROC_PTR	OldKeybWindowProc	;

//──────────────────────────────────────────────────────────────────────────
//extern	WINDOW_PROC	KeybWindowProc		;

//──────────────────────────────────────────────────────────────────────────
extern	void	ManageKeyboard()		;

//──────────────────────────────────────────────────────────────────────────
extern	S32	GetAscii()			;

//──────────────────────────────────────────────────────────────────────────
extern	void	ClearAsciiBuffer()		;

//──────────────────────────────────────────────────────────────────────────
extern	HANDLE	AsciiMutex			;

//──────────────────────────────────────────────────────────────────────────
#define	AllowExtendedKeyb()

//──────────────────────────────────────────────────────────────────────────
#define KeyIndex(x)     ((x)<256?(x):256-32+((x)>>3))
#define KeyBit(x)       ((x)<256?0x80:(1<<((x)&7)))
//#define CheckKey(x)     (TabKeys[KeyIndex(x)]&KeyBit(x))
#define CheckKey(x)     ((x)<256?TabKeys[(x)]&0x80:TabKeys[256-32+((x)>>3)]&(1<<((x)&7)))
#define FlagKey(x)      (CheckKey(x)>>((x)<256?7:(x)&7))
/*
#define KeyIndex(x)     ((x)/4)
#define KeyBit(x)       (128 << (((x)&3)*8))
#define CheckKey(x)     (TabKeys[KeyIndex(x)]&KeyBit(x))
#define FlagKey(x)      ((CheckKey(x)>>(((x)&3)*8))/128)
*/
//──────────────────────────────────────────────────────────────────────────
#define K_UP            SDL_SCANCODE_UP
#define K_DOWN          SDL_SCANCODE_DOWN
#define K_LEFT          SDL_SCANCODE_LEFT
#define K_RIGHT         SDL_SCANCODE_RIGHT
#define K_HOME          SDL_SCANCODE_HOME
#define K_PAGE_UP       SDL_SCANCODE_PAGEUP
#define K_PAGE_DOWN     SDL_SCANCODE_PAGEDOWN
#define K_END           SDL_SCANCODE_END

#define K_SPACE         SDL_SCANCODE_SPACE
#define K_ENTER         SDL_SCANCODE_RETURN
#define K_SUPPR         SDL_SCANCODE_DELETE
#define K_INSER         SDL_SCANCODE_INSERT
#define K_CAPSLOCK      SDL_SCANCODE_CAPSLOCK
#define K_ALT           SDL_SCANCODE_LALT
#define K_CTRL          SDL_SCANCODE_LCTRL
#define K_CTRL_LEFT     SDL_SCANCODE_LCTRL
#define K_CTRL_RIGHT    SDL_SCANCODE_RCTRL
#define K_ALT_LEFT      SDL_SCANCODE_LALT
#define K_ALT_RIGHT     SDL_SCANCODE_RALT
#define K_SHIFT         SDL_SCANCODE_LSHIFT
#define K_SHIFT_LEFT    SDL_SCANCODE_LSHIFT
#define K_SHIFT_RIGHT   SDL_SCANCODE_RSHIFT

#define K_ESC           SDL_SCANCODE_ESCAPE

#define K_F1            SDL_SCANCODE_F1
#define K_F2            SDL_SCANCODE_F2
#define K_F3            SDL_SCANCODE_F3
#define K_F4            SDL_SCANCODE_F4
#define K_F5            SDL_SCANCODE_F5
#define K_F6            SDL_SCANCODE_F6
#define K_F7            SDL_SCANCODE_F7
#define K_F8            SDL_SCANCODE_F8
#define K_F9            SDL_SCANCODE_F9
#define K_F10           SDL_SCANCODE_F10
#define K_F11           SDL_SCANCODE_F11
#define K_F12           SDL_SCANCODE_F12

#define K_PLUS          SDL_SCANCODE_KP_PLUS
#define K_MOINS         SDL_SCANCODE_KP_MINUS
#define K_BACKSPACE     SDL_SCANCODE_KP_BACKSPACE
#define K_TAB           SDL_SCANCODE_KP_TAB
//#define K_NUMLOCK       SDL_SCANCODE_KP_NUMLOCK

#define	K_PRINTSCR      SDL_SCANCODE_PRINTSCREEN
#define	K_SCROLLLOCK	SDL_SCANCODE_SCROLLLOCK
#define	K_PAUSE         SDL_SCANCODE_PAUSE

#define K_NUMPAD_1      SDL_SCANCODE_KP_1
#define K_NUMPAD_2      SDL_SCANCODE_KP_2
#define K_NUMPAD_3      SDL_SCANCODE_KP_3
#define K_NUMPAD_4      SDL_SCANCODE_KP_4
#define K_NUMPAD_5      SDL_SCANCODE_KP_5
#define K_NUMPAD_6      SDL_SCANCODE_KP_6
#define K_NUMPAD_7      SDL_SCANCODE_KP_7
#define K_NUMPAD_8      SDL_SCANCODE_KP_8
#define K_NUMPAD_9      SDL_SCANCODE_KP_9
#define K_NUMPAD_0      SDL_SCANCODE_KP_0

#define	K_NUMPAD_ENTER	SDL_SCANCODE_KP_ENTER

#define	K_NUMPAD_POINT	SDL_SCANCODE_KP_DECIMAL

#define	K_NUMPAD_DIV	VK_DIVIDE
#define	K_NUMPAD_MUL	VK_MULTIPLY
#define K_NUMPAD_PLUS   VK_ADD
#define K_NUMPAD_MOINS  VK_SUBTRACT

#define K_WINDOWS_LEFT	VK_LWIN
#define K_WINDOWS_RIGHT	VK_RWIN
#define K_APPLICATION	VK_APPS

#define K_GRAY_UP	SDL_SCANCODE_UP
#define K_GRAY_DOWN	SDL_SCANCODE_DOWN
#define K_GRAY_LEFT	SDL_SCANCODE_LEFT
#define K_GRAY_RIGHT 	SDL_SCANCODE_RIGHT
#define K_GRAY_HOME	K_HOME
#define K_GRAY_END	K_END
#define K_GRAY_PAGE_UP	K_PAGE_UP
#define K_GRAY_PAGE_DOWN K_PAGE_DOWN
#define K_GRAY_INSER	K_INSER
#define K_GRAY_SUPPR	K_SUPPR

#define	K_CARRE		0xDE

#define K_A             SDL_SCANCODE_A
#define K_B             SDL_SCANCODE_B
#define K_C             SDL_SCANCODE_C
#define K_D             SDL_SCANCODE_D
#define K_E             SDL_SCANCODE_E
#define K_F             SDL_SCANCODE_F
#define K_G             SDL_SCANCODE_G
#define K_H             SDL_SCANCODE_H
#define K_I             SDL_SCANCODE_I
#define K_J             SDL_SCANCODE_J
#define K_K             SDL_SCANCODE_K
#define K_L             SDL_SCANCODE_L
#define K_M             SDL_SCANCODE_M
#define K_N             SDL_SCANCODE_N
#define K_O             SDL_SCANCODE_O
#define K_P             SDL_SCANCODE_P
#define K_Q             SDL_SCANCODE_Q
#define K_R             SDL_SCANCODE_R
#define K_S             SDL_SCANCODE_S
#define K_T             SDL_SCANCODE_T
#define K_U             SDL_SCANCODE_U
#define K_V             SDL_SCANCODE_V
#define K_W             SDL_SCANCODE_W
#define K_X             SDL_SCANCODE_X
#define K_Y             SDL_SCANCODE_Y
#define K_Z             SDL_SCANCODE_Z

#define K_1             SDL_SCANCODE_1
#define K_2             SDL_SCANCODE_2
#define K_3             SDL_SCANCODE_3
#define K_4             SDL_SCANCODE_4
#define K_5             SDL_SCANCODE_5
#define K_6             SDL_SCANCODE_6
#define K_7             SDL_SCANCODE_7
#define K_8             SDL_SCANCODE_8
#define K_9             SDL_SCANCODE_9
#define K_0             SDL_SCANCODE_0

#define K_INF			SDL_SCANCODE_KP_LESS
#define K_EXC			SDL_SCANCODE_SLASH

//──────────────────────────────────────────────────────────────────────────
#define A_UP            SDLK_UP
#define A_DOWN          SDLK_DOWN
#define A_LEFT          SDLK_LEFT
#define A_RIGHT         SDLK_RIGHT

#define A_HOME          SDLK_HOME
#define A_END           SDLK_END
#define A_PGUP          SDLK_PAGEUP
#define A_PGDN          SDLK_PAGEDOWN

#define A_SPACE         VK_SPACE
#define A_RETURN        SDLK_RETURN

#define A_SUPPR         SDLK_DELETE
#define A_INS			SDLK_INSERT
#define A_CARRE			SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_GRAVE)

#define A_ESC           SDLK_ESCAPE

#define A_F1            VK_F1
#define A_F2            VK_F2
#define A_F3            VK_F3
#define A_F4            VK_F4
#define A_F5            VK_F5
#define A_F6            VK_F6
#define A_F7            VK_F7
#define A_F8            VK_F8
#define A_F9            VK_F9
#define A_F10           VK_F10
#define A_F11           VK_F11
#define A_F12           VK_F12


#define A_N_PLUS        VK_ADD
#define A_N_MOINS       VK_SUBTRACT
#define A_BACKSPACE     0x8
#define A_TAB           VK_TAB
#define A_NUMLOCK       VK_NUMLOCK

#define A_A             'A'
#define A_B             'B'
#define A_C             'C'
#define A_D             'D'
#define A_E             'E'
#define A_F             'F'
#define A_G             'G'
#define A_H             'H'
#define A_I             'I'
#define A_J             'J'
#define A_K             'K'
#define A_L             'L'
#define A_M             'M'
#define A_N             'N'
#define A_O             'O'
#define A_P             'P'
#define A_Q             'Q'
#define A_R             'R'
#define A_S             'S'
#define A_T             'T'
#define A_U             'U'
#define A_V             'V'
#define A_W             'W'
#define A_X             'X'
#define A_Y             'Y'
#define A_Z             'Z'

#define A_1             '1'
#define A_2             '2'
#define A_3             '3'
#define A_4             '4'
#define A_5             '5'
#define A_6             '6'
#define A_7             '7'
#define A_8             '8'
#define A_9             '9'
#define A_0             '0'

//──────────────────────────────────────────────────────────────────────────
#ifdef	__cplusplus
}
#endif

//──────────────────────────────────────────────────────────────────────────
#endif//LIB_SYSTEM_KEYBWIN

//──────────────────────────────────────────────────────────────────────────
