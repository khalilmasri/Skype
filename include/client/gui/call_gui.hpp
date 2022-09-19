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
  
  void call_init(std::string &t_token, int t_contact_id, QString &t_username);
  void video_init(std::string &t_token, int t_contact_id, QString &t_username);

  ~CallGui();

private:
  Ui::CallGui *m_ui;

private: // Methods

bool connect_to(P2P &t_call, std::string t_id);
};

#endif // CALL_GUI_HPP
