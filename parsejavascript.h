#ifndef PARSEJAVASCRIPT_H
#define PARSEJAVASCRIPT_H

#include "variousstrvalidations.h"
#include "fileoperations.h"
#include "downloads.h"

#include <tuple>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QThread>
#include <QXmlStreamReader>
#include <QEventLoop>
#include <QFile>
#include <QUrl>
#include <QMessageBox>
#include <QTextDocument>
#include <QDebug>
#include <QTimer>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineDownloadItem>
#include <QWebEngineProfile>
#include <QNetworkReply>
#include <QList>
#include <QDir>

class ParseJavaScript : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit ParseJavaScript(QString profileName, QObject *parent = nullptr);
    ~ParseJavaScript();

private:
    QFile *xmlRead; //Для считывания профиля настроек
    QXmlStreamReader *xsr; //Парсинг xml файла настроек

    QStringList JSparseSettingParam; //Настройки парсинга java script

    QString bufPage; //Буфер сохранения результатов работы QWebEnginePage::toHtml

    uint depthFindandRunJS; //глубина поиска и запуска JS на одной странице НЕ НУЖНО

     //для парсинга JS
    QTimer *timer;
    QEventLoop *event;
    QRegExp *pas;

    //Указывает, нужно перелистывать страницы java скриптом или нет
    bool checkJS = false;

   //Получение ответов от java скриптов(использовался для отладки)
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                  const QString &message,
                                  int lineNumber,
                                  const QString &sourceID) override;


    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;

    uint ssize; //Количество найденных скриптов
    uint lsize; //Количество регулярных выражений

    quint64 numberSaveFile = 0; //Номер файла, сохраняемого скриптом
    quint64 numberSavePage = 0; //Номер страницы, загружаемой скриптом

    QUrl qurl;

    QStringList pathToHtml; //Имена HTML скачанных посредством запуска java скриптов
    static QStringList regExpForParseJS;  //Регулярки для парсинга java скриптов
    static QStringList scripts; //Найденные на странице java скрипты

    QByteArray urlDownload; //Сюда сохраняются скачанные страницы или файлы

    VariousStrValidations *vsv;
    FileOperations *fo;
    Downloads *dwl;

    QNetworkReply *reply;

    QDir *dirProfile;
    QEventLoop *localEvent;

signals:
    void infoAllarm(QString hendlerMes, QString bodyMes);    
    void finishedGetHtml(); //Испускается после окончания работы ф-и toHtml

    void typeNavigationRequest();

private slots:
    void settingJSToParse(QString profileName);

    //Запускает js скрипты на странице и сохраняет ее экземпляр
    //в файл после запуска очередно скрипта
    QStringList jsTreeParse(QStringList &bufRegExpForParseJS,
                     const QString &url, const QString &pathToSave);

      //Запускает java скрипты и сохраняет файл, если необходимо
    void runJSorSaveFile(QWebEnginePage *page, const QString &script, const QString &pathToSave);

     //Обработчик загрузок
    void downloadRequested(QWebEngineDownloadItem*download);


public slots:
    std::tuple<QString, QStringList> parseJS(const QString &url, const QString &profileDir);

};

#endif // PARSEJAVASCRIPT_H
