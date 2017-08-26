#include "recordingwindow.h"
#include "ui_recordingwindow.h"
#include "sessionevent.h"

#include <QDebug>
#include <QKeyEvent>

RecordingWindow::RecordingWindow(QWidget *parent) : QDialog(parent), ui(new Ui::RecordingWindow)
{
    ui->setupUi(this);

    installEventFilter(this);

    connect(&baseTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));
    baseTimer.start(50);

    startTime = QDateTime::currentDateTime();

    SessionEvent loggedKey;
    loggedKey.TimePressed = startTime;
    loggedKey.MeasurementType = Measurement::Schedule;
    loggedKey.ScheduleType = Schedule::One;

    KeySetEntry loggedKeySet;
    loggedKeySet.KeyCode = Qt::Key_Z;
    loggedKeySet.KeyName = "Z";

    loggedKey.KeyEntered = loggedKeySet;

    AddKey(loggedKey);

    CurrentSchedule = Schedule::One;
}

void RecordingWindow::LoadKeys(KeySet mKeyset)
{
    keySet = mKeyset;

    ui->tableWidgetFrequency->setRowCount(0);

    for (KeySetEntry mKey : keySet.FrequencyKeys)
    {
        ui->tableWidgetFrequency->insertRow(ui->tableWidgetFrequency->rowCount());
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 2, new QTableWidgetItem("0"));
    }

    ui->tableWidgetDuration->setRowCount(0);

    for (KeySetEntry mKey : keySet.DurationKeys)
    {
        ui->tableWidgetDuration->insertRow(ui->tableWidgetDuration->rowCount());
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 2, new QTableWidgetItem("0"));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 3, new QTableWidgetItem("0"));

        DurationFlags.append(false);
        DurationSums.append(0);
        DurationFlaggedTimes.append(QDateTime());
    }
}

void RecordingWindow::SetGroup(QString value)
{
    ui->editGroup->setText(value);
}

void RecordingWindow::SetIndividual(QString value)
{
    ui->editPatient->setText(value);
}

void RecordingWindow::SetEvaluation(QString value)
{
    ui->editEvaluation->setText(value);
}

void RecordingWindow::SetCondition(QString value)
{
    ui->editCondition->setText(value);
}

void RecordingWindow::SetCollector(QString value)
{
    ui->editCollector->setText(value);
}

void RecordingWindow::SetRole(QString value)
{
    ui->editPrimary->setText(value);
}

void RecordingWindow::UpdateGUI()
{
    endTime = QDateTime::currentDateTime();

    ui->editTimerBase->setText(formatTimeLabel(startTime.msecsTo(endTime)));

    ParseTimes();
}

QString RecordingWindow::formatTimeLabel(int msecs)
{
    int msecMod = msecs % 1000;
    int secs = msecs / 1000;
    int mins = (secs / 60) % 60;
    int hours = (secs / 3600);
    secs = secs % 60;

    return QString("%1:%2:%3:%4")
    .arg(hours, 2, 10, QLatin1Char('0'))
    .arg(mins, 2, 10, QLatin1Char('0'))
    .arg(secs, 2, 10, QLatin1Char('0'))
    .arg(msecMod, 2, 10, QLatin1Char('0'));
}

bool RecordingWindow::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::KeyRelease)
    {
        QKeyEvent * mKey = static_cast<QKeyEvent *>(e);

        DetectFrequencyKey(mKey);
        DetectDurationKey(mKey);

        DetectScheduleKey(mKey);

        if (mKey->key() == Qt::Key_Backspace)
        {
            RemoveKey();
        }
    }

    return false;
}

