#include "chromiumwebserf.h"
#include "ui_chromiumwebserf.h"

ChromiumWebSerf::ChromiumWebSerf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChromiumWebSerf)
{
    ui->setupUi(this);

    files = new QDir("C:/Parser setting/");

    dirContent = files->entryInfoList(QStringList() << "*.xml", QDir::Files);

    quint64 s = dirContent.size();
    for(quint64 i=0;i<s;i++)
    {
        if(dirContent.at(i).fileName().indexOf("JSONLY") != -1)
        {
           ui->comboBoxLinks->addItem(dirContent.at(i).fileName());
        }
    }

    pjsqui = new ParseJSSettingsGUI;
}

ChromiumWebSerf::~ChromiumWebSerf()
{
    delete pjsqui;

    delete ui;
}

void ChromiumWebSerf::addRegExpForParseJS(QString regExp)
{
    regExpForParseJS.append(regExp);

    return;
}

void ChromiumWebSerf::on_pushSelectDir_clicked()
{
    profileDir = QFileDialog::getExistingDirectory(this, "Choose a directory to save the site" , "");
    if(profileDir.isEmpty())
    {
        QMessageBox::information(this, "Error" , "Directory selection error");
        return;
    }
    ui->lineDirToDBEdit->setText(profileDir + "/");

    return;
}

void ChromiumWebSerf::on_pushPrSettings_clicked()
{
    pjsqui->show();
    ui->pushPrSettings->setEnabled(false);
    connect(pjsqui, &ParseJSSettingsGUI::finished, this, [&](){ui->pushPrSettings->setEnabled(true);});

    return;
}

void ChromiumWebSerf::on_pushSaveSetting_clicked()
{
    targetUrl = ui->urlEdit->text();
    profileName = ui->lineEditFileName->text();

    dirProfile = new QDir;
    if(!dirProfile->exists("C://Parser setting"))
    {
        dirProfile->mkdir("C://Parser setting");
    }

    saveProfile = new QFile("C://Parser setting/" + profileName + "JSONLY" + ".xml");
    if(saveProfile->exists())
    {
        QMessageBox::information(this, "Error", "Данный профайл уже существует");
        delete saveProfile;
        delete dirProfile;
        return;
    }

    if(saveProfile->open(QFile::WriteOnly))
    {
        xmlWriter = new QXmlStreamWriter(saveProfile);
        xmlWriter->setAutoFormatting(true);  // Устанавливаем автоформатирование текста
        xmlWriter->writeStartDocument();     // Запускаем запись в документ
        xmlWriter->writeStartElement("profile");

        xmlWriter->writeStartElement("targetUrl");
        xmlWriter->writeAttribute("targeturl", targetUrl);
        xmlWriter->writeCharacters("tu");
        xmlWriter->writeEndElement();

        xmlWriter->writeStartElement("profileDir");
        xmlWriter->writeAttribute("dir", profileDir + "/");
        xmlWriter->writeCharacters("d");
        xmlWriter->writeEndElement();

         //Регулярки парсинга java скриптов
        quint64 s = regExpForParseJS.size();
        for(int i=0;i<s;i++)
        {
            xmlWriter->writeStartElement("regexpJS");
            xmlWriter->writeAttribute("RegExpJS", regExpForParseJS.at(i));
            xmlWriter->writeCharacters("reJS");
            xmlWriter->writeEndElement();
        }

         //Доп настройки парсинга JS
        if(changeSettings == true)
        {
           for(quint8 i=0;i<11;i++)
           {
               xmlWriter->writeStartElement("settingsJS");
               xmlWriter->writeAttribute("settingJS", QString::number(pjsqui->settingJSparse[i]));
               xmlWriter->writeCharacters("sJS");
               xmlWriter->writeEndElement();
           }
        }

        xmlWriter->writeEndElement();
        xmlWriter->writeEndDocument();

        QMessageBox::information(this, "Exelent!", "Файл настроек добавлен");
    }
    else
    {
        QMessageBox::information(this, "Error open file", saveProfile->errorString());
    }

    saveProfile->close();
    saveProfile->deleteLater();

    delete dirProfile;
    delete saveProfile;
    delete xmlWriter;

     //Перезапускаем поиск xml файлов для добавления в меню профилей(QComboBox)
    dirContent = files->entryInfoList(QStringList() << "*.xml", QDir::Files);

    quint64 s = dirContent.size();
    for(quint64 i=0;i<s;i++)
    {
        if(dirContent.at(i).fileName().indexOf("JSONLY") != -1)
        {
           ui->comboBoxLinks->addItem(dirContent.at(i).fileName());
        }
    }

    return;
}

void ChromiumWebSerf::on_pushDelCurProfile_clicked()
{
    QFile::remove("C:/Parser setting/" + ui->comboBoxLinks->currentText());
    ui->comboBoxLinks->removeItem(ui->comboBoxLinks->currentIndex());
    QMessageBox::information(this, "Exelent!", "Профайл успешно удален");

    return;
}

void ChromiumWebSerf::infoAllarm(QString hendlerMes, QString bodyMes)
{
    QMessageBox::information(this, hendlerMes, bodyMes);
}

std::tuple<QString, QString> ChromiumWebSerf::getProfileDirAndURL(const QString &dir, const QString &profileName)
{
    QString targetUrl, profileDir;

    QFile *readFile = new QFile("C://Parser setting/" + profileName);
    if (!readFile->open(QFile::ReadOnly | QIODevice::Text))
    {
        emit infoAllarm("Error", readFile->errorString());
        return {"", ""};
    }

    xsr = new QXmlStreamReader(readFile);
    QXmlStreamAttributes attrib;

    while (!xsr->atEnd())
    {
       xsr->readNext();
       if (xsr->isStartElement())
       {
          if(xsr->name() == "targetUrl")
          {
              attrib = xsr->attributes();
              targetUrl = attrib.at(0).value().toString();
          }

          if(xsr->name() == "profileDir")
          {
              attrib = xsr->attributes();
              profileDir = attrib.at(0).value().toString();
          }

        if(xsr->hasError())
        {
            emit infoAllarm("Error parse XML", xsr->errorString());
            readFile->close();
            delete readFile;
            delete xsr;
            return {"", ""};
        }
       }
     }

    attrib.clear();
    readFile->close();
    delete readFile;
    delete xsr;

    return {targetUrl, profileDir};
}




void ChromiumWebSerf::on_pushStart_clicked()
{
   profileName = ui->comboBoxLinks->currentText();

   std::tie(targetUrl, profileDir) = getProfileDirAndURL("C://Parser setting/", profileName);

   pjs = new ParseJavaScript(profileName);

   connect(pjs, &ParseJavaScript::infoAllarm, this, &ChromiumWebSerf::infoAllarm, Qt::QueuedConnection);

   QString bufHTML; QStringList pathToHtml;

   std::tie(bufHTML, pathToHtml) = pjs->parseJS(targetUrl, profileDir); //Парсинг java скриптов

   if(pathToHtml.isEmpty())
   {
      infoAllarm("Error", "Ни чего не найдено");
   }
   else
   {
       infoAllarm("Error", "Parsing complate");
   }

   delete pjs;
}


