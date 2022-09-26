#ifndef CUSTOMREGEXP_H
#define CUSTOMREGEXP_H

#include "SearchHighLight.h"

#include <QMessageBox>

namespace Ui {
class CustomRegExp;
}

class CustomRegExp : public QDialog
{
    Q_OBJECT

public:
    explicit CustomRegExp(QWidget *parent = nullptr);
    ~CustomRegExp();

private slots:
    void on_pBFindRegExp_clicked();
     //Выводит сообщение об ошибке по сигналу из SearchHighLight
    void checkValidRegExp(const QString &title, const QString &body);

private:
    Ui::CustomRegExp *ui;

    SearchHighLight *shl;

    QStringList result; //Сюда сохраняются найденные подстроки

};

#endif // CUSTOMREGEXP_H
