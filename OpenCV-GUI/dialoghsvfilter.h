#ifndef DIALOGHSVFILTER_H
#define DIALOGHSVFILTER_H

#include <QDialog>

namespace Ui {
class DialogHSVFilter;
}

class DialogHSVFilter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHSVFilter(QWidget *parent = nullptr);
    ~DialogHSVFilter();

private:
    Ui::DialogHSVFilter *ui;
};

#endif // DIALOGHSVFILTER_H
