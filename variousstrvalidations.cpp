#include "variousstrvalidations.h"


#include "fileoperations.h" //WARNING!

VariousStrValidations::VariousStrValidations() : QTextDocument()
{
    //Данный класс производит валидацию строк, в соответствии с заданными правилами и их исправление при
    // выявлении несоответствия шаблону

    qurl = new QUrl;
}

VariousStrValidations::~VariousStrValidations()
{
    delete qurl;
}

bool VariousStrValidations::isExistingUrl(QString url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));

    QEventLoop *event = new QEventLoop;
    connect(reply, &QNetworkReply::finished, event, &QEventLoop::quit); //НЕ ВСЕГДА ВЫДАЕТ ОШИБКУ ПРИ НЕВАЛИДНЫХ URL

    event->exec();

    if (reply->error() != QNetworkReply::NoError)
    {qDebug() << "NO EXISTING: " << url;
        event->destroyed();
        delete event;
        reply->close();
        delete reply;
        manager->clearAccessCache();
        manager->clearConnectionCache();
        delete manager;

        return 0;
    }
    if(!(reply->isFinished()))
    {
        event->destroyed();
        delete event;
        reply->close();
        delete reply;
        manager->clearAccessCache();
        manager->clearConnectionCache();
        delete manager;

        return 0;
    }

    event->destroyed();
    delete event;
    reply->close();
    delete reply;
    manager->clearAccessCache();
    manager->clearConnectionCache();
    delete manager;
qDebug() << "IS EXISTING: " << url;
    return 1;
}

QString VariousStrValidations::urlValidation(QStringList &baseUrl, QString &relativeUrl, bool getValigation) //Соединяет относительный URL с базовым                                                                      //второй, если второй не полноценный url
{
   qurl->setUrl(relativeUrl, QUrl::StrictMode);
   if(!qurl->isRelative()) //Если URL не относительный, то его не требуется исправлять
   {
       return VariousStrValidations::deleteEntityInThisUrl(qurl->url());
   }

   for(QStringList::iterator itr=baseUrl.begin(); itr!=baseUrl.end();itr++)
   {
       qurl->setUrl(*itr, QUrl::StrictMode);
       if(!qurl->isLocalFile()) //Если URl относительный, соединяет его с базовым
       {
           //this->setHtml(qurl->resolved(relativeUrl).toString()); //Не всегда корректно работает
           this->setHtml(qurl->url() + relativeUrl);

           if(getValigation) //Вызывает проверку существования URL
           {
                if(VariousStrValidations::isExistingUrl(this->toPlainText()))
                {
                    return this->toPlainText();
                }
                else
                {
                    continue;
                }
           }
           else
           {
                return this->toPlainText();
           }

            /*
             Возвращает результат слияния базового URL с относительным.
             Если относительный URL-адрес не является относительным,
             эта конструкция будет возвращать относительный напрямую.

             Пользователь может задать несколько базовых URL, данная конструкция будет
             склеивать URLы, пока не найдет валадный, путем проверки запроса на получившийся URl.

             Перед возвратом результата, данная конструкция проверит существование данного url.
            */
       }
       else
       {
           emit infoAllarm("Error", "Заданный url: " + qurl->url() + " является локальным файлом!");

           return "0";
       }
   }
}

QString VariousStrValidations::substr(QString str, QString x, QString y)
{
    QString::iterator buf = nullptr, itr, total;
    QString res;

    for(itr=str.begin(), total = str.end(); itr!=total;++itr)
    {
       if(*itr == x)
       {
           ++itr;
           buf = itr;
           break;
       }
    }

    for(itr=buf, total=str.end(); itr!=total;++itr)
    {
        if(*itr == y)
        {
            break;
        }
        res += *itr;
    }

    return res;
}

QString VariousStrValidations::deleteEntityInThisUrl(const QString url)
{
    this->setHtml(url); //Удаляет из url сущности,
    return this->toPlainText(); //делая его валидным
}

QStringList VariousStrValidations::findSubStrToRegExp(const QString &pattern, const QString &document, bool delHtmlEntity)
{
    result.clear();

    QRegularExpressionMatchIterator matchIterator = QRegularExpression(pattern).globalMatch(document);

    if(!QRegularExpression(pattern).isValid())
    {
        emit infoAllarm("Error", "Invalid regular expression");
        return QStringList();
    }

    while (matchIterator.hasNext())
    {
        // Highlight all matches
        QRegularExpressionMatch match = matchIterator.next();
        result.append(deleteEntityInThisUrl(match.captured()));
    }

    return result;
}
