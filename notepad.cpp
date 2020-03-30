#include "notepad.h"
#include "ui_notepad.h"
#include "finddialog.h"
#include "docproperties.h"
#include "word.h"
#include "fstream"
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>
#include <QFontDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QStringList>
#include <QRegularExpression>
#include <QDate>

using std::ofstream;

Notepad::Notepad(const QString & fileName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    ui->textEdit->setFocus();
    ui->actionToolbar->setCheckable(true);
    //ui->actionWord_Count->setCheckable(true);
    setWindowTitle("TextEditor[*]");

    // Connects
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(saveFile()));
    connect(ui->actionSave_As, SIGNAL(triggered(bool)), this, SLOT(saveFileAs()));
    connect(ui->actionCut, SIGNAL(triggered(bool)), ui->textEdit, SLOT(cut()));
    connect(ui->actionPaste, SIGNAL(triggered(bool)), ui->textEdit, SLOT(paste()));
    connect(ui->actionCopy, SIGNAL(triggered(bool)), ui->textEdit, SLOT(copy()));
    connect(ui->actionUndo, SIGNAL(triggered(bool)), ui->textEdit, SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered(bool)), ui->textEdit, SLOT(redo()));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(documentModified()));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(wordCount()));

    findDialog = 0;
    // Doc Properties Creation and connects
    docProperties = new DocProperties;
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(sendNewText()));
    connect(this, SIGNAL(sendingNewText(QString)), docProperties, SLOT(calculateCounts(QString)));
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::on_actionNEw_triggered()
{
    Notepad *editor = new Notepad;
    editor->setFileName();
    editor->show();
}

void Notepad::loadFile(const QString &fileName)
{
    if(fileName.isEmpty()){
        setFileName(QString());
        return;
    }

    QFile newFile(fileName);
    if(!newFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::warning(this, tr("Load File error"),QString(tr("Unable to load file, %s")).arg(fileName));
        setFileName(fileName);
        return;
    }else {
        QTextStream inputFileStream(&newFile);

        ui->textEdit->setText(inputFileStream.readAll());
        newFile.close();
        setFileName(fileName);
        setWindowModified(false);

        QFileInfo fileInfo(newFile);
        QString fileNameWithoutPath (fileInfo.fileName());

        ui->statusBar->showMessage(QString(tr("File %1 successfully opened."))
                                   .arg(fileNameWithoutPath));
    }
}

void Notepad::setFileName(const QString &fileName)
{
    m_fileName = fileName;
    setWindowTitle(QString("%1[*] - %2")
                   .arg(m_fileName.isNull()?"untitled":QFileInfo(m_fileName).fileName())
                   .arg(QApplication::applicationName()));
}

int Notepad::getWordCount()
{
    QString x = ui->textEdit->toPlainText();
    return x.split(QRegularExpression("[.!?,\\s]"), QString::SkipEmptyParts).count();
}

int Notepad::getUniqueWordCount()
{
//    // Get the Full Text
//    QString word = ui->textEdit->toPlainText();
//    // Split the words through punctuation or spaces or enters etc
//    QStringList list = word.split(QRegularExpression("[.!?,\\s]"), QString::SkipEmptyParts);


//    // Count occurences of words that appear only once
//    int count = 0;
//    for(int i = 0; i < list.count(); i++ ){
//        // Get the next word
//        QString currWord = list.at(i);

//        // If the current word only appears once, increment by 1
//        if(list.count(currWord)==1){
//            count++;
//        }
//    }

//    return count;

    QString text = ui->textEdit->toPlainText();
    QStringList wordList = text.split(QRegularExpression("[.!?,\\s]"), QString::SkipEmptyParts);
    QList<Word> uniqueList;
    int count = 0;
    for(int i = 0; i < wordList.count(); i++){
        bool unique = true;
        Word here(wordList.at(i));
        for(int k = 0; k < uniqueList.count(); k++){
            if(!uniqueList.at(k).getText().compare(here.getText(), Qt::CaseInsensitive)){
                unique = false;
                break;
            }
        }
        if(unique){
            here.setFrequency(wordList.count(here.getText()));
            uniqueList.append(here);
            count++;
        }
    }

    return count;
}

