#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>

void scanNetwork(const std::string& interface) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Erro ao criar socket" << std::endl;
        return;
    }

    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
        std::cerr << "Erro ao obter IP da interface" << std::endl;
        close(sock);
        return;
    }

    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    std::string localIP = inet_ntoa(ipaddr->sin_addr);
    std::cout << "Escaneando rede na faixa de " << localIP.substr(0, localIP.rfind('.') + 1) << "0/24" << std::endl;

    for (int i = 1; i < 255; ++i) {
        std::string targetIP = localIP.substr(0, localIP.rfind('.') + 1) + std::to_string(i);
        std::string command = "ping -c 1 -W 1 " + targetIP + " > /dev/null 2>&1";
        if (system(command.c_str()) == 0) {
            std::cout << "Dispositivo encontrado: " << targetIP << std::endl;
        }
    }

    close(sock);
}

int main() {
    std::string interface = "eth0"; // Altere para "wlan0" se estiver no Wi-Fi
    scanNetwork(interface);
    return 0;
}
