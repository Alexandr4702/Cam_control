#include "mainwindow.h"
#include "ui_mainwindow.h"





uint8_t calc_crc(uint8_t* ptr,uint16_t size)
{
    uint8_t crc=0;
    for(uint16_t i=0;i<size;i++){crc+=ptr[i];}
    return crc;
}






MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    test.setBaudRate(9600,QSerialPort::AllDirections);
    test.setDataBits(QSerialPort::Data8);
    test.setParity(QSerialPort::NoParity);
    test.setStopBits(QSerialPort::OneStop);
    test.setFlowControl(QSerialPort::NoFlowControl);

    ui->combo_baud->addItem("9600");
    ui->combo_baud->addItem("115200");



    connect(ui->search_button,SIGNAL(clicked()),this,SLOT(clicked_search_button(void)));
    connect(ui->connect_button,SIGNAL(clicked()),this,SLOT(clicked_connect_button(void)));
    connect(ui->disconnect_button,SIGNAL(clicked()),this,SLOT(clicked_disconnect_button(void)));
    connect(ui->Get_angle_button,SIGNAL(clicked()),this,SLOT(Get_angle(void)));


    connect(ui->combo_port,SIGNAL(currentIndexChanged(const QString)),this,SLOT(choosed_port_name(QString)));
    connect(ui->combo_baud,SIGNAL(currentIndexChanged(const QString)),this,SLOT(choosed_baud_reate(QString)));


    QPalette p = ui->plainTextEdit->palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    ui->plainTextEdit->setPalette(p);


    tim=new QTimer;
    //connect(tim,SIGNAL(timeout()),this,SLOT(sender(void)));

    connect(&test,SIGNAL(readyRead()),this,SLOT(reader(void)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clicked_connect_button(void)
{
    if(name_port==nullptr){ui->plainTextEdit->appendPlainText("please set port");return;}
    test.setPortName(name_port);
    if(test.open(QIODevice::ReadWrite))
    {
        printt(name_port+"  open");
        //tim->start(200);
        connect(ui->roll,SIGNAL(valueChanged(int)),this,SLOT(sender_roll(int)));
        connect(ui->pitch,SIGNAL(valueChanged(int)),this,SLOT(sender_pitch(int)));
        connect(ui->yaw,SIGNAL(valueChanged(int)),this,SLOT(sender_yaw(int)));

    }
    else{printt(test.errorString());}
}

void MainWindow::clicked_search_button(void)
{
    ui->combo_port->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
        ui->combo_port->addItem(info.portName());
        }
}


void MainWindow::choosed_port_name(QString a)
{
    name_port=a;
}

void MainWindow::choosed_baud_reate(QString a)
{
    test.setBaudRate(a.toInt(),QSerialPort::AllDirections);

}

void MainWindow::printt(QString a)
{
    ui->plainTextEdit->appendPlainText(a);
}

void MainWindow::clicked_disconnect_button(void)
{
    if(test.isOpen())
    {
        test.close();
        printt(name_port+"  close");
        //tim->stop();
        disconnect(ui->roll,SIGNAL(valueChanged(int)),this,SLOT(sender_roll(int)));
        disconnect(ui->pitch,SIGNAL(valueChanged(int)),this,SLOT(sender_pitch(int)));
        disconnect(ui->yaw,SIGNAL(valueChanged(int)),this,SLOT(sender_yaw(int)));
    }
    else{printt("device is not open ");}
}


void MainWindow::reader(void)
{
    QByteArray new_Mes;

    /*
    new_Mes=test.readAll();

    new_Mes+=test.readAll();
    */

}

void MainWindow::sender_roll(int a)
{
    //a=0;
    QString txt;
    if(test.isWritable())
    {
        combine_long_cmd_control(mode_angle,mode_no_control,mode_no_control,a,0,0);
        txt.setNum(a);
        ui->plainTextEdit->appendPlainText(txt);
    }
}

void MainWindow::sender_pitch(int a)
{
    //a=0;
    QString txt;
    if(test.isWritable())
    {
        combine_long_cmd_control(mode_no_control,mode_angle,mode_no_control,0,a,0);
        txt.setNum(a);
        ui->plainTextEdit->appendPlainText(txt);
    }
}

void MainWindow::sender_yaw(int a)
{
    //a=0;
    QString txt;
    if(test.isWritable())
    {
        combine_long_cmd_control(mode_no_control,mode_no_control,mode_angle,0,0,a);
        txt.setNum(a);
        ui->plainTextEdit->appendPlainText(txt);
    }
}

void MainWindow::combine_long_cmd_control(mode_control RM, mode_control PM,mode_control YM,double Roll,double Pitch,double Yaw )
{
    uint8_t message[20];

    uint8_t header[4]={0xFF,0x01,0x0F,0x10};

    memcpy(message,header,4);
    message[4]=static_cast<uint8_t>( RM);
    message[5]=(uint8_t) PM;
    message[6]=(uint8_t) YM;

    switch(RM)
    {
    case mode_no_control:
        *((int16_t *)(message+7))=0;
        *((int16_t *)(message+9))=0;
        break;
    case mode_speed:
    {
        int16_t Roll_raw=static_cast<int16_t>(round(Roll/0.122));
        *((int16_t *)(message+7))=Roll_raw;
        *((int16_t *)(message+9))=0;
    }
        break;
    case mode_angle:
    {
        int16_t Roll_raw=static_cast<int16_t>(round(Roll/0.02197));
        *((int16_t *)(message+7))=0;
        *((int16_t *)(message+9))=Roll_raw;
    }
        break;
    case mode_angle_rel_frame:
    {
        int16_t Roll_raw=static_cast<int16_t>(round(Roll/0.02197));
        *((int16_t *)(message+7))=0;
        *((int16_t *)(message+9))=Roll_raw;
    }
        break;
    }

    switch(PM)
    {
    case mode_no_control:
        *((int16_t *)(message+11))=0;
        *((int16_t *)(message+13))=0;
        break;
    case mode_speed:
    {
        int16_t Pitch_raw=static_cast<int16_t>(round(Pitch/0.122));
        *((int16_t *)(message+11))=Pitch_raw;
        *((int16_t *)(message+13))=0;
    }
        break;
    case mode_angle:
    {
        int16_t Pitch_raw=static_cast<int16_t>(round(Pitch/0.02197));
        *((int16_t *)(message+11))=0;
        *((int16_t *)(message+13))=Pitch_raw;
    }
        break;
    case mode_angle_rel_frame:
    {
        int16_t Pitch_raw=static_cast<int16_t>(round(Pitch/0.02197));
        *((int16_t *)(message+11))=0;
        *((int16_t *)(message+13))=Pitch_raw;
    }
        break;
    }

    switch(YM)
    {
    case mode_no_control:
        *((int16_t *)(message+15))=0;
        *((int16_t *)(message+17))=0;
        break;
    case mode_speed:
    {
        int16_t Yaw_raw=static_cast<int16_t>(round(Yaw/0.122));
        *((int16_t *)(message+15))=Yaw_raw;
        *((int16_t *)(message+17))=0;
    }
        break;
    case mode_angle:
    {
        int16_t Yaw_raw=static_cast<int16_t>(round(Yaw/0.02197));
        *((int16_t *)(message+15))=0;
        *((int16_t *)(message+17))=Yaw_raw;
    }
        break;
    case mode_angle_rel_frame:
    {
        int16_t Yaw_raw=static_cast<int16_t>(round(Yaw/0.02197));
        *((int16_t *)(message+15))=0;
        *((int16_t *)(message+17))=Yaw_raw;
    }
        break;
    }
    message[19]=calc_crc(message+4,15);


    test.write((char*)message,20);
}

//------------------------------------------------------------


void MainWindow::Get_angle(void)
{
    char command[]={0x3e,0x3D,0x00,0x3D,0x00};
    if(test.isWritable())
    {
        test.write(command,5);
    }
}
