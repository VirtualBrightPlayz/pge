#include <Graphics/Graphics.h>

using namespace PGE;

Graphics::Graphics() {
    
}

Window* Graphics::getWindow() const {
    return window;
}

void Graphics::update() {
    window->update();
}

void Graphics::swap(bool vsync) {
    window->swap(vsync);
}
