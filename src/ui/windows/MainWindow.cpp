#include "MainWindow.hpp"
#include "AddHostWindow.hpp"
#include "AddHostButton.hpp"
#include "HostButton.hpp"
#include "GameStreamClient.hpp"
#include "LoadingOverlay.hpp"
#include "AppListWindow.hpp"
#include "SettingsWindow.hpp"
#include "Settings.hpp"
#include "InputSettingsWindow.hpp"
#include "Alert.hpp"
#include "nanovg.h"

using namespace nanogui;

MainWindow::MainWindow(Widget *parent): ContentWindow(parent, "月光串流") {
    set_box_layout(Orientation::Vertical, Alignment::Minimum);
    
    set_right_title_button(FA_SYNC, [this] {
        this->reload();
    });
    set_right_title_button(FA_GAMEPAD, [this] {
        push<InputSettingsWindow>(0, "默认");
    });
    set_right_title_button(FA_COG, [this] {
        push<SettingsWindow>();
    });
}

void MainWindow::window_appear() {
    reload();
}

void MainWindow::reload() {
    clean_container();
    
    container()->add<Label>("* 要获得最佳体验，请使用5G WiFi 、最新的系统 (11.0.0/11.0.1) 和大气层 (0.18.0)。如果卡顿，可尝试CPU超频。");
    container()->add<Widget>()->set_fixed_height(6);
    
    auto button_container = container()->add<Widget>();
    button_container->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 10));
    
    for (auto host: Settings::instance().hosts()) {
        auto button = button_container->add<HostButton>(host);
        button->set_fixed_size(Size(200, 200));
        button->set_callback([this, button] {
            if (button->is_active()) {
                if (button->is_paired()) {
                    push<AppListWindow>(button->host().address);
                } else {
                    auto loader = add<LoadingOverlay>("正在配对... (请输入 0000)");
                    
                    GameStreamClient::instance().pair(button->host().address, "0000", [this, loader](auto result){
                        loader->dispose();
                        
                        if (result.isSuccess()) {
                            reload();
                        } else {
                            screen()->add<Alert>("错误", result.error());
                        }
                    });
                }
            } else {
                auto alert = screen()->add<Alert>("错误", "主机无响应...", false);
                
                if (!button->host().mac.empty()) {
                    alert->add_button("唤醒", [this, button] {
                        wake_up_host(button->host());
                    });
                }
                
                alert->add_button("删除", [this, button] {
                    Settings::instance().remove_host(button->host());
                    reload();
                });
                alert->add_button("取消");
            }
        });
    }
    
    auto find_button = button_container->add<AddHostButton>(AddHostButtonTypeFind);
    find_button->set_fixed_size(Size(200, 200));
    find_button->set_callback([this] {
        auto addresses = GameStreamClient::instance().host_addresses_for_find();
        
        if (addresses.empty()) {
            screen()->add<Alert>("错误", "无法获取IP地址...");
        } else {
            auto alert = screen()->add<Alert>("查找主机", "查找与Switch处于相同网络的主机。查找范围：" + addresses.front() + " 到 " + addresses.back() + "。\n执行查找前请先检查PC 和 Switch 的网络。", false);
            alert->add_button("查找", [this] {
                find_host();
            });
            alert->add_button("取消");
        }
    });
    
    auto add_button = button_container->add<AddHostButton>(AddHostButtonTypeAdd);
    add_button->set_fixed_size(Size(200, 200));
    add_button->set_callback([this] {
        push<AddHostWindow>();
    });
    
    perform_layout();
}

void MainWindow::draw(NVGcontext *ctx) {
    ContentWindow::draw(ctx);
    
    nvgSave(ctx);
    
    nvgFillColor(ctx, Color(255, 255, 255, 200));
    nvgFontSize(ctx, 20);
    nvgFontFace(ctx, "sans");
    
    int text_width = nvgTextBounds(ctx, 0, 0, MOONLIGHT_VERSION, NULL, NULL);
    nvgText(ctx, width() - text_width - 8, height() - 8, MOONLIGHT_VERSION, NULL);
    
    nvgRestore(ctx);
}

void MainWindow::wake_up_host(const Host &host) {
    auto loader = add<LoadingOverlay>("发送唤醒命令...");
    
    GameStreamClient::instance().wake_up_host(host, [this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            reload();
        } else {
            screen()->add<Alert>("错误", result.error());
        }
    });
}

void MainWindow::find_host() {
    auto loader = add<LoadingOverlay>("正在查找主机...");
    
    GameStreamClient::instance().find_host([this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            Settings::instance().add_host(result.value());
            reload();
        } else {
            screen()->add<Alert>("错误", result.error());
        }
    });
}
