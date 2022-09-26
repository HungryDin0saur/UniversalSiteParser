#ifndef SEARCHHIGHLIGHT_H
#define SEARCHHIGHLIGHT_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class SearchHighLight : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SearchHighLight(QObject* parent = nullptr);
    ~SearchHighLight();

signals:
    void infoAllarm(const QString &, const QString &);

public slots:
     //Поиск по регулярным выражениям с подсветкой теста
    QStringList searchText(const QString& text);

private:
    virtual void highlightBlock(const QString &text) override;

private:
    QTextCharFormat *format; // Text formatting, highlighting
    QRegularExpression pattern;
    QStringList result; //Сюда сохраняются найденные подстроки
};

#endif // SEARCHHIGHLIGHT_H
