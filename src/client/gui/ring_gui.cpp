#include "ring_gui.hpp"
#include "ui/ui_ring.h"

#include <QMovie>
#include <qmovie.h>

RingGui::RingGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::RingGui)
{
    m_ui->setupUi(this);
}

RingGui::~RingGui()
{
    delete m_ui;
}

void RingGui::set_message(QString &t_user)
{
    this->setWindowTitle("Incoming call from " + t_user);
    m_ui->details->setText("Would you like to accept the call from " + t_user + "?");
    QMovie *ringing_gif = new QMovie("../misc/gif/ringing.gif");
    ringing_gif->setScaledSize(QSize(100,100));
    m_ui->ringing->setMovie(ringing_gif);
    ringing_gif->start();
}
