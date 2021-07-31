#ifdef _WIN32
#include "GraphicsDX11.h"
#endif
#include "GraphicsOGL3.h"

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#include <PGE/SDL_syswm.h>
#endif

using namespace PGE;

String GraphicsInternal::appendInfoLine(const String& name, int value) {
    return "\n" + name + ": " + String::fromInt(value);
}

String GraphicsInternal::appendInfoLine(const String& name, bool value) {
    return "\n" + name + ": " + (value ? "true" : "false");
}

#if defined(__APPLE__) && defined(__OBJC__)
NSWindow* GraphicsInternal::getCocoaWindow() const {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(getSdlWindow(), &info);

    return info.info.cocoa.window;
}
#endif

GraphicsInternal::GraphicsInternal(const String& rendererName, const String& name, int w, int h, bool fs, SDL_WindowFlags windowFlags)
    : Graphics(name, w, h, fs), RENDERER_NAME(rendererName) {
    sdlWindow = resourceManager.addNewResource<SDLWindow>(name, w, h, windowFlags);
}

String GraphicsInternal::getInfo() const {
    return caption + " (" + RENDERER_NAME + ") "
        + String::fromInt(dimensions.x) + 'x' + String::fromInt(dimensions.y) + " / "
        + String::fromInt(viewport.width()) + 'x' + String::fromInt(viewport.height())
        + appendInfoLine("open", open)
        + appendInfoLine("focused", focused)
        + appendInfoLine("fullscreen", fullscreen)
        + appendInfoLine("vsync enabled", vsync)
        + appendInfoLine("depth test enabled", depthTest);
}

SDL_Window* GraphicsInternal::getWindow() const {
    return sdlWindow;
}

GraphicsInternal::SDLWindow::SDLWindow(const String& title, int width, int height, u32 flags) {
    resource = SDL_CreateWindow(title.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    PGE_ASSERT(resource != nullptr, "Failed to create SDL window (SDLERROR: " + String(SDL_GetError()) + ")");
}

GraphicsInternal::SDLWindow::~SDLWindow() {
    SDL_DestroyWindow(resource);
}

Graphics* Graphics::create(const String& name, int w, int h, bool fs, Renderer r) {
    if (r == Renderer::Default) {
#ifdef _WIN32
        r = Renderer::DirectX11;
#else
        r = Renderer::OpenGL;
#endif
    }
    Graphics* gfx;
    switch (r) {
#ifdef _WIN32
        case Renderer::DirectX11: {
            gfx = new GraphicsDX11(name, w, h, fs);
        } break;
#endif
        case Renderer::OpenGL: {
            gfx = new GraphicsOGL3(name, w, h, fs);
        } break;
        default: {
            gfx = nullptr;
        } break;
    }
    return gfx;
}

Shader* Shader::load(Graphics* gfx, const FilePath& path) {
    return ((GraphicsInternal*)gfx)->loadShader(path);
}

Mesh* Mesh::create(Graphics* gfx) {
    return ((GraphicsInternal*)gfx)->createMesh();
}

Texture* Texture::createRenderTarget(Graphics* gfx, int w, int h, Format fmt) {
    return ((GraphicsInternal*)gfx)->createRenderTargetTexture(w, h, fmt);
}

Texture* Texture::createBlank(Graphics* gfx, int w, int h, Format fmt, bool mipmaps) {
    std::vector<byte> bufferData = std::vector<byte>(w * h * 4, 0);
    return ((GraphicsInternal*)gfx)->loadTexture(w, h, bufferData.data(), fmt, mipmaps);
}

Texture* Texture::load(Graphics* gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps) {
    PGE_ASSERT(buffer != nullptr, "Tried to load texture from nullptr");
    return ((GraphicsInternal*)gfx)->loadTexture(w, h, buffer, fmt, mipmaps);
}
