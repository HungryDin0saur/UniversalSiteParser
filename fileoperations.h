#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QTextStream>
#include <QDataStream>
#include <QByteArray>
#include <QObject>
#include <QFile>

#include <QDebug>

class FileOperations : public QFile
{
    Q_OBJECT
public:
    FileOperations();
    ~FileOperations();

private:
    QTextStream *qts;
    QString bufString;
    QByteArray bufByte;

signals:
    void infoAllarm(const QString &, const QString &);

public slots:
    void saveFile(const QString &filePath, const QString &fileName, const QByteArray &fileBody); //Сохраняет файл QByteArray
    void saveFile(const QString &filePath, const QString &fileName, const QString &fileBody); //Сохраняет файл QString

    QString readFile(const QString &dirrectory, const QString &file_name, const QString &extension); //Считывае файл в QString
    QByteArray readFile(const QString &dirrectory, const QString & file_name, const QString & extension, bool); //Считывае файл в QByteArray
};

#endif // FILEOPERATIONS_H
