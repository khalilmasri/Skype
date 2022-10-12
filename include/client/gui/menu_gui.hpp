#ifndef MENU_GUI_HPP
#define MENU_GUI_HPP

#include "av_settings.hpp"
#include <QDialog>
#include <QString>
#include "audio_device_config.hpp"

namespace Ui {
    class MenuGui;
}

class MenuGui : public QDialog
{
    Q_OBJECT

public:
    explicit MenuGui(QWidget *parent = nullptr);
    ~MenuGui();

private:
    Ui::MenuGui *m_ui;

    AudioDevConfig *m_config = nullptr;
    
    QString     m_input_name;
    QString     m_output_name;
    QString     m_webcam_name;
};

#endif // MENU_GUI_HPP
