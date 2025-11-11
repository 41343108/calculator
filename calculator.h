#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QtWidgets>

class Calculator : public QWidget
{
    Q_OBJECT
public:
    explicit Calculator(QWidget *parent = nullptr);

private slots:
    void onDigit();
    void onDot();
    void onBackspace();
    void onOperator();
    void onEqual();
    void onClearAll();     // C 全部清除

private:
    QPushButton* makeButton(const QString &text, const char *member);
    bool applyPending(double operand);
    void setDisplay(const QString &s);
    void appendDisplay(const QString &s);

    QLineEdit *display{};
    QPushButton* digitBtn[10]{};

    QString pendingOp;
    double  accumulator = 0.0;
    bool    waitingForOperand = true;
};

#endif
