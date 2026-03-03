#include "engine/UI.hpp"

namespace CE::UI::Widgets {

Button::Button(int x, int y, int w, int h, 
               Color inactive, Color hover, Color clicked, Color textColour, std::string text)
{
    PosX = x;
    PosY = y;
    width = w;
    height = h;
    Inactive = inactive;
    Hover = hover;
    Clicked = clicked;
    TextColour = textColour;
    Text = text;

    rec = { (float)PosX, (float)PosY, (float)width, (float)height };
    isHovering = false;
    isClicked = false;
}

void Button::update() {
    Vector2 MousePos = GetMousePosition();
    isHovering = CheckCollisionPointRec(MousePos, rec);
    isClicked = isHovering && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (isClicked) {
        DrawRectangleRec(rec, Clicked);
    } else if (isHovering) {
        DrawRectangleRec(rec, Hover);
    } else {
        DrawRectangleRec(rec, Inactive);
    }

    int TextWidth = MeasureText(Text.c_str(), 20);
    int TextHeight = 20;
    DrawText(Text.c_str(), PosX + (width - TextWidth)/2, PosY + (height - TextHeight)/2, 20, TextColour);
}

bool Button::hovering() {
    return isHovering;
}

bool Button::clicked() {
    return isClicked;
}

} 