void RecordingWindow::DetectScheduleKey(QKeyEvent * mKey)
{
    if (mKey->key() == Qt::Key_Z)
    {
        if (CurrentSchedule == Schedule::One)
        {
            return;
        }
        else
        {
            Schedule scheduleTrans = (CurrentSchedule == Schedule::Two) ? Schedule::Two : Schedule::Three;
            int scheduleTransCode = (CurrentSchedule == Schedule::Two) ? Qt::Key_X : Qt::Key_C;
            QString scheduleTransString = (CurrentSchedule == Schedule::Two) ? "X" : "C";

            SessionEvent endOldSchedule;
            endOldSchedule.TimePressed = QDateTime::currentDateTime();
            endOldSchedule.MeasurementType = Measurement::Schedule;
            endOldSchedule.ScheduleType = scheduleTrans;

            KeySetEntry loggedScheduleSet;
            loggedScheduleSet.KeyCode = scheduleTransCode;
            loggedScheduleSet.KeyName = scheduleTransString;

            endOldSchedule.KeyEntered = loggedScheduleSet;

            AddKey(endOldSchedule);

        }

        SessionEvent loggedKey;
        loggedKey.TimePressed = QDateTime::currentDateTime();
        loggedKey.MeasurementType = Measurement::Schedule;
        loggedKey.ScheduleType = Schedule::One;

        KeySetEntry loggedKeySet;
        loggedKeySet.KeyCode = mKey->key();
        loggedKeySet.KeyName = mKey->text();

        loggedKey.KeyEntered = loggedKeySet;

        AddKey(loggedKey);

        CurrentSchedule = loggedKey.ScheduleType;
    }

    if (mKey->key() == Qt::Key_X)
    {
        if (CurrentSchedule == Schedule::Two)
        {
            return;
        }
        else
        {
            Schedule scheduleTrans = (CurrentSchedule == Schedule::One) ? Schedule::One : Schedule::Three;
            int scheduleTransCode = (CurrentSchedule == Schedule::Two) ? Qt::Key_Z : Qt::Key_C;
            QString scheduleTransString = (CurrentSchedule == Schedule::One) ? "Z" : "C";

            SessionEvent endOldSchedule;
            endOldSchedule.TimePressed = QDateTime::currentDateTime();
            endOldSchedule.MeasurementType = Measurement::Schedule;
            endOldSchedule.ScheduleType = scheduleTrans;

            KeySetEntry loggedScheduleSet;
            loggedScheduleSet.KeyCode = scheduleTransCode;
            loggedScheduleSet.KeyName = scheduleTransString;

            endOldSchedule.KeyEntered = loggedScheduleSet;

            AddKey(endOldSchedule);

        }

        SessionEvent loggedKey;
        loggedKey.TimePressed = QDateTime::currentDateTime();
        loggedKey.MeasurementType = Measurement::Schedule;
        loggedKey.ScheduleType = Schedule::Two;

        KeySetEntry loggedKeySet;
        loggedKeySet.KeyCode = mKey->key();
        loggedKeySet.KeyName = mKey->text();

        loggedKey.KeyEntered = loggedKeySet;

        AddKey(loggedKey);

        CurrentSchedule = loggedKey.ScheduleType;
    }

    if (mKey->key() == Qt::Key_C)
    {
        if (CurrentSchedule == Schedule::Three)
        {
            return;
        }
        else
        {
            Schedule scheduleTrans = (CurrentSchedule == Schedule::One) ? Schedule::One : Schedule::Two;
            int scheduleTransCode = (CurrentSchedule == Schedule::Two) ? Qt::Key_Z : Qt::Key_X;
            QString scheduleTransString = (CurrentSchedule == Schedule::One) ? "Z" : "X";

            SessionEvent endOldSchedule;
            endOldSchedule.TimePressed = QDateTime::currentDateTime();
            endOldSchedule.MeasurementType = Measurement::Schedule;
            endOldSchedule.ScheduleType = scheduleTrans;

            KeySetEntry loggedScheduleSet;
            loggedScheduleSet.KeyCode = scheduleTransCode;
            loggedScheduleSet.KeyName = scheduleTransString;

            endOldSchedule.KeyEntered = loggedScheduleSet;

            AddKey(endOldSchedule);

        }

        SessionEvent loggedKey;
        loggedKey.TimePressed = QDateTime::currentDateTime();
        loggedKey.MeasurementType = Measurement::Schedule;
        loggedKey.ScheduleType = Schedule::Three;

        KeySetEntry loggedKeySet;
        loggedKeySet.KeyCode = mKey->key();
        loggedKeySet.KeyName = mKey->text();

        loggedKey.KeyEntered = loggedKeySet;

        AddKey(loggedKey);

        CurrentSchedule = loggedKey.ScheduleType;
    }
}

void RecordingWindow::DetectFrequencyKey(QKeyEvent * mKey)
{
    for (int i=0; i<keySet.FrequencyKeys.count(); i++)
    {
        if (mKey->key() == keySet.FrequencyKeys.at(i).KeyCode)
        {
            // TODO more schedules
            SessionEvent loggedKey;
            loggedKey.TimePressed = QDateTime::currentDateTime();
            loggedKey.MeasurementType = Measurement::Rate;
            loggedKey.ScheduleType = Schedule::One;

            KeySetEntry loggedKeySet;
            loggedKeySet.KeyCode = mKey->key();
            loggedKeySet.KeyName = mKey->text();

            loggedKey.KeyEntered = loggedKeySet;

            AddKey(loggedKey);
        }
    }
}

