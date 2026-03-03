#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

// Alphanumeric keys
#define KEY_NULL            ((uint32_t)0)
#define KEY_APOSTROPHE      ((uint32_t)39)
#define KEY_COMMA           ((uint32_t)44)
#define KEY_MINUS           ((uint32_t)45)
#define KEY_PERIOD          ((uint32_t)46)
#define KEY_SLASH           ((uint32_t)47)
#define KEY_ZERO            ((uint32_t)48)
#define KEY_ONE             ((uint32_t)49)
#define KEY_TWO             ((uint32_t)50)
#define KEY_THREE           ((uint32_t)51)
#define KEY_FOUR            ((uint32_t)52)
#define KEY_FIVE            ((uint32_t)53)
#define KEY_SIX             ((uint32_t)54)
#define KEY_SEVEN           ((uint32_t)55)
#define KEY_EIGHT           ((uint32_t)56)
#define KEY_NINE            ((uint32_t)57)
#define KEY_SEMICOLON       ((uint32_t)59)
#define KEY_EQUAL           ((uint32_t)61)
#define KEY_A               ((uint32_t)65)
#define KEY_B               ((uint32_t)66)
#define KEY_C               ((uint32_t)67)
#define KEY_D               ((uint32_t)68)
#define KEY_E               ((uint32_t)69)
#define KEY_F               ((uint32_t)70)
#define KEY_G               ((uint32_t)71)
#define KEY_H               ((uint32_t)72)
#define KEY_I               ((uint32_t)73)
#define KEY_J               ((uint32_t)74)
#define KEY_K               ((uint32_t)75)
#define KEY_L               ((uint32_t)76)
#define KEY_M               ((uint32_t)77)
#define KEY_N               ((uint32_t)78)
#define KEY_O               ((uint32_t)79)
#define KEY_P               ((uint32_t)80)
#define KEY_Q               ((uint32_t)81)
#define KEY_R               ((uint32_t)82)
#define KEY_S               ((uint32_t)83)
#define KEY_T               ((uint32_t)84)
#define KEY_U               ((uint32_t)85)
#define KEY_V               ((uint32_t)86)
#define KEY_W               ((uint32_t)87)
#define KEY_X               ((uint32_t)88)
#define KEY_Y               ((uint32_t)89)
#define KEY_Z               ((uint32_t)90)
#define KEY_LEFT_BRACKET    ((uint32_t)91)
#define KEY_BACKSLASH       ((uint32_t)92)
#define KEY_RIGHT_BRACKET   ((uint32_t)93)
#define KEY_GRAVE           ((uint32_t)96)

// Function keys
#define KEY_SPACE           ((uint32_t)32)
#define KEY_ESCAPE          ((uint32_t)256)
#define KEY_ENTER           ((uint32_t)257)
#define KEY_TAB             ((uint32_t)258)
#define KEY_BACKSPACE       ((uint32_t)259)
#define KEY_INSERT          ((uint32_t)260)
#define KEY_DELETE          ((uint32_t)261)
#define KEY_RIGHT           ((uint32_t)262)
#define KEY_LEFT            ((uint32_t)263)
#define KEY_DOWN            ((uint32_t)264)
#define KEY_UP              ((uint32_t)265)
#define KEY_PAGE_UP         ((uint32_t)266)
#define KEY_PAGE_DOWN       ((uint32_t)267)
#define KEY_HOME            ((uint32_t)268)
#define KEY_END             ((uint32_t)269)
#define KEY_CAPS_LOCK       ((uint32_t)280)
#define KEY_SCROLL_LOCK     ((uint32_t)281)
#define KEY_NUM_LOCK        ((uint32_t)282)
#define KEY_PRINT_SCREEN    ((uint32_t)283)
#define KEY_PAUSE           ((uint32_t)284)
#define KEY_F1              ((uint32_t)290)
#define KEY_F2              ((uint32_t)291)
#define KEY_F3              ((uint32_t)292)
#define KEY_F4              ((uint32_t)293)
#define KEY_F5              ((uint32_t)294)
#define KEY_F6              ((uint32_t)295)
#define KEY_F7              ((uint32_t)296)
#define KEY_F8              ((uint32_t)297)
#define KEY_F9              ((uint32_t)298)
#define KEY_F10             ((uint32_t)299)
#define KEY_F11             ((uint32_t)300)
#define KEY_F12             ((uint32_t)301)
#define KEY_LEFT_SHIFT      ((uint32_t)340)
#define KEY_LEFT_CONTROL    ((uint32_t)341)
#define KEY_LEFT_ALT        ((uint32_t)342)
#define KEY_LEFT_SUPER      ((uint32_t)343)
#define KEY_RIGHT_SHIFT     ((uint32_t)344)
#define KEY_RIGHT_CONTROL   ((uint32_t)345)
#define KEY_RIGHT_ALT       ((uint32_t)346)
#define KEY_RIGHT_SUPER     ((uint32_t)347)
#define KEY_KB_MENU         ((uint32_t)348)

// Keypad keys
#define KEY_KP_0            ((uint32_t)320)
#define KEY_KP_1            ((uint32_t)321)
#define KEY_KP_2            ((uint32_t)322)
#define KEY_KP_3            ((uint32_t)323)
#define KEY_KP_4            ((uint32_t)324)
#define KEY_KP_5            ((uint32_t)325)
#define KEY_KP_6            ((uint32_t)326)
#define KEY_KP_7            ((uint32_t)327)
#define KEY_KP_8            ((uint32_t)328)
#define KEY_KP_9            ((uint32_t)329)
#define KEY_KP_DECIMAL      ((uint32_t)330)
#define KEY_KP_DIVIDE       ((uint32_t)331)
#define KEY_KP_MULTIPLY     ((uint32_t)332)
#define KEY_KP_SUBTRACT     ((uint32_t)333)
#define KEY_KP_ADD          ((uint32_t)334)
#define KEY_KP_ENTER        ((uint32_t)335)
#define KEY_KP_EQUAL        ((uint32_t)336)

// Android buttons
#define KEY_BACK            ((uint32_t)4)
#define KEY_MENU            ((uint32_t)5)
#define KEY_VOLUME_UP       ((uint32_t)24)
#define KEY_VOLUME_DOWN     ((uint32_t)25)

struct Globals {

};

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

Globals* gGlobal = nullptr;
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