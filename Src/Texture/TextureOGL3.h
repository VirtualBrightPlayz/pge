#ifndef PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
#define PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

#include <Texture/Texture.h>
#include <Threading/ThreadManager.h>

#include "../ResourceManagement/OGL3.h"
#include "../ResourceManagement/ResourceManagerOGL3.h"

namespace PGE {

class TextureOGL3 : public Texture {
    public:
        TextureOGL3(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer, FORMAT fmt);
        TextureOGL3(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager);
        TextureOGL3(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn);

        virtual Texture* copy() const override;

        GLuint getGlTexture() const;
        //GLuint getGlFramebuffer() const;
        GLuint getGlDepthbuffer() const;
        virtual void* getNative() const;

    private:
        GLTexture::Ref glTexture;
        //GLuint glFramebuffer;
        GLDepthBuffer::Ref glDepthbuffer;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
