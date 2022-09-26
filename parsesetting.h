#ifndef PARSESETTING_H
#define PARSESETTING_H

#include <QDebug>

#include <QFile>
#include <QDir>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QXmlStreamWriter>

#include "parsejssettingsgui.h"

namespace Ui {
class ParseSetting;
}

class ParseSetting : public QDialog
{
    Q_OBJECT

public:
    explicit ParseSetting(QWidget *parent = nullptr);
    ~ParseSetting();

private:
    Ui::ParseSetting *ui;

    bool runJS; //Парсить JS или нет
    bool changeSettings = false; //Изменены доп настройки парсинга JS или нет
    ParseJSSettingsGUI *pjssgui; //Настройки парсинга JS

    short int setNextPageMode = 0; //поумолчанию не выбрана ни одна настройка

    QString PagesRange; //Диапазон перелистывания страниц
    int parseDepth; //Глубина парсинга

    bool existingUrl; //Вкл/выкл проверки URL на существование

    QString targetUrl;
    QString profileName;
    QString profileDir;

    QString followingPagesViaUrl = ""; //Для перелистывания страниц через url
    QStringList UrlSwypeRegExp; //Для перелистывания страниц через url(регулярки страниц)

    QStringList regExpForParsePostTitle; //Регулярки для парсинга ссылок
    QStringList regExpForParseJS; //Регулярки для парсинга java скриптов
    QStringList BaseForFixURL; //Базовые URLs для исправления некорректных ссылок

    QFile *saveProfile;
    QDir *dirProfile;

    QXmlStreamWriter *xmlWriter;

signals:
    void statisticsEnabled();


private slots:
    void on_pushSetSetting_clicked();

    void addRegExpForParse(QString);
    void addRegExpForParseJS(QString);
    void addUrlSwypeRegExp(QString);  //Регулярки для перелистывания страниц через url
    void addBaseUrlForFixrelativeUrl(QString); //Регулярки для исправления некорректных ссылокs

    void saveProfileToXML();
    void on_pushSelectDir_clicked();

    void on_pushParseJSSettings_clicked(); //Открытие окна настройки парсинга JS
    void pushParseJSSettingsDelete(); //Закрытие окна настройки парсинга JS
    void saveJSSettingClicked(); //Сохранение настрок парсинга JS
};

#endif // PARSESETTING_H
