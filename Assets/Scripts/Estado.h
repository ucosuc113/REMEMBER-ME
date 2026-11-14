#pragma once

#include "Engine/Scene/GameObject.h"
#include "Engine/Scene/SceneTransform.h"
#include "Engine/Scene/ComponentQuery.h"
#include "Engine/Components/Colliders/BoxCollider3D.h"
#include "Engine/Components/Colliders/CapsuleCollider.h"
#include "Engine/Components/UI/UiComponents.h"

#include <algorithm>
#include <cmath>

namespace remember {

inline bool controlBloqueado = false;

struct Caja3 {
    float min[3];
    float max[3];
};

inline bool CajaDe(strace::GameObject& n, Caja3& out) {
    using namespace strace;
    DirectX::XMFLOAT3 p = ComputeWorldPosition(n);
    const float* s = n.transform.scale;
    if (BoxCollider3D* b = GetComponent<BoxCollider3D>(n)) {
        float c[3] = {p.x + b->center.x * s[0], p.y + b->center.y * s[1], p.z + b->center.z * s[2]};
        float h[3] = {b->size.x * 0.5f * s[0], b->size.y * 0.5f * s[1], b->size.z * 0.5f * s[2]};
        for (int i = 0; i < 3; ++i) { out.min[i] = c[i] - std::fabs(h[i]); out.max[i] = c[i] + std::fabs(h[i]); }
        return true;
    }
    if (CapsuleCollider* k = GetComponent<CapsuleCollider>(n)) {
        float c[3] = {p.x + k->center.x * s[0], p.y + k->center.y * s[1], p.z + k->center.z * s[2]};
        float h[3] = {k->radius * s[0], k->height * 0.5f * s[1], k->radius * s[2]};
        for (int i = 0; i < 3; ++i) { out.min[i] = c[i] - std::fabs(h[i]); out.max[i] = c[i] + std::fabs(h[i]); }
        return true;
    }
    return false;
}

inline bool Tocan(const Caja3& a, const Caja3& b) {
    for (int i = 0; i < 3; ++i)
        if (a.max[i] <= b.min[i] || b.max[i] <= a.min[i]) return false;
    return true;
}

inline void Pintar(strace::GameObject* panel, float r, float g, float b, float a) {
    if (!panel) return;
    if (strace::UiPanel* p = strace::GetComponent<strace::UiPanel>(*panel)) p->color = strace::UiColor::Rgba(r, g, b, a);
}

inline float Suave(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

}