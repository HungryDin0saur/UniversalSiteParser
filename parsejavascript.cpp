#include "parsejavascript.h"

//Объявления статических членов класса
QStringList ParseJavaScript::regExpForParseJS, ParseJavaScript::scripts;

ParseJavaScript::ParseJavaScript(QString profileName, QObject *parent) : QWebEnginePage(parent)
{
  timer = new QTimer;
  event = new QEventLoop;
  pas = new QRegExp;

  vsv = new VariousStrValidations;
  fo = new FileOperations;
  dwl = new Downloads;

   //Соединение для древовидного парсинга
  connect(this, &ParseJavaScript::typeNavigationRequest, this, [&](){checkJS = true;}); //Для запуска "древовидного" парсинга
    //Соединение для загрузки очередной html или другого файла в слоте ParseJavaScript::jsTreeParse
  connect(this->profile(), &QWebEngineProfile::downloadRequested, this, &ParseJavaScript::downloadRequested);

  this->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

  ParseJavaScript::settingJSToParse(profileName);

}

ParseJavaScript::~ParseJavaScript()
{
    this->deleteLater();
    regExpForParseJS.clear();
    bufPage.clear();

    delete vsv;
    delete dwl;

    scripts.clear();

    delete event;
    delete pas;
    delete timer;
}

void ParseJavaScript::settingJSToParse(QString profileName)
{
    xmlRead = new QFile("C://Parser setting/" + profileName);
    if (!xmlRead->open(QFile::ReadOnly | QIODevice::Text))
    {
        emit infoAllarm("Error", xmlRead->errorString());
        return;
    }

    xsr = new QXmlStreamReader(xmlRead);
    QXmlStreamAttributes attrib;

    while (!xsr->atEnd())
    {
       xsr->readNext();
       if (xsr->isStartElement())
       {
          if(xsr->name() == "regexpJS")
          {
              attrib = xsr->attributes();
              regExpForParseJS.append(attrib.at(0).value().toString());
          }
          if(xsr->name() == "settingsJS")
          {
              attrib = xsr->attributes();
              JSparseSettingParam.append(attrib.at(0).value().toString()); //Парсим настройки парсинга JS
          }
        if(xsr->hasError())
        {
            emit infoAllarm("Error parse XML", xsr->errorString());
            xmlRead->close();
            delete xmlRead;
            delete xsr;
            return;
        }
       }
     }

    attrib.clear();
    xmlRead->close();
    delete xmlRead;
    delete xsr;

    if(JSparseSettingParam.isEmpty())
    {
        this->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
         //открывать JavaScript всплывающие окна без участия пользователя
        this->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
         //Позволяет активировать окна с помощью метода JavaScript window.focus ()
        this->settings()->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, false);
         //Включить захват экрана
        this->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, false);
         //Ассоциация со страницей, когда происходит операция навигации
        this->settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, false);
         //Поддержка полного экрана
        this->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false); //Остановился на этом
         //ping - этот атрибут принимает список URL, разделенных пробелом, которые должны быть
         //опрошены в случае успешного перехода пользователем на URL, обозначенный в href
        this->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
         //Запрещает воспроизведение мультимедийного контента до тех пор, пока
         //пользователь не будет взаимодействовать со страницей.
        this->settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, true);
         //
         //Позволяет локально загруженным документам получать доступ к другим локальным URL-адресам
        this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
         //Позволяет локально загруженным документам игнорировать правила перекрестного происхождения
         //чтобы они могли обращаться к удаленным ресурсам
        this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        this->settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, false); //Warning!
    }
    else
    {
        this->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, JSparseSettingParam[0].toInt());
         //открывать JavaScript всплывающие окна без участия пользователя
        this->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, JSparseSettingParam[1].toInt());
         //Позволяет активировать окна с помощью метода JavaScript window.focus ()
        this->settings()->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, JSparseSettingParam[2].toInt());
         //Включить захват экрана
        this->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, JSparseSettingParam[3].toInt());
         //Ассоциация со страницей, когда происходит операция навигации
        this->settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, JSparseSettingParam[4].toInt());
         //Поддержка полного экрана
        this->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, JSparseSettingParam[5].toInt()); //Остановился на этом
         //ping - этот атрибут принимает список URL, разделенных пробелом, которые должны быть
         //опрошены в случае успешного перехода пользователем на URL, обозначенный в href
        this->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, JSparseSettingParam[6].toInt());
         //Запрещает воспроизведение мультимедийного контента до тех пор, пока
         //пользователь не будет взаимодействовать со страницей.
        this->settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, JSparseSettingParam[7].toInt());
         //Позволяет локально загруженным документам получать доступ к другим локальным URL-адресам
        this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, JSparseSettingParam[8].toInt());
         //Позволяет локально загруженным документам игнорировать правила перекрестного происхождения
         //чтобы они могли обращаться к удаленным ресурсам
        this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, JSparseSettingParam[9].toInt());
        this->settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, JSparseSettingParam[10].toInt()); //Warning!

        JSparseSettingParam.clear();
    }

    this->settings()->setAttribute(QWebEngineSettings::JavascriptCanPaste, false);
    this->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
    this->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);



    this->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
    this->settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
    this->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
    this->settings()->setAttribute(QWebEngineSettings::TouchIconsEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::PrintElementBackgrounds, false);
    this->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, false);

     //Ограничивает WebRTC только публичными IP-адресами.
    this->settings()->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, false);
     //гиперссылки в цепочке фокусоровки клавиатуры
    this->settings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, false);
    this->settings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
    this->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, false);
    this->settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, true);

    JSparseSettingParam.clear();

    return;
}

