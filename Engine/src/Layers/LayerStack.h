#pragma once

#include <set>
#include <memory>

#include "BaseLayer.h"

class LayerStack
{
public:
    LayerStack() = default;
    ~LayerStack();

    void AttachLayer(const std::shared_ptr<BaseLayer>& layer);
    bool DetachLayer(const std::shared_ptr<BaseLayer>& layer);

    void UpdateLayers(const std::shared_ptr<FrameTime>& frameTime) const;
    void DrawLayers() const;

    void ClearLayers();

private:
    struct LayerComparison
    {
        bool operator()(const std::shared_ptr<BaseLayer>& lhs, const std::shared_ptr<BaseLayer>& rhs) const
        {
            return lhs->GetLayerIndex() < rhs->GetLayerIndex();
        }
    };

    std::set<std::shared_ptr<BaseLayer>, LayerComparison> _layers;
};