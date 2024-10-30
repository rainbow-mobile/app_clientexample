#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>

void setConnected(QLabel *label){
    label->setText("connected");
    label->setStyleSheet("color:#12d27c; font-weight:bold;");
}
void setDisConnected(QLabel *label){
    label->setText("disconnected");
    label->setStyleSheet("color:red; font-weight:bold;");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()),this,SLOT(onTimer()));
    timer->start(500);

    setDisConnected(ui->lb_connect);

    connect(&client, &SocketClient::moveStart,this,&MainWindow::moveStart);
    connect(&client, &SocketClient::movePause,this,&MainWindow::movePause);
    connect(&client, &SocketClient::moveResume,this,&MainWindow::moveResume);
    connect(&client, &SocketClient::moveStop,this,&MainWindow::moveStop);
    connect(&client, &SocketClient::socketConnected, this, &MainWindow::socketConnected);
    connect(&client, &SocketClient::socketDisconnected, this, &MainWindow::socketDisconnected);
    connect(&client, &SocketClient::getStatus, this, &MainWindow::getStatus);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getStatus(QString data){
    QScrollBar *scrollBar = ui->te_status->verticalScrollBar();
    int scrollValue = scrollBar->value();
    int scrollMax = scrollBar->maximum();
    ui->te_status->setText(data);

    if (scrollValue != scrollMax) {
        scrollBar->setValue(scrollValue);
    } else {
        scrollBar->setValue(scrollBar->maximum());
    }

}

void MainWindow::onTimer(){
    on_btn_get_connection_clicked();

    if(ui->lb_connect->text() == "connected"){
        ui->cb_socket->setChecked(true);
    }else{
        ui->cb_socket->setChecked(false);
    }

    if(ui->lb_con_slamnav->text() == "connected"){
        ui->cb_slam->setChecked(true);
    }else{
        ui->cb_slam->setChecked(false);
    }

    if(client.status["state"].toObject()["map"].toString() != ""){
        ui->cb_map->setChecked(true);
    }else{
        ui->cb_map->setChecked(false);
    }

    if(client.status["state"].toObject()["localization"].toString() == "good"){
        ui->cb_local->setChecked(true);
    }else{
        ui->cb_local->setChecked(false);
    }

    ui->cb_ready->setChecked(false);
    if(client.status["motor"].toArray()[0].toObject()["status"].toString() == "1"){
        if(client.status["motor"].toArray()[1].toObject()["status"].toString() == "1"){
            ui->cb_ready->setChecked(true);
        }
    }

    ui->btn_movegoal->setDisabled(true);
    ui->btn_movetarget->setDisabled(true);
    if(ui->cb_socket->isChecked()){
        if(ui->cb_slam->isChecked()){
            if(ui->cb_map->isChecked()){
                if(ui->cb_local->isChecked()){
                    if(ui->cb_ready->isChecked()){
                        ui->btn_movegoal->setDisabled(false);
                        ui->btn_movetarget->setDisabled(false);
                    }
                }
            }
        }
    }
}
void MainWindow::moveStart(){
    qDebug() << "moveStart";
    count = 0;
    flag_stop = false;
    flag_move = true;
    flag_pause = false;
}
void MainWindow::moveStop(){
    flag_stop = true;
}
void MainWindow::movePause(){
    flag_pause = true;
}
void MainWindow::moveResume(){
    flag_pause = false;
}
void MainWindow::socketConnected(){
    setConnected(ui->lb_connect);
    initServer();
}
void MainWindow::socketDisconnected(){
    setDisConnected(ui->lb_connect);
}

void MainWindow::on_btn_connect_clicked()
{
    client.connect(ui->le_socketip->text());
}

void MainWindow::initServer(){
    on_btn_connect_clicked();
    on_btn_init_clicked();
    on_btn_getmap_clicked();
    on_btn_get_connection_clicked();
}
void MainWindow::on_btn_get_connection_clicked()
{
    GET *request = new GET(ui->le_serverip->text()+"/connection");
    connect(request, &GET::response, this, &MainWindow::getConnection);
    connect(request, &GET::response, request, &GET::deleteLater);
    connect(request, &GET::error, request, &GET::deleteLater);
}

