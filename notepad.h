#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>

class FindDialog;
class DocProperties;

namespace Ui {
class Notepad;
}

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Notepad(const QString & fileName=QString(), QWidget *parent = nullptr);
    ~Notepad();

signals:
    void sendingNewText(QString);

private slots:
    void on_actionNEw_triggered();

    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    bool saveFile();
    bool saveFileAs();
    void documentModified();
    void sendNewText();

    void on_actionSelect_Font_triggered();

    void on_actionToolbar_triggered();

    void on_actionPrint_triggered();

    void on_actionUppercase_triggered();

    void on_actionLowercase_triggered();

    void on_actionWord_Count_triggered();

    void wordCount();

    void on_actionFind_triggered();

    void on_actionReverse_Text_Dot_Follows_triggered();

    void on_actionReverse_Text_triggered();

    void on_actionUppercase_First_Letter_triggered();

    void on_actionUnique_Word_Count_triggered();

    void on_actionSave_As_PDF_triggered();

    void on_actionDocument_Properties_triggered();

    void on_actionGenerate_Statistical_File_triggered();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::Notepad *ui;
    QString m_fileName;
    FindDialog * findDialog;
    DocProperties * docProperties;

    void loadFile (const QString &fileName);
    void setFileName (const QString &fileName = nullptr);
    int getWordCount(); // Word count
    int getUniqueWordCount(); // Word count with only 1 frequency
};

#endif // NOTEPAD_H
