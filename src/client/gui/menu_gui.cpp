#include "menu_gui.hpp"
#include "audio_device_config.hpp"
#include "logger.hpp"
#include "ui/ui_menu.h"
#include <QMediaDevices>
#include <QCameraDevice>
#include <QCamera>
#include <qdebug.h>
#include <qmediadevices.h>
#include <QAudioDevice>

MenuGui::MenuGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MenuGui)
{
    m_ui->setupUi(this);
    m_config = AudioDevConfig::get_instance(); 

    m_input_name = QString::fromStdString(m_config->get_input());
    m_output_name = QString::fromStdString(m_config->get_output());
    m_webcam_name = QMediaDevices::defaultVideoInput().description();

    refresh_devices();
}

MenuGui::~MenuGui()
{
    delete m_ui;
}

void MenuGui::refresh_devices()
{
    m_ui->input_drop->addItem(m_input_name);
    m_ui->output_drop->addItem(m_output_name);
    m_ui->webcam_drop->addItem(m_webcam_name);

    LOG_DEBUG("Input %s\nOutput %s\nWebcam %s\n", m_input_name.toStdString().c_str(), m_output_name.toStdString().c_str(), m_webcam_name.toStdString().c_str());

    for(auto &device : QMediaDevices::audioInputs())
    {
        qDebug() << device.id() << " " << device.description();
        if ( -1 == m_ui->input_drop->findText(device.description()))
        {   
            m_ui->input_drop->addItem(device.description());
        }  
    }

    for(auto &device : QMediaDevices::audioOutputs())
    {
        qDebug() << device.id() << " " << device.description();
        if ( -1 == m_ui->output_drop->findText(device.description()))
        {   
            m_ui->output_drop->addItem(device.description());
        }
    }

    for(auto &device : QMediaDevices::videoInputs())
    {
        qDebug() << device.id() << " " << device.description();
        if ( -1 == m_ui->webcam_drop->findText(device.description()))
        {
            m_ui->webcam_drop->addItem(device.description());
        }
    }
}