#include "parser.h"
#include "ui_parser.h"

Parser::Parser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Parser)
{
    ui->setupUi(this);
}

Parser::~Parser()
{
    //pas.clear();
    //pasTh.clear();
    //pasgui.clear();

    cws->deleteLater();
    ps->deleteLater();
    pas->deleteLater();
    pasgui->deleteLater();
    cre->deleteLater();

    delete ui;
}

void Parser::on_pushParseTarget_clicked()
{    
    ui->pushParseTarget->setEnabled(false);
    ui->pushChromiumWebSerf->setEnabled(false);

    //parsAllSiteQuantity = ui->spinBoxSitesQuantity->value();

    //pas.resize(parsAllSiteQuantity);
    //pasTh.resize(parsAllSiteQuantity);
    //pasgui.resize(parsAllSiteQuantity);

//    for(int i=0; i<parsAllSiteQuantity; i++)
//    {
       //pas = new ParseTargetSite(i);
       //pasTh[i] = new QThread;
       //pasgui[i] = new ParseTargetGui;

       pas = new ParseTargetSite;
       pasgui = new ParseTargetGui;

       pasgui->show();

       connect(pasgui, &ParseTargetGui::pushParseClicked, pas, &ParseTargetSite::selectProfile);
       //connect(pasTh[i], &QThread::started, pas[i], &ParseTargetSite::run);
       connect(pas, &ParseTargetSite::infoAllarm, pasgui, &ParseTargetGui::informationView);
       connect(pas, &ParseTargetSite::appendToTextBr, pasgui, &ParseTargetGui::viewToTextBr);
       //connect(pas[i], &ParseTargetSite::finishedPAS, pasTh[i], &QThread::terminate);
       connect(pas, &ParseTargetSite::finishedPAS, this, &Parser::ParseTargetDestruct);

       //pas[i]->moveToThread(pasTh[i]);
       //pasTh[i]->start();
//    }

    return;
}

void Parser::ParseTargetDestruct()
{
     ui->pushParseTarget->setEnabled(true);
     ui->pushChromiumWebSerf->setEnabled(true);

     delete pas;
     //delete pasTh[id];
     qDebug() << "TestMainDestruct";
     pasgui->deleteLater();
}




void Parser::on_pushAddSetting_clicked()
{
    ps = new ParseSetting();
    connect(ps, &ParseSetting::finished, this, &Parser::AddSettingDestruct);
    ui->pushAddSetting->setEnabled(false);
    ps->show();
}

void Parser::AddSettingDestruct()
{
    ui->pushAddSetting->setEnabled(true);
    ps->deleteLater();
}

void Parser::on_pBCustomRegExp_clicked()
{
    cre = new CustomRegExp;
    connect(cre, &CustomRegExp::finished, this, &Parser::pBCustomRegExpDestruct);
    ui->pBCustomRegExp->setEnabled(false);
    cre->show();
}

void Parser::pBCustomRegExpDestruct()
{
    ui->pBCustomRegExp->setEnabled(true);
    cre->deleteLater();
}

void Parser::on_pushChromiumWebSerf_clicked()
{
    ui->pushChromiumWebSerf->setEnabled(false);
    ui->pushParseTarget->setEnabled(false);
    cws = new ChromiumWebSerf;
    cws->show();
    connect(cws, &ChromiumWebSerf::finished, this, [=](){ui->pushChromiumWebSerf->setEnabled(true);
        ui->pushParseTarget->setEnabled(true); cws->deleteLater();});
}
