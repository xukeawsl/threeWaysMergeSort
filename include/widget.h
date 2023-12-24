
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <random>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class QGraphicsScene;
class QGraphicsRectItem;
class SortingThread;

class Widget : public QWidget

{
    Q_OBJECT

private:
    enum class AppState { Waiting, Running };

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void initWidget();

    void initSlots();

public slots:
    void shuffleHeights();

    void sortStart();

private:
    Ui::Widget *ui;

    AppState state;

    SortingThread *sThread;

    QGraphicsScene *scene;
    std::vector<QGraphicsRectItem *> items;

    std::random_device rd;
    std::mt19937 gen;
    std::vector<qreal> heights;
};

#endif    // WIDGET_H
