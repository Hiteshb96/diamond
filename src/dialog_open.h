/**************************************************************************
*
* Copyright (c) 2012-2015 Barbara Geller
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

#ifndef DIALOG_OPEN_H
#define DIALOG_OPEN_H

#include "ui_dialog_open.h"
#include "mainwindow.h"

#include <QDialog>

class Dialog_Open : public QDialog
{
   CS_OBJECT(Dialog_Open)

   public:
      Dialog_Open(MainWindow *parent, QStringList list);
      ~Dialog_Open();
      QString get_FileName();

   private:
      Ui::Dialog_Open *m_ui;    
      QStringList m_list;

      CS_SLOT_1(Private, void Open())
      CS_SLOT_2(Open) 

      CS_SLOT_1(Private, void Cancel())
      CS_SLOT_2(Cancel)      
};

#endif