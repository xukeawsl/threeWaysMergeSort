
#include "widget.h"

#include <QGraphicsRectItem>

#include "./ui_widget.h"
#include "sortingthread.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget),
      state(AppState::Waiting),
      sThread(nullptr),
      scene(new QGraphicsScene),
      gen(rd()) {
    ui->setupUi(this);

    initWidget();

    initSlots();

    shuffleHeights();
}

Widget::~Widget() {
    if (sThread != nullptr) {
        sThread->terminate();
        delete sThread;
    }
    delete ui;
}

void Widget::initWidget() {
    this->setFixedSize(1000, 600);

    ui->btn_shuffle->setCursor(Qt::PointingHandCursor);
    ui->btn_sort->setCursor(Qt::PointingHandCursor);

    ui->btn_sort->setStyleSheet(
        "background-color: rgb(85, 0, 255); color: #fff");

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qreal w = ui->graphicsView->rect().size().width();
    qreal h = ui->graphicsView->rect().size().height();

    scene->setSceneRect(-w / 2, -h / 2, w, h);
    scene->addLine(-w / 2, 0, w / 2, 0);
    scene->addLine(0, -h / 2, 0, h / 2);

    scene->setBackgroundBrush(Qt::black);

    ui->graphicsView->setScene(scene);
}

void Widget::initSlots() {
    connect(ui->btn_shuffle, &QPushButton::clicked, this,
            &Widget::shuffleHeights);

    connect(ui->btn_sort, &QPushButton::clicked, this, [this] {
        if (state == AppState::Waiting) {
            sortStart();
            ui->btn_sort->setText("Terminate");
            ui->btn_sort->setStyleSheet("background-color: #000; color: #fff");
            ui->btn_sort->setEnabled(true);
            state = AppState::Running;
        } else if (state == AppState::Running) {
            if (sThread != nullptr) {
                sThread->terminate();
                delete sThread;
                sThread = nullptr;
            }
            ui->btn_shuffle->setEnabled(true);
            ui->btn_sort->setEnabled(false);
            ui->btn_sort->setText("Sort");
            ui->btn_sort->setStyleSheet(
                "background-color: rgb(85, 0, 255); color: #fff");
            state = AppState::Waiting;
        }
    });

    connect(ui->horizontalSlider, &QSlider::valueChanged, this,
            [this](int value) {
                if (sThread != nullptr) {
                    sThread->updateSortDelay(value);
                }
            });

    connect(ui->spinBox_ArraySize, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) {
                if (state == AppState::Waiting) {
                    shuffleHeights();
                }
            });
}

void Widget::shuffleHeights() {
    int ArraySize = ui->spinBox_ArraySize->value();

    scene->clear();
    heights.resize(ArraySize);
    items.resize(ArraySize);

    qreal w = ui->graphicsView->rect().size().width();
    qreal h = ui->graphicsView->rect().size().height() / 2;

    qreal dw = w / ArraySize;
    qreal dh = h / ArraySize;

    qreal x = -w / 2;

    w = dw;
    h = dh;

    for (int i = 0; i < ArraySize; i++) {
        QGraphicsRectItem *item = new QGraphicsRectItem;

        item->setRect(x, 0, w, 0);
        item->setBrush(Qt::red);

        heights[i] = h;
        items[i] = item;

        x += dw;
        h += dh;
    }

    std::shuffle(heights.begin(), heights.end(), gen);

    for (int i = 0; i < ArraySize; i++) {
        auto r = items[i]->rect();
        r.setY(-heights[i]);
        r.setHeight(heights[i]);

        items[i]->setRect(r);

        scene->addItem(items[i]);
    }

    ui->btn_sort->setEnabled(true);
}

void Widget::sortStart() {
    if (sThread != nullptr) {
        delete sThread;
        sThread = nullptr;
    }

    sThread = new SortingThread(items, ui->graphicsView->rect().width(),
                                ui->graphicsView->rect().height(),
                                ui->horizontalSlider->value());

    connect(sThread, &SortingThread::signal_moveBy, this,
            [this](int index, qreal dx, qreal dy) {
                auto r = items[index]->rect();
                items[index]->setRect(r.x() + dx, r.y() + dy, r.width(),
                                      r.height());
            });

    connect(sThread, &SortingThread::signal_changeColor, this,
            [this](int index, const QBrush &brush) {
                items[index]->setBrush(brush);
            });

    connect(sThread, &SortingThread::signal_sortComplete, this, [=] {
        ui->btn_shuffle->setEnabled(true);
        ui->btn_sort->setEnabled(false);
        sThread->terminate();
        delete sThread;
        sThread = nullptr;

        ui->btn_sort->setText("Sort");
        ui->btn_sort->setStyleSheet(
            "background-color: rgb(85, 0, 255); color: #fff");

        state = AppState::Waiting;
    });

    ui->btn_shuffle->setEnabled(false);
    ui->btn_sort->setEnabled(false);

    sThread->start();
}
