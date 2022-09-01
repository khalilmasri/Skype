#ifndef POPUP_H
#define POPUP_H
 
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <qboxlayout.h>
 
class Notification : public QWidget
{
    Q_OBJECT
 
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)
 
    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;
 
public:
    explicit Notification(QWidget *parent = 0);
 
protected:
    void paintEvent(QPaintEvent *event);    // The background will be drawn through the redraw method
 
public slots:
    void setPopupText(const QString &t_title, const QString &t_content); // Setting text notification
    void show();                            /* own widget displaying method 
                                             * It is necessary to pre-animation settings
                                             * */
 
private slots:
    void hideAnimation();                   // Slot start the animation hide
    void hide();                            /* At the end of the animation, it is checked in a given slot,
                                             * Does the widget visible, or to hide
                                             * */
 
private:
    QLabel title;
    QLabel content; 
    QLabel icon;
    QVBoxLayout layout;     
    QPropertyAnimation animation;  
    float popupOpacity;     
    QTimer *timer;          
};
 
#endif // POPUP_H