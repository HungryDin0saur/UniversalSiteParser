#include "downloads.h"
#include <QThread>

Downloads::Downloads()
{
    event = new QEventLoop;
    request = new QNetworkRequest;
    qurl = new QUrl;
}

Downloads::~Downloads()
{
    //reply->close();
    //reply->deleteLater();

    delete event;
}

void Downloads::requestSslError(const QList<QSslError> &sslErrorList)
{
QString result = "SSL Errors: ";
QSslError sslerror;

foreach (sslerror, sslErrorList) {
result.append(sslerror.errorString()).append("\n");
}
    qDebug() << result;
}


QNetworkReply *Downloads::downloadFile_get(const QString &url, const QString &scheme)
{
    qurl->setUrl(url);
    qurl->setScheme(scheme);

    request->setUrl(*qurl);

/*
     //Отключить верификацию ssl (использовалось для отладки)
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    //config.setProtocol(QSsl::TlsV1_3);
    request->setSslConfiguration(config);
*/
/*
     //Генерация заголовка запроса (использовалось для отладки)
    QList<QNetworkCookie> cookies;
    QNetworkCookie cookie;
    cookie.setName("ASP.NET_SessionId");
    cookie.setValue("ayx25q554igb5w55ayerkrng");
    cookies.append(cookie);
    cookie.setName("History");
    cookie.setValue("Toyota");
    cookies.append(cookie);

    request->setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request->setAttribute(QNetworkRequest::ConnectionEncryptedAttribute, true);
    request->setRawHeader("Connection", "keep-alive");
    request->setRawHeader("Cache-Control", "max-age=0");
    request->setRawHeader("DNT", "1");
    request->setRawHeader("Upgrade-Insecure-Requests", "1");
    request->setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
                      "(KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36");
    request->setRawHeader("Sec-Fetch-Dest", "document");
    request->setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,");

    request->setRawHeader("Sec-Fetch-Site", "none");
    request->setRawHeader("Sec-Fetch-Mode", "navigate");
    request->setRawHeader("Sec-Fetch-User", "?1");
    request->setRawHeader("Accept-Encoding", "gzip, deflate, br");
    request->setRawHeader("Accept-Language", "ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7");

    request->setHeader(QNetworkRequest::CookieHeader, qVariantFromValue(cookies));
*/

    reply = this->get(*request);

    connect(reply, &QNetworkReply::sslErrors, this, &Downloads::requestSslError);
    connect(reply, &QNetworkReply::finished, event, &QEventLoop::quit); //НЕ ВСЕГДА ВЫДАЕТ ОШИБКУ ПРИ НЕВАЛИДНЫХ URL

    event->exec();

//qDebug() << "STATUS CODE: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//qDebug() << "Error: " << reply->error();
//qDebug() << "Header: " << reply->header(QNetworkRequest::SetCookieHeader).toList().size();


    if (reply->error() != QNetworkReply::NoError) //При возникновении ошибки
    {
        emit infoAllarm("Error", reply->errorString());
        return nullptr;
    }
    if(!(reply->isFinished()))
    {
        emit infoAllarm("Error", "Unexpected error");
        return nullptr;
    }

    return reply;
}
