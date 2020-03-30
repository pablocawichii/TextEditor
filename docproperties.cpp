#include "docproperties.h"
#include "ui_docproperties.h"
#include <QComboBox>
#include "word.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QBarSet>
#include <QBarSeries>
#include <QChartView>
#include <QLineSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QChart>
#include <QStringList>
#include <QPieSeries>
#include <QPen>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

DocProperties::DocProperties(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::DocProperties)
{
    ui->setupUi(this);

    // Initialize the variables
    wordCount = 0;
    uniqueWordCount = 0;
    characterCountExcluding = 0;
    characterCountIncluding = 0;
    paragraphCount = 0;
    lineCount = 0;

    // Fill text for counts
    if(ui->wordCountCombobox->currentIndex() == 0){
        ui->wordCountLineEdit->setText(QString::number(wordCount));
    } else {
        // Unique Word Count is every word counted once, not single words counted
        ui->wordCountLineEdit->setText(QString::number(uniqueWordCount));
    }

    // Fill text for counts with and without space
    if(ui->characterCountComboBox->currentIndex() == 0){
        ui->characterCountLineEdit->setText(QString::number(characterCountExcluding));
    } else {
        ui->characterCountLineEdit->setText(QString::number(characterCountIncluding));
    }

    // Line and Paragraph Counts
    ui->paragraphCountLineEdit->setText(QString::number(paragraphCount));
    ui->lineCountLineEdit->setText(QString::number(lineCount));
}

DocProperties::~DocProperties()
{
    delete ui;
}

void DocProperties::on_wordCountCombobox_currentIndexChanged(int index)
{
    // When index changes, change text
    switch (index) {
    case 0:
        ui->wordCountLineEdit->setText(QString::number(wordCount));
        break;
    case 1:
        ui->wordCountLineEdit->setText(QString::number(uniqueWordCount));
        break;
    default:
        ui->wordCountLineEdit->setText("Error");
    }
}


void DocProperties::on_characterCountComboBox_currentIndexChanged(int index)
{
    // When index changes, change text
    switch (index) {
    case 0:
        ui->characterCountLineEdit->setText(QString::number(characterCountExcluding));
        break;
    case 1:
        ui->characterCountLineEdit->setText(QString::number(characterCountIncluding));
        break;
    default:
        ui->characterCountLineEdit->setText("Error");
    }
}

