#ifndef CALL_GUI_HPP
#define CALL_GUI_HPP

#include "peer_to_peer.hpp"

#include <QDialog>

namespace Ui {
  class CallGui;
}

class CallGui : public QDialog
{
  Q_OBJECT

public:
  explicit CallGui(QWidget *parent = nullptr);
  
  void call_init(int t_contact_id, QString &t_username);
  void video_init(int t_contact_id, QString &t_username);

  ~CallGui();

private slots:
  void on_webcam_clicked();
  void on_microphone_clicked();
  void on_hangup_clicked();
  void on_menu_clicked();

private:
  Ui::CallGui *m_ui;

private: // Methods
  void reject() override;

};

#endif // CALL_GUI_HPP
