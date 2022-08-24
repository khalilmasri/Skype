#include "contact_gui.hpp"
#include "ui/ui_contact.h"
#include "job_bus.hpp"
#include "job.hpp"

#include <QString>
#include <string>

ContactGui::ContactGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ContactGui)
{
    m_ui->setupUi(this);
}

ContactGui::~ContactGui()
{
    delete m_ui;
}

// ***** PUBLIC ***** //

void ContactGui::set_option(QString t_title)
{
    this->setWindowTitle(t_title);
    m_ui->label->setText(t_title);
    m_ui->action->setText(t_title);

    if ( "Search" == t_title){
        m_type = SEARCH;
    } else {
        m_type = ADD;
    }
}

// ***** PRIVATE ***** //

void ContactGui::handle_action()
{
    if( "" == m_ui->lineEdit->text()){
        return;
    }

    std::string user = m_ui->lineEdit->text().toStdString();

    if ( SEARCH == m_type ){
        JobBus::handle({Job::SEARCH, user});
    } else {
        JobBus::handle({Job::ADD, user});
    }

    JobBus::handle({Job::DISP_CONTACTS});
}

// ***** SLOTS ***** //

void ContactGui::on_lineEdit_returnPressed()
{
    handle_action();

}


void ContactGui::on_action_clicked()
{
    handle_action();
}


void ContactGui::on_cancel_clicked()
{
    this->hide();
}

