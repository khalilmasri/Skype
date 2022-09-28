#ifndef RING_GUI_HPP
#define RING_GUI_HPP

#include <QDialog>
#include <QString>

namespace Ui {
class RingGui;
}

class RingGui : public QDialog
{
    Q_OBJECT

public:
    explicit RingGui(QWidget *parent = nullptr);
    ~RingGui();

    void set_message(QString &t_user);

private:
    Ui::RingGui *m_ui;
};

#endif // RING_GUI_HPP
