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

#ifndef DIALOG_PRESET_H
#define DIALOG_PRESET_H

#include "ui_dialog_preset.h"
#include "mainwindow.h"

#include <QDialog>
#include <QString>
#include <QStringList>

class Dialog_Preset : public QDialog
{     
   CS_OBJECT(Dialog_Preset)

   public:
      Dialog_Preset(MainWindow *from, QStringList data);
      ~Dialog_Preset();

      QStringList getData();

   private:
      Ui::Dialog_Preset *m_ui;
      QStringList m_dataList;
      MainWindow *m_parent;

      void verify_Folder(QString number, QLineEdit *field);
      void pick_Folder(QString number, QLineEdit *field);

      CS_SLOT_1(Private, void save())
      CS_SLOT_2(save)

      CS_SLOT_1(Private, void cancel())
      CS_SLOT_2(cancel)

};

#endif
