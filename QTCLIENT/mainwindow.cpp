#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>

//конструктор
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , m_nNextBlockSize(0)

{
    ui->setupUi(this);
    //создаем сокет QTcpSocket класс типа
    socket = new QTcpSocket(this);
    //ждём сообшений от сервера
    connect(socket, SIGNAL(readyRead()), this, SLOT(newConnection()));
    ui->pushButton->setDisabled(true);
    first=0; // флаг первого сообщения/подключения
}
//деструктор
MainWindow::~MainWindow()
{
//закрываем сокет
    socket->close();
    delete ui;
}

//пришло сообшение
void MainWindow::newConnection()
{
    //читаем и показывем на экране
    QString recvmessage = "";
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (socket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (socket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QString str;
        in >> str;

        recvmessage += str;
        m_nNextBlockSize = 0;
    }
    if(first==0)
    {
        QString clientID=recvmessage.mid(recvmessage.indexOf('=')+1,recvmessage.length()); // достаем ID из строки
        ui->lineEdit_4->setText(clientID); // отображаем ID в label Your ID
        first=1;
    }
    else
    {
        ui->plainTextEdit->appendPlainText(recvmessage); // отображаем строку в plainTextEdit
    }
    //*****************************
}

//при нажатии кнопки "send"
void MainWindow::on_pushButton_clicked()
{
    //отправляем слова
    QString sendmesage;
    sendmesage= ui->lineEdit_3->text() + ":" + ui->lineEdit_2->text(); //отправка в виде ID:Text

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << sendmesage;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(ui->lineEdit->text().length()>5)
    {
        //подключаемся к указному IP адресу
        socket->connectToHost(ui->lineEdit->text(), 4563);
        ui->pushButton->setDisabled(false);
    }
}
