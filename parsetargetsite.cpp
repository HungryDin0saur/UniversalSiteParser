#include "parsetargetsite.h"

ParseTargetSite::ParseTargetSite(QObject *parent) : QObject(parent)
{
   //ПРИ УДАЛЕНИИ КЛАССА СИГНАЛЫ УДАЛЯЮТСЯ АВТОМАТИЧЕСКИ
   connect(this, &ParseTargetSite::callparseSite, this, (&ParseTargetSite::parseSite));
   connect(this, &ParseTargetSite::callwhileParse, this, &ParseTargetSite::whileParse);

   connect(this, static_cast<void(ParseTargetSite::*)(QString)>(&ParseTargetSite::callfindRegExp),
           this, static_cast<void(ParseTargetSite::*)(QString)>(&ParseTargetSite::fingRegExp));

   connect(this, static_cast<void(ParseTargetSite::*)(QString, QString)>(&ParseTargetSite::callfindRegExp),
           this, static_cast<void(ParseTargetSite::*)(QString, QString)>(&ParseTargetSite::fingRegExp));

   regExp = new QRegExp;

   fo = new FileOperations; //Считать/записать файл

   dwd = new Downloads;

   vsv = new VariousStrValidations; //Валидация ссылок и прочих строк

   connect(vsv, &VariousStrValidations::infoAllarm, this, &ParseTargetSite::infoAllarm);
   connect(dwd, &Downloads::infoAllarm, this, &ParseTargetSite::infoAllarm);
   connect(fo, &FileOperations::infoAllarm, this, &ParseTargetSite::infoAllarm);
}

ParseTargetSite::~ParseTargetSite()
{
    delete regExp;

    delete vsv; //Валидация ссылок и прочих строк

    delete fo;

    delete dwd;

    regExpForParsePostTitle.clear();
    if(runJS == true)
    {
       delete pjs;
    }
}

void ParseTargetSite::run()
{
    return;
}

/*
    Select data from xml profile
 */
void ParseTargetSite::selectProfile(QString profile)
{
    xmlRead = new QFile("C://Parser setting/" + profile);
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
          if(xsr->name() == "pro")
          {
              attrib = xsr->attributes();
              profileParam.append(attrib.at(0).value().toString());
          }
          if(xsr->name() == "regexp")
          {
              attrib = xsr->attributes();
              regExpForParsePostTitle.append(attrib.at(0).value().toString());
          }
          if(xsr->name() == "fpvure") //URL для перелистывания страниц через url
          {
              attrib = xsr->attributes();
              UrlSwypeRegExp.append(attrib.at(0).value().toString());
          }
          if(xsr->name() == "bffu") //URLs-предки для исправления некорректных ссылок
          {
              attrib = xsr->attributes();
              BaseForFixURL.append(attrib.at(0).value().toString());
          }
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

    targetUrl = profileParam.at(0);
    profileDir = profileParam.at(1);
    setNextPageMode = profileParam.at(2).toInt();  //УДАЛИТЬ И ИЗ СЕТТИНГОВ ТОЖЕ
    RangePages = profileParam.at(3);
    parseDepth = profileParam.at(4).toInt();
    existingUrl = profileParam.at(5).toInt(); //Вкл/выкл проверки URL на существование

    followingPagesViaUrl = profileParam.at(6); //Переличстывание страниц через изменение url
    if(followingPagesViaUrl != "=====")
    {
        fpvu = true;
    }

    runJS = profileParam.at(7).toInt();

    attrib.clear();
    profileParam.clear();
    xmlRead->close();
    delete xmlRead;
    delete xsr;

    //Если активирован парсинг JS
   if(runJS == true)
   {
      pjs = new ParseJavaScript(profile);

      connect(pjs, &ParseJavaScript::infoAllarm, this, &ParseTargetSite::infoAllarm, Qt::QueuedConnection);
   }

    profile = profile.left(profile.indexOf("."));

    emit callparseSite(profile);

    return;
}




void ParseTargetSite::parseSite(QString profile)
{
    targetUrl = vsv->urlValidation(BaseForFixURL, targetUrl, existingUrl);

    reply = dwd->downloadFile_get(targetUrl, "https");
    if(reply != nullptr) //Если запрос успешен
    {
        urlDownload = reply->readAll();

       //Третье слагаемое для установки правилного расширения сохраняемого файла
      writePage = new QFile(profileDir + profile + "." + vsv->substr(((reply->header(QNetworkRequest::ContentTypeHeader)).toString()), "/", ";"));
      if(writePage->open(QFile::WriteOnly))
      {qDebug() << "FILE: " << writePage->fileName();
           writePage->write(urlDownload);
      }
      else
      {
           emit infoAllarm("Error", writePage->errorString());
           return;
      }

      writePage->close();
      writePage->deleteLater();
      delete writePage;
      urlDownload.clear();
    }

    delete reply;

    if(fpvu == true)  //Переличстывание страниц через изменение url
    {
        for(int i=0; i < UrlSwypeRegExp.size(); i++)
        {
            regExp->setPattern(UrlSwypeRegExp.at(i));
            regExp->setMinimal(false);
            if(regExp->exactMatch(targetUrl) == true)
            {
               ParseTargetSite::leafingThroughUrl(profile);
               break;
            }
        }
    }

    if(blockEmit == 0)
    {
       if(runJS == true) //Если необходимо запускать java скрипты
       {
           emit callfindRegExp(profile, targetUrl);
       }
       else
       {
          emit callfindRegExp(profile);
       }
    }

    return;
}





