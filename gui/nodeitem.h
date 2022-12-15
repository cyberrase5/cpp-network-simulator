#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsSimpleTextItem>
#include <QGraphicsScene>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>

#include "packetitem.h"

/**
 * @brief The NodeCaller class
 * An object for interacting between nodeitems and mainwindow.
 */
class NodeCaller : public QObject {
    Q_OBJECT // necessary for signals and slots

public:
    /**
     * @brief getId
     * A getter for the id of the nodeitem signalling to mainwindow.
     * @return id of the nodeitem.
     */
    unsigned short getId() { return id_; }

public slots:
    /**
     * @brief change_max_capacity
     * Signals mainwindow to change nodeitems max capacity.
     * @param id The id of the calling nodeitem.
     */
    void change_max_capacity(unsigned short id)
    {
        id_ = id;
        emit change_max_capacity_called();
    }

    /**
     * @brief call_print
     * Signals mainwindow to print out nodeitem's information.
     * @param id The id of the calling nodeitem.
     */
    void call_print(unsigned short id)
    {
        id_ = id;
        emit print_called();
    }

    /**
     * @brief call_print_routing_table
     * Signals mainwindow to call print_routing_table()
     * @param id The id of the calling router.
     */
    void call_print_routing_table(unsigned short id)
    {
        id_ = id;
        emit print_routing_table_called();
    }

    /**
     * @brief send_packet
     * Signals the mainwindow to call the send() method of class Node.
     * @param id The id of the calling node.
     */
    void send_packet(unsigned short id)
    {
        id_ = id;
        emit send_packet_called();
    }

signals:

    /**
     * @brief print_called
     * A request to call the print() method of the Node class was made.
     */
    void print_called();

    /**
     * @brief change_max_capacity_called
     * A request to call the change_max_capacity() method of the Node class was made.
     */
    void change_max_capacity_called();

    /**
     * @brief print_routing_table_called
     * A request was made to call the print_routing_table() method of the Router class.
     */
    void print_routing_table_called();

    /**
     * @brief send_packet_called
     * A request was made to call the send() method of the Node class.
     */
    void send_packet_called();

private:
    /**
     * @brief id_
     * The id of the calling node.
     */
    unsigned short id_;
};


/**
 * @brief The NodeItem class
 * A grapchical item which represents a node.
 */
class NodeItem : public QGraphicsPolygonItem
{
public:
    /**
     * @brief NodeItem
     * Constructor of nodeitem.
     *
     * @param id The id of the nodeitem.
     * @param name The name of the nodeitem.
     * @param x The x coordinate of the top left corner of the nodeitem.
     * @param y The y coordinate of the top left corner of the nodeitem.
     * @param caller A QObject for signalling between nodeitems and mainwindow.
     * @param parent An optional pointer to a parent QObject.
     */
    NodeItem(unsigned short id, std::string name, double x, double y, NodeCaller *caller, QGraphicsItem *parent = nullptr) : QGraphicsPolygonItem(parent),
        caller_(caller), name_(name), id_(id)
    {
        position_ = QPointF(0,0);
        offset_ = QPointF(x+12,y+12);
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        QBrush brush(Qt::white);
        color = brush;
        isDroppingPacket_ = false;
        ticks_ = 0;
    }

    /**
     * @brief itemChange
     * Describes how this nodeitem should react to a change in position.
     *
     * @param change The type of change which occurred.
     * @param value The value (magnitude) of the change.
     * @return Changed item.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        if (change == ItemPositionChange && scene()) {
            QPointF newPos = value.toPointF();
            position_ = newPos;
            updateConnections();
        }
        return QGraphicsItem::itemChange(change, value);
    }

    /**
     * @brief addConnection
     * Adds a new connection to static collections.
     *
     * @param connection The connection to be added.
     * @param line A pointer to a lineitem.
     */
    void addConnection(std::pair<NodeItem *, NodeItem *> connection, QGraphicsLineItem* line)
    {
        lines_.push_back(line);
        connections_.push_back(connection);
    }


    /**
     * @brief updateConnections
     * Updates the graphical lines between each connected nodeitem.
     *
     */
    void updateConnections()
    {
        int i = 0;
        for (auto connection : connections_)
        {
            lines_[i]->setLine(QLineF(connection.first->position(),
                                      connection.second->position()));
            i++;
        }
    }

    /**
     * @brief position
     * Returns the centre of this nodeitem.
     *
     * @return Returns the centre coordinate of this nodeitem.
     */
    QPointF position() { return position_ + offset_; }

    /**
     * @brief addName
     * Adds a graphical textitem to this nodeitem.
     *
     * @param name The desired name.
     */
    void addName(std::string name)
    {
        nameItem_ = new QGraphicsSimpleTextItem(QString::fromStdString(name), this);
        QPointF center = this->boundingRect().center();
        QPointF newPos(this->mapToScene(center).x() - (nameItem_->boundingRect().width() / 2),
                       this->mapToScene(center).y() - (nameItem_->boundingRect().height() / 2));
        nameItem_->setPos(newPos);
    }

