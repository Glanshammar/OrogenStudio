#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class Highlighter : public QSyntaxHighlighter
{
public:
    explicit Highlighter(QTextDocument *doc)
        : QSyntaxHighlighter(doc) {}
protected:
    struct Rule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void addRule(const QRegularExpression &pattern, const QTextCharFormat &format);
    void setMultiLinePattern(const QRegularExpression &pattern, const QTextCharFormat &format);
    void highlightBlock(const QString &text) override;

private:
    QVector<Rule> m_rules;
    QTextCharFormat m_multiLineFormat;
    QRegularExpression m_multiLinePattern;
};

#endif // HIGHLIGHTER_H
