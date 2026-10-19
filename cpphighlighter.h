#include "highlighter.h"

class CppHighlighter : public Highlighter
{
public:
    explicit CppHighlighter(QTextDocument *doc)
        : Highlighter(doc)
    {
        // keywords
        QTextCharFormat kw;
        kw.setForeground(QColor(0, 0, 128));
        kw.setFontWeight(QFont::Bold);
        for (const auto &s : {"int", "void", "class", "struct", "return",
                              "if", "else", "for", "while", "switch", "case",
                              "break", "continue", "do", "const", "static",
                              "template", "typename", "namespace", "using",
                              "public", "private", "protected", "new", "delete",
                              "true", "false", "nullptr", "auto", "bool", "char",
                              "double", "float", "long", "short", "unsigned",
                              "signed", "virtual", "inline", "explicit", "friend"})
            addRule(QRegularExpression(QString("\\b%1\\b").arg(s)), kw);

        // strings
        QTextCharFormat str;
        str.setForeground(QColor(163, 21, 21));
        addRule(QRegularExpression("\"(\\\\.|[^\"\\\\])*\""), str);

        // multi-line comments
        QTextCharFormat cm;
        cm.setForeground(QColor(0, 128, 0));
        cm.setFontItalic(true);
        setMultiLinePattern(QRegularExpression("/\\*.*?\\*/"), cm);
    }
};