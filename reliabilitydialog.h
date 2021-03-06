/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Data Tracker is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Data Tracker is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Data Tracker.  If not, see http://www.gnu.org/licenses/.

   The Data Tracker is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#ifndef RELIABILITYDIALOG_H
#define RELIABILITYDIALOG_H

#include <QDialog>
#include "directorysearcher.h"
#include "reliabilityparse.h"
#include "reliabilitymeasure.h"

#include "displaytabledialog.h"

namespace Ui {
class ReliabilityDialog;
}

class ReliabilityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReliabilityDialog(QString mCurrentWorkingDirectory = "", QWidget *parent = 0);
    ~ReliabilityDialog();

public slots:
    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

private slots:
    void on_comboGroup_currentIndexChanged(int index);

    void on_comboIndividual_currentIndexChanged(int index);

    void on_comboEvaluation_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    Ui::ReliabilityDialog *ui;

    DisplayTableDialog tableDialog;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread *workerThread;
    DirectorySearcher *worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;
    QList<ReliabilityParse> SecondaryReliabilityObjects;

    QList<ReliabilityMeasure> ReliResults;

    QList<QStringList> mOutput;
};

#endif // RELIABILITYDIALOG_H
