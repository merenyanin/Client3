#include <iostream>
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

enum CommandOpcode {
    CLEAR_DISPLAY_OPCODE,
    DRAW_PIXEL_OPCODE,
    DRAW_LINE_OPCODE,
    DRAW_RECTANGLE_OPCODE,
    FILL_RECTANGLE_OPCODE,
    DRAW_ELLIPSE_OPCODE,
    FILL_ELLIPSE_OPCODE
};

// Функція для відправки байтового масиву на сервер
void sendCommand(SOCKET& clientSocket, sockaddr_in& serverAddr, const std::vector<uint8_t>& command) {
    sendto(clientSocket, (char*)command.data(), command.size(), 0,
        (sockaddr*)&serverAddr, sizeof(serverAddr));
}

int main() {
    // Ініціалізація WinSock
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cerr << "Error initializing WinSock" << std::endl;
        return -1;
    }

    // Налаштування сокета клієнта
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1111); // Порт сервера
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Локальний IP

    std::vector<std::vector<uint8_t>> commandList = {
        { CLEAR_DISPLAY_OPCODE, 0xFF, 0xFF },
        { DRAW_PIXEL_OPCODE, 0x00, 0x10, 0x00, 0x20, 0xAA, 0xBB },
        { DRAW_LINE_OPCODE, 0x00, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, 0x40, 0xCC, 0xDD },
        { DRAW_RECTANGLE_OPCODE, 0x00, 0x05, 0x00, 0x10, 0x00, 0x15, 0x00, 0x20, 0xEE, 0xFF },
        { FILL_RECTANGLE_OPCODE, 0x00, 0x05, 0x00, 0x10, 0x00, 0x15, 0x00, 0x20, 0x11, 0x22 },
        { DRAW_ELLIPSE_OPCODE, 0x00, 0x08, 0x00, 0x12, 0x00, 0x09, 0x00, 0x07, 0x33, 0x44 },
        { FILL_ELLIPSE_OPCODE, 0x00, 0x06, 0x00, 0x11, 0x00, 0x05, 0x00, 0x04, 0x55, 0x66 },
    };

    for (const auto& command : commandList) {
        sendCommand(clientSocket, serverAddr, command);
        std::cout << "Command sent to server." << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
