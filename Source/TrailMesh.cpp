#include "TrailMesh.h"
#include <cassert>
#include <cstdio>

void TrailMesh::AddNode(const DirectX::XMFLOAT3& top, const DirectX::XMFLOAT3& bottom)
{
    nodes.push_back({ top, bottom });
    if (nodes.size() > maxNodes) nodes.erase(nodes.begin());
}

HRESULT TrailMesh::Initialize(ID3D11Device* device, size_t maxNodes_)
{
    if (!device) return E_POINTER;

    // min 2 を保証（0 や 1 は IB サイズが 0 になる）
    maxNodes = std::max<size_t>(2, maxNodes_);

    vertexCount = (UINT)(maxNodes * 2);
    indexCount = (UINT)((maxNodes - 1) * 6);

    vbSize = vertexCount * sizeof(TrailVertex);
    ibSize = indexCount * sizeof(UINT);

    // --- Vertex Buffer (Dynamic, Map 書き換え用) ---
    D3D11_BUFFER_DESC vbd{};
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = (UINT)vbSize;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;

    HRESULT hr = device->CreateBuffer(&vbd, nullptr, vb.GetAddressOf());
    if (FAILED(hr) || !vb)
    {
        OutputDebugStringA("TrailMesh::Initialize - CreateBuffer(VB) failed\n");
        return hr;
    }

    // --- Index Buffer (Immutable, 固定インデックスを最初に作る) ---
    std::vector<UINT> indices(indexCount);
    // fill indices for max capacity
    for (UINT i = 0; i < maxNodes - 1; ++i)
    {
        UINT base = i * 2;
        indices[i * 6 + 0] = base + 0;
        indices[i * 6 + 1] = base + 1;
        indices[i * 6 + 2] = base + 2;

        indices[i * 6 + 3] = base + 2;
        indices[i * 6 + 4] = base + 1;
        indices[i * 6 + 5] = base + 3;
    }

    D3D11_BUFFER_DESC ibd{};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = (UINT)ibSize;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = indices.data();

    hr = device->CreateBuffer(&ibd, &init, ib.GetAddressOf());
    if (FAILED(hr) || !ib)
    {
        OutputDebugStringA("TrailMesh::Initialize - CreateBuffer(IB) failed\n");
        vb.Reset();
        return hr;
    }

    // 初期 currentIndexCount は 0
    currentIndexCount = 0;

    return S_OK;
}

void TrailMesh::UpdateMesh(ID3D11DeviceContext* ctx)
{
    if (!ctx) return;
    if (!vb || !ib)
    {
        OutputDebugStringA("TrailMesh::UpdateMesh: vb or ib is NULL\n");
        return;
    }

    if (nodes.size() < 2)
    {
        currentIndexCount = 0;
        return;
    }

    const int count = (int)nodes.size();
    // Map VB
    D3D11_MAPPED_SUBRESOURCE mappedV;
    HRESULT hr = ctx->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedV);
    if (FAILED(hr))
    {
        OutputDebugStringA("TrailMesh::UpdateMesh: Map(VB) failed\n");
        return;
    }

    TrailVertex* v = reinterpret_cast<TrailVertex*>(mappedV.pData);

    for (int i = 0; i < count; ++i)
    {
        float t = i / float(count - 1);
        v[i * 2 + 0].pos = nodes[i].top;
        v[i * 2 + 0].uv = DirectX::XMFLOAT2(0.f, t);
        v[i * 2 + 1].pos = nodes[i].bottom;
        v[i * 2 + 1].uv = DirectX::XMFLOAT2(1.f, t);
    }

    ctx->Unmap(vb.Get(), 0);

    // IB は Immutable にしてあるので更新不要（容量は最大分の索引が入ってる）
    currentIndexCount = (UINT)((count - 1) * 6);
}

void TrailMesh::Render(ID3D11DeviceContext* ctx)
{
    if (!ctx) return;
    if (!vb || !ib) return;
    if (currentIndexCount == 0) return;

    UINT stride = sizeof(TrailVertex);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
    ctx->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 描画前に適切なシェーダ・入力レイアウト・ラスタライザ・ブレンド状態をセットしておいてください
    ctx->DrawIndexed(currentIndexCount, 0, 0);
}
