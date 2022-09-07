#include "notification.hpp"

#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QDebug>
#include <qboxlayout.h>
#include <qimage.h>
#include <qpixmap.h>
 
Notification::Notification(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint |        // Disable window decoration
                   Qt::Tool |                       // Discard display in a separate window
                   Qt::WindowStaysOnTopHint);       // Set on top of all windows
    setAttribute(Qt::WA_TranslucentBackground);     // Indicates that the background will be transparent
    setAttribute(Qt::WA_ShowWithoutActivating);     // At the show, the widget does not get the focus automatically
 
    animation.setTargetObject(this);                // Set the target animation
    animation.setPropertyName("popupOpacity");      // 
    connect(&animation, &QAbstractAnimation::finished, this, &Notification::hide); 
 
    title.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); 
    title.setStyleSheet("QLabel { color : white; "
                        "margin-top: 2px;"
                        "margin-bottom: 2px;"
                        "margin-left: 10px;"
                        "margin-right: 10px; }");
    
    content.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); 
    content.setStyleSheet("QLabel { color : white; "
                        "margin-top: 2px;"
                        "margin-bottom: 2px;"
                        "margin-left: 10px;"
                        "margin-right: 10px; }");

    layout.addWidget(&title, 0);
    layout.addWidget(&content, 0);
    
    setLayout(&layout); 

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Notification::hideAnimation);
}
 
void Notification::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
 
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); 
 
    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);
 
    painter.setBrush(QBrush(QColor(0,0,0,180)));
    painter.setPen(Qt::NoPen); 
 
    painter.drawRoundedRect(roundedRect, 10, 10);
}
 
void Notification::setPopupText(const QString &t_title, const QString &t_content)
{
    title.setText("<img src=../misc/icons/bell.png> "+t_title);    // Set the text in the Label
    content.setText(t_content);

    adjustSize();           // With the recalculation notice sizes
}
 
void Notification::show()
{
    setWindowOpacity(0.0);  // Set the transparency to zero
 
    animation.setDuration(150);     // Configuring the duration of the animation
    animation.setStartValue(0.0);   // The start value is 0 (fully transparent widget)
    animation.setEndValue(1.0);     // End - completely opaque widget
 
    setGeometry(QApplication::primaryScreen()->availableGeometry().width() - 36 - width() + QApplication::primaryScreen() -> availableGeometry().x(),
                QApplication::primaryScreen()->availableGeometry().height() - 36 - height() + QApplication::primaryScreen() -> availableGeometry().y(),
                width(),
                height());
    QWidget::show();                
 
    animation.start();             
    timer->start(3000);             
}
 
void Notification::hideAnimation()
{
    timer->stop();                 
    animation.setDuration(1000);   
    animation.setStartValue(1.0);   
    animation.setEndValue(0.0);     
    animation.start();             
}
 
void Notification::hide()
{
    // If the widget is transparent, then hide it
    if(getPopupOpacity() == 0.0){
        QWidget::hide();
    }
}
 
void Notification::setPopupOpacity(float opacity)
{
    popupOpacity = opacity;
 
    setWindowOpacity(opacity);
}
 
float Notification::getPopupOpacity() const
{
    return popupOpacity;
}