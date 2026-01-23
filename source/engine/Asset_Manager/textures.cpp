#include <vector>
#include <string>

#include "engine/assets/assets.hpp"

extern "C" {
    #include <raylib.h> // I can't be bothered to deal with raylib CPP C is fine ;-;
}

namespace CE::Assets {

    Texture::Texture(const std::string& filepath) : name(filepath) { 
        if(!FileExists(filepath.c_str())) { // If the file path doesn't exist we show an error texture
            TraceLog(LOG_ERROR, "CE-Textures: %s does not exist", filepath.c_str());
            Image img = GenImageChecked(64, 64, 8, 8, MAGENTA, BLACK);
            tex = LoadTextureFromImage(img);
            UnloadImage(img);
        } else {
            tex = LoadTexture(filepath.c_str()); // Load normally if it exists
        }
    }

    Texture::~Texture() { // Unload texture IF its not zeros
        if (tex.id != 0) {
            UnloadTexture(tex);
            TraceLog(LOG_INFO, "CE-Textures: Unloaded %s", name.c_str());
        }
    }

    Texture::Texture(Texture&& other) noexcept : tex(other.tex), name(std::move(other.name)) { // used to safely transfer ownership
        other.tex = {};
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (tex.id != 0) UnloadTexture(tex);
        tex = other.tex;
        other.tex = {};
        name = std::move(other.name);
        return *this;
    }

    void Texture::draw(int x, int y, Color tint) const {
        if (tex.id != 0) DrawTexture(tex, x, y, tint);
    }

    Texture::operator Texture2D() const { // Let you access texture 2d directly 
        return tex;
    }

} 
