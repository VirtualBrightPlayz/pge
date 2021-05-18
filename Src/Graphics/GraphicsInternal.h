#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <Graphics/Graphics.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

#include <Misc/FilePath.h>
#include <Mesh/Mesh.h>
#include <Texture/Texture.h>

#define PGE_GFX_OBJ_DEC \
Shader* loadShader(const FilePath& path) override; \
Mesh* createMesh(Primitive::Type pt) override; \
Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) override; \
Texture* loadTexture(int w, int h, byte* buffer, Texture::Format fmt) override; \
String getRendererName() const override;

#define PGE_GFX_OBJ_DEF(GfxType) \
Shader* Graphics ## GfxType ## ::loadShader(const FilePath& path) { \
    return new Shader ## GfxType ## (this, path); \
} \
\
Mesh* Graphics ## GfxType ## ::createMesh(Primitive::Type pt) { \
    return new Mesh ## GfxType ## (this, pt); \
} \
\
Texture* Graphics ## GfxType ## ::createRenderTargetTexture(int w, int h, Texture::Format fmt) { \
    return new Texture ## GfxType ## (this, w, h, fmt); \
} \
\
Texture* Graphics ## GfxType ## ::loadTexture(int w, int h, byte* buffer, Texture::Format fmt) { \
    return new Texture ## GfxType ## (this, w, h, buffer, fmt); \
} \
\
String Graphics ## GfxType ## ::getRendererName() const { \
    return #GfxType; \
}

struct SDL_Window;

namespace PGE {

class Shader;
class ThreadManager;

class GraphicsInternal : public Graphics {
    public:
        using Graphics::Graphics;

        SDL_Window* getSdlWindow() const;
#if defined(__APPLE__) && defined(__OBJC__)
        NSWindow* getCocoaWindow() const;
#endif

        virtual Shader* loadShader(const FilePath& path) = 0;
        virtual Mesh* createMesh(Primitive::Type pt) = 0;
        virtual Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) = 0;
        virtual Texture* loadTexture(int w, int h, byte* buffer, Texture::Format fmt) = 0;
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
