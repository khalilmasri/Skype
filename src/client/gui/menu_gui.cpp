#include "menu_gui.hpp"
#include "audio_device_config.hpp"
#include "logger.hpp"
#include "client.hpp"
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


	for(auto &device : m_config->list_input_name())
	{
	LOG_TRACE("Input -> %s", device.c_str());
		if ( -1 == m_ui->input_drop->findText(QString::fromStdString(device)))
		{   
			m_ui->input_drop->addItem(QString::fromStdString(device));
		}  
	}

	for(auto &device : m_config->list_output_name())
	{
		LOG_TRACE("Input -> %s -> %d", device.c_str(), m_ui);
		if ( -1 == m_ui->output_drop->findText(QString::fromStdString(device)))
		{   
			m_ui->output_drop->addItem(QString::fromStdString(device));
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


// SIGNALS

void MenuGui::on_output_drop_currentIndexChanged(int index)
{
	LOG_DEBUG("Setting output to %d", index);
	m_temp_output_index = index;
}

void MenuGui::on_input_drop_currentIndexChanged(int index)
{
	LOG_DEBUG("Setting input to %d", index);
	m_temp_input_index = index;
}


void MenuGui::on_accept_clicked()
{	
	LOG_DEBUG("Input : %d | Output : %d", m_temp_input_index, m_temp_output_index);
	if (-1 != m_temp_input_index)
	{
		LOG_TRACE("Changed input device to [%s]", m_ui->input_drop->itemText(m_temp_input_index).toStdString().c_str());
		m_config->select_input(m_temp_input_index);
		m_temp_input_index = -1;

    Client::reinit_call();
	}

	if (-1 != m_temp_output_index)
	{
		LOG_TRACE("Changed input device to [%s]", m_ui->output_drop->itemText(m_temp_output_index).toStdString().c_str());
		m_config->select_output(m_temp_output_index);
		m_temp_output_index = -1;
	}

	this->hide();
}

void MenuGui::on_cancel_clicked()
{
	this->hide();
}