void ParseTargetSite::fingRegExp(QString profile)
{
    bufHTMLtoFind = fo->readFile(profileDir, profile, ".html"); //Открываем файл для поиска html

     //Поиск ссылок по регулярным выражениям в bufHTMLtoFind
    for(QStringList::iterator itr=regExpForParsePostTitle.begin(); itr!=regExpForParsePostTitle.end();itr++)
    {
        links.append(vsv->findSubStrToRegExp(*itr, bufHTMLtoFind, false));
    }

    bufHTMLtoFind.clear();

     //Удаляет одинаковые ссылки
    links.removeDuplicates();

     //Добавляет найденные ссылки в TextBrowser
    emit appendToTextBr(links);

    if((blockEmit == 0) && (parseDepth > 1))
    {
       emit callwhileParse(profile);
    }
    else if(blockEmit == 0)
    {
        emit infoAllarm("Exelent work!", "Site is save");
        emit finishedPAS();
    }

    return;
}


void ParseTargetSite::fingRegExp(QString profile, quint64 itrlinksForJSParse)// 1-я перегрузка для парсинга JS
{
    std::tie(bufHTML, pathToHtml) = pjs->parseJS(vsv->urlValidation(BaseForFixURL,linksForJSParse[itrlinksForJSParse], existingUrl), profileDir); //Парсинг java скриптов

    if(!pathToHtml.isEmpty())
    {
        findLinksInSaveTOJS(pathToHtml, links); //Ищет ссылки в файлах, скачанных функцией ParseJavaScript::jsTreeParse
    }

    fo->saveFile(profileDir, QUrl(linksForJSParse[itrlinksForJSParse]).host() + QString::number(counter) +"FRE.html", bufHTML);
    counter++;

     //Поиск ссылок по регулярным выражениям в bufHTMLtoFind
    for(QStringList::iterator itr=regExpForParsePostTitle.begin(); itr!=regExpForParsePostTitle.end();itr++)
    {qDebug() << "PATTERN: " << *itr;
       links.append(vsv->findSubStrToRegExp(*itr, bufHTML, false));
    }
qDebug() << "LINKS1: " << links;
     //Удаляет одинаковые ссылки
    links.removeDuplicates();

    if(links.indexOf(linksForJSParse[itrlinksForJSParse]) != -1)
    {
        links.removeAt(links.indexOf(linksForJSParse[itrlinksForJSParse]));
    }
qDebug() << "LINKS2: " << links;
     //Добавляет найденные ссылки в TextBrowser
    emit appendToTextBr(links);

    if((blockEmit == 0) && (parseDepth > 1))
    {
       emit callwhileParse(profile);
    }
    else if(blockEmit == 0)
    {
        emit infoAllarm("Exelent work!", "Site is save");
        emit finishedPAS();
    }
qDebug() << "END_MODUL_fingRegExp_1";
    return;
}

void ParseTargetSite::fingRegExp(QString profile, QString url)// 2-я перегрузка для парсинга JS
{qDebug() << "MODUL_fingRegExp_2";
    std::tie(bufHTML, pathToHtml) = pjs->parseJS(vsv->urlValidation(BaseForFixURL,url, existingUrl), profileDir); //Парсинг java скриптов

qDebug() << "URLForJSParse: " << url;

    if(!pathToHtml.isEmpty())
    {qDebug() << "ERROR_AAAAAAAA";
        findLinksInSaveTOJS(pathToHtml, links); //Ищет ссылки в файлах, скачанных функцией ParseJavaScript::jsTreeParse
    }

    fo->saveFile(profileDir, QUrl(url).host() + QString::number(counter) +"PFRE.html", bufHTML);
    counter++;
qDebug() << "DIR FOR SAVE BUF AFTER JS PARSE: " << profileDir + QUrl(url).host() + QString::number(counter) +"PFRE.html";
     //Поиск ссылок по регулярным выражениям в bufHTMLtoFind
    for(QStringList::iterator itr=regExpForParsePostTitle.begin(); itr!=regExpForParsePostTitle.end();itr++)
    {qDebug() << "PATTERN: " << *itr;
        links.append(vsv->findSubStrToRegExp(*itr, bufHTML, false));
    }
qDebug() << "LINKS1: " << links;
     //Удаляет одинаковые ссылки
    links.removeDuplicates();

    if(links.indexOf(url) != -1)
    {
        links.removeAt(links.indexOf(url));
    }
qDebug() << "LINKS2: " << links;

     //Добавляет найденные ссылки в TextBrowser
    emit appendToTextBr(links);

    if((blockEmit == 0) && (parseDepth > 1))
    {
       emit callwhileParse(profile);
    }
    else if(blockEmit == 0)
    {
        emit infoAllarm("Exelent work!", "Site is save");
        emit finishedPAS();
    }
qDebug() << "END_MODUL_fingRegExp_2";
    return;
}

  //Ищет ссылки в файлах, скачанных функцией ParseJavaScript::jsTreeParse
