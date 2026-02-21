#pragma once

#pragma once
#include <string>

extern "C" {
    #include <raylib.h>
}

namespace CE::UI::Widgets {

class Button {
public:
    int width;
    int height;
    int PosX;
    int PosY;

    Color Inactive;
    Color Hover;
    Color Clicked;
    Color TextColour; 
    std::string Text;

    Rectangle rec;
    bool isHovering;   
    bool isClicked;     

    // Constructor
    Button(int x, int y, int w, int h, 
           Color inactive = PURPLE, 
           Color hover = PINK, 
           Color clicked = VIOLET,
           Color textColour = BLACK,
           std::string text = "NO TEXT ASKED, FIX ME");

    void update();

    bool hovering();

    bool clicked();
};

} 
