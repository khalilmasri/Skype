#ifndef CONTACT_GUI_HPP
#define CONTACT_GUI_HPP

#include <QDialog>
#include <QString>

namespace Ui {
class ContactGui;
}

class ContactGui : public QDialog
{
    Q_OBJECT

public:
    explicit ContactGui(QWidget *parent = nullptr);
    ~ContactGui();

    void set_option(QString t_title);

private slots:
    void on_lineEdit_returnPressed();
    void on_action_clicked();
    void on_cancel_clicked();

private: // Variables
    Ui::ContactGui *m_ui;
    enum Type{SEARCH, ADD};

    Type m_type;

private: // Methods
    void handle_action();
};

#endif // CONTACT_GUI_HPP
