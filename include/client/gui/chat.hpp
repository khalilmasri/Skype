#ifndef CHAT_HPP
#define CHAT_HPP

#include <QDialog>

namespace Ui {
class ChatGui;
}

class ChatGui : public QDialog
{
    Q_OBJECT

public:
    explicit ChatGui(QWidget *parent = nullptr);
    ~ChatGui();

private:
    Ui::ChatGui *ui;
};

#endif // CHAT_HPP
