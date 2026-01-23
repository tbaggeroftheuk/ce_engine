#pragma once
#include <string>

extern "C" { 
    #include "raylib.h" 
}

namespace CE::Assets {

class Texture {
private:
    Texture2D tex{};
    std::string name;

public:
    Texture(const std::string& filepath);

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void draw(int x, int y, Color tint = WHITE) const;

    operator Texture2D() const;
};

}
