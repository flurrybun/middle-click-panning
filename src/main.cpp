#include <Geode/modify/LevelEditorLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(ModLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        ListenerHandle clickListener;
        ListenerHandle moveListener;
        CCPoint prevMousePos;
        bool isPanning = false;
    };

    $override
    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;

        auto dragButton = getDragButton();

        m_fields->clickListener = MouseInputEvent().listen([this, dragButton](MouseInputData& data) {
            if (data.button != dragButton || m_playbackMode == PlaybackMode::Playing) return;

            if (data.action == MouseInputData::Action::Press) {
                startPanning();
            } else if (data.action == MouseInputData::Action::Release) {
                stopPanning();
            }
        });

        m_fields->moveListener = MouseMoveEvent().listen([this](int32_t x, int32_t y) {
            // the x and y params are evil

            updatePanning();
        });

        return true;
    }

    void startPanning() {
        m_fields->prevMousePos = getMousePos();
        m_fields->isPanning = true;
    }

    void stopPanning() {
        m_fields->isPanning = false;
    }

    void updatePanning() {
        if (!m_fields->isPanning) return;

        if (m_playbackMode == PlaybackMode::Playing) {
            stopPanning();
            return;
        }

        auto mousePos = getMousePos();

        CCPoint mouseDelta = mousePos - m_fields->prevMousePos;
        m_fields->prevMousePos = mousePos;

        m_objectLayer->setPosition(m_objectLayer->getPosition() + mouseDelta);
        m_editorUI->m_swipeStart += mouseDelta;

        m_editorUI->constrainGameLayerPosition(-100, -100);
        m_editorUI->updateSlider();
    }

    MouseInputData::Button getDragButton() {
        switch (hash(Mod::get()->getSettingValue<std::string>("drag-button"))) {
            case hash("Right Click"):
                return MouseInputData::Button::Right;
            case hash("Middle Click"):
                return MouseInputData::Button::Middle;
            case hash("Side Button 1"):
                return MouseInputData::Button::Button4;
            case hash("Side Button 2"):
                return MouseInputData::Button::Button5;
        }

        return MouseInputData::Button::Right;
    }
};
