#ifndef DOWNLOADS_H
#define DOWNLOADS_H

#include <QFile>
#include <QObject>
#include <QEventLoop>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>


class Downloads : public QNetworkAccessManager
{
    Q_OBJECT

public:
    Downloads();
    ~Downloads();

signals:
    void infoAllarm(const QString &, const QString &);

private:
    QEventLoop *event;
    QNetworkReply *reply;
    QNetworkRequest *request;
    QUrl *qurl;

public slots:
    QNetworkReply *downloadFile_get(const QString &url, const QString &scheme);

    void requestSslError(const QList<QSslError> &sslErrorList);
};

#endif // DOWNLOADS_H
