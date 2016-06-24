#include "arch/x86/keyboard.h"

#include <stdio.h>
#include <string.h>
#include <arch/x86/pic.h>
#include <arch/x86/idt.h>
#include <arch/x86/ioport.h>

// Keyboard encoder 8048's registers
#define KEYBOARD_ENC_CMD_REG   0x60 // out
#define KEYBOARD_ENC_INPUT_BUF 0x60 // in

// Keyboard encoder's commands
enum KYBRD_ENC_CMDS {
    KEYBOARD_ENC_CMD_SET_LED            = 0xED,
    KEYBOARD_ENC_CMD_ECHO               = 0xEE,
    KEYBOARD_ENC_CMD_SCAN_CODE_SET      = 0xF0,
    KEYBOARD_ENC_CMD_ID                 = 0xF2,
    KEYBOARD_ENC_CMD_AUTODELAY          = 0xF3,
    KEYBOARD_ENC_CMD_ENABLE             = 0xF4,
    KEYBOARD_ENC_CMD_RESETWAIT          = 0xF5,
    KEYBOARD_ENC_CMD_RESETSCAN          = 0xF6,
    KEYBOARD_ENC_CMD_ALL_AUTO           = 0xF7,
    KEYBOARD_ENC_CMD_ALL_MAKEBREAK      = 0xF8,
    KEYBOARD_ENC_CMD_ALL_MAKEONLY       = 0xF9,
    KEYBOARD_ENC_CMD_ALL_MAKEBREAK_AUTO = 0xFA,
    KEYBOARD_ENC_CMD_SINGLE_AUTOREPEAT  = 0xFB,
    KEYBOARD_ENC_CMD_SINGLE_MAKEBREAK   = 0xFC,
    KEYBOARD_ENC_CMD_SINGLE_BREAKONLY   = 0xFD,
    KEYBOARD_ENC_CMD_RESEND             = 0xFE,
    KEYBOARD_ENC_CMD_RESET              = 0xFF
};

enum KEYBOARD_ERROR {
    KEYBOARD_ERR_BUF_OVERRUN = 0,
    KEYBOARD_ERR_ID_RET      = 0x83AB,
    KEYBOARD_ERR_BAT         = 0xAA, //note: can also be LEFT SHIFT key make code
    KEYBOARD_ERR_ECHO_RET    = 0xEE,
    KEYBOARD_ERR_ACK         = 0xFA,
    KEYBOARD_ERR_BAT_FAILED  = 0xFC,
    KEYBOARD_ERR_DIAG_FAILED = 0xFD,
    KEYBOARD_ERR_RESEND_CMD  = 0xFE,
    KEYBOARD_ERR_KEY         = 0xFF
};

// Keyboard controller 8042's registers
#define KEYBOARD_CTRL_CMD_REG    0x64 // out
#define KEYBOARD_CTRL_STATUS_REG 0x64 // in

// Keyboard controller's commands
enum KYBRD_CTRL_CMDS {
    KEYBOARD_CTRL_CMD_READ             = 0x20,
    KEYBOARD_CTRL_CMD_WRITE            = 0x60,
    KEYBOARD_CTRL_CMD_SELF_TEST        = 0xAA,
    KEYBOARD_CTRL_CMD_INTERFACE_TEST   = 0xAB,
    KEYBOARD_CTRL_CMD_DISABLE          = 0xAD,
    KEYBOARD_CTRL_CMD_ENABLE           = 0xAE,
    KEYBOARD_CTRL_CMD_READ_IN_PORT     = 0xC0,
    KEYBOARD_CTRL_CMD_READ_OUT_PORT    = 0xD0,
    KEYBOARD_CTRL_CMD_WRITE_OUT_PORT   = 0xD1,
    KEYBOARD_CTRL_CMD_READ_TEST_INPUTS = 0xE0,
    KEYBOARD_CTRL_CMD_SYSTEM_RESET     = 0xFE,
    KEYBOARD_CTRL_CMD_MOUSE_DISABLE    = 0xA7,
    KEYBOARD_CTRL_CMD_MOUSE_ENABLE     = 0xA8,
    KEYBOARD_CTRL_CMD_MOUSE_PORT_TEST  = 0xA9,
    KEYBOARD_CTRL_CMD_MOUSE_WRITE      = 0xD4
};

