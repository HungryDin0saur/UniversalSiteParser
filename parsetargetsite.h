#ifndef PARSEALLSITE_H
#define PARSEALLSITE_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QEventLoop>
#include <QFile>
#include <QUrl>

#include "downloads.h"
#include "fileoperations.h"
#include "parsejavascript.h"
#include "variousstrvalidations.h"

class ParseTargetSite : public QObject
{
    Q_OBJECT
public:
    explicit ParseTargetSite(QObject *parent = nullptr);
    ~ParseTargetSite();

public slots:
    void run();

    void selectProfile(QString profile);

private slots:
    void parseSite(QString profile);

    void fingRegExp(QString profile); //Ищет регулярки в исходной странице, вызывается при обычном парсинге
    void fingRegExp(QString profile, quint64 itrlinksForJSParse); // 1-я перегрузка для парсинга JS
    void fingRegExp(QString profile, QString url); // 2-я перегрузка для парсинга JS

    void findLinksInSaveTOJS(QStringList &path, QStringList &result); //Ищет ссылки в файлах, скачанных функцией ParseJavaScript::jsTreeParse

    void whileParse(QString profile);  //Функция с реализацией цикла парсинга вглубину

    void leafingThroughUrl(QString profile); // Перелистывает страницы через url

private:
    QWebEnginePage *wep;
    bool runJS; //Парсить JS или нет
    ParseJavaScript *pjs; //Класс парсинга java script
    QStringList linksForJSParse; //Ссылки для парсинга JS


    QNetworkReply *reply;
    QNetworkAccessManager *manager;

    bool blockEmit = 0; //Block emit signals: callfingRegExp, callwhileParse

    quint64 ItrwhileParse;  //Iterator parse in depth

    QByteArray urlDownload; //Сюда сохраняются скачанные страницы или файлы
    QString bufHTMLtoFind;//(Buf for save page)(=//= for find to regexp page)

    QFile *xmlRead, *writePage;
    QXmlStreamReader *xsr;
    QXmlStreamAttributes attributes;

    QStringList links; //Links to other page
    //Path to save html for processing fingRegExp(QString profile)
    QStringList pathToLink; //Сохранения путей к скачанным html для поиска в них ссылок по регулярккам

    QStringList pathToHtml; //Пути к html, скачанным посредством запуска java скриптов


    QStringList profileParam; //Buf read parametrs from xml
    //Param to profile
    short int setNextPageMode = 0;//default not select no one setting

    quint64 maxPages = 0;
    quint64 minPages = 0;
    QString RangePages; //Выражение для перелистывания страниц через url
    int parseDepth;   //parsing dept

    bool existingUrl; //Вкл/выкл проверки URL на существование

    QString targetUrl;
    QString profileName;
    QString profileDir; //Путь к дирректории для сохранения сайта

    QString followingPagesViaUrl; //Переличстывание страниц через изменение url
    bool fpvu = false; //Перелистывать?: да/нет
    QStringList UrlSwypeRegExp; //Для перелистывания страниц через url(регулярки страниц)
    QRegExp *regExp;

    QStringList regExpForParsePostTitle; //Регулярки для парсинга ссылок

    VariousStrValidations *vsv;
    QStringList BaseForFixURL; //URLs-предки для исправления некорректных ссылок

    FileOperations *fo;
    Downloads *dwd;

    QString bufHTML; //Сюда сохраня страница, обработанная ParseJavaScript::parseJS
    quint64 counter=0; //Нужен для задания путей к файлам

signals:
    void infoAllarm(QString hendlerMes, QString bodyMes); //Сигнал в gui для вывода информации
    void appendToTextBr(QStringList); //Добавляет найденные ссылки в TextBrowser

    void callparseSite(QString profile);

    void callfindRegExp(QString profile);
    void callfindRegExp(QString profile, QString url);

    void callwhileParse(QString profile);

    void finishedPAS(); //Сигнал об окончании парсинга
};

#endif // PARSEALLSITE_H
