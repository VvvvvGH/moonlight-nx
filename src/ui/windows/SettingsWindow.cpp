#include "SettingsWindow.hpp"
#include "Settings.hpp"
#include "LogsWindow.hpp"

using namespace nanogui;

#define SET_SETTING(n, func) \
    case n: \
        Settings::instance().func; \
        break;

#define GET_SETTINGS(combo_box, n, i) \
    case n: \
        combo_box->set_selected_index(i); \
        break;

#define DEFAULT \
    default: \
        break;

SettingsWindow::SettingsWindow(nanogui::Widget* parent): ContentWindow(parent, "Settings") {
    set_left_pop_button();
    
    set_box_layout(Orientation::Horizontal, Alignment::Minimum);
    
    int container_width = 380;
    int component_width = 290;
    
    auto left_container = container()->add<Widget>();
    left_container->set_layout(new GroupLayout(30, 10, 30, 10));
    left_container->set_fixed_width(container_width);
    
    left_container->add<Label>("分辨率");
    std::vector<std::string> resolutions = { "720p", "1080p" };
    auto resolution_combo_box = left_container->add<ComboBox>(resolutions);
    resolution_combo_box->set_fixed_width(component_width);
    resolution_combo_box->popup()->set_fixed_width(component_width);
    resolution_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_resolution(720));
            SET_SETTING(1, set_resolution(1080));
            DEFAULT;
        }
    });
    
    switch (Settings::instance().resolution()) {
        GET_SETTINGS(resolution_combo_box, 720, 0);
        GET_SETTINGS(resolution_combo_box, 1080, 1);
        DEFAULT;
    }
    
    auto ignore_unsupported_resolutions = left_container->add<CheckBox>("忽略不支持的分辨率");
    ignore_unsupported_resolutions->set_checked(Settings::instance().ignore_unsupported_resolutions());
    ignore_unsupported_resolutions->set_callback([](auto value) {
        Settings::instance().set_ignore_unsupported_resolutions(value);
    });
    
    left_container->add<Label>("帧数");
    std::vector<std::string> fps = { "30", "60" };
    auto fps_combo_box = left_container->add<ComboBox>(fps);
    fps_combo_box->set_fixed_width(component_width);
    fps_combo_box->popup()->set_fixed_width(component_width);
    fps_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_fps(30));
            SET_SETTING(1, set_fps(60));
            DEFAULT;
        }
    });
    
    switch (Settings::instance().fps()) {
        GET_SETTINGS(fps_combo_box, 30, 0);
        GET_SETTINGS(fps_combo_box, 60, 1);
        DEFAULT;
    }
    
    left_container->add<Label>("视频编码");
    std::vector<std::string> video_codec = { "H.264", "HEVC (H.265, 实验性)" };
    auto video_codec_combo_box = left_container->add<ComboBox>(video_codec);
    video_codec_combo_box->set_fixed_width(component_width);
    video_codec_combo_box->popup()->set_fixed_width(component_width);
    video_codec_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_video_codec(H264));
            SET_SETTING(1, set_video_codec(H265));
            DEFAULT;
        }
    });
    
    switch (Settings::instance().video_codec()) {
        GET_SETTINGS(video_codec_combo_box, H264, 0);
        GET_SETTINGS(video_codec_combo_box, H265, 1);
        DEFAULT;
    }
    
    char bitrate_str[100];
    sprintf(bitrate_str, "视频码率: %0.1f Mbps", float(Settings::instance().bitrate()) / 1000);
    
    auto video_bitrate_label = left_container->add<Label>(bitrate_str);
    auto video_bitrate_slider = left_container->add<Slider>();
    video_bitrate_slider->set_step(0.5);
    video_bitrate_slider->set_highlight_color(Color(62, 78, 184, 255));
    video_bitrate_slider->set_range({0.5, 150});
    video_bitrate_slider->set_value(float(Settings::instance().bitrate()) / 1000);
    video_bitrate_slider->set_highlighted_range({0, float(Settings::instance().bitrate()) / 1000 / 150});
    video_bitrate_slider->set_fixed_size(Size(container_width - 80, 30));
    video_bitrate_slider->set_callback([video_bitrate_label, video_bitrate_slider](auto value) {
        video_bitrate_slider->set_highlighted_range({0, value / 150});
        float bitrate = round(2.0 * value) / 2.0;
        char bitrate_str[100];
        sprintf(bitrate_str, "视频码率: %0.1f Mbps", bitrate);
        video_bitrate_label->set_caption(bitrate_str);
        Settings::instance().set_bitrate(bitrate * 1000);
    });
    
    left_container->add<Label>("输入设置");
    auto click_by_tap = left_container->add<CheckBox>("点按屏幕作为鼠标左键");
    click_by_tap->set_checked(Settings::instance().click_by_tap());
    click_by_tap->set_callback([](auto value) {
        Settings::instance().set_click_by_tap(value);
    });
    
    auto right_container = container()->add<Widget>();
    right_container->set_layout(new GroupLayout(30, 10, 30, 10));
    right_container->set_fixed_width(container_width + 90);
    
    right_container->add<Label>("解码器线程");
    std::vector<std::string> decoder_threads = { "0 (不用多线程)", "2", "3", "4" };
    auto decoder_threads_combo_box = right_container->add<ComboBox>(decoder_threads);
    decoder_threads_combo_box->set_fixed_width(component_width);
    decoder_threads_combo_box->popup()->set_fixed_width(component_width);
    decoder_threads_combo_box->set_callback([](auto value) {
        switch (value) {
            SET_SETTING(0, set_decoder_threads(0));
            SET_SETTING(1, set_decoder_threads(2));
            SET_SETTING(2, set_decoder_threads(3));
            SET_SETTING(3, set_decoder_threads(4));
            DEFAULT;
        }
    });
    
    switch (Settings::instance().decoder_threads()) {
        GET_SETTINGS(decoder_threads_combo_box, 0, 0);
        GET_SETTINGS(decoder_threads_combo_box, 2, 1);
        GET_SETTINGS(decoder_threads_combo_box, 3, 2);
        GET_SETTINGS(decoder_threads_combo_box, 4, 3);
        DEFAULT;
    }
    
    right_container->add<Label>("串流设置");
    auto sops = right_container->add<CheckBox>("使用优化的游戏设置");
    sops->set_checked(Settings::instance().sops());
    sops->set_callback([](auto value) {
        Settings::instance().set_sops(value);
    });
    
    auto play_audio = right_container->add<CheckBox>("在电脑上播放音频");
    play_audio->set_checked(Settings::instance().play_audio());
    play_audio->set_callback([](auto value) {
        Settings::instance().set_play_audio(value);
    });
    
    right_container->add<Label>("调试");
    auto write_log = right_container->add<CheckBox>("写日志");
    write_log->set_checked(Settings::instance().write_log());
    write_log->set_callback([](auto value) {
        Settings::instance().set_write_log(value);
    });
    
    auto log_button = right_container->add<Button>("显示日志");
    log_button->set_fixed_width(component_width);
    log_button->set_callback([this] {
        push<LogsWindow>();
    });
}

void SettingsWindow::window_disappear() {
    Settings::instance().save();
}
