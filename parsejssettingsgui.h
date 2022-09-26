#ifndef PARSEJSSETTINGS_H
#define PARSEJSSETTINGS_H

#include <QDebug>

#include <QDialog>

namespace Ui {
class ParseJSSettingsGUI;
}

class ParseJSSettingsGUI : public QDialog
{
    Q_OBJECT

public:
    explicit ParseJSSettingsGUI(QWidget *parent = nullptr);
    ~ParseJSSettingsGUI();

    bool *settingJSparse;

signals:
    void sendSettingsJSParse(); //Сигнал обработки заданных настроек в классе parsesetting

private slots:
    void on_pushSaveSettings_clicked();

    void on_pushClose_clicked();

private:
    Ui::ParseJSSettingsGUI *ui;
};

#endif // PARSEJSSETTINGS_H
