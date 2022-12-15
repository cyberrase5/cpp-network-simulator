#include "mainwindow.h"
#include "nodes.h"

#include <QApplication>

std::map<unsigned short, Node*> main_map; // all nodes in simulation <node_id, Node>
std::map<std::string, unsigned short> DNS;
std::queue<Node*> nodequeue;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(&main_map, &DNS);
    w.show();


    /*
    Client client1 = Client(1, 5000000, 2, std::make_pair(50000, 50), "myComputer");
    Client client2 = Client(8, 5000000, 3, std::make_pair(100000, 50), "otherComputer");
    Router router2 = Router(2, 2, 1, std::make_pair(50000, 50));
    Router router3 = Router(3, 5000000, 8, std::make_pair(100000, 50));
    Router router4 = Router(4, 5000000, 5, std::make_pair(10000000, 50));
    Router router6 = Router(6, 5000000, 7, std::make_pair(10000000, 50));
    Server server_instagram = Server(5, 10000000, 4, std::make_pair(10000000, 50), "Instagram", ".jpg", 300);
    Server server_netflix = Server(7, 10000000, 6, std::make_pair(10000000, 50), "Netflix", ".mp4", 200000);

    w.addClient("myComputer", 0, 0);
    w.addClient("otherComputer", 0, 100);
    w.addRouter("2", 135, -20);
    w.addRouter("3", 180, 70);
    w.addRouter("4", 100, 100);
    w.addRouter("6", 155, 180);
    w.addServer("Instagram", 300, 15);
    w.addServer("Netflix", 340, 200);

    DNS["Instagram"] = 4;
    DNS["Netflix"] = 6;

    main_map.insert(std::pair<unsigned short, Node*>(1, &client1));
    main_map.insert(std::pair<unsigned short, Node*>(8, &client2));
    main_map.insert(std::pair<unsigned short, Node*>(2, &router2));
    main_map.insert(std::pair<unsigned short, Node*>(3, &router3));
    main_map.insert(std::pair<unsigned short, Node*>(4, &router4));
    main_map.insert(std::pair<unsigned short, Node*>(5, &server_instagram));
    main_map.insert(std::pair<unsigned short, Node*>(6, &router6));
    main_map.insert(std::pair<unsigned short, Node*>(7, &server_netflix));

    addLink(&router2, 1200000, 50, &router3);
    addLink(&router2, 500000, 50, &router4);
    addLink(&router3, 1200000, 50, &router4);
    addLink(&router3, 500000, 50, &router6);
    addLink(&router4, 500000, 50, &router6);

    w.addLine(w.getNode("2"), w.getNode("3"));
    w.addLine(w.getNode("2"), w.getNode("4"));
    w.addLine(w.getNode("3"), w.getNode("4"));
    w.addLine(w.getNode("3"), w.getNode("6"));
    w.addLine(w.getNode("4"), w.getNode("6"));




    w.addLine(w.getNode("myComputer"), w.getNode("2"));
    w.addLine(w.getNode("otherComputer"), w.getNode("4"));

    w.addLine(w.getNode("Instagram"), w.getNode("3"));
    w.addLine(w.getNode("Netflix"), w.getNode("6"));


    readNodes(w);
    readLinks(w);
    create_routing_tables();
    create_lineitems(w);


    client1.create_packet(2, 4, "doge", 0);
    client1.create_packet(2, 4, "doge1", 0);
    client1.create_packet(2, 4, "doge2", 0);

    std::cout << client1;

    client1.send();
    client1.send();
    client1.send();

    std::cout << client1 << router2;
    */

    return a.exec();
}
