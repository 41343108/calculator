#include "calculator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Calculator w;
    w.show();
    return app.exec();
}
