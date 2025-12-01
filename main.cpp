#include <QApplication>
#include "imageprocessing.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageProcessing w;
    w.show();
    return a.exec();
}