void ParseJavaScript::downloadRequested(QWebEngineDownloadItem *download)
{
    Q_ASSERT(download && download->state() == QWebEngineDownloadItem::DownloadRequested);

     //Сохраняет страницу в указанную дирректорию
    download->setSavePageFormat(QWebEngineDownloadItem::CompleteHtmlSaveFormat);
    download->setPath(download->path());
    download->accept();

    return;
}

QStringList ParseJavaScript::jsTreeParse(QStringList &bufRegExpForParseJS, const QString &url, const QString &pathToSave)
{qDebug() << "OBLATERRORSTOOPSTOOP!";
    dirProfile = new QDir;
    QUrl currentUrl; //Текущий URL, с на кт требуется запускать java скрипты
    QString buf, currentDirrectory;
    QStringList pathToSaveHtml; //Пути к сохраненным файлам при древовидном парсинге
    QStringList openUrl; //URL, полученные выполнением java скриптов
    QStringList scriptsJS; //Сюда сохраняются найденные по регулярным выражениям скрипты
      //Сюда сохраняются номера страниц, для создания дирректорий для задания названий соответствующим дирректориям
    QVector<unsigned long long int> numSavePageBuf;
    localEvent = new QEventLoop;

    connect(this, &ParseJavaScript::finishedGetHtml, localEvent, &QEventLoop::quit);
    connect(this, &QWebEnginePage::loadFinished, localEvent, &QEventLoop::quit);

    openUrl.push_back(url);

    bool downloadLock = false;

    while(!openUrl.isEmpty())
    {
        currentUrl = QUrl(openUrl.front());

        this->load(currentUrl);
        localEvent->exec();

        openUrl.pop_front();

        this->toHtml([&](const QString &result){
            buf = result;
            emit finishedGetHtml();
        });
        localEvent->exec();

        if(downloadLock==false)
        {
            currentDirrectory = currentUrl.host() + "A" + QString::number(numberSavePage);
            dirProfile->mkdir(pathToSave + currentDirrectory);

            this->download(currentUrl, pathToSave + currentDirrectory + "/" +
                           this->url().host() + "A" + QString::number(numberSavePage) + ".html");
            downloadLock = true;
        }
        else
        {
            currentDirrectory = currentUrl.host() + "A" + QString::number(numSavePageBuf.front());
            dirProfile->mkdir(pathToSave + currentDirrectory);
            numSavePageBuf.pop_front();
        }

        pathToSaveHtml.append(currentDirrectory + "/" + currentUrl.host() + "A" + QString::number(numberSavePage) + ".html");

        numberSavePage++;

         //Поиск скриптов по регулярным выражениям в buf
        for(QStringList::iterator itr=bufRegExpForParseJS.begin(); itr!=bufRegExpForParseJS.end();itr++)
        {
             scriptsJS.append(vsv->findSubStrToRegExp(*itr, buf, true));
        }

        buf.clear();
        scriptsJS.removeDuplicates(); //Удаляет одинаковые скрипты

        if(scriptsJS.isEmpty()) // Если скрипты не найдены
        {
            break;
        }

        for(QStringList::iterator itr=scriptsJS.begin(); itr!=scriptsJS.end();itr++)
        {
            ParseJavaScript::runJSorSaveFile(this ,static_cast<QString>(*itr), pathToSave);

            this->download(this->url(), pathToSave + currentDirrectory + "/"
                           + this->url().host() + "F" + QString::number(numberSavePage) + ".html");

            openUrl.push_back(this->url().url());
            numSavePageBuf.push_back(numberSavePage);

            pathToSaveHtml.append(currentDirrectory + "/" + this->url().host() + "F" + QString::number(numberSavePage) + ".html");

            numberSavePage++;

            this->load(currentUrl);
            localEvent->exec();
        }

        scriptsJS.clear();
    }

    openUrl.clear();
    scriptsJS.clear();
    buf.clear();
    delete localEvent;
    delete dirProfile;

    return pathToSaveHtml;
}


