#ifndef VARIOUSSTRVALIDATION_H
#define VARIOUSSTRVALIDATION_H

#include <QObject>
#include <QUrl>
#include <QTextDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QRegularExpression>

class VariousStrValidations : public QTextDocument
{
    Q_OBJECT
public:
    explicit VariousStrValidations();
    ~VariousStrValidations();

private:
    QUrl *qurl;

    QStringList result;

signals:

    void infoAllarm(QString hendlerMes, QString bodyMes); //Сигнал в gui для вывода информации

public slots:

    bool isExistingUrl(QString url);
    QString urlValidation(QStringList &baseUrl, QString &relativeUrl, bool getValigation); //Соединение baseURL и relativeURL

    QString substr(QString str, QString x, QString y); //Вытаскивает подстроку без учета крайних символов;

    QString deleteEntityInThisUrl(const QString url); //Удаляет не нужные сущности из url

     //Поиск по регулярным выражениям
    QStringList findSubStrToRegExp(const QString &pattern, const QString &document, bool delHtmlEntity);
};

#endif // VARIOUSSTRVALIDATION_H