void Notepad::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("Text Editor"), tr("This is better than Notepad++"));
}


void Notepad::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("open document"), QDir::currentPath(), tr("Text Documents Only ( *.txt )") );
    if(filename.isNull()) return;

    if(this->m_fileName.isNull() && !isWindowModified())
        loadFile(filename);
    else {
        Notepad *editor = new Notepad();
        editor->loadFile(filename);
        editor->show();
    }
}



void Notepad::on_actionSave_triggered()
{

}

void Notepad::documentModified(){
    this->setWindowModified(true);
}

void Notepad::sendNewText()
{
    QString text = ui->textEdit->toPlainText();

    emit sendingNewText(text);
}

bool Notepad::saveFile()
{
    if(m_fileName.isNull())
        return saveFileAs();
    else {
        QFile newFile(m_fileName);
        if(!newFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Load File error"), QString (tr("Unable to load file, %1"))
                                 .arg(m_fileName));
            setFileName(QString());
            return false;
        }
        else {
            QTextStream outFileStream(&newFile);
            outFileStream<<ui->textEdit->toPlainText();

            newFile.close();

            setWindowModified(false);

            QFileInfo fileInfo(newFile);
            QString fileNameWithoutPath (fileInfo.fileName());

            ui->statusBar->showMessage(QString(tr("File %1 successfully saved."))
                                       .arg(fileNameWithoutPath));
            return true;
        }
    }
}

bool Notepad::saveFileAs(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Document"),
                                                    m_fileName.isNull()?QDir::currentPath():m_fileName,
                                                    tr("Text documents (*.txt)"));

    if(fileName.isNull()){
        return false;
    }
    else{
        setFileName(fileName);
        return saveFile();
    }
}

void Notepad::closeEvent(QCloseEvent *e){
    if(isWindowModified()){
        QMessageBox::StandardButton response = QMessageBox::warning(this, tr("Document has  been modified"),
                                                tr("The document has been modified. Do you wish to save your changes?\nAny Unsaved Changes will be lost."),
                                                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Cancel);

        switch(response){
        case QMessageBox::Yes:
            if(saveFile()){
                e->accept();
            } else {
                e->ignore();
            }
            break;
        case QMessageBox::No:
            e->accept();
            break;
        case QMessageBox::Cancel:
            e->ignore();
            break;
        default:
            break;
        }
    }
    else
        e->accept();

}

void Notepad::on_actionSelect_Font_triggered()
{
    QFontDialog fontDialog;
    bool ok = false;
    QFont font = fontDialog.getFont(&ok, ui->textEdit->font(), this,tr("Choose a font"));
    if(ok){
        ui->textEdit->setFont(font);
    }
}

void Notepad::on_actionToolbar_triggered()
{
    if(!ui->actionToolbar->isChecked()){
        ui->mainToolBar->show();
    } else{
        ui->mainToolBar->hide();
    }
}

void Notepad::on_actionPrint_triggered()
{
    QPrinter printer(QPrinter::HighResolution);

//    printer.setOutputFormat(QPrinter::PdfFormat);
//    QString fName = m_fileName.append(".pdf");
//    printer.setOutputFileName(fName);

//    QPainter painter(&printer);
//    painter.setFont(ui->textEdit->font());
//    painter.drawText(200,200,ui->textEdit->toPlainText());
//    painter.end();


    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));
    if(dlg->exec() == QDialog::Accepted){
        ui->textEdit->print(&printer);
    }
}

void Notepad::on_actionUppercase_triggered()
{
    ui->textEdit->setText(ui->textEdit->toPlainText().toUpper());
}

void Notepad::on_actionLowercase_triggered()
{
    ui->textEdit->setText(ui->textEdit->toPlainText().toLower());
}



void Notepad::on_actionWord_Count_triggered()
{
    QString msg(tr("Word Count: "));

    QString x = ui->textEdit->toPlainText();


    msg.append(QString::number( x.split(" ", QString::SkipEmptyParts).count() ));
    QMessageBox::information(this, tr("Word Count"), msg);
}

