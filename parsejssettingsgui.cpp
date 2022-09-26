#include "parsejssettingsgui.h"
#include "ui_parsejssettingsgui.h"

ParseJSSettingsGUI::ParseJSSettingsGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParseJSSettingsGUI)
{
    ui->setupUi(this);
}

ParseJSSettingsGUI::~ParseJSSettingsGUI()
{
    delete ui;
}



void ParseJSSettingsGUI::on_pushSaveSettings_clicked() //Сохранение измененных доп настроек парсинга JS
{
    settingJSparse = new bool[11];

    settingJSparse[0] = ui->checkBoxAllowRunningInsecureContent->isChecked();
    settingJSparse[1] = ui->checkBoxJavascriptCanOpenWindows->isChecked();
    settingJSparse[2] = ui->checkBoxAllowWindowActivationFromJavaScript->isChecked();
    settingJSparse[3] = ui->checkBoxScreenCaptureEnabled->isChecked();
    settingJSparse[4] = ui->checkBoxFocusOnNavigationEnabled->isChecked();
    settingJSparse[5] = ui->checkBoxFullScreenSupportEnabled->isChecked();
    settingJSparse[6] = ui->checkBoxHyperlinkAuditingEnabled->isChecked();
    settingJSparse[7] = ui->checkBoxPlaybackRequiresUserGesture->isChecked();
    settingJSparse[8] = ui->checkBoxLocalContentCanAccessFileUrls->isChecked();
    settingJSparse[9] = ui->checkBoxLocalContentCanAccessRemoteUrls->isChecked();
    settingJSparse[10] = ui->checkBoxPdfViewerEnabled->isChecked();

    emit sendSettingsJSParse();

    return;
}

void ParseJSSettingsGUI::on_pushClose_clicked() //Очистка памяти
{
    delete settingJSparse;
}
