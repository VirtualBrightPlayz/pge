#ifndef PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
#define PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#include <Texture/Texture.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "../ResourceManagement/DX11.h"

namespace PGE {

class TextureDX11 : public Texture {
    public:
        // Render target.
        TextureDX11(Graphics* gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureDX11(Graphics* gfx, int w, int h, byte* buffer, Format fmt);

        void useTexture(int index);

        Texture* copy() const override;

        ID3D11RenderTargetView* getRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;
        void* getNative() const override;

    private:
        D3D11Texture2D::View dxTexture;
        D3D11ShaderResourceView::View dxShaderResourceView;

        D3D11RenderTargetView::View dxRtv;
        D3D11Texture2D::View dxZBufferTexture;
        D3D11DepthStencilView::View dxZBufferView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
