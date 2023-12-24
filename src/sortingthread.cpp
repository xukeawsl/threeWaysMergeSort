
#include "sortingthread.h"

#include <QGraphicsRectItem>

SortingThread::SortingThread(const std::vector<QGraphicsRectItem *> &items,
                             qreal w, qreal h, unsigned long delay)
    : items(items),
      width(w),
      height(h),
      sortDelay(delay),
      height_idx(items.size()),
      temp_idx(items.size()),
      temp_rect(items.size()) {
    for (int i = 0; i < items.size(); i++) {
        auto item_rect = items[i]->rect();

        height_idx[i] = i;
        temp_rect[i].setRect(item_rect.x(), height / 2, item_rect.width(), 0);
    }
}

void SortingThread::updateSortDelay(unsigned long delay) { sortDelay = delay; }

void SortingThread::run() {
    threeWaysMergeSort(0, items.size() - 1);

    for (auto idx : height_idx) {
        emit signal_changeColor(idx, Qt::green);
        QThread::usleep(sortDelay);
    }

    emit signal_sortComplete();
}

void SortingThread::threeWaysMergeSort(int left, int right) {
    if (left >= right) return;

    int len = right - left + 1;

    int mid1 = left + len / 3;
    int mid2 = left + 2 * len / 3;

    threeWaysMergeSort(left, mid1);
    threeWaysMergeSort(mid1 + 1, mid2);
    threeWaysMergeSort(mid2 + 1, right);

    merge(left, mid1, mid2, right);
}

void SortingThread::merge(int left, int mid1, int mid2, int right) {
    int i = left, j = mid1 + 1, k = mid2 + 1;

    int idx = left;

    while (i <= mid1 && j <= mid2 && k <= right) {
        if (items[height_idx[j]]->rect().height() <
                items[height_idx[i]]->rect().height() &&
            items[height_idx[j]]->rect().height() <=
                items[height_idx[k]]->rect().height()) {
            moveItemToTemp(height_idx[j], temp_rect[idx]);
            temp_idx[idx++] = height_idx[j++];
        } else if (items[height_idx[k]]->rect().height() <
                       items[height_idx[i]]->rect().height() &&
                   items[height_idx[k]]->rect().height() <
                       items[height_idx[j]]->rect().height()) {
            moveItemToTemp(height_idx[k], temp_rect[idx]);
            temp_idx[idx++] = height_idx[k++];
        } else {
            moveItemToTemp(height_idx[i], temp_rect[idx]);
            temp_idx[idx++] = height_idx[i++];
        }
    }

    while (i <= mid1 && j <= mid2) {
        if (items[height_idx[j]]->rect().height() <
            items[height_idx[i]]->rect().height()) {
            moveItemToTemp(height_idx[j], temp_rect[idx]);
            temp_idx[idx++] = height_idx[j++];
        } else {
            moveItemToTemp(height_idx[i], temp_rect[idx]);
            temp_idx[idx++] = height_idx[i++];
        }
    }

    while (j <= mid2 && k <= right) {
        if (items[height_idx[k]]->rect().height() <
            items[height_idx[j]]->rect().height()) {
            moveItemToTemp(height_idx[k], temp_rect[idx]);
            temp_idx[idx++] = height_idx[k++];
        } else {
            moveItemToTemp(height_idx[j], temp_rect[idx]);
            temp_idx[idx++] = height_idx[j++];
        }
    }

    while (i <= mid1 && k <= right) {
        if (items[height_idx[k]]->rect().height() <
            items[height_idx[i]]->rect().height()) {
            moveItemToTemp(height_idx[k], temp_rect[idx]);
            temp_idx[idx++] = height_idx[k++];
        } else {
            moveItemToTemp(height_idx[i], temp_rect[idx]);
            temp_idx[idx++] = height_idx[i++];
        }
    }

    while (i <= mid1) {
        moveItemToTemp(height_idx[i], temp_rect[idx]);
        temp_idx[idx++] = height_idx[i++];
    }

    while (j <= mid2) {
        moveItemToTemp(height_idx[j], temp_rect[idx]);
        temp_idx[idx++] = height_idx[j++];
    }

    while (k <= right) {
        moveItemToTemp(height_idx[k], temp_rect[idx]);
        temp_idx[idx++] = height_idx[k++];
    }

    for (int i = left; i <= right; i++) {
        moveTempToItem(temp_idx[i]);
        height_idx[i] = temp_idx[i];
    }
}

void SortingThread::moveItemToTemp(int index, QRectF dest) {
    int step = 10;

    emit signal_changeColor(index, Qt::blue);

    // move down
    qreal dy = height / 2 / step;

    for (int i = 0; i < step; i++) {
        emit signal_moveBy(index, 0, dy);
        QThread::usleep(sortDelay);
    }

    // move level
    qreal dx = (dest.x() - items[index]->rect().x()) / step;

    if (dx != 0) {
        for (int i = 0; i < step; i++) {
            emit signal_moveBy(index, dx, 0);
            QThread::usleep(sortDelay);
        }
    }
}

void SortingThread::moveTempToItem(int index) {
    int step = 10;

    // move up
    qreal dy = height / 2 / step;

    for (int i = 0; i < step; i++) {
        emit signal_moveBy(index, 0, -dy);
        QThread::usleep(sortDelay);
    }

    emit signal_changeColor(index, Qt::red);
}
