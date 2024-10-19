#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "renderer/Topl_Renderer.hpp"

namespace DX11 {
    // Buffer

    struct Buffer : public Topl_Buffer {
        Buffer(ID3D11Buffer* b) : Topl_Buffer(){ buffer = b; }
        Buffer(unsigned id, enum BUFF_Type t, ID3D11Buffer* b) : Topl_Buffer(id, t) { buffer = b; }
        Buffer(unsigned id, enum BUFF_Type t, ID3D11Buffer* b, unsigned c) : Topl_Buffer(id, t, c) { buffer = b; }

        ID3D11Buffer* buffer; // DirectX buffer
    };

    // Texture

    struct Texture : public Topl_Texture {
        Texture() : Topl_Texture() {}
        Texture(unsigned id, enum TEX_Frmt f, enum TEX_Mode m, ID3D11SamplerState* s, ID3D11ShaderResourceView* r) : Topl_Texture(id, f, m) { 
            sampler = s; resource = r;
        }

        Texture(unsigned id, unsigned short b, enum TEX_Frmt f, enum TEX_Mode m, ID3D11SamplerState* s, ID3D11ShaderResourceView* r) : Topl_Texture(id, b, f, m) {
            sampler = s; resource = r;
        }

        ID3D11SamplerState* sampler = nullptr;
        ID3D11ShaderResourceView* resource = nullptr;
    };

    // Functions

    bool createBuff(ID3D11Device** device, ID3D11Buffer** buffer, UINT byteWidth, D3D11_USAGE usage, UINT bindFlags, UINT cpuFlags, const void* data);
    bool createVertexBuff(ID3D11Device** device, ID3D11Buffer** vBuff, vertex_cptr_t vData, unsigned vCount);
    bool createBlockBuff(ID3D11Device** device, ID3D11Buffer** cBuff, const blockBytes_t* const blockBytes);

    bool compileShader(const char* shaderFilePath, LPCSTR shaderTarget, ID3DBlob** blob);
    DXGI_FORMAT getShaderFormat(enum SHDR_ValueType type);

    D3D11_VIEWPORT createViewport(const Topl_Viewport* const vp);
    D3D11_SAMPLER_DESC genSamplerDesc(enum TEX_Mode mode);
}