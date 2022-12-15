#ifndef PACKETITEM_H
#define PACKETITEM_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QtMath>

/**
 * @brief The PacketItem class
 * Is the graphical item which moves in the animation of packages.
 */
class PacketItem : public QGraphicsPolygonItem
{

public:
    /**
     * @brief PacketItem
     * Constructor of the animation item.
     *
     * @param start The start position of the packetitem.
     * @param end The end position of the packetitem.
     * @param scene The scene in which the packetitem appears in.
     */
    PacketItem(QPointF start, QPointF end, QGraphicsScene *scene) : QGraphicsPolygonItem()
    {
        qreal x = 0, y = 0;

        QPolygonF Triangle;
        Triangle.append(QPointF(x,y));
        Triangle.append(QPointF(x-10,y+13));
        Triangle.append(QPointF(x+10,y+13));
        Triangle.append(QPointF(x,y));

        QBrush redBrush(Qt::yellow);
        QPen blackpen(Qt::black);

        this->setPolygon(Triangle);
        this->setPen(blackpen);
        this->setBrush(redBrush);
        this->setZValue(1);

        scene->addItem(this);

        QPointF vector = end - start;
        qreal dot = QPointF::dotProduct(vector, QPointF(0, vector.ry()));
        qreal mvector = qSqrt(qPow(vector.rx(), 2) + qPow(vector.ry(), 2));

        qreal angle;
        if (vector.rx() >= 0)
            angle = qRadiansToDegrees(qAcos(-(dot/(mvector*vector.ry()))));
        else
            angle = qRadiansToDegrees(qAcos((dot/(mvector*vector.ry())))) - 180;

        this->setRotation(angle);
        this->setSpeed(mvector);
        this->setPos(start + (vector/3));
    }

    /**
     * @brief resetTicks
     * Resets the animation counter to 0.
     */
    void resetTicks() { ticks = 0; }

    /**
     * @brief setSpeed
     * Sets the speed of the packetitem. Depends on the distance that needs to be covered.
     * @param magnitude The distance moved.
     */
    void setSpeed(qreal magnitude)
    {
        speed = animationSpeed * magnitude / 75;
    }

    /**
     * @brief changeSpeed
     * Changes the animation speed of the packetitem.
     * @param multiplier
     */
    void changeSpeed(double multiplier)
    {
        if (multiplier > 0) animationSpeed = multiplier;
    }

protected:
    /**
     * @brief advance
     * Describes how the packetitem should act in each tick of the animation.
     * @param phase Describes whether the change has happened or not.
     */
    void advance(int phase)
    {
        if (!phase) return;
        this->setPos(mapToParent(0,-(speed)));
        ticks += animationSpeed;
        if (ticks > 30) {
            this->scene()->removeItem(this);
            delete this;
        }
    }

private:
    /**
     * @brief angle
     * The angle this packetitem points towards.
     */
    qreal angle;

    /**
     * @brief speed
     * The speed of the packetitem.
     */
    qreal speed;

    /**
     * @brief ticks
     * Describes for how many ticks the packetitem should appear on the scene.
     */
    double ticks;

    /**
     * @brief animationSpeed
     * A static variable that describes how fast the animation should be.
     */
    inline static double animationSpeed;
};

#endif // PACKETITEM_H
