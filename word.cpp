#include "word.h"

Word::Word(QString t)
{
    text = t;
    length = t.length();
    frequency = 1;
}

Word::Word(QString t, int len)
{
    text = t;
    length = len;
    frequency = 1;
}

void Word::setLength(int i)
{
    length = i;
}

int Word::getLength() const
{
    return length;
}

void Word::setFrequency(int i)
{
    frequency = i;
}

int Word::getFrequency() const
{
    return frequency;
}

QString Word::getText() const
{
    return text;
}

void Word::operator++()
{
    frequency++;
}

bool Word::operator <(Word &w)
{ 
    // compare frequency, if same, compare text
    if(frequency == w.frequency){
        return (text.compare(w.text, Qt::CaseInsensitive) < 0 );
    }
    return frequency < w.frequency;
}

bool Word::operator >(Word &w)
{
    // compare frequency, if same, compare text
    if(frequency == w.frequency){
        return (text.compare(w.text, Qt::CaseInsensitive) > 0 );
    }
    return frequency > w.frequency;
}

bool Word::operator ==(Word &w)
{
    // compare text
    return (text.compare(w.text, Qt::CaseInsensitive) == 0 );
    //return frequency == w.frequency;
}
