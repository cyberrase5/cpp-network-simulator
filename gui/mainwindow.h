#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QGraphicsView>
#include <QTimer>


#include <map>

#include "nodeitem.h"
#include "nodes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


/**
 * @brief The MainWindow class
 * The main window of the program.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT // necessary for slots and signals

public:
    /**
     * @brief MainWindow
     * Consructor for the MainWindow class
     *
     * @param map1 A map with id keys and Node values
     * @param map2 A DNS map
     * @param parent An optional pointer to a parent QObject
     */
    MainWindow(std::map<unsigned short, Node*> *map1, std::map<std::string, unsigned short> *map2, QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief addServer
     * Creates a graphical server nodeitem and adds it to the scene.
     *
     * @param id The id of the server node the created nodeitem corresponds to.
     * @param name The name of the server
     * @param x The x coordinate of the created nodeitem.
     * @param y The y coordinate of the created nodeitem.
     */
    void addServer(unsigned short id, std::string name, float x, float y);

    /**
     * @brief addRouter
     * Creates a graphical router noditem and adds it to the scene.
     *
     * @param id The id of the router node the created nodeitem corresponds to.
     * @param name The name of the router
     * @param x The x coordinate of the created nodeitem.
     * @param y The y coordinate of the created nodeitem.
     */
    void addRouter(unsigned short id, std::string name, float x, float y);

    /**
     * @brief addClient
     * Creates a graphical client nodeitem and adds it to the scene.
     *
     * @param id The id of the client node the created nodeitem corresponds to.
     * @param name The name of the client.
     * @param x The x coordinate of the created nodeitem.
     * @param y The y coordinate of the created nodeitem.
     */
    void addClient(unsigned short id, std::string name, float x, float y);

    /**
     * @brief addLine
     * Adds a graphical line between two nodeitems.
     *
     * @param node1 The first nodeitem to be connected.
     * @param node2 The second nodeitem to be connected.
     */
    void addLine(NodeItem *node1, NodeItem *node2);

    /**
     * @brief sendPacket
     * Animates a moving packet between two nodeitems
     *
     * @param node1 The node the packet leaves from.
     * @param node2 The node the packet approaches.
     */
    void sendPacket(NodeItem *node1, NodeItem *node2);

    /**
     * @brief dropPacket
     * Animates a dropped packet by changing the color of the node to black.
     *
     * @param node The node which drops a packet.
     */
    void dropPacket(NodeItem *node);

    /**
     * @brief getNode
     * Getter for the graphical nodeitem.
     *
     * @param id The id of the nodeitem.
     * @return A pointer to the nodeitem.
     */
    NodeItem* getNode(unsigned short id);

    /**
     * @brief getNodeByName
     * Getter for the graphical nodeitem.
     *
     * @param name The name of the node.
     * @return A pointer to the nodeitem.
     */
    NodeItem* getNodeByName(std::string name);


private slots:

    /**
     * @brief on_actionLoad_triggered
     * Loads a simulation from a nodefile and connection file. Constructs both the underlying
     * simulation and their corresponding graphical elements.
     */
    void on_actionLoad_triggered();


    void on_actionClient_triggered();
    void on_actionRouter_triggered();
    void on_actionServer_triggered();
    void on_actionAdd_a_connection_triggered();

    /**
     * @brief on_actionCreate_a_packet_triggered
     * Creates a new packet on a node which can be found by checking the id of NodeCaller caller_
     */
    void on_actionCreate_a_packet_triggered();

    /**
     * @brief on_actionSend_packets_triggered
     * Calls send() on all nodes.
     */
    void on_actionSend_packets_triggered();

    /**
     * @brief on_actionSend_packets_triggered
     * Calls send() on all nodes.
     */
    void sendPackages();

    /**
     * @brief send_package
     * Calls send() on a specific node which is found by checking the id of NodeCaller caller_
     */
    void send_package();

    /**
     * @brief change_node_max_capacity
     * Changes the maximm capacity a node can hold. The node is found by checking the id of NodeCaller caller_
     */
    void change_node_max_capacity();

    /**
     * @brief print_node
     * Calls node's print() method. The node is found by checking the id of NodeCaller caller_
     */
    void print_node();

    /**
     * @brief print_routing_table
     * Calls router's print_routing_table() The node is found by checking the id of NodeCaller caller_
     */
    void print_routing_table();

    /**
     * @brief on_actionChange_animation_speed_triggered
     * Ask for a multiplier and change animation speed of packetitems and nodeitems.
     */
    void on_actionChange_animation_speed_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    /**
     * @brief nodeitems
     * Map of graphical nodes.
     */
    std::map<unsigned short, NodeItem *> nodeitems;

    /**
     * @brief timer
     * A timer for handling animations.
     */
    QTimer *timer;

    /**
     * @brief caller_
     * A QObject that connects NodeItems and MainWindow through slots and signals.
     */
    NodeCaller *caller_;

    /**
     * @brief client_nodes
     * Used for accessing Client class specific methods. Probably can be avoided by typecasting nodes in the main map.
     */
    std::map<unsigned short, Client*> client_nodes;
    /**
     * @brief router_nodes
     * Used for accessing Router class specific methods. Probably can be avoided by typecasting nodes in the main map.
     */
    std::map<unsigned short, Router*> router_nodes;

    std::map<unsigned short, Node*> *main_map; // all nodes in simulation <node_id, Node>
    std::map<std::string, unsigned short> *DNS;


    /**
     * @brief addLink
     * Adds a simulation link between two routers.
     *
     * @param first The first router.
     * @param bandwidth The bandwidth of the connection.
     * @param latency The latency of the connection.
     * @param second The second router.
     */
    void addLink(Router* first, unsigned int bandwidth, unsigned short latency, Router* second);

    /**
     * @brief readLinks
     * Reads and constructs router links from a file.
     * @return Whether the read was successful.
     */
    bool readLinks();

    /**
     * @brief nextPos
     * Gets a random position on the scene.
     * @return A random position on the scene as a pair of floats (x,y)
     */
    std::pair<float, float> nextPos();

    /**
     * @brief readNodes
     * Reads and constructs nodes and graphical nodeitems from a file.
     * @return Whether the read was successful or not.
     */
    bool readNodes();

    /**
     * @brief create_routing_tables
     * Stores the connections each router has with other routers in each router.
     */
    void create_routing_tables();

    /**
     * @brief create_lineitems
     * Creates graphical lines between each connected node.
     */
    void create_lineitems();
};



#endif // MAINWINDOW_H
