#include "dialoghsvfilter.h"
#include "ui_dialoghsvfilter.h"

DialogHSVFilter::DialogHSVFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHSVFilter)
{
    ui->setupUi(this);
}

DialogHSVFilter::~DialogHSVFilter()
{
    delete ui;
}