// Keyboard status register
#define KB_SR_OUTPUT_BUFFER_EMPTY (0 << 0)
#define KB_SR_OUTPUT_BUFFER_FULL  (1 << 0)

#define KB_SR_INPUT_BUFFER_EMPTY  (0 << 1)
#define KB_SR_INPUT_BUFFER_FULL   (1 << 1)

#define KB_SR_SELFTEST_ONGOING    (0 << 2)
#define KB_SR_SELFTEST_COMPLETED  (1 << 2)

#define KB_SR_LAST_WRITE_DATA     (0 << 3)
#define KB_SR_LAST_WRITE_CMD      (1 << 3)

#define KB_SR_LOCKED              (0 << 4)
#define KB_SR_UNLOCKED            (1 << 4)

#define KB_SR_PS2_KEYBOARD_DATA   (0 << 5)
#define KB_SR_PS2_MOUSE_DATA      (1 << 5)
#define KB_SR_ATSYS_OK_FLAG       (0 << 5)
#define KB_SR_ATSYS_TIMEOUT       (1 << 5)

#define KB_SR_NOTIMEOUT           (0 << 6)
#define KB_SR_TIMEOUT             (1 << 6)

#define KB_SR_NOERROR             (0 << 7)
#define KB_SR_PARITY_ERROR        (1 << 7)

enum KEYBOARD_CTRL_STATUS_MASK {
    KEYBOARD_CTRL_STATUS_MASK_OUT_BUF  = 0x01,
    KEYBOARD_CTRL_STATUS_MASK_IN_BUF   = 0x02,
    KEYBOARD_CTRL_STATUS_MASK_SYSTEM   = 0x04,
    KEYBOARD_CTRL_STATUS_MASK_CMD_DATA = 0x08,
    KEYBOARD_CTRL_STATUS_MASK_LOCKED   = 0x10,
    KEYBOARD_CTRL_STATUS_MASK_AUX_BUF  = 0x20,
    KEYBOARD_CTRL_STATUS_MASK_TIMEOUT  = 0x40,
    KEYBOARD_CTRL_STATUS_MASK_PARITY   = 0x80
};

typedef struct {
    uint8_t output_buf_status : 1;
    uint8_t input_buf_status : 1;
    uint8_t system_flag : 1;
    uint8_t command_data_flag : 1;
    uint8_t keyboard_locked : 1;
    uint8_t auxiliary_output_buf : 1;
    uint8_t timeout : 1;
    uint8_t parity_error : 1;
} keyboard_status_reg_t;


// Basic communication with keyboard controllers
static inline uint8_t keyboard_ctrl_read_status(void)
{
    return inb(KEYBOARD_CTRL_STATUS_REG);
}

static inline uint8_t keyboard_enc_read_buf(void)
{
    return inb(KEYBOARD_ENC_INPUT_BUF);
}

static void keyboard_ctrl_send_cmd(uint8_t cmd)
{
    while ( (keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATUS_MASK_IN_BUF) != KB_SR_INPUT_BUFFER_EMPTY ) {};
    outb(KEYBOARD_CTRL_CMD_REG, cmd);
}

static void keyboard_enc_send_cmd(uint8_t cmd)
{
    while ( (keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATUS_MASK_IN_BUF) != KB_SR_INPUT_BUFFER_EMPTY ) {};
    outb(KEYBOARD_ENC_CMD_REG, cmd);
}

// Typematics
#define set_repeat_rate(nbchars) \
    (0x1f * (nbchars - REPEAT_RATE_MIN) / (REPEAT_RATE_MAX - REPEAT_RATE_MIN + 1))

typedef struct {
    uint8_t repeat_rate : 5;
    uint8_t repeat_delay : 2;
    uint8_t unused : 1;
} typematic_options_t;

