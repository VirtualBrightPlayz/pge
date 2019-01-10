#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <Window/Window.h>
#include <Math/Rectangle.h>
#include <Math/Matrix.h>
#include <Color/Color.h>

namespace PGE {

class Graphics {
    public:
        static Graphics* create(int w, int h, bool fs);

        Window* getWindow() const;
        
        virtual void update();
        void swap(bool vsync=true);

        virtual void clear(Color color) =0;

        //TODO: implement general matrix container
        Matrix4x4f projectionMatrix;
        Matrix4x4f viewMatrix;

        Rectanglei viewport;

        ~Graphics(){};
    protected:
        Graphics();

        Window* window;
};

}

#endif
