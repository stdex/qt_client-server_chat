#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void newConnection();
    void readMessage();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *socket[100];
    int client_i;
    quint16     m_nNextBlockSize;
};

#endif // MAINWINDOW_H
