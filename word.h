#ifndef WORD_H
#define WORD_H

#include <QString>
#include <iostream>
#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <QStringList>

using std::ostream;
using std::istream;

class Word
{
public:
    Word(QString t);
    Word(QString t, int len);
    void setLength(int i);
    int getLength() const;
    void setFrequency(int i);
    int getFrequency() const;
    QString getText() const;
    void operator++();
    bool operator <(Word & w);
    bool operator >(Word & w);
    bool operator ==(Word & w);
    // Output
    friend ostream& operator<<(ostream & out, const Word & w){
        qDebug()<<w;
        return out;
    }

    // Output
    friend QDebug operator <<(QDebug out, const Word & w){
        out<<w.text<<w.frequency;
        return out;
    }

    friend QTextStream& operator >>(QTextStream &in, Word & w){
        QString text = in.readLine();
        QStringList splitUp = text.split(QRegularExpression(" : "));
        // If error throw so it doesnt crash
        if(splitUp.count() != 3){
            throw 1;
        }
        w.text = QString(splitUp.at(0));
        w.frequency = splitUp.at(1).toInt();
        w.length= splitUp.at(2).toInt();
        return in;
    }

    friend istream& operator>>(istream &in, Word & w){
        char t[99];
        in.getline(t, 99, '\n');

        QString text(t);
        QStringList splitUp = text.split(QRegularExpression(" : "));
        // If error throw so it doesnt crash
        if(splitUp.count() != 3){
            throw 2;
        }
        w.text = QString(splitUp.at(0));
        w.frequency = splitUp.at(1).toInt();
        w.length= splitUp.at(2).toInt();
        return in;
    }

private:
    QString text;
    int length;
    int frequency;

};

#endif // WORD_H
