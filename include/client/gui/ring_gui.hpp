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

    void set_details(QString &t_user, int t_id);

public slots:
    void on_ignore_clicked();
    void on_answer_clicked();

signals:
    void start_call(int t_caller);

private:
    Ui::RingGui *m_ui;
    int m_caller_id;
};

#endif // RING_GUI_HPP