void Notepad::wordCount()
{
    QString msg(tr("Word Count: "));

    int count = getWordCount();

    msg.append(QString::number( count ));
    ui->statusBar->showMessage(msg);
}

void Notepad::on_actionFind_triggered()
{
    if(!findDialog){

        findDialog = new FindDialog();
    }
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void Notepad::on_actionReverse_Text_Dot_Follows_triggered()
{
    // Gets original Text
    QString baseT = ui->textEdit->toPlainText();

    // Splits words based on spaces, enters, tabs, etc.
    QStringList wordList = baseT.split(QRegularExpression("\\s"));

    // ReveresedT will be the text backwords.
    QString reversedT("");
    // Iterate backwards to reverse
    for(int k = wordList.count(); k > 0; k--){
        // Grabs last word from list.
        // For some reason does not work with .at(k-1)
        QString currWord = wordList.back();
        // Take off from original text
        baseT.chop(currWord.count());
        if(k != 1){
            // Put words in reverse, seperated by type of space used previously.
            // Allows \n and \t etc
            reversedT.append(currWord).append(baseT.back());
            // Remove seperator
            baseT.chop(1);
        } else {
            // If only one word, there will be no seperator, so append(back()) would crash.
            reversedT.append(currWord);
        }
        // Removes word at back
        wordList.pop_back();
    }

    // Display Text
    ui->textEdit->setText(reversedT);

}

void Notepad::on_actionReverse_Text_triggered()
{

    QString baseT = ui->textEdit->toPlainText().simplified();


    QStringList wordList = baseT.split(" ",QString::SkipEmptyParts);

    for(int k = 0; k < wordList.count(); k++){
        QString currWord = wordList.at(k);
        int splitAt = -1;

        if(currWord.indexOf(QRegularExpression("[.?!,\\s]")) != -1 && currWord.count() > 1){
            splitAt = currWord.indexOf(QRegularExpression("[.?!,\\s]"));

            if(currWord.at(splitAt) == currWord.back()){
                wordList.insert(k+1, QString(currWord.back())); // Insert ahead, as to put it behind
                currWord.chop(1); // Take off last character
            } else if(currWord.at(splitAt) == currWord.front()){
                wordList.insert(k+1, currWord.right(currWord.size() - 1)); // Insert ahead, as to put it behind
                currWord.truncate(1); // Leave only front character
            } else {
                QString endSubstring = currWord.right(currWord.size() - splitAt - 1);
                QChar splitChar = currWord.at(splitAt);
                currWord.chop(currWord.size() - splitAt);
                wordList.insert(k+1, splitChar);
                wordList.insert(k+2, endSubstring);
            }

            wordList.replace(k, currWord);
        }

    }

    QString reversedT("");
    for(int k = wordList.count(); k > 0; k--){
        QString currWord = wordList.back();
        if(currWord.contains(QRegularExpression("[.?!,\\s]"))){
            reversedT.append(currWord);
        } else {
            reversedT.append(currWord).append(" ");
        }
        wordList.pop_back();
    }

    ui->textEdit->setText(reversedT);
}

void Notepad::on_actionUppercase_First_Letter_triggered()
{
    // Get Original Text
    QString baseT = ui->textEdit->toPlainText();
    // Seperate into sentences. By punctuation and tabs, new lines. NOT SPACES
    QStringList sentenceList = baseT.split(QRegularExpression("[.?!\\n\\t\\r]"));

    QString finalT = ""; // Final Text
    for(int i = 0; i < sentenceList.count(); i++){
        // Current Sentence
        QString sentence = sentenceList.at(i);

        // Do when sentence is more than one character.
        if(sentence.size() > 1){
            // Get Front Character
            QChar fro = sentence.front();
            // Fron will be a temp value to hold non characters
            QString fron = "";
            // Put non characters in front, so we capitalize first character
            while(!fro.isLetterOrNumber()){
                fron.append(sentence.front());
                sentence.remove(0,1);
                fro = sentence.front();
            }

            // Pass when next character is letter or number
            // Remove it, then add it back uppercased at front.
            sentence.remove(0,1);
            sentence.prepend(fro.toUpper());
            // At the non character back at front
            sentence.prepend(fron);
        } else {
            // Enter when one character only. Will uppercase one character.
            // Will not cause error at spaces, unlike above.
            sentence = sentence.toUpper();
        }

        // Adds end character sentence. By finding the first, which is current sentence punctuation.
        int endChar = baseT.indexOf(QRegularExpression("[.?!\\n\\t\\r]"));
        if(endChar != -1)
            sentence.append(baseT.at(endChar));

        // Puts sentence back into text
        finalT.append(sentence);
        // Remove all the way to end character, so it ends right before new sentence.
        baseT.remove(0, sentence.size());
    }

    // Set Text
    ui->textEdit->setText(finalT);
}

void Notepad::on_actionUnique_Word_Count_triggered()
{
    int count = getUniqueWordCount();


    // Print words that appear only once
    QMessageBox wordC(ui->textEdit);
    wordC.information(this,tr("Word count"),QString::number(count));
    wordC.show();
}

void Notepad::on_actionSave_As_PDF_triggered()
{
    // Get the file name
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export PDF"),
                                                    m_fileName.isNull()?QDir::currentPath():m_fileName,
                                                    tr("PDF (*.pdf)"));


    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::Letter);

    // If null, put the name to output
    if(fileName.isNull()){
        printer.setOutputFileName("output.pdf");
    }
    else{
        // If not, set the name to specified name
        printer.setOutputFileName(fileName);
    }

    ui->textEdit->print(&printer);

}