// original xt scan code set (array index == make code)
static int keyboard_scancode_std[] = {
   /* scancode                   */  // key code        // normal          // (SHIFT || CAPSLOCK) + key
   /* 0x00                       */  KEY_UNKNOWN,       '\0',              '\0',
   /* 0x01                       */  KEY_ESCAPE,        KEY_ESCAPE,        KEY_ESCAPE,
   /* 0x02                       */  KEY_1,             '1',               '!',
   /* 0x03                       */  KEY_2,             '2',               '@',
   /* 0x04                       */  KEY_3,             '3',               '#',
   /* 0x05                       */  KEY_4,             '4',               '$',
   /* 0x06                       */  KEY_5,             '5',               '%',
   /* 0x07                       */  KEY_6,             '6',               '^',
   /* 0x08                       */  KEY_7,             '7',               '&',
   /* 0x09                       */  KEY_8,             '8',               '*',
   /* 0x0a                       */  KEY_9,             '9',               '(',
   /* 0x0b                       */  KEY_0,             '0',               ')',
   /* 0x0c                       */  KEY_MINUS,         '-',               '_',
   /* 0x0d                       */  KEY_EQUAL,         '=',               '+',
   /* 0x0e                       */  KEY_BACKSPACE,     '\b',              '\b',
   /* 0x0f                       */  KEY_TAB,           '\t',              '\t',
   /* 0x10                       */  KEY_Q,             'q',               'Q',
   /* 0x11                       */  KEY_W,             'w',               'W',
   /* 0x12                       */  KEY_E,             'e',               'E',
   /* 0x13                       */  KEY_R,             'r',               'R',
   /* 0x14                       */  KEY_T,             't',               'T',
   /* 0x15                       */  KEY_Y,             'y',               'Y',
   /* 0x16                       */  KEY_U,             'u',               'U',
   /* 0x17                       */  KEY_I,             'i',               'I',
   /* 0x18                       */  KEY_O,             'o',               'O',
   /* 0x19                       */  KEY_P,             'p',               'P',
   /* 0x1a                       */  KEY_LEFTBRACKET,   '[',               '{',
   /* 0x1b                       */  KEY_RIGHTBRACKET,  ']',               '}',
   /* 0x1c                       */  KEY_RETURN,        '\r',              '\r',
   /* 0x1d                       */  KEY_LCTRL,         KEY_LCTRL,         KEY_LCTRL,
   /* 0x1e                       */  KEY_A,             'a',               'A',
   /* 0x1f                       */  KEY_S,             's',               'S',
   /* 0x20                       */  KEY_D,             'd',               'D',
   /* 0x21                       */  KEY_F,             'f',               'F',
   /* 0x22                       */  KEY_G,             'g',               'G',
   /* 0x23                       */  KEY_H,             'h',               'H',
   /* 0x24                       */  KEY_J,             'j',               'J',
   /* 0x25                       */  KEY_K,             'k',               'K',
   /* 0x26                       */  KEY_L,             'l',               'L',
   /* 0x27                       */  KEY_SEMICOLON,     ';',               ':',
   /* 0x28                       */  KEY_QUOTE,         '\'',              '\"',
   /* 0x29                       */  KEY_GRAVE,         '`',               '~',
   /* 0x2a                       */  KEY_LSHIFT,        KEY_LSHIFT,        KEY_LSHIFT,
   /* 0x2b                       */  KEY_BACKSLASH,     '\\',              '|',
   /* 0x2c                       */  KEY_Z,             'z',               'Z',
   /* 0x2d                       */  KEY_X,             'x',               'X',
   /* 0x2e                       */  KEY_C,             'c',               'C',
   /* 0x2f                       */  KEY_V,             'v',               'V',
   /* 0x30                       */  KEY_B,             'b',               'B',
   /* 0x31                       */  KEY_N,             'n',               'N',
   /* 0x32                       */  KEY_M,             'm',               'M',
   /* 0x33                       */  KEY_COMMA,         ',',               '<',
   /* 0x34                       */  KEY_DOT,           '.',               '>',
   /* 0x35                       */  KEY_SLASH,         '/',               '?',
   /* 0x36                       */  KEY_RSHIFT,        KEY_RSHIFT,        KEY_RSHIFT,
   /* 0x37                       */  KEY_KP_ASTERISK,   '*',               '*',
   /* 0x38                       */  KEY_RALT,          KEY_RALT,          KEY_RALT,
   /* 0x39                       */  KEY_SPACE,         ' ',               ' ',
   /* 0x3a                       */  KEY_CAPSLOCK,      KEY_CAPSLOCK,      KEY_CAPSLOCK,
   /* 0x3b                       */  KEY_F1,            KEY_F1,            KEY_F1,
   /* 0x3c                       */  KEY_F2,            KEY_F2,            KEY_F2,
   /* 0x3d                       */  KEY_F3,            KEY_F3,            KEY_F3,
   /* 0x3e                       */  KEY_F4,            KEY_F4,            KEY_F4,
   /* 0x3f                       */  KEY_F5,            KEY_F5,            KEY_F5,
   /* 0x40                       */  KEY_F6,            KEY_F6,            KEY_F6,
   /* 0x41                       */  KEY_F7,            KEY_F7,            KEY_F7,
   /* 0x42                       */  KEY_F8,            KEY_F8,            KEY_F8,
   /* 0x43                       */  KEY_F9,            KEY_F9,            KEY_F9,
   /* 0x44                       */  KEY_F10,           KEY_F10,           KEY_F10,
   /* 0x45                       */  KEY_KP_NUMLOCK,    KEY_KP_NUMLOCK,    KEY_KP_NUMLOCK,
   /* 0x46                       */  KEY_SCROLLLOCK,    KEY_SCROLLLOCK,    KEY_SCROLLLOCK,
   /* 0x47                       */  KEY_HOME,          KEY_HOME,          KEY_HOME,
   /* 0x48  //keypad up arrow    */  KEY_KP_8,          KEY_KP_8,          KEY_KP_8,
   /* 0x49                       */  KEY_PAGEUP,        KEY_PAGEUP,        KEY_PAGEUP,
   /* 0x50  //keypad down arrow  */  KEY_KP_2,          KEY_KP_2,          KEY_KP_2,
   /* 0x51  //keypad page down   */  KEY_KP_3,          KEY_KP_3,          KEY_KP_3,
   /* 0x52  //keypad insert key  */  KEY_KP_0,          KEY_KP_0,          KEY_KP_0,
   /* 0x53  //keypad delete key  */  KEY_KP_DECIMAL,    '.',               '.',
   /* 0x54                       */  KEY_UNKNOWN,       '\0',              '\0',
   /* 0x55                       */  KEY_UNKNOWN,       '\0',              '\0',
   /* 0x56                       */  KEY_UNKNOWN,       '\0',              '\0',
   /* 0x57                       */  KEY_F11,           KEY_F11,           KEY_F11,
   /* 0x58                       */  KEY_F12,           KEY_F12,           KEY_F12
};


