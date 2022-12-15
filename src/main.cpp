#ifndef MAIN_CPP
#define MAIN_CPP

#include "nodes.hpp"

#include <sstream>
#include <fstream>

std::map<unsigned short, Node*> main_map; // all nodes in simulation <node_id, Node>
std::map<std::string, unsigned short> DNS;

void addLink(Router* first, unsigned int bandwidth, unsigned short latency, Router* second) {
    first->addNeighbor(second->getId(), std::make_pair(bandwidth, latency));
    second->addNeighbor(first->getId(), std::make_pair(bandwidth, latency));

}

void gui_call_create_packet(std::string client_name, std::string server_name, std::string content, 
                                                unsigned int flag) {

    for (auto node: main_map) {
        Client* c = dynamic_cast<Client*>(node.second);
        if (c) {
            if (c->getName().compare(client_name) == 0) {
                c->create_packet(server_name, content, flag);
            }
        }
    }

}

bool readLinks() { //if reading at some point fails returns false
    std::string filename = "/home/rassalm/Documents/Koulu/cpp-project-network-simulator/src/testlinks.txt";
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
            if(main_map.count(node_id) > 0) { //if key exists
                Router* first = dynamic_cast<Router*>(main_map.find(node_id)->second);
                Router* second = dynamic_cast<Router*>(main_map.find(neighbour_id)->second);
                if(first == nullptr || second == nullptr) {
                    std::cout << "Reading fail/Router not found" << node_id << std::endl;
                    return false;
                } else {
                    addLink(first, std::stoul(speed), std::stoul(delay), second);
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


bool readNodes() { //if reading at some point fails returns false
    std::string filename = "/home/rassalm/Documents/Koulu/cpp-project-network-simulator/src/testnodes.txt";
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
                main_map.insert(std::pair<unsigned short, Node*>(id_, new Server(id_, max_capacity_, connection_id_,
                    std::pair<unsigned int, unsigned short>(std::stoi(bw), std::stoi(latency)),
                    name, c_type, c_size_
                )));
            DNS.insert(std::make_pair(name, std::stoul(connection_id)));
            }
            else if(type == "1") { 
                if(!std::getline(ss, latency, '\n')) { return false; }
                unsigned short id_ = std::stoul(id);
                unsigned int max_capacity_ = std::stoi(max_capacity);
                unsigned short connection_id_ = std::stoul(connection_id);
                main_map.insert(std::pair<unsigned short, Node*>(id_, new Router(id_, max_capacity_, connection_id_,
                    std::pair<unsigned int, unsigned short>(std::stoi(bw), std::stoi(latency))
                ))); 
            }
            else if(type == "0") {
                if(!std::getline(ss, latency, ';')) { return false; }
                if(!std::getline(ss, name, '\n')) { return false; }
                unsigned short id_ = std::stoul(id);
                unsigned int max_capacity_ = std::stoi(max_capacity);
                unsigned short connection_id_ = std::stoul(connection_id);
                main_map.insert(std::pair<unsigned short, Node*>(id_, new Client(id_, max_capacity_, connection_id_,
                    std::pair<unsigned int, unsigned short>(std::stoi(bw), std::stoi(latency)),
                    name
                ))); 
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

void create_routing_tables() {

    // routing table creation
    std::map<unsigned short, Router*> routers;
    for (auto node: main_map) {
        Router* it = dynamic_cast<Router*>(node.second);
        if (it) {
            routers.emplace(std::pair<unsigned short, Router*>(it->getId(), it));
        }
    }

    for (auto router: routers) {
        router.second->create_routing_table(routers);
    }
}


int main() {
/*
    Client client1 = Client(1, 5000000, 2, std::make_pair(50000, 50), "myComputer");
    Client client2 = Client(8, 5000000, 3, std::make_pair(100000, 50), "otherComputer");
    Router router2 = Router(2, 2, 1, std::make_pair(50000, 50));
    Router router3 = Router(3, 5000000, 8, std::make_pair(100000, 50));
    Router router4 = Router(4, 5000000, 5, std::make_pair(10000000, 50));
    Router router6 = Router(6, 5000000, 7, std::make_pair(10000000, 50));
    Server server_instagram = Server(5, 10000000, 4, std::make_pair(10000000, 50), "Instagram", ".jpg", 300);
    Server server_netflix = Server(7, 10000000, 6, std::make_pair(10000000, 50), "Netflix", ".mp4", 200000);

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
*/
    readNodes();
    readLinks();
    create_routing_tables();

    for (auto node: main_map) {
        node.second->send();
    }

    // lopuksi

    return 0;
}

#endif
