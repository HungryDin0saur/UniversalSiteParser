#include "parsetargetgui.h"
#include "ui_parsetargetgui.h"

ParseTargetGui::ParseTargetGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParseTargetGui)
{
    ui->setupUi(this);

    files = new QDir("C:/Parser setting/");

    dirContent = files->entryInfoList(QStringList() << "*.xml", QDir::Files);

    quint64 s = dirContent.size();
    for(quint64 i=0;i<s;i++)
    {
        if(dirContent.at(i).fileName().indexOf("JSONLY") == -1)
        {
           ui->comboBoxLinks->addItem(dirContent.at(i).fileName());
        }
    }
}

ParseTargetGui::~ParseTargetGui()
{
    delete files;

    delete ui;

    dirContent.clear();
}

void ParseTargetGui::on_pushToParseSite_clicked()
{
    emit pushParseClicked(ui->comboBoxLinks->currentText());

    return;
}

void ParseTargetGui::viewToTextBr(QStringList info)
{
    quint64 s = info.size();
    for(quint64 i=0;i<s;i++)
    {
        ui->textBrowserLinks->append(info.at(i));
    }
}

void ParseTargetGui::informationView(QString hendlerMes, QString bodyMes)
{
    QMessageBox::information(this, hendlerMes, bodyMes);
    return;
}


void ParseTargetGui::on_pushDelCurProfile_clicked()
{
    QFile::remove("C:/Parser setting/" + ui->comboBoxLinks->currentText());
    ui->comboBoxLinks->removeItem(ui->comboBoxLinks->currentIndex());
    QMessageBox::information(this, "Exelent!", "Профайл успешно удален");
}