void RecordingWindow::DetectDurationKey(QKeyEvent * mKey)
{
    for (int i=0; i<keySet.DurationKeys.count(); i++)
    {
        if (mKey->key() == keySet.DurationKeys.at(i).KeyCode)
        {
            // TODO more schedules
            SessionEvent loggedKey;
            loggedKey.TimePressed = QDateTime::currentDateTime();
            loggedKey.MeasurementType = Measurement::Time;
            loggedKey.ScheduleType = Schedule::One;

            KeySetEntry loggedKeySet;
            loggedKeySet.KeyCode = mKey->key();
            loggedKeySet.KeyName = mKey->text();

            loggedKey.KeyEntered = loggedKeySet;

            AddKey(loggedKey);
        }
    }
}

void RecordingWindow::AddKey(SessionEvent pressedKey)
{
    PressedKeys.append(pressedKey);

    ui->tableWidgetLog->insertRow(ui->tableWidgetLog->rowCount());

    ui->tableWidgetLog->setItem(ui->tableWidgetLog->rowCount() - 1, 0, new QTableWidgetItem(pressedKey.KeyEntered.KeyName));
    ui->tableWidgetLog->setItem(ui->tableWidgetLog->rowCount() - 1, 1, new QTableWidgetItem("TODO"));
    ui->tableWidgetLog->setItem(ui->tableWidgetLog->rowCount() - 1, 2, new QTableWidgetItem(formatTimeLabel(startTime.msecsTo(pressedKey.TimePressed))));

    ui->tableWidgetLog->scrollToBottom();

    UpdateTables();
}

void RecordingWindow::RemoveKey()
{
    if (PressedKeys.count() == 0)
    {
        return;
    }

    PressedKeys.removeLast();

    ui->tableWidgetLog->removeRow(ui->tableWidgetLog->rowCount() - 1);

    UpdateTables();
}

void RecordingWindow::UpdateTables()
{
    for (int i=0; i<keySet.FrequencyKeys.count(); i++)
    {
        int counter = 0;

        for (int j=0; j<PressedKeys.count(); j++)
        {
            if (PressedKeys[j].MeasurementType == Measurement::Rate &&
                    PressedKeys[j].KeyEntered.KeyCode == keySet.FrequencyKeys[i].KeyCode)
            {
                counter++;
            }
        }

        ui->tableWidgetFrequency->item(i, 2)->setText(QString::number(counter));
    }

    bool waitingForNext;
    QDateTime prev, after;

    for (int i=0; i<keySet.DurationKeys.count(); i++)
    {
        int counter = 0;
        DurationSums[i] = 0;
        waitingForNext = false;

        for (int j=0; j<PressedKeys.count(); j++)
        {
            if (PressedKeys[j].MeasurementType == Measurement::Time &&
                    PressedKeys[j].KeyEntered.KeyCode == keySet.DurationKeys[i].KeyCode)
            {
                if (waitingForNext)
                {
                    after = PressedKeys[j].TimePressed;

                    waitingForNext = false;

                    DurationSums[i] = DurationSums[i] + prev.msecsTo(after);
                }
                else
                {
                    prev = PressedKeys[j].TimePressed;

                    DurationFlaggedTimes[i] = prev;

                    waitingForNext = true;
                }

                counter++;
            }
        }

        ui->tableWidgetDuration->item(i, 2)->setText(QString::number((int)(((double)counter / 2)+(0.5))));

        DurationFlags[i] = (counter % 2) == 1;
    }
}

void RecordingWindow::ParseTimes()
{
    for (int i=0; i<DurationSums.length(); i++)
    {
        if (DurationFlags[i])
        {
            qint64 extraTime = DurationFlaggedTimes[i].msecsTo(QDateTime::currentDateTime());
            ui->tableWidgetDuration->item(i, 3)->setText(formatTimeLabel(DurationSums[i] + extraTime));
        }
        else
        {
            ui->tableWidgetDuration->item(i, 3)->setText(formatTimeLabel(DurationSums[i]));
        }
    }
}

RecordingWindow::~RecordingWindow()
{
    delete ui;
}
