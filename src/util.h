/**************************************************************************
*
* Copyright (c) 2012-2018 Barbara Geller
* All rights reserved.
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
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

#ifndef UTIL_H
#define UTIL_H

#include <QMessageBox>
#include <QString>

void csMsg(const QString &msg);
void csMsg(QString msg, int value);
void csMsg(QWidget *parent, const QString &title, const QString &msg);
void csError(QString title, QString msg);
void showDialog(QDialog &t_Dialog);

void showHtml(QString route, QString file);

#endif
