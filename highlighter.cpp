#include "highlighter.h"

void Highlighter::addRule(const QRegularExpression &pattern, const QTextCharFormat &format)
{
    m_rules.append({pattern, format});
}

void Highlighter::setMultiLinePattern(const QRegularExpression &pattern, const QTextCharFormat &format)
{
    m_multiLinePattern = pattern;
    m_multiLineFormat = format;
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const auto &rule : std::as_const(m_rules)) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Multi-line comment logic (same as before)
    setCurrentBlockState(0);
    if (previousBlockState() != 1 && !m_multiLinePattern.pattern().isEmpty()) {
        auto it = m_multiLinePattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            int start = match.capturedStart();
            int end = match.capturedEnd() - 1;
            if (end < text.length()) {
                setFormat(start, end - start + 1, m_multiLineFormat);
            } else {
                setFormat(start, text.length() - start, m_multiLineFormat);
                setCurrentBlockState(1);
                break;
            }
        }
    } else if (previousBlockState() == 1) {
        auto it = m_multiLinePattern.globalMatch(text);
        if (it.hasNext()) {
            auto match = it.next();
            int end = match.capturedEnd() - 1;
            setFormat(0, end + 1, m_multiLineFormat);
            setCurrentBlockState(0);
        } else {
            setFormat(0, text.length(), m_multiLineFormat);
            setCurrentBlockState(1);
        }
    }
}