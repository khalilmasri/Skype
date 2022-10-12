#include "menu_gui.hpp"
#include "audio_device_config.hpp"
#include "logger.hpp"
#include "ui/ui_menu.h"
#include <QMediaDevices>
#include <QCameraDevice>

MenuGui::MenuGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MenuGui)
{
    m_ui->setupUi(this);
    m_config = AudioDevConfig::get_instance(); 

    m_input_name = QString::fromStdString(m_config->get_input());
    m_output_name = QString::fromStdString(m_config->get_output());
    m_webcam_name = QMediaDevices::defaultVideoInput().description();
    
    LOG_DEBUG("Input %s Output %s Webcam %s", m_input_name.toStdString().c_str(), m_output_name.toStdString().c_str(), m_webcam_name.toStdString().c_str());

    m_ui->input_drop->addItem(m_input_name);
    m_ui->output_drop->addItem(m_input_name);
    m_ui->webcam_drop->addItem(m_webcam_name);
}

MenuGui::~MenuGui()
{
    delete m_ui;
}