    /**
     * @brief moveTextItem
     * Moves this nodeitems textitem by (x,y)
     *
     * @param x The change in x.
     * @param y The change in y.
     */
    void moveTextItem(qreal x, qreal y)
    {
        nameItem_->moveBy(x,y);
    }

    /**
     * @brief getNameItem
     * A getter for the graphical textitem underneath each nodeitem.
     *
     * @return A pointer to the textitem.
     */
    QGraphicsSimpleTextItem* getNameItem() { return nameItem_; }

    /**
     * @brief resetTicks
     * Reset ticks_. Used for calculating how long the animation ought to be.
     */
    void resetTicks() { ticks_ = 0; }

    /**
     * @brief setColor
     * Changes the color of this NodeItem.
     *
     * @param brush The wanted color.
     */
    void setColor(QBrush brush)
    {
        color = brush;
    }

    /**
     * @brief dropPacket
     * Setter for isDroppingPacket_. Makes this NodeItem change its color to black.
     */
    void dropPacket()
    {
        isDroppingPacket_ = true;
    }

    /**
     * @brief changeSpeed
     * Multiplies the animationSpeed_
     *
     * @param multiplier
     */
    void changeSpeed(double multiplier)
    {
        if (multiplier > 0) animationSpeed_ = multiplier;
    }

    /**
     * @brief setSpeed
     * Setter for animationSpeed_
     */
    void setSpeed()
    {
        animationSpeed_ = 1;
    }

    /**
     * @brief operator <<
     * Animates a packetitem between this and other nodeitems.
     *
     * @param other The other nodeitem.
     */
    void operator<<(NodeItem *other)
    {
        new PacketItem(this->position(), other->position(), this->scene());
    }


    /**
     * @brief contextMenuEvent
     * Opens a contextmenu and emits a signal to MainWindow if an action was chosen.
     *
     * @param event Right click pressed on nodeitem.
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
    {
        QMenu *menu = new QMenu();
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->clear();
        menu->addAction("Send a packet");
        menu->addAction("Change maximum capacity");
        menu->addAction("Print information");
        if (this->name_ == std::to_string(this->id_))
            menu->addAction("Print routing table");
        QAction *a = menu->exec(event->screenPos());
        if (a == nullptr) return;
        else if (a->text().toStdString() == "Send a packet") {
            caller_->send_packet(id_);
        }
        else if (a->text().toStdString() == "Change maximum capacity") {
            caller_->change_max_capacity(id_);
        }
        else if (a->text().toStdString() == "Print information") {
            caller_->call_print(id_);
        }
        else if (a->text().toStdString() == "Print routing table") {
            caller_->call_print_routing_table(id_);
        }
    }

    std::string getName() const { return name_; }
    unsigned short getID() const { return id_; }

protected:
    /**
     * @brief advance
     * Animates the nodeitem.
     * Describes what the nodeitem ought to do in each tick of the timer.
     * @param phase Describes whether the function was called before or after the change occurred.
     */
    void advance(int phase)
    {
        if (!phase) return;
        if (this->isDroppingPacket_) this->setBrush(Qt::black);
        else this->setBrush(color);

        ticks_ += animationSpeed_;
        if (ticks_ > 30) {
            isDroppingPacket_ = false;
            ticks_ = 0;
        }
    }


private:
    /**
     * @brief lines_
     * A static collection for holding the pointers of graphical lineitems.
     */
    inline static std::vector<QGraphicsLineItem *> lines_;

    /**
     * @brief connections_
     * A static collection for holding the connections of nodeitems.
     */
    inline static std::vector<std::pair<NodeItem *, NodeItem *>> connections_;

    /**
     * @brief animationSpeed_
     * A static variable for holding the animation speed of dropping packets.
     */
    inline static double animationSpeed_;

    /**
     * @brief isDroppingPacket_
     * A variable used for checking whether an animation is ongoing.
     */
    bool isDroppingPacket_;

    /**
     * @brief ticks_
     * A variable used for checking how long into an animation this nodeitem is.
     */
    double ticks_;


    /**
     * @brief id_
     * The id of this nodeitem.
     */
    unsigned short id_;

    /**
     * @brief name_
     * The name of this nodeitem.
     */
    std::string name_;

    /**
     * @brief color
     * The color of this nodeitem.
     */
    QBrush color;

    /**
     * @brief offset_
     * The offset to centre from the top left corner of this nodeitem.
     */
    QPointF offset_;

    /**
     * @brief position_
     * Holds the position of the top left corner of this nodeitem.
     */
    QPointF position_;

    /**
     * @brief nameItem_
     * Pointer to the graphical nameitem of this nodeitem.
     * (Technically unnecessary as it can be accessed from the children of this class)
     */
    QGraphicsSimpleTextItem* nameItem_;

    /**
     * @brief caller_
     * A pointer to a caller for signalling to mainwindow.
     */
    NodeCaller *caller_;
};

#endif // NODEITEM_H