void ParseTargetSite::findLinksInSaveTOJS(QStringList &path, QStringList &result)
{qDebug() << "MODUL_findLinksInSaveTOJS";
   for(QStringList::iterator i=path.begin(); i!=path.end();i++)
   {
       bufHTMLtoFind = fo->readFile(profileDir, *i, ".html"); //Открываем файл для поиска html

        //Поиск ссылок по регулярным выражениям в bufHTMLtoFind
       for(QStringList::iterator itr=regExpForParsePostTitle.begin(); itr!=regExpForParsePostTitle.end();itr++)
       {
           result.append(vsv->findSubStrToRegExp(*itr, bufHTMLtoFind, false));
       }

       bufHTMLtoFind.clear();
   }
qDebug() << "END_MODUL_findLinksInSaveTOJS";
}



void ParseTargetSite::whileParse(QString profile)
{qDebug() << "MODUL_whileParse";
    blockEmit = 1;

    if(ItrwhileParse == 0)
    {
        return;
    }

    while((parseDepth != 0) && (!links.isEmpty()))  //Пока парсинг вглубину на заданное значение не завершен
    {
        linksForJSParse = links; //Ссылки на страницы для парсинга JS
        ItrwhileParse = links.size();
qDebug() << "LINKS SIZE: " << ItrwhileParse;
          //ЗАКАЧАЛИ ФАЙЛЫ ПО НАЙДЕННЫМ ССЫЛКАМ, СОБРАЛИ ПУТИ К НИМ В pathToLink
        for(quint64 i=0;i<ItrwhileParse;i++) //Загрузка страниц по найденным ссылка
        {
            profile = QString::number(parseDepth) + QString::number(i);
            pathToLink.append(profile);

            targetUrl = links.at(i);
qDebug() << "TARGET WHILE URL: " << targetUrl;
            ParseTargetSite::parseSite(profile);
qDebug() << "FILE NAME FOR  ParseTargetSite: " << profile;
        }
        links.clear();

        if(parseDepth == 1)
        {
            break;
        }

         //ТЕПЕРЬ ОТКРЫВАЕМ ЗАКАЧАННЫЕ HTML(ПУТИ К НИМ В pathToLink) И ИЩЕМ В НИХ ССЫЛКИ НА ДРУГИЕ СТРАНИЦЫ
        for(quint64 i=0;i<ItrwhileParse;i++) //Пасинг страниц по найденным ссылкам
        {
            if(runJS!=true)
            {
                ParseTargetSite::fingRegExp(profile);
            }
            else
            {qDebug() << "OPEN fingRegExp_1!";
                ParseTargetSite::fingRegExp(profile, i);
            }
        }
        linksForJSParse.clear();
        pathToLink.clear();
        parseDepth--;
    }

    linksForJSParse.clear();
    pathToLink.clear();

   emit infoAllarm("Exelent work!", "Site is save");

   emit finishedPAS();
qDebug() << "END_MODUL_whileParse";
    return;
}



void ParseTargetSite::leafingThroughUrl(QString profile) // Перелистывает страницы через url
{
    QString buf;

    manager = new QNetworkAccessManager();

    QEventLoop *event = new QEventLoop;

    QFile *writePageLT = new QFile(profileDir + profile + ".html");

    minPages = RangePages.left(RangePages.indexOf("-")).toInt();
    maxPages = RangePages.right(RangePages.indexOf("-")).toInt();

    for(quint64 i=minPages; i<=maxPages; i++)
    {
       buf = targetUrl + followingPagesViaUrl;
       reply = manager->get(QNetworkRequest(QUrl(buf.replace("%" , QString::number(i)))));

       connect(reply, &QNetworkReply::finished, event, &QEventLoop::quit); //НЕ ВСЕГДА ВЫДАЕТ ОШИБКУ ПРИ НЕВАЛИДНЫХ URL

       event->exec();

       if (reply->error() != QNetworkReply::NoError)
       {
           emit infoAllarm("Error", reply->errorString());
           break;
       }
       if(!(reply->isFinished()))
       {
           emit infoAllarm("Error", "Unexpected error");
           break;
       }

       urlDownload = reply->readAll();


       if(writePageLT->open(QFile::Append))
       {
            writePageLT->write(urlDownload);
       }
       else
       {
            emit infoAllarm("Error", writePageLT->errorString());
            break;
       }

       writePageLT->close();

       disconnect(reply, &QNetworkReply::finished, event, &QEventLoop::quit);
       urlDownload.clear();
    }

    disconnect(reply, &QNetworkReply::finished, event, &QEventLoop::quit);

    event->destroyed();
    delete event;
    reply->close();
    delete reply;
    manager->clearAccessCache();
    manager->clearConnectionCache();
    delete manager;

    writePageLT->close();
    writePageLT->deleteLater();
    delete writePageLT;

    urlDownload.clear();

    buf.clear();
}
