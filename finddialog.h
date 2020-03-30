#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QWidget
{
    Q_OBJECT
public:
    explicit FindDialog(QWidget *parent = nullptr);

signals:
    void findNext(const QString &str, Qt::CaseSensitivity cs);
    void findPrev(const QString &str, Qt::CaseSensitivity cs);

private slots:
    void findClicked();
    void enableFindButton(const QString &text);

private:
    QCheckBox *caseCheckBox;
    QCheckBox *backwardCheckBox;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *findButton;
    QPushButton *closeButton;
};

#endif // FINDDIALOG_H
