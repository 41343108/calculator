#include "ccalculator.h"

Calculator::Calculator(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Qt Calculator (qmake, no .ui)");

    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    QFont f = display->font();
    f.setPointSize(f.pointSize() + 6);
    display->setFont(f);

    // digits
    for (int i = 0; i <= 9; ++i) {
        digitBtn[i] = makeButton(QString::number(i), SLOT(onDigit()));
        digitBtn[i]->setProperty("digit", i);
    }

    // other buttons
    auto btnDot  = makeButton(".",       SLOT(onDot()));
    auto btnC    = makeButton("C",       SLOT(onClearAll()));
    auto btnCE   = makeButton("CE",      SLOT(onClearEntry()));
    auto btnBack = makeButton("⌫",       SLOT(onBackspace()));
    auto btnSign = makeButton("±",       SLOT(onChangeSign()));

    auto btnDiv  = makeButton("÷",       SLOT(onOperator()));  btnDiv->setProperty("op", "/");
    auto btnMul  = makeButton("×",       SLOT(onOperator()));  btnMul->setProperty("op", "*");
    auto btnSub  = makeButton("−",       SLOT(onOperator()));  btnSub->setProperty("op", "-");
    auto btnAdd  = makeButton("+",       SLOT(onOperator()));  btnAdd->setProperty("op", "+");
    auto btnEq   = makeButton("=",       SLOT(onEqual()));

    // ----- layout (依草圖，大致配置，+ 與 = 做直向加高) -----
    auto root = new QVBoxLayout(this);
    root->addWidget(display);

    auto grid = new QGridLayout();
    root->addLayout(grid);

    // Top row
    grid->addWidget(btnBack, 0, 0);
    grid->addWidget(btnDiv,  0, 1);
    grid->addWidget(btnMul,  0, 2);
    grid->addWidget(btnSub,  0, 3);

    // 7 8 9
    grid->addWidget(digitBtn[7], 1, 0);
    grid->addWidget(digitBtn[8], 1, 1);
    grid->addWidget(digitBtn[9], 1, 2);
    grid->addWidget(btnAdd,      1, 3, 3, 1); // plus spans 3 rows

    // 4 5 6
    grid->addWidget(digitBtn[4], 2, 0);
    grid->addWidget(digitBtn[5], 2, 1);
    grid->addWidget(digitBtn[6], 2, 2);

    // 1 2 3
    grid->addWidget(digitBtn[1], 3, 0);
    grid->addWidget(digitBtn[2], 3, 1);
    grid->addWidget(digitBtn[3], 3, 2);

    // Bottom row: CE, 0, dot, and a tall "=" on far right
    grid->addWidget(btnCE,        4, 0);
    grid->addWidget(digitBtn[0],  4, 1);
    grid->addWidget(btnDot,       4, 2);
    grid->addWidget(btnEq,        1, 4, 4, 1); // tall equals

    // Under grid: C + ±
    auto bottom = new QHBoxLayout();
    bottom->addWidget(btnC);
    bottom->addWidget(btnSign);
    root->addLayout(bottom);

    setLayout(root);
    setMinimumSize(QSize(360, 480));

    // ---- keyboard shortcuts ----
    for (int i = 0; i <= 9; ++i)
        new QShortcut(QKeySequence(Qt::Key_0 + i), this, [=]{ digitBtn[i]->click(); });
    new QShortcut(QKeySequence(Qt::Key_Period), this, [=]{ btnDot->click(); });
    new QShortcut(QKeySequence(Qt::Key_Plus),   this, [=]{ btnAdd->click(); });
    new QShortcut(QKeySequence(Qt::Key_Minus),  this, [=]{ btnSub->click(); });
    new QShortcut(QKeySequence(Qt::Key_Asterisk), this, [=]{ btnMul->click(); });
    new QShortcut(QKeySequence(Qt::Key_Slash),    this, [=]{ btnDiv->click(); });
    new QShortcut(QKeySequence(Qt::Key_Return),   this, [=]{ btnEq->click(); });
    new QShortcut(QKeySequence(Qt::Key_Enter),    this, [=]{ btnEq->click(); });
    new QShortcut(QKeySequence(Qt::Key_Backspace),this, [=]{ btnBack->click(); });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_L), this, [=]{ btnC->click(); });
}

// ---------- helpers ----------
QPushButton* Calculator::makeButton(const QString &text, const char *member)
{
    auto *btn = new QPushButton(text);
    btn->setMinimumSize(56, 48);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(btn, SIGNAL(clicked()), this, member);
    return btn;
}

QPushButton* Calculator::makeButton(const QString &text, std::function<void()> fn)
{
    auto *btn = new QPushButton(text);
    btn->setMinimumSize(56, 48);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(btn, &QPushButton::clicked, this, fn);
    return btn;
}

void Calculator::setDisplay(const QString &s) { display->setText(s); }
void Calculator::appendDisplay(const QString &s) { display->setText(display->text() + s); }

// ---------- slots ----------
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

void Calculator::onClearAll()
{
    accumulator = 0.0;
    pendingOp.clear();
    setDisplay("0");
    waitingForOperand = true;
}

void Calculator::onClearEntry()
{
    setDisplay("0");
    waitingForOperand = true;
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

void Calculator::onChangeSign()
{
    if (display->text() == "0") return;
    if (display->text().startsWith('-'))
        setDisplay(display->text().mid(1));
    else
        setDisplay("-" + display->text());
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

bool Calculator::applyPending(double operand)
{
    if (pendingOp == "+")        accumulator += operand;
    else if (pendingOp == "-")   accumulator -= operand;
    else if (pendingOp == "*")   accumulator *= operand;
    else if (pendingOp == "/") {
        if (qFuzzyIsNull(operand)) {
            QMessageBox::warning(this, "Error", "Cannot divide by zero.");
            onClearAll();
            return false;
        }
        accumulator /= operand;
    }
    setDisplay(QString::number(accumulator, 'g', 15));
    return true;
}
