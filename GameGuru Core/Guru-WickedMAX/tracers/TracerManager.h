#pragma once

#include "wiGraphicsDevice.h"
#include "wiScene.h"
#include "wiRenderer.h"
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
using namespace wiGraphics;

namespace Tracers
{

    struct Tracer
    {
        XMFLOAT3 startPos;
        XMFLOAT3 endPos;
        float spawnTime;
        float lifeTime;
        XMFLOAT4 color;
        float glowIntensity;
        float scrollSpeed;
        float scaleV;
        float width;
        float max_length;
        uint32_t tracerID = 0;
    };

    void Initialize();
    void AddTracer(const XMFLOAT3& start, const XMFLOAT3& end, float lifeTime, XMFLOAT4 color, float glow, float scroll,float scaleV, float width = 0.5f, float max_length = 0, uint32_t tracerID = 0);
    void Update();
    void LoadTracerImage(char* filename, uint32_t gunid);
    void Draw(CommandList cmd, const XMMATRIX& viewProj);
    extern "C" void tracer_draw(const wiScene::CameraComponent& camera, wiGraphics::CommandList cmd);

    struct TracerCB
    {
        XMMATRIX g_mWorldViewProj;
        XMFLOAT4 g_TintColor;
        float g_GlowIntensity;
        float g_ScrollSpeed;
        float g_Time;
        float g_ScaleV;
    };
    void CreateQuad();
    void CreatePipelineState();

}