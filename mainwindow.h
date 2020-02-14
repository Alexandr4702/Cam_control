#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>

#include <QTimer>

namespace Ui {
class MainWindow;

}

typedef enum
{
    mode_no_control=00,
    mode_speed=01,
    mode_angle=2,
    mode_angle_rel_frame=05
}mode_control;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void clicked_search_button(void);
    void clicked_connect_button(void);
    void choosed_port_name(QString);
    void printt(QString);
    void clicked_disconnect_button(void);
    void choosed_baud_reate(QString a);
    void reader(void);
    void sender_roll(int );
    void sender_pitch(int );
    void sender_yaw(int );
    void combine_long_cmd_control(mode_control RM, mode_control PM,mode_control YM,double Roll,double Pitch,double Yaw );
    void Get_angle(void);

private:
    Ui::MainWindow *ui;
    QSerialPort test;
    QString name_port;
    QTimer *tim;
};

#endif // MAINWINDOW_H
