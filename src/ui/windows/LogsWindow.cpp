#include "LogsWindow.hpp"
#include "Settings.hpp"
#include "Data.hpp"

using namespace nanogui;

LogsWindow::LogsWindow(Widget *parent): ContentWindow(parent, "日志") {
    set_left_pop_button();
    set_right_title_button(FA_TRASH_ALT, [this] {
        remove(Settings::instance().log_path().c_str());
        this->reload();
    });
    set_box_layout(Orientation::Vertical, Alignment::Minimum);
    
    reload();
}

void LogsWindow::reload() {
    clean_container();
    
    Data data = Data::read_from_file(Settings::instance().log_path());
    
    if (data.is_empty()) {
        auto label = container()->add<Label>("没有找到日志...");
        label->set_fixed_width(container()->width());
    } else {
        std::stringstream stream((char *)data.bytes());
        std::string string;
        std::vector<std::string> logs;
        
        while (std::getline(stream, string, '\n')) {
            if (string.size() > 100) {
                string = string.substr(0, 100) + "...";
            }
            
            logs.push_back(string);
        }
        
        if (logs.size() > 100) {
            logs = std::vector<std::string>(logs.end() - 100, logs.end());
            logs.insert(logs.begin(), "* 显示后100行");
        }
        
        for (auto log: logs) {
            auto label = container()->add<Label>(log);
            label->set_selectable(true);
        }
    }
    perform_layout();
}
