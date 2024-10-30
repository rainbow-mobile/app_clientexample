#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <iostream>
#include <QDateTime>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <sio_client.h>

#define BIND_EVENT(IO,EV,FN) \
IO->on(EV,FN)
    using namespace sio;

class SocketClient : public QObject
{
    Q_OBJECT

public:
    explicit SocketClient(QObject *parent = nullptr) :
        QObject(parent),
        io(new client())
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;
        using std::placeholders::_4;

        // "mappingstart" 이벤트 핸들러 설정
        sio::socket::ptr sock = io->socket();
        BIND_EVENT(sock,"status",std::bind(&SocketClient::onStatus,this,_1,_2,_3,_4));

        // WebSocket 클라이언트 설정 및 연결
        io->set_open_listener(std::bind(&SocketClient::onConnected, this));
        io->set_close_listener(std::bind(&SocketClient::onDisconnected, this, std::placeholders::_1));
        io->set_fail_listener(std::bind(&SocketClient::onError, this));
    }

    QString getJson(sio::message::ptr const& data,QString key){
        qDebug() << key << data->get_map()[key.toStdString()].get();
        return QString::fromStdString(data->get_map()[key.toStdString()]->get_string());
    }

    void connect(QString socket_ip){
        // WebSocket 서버에 연결
        io->connect(socket_ip.toStdString());
    }

    bool connected = false;
    QJsonObject status;

Q_SIGNALS:
    void moveStart();
    void movePause();
    void moveResume();
    void moveStop();
    void socketConnected();
    void socketDisconnected();
    void getStatus(QString data);

private Q_SLOTS:
    // 연결 성공 시 호출될 슬롯
    void onConnected()
    {
        std::cout << "WebSocket connected" << std::endl;
        connected = true;
        socketConnected();
    }

    // 연결 종료 시 호출될 슬롯
    void onDisconnected(sio::client::close_reason const& reason)
    {
        std::cout << "WebSocket disconnected" << std::endl;
        connected = false;
        socketDisconnected();
    }

    // 연결 오류 시 호출될 슬롯
    void onError()
    {
        std::cout << "WebSocket error" << std::endl;
    }

    void onStatus(std::string const& name,message::ptr const& data,bool hasAck,message::list &ack_resp)
    {
        if (data->get_flag() == sio::message::flag_string) {
            // 메시지가 문자열일 때
            QString jsonString = QString::fromStdString(data->get_string());

            // JSON 문자열을 QJsonDocument로 변환
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
            status = jsonDoc.object();
            getStatus(jsonDoc.toJson(QJsonDocument::Indented));
        } else {
            qDebug() << "Received message is not a string.";
        }
    }

private:
    std::unique_ptr<client> io;
public:
    void runTimer(){
        std::cout << "Timer Run" << std::endl;
        // JSON 데이터를 문자열 메시지로 변환하여 전송
        sio::message::ptr response = sio::string_message::create("HI");
        io->socket()->emit("move", response);
    }
    void moveDone(QString result){
        QJsonObject json;
        json["result"] = result;

        sio::message::ptr response = sio::string_message::create(QJsonDocument(json).toJson().toStdString());
        io->socket()->emit("move", response);
    }
};

#endif // SOCKET_H
