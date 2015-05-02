#include "mainwindow.h"
#include "ui_mainwindow.h"

//конструктор
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , m_nNextBlockSize(0)
{
    ui->setupUi(this);

    // создаем сервер
    // позволяет получать информацию с некоторого порта сервера.
    // Данный класс испускает сигнал newConnection() каждый раз, когда клиент пытается соединиться с сервером.
    // После того, как связь установлена можно общаться с клиентом использующим QTcpSocket
    server = new QTcpServer(this);
    //количество клиентов
    client_i=0;

}

//деструктор
MainWindow::~MainWindow()
{
    //закрываем сокет
    server->close();
    delete ui;
}

//клиент подключился
void MainWindow::newConnection()
{

    //добавляем количество клиентов на единицу
    client_i++;
    //создаем сокет QTcpSocket класс типа для клиента
    socket[client_i] = new QTcpSocket(this);
    //qDebug() << "ID = " <<client_i;
    socket[client_i]=server->nextPendingConnection(); // Возвращает указатель на экземпляр QTcpSocket
    QString ClientID="ID="+QString::number(client_i); // Строка будет имет вид ID=N

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << ClientID;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket[client_i]->write(arrBlock);// записываем в сокет получателя сообщение

    //ждём сообшений от него
    connect(socket[client_i], SIGNAL(readyRead()), this, SLOT(readMessage()));
}

//пришло сообшение
void MainWindow::readMessage()
{
    int otpr=0; // id отправителя
    QString recvmessage = "";
    QString clientID;
    for(int i=1;i<=client_i;i++)
    {
        QDataStream in(socket[i]);
        in.setVersion(QDataStream::Qt_4_5);
        for (;;) {
            if (!m_nNextBlockSize) {
                if (socket[i]->bytesAvailable() < sizeof(quint16)) {
                    break;
                }
                in >> m_nNextBlockSize;
            }

            if (socket[i]->bytesAvailable() < m_nNextBlockSize) {
                break;
            }
            QString str;
            in >> str;

            recvmessage += str;
            otpr=i;
            m_nNextBlockSize = 0;

            //qDebug() <<"2 str = "<<recvmessage;
            clientID=recvmessage.mid(0,recvmessage.indexOf(':')); // парсим из строки ID
            //qDebug() <<"3";
            recvmessage=recvmessage.mid(recvmessage.indexOf(':')+1,recvmessage.length()); // парсим из строки сообщениеы
            //qDebug() <<"4 ID = " <<clientID;

            //*******************************
            if(clientID.toInt()>0 && clientID.toInt()<=client_i)
            {
                QByteArray  arrBlock;
                QDataStream out(&arrBlock, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_4_5);
                out << quint16(0) << recvmessage;

                out.device()->seek(0);
                out << quint16(arrBlock.size() - sizeof(quint16));

                socket[clientID.toInt()]->write(arrBlock);// записываем в сокет получателя сообщение
            }
        }

    }
	
	
    //показываем от кого и куда отправили
    if(clientID.toInt()>0 && clientID.toInt()<=client_i)
    {
    ui->plainTextEdit_2->appendPlainText(QString::number(otpr)+" -> "+clientID + "::" + recvmessage);
    }

}

//при нажатеи кнопки "start server"
void MainWindow::on_pushButton_clicked()
{
    //создает подключенияе SIGNAL SLOT ждет подключения клиента
    connect(server,SIGNAL(newConnection()),this, SLOT(newConnection()));
    //запускаем лиснер (подключится можно с любого IP на порт 4563)
    if(!server->listen(QHostAddress::Any,4563))
    {
        ui->plainTextEdit->appendPlainText("NO STARTED");
    }
    else
    {
        ui->plainTextEdit->appendPlainText("STARTED");
        ui->plainTextEdit_2->appendPlainText("WAIT MESSAGE");
    }
}
