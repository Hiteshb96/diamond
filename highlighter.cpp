/**************************************************************************
*
* Copyright (c) 2012 Barbara Geller
* All rights reserved.
*
* This file is part of Diamond Editor.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 3
* as published by the Free Software Foundation.
*
* Diamond Editor is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Diamond Editor.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "highlighter.h"
#include "util.h"

#include <QtGui>
#include <QFile>
#include <QString>

Highlighter::Highlighter(QTextDocument *parent)
   : QSyntaxHighlighter(parent)
{     
   QString fileName = "syn_cpp.txt";
   QFile file(fileName);

   if (! file.open(QFile::ReadOnly | QFile::Text)) {
      csMsg("Syntax C/C++ is missing");
   }

   file.seek(0);
   QByteArray temp = file.readAll();

   QString fileData = QString::fromUtf8(temp);
   file.close();  

   QStringList keywordPatterns = fileData.split("\n");

   //
   m_struct.syn_KeyWeight    = QFont::Bold;
   m_struct.syn_KeyText      = QColor(Qt::blue);
   m_struct.syn_ClassWeight  = QFont::Normal;
   m_struct.syn_ClassText    = QColor(Qt::darkMagenta);
   m_struct.syn_FuncWeight   = QFont::Normal;
   m_struct.syn_FuncText     = QColor(Qt::blue);
   m_struct.syn_LineText     = QColor(Qt::darkGreen);
   m_struct.syn_QuoteText    = QColor(Qt::darkGreen);
   m_struct.syn_MLineText    = QColor(Qt::darkGreen);

   //
   HighlightingRule rule;

   foreach (const QString &pattern, keywordPatterns) {

      if (pattern.trimmed().isEmpty()) {
         continue;
      }

      // keywords
      rule.format.setFontWeight(m_struct.syn_KeyWeight);
      rule.format.setForeground(m_struct.syn_KeyText);
      rule.pattern = QRegExp(pattern);
      highlightingRules.append(rule);
   }

   // class
   rule.format.setFontWeight(m_struct.syn_ClassWeight);
   rule.format.setForeground(m_struct.syn_ClassText);
   rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
   highlightingRules.append(rule);

   // functions    
   rule.format.setFontWeight(m_struct.syn_FuncWeight);
   rule.format.setForeground( m_struct.syn_FuncText);
   rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
   highlightingRules.append(rule);

   // single line
   rule.format.setForeground(m_struct.syn_LineText);
   rule.pattern = QRegExp("//[^\n]*");
   highlightingRules.append(rule);

   // quoted text
   rule.format.setForeground(m_struct.syn_QuoteText);
   rule.pattern = QRegExp("\".*\"");
   highlightingRules.append(rule);

   multiLineCommentFormat.setForeground(m_struct.syn_MLineText);

   commentStartExpression = QRegExp("/\\*");
   commentEndExpression   = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
   foreach (const HighlightingRule &rule, highlightingRules) {
      QRegExp expression(rule.pattern);
      int index = expression.indexIn(text);

      while (index >= 0) {
         int length = expression.matchedLength();
         setFormat(index, length, rule.format);
         index = expression.indexIn(text, index + length);
      }
   }

   setCurrentBlockState(0);

   int startIndex = 0;
   if (previousBlockState() != 1) {
      startIndex = commentStartExpression.indexIn(text);
   }

   while (startIndex >= 0) {
      int endIndex = commentEndExpression.indexIn(text, startIndex);
      int commentLength;

      if (endIndex == -1) {
         setCurrentBlockState(1);
         commentLength = text.length() - startIndex;

      } else {
         commentLength = endIndex - startIndex + commentEndExpression.matchedLength();

      }

      setFormat(startIndex, commentLength, multiLineCommentFormat);
      startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
   }
}