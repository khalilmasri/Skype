#ifndef CALL_GUI_HPP
#define CALL_GUI_HPP

#include "peer_to_peer.hpp"
#include "video_playback.hpp"
#include "menu_gui.hpp"

#include <QDialog>
#include <QImage>

namespace Ui {
	class CallGui;
}

class CallGui : public QDialog
{
	Q_OBJECT

	public:
	explicit CallGui(QWidget *parent = nullptr);

	void call_init(int t_contact_id, QString &t_username);
	void call_accept(QString &t_username);
	void video_stream(VideoPlayback::VideoQueuePtr t_stream_queue);

	~CallGui();

	private slots:
	void on_hangup_clicked();

	private:
	Ui::CallGui *m_ui;
	bool m_stop_stream = false;

	private: // Methods
	void reject() override;
	QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format); 

};

#endif // CALL_GUI_HPP
