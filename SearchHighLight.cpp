#include "SearchHighLight.h"

SearchHighLight::SearchHighLight(QObject *parent) : QSyntaxHighlighter(parent)
{
    format = new QTextCharFormat;
    format->setBackground(Qt::yellow);
}

SearchHighLight::~SearchHighLight()
{
    result.clear();

    delete format;
}

void SearchHighLight::highlightBlock(const QString& text)
{
    // Using a regular expression, we find all matches.
    QRegularExpressionMatchIterator matchIterator = pattern.globalMatch(text);

    while (matchIterator.hasNext())
    {
        // Highlight all matches
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), *format);
        result.append(match.captured());
    }
}

QStringList SearchHighLight::searchText(const QString& text)
{
    result.clear();

    // Set the text as a regular expression.
    pattern = QRegularExpression(text);

    if(!pattern.isValid())
    {
        emit infoAllarm("Error", "Invalid regular expression");
        return QStringList();
    }

    rehighlight(); // Перезапускаем подсветку

    return result;
}

