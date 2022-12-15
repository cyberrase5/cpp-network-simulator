#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QtCore>
#include <QtGui>
#include <QtMath>
#include <QInputDialog>
#include <QFileDialog>
#include <QRandomGenerator>
#include <QFileDialog>

#include <sstream>
#include <fstream>
#include <string>

void gui_call_create_packet(std::string client_name, std::string server_name, std::string content, 
                                                unsigned int flag) {

    for (auto node: main_map) {
        Client* c = dynamic_cast<Client*>(node.second);
        if (c) {
            if (c->getName().compare(client_name) == 0) {
                c->create_packet(server_name, content, flag);
                return;
            }
        }
    }

    std::cout << "No such client" << std::endl;

}



MainWindow::MainWindow(std::map<unsigned short, Node*> *map1, std::map<std::string, unsigned short> *map2, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , main_map(map1), DNS(map2)
{
    ui->setupUi(this);

    this->scene = new QGraphicsScene(this);
    QGraphicsView *view = new QGraphicsView();
    this->setCentralWidget(view);
    this->setWindowTitle("Network Simulator");

    view->setScene(this->scene);
    view->setBackgroundBrush(Qt::white);

    getNodeByName("")->setSpeed();
    ((PacketItem*)nullptr)->changeSpeed(1);

    caller_ = new NodeCaller();
    connect(caller_, SIGNAL(print_called()), this, SLOT(print_node()));
    connect(caller_, SIGNAL(change_max_capacity_called()), this, SLOT(change_node_max_capacity()));
    connect(caller_, SIGNAL(print_routing_table_called()), this, SLOT(print_routing_table()));
    connect(caller_, SIGNAL(send_packet_called()), this, SLOT(send_package()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), scene, SLOT(advance()));
    timer->start(25);
}

NodeItem* MainWindow::getNode(unsigned short id)
{
    auto node = this->nodeitems.find(id);
    if (node != nodeitems.end()) return node->second;
    else return nullptr;
}


NodeItem* MainWindow::getNodeByName(std::string name)
{
    for (auto node = this->nodeitems.begin(); node != nodeitems.end(); node++) {
        if (node->second->getName() == name) return node->second;
    }
    return nullptr;
}

void MainWindow::addRouter(unsigned short id, std::string name, float x, float y)
{
    QPolygonF Triangle;
    Triangle.append(QPointF(x,y));
    Triangle.append(QPointF(x-15,y+25));
    Triangle.append(QPointF(x+15,y+25));
    Triangle.append(QPointF(x,y));

    QBrush redBrush(Qt::red);
    QPen blackpen(Qt::black);

    auto router = new NodeItem(id, name, x-12, y+3, caller_);
    router->setPolygon(Triangle);
    router->setPen(blackpen);
    router->setColor(redBrush);
    router->setZValue(1);
    router->addName(name);
    router->moveTextItem(0, 18);

    if (this->nodeitems.find(id) != this->nodeitems.end()) this->nodeitems[id] = router;
    else this->nodeitems.insert({id, router});
    this->scene->addItem(router);
}

void MainWindow::addServer(unsigned short id, std::string name, float x, float y)
{
    QPolygonF polygon;
    polygon << QPointF(x+10,y+10) << QPointF(x-10,y-10) << QPointF(x-10,y+10) << QPointF(x+10,y-10);

    QBrush brush(Qt::blue);
    QPen pen(Qt::black);

    auto server = new NodeItem(id, name, x-12,y-12, caller_);
    server->setPolygon(polygon);
    server->setColor(brush);
    server->setPen(pen);
    server->setZValue(1);
    server->addName(name + ": " + std::to_string(id));
    server->moveTextItem(0,16);

    if (this->nodeitems.find(id) != this->nodeitems.end()) this->nodeitems[id] = server;
    else this->nodeitems.insert({id, server});
    this->scene->addItem(server);
}

void MainWindow::print_node()
{
    (*main_map)[caller_->getId()]->print();
}

void MainWindow::change_node_max_capacity()
{
    unsigned short id = caller_->getId();
    unsigned int capacity = QInputDialog::getInt(this, "Change max capacity", "Choose a capacity", 0, 0);
    (*main_map)[id]->change_max_capacity(capacity);
}

void MainWindow::print_routing_table()
{
    router_nodes[caller_->getId()]->print_routing_table();
}

void MainWindow::send_package()
{
    (*main_map)[caller_->getId()]->send();
}

void MainWindow::addClient(unsigned short id, std::string name, float x, float y)
{
    QPolygonF Square;
    Square.append(QPointF(x+10,y+10));
    Square.append(QPointF(x-10,y+10));
    Square.append(QPointF(x-10,y-10));
    Square.append(QPointF(x+10,y-10));

    QBrush brush(Qt::green);
    QPen pen(Qt::black);

    auto client = new NodeItem(id, name, x-12,y-12, caller_);
    client->setPolygon(Square);
    client->setColor(brush);
    client->setPen(pen);
    client->setZValue(1);
    client->addName(name + ": " + std::to_string(id));
    client->moveTextItem(0, 17);

    if (this->nodeitems.find(id) != this->nodeitems.end()) this->nodeitems[id] = client;
    else this->nodeitems.insert({id, client});
    this->scene->addItem(client);
}

void MainWindow::addLine(NodeItem *node1, NodeItem *node2)
{
    NodeItem * tmp = (NodeItem *)nullptr;
    tmp->addConnection(std::pair(node1, node2), scene->addLine(QLineF(node1->position(), node2->position())));
}


void MainWindow::sendPacket(NodeItem *node1, NodeItem *node2)
{
    *node1 << node2;
}

void MainWindow::dropPacket(NodeItem *node)
{
    node->resetTicks();
    node->dropPacket();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered()
{
    readNodes();
    readLinks();
    create_routing_tables();
    create_lineitems();
}


void MainWindow::on_actionClient_triggered()
{
    QString str = QInputDialog::getText(this, "Add a client", "Choose a distinct name");
    //if (nodes[str.toStdString()] || str.toStdString() == "") return;
    //addClient(str.toStdString(), 100,0);
}


void MainWindow::on_actionRouter_triggered()
{
    QString str = QInputDialog::getText(this, "Add a router", "Choose a distinct name");
    //if (nodes[str.toStdString()] || str.toStdString() == "") return;
    //addRouter(str.toStdString(),500,100);
}


void MainWindow::on_actionServer_triggered()
{
    QString str = QInputDialog::getText(this, "Add a server", "Choose a distinct name");
    //if (nodes[str.toStdString()] || str.toStdString() == "") return;
    //addServer(str.toStdString(), 300,100);
}


void MainWindow::on_actionAdd_a_connection_triggered()
{
    QString str1 = QInputDialog::getText(this, "Add a client", "Choose a node");
    QString str2 = QInputDialog::getText(this, "Add a client", "Choose another node");
    auto node1 = str1.toStdString();
    auto node2 = str2.toStdString();
    //if (nodes.find(node1) == nodes.end() || nodes.find(node2) == nodes.end()) return;
    //addLine(nodes[node1], nodes[node2]);
}


void MainWindow::addLink(Router* first, unsigned int bandwidth, unsigned short latency, Router* second) {
    first->addNeighbor(second->getId(), std::make_pair(bandwidth, latency));
    second->addNeighbor(first->getId(), std::make_pair(bandwidth, latency));

}

bool MainWindow::readLinks() { //if reading at some point fails returns false
    std::string filename = QFileDialog::getOpenFileName(this, "Load linkfile").toStdString();
    std::ifstream file(filename);
    if (file.is_open()) { //check that opening file was succesful
        while(!file.eof()) {
            std::string line;
            if(!std::getline(file, line)) { return false; } //Readline and check that successful
            std::stringstream ss(line);
            std::string node, speed, delay, neighbour;
            if(!std::getline(ss, node, ';')) { return false; }
            if(!std::getline(ss, speed, ';')) { return false; }
            if(!std::getline(ss, delay, ';')) { return false; }
            if(!std::getline(ss, neighbour, '\n')) { return false; }
            unsigned short node_id = std::stoul(node);
            unsigned short neighbour_id = std::stoul(neighbour);
            if(main_map->count(node_id) > 0) { //if key exists
                Router* first = dynamic_cast<Router*>(main_map->find(node_id)->second);
                Router* second = dynamic_cast<Router*>(main_map->find(neighbour_id)->second);
                if(first == nullptr || second == nullptr) {
                    std::cout << "Reading fail/Router not found" << node_id << std::endl;
                    return false;
                } else {
                    addLink(first, std::stoul(speed), std::stoul(delay), second);
                    this->addLine(this->getNode(first->getId()), this->getNode(second->getId()));
                }
            }
        }
        file.close();
        std::cout << "Links read successfully" << std::endl;
        return true;
    } else {
        std::cout << "Failed opening links file" << std::endl;
        return false;
    }
}

std::pair<float, float> MainWindow::nextPos()
{
    float x = QRandomGenerator::global()->bounded(1000.0);
    float y = QRandomGenerator::global()->bounded(1000.0);
    return std::pair(x,y);
}


bool MainWindow::readNodes() { //if reading at some point fails returns false
    std::string filename = QFileDialog::getOpenFileName(this, "Load nodefile").toStdString();
    std::ifstream file(filename);
    if (file.is_open()) { //check that opening file was succesful
        while(!file.eof()) {
            std::string line;
            if(!std::getline(file, line)) { return false; } //Readline and check that successful
            std::stringstream ss(line);
            std::string type, id, max_capacity, name, connection_id, bw, latency, c_type, c_size;
            if(!std::getline(ss, type, ';')) { return false; } //parse parts of the line that was read, if not succesful -> exit parsing
            if(!std::getline(ss, id, ';')) { return false; }
            if(!std::getline(ss, max_capacity, ';')) { return false; }
            if(!std::getline(ss, connection_id, ';')) { return false; }
            if(!std::getline(ss, bw, ';')) { return false; }

            if(type == "2") {  // 0 client, 1 router, 2 server
                if(!std::getline(ss, latency, ';')) { return false; }
                if(!std::getline(ss, name, ';')) { return false; }
                if(!std::getline(ss, c_type, ';')) { return false; }
                if(!std::getline(ss, c_size, '\n')) { return false; }
                unsigned short id_ = std::stoul(id);
                unsigned int max_capacity_ = std::stoi(max_capacity);
                unsigned short connection_id_ = std::stoul(connection_id);
                unsigned int c_size_ = std::stoi(c_size);
                this->addServer(id_, name, nextPos().first, nextPos().second);
                main_map->insert(std::pair<unsigned short, Node*>(id_, new Server(id_, max_capacity_, connection_id_,
                    std::pair<unsigned int, unsigned short>(std::stoi(bw), std::stoi(latency)),
                    name, c_type, c_size_, getNode(id_)
                )));

            DNS->insert(std::make_pair(name, std::stoul(connection_id)));
            }
            else if(type == "1") {
                if(!std::getline(ss, latency, '\n')) { return false; }
                unsigned short id_ = std::stoul(id);
                unsigned int max_capacity_ = std::stoi(max_capacity);
                unsigned short connection_id_ = std::stoul(connection_id);
                this->addRouter(id_, std::to_string(id_), nextPos().first, nextPos().second);
                auto router = new Router(id_, max_capacity_, connection_id_,
                                         std::pair<unsigned int, unsigned short>(std::stoi(bw), std::stoi(latency)), getNode(id_)
                                     );
                router_nodes.insert(std::pair<unsigned short, Router*>(id_, router));
                main_map->insert(std::pair<unsigned short, Node*>(id_, router));

            }
            else if(type == "0") {
                if(!std::getline(ss, latency, ';')) { return false; }
                if(!std::getline(ss, name, '\n')) { return false; }
                unsigned short id_ = std::stoul(id);
                unsigned int max_capacity_ = std::stoi(max_capacity);
                unsigned short connection_id_ = std::stoul(connection_id);
                this->addClient(id_, name, nextPos().first, nextPos().second);
                auto client = new Client(id_, max_capacity_, connection_id_,
                                         std::pair<unsigned int, unsigned short>(std::stoi(bw), std::stoi(latency)),
                                         name, getNode(id_));
                client_nodes.insert(std::pair<unsigned short, Client*>(id_, client));
                main_map->insert(std::pair<unsigned short, Node*>(id_, client));
            }

        }
        file.close();
        std::cout << "Nodes read successfully" << std::endl;
        return true;
    } else {
        std::cout << "Failed opening nodes file" << std::endl;
        return false;
    }
}


void MainWindow::create_routing_tables() {

    // routing table creation
    std::map<unsigned short, Router*> routers;
    for (auto node: *main_map) {
        Router* it = dynamic_cast<Router*>(node.second);
        if (it) {
            routers.emplace(std::pair<unsigned short, Router*>(it->getId(), it));
        }
    }

    for (auto router: routers) {
        router.second->create_routing_table(routers);
    }
}

void MainWindow::create_lineitems()
{
    for (auto node : *main_map) {
        this->addLine(this->getNode(node.second->getId()),
                  this->getNode((*main_map)[node.second->getConnectionId()]->getId()));
    }
}

void MainWindow::on_actionCreate_a_packet_triggered()
{
    QString str1 = QInputDialog::getText(this, "Create a packet", "Choose a client");
    auto node1 = str1.toStdString();
    QString str2 = QInputDialog::getText(this, "Create a packet", "Choose a server (eg. Youtube, Instagram)");
    auto node2 = str2.toStdString();
    QString str3 = QInputDialog::getText(this, "Create a packet", "Requested content");
    auto node3 = str3.toStdString();
    QString str4 = QInputDialog::getText(this, "Create a packet", "Flag, 0 = normal\n1 = debug packet\n2 = cheat packet\n3 = both");
    std::string node4 = str4.toStdString();
    int x = stoul(node4);

    gui_call_create_packet(node1, node2, node3, x);

}

void MainWindow::on_actionSend_packets_triggered() // THIS
{
    QString str1 = QInputDialog::getText(this, "Send a packet from queue", "Node id to send from");
    auto node1 = str1.toStdString();
    int x = stoul(node1);

    (*main_map)[x]->send();
}



void MainWindow::sendPackages()
{
    for (auto pair : *main_map) {
        pair.second->send();
    }
}


void MainWindow::on_actionChange_animation_speed_triggered()
{
    double multiplier = QInputDialog::getDouble(this, "Change animation speed", "Choose a multiplier", 0, 0, 10, 2);
    ((NodeItem*)nullptr)->changeSpeed(multiplier);
    ((PacketItem*)nullptr)->changeSpeed(multiplier);
}

