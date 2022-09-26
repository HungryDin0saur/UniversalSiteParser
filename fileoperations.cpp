#include "fileoperations.h"

FileOperations::FileOperations()
{
    qts = new QTextStream;
}

FileOperations::~FileOperations()
{
    delete qts;
}

void FileOperations::saveFile(const QString &filePath, const QString &fileName, const QByteArray &fileBody)
{
    this->setFileName(filePath + fileName);

    if(this->open(QFile::WriteOnly))
    {
         this->write(fileBody);
    }
    else
    {
         emit infoAllarm("Error", this->errorString());
         this->close();
         return;
    }

    this->close();

    return;
}

void FileOperations::saveFile(const QString &filePath, const QString &fileName, const QString &fileBody)
{
    this->setFileName(filePath + fileName);

    if(this->open(QFile::WriteOnly))
    {
         qts->setDevice(this);
         qts->setCodec("UTF-8");
         *qts << fileBody;
    }
    else
    {
         emit infoAllarm("Error", this->errorString());
         this->close();
         return;
    }

    this->close();

    return;
}

QString FileOperations::readFile(const QString &dirrectory, const QString & file_name, const QString & extension)
{
    this->setFileName(dirrectory + file_name + extension);
    if(this->open(QFile::ReadOnly))
    {
        bufString = this->readAll();
        this->close();
        return bufString;
    }
    else
    {
         emit infoAllarm("Error", this->errorString());
         return "Error";
    }
}

QByteArray FileOperations::readFile(const QString &dirrectory, const QString & file_name, const QString & extension, bool)
{
    this->setFileName(dirrectory + file_name + extension);
    if(this->open(QFile::ReadOnly))
    {
        bufByte = this->readAll();
        this->close();
        return bufByte;
    }
    else
    {
         emit infoAllarm("Error", this->errorString());
         return "Error";
    }
}