void ParseJavaScript::runJSorSaveFile(QWebEnginePage *page , const QString &script, const QString &pathToSave)  //Запускает java скрипты и сохраняет файл, если необходимо
{
    QTimer localTimer;
    QEventLoop localEvent;

    connect(&localTimer, &QTimer::timeout, &localEvent, &QEventLoop::quit);

    QString buf;

    page->runJavaScript(script, [&](const QVariant &v)
    {
        if(!v.toString().isEmpty())
        {
            reply = dwl->downloadFile_get(vsv->deleteEntityInThisUrl(QUrl::fromUserInput(qurl.host()).toString() + v.toString()),
                         "https"); //ПОКА ПО ДЕФЛТУ HTTPS

            fo->saveFile(pathToSave, reply->url().fileName() + "."
               + vsv->substr(((reply->header(QNetworkRequest::ContentTypeHeader)).toString()), "/", ";"),
                reply->readAll());
             numberSaveFile++;
        }

     });

    localTimer.start(20000);
    localEvent.exec();

    return;
}



bool ParseJavaScript::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{qDebug() << "TYPE: " << type;
    if((type == QWebEnginePage::NavigationTypeOther) && (isMainFrame==true)) //Если выполнение скрипта приводит к загрузке новой html
    {
         //Если выполнение скрипта приводит к загрузке новой html, то вызывается функция ParseJavaScript::jsTreeParse
        emit typeNavigationRequest(); //ВРЕМЕННО ОТКЛЮЧЕНО
        qDebug() << "IS EMIT!";
    }

    qDebug() << "ISMAINFRAME: " << isMainFrame;

    return true;
}



 //ПОЛУЧЕНИЕ КОНСОЛЬНЫХ СООБЩЕНИЙ ОТ JAVA SCRIPT
void ParseJavaScript::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    Q_UNUSED(lineNumber)
    Q_UNUSED(sourceID)

    switch (level) {
    default:
    case QWebEnginePage::InfoMessageLevel:
        qDebug() << "SCRIPT OUT InfoMessageLevel " << message;
        break;
    case QWebEnginePage::WarningMessageLevel:
        qWarning() << "SCRIPT OUT WarningMessageLevel " << message;
        break;
    case QWebEnginePage::ErrorMessageLevel:
        qCritical() << "SCRIPT OUT ErrorMessageLevel " << message;
        break;
    }
}




std::tuple<QString, QStringList> ParseJavaScript::parseJS(const QString &url, const QString &profileDir) //Сбор ссылок, спрятанных под java скриптами
{qDebug() << "FOR JS URL: " << url;
       QStringList bufRegExpJS = regExpForParseJS;

       scripts.clear();

       qurl.setUrl(vsv->deleteEntityInThisUrl(url));

       uint lsize = bufRegExpJS.size(); //Количество регулярных выражений

       connect(this, &QWebEnginePage::loadFinished, event, &QEventLoop::quit);

       this->load(qurl);
       event->exec();

       connect(this, &ParseJavaScript::finishedGetHtml, event, &QEventLoop::quit);

       this->toHtml([&](const QString &result){
            bufPage = result;
            emit finishedGetHtml();
       });

       event->exec();

       QStringList bufbufRegExpJS = bufRegExpJS; //Бэкапим регулярки на случай запуска древовидного поиска

       while(lsize>0)
       {
           //Поиск ссылок по регулярным выражениям в bufPage
          scripts.append(vsv->findSubStrToRegExp(bufRegExpJS.front(), bufPage, true));

           scripts.removeDuplicates(); //Удаляет одинаковые скрипты
           if(scripts.isEmpty())
           {
               lsize--;
               bufRegExpJS.pop_front(); //Удаление ненужного регулярного выражения
               continue;
           }
qDebug() << "SCRIPTS: " << scripts;
           ssize = scripts.size();
           for(uint i=0; i<ssize; i++) //Запускает найденные java скрипты
           {qDebug() << "TARGET SCRIPT: " << static_cast<QString>(scripts.at(i));
               ParseJavaScript::runJSorSaveFile(this, static_cast<QString>(scripts.at(i)), profileDir);
               if(checkJS == true) //Если скрипт загружает "новую" страницу
               {
                   pathToHtml = ParseJavaScript::jsTreeParse(bufbufRegExpJS, url, profileDir);
                   break;
               }
           }

           if(checkJS == false) //Если скрипт не загружает "новую" страницу
           {
               this->toHtml([&](const QString &result) {
                    bufPage = result;
                    emit finishedGetHtml();
               });

               event->exec();
           }

           scripts.clear();
           lsize--;
           bufRegExpJS.pop_front(); //Удаление ненужного регулярного выражения
       }

       bufbufRegExpJS.clear();

       return {bufPage, pathToHtml};
}


