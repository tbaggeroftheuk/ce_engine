#include <cfloat>

CE::Plugins::Globals* gGlobal = nullptr;

namespace CE::Plugins {
    struct Globals {

    };
}

namespace CE::Input {
    enum Keys : uint32_t   {
        // Alphanumeric keys
        KEY_NULL            = 0,
        KEY_APOSTROPHE      = 39,
        KEY_COMMA           = 44,
        KEY_MINUS           = 45,
        KEY_PERIOD          = 46,
        KEY_SLASH           = 47,
        KEY_ZERO            = 48,
        KEY_ONE             = 49,
        KEY_TWO             = 50,
        KEY_THREE           = 51,
        KEY_FOUR            = 52,
        KEY_FIVE            = 53,
        KEY_SIX             = 54,
        KEY_SEVEN           = 55,
        KEY_EIGHT           = 56,
        KEY_NINE            = 57,
        KEY_SEMICOLON       = 59,
        KEY_EQUAL           = 61,
        KEY_A               = 65,
        KEY_B               = 66,
        KEY_C               = 67,
        KEY_D               = 68,
        KEY_E               = 69,
        KEY_F               = 70,
        KEY_G               = 71,
        KEY_H               = 72,
        KEY_I               = 73,
        KEY_J               = 74,
        KEY_K               = 75,
        KEY_L               = 76,
        KEY_M               = 77,
        KEY_N               = 78,
        KEY_O               = 79,
        KEY_P               = 80,
        KEY_Q               = 81,
        KEY_R               = 82,
        KEY_S               = 83,
        KEY_T               = 84,
        KEY_U               = 85,
        KEY_V               = 86,
        KEY_W               = 87,
        KEY_X               = 88,
        KEY_Y               = 89,
        KEY_Z               = 90,
        KEY_LEFT_BRACKET    = 91,
        KEY_BACKSLASH       = 92,
        KEY_RIGHT_BRACKET   = 93,
        KEY_GRAVE           = 96,

        // Function keys
        KEY_SPACE           = 32,
        KEY_ESCAPE          = 256,
        KEY_ENTER           = 257,
        KEY_TAB             = 258,
        KEY_BACKSPACE       = 259,
        KEY_INSERT          = 260,
        KEY_DELETE          = 261,
        KEY_RIGHT           = 262,
        KEY_LEFT            = 263,
        KEY_DOWN            = 264,
        KEY_UP              = 265,
        KEY_PAGE_UP         = 266,
        KEY_PAGE_DOWN       = 267,
        KEY_HOME            = 268,
        KEY_END             = 269,
        KEY_CAPS_LOCK       = 280,
        KEY_SCROLL_LOCK     = 281,
        KEY_NUM_LOCK        = 282,
        KEY_PRINT_SCREEN    = 283,
        KEY_PAUSE           = 284,
        KEY_F1              = 290,
        KEY_F2              = 291,
        KEY_F3              = 292,
        KEY_F4              = 293,
        KEY_F5              = 294,
        KEY_F6              = 295,
        KEY_F7              = 296,
        KEY_F8              = 297,
        KEY_F9              = 298,
        KEY_F10             = 299,
        KEY_F11             = 300,
        KEY_F12             = 301,
        KEY_LEFT_SHIFT      = 340,
        KEY_LEFT_CONTROL    = 341,
        KEY_LEFT_ALT        = 342,
        KEY_LEFT_SUPER      = 343,
        KEY_RIGHT_SHIFT     = 344,
        KEY_RIGHT_CONTROL   = 345,
        KEY_RIGHT_ALT       = 346,
        KEY_RIGHT_SUPER     = 347,
        KEY_KB_MENU         = 348,

        // Keypad keys
        KEY_KP_0            = 320,
        KEY_KP_1            = 321,
        KEY_KP_2            = 322,
        KEY_KP_3            = 323,
        KEY_KP_4            = 324,
        KEY_KP_5            = 325,
        KEY_KP_6            = 326,
        KEY_KP_7            = 327,
        KEY_KP_8            = 328,
        KEY_KP_9            = 329,
        KEY_KP_DECIMAL      = 330,
        KEY_KP_DIVIDE       = 331,
        KEY_KP_MULTIPLY     = 332,
        KEY_KP_SUBTRACT     = 333,
        KEY_KP_ADD          = 334,
        KEY_KP_ENTER        = 335,
        KEY_KP_EQUAL        = 336,

        // Android buttons
        KEY_BACK            = 4,
        KEY_MENU            = 5,
        KEY_VOLUME_UP       = 24,
        KEY_VOLUME_DOWN     = 25
    };
}

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#if defined(_WIN32) || defined(_WIN64)
    #define CE_EXPORT extern "C" __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
    #define CE_EXPORT extern "C" __attribute__((visibility("default")))
#else
    #define CE_EXPORT extern "C"
#endif

#define LOG_INFO ((uint32_t)1)
#define LOG_DEBUG ((uint32_t)2)
#define LOG_WARN ((uint32_t)3)
#define LOG_ERROR ((uint32_t)4)

typedef float f32;
static_assert(sizeof(f32) == 4, "f32 must be 32-bit");
static_assert(FLT_RADIX == 2);
static_assert(FLT_MANT_DIG == 24);

// Crap I've stolen from raylib.h

typedef struct Vector2 {
    f32 x;
    f32 y;
};

// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} Color;

// Stuff that is not stolen from raylib.h

struct CE_Funcs {
    uint32_t Version;
    
    // Texture functions
    void (*TexturesDraw)(const char* path, uint32_t posX, uint32_t posY);
    void (*TexturesUnload)(const char* name);
    void (*TexturesUnloadAll)(void);
    
    // Keyboard functions (CE prefix)
    uint32_t (*CeIsKeyPressed)(uint32_t key);
    uint32_t (*CeIsKeyPressedRepeat)(uint32_t key);
    uint32_t (*CeIsKeyDown)(uint32_t key);
    uint32_t (*CeIsKeyReleased)(uint32_t key);
    uint32_t (*CeIsKeyUp)(uint32_t key);
    uint32_t (*CeGetKeyPressed)(void);
    uint32_t (*CeGetCharPressed)(void);

    // Logging helper
    void (*Log)(uint32_t level, const char* message);
};

CE_Funcs* gFunc = nullptr;

typedef struct CE_PluginInfo {
    uint32_t StructSize;

    uint32_t Plugin_Version_Major;
    uint32_t Plugin_Version_Minor;

    uint32_t CE_Engine_Version_Major;
    uint32_t CE_Engine_version_Minor;

    const char* Name;
    const char* Description;

    uint32_t Capabilities;

} CE_PluginInfo;

enum CE_Capabilities : uint32_t {
    CE_Lua_Injects   = 1 << 0,
    CE_InGame        = 1 << 1,
    CE_UI_Main_Menu  = 1 << 2,
    CE_UI_InGame     = 1 << 3,
    CE_UI_Pause_Menu = 1 << 4
};

#ifdef __cplusplus
}
#endif