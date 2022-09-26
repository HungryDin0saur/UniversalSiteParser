#include "parsesetting.h"
#include "ui_parsesetting.h"

ParseSetting::ParseSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParseSetting)
{
    ui->setupUi(this);
}

ParseSetting::~ParseSetting()
{
    delete ui;
}

void ParseSetting::on_pushSetSetting_clicked()
{
    //JS settings

    runJS = ui->checkBoxTuneParseJS->isChecked();
    if((runJS == true) && (regExpForParseJS.isEmpty()))
    {
        QMessageBox::information(this, "Error", "Заполните регулярные выражения для java скриптов");
        return;
    }

    PagesRange = ui->minNextPagesParse->text() + "-" + ui->maxNextPagesParse->text();

    if(!ui->checkBoxParseDepth->isChecked())
    {
        parseDepth = 2147483647;
    }
    else
    {
        parseDepth = ui->spinBoxParseDepth->value();
    }

    existingUrl = ui->checkBoxIsExistingUrl->isChecked(); //Вкл/выкл проверки URL на существование

    profileName = ui->lineEditFileName->text();

    targetUrl = ui->urlEdit->text();
    profileDir = (ui->lineDirToDBEdit->text());

    if(!ui->checkBoxUrlSwype->isChecked())
    {
       followingPagesViaUrl = "=====";
    }
    else
    {
       followingPagesViaUrl = ui->comboEditUrlSwype->currentText();
    }

    if(targetUrl == "")
    {
       QMessageBox::information(this, "Error", "Целевой URl не задан!");
       return;
    }
    if(profileName == "")
    {
       QMessageBox::information(this, "Error", "Имя профиля не задано!");
       return;
    }
    if(profileDir == "")
    {
       QMessageBox::information(this, "Error", "Дирректория для сохранения не задана!");
       return;
    }

    ParseSetting::saveProfileToXML();

    return;
}



void ParseSetting::addRegExpForParse(QString regExp) //Вызывается по сигналу с формы activated()
{
    regExpForParsePostTitle.append(regExp);
}

void ParseSetting::addRegExpForParseJS(QString regExp) //Вызывается по сигналу с формы activated()
{
    regExpForParseJS.append(regExp);
}

void ParseSetting::addUrlSwypeRegExp(QString regExp)
{
    UrlSwypeRegExp.append(regExp);
}

void ParseSetting::addBaseUrlForFixrelativeUrl(QString regExp) //Базовые URL исправления некорректных ссылок
{
   BaseForFixURL.append(regExp);
}


void ParseSetting::saveProfileToXML()
{
   dirProfile = new QDir;
   if(!dirProfile->exists("C://Parser setting"))
   {
       dirProfile->mkdir("C://Parser setting");
   }

   saveProfile = new QFile("C://Parser setting/" + profileName + ".xml");
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

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("targeturl", targetUrl);
       xmlWriter->writeCharacters("tu");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("dir", profileDir);
       xmlWriter->writeCharacters("d");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("nextpagemode", QString::number(setNextPageMode));
       xmlWriter->writeCharacters("npm");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("maxpage", PagesRange);
       xmlWriter->writeCharacters("mp");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("parsedepth", QString::number(parseDepth));
       xmlWriter->writeCharacters("mp");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("existingUrl", QString::number(existingUrl));
       xmlWriter->writeCharacters("eu");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro");
       xmlWriter->writeAttribute("followingPagesViaUrl", followingPagesViaUrl);
       xmlWriter->writeCharacters("fpvu");
       xmlWriter->writeEndElement();

       xmlWriter->writeStartElement("pro"); //Парсить JS: true/false
       xmlWriter->writeAttribute("JSparse", QString::number(runJS));
       xmlWriter->writeCharacters("runJS");
       xmlWriter->writeEndElement();

       //Регулярки для парсинга ссылок и перехода по ним
      quint64 s = regExpForParsePostTitle.size();
      for(int i=0;i<s;i++)
      {
          xmlWriter->writeStartElement("regexp");
          xmlWriter->writeAttribute("RegExp", regExpForParsePostTitle.at(i));
          xmlWriter->writeCharacters("re");
          xmlWriter->writeEndElement();
      }

       //Регулярки для перелистывания страниц через url
      s = UrlSwypeRegExp.size();
      for(int i=0;i<s;i++)
      {
          xmlWriter->writeStartElement("fpvure");
          xmlWriter->writeAttribute("followingPagesViaUrlRegExp", UrlSwypeRegExp.at(i));
          xmlWriter->writeCharacters("fpvure");
          xmlWriter->writeEndElement();
      }

       //Базовые URLs для исправления некорректных ссылок
      s = BaseForFixURL.size();
      if(s == 0) //Если базовые URLs не заданы, то создать пустю запись
      {
          xmlWriter->writeStartElement("bffu");
          xmlWriter->writeAttribute("BaseForFixURL", "0");
          xmlWriter->writeCharacters("bffu");
          xmlWriter->writeEndElement();
      }
      else
      {
          for(int i=0;i<s;i++)
          {
              xmlWriter->writeStartElement("bffu");
              xmlWriter->writeAttribute("BaseForFixURL", BaseForFixURL.at(i));
              xmlWriter->writeCharacters("bffu");
              xmlWriter->writeEndElement();
          }
      }

       //JS settings

        //Регулярки парсинга java скриптов
       s = regExpForParseJS.size();
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
              xmlWriter->writeAttribute("settingJS", QString::number(pjssgui->settingJSparse[i]));
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

   regExpForParsePostTitle.clear();

   saveProfile->close();
   saveProfile->deleteLater();

   delete dirProfile;
   delete saveProfile;
   delete xmlWriter;

   return;
}




void ParseSetting::on_pushSelectDir_clicked()
{
    profileDir = QFileDialog::getExistingDirectory(this, "Choose a directory to save the site" , "");
    if(profileDir.isEmpty())
    {
        QMessageBox::information(this, "Error" , "Directory selection error");
        return;
    }
    ui->lineDirToDBEdit->setText(profileDir + "/");
}




void ParseSetting::on_pushParseJSSettings_clicked() //Доп настройки парсинга JS
{
    pjssgui = new ParseJSSettingsGUI;
    connect(pjssgui, &ParseJSSettingsGUI::close, this, &ParseSetting::pushParseJSSettingsDelete);
    connect(pjssgui, &ParseJSSettingsGUI::sendSettingsJSParse, this, &ParseSetting::saveJSSettingClicked);
    ui->pushParseJSSettings->setEnabled(false);
    pjssgui->show();
}

void ParseSetting::pushParseJSSettingsDelete() //ЗАкрытие окна доп настроек парсинга JS
{
    disconnect(pjssgui, &ParseJSSettingsGUI::close, this, &ParseSetting::pushParseJSSettingsDelete);
    disconnect(pjssgui, &ParseJSSettingsGUI::sendSettingsJSParse, this, &ParseSetting::saveJSSettingClicked);
    pjssgui->deleteLater();
    ui->pushParseJSSettings->setEnabled(true);
}

void ParseSetting::saveJSSettingClicked()//Если изменены стандартные настройки парсинга JS
{
    changeSettings = true;
}
