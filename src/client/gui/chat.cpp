#include "chat.hpp"
#include "ui/ui_chat.h"

ChatGui::ChatGui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatGui)
{
    ui->setupUi(this);
}

ChatGui::~ChatGui()
{
    delete ui;
}
