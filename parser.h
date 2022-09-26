#ifndef PARSER_H
#define PARSER_H

#include <QMainWindow>
//#include <QThread>

#include "parsesetting.h"
#include "parsetargetgui.h"
#include "parsetargetsite.h"
#include "customregexp.h"
#include "chromiumwebserf.h"

namespace Ui {
class Parser;
}

class Parser : public QMainWindow
{
    Q_OBJECT

public:
    explicit Parser(QWidget *parent = nullptr);
    ~Parser();

private: //Parse to post
    int postsQuantity = 1; //Сколько сайтов парсить одновременно по постам

    int parsAllSiteQuantity = 1;//Сколько сайтов парсить одновременно полностью

    ParseTargetSite *pas;
    ParseTargetGui *pasgui;

    //QVector<ParseTargetGui*> pasgui;
    //QVector<ParseTargetSite*> pas;
    //QVector<QThread*> pasTh;

    ParseSetting *ps; //Добавление настроек нового профиля
    CustomRegExp *cre; //Настройка регулярных выражений для парсинга
    ChromiumWebSerf *cws; //Парсинг сайтов, работающих только на java скриптах


private slots:
    void on_pushParseTarget_clicked();
    void ParseTargetDestruct();

    void on_pushAddSetting_clicked();
    void AddSettingDestruct();

    void on_pBCustomRegExp_clicked(); //Настройка регулярных выражений
    void pBCustomRegExpDestruct(); //Очистка памяти

    void on_pushChromiumWebSerf_clicked();

signals:
    void getHtml(QString html);

private:
    Ui::Parser *ui;
};

#endif // PARSER_H