// Show the Document Properties
void Notepad::on_actionDocument_Properties_triggered()
{
    docProperties->show();
    docProperties->raise();
    docProperties->activateWindow();

}

// Generate the Statistical File
void Notepad::on_actionGenerate_Statistical_File_triggered()
{
    QString tempfilename = m_fileName;

    // If no file name error
    if(tempfilename.isEmpty())
    {
        QMessageBox::warning(this, tr("File Name Empty"),QString(tr("Please save the file, then try again.")));
        return;
    }

    // Deal with names
    QFileInfo fileInfo(tempfilename);
    QString outputFileName(fileInfo.fileName());
    outputFileName.chop(4);
    outputFileName.append("_statistics.sta");
    QString path(fileInfo.filePath());
    path.chop(4);
    QFile outputFile(path.append("_statistics.sta"));
    if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    // Output human readability Data
    QTextStream out(&outputFile);
    out<<"Statistical Data for \""<<fileInfo.fileName()<<"\"\n";
    out<<"Generated Date: "<<QDate::currentDate().toString()<<endl;
    out<<"Total Word Count: "<<getWordCount()<<endl;
    out<<"Unique Word Count: "<< getUniqueWordCount()<<endl;
    out<<"Word Frequency: "<<endl;

    // Parse Words
    QString text = ui->textEdit->toPlainText();
    QStringList wordList = text.split(QRegularExpression("[.!?,\\s]"), QString::SkipEmptyParts);
    QList<Word> uniqueList;
    for(int i = 0; i < wordList.count(); i++){
        bool unique = true;
        Word here(wordList.at(i));
        for(int k = 0; k < uniqueList.count(); k++){
            if(!uniqueList.at(k).getText().compare(here.getText(), Qt::CaseInsensitive)){
                unique = false;
                break;
            }
        }
        if(unique){
            here.setFrequency(wordList.count(here.getText()));
            uniqueList.append(here);
        }
    }

    // Sort lowest frequency to highest
    std::sort(uniqueList.begin(), uniqueList.end());

    // Put into file highest to lowest
    for(int i = uniqueList.count() - 1; i >= 0; i--){
        out<<uniqueList.at(i).getText()<<" : "<<uniqueList.at(i).getFrequency()<<" : "<<uniqueList.at(i).getLength()<<endl;
    }

    // Close exit
    outputFile.close();
    QMessageBox::information(this, tr("Statistics File Created"),QString(tr("The file %1 has been created successfuly")).arg(outputFileName));

}