void DocProperties::calculateCounts(QString text)
{
    // character counts
    characterCountIncluding = text.length();
    notepadText = text;

    // Split words
    QStringList wordList = text.split(QRegularExpression("[.!?,\\s]"), QString::SkipEmptyParts);
    wordCount = wordList.count(); // Word counts
    int tempCharacterCount = 0; // To hold character counts without spaces
    QList<Word> uniqueList;
    int count = 0;
    // Get unique words
    for(int i = 0; i < wordList.count(); i++){
        bool unique = true;
        Word here(wordList.at(i));
        tempCharacterCount += here.getText().length();
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
    uniqueWordCount = count;
    characterCountExcluding = tempCharacterCount;

    // Paragraph count
    QStringList paragraphList = text.split(QRegularExpression("[\n]"), QString::SkipEmptyParts);
    paragraphCount = paragraphList.count();

    // Line Count
    QStringList lineList = text.split(QRegularExpression("[\r\n]"), QString::SkipEmptyParts);
    lineCount = lineList.count();

    // Update text
    if(ui->wordCountCombobox->currentIndex() == 0){
        ui->wordCountLineEdit->setText(QString::number(wordCount));
    } else {
        ui->wordCountLineEdit->setText(QString::number(uniqueWordCount));
    }

    // Update text
    if(ui->characterCountComboBox->currentIndex() == 0){
        ui->characterCountLineEdit->setText(QString::number(characterCountExcluding));
    } else {
        ui->characterCountLineEdit->setText(QString::number(characterCountIncluding));
    }

    // Update text
    ui->paragraphCountLineEdit->setText(QString::number(paragraphCount));
    ui->lineCountLineEdit->setText(QString::number(lineCount));

}


// Button for chart
void DocProperties::on_buttonBox_accepted()
{

    // How many words will be in the list
    int quan = 0;
    switch(ui->quantityComboBox->currentIndex()){
        case 0:
        quan = 10;
        break;
    case 1:
        quan = 20;
        break;
    case 2:
        quan = -1;
        break;
    }



    QList<Word> listOfWords; // To hold the words

    if(ui->dataSourceComboBox->currentIndex() == 0){ // Open File
        QString filename = QFileDialog::getOpenFileName(this, tr("open document"), QDir::currentPath(), tr("STA Documents Only ( *.sta )") );
        if(filename.isNull()) return; // If click cancel do not error since did not want to create graph

        QFile dataFile(filename);

        // If file empty, dont open
        if(!dataFile.size()){
            QMessageBox::warning(this, tr("File Empty"), QString (tr("File %1 is empty."))
                                 .arg(filename));
            return;
        }

        // If file doesnt open, end
        if(!dataFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, tr("Load File error"), QString (tr("Unable to load file, %1"))
                                 .arg(filename));
            return;
        }

        QStringList lines;
        QTextStream in(&dataFile);
        in.readLine();
        in.readLine();
        in.readLine();
        in.readLine();
        in.readLine();
        int count = 0;
        bool all = false;
        if(quan == -1){
            quan = INT_MAX;
            all = true;
        }
        while(!in.atEnd() && count < quan){
//            QString line = in.readLine();
//            lines.append(line);
//            count++;
            try{
                Word currWord("");
                in>>currWord;
                listOfWords.append(currWord);
                count++;
            }
            catch (...){
                qDebug()<<"An error has occured while reading the file.\nSkipping Word.";

            }
        }

        if(all)
            quan = count;

        dataFile.close();

//        // Remove the 5 human data tabs
//        lines.removeFirst();
//        lines.removeFirst();
//        lines.removeFirst();
//        lines.removeFirst(); // Commented code is before overloading QTextStream >>
//        lines.removeFirst();

//        // Get the actual data
//        if(quan == -1 || quan > lines.count()){
//            quan = lines.count();
//        }
//        for(int i = 0; i < quan; i++){
//            QString a = lines.at(i);
//            QStringList currLine = a.split(QRegularExpression(" : "));
//            Word tempWord = Word(currLine.at(0), currLine.at(2).toInt());
//            tempWord.setFrequency(currLine.at(1).toInt());
//            listOfWords.append(tempWord);
//        }

    } else { // Auto Graph
        if(notepadText.isEmpty()){ // Empty notepad
            QMessageBox::warning(this, tr("Read File error"), QString (tr("Cannot graph empty file.")));
            return;
        }

        QStringList wordList = notepadText.split(QRegularExpression("[.!?,\\s]"), QString::SkipEmptyParts);
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
        } // Get unique list

        // Put words into list from most frequent to least frequent
        std::sort(uniqueList.begin(), uniqueList.end());
        if(quan == -1 || quan > uniqueList.count()){
            quan = uniqueList.count();
        }
        for(int i = uniqueList.count()-1; i > (uniqueList.count()-1 - quan); i--){
            listOfWords.append(uniqueList.at(i));
        }
    }

    // Create the chart
    Charts *c = new Charts();
    c->show();

    if(ui->chartTypeComboBox->currentIndex() == 0){
        // Line Chart
        QLineSeries *series = new QLineSeries();

        for(int i = 0; i < quan; i++){
            series->append(i,listOfWords.at(i).getFrequency());
        }

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Frequency of Words");
        chart->legend()->hide();

        chart->createDefaultAxes();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        c->setCentralWidget(chartView);

    } else if(ui->chartTypeComboBox->currentIndex() == 1){
        // Bar Chart
        QBarSeries *barSeries = new QBarSeries();
        QStringList categories;


        QBarSet *set = new QBarSet("Count");
        for(int i = 0; i < quan; i++){
            categories<<(listOfWords.at(i).getText());
            *set<<(listOfWords.at(i).getFrequency());
        }
        barSeries->append(set);


        QChart *chart = new QChart();
        chart->addSeries(barSeries);
        chart->setTitle("Most Frequent Words");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QBarCategoryAxis *axis = new QBarCategoryAxis();
        axis->append(categories);
        chart->createDefaultAxes();
        chart->setAxisX(axis,barSeries);

        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        c->setCentralWidget(chartView);

    } else {
        // Pie Chart
        QPieSeries *series = new QPieSeries();

        int max = listOfWords.at(0).getFrequency();

        for(int i = 0; i < quan; i++){
            series->append(listOfWords.at(i).getText(),listOfWords.at(i).getFrequency());
            if(listOfWords.at(i).getFrequency() == max){ // Show all most frequent words
                QPieSlice *slice = series->slices().at(i);
                slice->setExploded();
                slice->setLabelVisible();
            }
        }

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Most Frequent Words");
        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        c->setCentralWidget(chartView);
    }
}

void DocProperties::on_buttonBox_rejected()
{ // If cancelled, hide the thing
    this->hide();
}
