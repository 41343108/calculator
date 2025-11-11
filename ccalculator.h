#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QtWidgets>

class Calculator : public QWidget
{
    Q_OBJECT
public:
    explicit Calculator(QWidget *parent = nullptr);

private slots:
    void onDigit();           // 0-9
    void onDot();             // .
    void onClearAll();        // C
    void onClearEntry();      // CE
    void onBackspace();       // ⌫
    void onChangeSign();      // ±
    void onOperator();        // + - * /
    void onEqual();           // =

private:
    QPushButton* makeButton(const QString &text, const char *member);
    QPushButton* makeButton(const QString &text, std::function<void()> fn);

    bool applyPending(double operand);
    void setDisplay(const QString &s);
    void appendDisplay(const QString &s);

    QLineEdit *display{};
    QPushButton* digitBtn[10]{};

    QString pendingOp;     // "+", "-", "*", "/"
    double  accumulator = 0.0;
    bool    waitingForOperand = true;
};

#endif // CALCULATOR_H
