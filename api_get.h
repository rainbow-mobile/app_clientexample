#ifndef API_GET_H
#define API_GET_H
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QObject>
#include <QDebug>

class GET : public QObject {
    Q_OBJECT

public:
    GET(QString _url) {
        // GET 요청 수행
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QUrl url(_url);
        QNetworkRequest request(url);
        // qDebug() << "GET : " << _url;
        connect(manager, &QNetworkAccessManager::finished, this, &GET::onGetReply);
        manager->get(request);
    }

Q_SIGNALS:
    void response(const QByteArray &data);
    void error(const QByteArray &data);

private Q_SLOTS:
    void onGetReply(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            response(responseData);
        } else {
            QByteArray responseData = reply->readAll();
            qDebug() << "onGetReply Error : " << responseData;
            error(responseData);
        }
        reply->deleteLater();
    }
};

#endif // API_GET_H
