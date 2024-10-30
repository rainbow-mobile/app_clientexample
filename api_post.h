#ifndef API_POST_H
#define API_POST_H
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QDebug>

class POST : public QObject {
    Q_OBJECT

public:
    POST(QString _url, QJsonObject _data) {
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QUrl url(_url);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); // JSON 형식으로 설정

        QByteArray jsonData = QJsonDocument(_data).toJson();

        qDebug() << _url << _data;
        connect(manager, &QNetworkAccessManager::finished, this, &POST::onPostReply);
        manager->post(request, jsonData);
    }

Q_SIGNALS:
    void response(const QByteArray &data);
    void error(const QByteArray &data);

private Q_SLOTS:
    void onPostReply(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            response(responseData);
        } else {
            QByteArray responseData = reply->readAll();
            qDebug() << "onPostReply Error : " << reply->errorString();
            error(responseData);
        }
        reply->deleteLater();
    }
};
#endif // API_POST_H
