
#ifndef SORTINGTHREAD_H
#define SORTINGTHREAD_H

#include <QAtomicInteger>
#include <QBrush>
#include <QRectF>
#include <QThread>

class QGraphicsRectItem;

class SortingThread : public QThread {
    Q_OBJECT
public:
    SortingThread(const std::vector<QGraphicsRectItem *> &items, qreal w,
                  qreal h, unsigned long delay = 1);

    void updateSortDelay(unsigned long delay);

protected:
    void run() override;

private:
    void threeWaysMergeSort(int left, int right);

    void merge(int left, int mid1, int mid2, int right);

    void moveItemToTemp(int index, QRectF dest);

    void moveTempToItem(int index);

signals:
    void signal_moveBy(int index, qreal dx, qreal dy);

    void signal_changeColor(int index, const QBrush &brush);

    void signal_sortComplete();

private:
    const std::vector<QGraphicsRectItem *> &items;
    qreal width;
    qreal height;
    QAtomicInteger<unsigned long> sortDelay;
    std::vector<int> height_idx;
    std::vector<int> temp_idx;
    std::vector<QRectF> temp_rect;
};

#endif    // SORTINGTHREAD_H
