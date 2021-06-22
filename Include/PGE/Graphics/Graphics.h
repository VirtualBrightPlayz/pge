#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <vector>
#include <list>

#include "../../../Src/ResourceManagement/Misc.h"

#include <PGE/Math/Rectangle.h>
#include <PGE/Color/Color.h>
#include <PGE/SysEvents/SysEvents.h>

struct SDL_Window;

namespace PGE {

class Texture;

class Graphics {
    public:
        enum class Renderer {
            OpenGL,
            DirectX11,
            Default
        };

        static Graphics* create(const String& name="PGE Application", int w=1280, int h=720, bool fs=false, Renderer r=Renderer::Default);

        static const std::list<Graphics*>& getActiveInstances();

        Graphics(const String& name, int w, int h, bool fs, u32 windowFlags);
        virtual ~Graphics();
        
        virtual void update();
        virtual void swap() = 0;

        virtual void clear(const Color& color) = 0;

        virtual void setRenderTarget(Texture* renderTarget) = 0;
        virtual void setRenderTargets(const std::vector<Texture*>& renderTargets) = 0;
        virtual void resetRenderTarget() = 0;

        virtual void setViewport(const Rectanglei& vp) = 0;
        const Rectanglei& getViewport() const;

        const Vector2i& getDimensions() const; float getAspectRatio() const;

        bool isWindowOpen() const;
        bool isWindowFocused() const;

        virtual void setDepthTest(bool isEnabled);
        virtual bool getDepthTest() const;

        virtual void setVsync(bool isEnabled);
        virtual bool getVsync() const;

        enum class Culling {
            BACK,
            FRONT,
            NONE
        };
        virtual void setCulling(Culling mode);
        virtual Culling getCulling() const;

        String getInfo() const;
        virtual String getRendererName() const = 0;

    protected:
        String caption;

        Rectanglei viewport;

        Vector2i dimensions; float aspectRatio;
        bool fullscreen;

        bool open;
        bool focused;

        bool depthTest;
        bool vsync;
        Culling cullingMode;

        // Base class always automatically takes care of SysEvents and the window.
        WindowEventSubscriber::View eventSubscriber;
        SDLWindow::View sdlWindow;

    private:
        ResourceManager resourceManager;
        static std::list<Graphics*> activeGraphics;
};

}

#endif // PGE_GRAPHICS_H_INCLUDED
