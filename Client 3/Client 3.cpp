#include "GraphicsLib.h"
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

class DisplayClient : public GraphicsLib {
public:
    DisplayClient(uint_least16_t w, uint_least16_t h) : GraphicsLib(w, h) {
        initSocket();
    }

    ~DisplayClient() {
        closesocket(clientSocket);
        WSACleanup();
    }

    void fillScreen(uint_least16_t color) override {
       
        std::vector<uint8_t> command = { CLEAR_DISPLAY_OPCODE };
        addColorToCommand(command, color);
        sendCommand(command);
    }

    void drawPixel(int_least16_t x0, int_least16_t y0, uint_least16_t color) override {
        std::vector<uint8_t> command = { DRAW_PIXEL_OPCODE };
        addCoordinatesToCommand(command, x0, y0);
        addColorToCommand(command, color);
        sendCommand(command);
       
    }

    void drawLine(int_least16_t x0, int_least16_t y0, int_least16_t x1, int_least16_t y1, uint_least16_t color) override {
        std::vector<uint8_t> command = { DRAW_LINE_OPCODE };
        addCoordinatesToCommand(command, x0, y0);
        addCoordinatesToCommand(command, x1, y1);
        addColorToCommand(command, color);
        sendCommand(command);
       
    }

    void drawRect(int_least16_t x0, int_least16_t y0, int_least16_t w, int_least16_t h, uint_least16_t color) override {
        std::vector<uint8_t> command = { DRAW_RECTANGLE_OPCODE };
        addCoordinatesToCommand(command, x0, y0);
        addCoordinatesToCommand(command, x0 + w, y0 + h);
        addColorToCommand(command, color);
        sendCommand(command);
    }

    void fillRect(int_least16_t x0, int_least16_t y0, int_least16_t w, int_least16_t h, uint_least16_t color) override {
        std::vector<uint8_t> command = { FILL_RECTANGLE_OPCODE };
        addCoordinatesToCommand(command, x0, y0);
        addCoordinatesToCommand(command, x0 + w, y0 + h);
        addColorToCommand(command, color);
        sendCommand(command);
    }

    void drawEllipse(int_least16_t x0, int_least16_t y0, int_least16_t r_x, int_least16_t r_y, uint_least16_t color) override {
        std::vector<uint8_t> command = { DRAW_ELLIPSE_OPCODE };
        addCoordinatesToCommand(command, x0, y0);
        addCoordinatesToCommand(command, r_x, r_y);
        addColorToCommand(command, color);
        sendCommand(command);
    }

    void fillEllipse(int_least16_t x0, int_least16_t y0, int_least16_t r_x, int_least16_t r_y, uint_least16_t color) override {
        std::vector<uint8_t> command = { FILL_ELLIPSE_OPCODE };
        addCoordinatesToCommand(command, x0, y0);
        addCoordinatesToCommand(command, r_x, r_y);
        addColorToCommand(command, color);
        sendCommand(command);
    }

private:
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    void initSocket() {
        WSAData wsaData;
        WORD DLLVersion = MAKEWORD(2, 2);
        if (WSAStartup(DLLVersion, &wsaData) != 0) {
            throw std::runtime_error("Error initializing WinSock");
        }
        clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (clientSocket == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Error creating socket");
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(1111); // Порт сервера
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Локальний IP
    }

    void sendCommand(const std::vector<uint8_t>& command) {
        sendto(clientSocket, (char*)command.data(), command.size(), 0,
            (sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    void addColorToCommand(std::vector<uint8_t>& command, uint16_t color) {
        command.push_back((color >> 8) & 0xFF);
        command.push_back(color & 0xFF);
    }

    void addCoordinatesToCommand(std::vector<uint8_t>& command, int16_t x, int16_t y) {
        command.push_back((x >> 8) & 0xFF);
        command.push_back(x & 0xFF);
        command.push_back((y >> 8) & 0xFF);
        command.push_back(y & 0xFF);
    }
};


int main() {
    try {
        DisplayClient display(800, 600);
        display.fillScreen(RGB(0, 0,0)); 
        display.drawPixel(100, 100, RGB(255, 0, 0)); 
        display.drawLine(100, 50, 150, 150, RGB(0, 255, 0)); 
        display.drawRect(200, 200, 200, 200, RGB(0, 0, 255)); 
        display.fillRect(200, 200, 200, 200, RGB(255, 255, 0)); 
        display.drawEllipse(400, 400, 30, 15, RGB(255, 0, 255)); 
        display.fillEllipse(400, 400, 30, 15, RGB(0, 255, 255)); 
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