// Harware interrupt's handler
extern void _asm_keyboard_int(void);

// Keyboard's variables
static int scancode;
static bool scrolllock = false, numlock = false, capslock = false;
static bool shift = false, alt = false, ctrl = false;

static void keyboard_setleds(bool num, bool caps, bool scroll);

void keyboard_init(void)
{
    shift = false;
    alt = false;
    ctrl = false;

    // reset LEDs
    numlock = false;
    capslock = false;
    scrolllock = false;
    keyboard_setleds(numlock, capslock, scrolllock);

    // install keyboard's ISR in IDT
    idt_descriptor_flags_t flags;
    memset(&flags, 0, sizeof(idt_descriptor_flags_t));

    flags.gate_type = IDT_DESC_TYPE_32BIT_INTERRUPT_GATE;
    flags.privilege_level = IDT_DESC_PRIVILEGE_RING_0;
    flags.present = IDT_DESC_PRESENT_IN_MEMORY;

    i86_idt_install_ir(IDT32_HARDWARE_INT_OFFSET(KEYBOARDINT), _asm_keyboard_int, flags);

    // unmask keyboard's interrupt
    uint16_t intmask = i86_pic_getintmask();
    intmask = mask_enableinterrupt(intmask, KEYBOARDINT);
    i86_pic_setintmask(intmask);
}

