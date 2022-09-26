#include "customregexp.h"
#include "ui_customregexp.h"

CustomRegExp::CustomRegExp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomRegExp)
{
    ui->setupUi(this);

    shl = new SearchHighLight(this);

    connect(shl, &SearchHighLight::infoAllarm, this, &CustomRegExp::checkValidRegExp);
}

CustomRegExp::~CustomRegExp()
{
    result.clear();

    delete ui;
}

void CustomRegExp::on_pBFindRegExp_clicked()
{    
    shl->setDocument(ui->tBCurrentHtml->document());
    result = shl->searchText(ui->lESetRegExp->text());

    for(QStringList::iterator itr=result.begin(); itr!=result.end();itr++)
    {
        ui->tBFoundSubStr->append(*itr);
        ui->tBFoundSubStr->append("\n");
    }

    result.clear();
}

void CustomRegExp::checkValidRegExp(const QString &title, const QString &body)
{
    QMessageBox::information(this, title, body);
}
