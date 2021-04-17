#ifndef PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED
#define PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

#include "GraphicsInternal.h"

#include "../ResourceManagement/OGL3.h"
#include "../ResourceManagement/ResourceManagerOGL3.h"

#include <SDL.h>
#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#endif
#include <OpenGL/GL.h>
#endif

namespace PGE {

class Texture;

class GraphicsOGL3 : public GraphicsInternal {
    public:
        GraphicsOGL3(const String& name,int w,int h,bool fs);

        void update() override;
        void swap() override;

        void clear(Color color) override;
    
        void setDepthTest(bool enabled) override;

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(std::vector<Texture*> renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(Rectanglei vp) override;

        void setVsync(bool isEnabled) override;

        void takeGlContext();
        SDL_GLContext getGlContext() const;

        __GFX_OBJ_DEC

    private:
        GLContext::Ref glContext;
        GLFramebuffer::Ref glFramebuffer;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED
