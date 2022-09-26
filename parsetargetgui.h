#ifndef PARSETARGETGUI_H
#define PARSETARGETGUI_H

#include <QWidget>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QXmlStreamWriter>

namespace Ui {
class ParseTargetGui;
}

class ParseTargetGui : public QWidget
{
    Q_OBJECT

public:
    explicit ParseTargetGui(QWidget *parent = nullptr);
    ~ParseTargetGui();

private:
    Ui::ParseTargetGui *ui;

    QStringList profiles;

    QXmlStreamWriter *xmlWriter;
    const QDir *files;
    QFileInfoList dirContent;

private slots:
    void on_pushToParseSite_clicked(); //Начать парсинг

    void on_pushDelCurProfile_clicked(); //Удалить выбранный профиль

public slots:
    void informationView(QString hendlerMes, QString bodyMes);
    void viewToTextBr(QStringList info);

signals:
    void pushParseClicked(QString profile);

};

#endif // PARSETARGETGUI_H
