#ifndef CHROMIUMWEBSERF_H
#define CHROMIUMWEBSERF_H

#include <tuple>
#include <QDir>
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfoList>

#include "parsejavascript.h"
#include "parsejssettingsgui.h"

namespace Ui {
class ChromiumWebSerf;
}

class ChromiumWebSerf : public QDialog
{
    Q_OBJECT

public:
    explicit ChromiumWebSerf(QWidget *parent = nullptr);
    ~ChromiumWebSerf();

private: slots:

     //Выбор дирректории для сохранения настроек
    void on_pushSelectDir_clicked();

     //По сигналу QComboBox::activated добаляет регулярные выражения для поиска java скриптов в массив regExpForParseJS
    void addRegExpForParseJS(QString regExp);

     //Начать парсинг согласно выбранному профилю настроек
    void on_pushStart_clicked();

     //Дополнительные настройки работы с java скриптами
    void on_pushPrSettings_clicked();

     //Сохранить настройки парсинга в xml файл
    void on_pushSaveSetting_clicked();

     //Удалить xml файл с настройками парсинга
    void on_pushDelCurProfile_clicked();

     //Вывод пользователю сообщений от дргуих модулей, по их сигналу, например ParseJavaScript::infoAllarm
    void infoAllarm(QString hendlerMes, QString bodyMes);

     //Считывает путь для сохранения страниц и целевой URL из выбранного xml файла
    std::tuple<QString, QString> getProfileDirAndURL(const QString &dir, const QString &profileName);

private:
    Ui::ChromiumWebSerf *ui;

    const QDir *files;
    QFileInfoList dirContent;

    ParseJavaScript *pjs;
    ParseJSSettingsGUI *pjsqui;

    QString targetUrl;
    QString profileName;
    QString profileDir; //Дирректория для сохранения собранной информации

    bool changeSettings = false; //Изменены доп настройки парсинга JS или нет
    uint depthFindandRunJS; //глубина поиска и запуска JS на одной странице

    QFile *saveProfile;
    QDir *dirProfile;
    QXmlStreamWriter *xmlWriter; //Запись в файл xml
    QXmlStreamReader *xsr; //Считывание из файла xml

    QStringList regExpForParseJS; //Регулярные выражения для поиска java скриптов на странице



};

#endif // CHROMIUMWEBSERF_H