void MainWindow::getConnection(const QByteArray &data){
    QJsonObject json_data = QJsonDocument::fromJson(data).object();
    if(json_data["SLAMNAV"] == true){
        setConnected(ui->lb_con_slamnav);
    }else{
        setDisConnected(ui->lb_con_slamnav);
    }
    if(json_data["TASK"] == true){
        setConnected(ui->lb_con_taskman);
    }else{
        setDisConnected(ui->lb_con_taskman);
    }
}
void MainWindow::getMaps(const QByteArray &data){
    QJsonArray json_data = QJsonDocument::fromJson(data).array();

    ui->cb_maps->clear();
    for(int i=0; i<json_data.size(); i++){
        ui->cb_maps->addItem(json_data[i].toObject()["name"].toString());
        if(json_data[i].toObject()["name"].toString() == client.status["state"].toObject()["map"].toString()){
            ui->cb_maps->setCurrentIndex(i);
        }
    }


}
void MainWindow::getPresets(const QByteArray &data){
    QJsonArray json_data = QJsonDocument::fromJson(data).array();
    ui->cb_preset_goal->clear();
    ui->cb_preset_target->clear();
    for(int i=0; i<json_data.size(); i++){
        ui->cb_preset_goal->addItem(QString::number(json_data[i].toInt()));
        ui->cb_preset_target->addItem(QString::number(json_data[i].toInt()));
    }
}
void MainWindow::loadMapSuccess(const QByteArray &data){
    QJsonObject json_data = QJsonDocument::fromJson(data).object();
    mapname = json_data["name"].toString();
    qDebug() << "Map Load Success" << mapname;
    on_btn_getgoals_clicked();

}
void MainWindow::initSuccess(const QByteArray &data){
    qDebug() << "Init Success" << data;
}
void MainWindow::getGoals(const QByteArray &data){
    QJsonArray json_data = QJsonDocument::fromJson(data).array();

    ui->cb_goals->clear();
    for(int i=0; i<json_data.size(); i++){
        ui->cb_goals->addItem(json_data[i].toString());
    }
}
void MainWindow::goalMove(const QByteArray &data){
    qDebug() << "goalMove" << data;
}




void MainWindow::on_btn_getmap_clicked()
{
    GET *request = new GET(ui->le_serverip->text()+"/map/list");
    connect(request, &GET::response, this, &MainWindow::getMaps);
    connect(request, &GET::response, request, &GET::deleteLater);
    connect(request, &GET::error, request, &GET::deleteLater);
}


void MainWindow::on_btn_loadmap_clicked()
{
    QJsonObject data;
    data["name"] = ui->cb_maps->currentText();
    POST *request = new POST(ui->le_serverip->text()+"/map/current", data);
    connect(request, &POST::response, this, &MainWindow::loadMapSuccess);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);
}


void MainWindow::on_btn_init_clicked()
{
    QJsonObject data;
    data["command"] = "semiautoinit";
    POST *request = new POST(ui->le_serverip->text()+"/localization", data);
    connect(request, &POST::response, this, &MainWindow::initSuccess);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);
}


void MainWindow::on_btn_getgoals_clicked()
{
    GET *request = new GET(ui->le_serverip->text()+"/map/goal/"+client.status["state"].toObject()["map"].toString());
    connect(request, &GET::response, this, &MainWindow::getGoals);
    connect(request, &GET::response, request, &GET::deleteLater);
    connect(request, &GET::error, request, &GET::deleteLater);
    GET *request2 = new GET(ui->le_serverip->text()+"/setting/preset/"+client.status["setting"].toObject()["platform_type"].toString());
    connect(request2, &GET::response, this, &MainWindow::getPresets);
    connect(request2, &GET::response, request2, &GET::deleteLater);
    connect(request2, &GET::error, request2, &GET::deleteLater);
}


void MainWindow::on_btn_set_clicked()
{
    initServer();
}


void MainWindow::on_btn_movegoal_clicked()
{
    QJsonObject data;
    data["command"] = "goal";
    data["id"] = ui->cb_goals->currentText();
    data["preset"] = ui->cb_preset_goal->currentText();
    data["method"] = "pp";
    POST *request = new POST(ui->le_serverip->text()+"/control/move", data);
    connect(request, &POST::response, this, &MainWindow::goalMove);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);

}


void MainWindow::on_btn_movetarget_clicked()
{
    QJsonObject data;
    data["command"] = "target";
    data["x"] = ui->le_target_x->text();
    data["y"] = ui->le_target_y->text();
    data["z"] = "0";
    data["rz"] = ui->le_target_rz->text();
    data["preset"] = ui->cb_preset_goal->currentText();
    data["method"] = "pp";
    POST *request = new POST(ui->le_serverip->text()+"/control/move", data);
    connect(request, &POST::response, this, &MainWindow::goalMove);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);
}


void MainWindow::on_btn_pause_clicked()
{
    QJsonObject data;
    data["command"] = "pause";
    POST *request = new POST(ui->le_serverip->text()+"/control/move", data);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);
}


void MainWindow::on_btn_resume_clicked()
{
    QJsonObject data;
    data["command"] = "resume";
    POST *request = new POST(ui->le_serverip->text()+"/control/move", data);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);
}


void MainWindow::on_btn_stop_clicked()
{
    QJsonObject data;
    data["command"] = "stop";
    POST *request = new POST(ui->le_serverip->text()+"/control/move", data);
    connect(request, &POST::response, request, &GET::deleteLater);
    connect(request, &POST::error, request, &GET::deleteLater);
}

