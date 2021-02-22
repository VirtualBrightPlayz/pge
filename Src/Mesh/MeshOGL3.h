#ifndef PGEINTERNAL_MESHOGL3_H_INCLUDED
#define PGEINTERNAL_MESHOGL3_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Mesh/Mesh.h>

#include <vector>

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

#include "../Misc/SmartPrimitive.h"

namespace PGE {

class MeshOGL3 : public Mesh {
    public:
        MeshOGL3(Graphics* gfx, Primitive::TYPE pt);

        virtual void updateInternalData() override;

        virtual void render() override;
    private:
        virtual void uploadInternalData() override;

        SmartRef<GLuint> glVertexBufferObject;
        SmartRef<GLuint> glIndexBufferObject;

        SmartRef<GLuint> glVertexArrayObject;

        SmartOrderedDestructor destructor = 3;

        std::vector<uint8_t> glVertexData;
        std::vector<GLuint> glIndexData;
};

}

#endif // PGEINTERNAL_MESHOGL3_H_INCLUDED