void keyboard_settypematics(uint8_t repeat_nchars, enum REPEAT_DELAYS repeat_delay)
{
    uint8_t typematics_option = 0;

    ((typematic_options_t*) &typematics_option)->repeat_rate = set_repeat_rate(repeat_nchars);
    ((typematic_options_t*) &typematics_option)->repeat_delay = repeat_delay;

    keyboard_enc_send_cmd(KEYBOARD_ENC_CMD_AUTODELAY);
    keyboard_enc_send_cmd(typematics_option);
}

static void keyboard_setleds(bool num, bool caps, bool scroll)
{
    uint8_t data = 0x00;

    data = (scroll) ? (data | 0x01) : (data & ~0x01);
    data = (num)    ? (data | 0x02) : (data & ~0x02);
    data = (caps)   ? (data | 0x04) : (data & ~0x04);

    keyboard_enc_send_cmd(KEYBOARD_ENC_CMD_SET_LED);
    keyboard_enc_send_cmd(data);
}

void keyboard_turnon(void)
{
    keyboard_ctrl_send_cmd(KEYBOARD_CTRL_CMD_ENABLE);
}

void keyboard_turnoff(void)
{
    keyboard_ctrl_send_cmd(KEYBOARD_CTRL_CMD_DISABLE);
}

extern volatile char driver_kbd_inputc;
extern volatile bool driver_kbd_inputavailable;

void isr_keyboard_int(void)
{
    int code = 0;

    // read scan code only if the keyboard controller output buffer is full
    if ( (keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATUS_MASK_OUT_BUF) != KB_SR_OUTPUT_BUFFER_FULL ) {
        return;
    }

    // read the scan code
    code = keyboard_enc_read_buf();

    // Original XT Scan Code Set:
    //  - make code < 0x80
    //  - break code >= 0x80
    if (code >= 0x80) {

        // convert the break code into its make code equivelant
        code -= 0x80;

        //! grab the key
        int key = keyboard_scancode_std[code * 3];

        // Update the special keys's status (CTRL, SHIFT, ALT)
        switch (key) {
            case KEY_LCTRL:
            case KEY_RCTRL:
                {
                    ctrl = false;
                    break;
                }

            case KEY_LSHIFT:
            case KEY_RSHIFT:
                {
                    shift = false;
                    break;
                }

            case KEY_LALT:
            case KEY_RALT:
                {
                    alt = false;
                    break;
                }

            default:
                {
                    // nothing to do
                    break;
                }
        }

    } else {
        scancode = code;

        int key = keyboard_scancode_std[code * 3];

        //! test if user is holding down any special keys & set it
        switch (key) {
            // special keys
            case KEY_LCTRL:
            case KEY_RCTRL:
                {
                    ctrl = true;
                    break;
                }

            case KEY_LSHIFT:
            case KEY_RSHIFT:
                {
                    shift = true;
                    break;
                }

            case KEY_LALT:
            case KEY_RALT:
                {
                    alt = true;
                    break;
                }

            // LED keys
            case KEY_CAPSLOCK:
                {
                    capslock = (capslock) ? false : true;
                    keyboard_setleds(numlock, capslock, scrolllock);
                    break;
                }

            case KEY_KP_NUMLOCK:
                {
                    numlock = (numlock) ? false : true;
                    keyboard_setleds(numlock, capslock, scrolllock);
                    break;
                }

            case KEY_SCROLLLOCK:
                {
                    scrolllock = (scrolllock) ? false : true;
                    keyboard_setleds(numlock, capslock, scrolllock);
                    break;
                }

            default:
                {
                    char c;

                    if (capslock || shift) {
                        c = keyboard_scancode_std[code * 3 + 2];
                    } else {
                        c = keyboard_scancode_std[code * 3 + 1];
                    }

                    driver_kbd_inputc = c;
                    driver_kbd_inputavailable = true;

                    break;
                }
        }
    }
}
