/**************************************************************************
*
* Copyright (c) 2012-2014 Barbara Geller
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

#include "dialog_buffer.h"
#include "dialog_getline.h"
#include "dialog_xp_getdir.h"
#include "mainwindow.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QSysInfo>
#include <QUrl>

void MainWindow::argLoad(QList<QString> argList)
{
   int argCnt = argList.count();

   for (int k = 1; k < argCnt; k++) {
      QString tempFile = argList.at(k);

      // change to forward slash
      tempFile = QDir::fromNativeSeparators(tempFile);

      // expand for full path
      QFileInfo xx(tempFile);
      tempFile = xx.canonicalFilePath();

      if (tempFile.isEmpty()) {
         // do nothing

      } else if (m_openedFiles.contains(tempFile, Qt::CaseInsensitive) ) {
         // file is already open

      } else if ( QFile::exists(tempFile) ) {
         loadFile(tempFile, true, true);
      }
   }
}

void MainWindow::autoLoad()
{
   QString fileName;
   int count = m_openedFiles.size();

   if (count == 0) {
      tabNew();

   } else {
      for (int k = 0; k < count; k++)  {
         fileName = m_openedFiles.at(k);

         // load existing files
         loadFile(fileName, true, true);
      }
   }
}

void MainWindow::closeEvent(QCloseEvent *event)
{   
   bool exit = closeAll_Doc();

   if (exit) {
      json_Write(CLOSE);
      event->accept();

   } else {
      event->ignore();

   }
}

void MainWindow::changeFont()
{
   if (m_textEdit->get_ColumnMode()) {
      m_textEdit->setFont(m_struct.fontColumn);
   } else {
      m_textEdit->setFont(m_struct.fontNormal);
   }
}

void MainWindow::documentWasModified()
{    
   setWindowModified(m_textEdit->document()->isModified());
}

QString MainWindow::get_curFileName(int whichTab)
{
   QString name = m_tabWidget->tabWhatsThis(whichTab);

   if (name == "untitled.txt") {
      name = "";
   }

   return name;
}

QString MainWindow::get_DirPath(QString message, QString path)
{
   QFileDialog::Options options;
   options |= QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;

   QString retval;

#ifdef Q_OS_WIN

   if (QSysInfo::WindowsVersion < QSysInfo::WV_VISTA) {

      Dialog_XP_GetDir *dw = new Dialog_XP_GetDir(this, message, path, options);
      int result = dw->exec();

      if (result == QDialog::Accepted) {
         path = dw->getDirectory();
      }

   } else {
      path = QFileDialog::getExistingDirectory(this, message, path, options);

   }

#else
   // on X11 the title bar may not be displayed
   path = QFileDialog::getExistingDirectory(this, message, path, options);

#endif

   if (! path.isEmpty()) {
      // silly adjust for platform slash issue
      QDir temp(path + "/");
      retval = temp.canonicalPath() + "/";
   }

   return retval;
}

struct Settings MainWindow::get_StructData()
{
   return m_struct;
}

int MainWindow::get_Value1(const QString route)
{
   Dialog_GetLine *dw = new Dialog_GetLine();

   if (route == "col") {
      dw->set_ColNo();
   }

   int result = dw->exec();
   int col = 1;

   if ( result == QDialog::Accepted) {
      col = dw->get_Value().toInt();
   }

   delete dw;

   return col;
}

bool MainWindow::loadFile(const QString &fileName, bool addNewTab, bool isAuto)
{
   // part 1
   if (addNewTab)  {
      // test if file is open in another tab
      int pos = m_openedFiles.indexOf(fileName);

      if (pos >= 0) {
         int count = m_tabWidget->count();

         QWidget *temp;
         DiamondTextEdit *textEdit;

         for (int k = 0; k < count; ++k) {

            temp = m_tabWidget->widget(k);
            textEdit = dynamic_cast<DiamondTextEdit *>(temp);

            if (textEdit) {
               QString t_Fname = m_tabWidget->tabWhatsThis(k);

               if (t_Fname == fileName) {
                  // file is alredy open, select the tab

                  m_textEdit = textEdit;
                  m_tabWidget->setCurrentIndex(k);
                  return true;
               }
            }
         }
      }
   }

   // part 2
   QFile file(fileName);

   if (! file.open(QFile::ReadOnly | QFile::Text)) {

      if (! isAuto) {
         // do not show this message

         QString temp = fileName;
         if (temp.isEmpty()) {
            temp = "(No file name available)";
         }

         QString error = tr("Unable to open/read file:  %1\n%2.").arg(temp).arg(file.errorString());
         csError(tr("Open/Read File"), error);
         return false;
      }
   }

   setStatusBar(tr("Loading File..."),0);
   QApplication::setOverrideCursor(Qt::WaitCursor);

   file.seek(0);
   QByteArray temp = file.readAll();  

   if (addNewTab) {
      tabNew();

      m_struct.pathPrior = this->pathName(fileName);

      if (! isAuto) {
         json_Write(PATH_PRIOR);
      }
   }

   QString fileData = QString::fromUtf8(temp);      
   m_textEdit->setPlainText(fileData);   
   QApplication::restoreOverrideCursor();

   setCurrentTitle(fileName);
   setStatusBar(tr("File loaded"), 1500);

   if (! addNewTab)  {
      // recent folders
      rfolder_Add();
   }

   if ( addNewTab && (! isAuto) )  {
      // update open tab list
      openTab_Add();
   }

   return true;
}

QString MainWindow::pathName(QString fileName) const
{
   return QFileInfo(fileName).path();
}

bool MainWindow::querySave()
{
   if (m_textEdit->document()->isModified()) {     

      QString fileName = m_curFile;

      if (m_curFile.isEmpty())  {
         fileName = "(Unknown Filename)";
      }

      QMessageBox quest;
      quest.setWindowTitle(tr("Diamond Editor"));
      quest.setText( fileName + tr(" has been modified. Save changes?"));
      quest.setStandardButtons(QMessageBox::Save | QMessageBox::Discard  | QMessageBox::Cancel );
      quest.setDefaultButton(QMessageBox::Cancel);

      int retval = quest.exec();

      if (retval == QMessageBox::Save) {

         if (fileName == "untitled.txt") {
            return saveAs(false);
         } else {
            return save();
         }

      } else if (retval == QMessageBox::Cancel) {
         return false;

      }
   }

   return true;
}

bool MainWindow::saveFile(const QString &fileName, bool isSaveOne)
{
   int whichTab = m_tabWidget->currentIndex();
   m_tabWidget->setTabText(whichTab, strippedName(fileName));
   m_tabWidget->setTabWhatsThis(whichTab, fileName);


   // BROOM  
   QString t1 = m_tabWidget->tabWhatsThis(whichTab);
   QString t2 = m_tabWidget->tabText(whichTab);

   if (t2 != "untitled.txt") {

      if (fileName != t1 || ! fileName.endsWith(t2))  {

         csError("Save File Error", "Passed FN: " + fileName + "\n  Whats's This: " + t1 + "\n Tab Text: " + t2);

         return false;
      }
   }
   // BROOM


   QFile file(fileName);

   if (! file.open(QFile::WriteOnly | QFile::Text)) {

      QString temp = fileName;
      if (temp.isEmpty()) {
         temp = "(No file name available)";
      }

      QString error = tr("Unable to save/write file %1:\n%2.").arg(temp).arg(file.errorString());
      csError(tr("Save/Write File"), error);
      return false;
   }

   QApplication::setOverrideCursor(Qt::WaitCursor);     
   file.write(m_textEdit->toPlainText().toUtf8());
   QApplication::restoreOverrideCursor();

   if (isSaveOne) {
      m_textEdit->document()->setModified(false);
      setWindowModified(false);     
      setDiamondTitle(m_curFile);

      if (m_isSplit) {
         split_Title();
      }

      setStatusBar(tr("File saved"), 2000);
   }

   return true;
}

QString MainWindow::strippedName(const QString fileName)
{
   return QFileInfo(fileName).fileName();
}

QString MainWindow::suffixName() const
{
   return QFileInfo(m_curFile).suffix().toLower();
}


// title & status bar
void MainWindow::setCurrentTitle(const QString &fileName, bool tabChange)
{
   QString showName;

   // adjusts the * in the title bar
   setWindowModified(m_textEdit->document()->isModified());

   if (fileName.isEmpty()) {

      m_curFile = "";
      showName  = "untitled.txt";

      setStatus_FName(showName);

      // change the name on the tab to "untitled.txt"
      int index = m_tabWidget->currentIndex();

      m_tabWidget->setTabText(index, showName);
      m_tabWidget->setTabWhatsThis(index, showName);

      forceSyntax(SYN_TEXT);

   } else {
      // loading existing file

      m_curFile = fileName;
      showName  = m_curFile;

      setStatus_FName(m_curFile);

      // change the name on the tab to m_curFile
      int index = m_tabWidget->currentIndex();

      m_tabWidget->setTabText(index, strippedName(m_curFile));
      m_tabWidget->setTabWhatsThis(index, m_curFile);

      if (! m_rf_List.contains(m_curFile) ) {
         rf_Update();         
      }

      if (! tabChange)  {
         setSyntax();
      }
   }

   setDiamondTitle(showName);
}

void MainWindow::setDiamondTitle(const QString title)
{
   // displays as: File Name[*] --- Diamond Editor
   // setWindowFilePath(showName);

   // displays as: Diamond Editor --  File Name[*]
   QString temp = QChar(0x02014);
   setWindowTitle("Diamond Editor CS4 " + temp + " " + title + " [*]" );      // BROOM
}

void MainWindow::setStatus_LineCol()
{
   QTextCursor cursor(m_textEdit->textCursor());

   // BROOM - resolve for tabs
   int adjColNum = cursor.columnNumber()+1;

   m_statusLine->setText(" Line: "  + QString::number(cursor.blockNumber()+1) +
                         "  Col: "  + QString::number(adjColNum) + "  ");
}

void MainWindow::setStatus_ColMode()
{
   if (m_struct.isColumnMode) {
      m_statusMode->setText(" Column Mode  ");    

   } else {
      m_statusMode->setText(" Line Mode  ");      
   }

   m_textEdit->set_ColumnMode(m_struct.isColumnMode);
}

void MainWindow::setStatus_FName(QString fullName)
{
   m_statusName->setText(" " + fullName + "  ");
}

void MainWindow::setStatus_FName2(QString text)
{
   // test only !
   m_statusName->setText(" " + text + " ");
}

void MainWindow::setUpTabStops()
{
   int tabStop;

   for (int k = 1; k < 25; ++k)  {
      tabStop = (m_struct.tabSpacing * k) + 1;
      m_tabStops.append(tabStop);
   }
}


// copy buffer
void MainWindow::showCopyBuffer()
{
   QList<QString> copyBuffer = m_textEdit->copyBuffer();

   Dialog_Buffer *dw = new Dialog_Buffer(copyBuffer);
   int result = dw->exec();

   if (result == QDialog::Accepted) {
      int index = dw->get_Index();

      QString text = copyBuffer.at(index);
      m_textEdit->textCursor().insertText(text);
   }

   delete dw;
}


// drag & drop
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
   if (event->mimeData()->hasFormat("text/uri-list"))  {
      event->acceptProposedAction();
   } else if (event->mimeData()->hasFormat("text/plain"))  {
      event->acceptProposedAction();

   }
}

void MainWindow::dropEvent(QDropEvent *event)
{
   const QMimeData *mimeData = event->mimeData();

   if (mimeData->hasUrls()) {

      QList<QUrl> urls = mimeData->urls();
      if (urls.isEmpty()) {
         return;
      }

      QString fileName = urls.first().toLocalFile();
      if (! fileName.isEmpty()) {
         loadFile(fileName, true, false);
      }

   } else if (mimeData->hasText()) {     
      QTextCursor cursor(m_textEdit->textCursor());

      // set for undo stack
      cursor.beginEditBlock();

      cursor.insertText(mimeData->text());

      // set for undo stack
      cursor.endEditBlock();
   }

}
