#include "background.hpp"
#include "Engine.hpp"

void BackgroundController::addBkgLayer(const int layerno, Layer && layer) {
    std::lock_guard<std::mutex> lk(m_bgLayersMutex);
    m_bkgLayers[layerno] = std::move(layer);
}

void BackgroundController::addFgLayer(const int layerno, Layer && layer) {
    std::lock_guard<std::mutex> lk(m_fgLayersMutex);
    m_fgLayers[layerno] = std::move(layer);
}

LockedRef<LayerMap> BackgroundController::getBkgLayers() {
    return {m_bkgLayers, std::unique_lock<std::mutex>(m_bgLayersMutex)};
}

LockedRef<LayerMap> BackgroundController::getFgLayers() {
    return {m_fgLayers, std::unique_lock<std::mutex>(m_fgLayersMutex)};
}