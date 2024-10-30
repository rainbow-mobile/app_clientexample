#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTimer>
#include "socket.h"
#include "api_get.h"
#include "api_post.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool flag_move = false;
    bool flag_pause = false;
    bool flag_stop = false;
    int count=0;

    QString mapname = "";
    void initServer();

private Q_SLOTS:
    void onTimer();
    void moveStart();
    void movePause();
    void moveResume();
    void moveStop();
    void on_btn_connect_clicked();
    void socketConnected();
    void socketDisconnected();
    void getStatus(QString data);

    void on_btn_get_connection_clicked();

    void getConnection(const QByteArray &data);
    void getMaps(const QByteArray &data);
    void getGoals(const QByteArray &data);
    void getPresets(const QByteArray &data);
    void loadMapSuccess(const QByteArray &data);
    void initSuccess(const QByteArray &data);
    void goalMove(const QByteArray &data);

    void on_btn_getmap_clicked();

    void on_btn_loadmap_clicked();

    void on_btn_init_clicked();

    void on_btn_getgoals_clicked();

    void on_btn_set_clicked();

    void on_btn_movegoal_clicked();

    void on_btn_movetarget_clicked();

    void on_btn_pause_clicked();

    void on_btn_resume_clicked();

    void on_btn_stop_clicked();

private:
    Ui::MainWindow *ui;
    SocketClient client;
    QTimer *timer;
};
#endif // MAINWINDOW_H
