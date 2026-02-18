#include <Geode/modify/LevelEditorLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(ModLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        ListenerHandle clickListener;
        CCPoint previousMousePos;
    };

    $override
    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;

        auto dragButton = getDragButton();

        m_fields->clickListener = MouseInputEvent().listen([this, dragButton](MouseInputData& data) {
            if (data.button != dragButton) return;

            if (data.action == MouseInputData::Action::Press) {
                startPanning();
            } else if (data.action == MouseInputData::Action::Release) {
                stopPanning();
            }
        });

        return true;
    }

    void startPanning() {
        m_fields->previousMousePos = getMousePos();
        schedule(schedule_selector(ModLevelEditorLayer::updatePanning));
    }

    void stopPanning() {
        unschedule(schedule_selector(ModLevelEditorLayer::updatePanning));
    }

    void updatePanning(float dt) {
        CCPoint mouseDelta = getMousePos() - m_fields->previousMousePos;
        m_fields->previousMousePos = getMousePos();

        m_objectLayer->setPosition(m_objectLayer->getPosition() + mouseDelta);

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
