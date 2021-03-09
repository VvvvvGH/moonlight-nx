#include "AddHostButton.hpp"
#include "nanovg.h"

using namespace nanogui;

AddHostButton::AddHostButton(Widget* parent, AddHostButtonType type): Button(parent, ""), m_type(type) {
    set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    add<Widget>()->set_fixed_height(170);
    auto label = add<Label>(type == AddHostButtonTypeFind ? "查找主机" : "添加主机");
    label->set_font_size(20);
    screen()->perform_layout();
}

void AddHostButton::draw(NVGcontext *ctx) {
    Button::draw(ctx);
    
    nvgSave(ctx);
    nvgReset(ctx);
    
    nvgTranslate(ctx, absolute_position().x(), absolute_position().y());
    nvgFillColor(ctx, Color(255, 255, 255, 255));
    nvgFontSize(ctx, 130);
    nvgFontFace(ctx, "icons");
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgText(ctx, width() / 2, height() / 2 - 4, utf8(FA_DESKTOP).data(), NULL);
    
    nvgFontSize(ctx, 40);
    nvgText(ctx, width() / 2, height() / 2 - 16, utf8(m_type == AddHostButtonTypeFind ? FA_SEARCH : FA_PLUS).data(), NULL);
    
    nvgRestore(ctx);
}
