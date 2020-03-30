#ifndef DOCPROPERTIES_H
#define DOCPROPERTIES_H

#include <QTabWidget>
#include "charts.h"

class Charts;

namespace Ui {
class DocProperties;
}

class DocProperties : public QTabWidget
{
    Q_OBJECT

public:
    explicit DocProperties(QWidget *parent = nullptr);

    ~DocProperties();

private slots:
    void on_wordCountCombobox_currentIndexChanged(int index);
    void on_characterCountComboBox_currentIndexChanged(int index);
    void calculateCounts(QString text);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DocProperties *ui;
    int wordCount;
    int uniqueWordCount;
    int characterCountExcluding;
    int characterCountIncluding;
    int paragraphCount;
    int lineCount;
    QString notepadText;
    //Charts *c;
};

#endif // DOCPROPERTIES_H
