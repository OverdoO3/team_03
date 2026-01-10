#pragma once
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <algorithm>

using Microsoft::WRL::ComPtr;

class TrailMesh
{
private:
    struct TrailNode { DirectX::XMFLOAT3 top; DirectX::XMFLOAT3 bottom; };
    struct TrailVertex { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 uv; };

    std::vector<TrailNode> nodes;
    size_t maxNodes = 30;

    ComPtr<ID3D11Buffer> vb;
    ComPtr<ID3D11Buffer> ib;

    size_t vbSize = 0;
    size_t ibSize = 0;
    UINT vertexCount = 0;
    UINT indexCount = 0;
    UINT currentIndexCount = 0;

public:
    void AddNode(const DirectX::XMFLOAT3& top, const DirectX::XMFLOAT3& bottom);

    // Initialize ÇÕ HRESULT Çï‘Ç∑Åié∏îsÇèEÇ¶ÇÈÇÊÇ§Ç…Åj
    HRESULT Initialize(ID3D11Device* device, size_t maxNodes_);
    void UpdateMesh(ID3D11DeviceContext* ctx);
    void Render(ID3D11DeviceContext* ctx);
};
