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

	void refresh_devices();

private slots:
	void on_output_drop_currentIndexChanged(int index);
	void on_input_drop_currentIndexChanged(int index);
	void on_accept_clicked();
	void on_cancel_clicked();

private:
	Ui::MenuGui *m_ui;

	AudioDevConfig *m_config = nullptr;
	
	QString     m_input_name;
	QString     m_output_name;
	QString     m_webcam_name;

	int 			m_temp_input_index = -1;
	int			m_temp_output_index = -1;
};

#endif // MENU_GUI_HPP
