#include "LayerStack.h"

LayerStack::~LayerStack()
{
    ClearLayers();
}

void LayerStack::AttachLayer(const std::shared_ptr<BaseLayer>& layer)
{
    DetachLayer(layer);
    _layers.emplace(layer);
    layer->OnAttach();
}

bool LayerStack::DetachLayer(const std::shared_ptr<BaseLayer>& layer)
{
    std::set<std::shared_ptr<BaseLayer>>::iterator existingLayerIt = _layers.find(layer);
    if (existingLayerIt == _layers.end()) {
        return false;
    }

    _layers.erase(existingLayerIt);
    layer->OnDettach();
    return true;
}

void LayerStack::UpdateLayers(const std::shared_ptr<FrameTime>& frameTime) const
{
    for (const std::shared_ptr<BaseLayer>& layer : _layers) {
        layer->Update(frameTime);
    }
}

void LayerStack::DrawLayers() const
{
    for (const std::shared_ptr<BaseLayer>& layer : _layers) {
        layer->Draw();
    }
}

void LayerStack::ClearLayers()
{
    while (!_layers.empty())
    {
        std::set<std::shared_ptr<BaseLayer>>::iterator it = _layers.begin();
        (*it)->OnDettach();
        _layers.erase(it);
    }
}
