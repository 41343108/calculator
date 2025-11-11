#include "calculator.h"

Calculator::Calculator(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Qt Calculator (no .ui)");

    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    QFont f = display->font();
    f.setPointSize(f.pointSize() + 6);
    display->setFont(f);

    for (int i = 0; i <= 9; ++i) {
        digitBtn[i] = makeButton(QString::number(i), SLOT(onDigit()));
        digitBtn[i]->setProperty("digit", i);
    }

    auto btnDot  = makeButton(".", SLOT(onDot()));
    auto btnBack = makeButton("←", SLOT(onBackspace()));
    auto btnDiv  = makeButton("÷", SLOT(onOperator()));  btnDiv->setProperty("op", "/");
    auto btnMul  = makeButton("×", SLOT(onOperator()));  btnMul->setProperty("op", "*");
    auto btnSub  = makeButton("−", SLOT(onOperator()));  btnSub->setProperty("op", "-");
    auto btnAdd  = makeButton("+", SLOT(onOperator()));  btnAdd->setProperty("op", "+");
    auto btnEq   = makeButton("=", SLOT(onEqual()));
    auto btnC = makeButton("C", SLOT(onClearAll()));   // 全部清除鍵

    auto root = new QVBoxLayout(this);
    root->addWidget(display);

    auto grid = new QGridLayout();
    root->addLayout(grid);

    grid->addWidget(btnBack, 0, 0);
    grid->addWidget(btnDiv,  0, 1);
    grid->addWidget(btnMul,  0, 2);
    grid->addWidget(btnSub,  0, 3);

    grid->addWidget(digitBtn[7], 1, 0);
    grid->addWidget(digitBtn[8], 1, 1);
    grid->addWidget(digitBtn[9], 1, 2);
    grid->addWidget(btnAdd,      1, 3, 2, 1);

    grid->addWidget(digitBtn[4], 2, 0);
    grid->addWidget(digitBtn[5], 2, 1);
    grid->addWidget(digitBtn[6], 2, 2);

    grid->addWidget(digitBtn[1], 3, 0);
    grid->addWidget(digitBtn[2], 3, 1);
    grid->addWidget(digitBtn[3], 3, 2);
    grid->addWidget(btnEq,       3, 3, 2, 1);

    grid->addWidget(digitBtn[0], 4, 0, 1, 2);
    grid->addWidget(btnDot,      4, 2);

    grid->addWidget(btnC, 5, 0, 1, 4);

    for (int r = 0; r <= 5; ++r) grid->setRowStretch(r, 1);
    for (int c = 0; c <= 3; ++c) grid->setColumnStretch(c, 1);

    setLayout(root);
    setMinimumSize(QSize(420, 640));
}

QPushButton* Calculator::makeButton(const QString &text, const char *member)
{
    auto *btn = new QPushButton(text);
    btn->setMinimumSize(56, 48);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(btn, SIGNAL(clicked()), this, member);
    return btn;
}

void Calculator::setDisplay(const QString &s) { display->setText(s); }
void Calculator::appendDisplay(const QString &s) { display->setText(display->text() + s); }

void Calculator::onDigit()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    int d = btn ? btn->property("digit").toInt() : 0;

    if (display->text() == "0" && d == 0 && waitingForOperand) return;
    if (waitingForOperand) {
        setDisplay(QString::number(d));
        waitingForOperand = false;
    } else {
        appendDisplay(QString::number(d));
    }
}

void Calculator::onDot()
{
    if (waitingForOperand) {
        setDisplay("0.");
        waitingForOperand = false;
        return;
    }
    if (!display->text().contains('.'))
        appendDisplay(".");
}

void Calculator::onBackspace()
{
    if (waitingForOperand) return;
    QString t = display->text();
    if (t.size() > 1) {
        t.chop(1);
        if (t == "-" || t.isEmpty()) t = "0";
    } else {
        t = "0";
    }
    setDisplay(t);
}

void Calculator::onOperator()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    QChar op = btn ? btn->property("op").toString().at(0) : '+';

    double operand = display->text().toDouble();
    if (!pendingOp.isEmpty()) {
        if (!applyPending(operand)) return;
    } else {
        accumulator = operand;
    }
    pendingOp = op;
    waitingForOperand = true;
}

void Calculator::onEqual()
{
    double operand = display->text().toDouble();
    if (!pendingOp.isEmpty()) {
        if (!applyPending(operand)) return;
        pendingOp.clear();
    } else {
        accumulator = operand;
    }
    setDisplay(QString::number(accumulator, 'g', 15));
    waitingForOperand = true;
}
// 全部清除 C
void Calculator::onClearAll()
{
    accumulator = 0.0;
    pendingOp.clear();
    setDisplay("0");
    waitingForOperand = true;
}

bool Calculator::applyPending(double operand)
{
    if (pendingOp == "+")        accumulator += operand;
    else if (pendingOp == "-")   accumulator -= operand;
    else if (pendingOp == "*")   accumulator *= operand;
    else if (pendingOp == "/") {
        if (qFuzzyIsNull(operand)) {
            QMessageBox::warning(this, "Error", "Cannot divide by zero.");
            setDisplay("0");
            accumulator = 0;
            waitingForOperand = true;
            pendingOp.clear();
            return false;
        }
        accumulator /= operand;
    }
    setDisplay(QString::number(accumulator, 'g', 15));
    return true;
}
