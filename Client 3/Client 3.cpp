#include "GraphicsLib.h"
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <cstdint>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

enum CommandOpcode {
    CLEAR_DISPLAY_OPCODE,
    DRAW_PIXEL_OPCODE,
    DRAW_LINE_OPCODE,
    DRAW_RECTANGLE_OPCODE,
    FILL_RECTANGLE_OPCODE,
    DRAW_ELLIPSE_OPCODE,
    FILL_ELLIPSE_OPCODE,
    DRAW_TEXT_OPCODE,
    SET_ORIENTATION_OPCODE, // для встановлення орієнтації
    GET_WIDTH_OPCODE,       // для запиту ширини
    GET_HEIGHT_OPCODE
};



class DisplayClient : public GraphicsLib {
public:
    DisplayClient(uint_least16_t w, uint_least16_t h, const std::string& ipAddress, uint16_t port)
        : GraphicsLib(w, h), ipAddress(ipAddress), port(port) {
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
    std::vector<uint8_t> receiveResponse(size_t expectedSize) {
    std::vector<uint8_t> buffer(expectedSize);
    int receivedBytes = recv(clientSocket, reinterpret_cast<char*>(buffer.data()), expectedSize, 0);
    if (receivedBytes != expectedSize) {
        throw std::runtime_error("Failed to receive complete response from server");
    }
    return buffer;
}

    void setOrientation(int_least16_t orientation) {
        if (orientation != 0 && orientation != 90 && orientation != 180 && orientation != 270) {
            throw std::invalid_argument("Invalid orientation value");
        }

        std::vector<uint8_t> command = { SET_ORIENTATION_OPCODE };
        command.push_back((orientation >> 8) & 0xFF);
        command.push_back(orientation & 0xFF);

        sendCommand(command);
    }

    uint16_t getWidth() {
        std::vector<uint8_t> command = { GET_WIDTH_OPCODE };
        sendCommand(command);

        std::vector<uint8_t> response = receiveResponse(3); 
        if (response[0] != GET_WIDTH_OPCODE) {
            throw std::runtime_error("Invalid response opcode for GET_WIDTH");
        }
        return (response[1] << 8) | response[2];
    }

    uint16_t getHeight() {
        std::vector<uint8_t> command = { GET_HEIGHT_OPCODE };
        sendCommand(command);

        std::vector<uint8_t> response = receiveResponse(3); 
        if (response[0] != GET_HEIGHT_OPCODE) {
            throw std::runtime_error("Invalid response opcode for GET_HEIGHT");
        }
        return (response[1] << 8) | response[2];
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
    void drawText(int16_t x, int16_t y, uint16_t color, const std::string& text, int_least16_t orientation = 0, int_least16_t spacing = 19) {
        const int16_t centerX = x;
        const int16_t centerY = y;

       
        const double radians = orientation * 3.14 / 180.0;

        int16_t currentX = x;
        int16_t currentY = y;

        for (char c : text) {
      
            int16_t rotatedX = centerX + (currentX - centerX) * cos(radians) - (currentY - centerY) * sin(radians);
            int16_t rotatedY = centerY + (currentX - centerX) * sin(radians) + (currentY - centerY) * cos(radians);

           
            std::vector<uint8_t> command = { DRAW_TEXT_OPCODE };

            command.push_back(rotatedX >> 8);
            command.push_back(rotatedX & 0xFF);

            command.push_back(rotatedY >> 8);
            command.push_back(rotatedY & 0xFF);

            command.push_back(color >> 8);
            command.push_back(color & 0xFF);

            command.push_back(c); 

            sendCommand(command);

           
            currentX += spacing; 
        }
    }

    void animateCircle(GraphicsLib& display) {
        const int radius = 20;
        int width = display.getWidth();
        int height = display.getHeight();

        for (int i = 0; i < width; ++i) {
          
            display.fillScreen(toRGB565(255, 255, 255));

           
            display.fillEllipse(i, height / 2, radius, radius, toRGB565(255, 0, 0));

            Sleep(20);
        }
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
    std::string ipAddress;
    uint16_t port;

    void initSocket() {
        WSAData wsaData;
        WORD DLLVersion = MAKEWORD(2, 2);
        if (WSAStartup(DLLVersion, &wsaData) != 0) {
            throw std::runtime_error("Помилка ініціалізації WinSock");
        }
        clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (clientSocket == INVALID_SOCKET) {
            WSACleanup();
            throw std::runtime_error("Помилка створення сокета");
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
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



void displayTrafficLight(GraphicsLib& display) {
    const int_least16_t centerX = display.getWidth() / 2;
    const int_least16_t centerY = display.getHeight() / 2;
    const int_least16_t radius = 50;
    const int_least16_t spacing = 100;


    const int_least16_t boxWidth = radius * 2;
    const int_least16_t boxHeight = spacing * 3;
    const int_least16_t boxX = centerX - boxWidth / 2;
    const int_least16_t boxY = centerY - spacing - radius;

    display.fillRect(boxX, boxY, boxWidth, boxHeight, toRGB565(123, 50, 50));


    const int_least16_t poleWidth = 20;
    const int_least16_t poleHeight = 200;
    const int_least16_t poleX = centerX - poleWidth / 2;
    const int_least16_t poleY = boxY + boxHeight;

    display.fillRect(poleX, poleY, poleWidth, poleHeight, toRGB565(10, 0, 10));

    while (true) {

        display.fillEllipse(centerX, centerY - spacing, radius, radius, toRGB565(255, 0, 0));
        display.fillEllipse(centerX, centerY, radius, radius, toRGB565(0, 0, 0));
        display.fillEllipse(centerX, centerY + spacing, radius, radius, toRGB565(0, 0, 0));
        Sleep(2000);


        display.fillEllipse(centerX, centerY - spacing, radius, radius, toRGB565(0, 0, 0));
        display.fillEllipse(centerX, centerY, radius, radius, toRGB565(255, 255, 0));
        display.fillEllipse(centerX, centerY + spacing, radius, radius, toRGB565(0, 0, 0));
        Sleep(1000);


        display.fillEllipse(centerX, centerY - spacing, radius, radius, toRGB565(0, 0, 0));
        display.fillEllipse(centerX, centerY, radius, radius, toRGB565(0, 0, 0));
        display.fillEllipse(centerX, centerY + spacing, radius, radius, toRGB565(0, 255, 0));
        Sleep(2000);
    }



}




int main() {
    try {
        DisplayClient display(800, 600, "127.0.0.1", 1111);
        display.fillScreen(toRGB565(255, 255, 255));
        display.drawText(300, 200, toRGB565(255, 0, 0), "HELLO WORLD", 270, 30); 
        display.drawText(300, 200, toRGB565(255, 0, 0), "HELLO WORLD", 90, 30);
        display.drawText(300, 200, toRGB565(255, 0, 0), "HELLO WORLD", 180, 30);
        display.drawText(300, 200, toRGB565(255, 0, 0), "HELLO WORLD", 0, 30);
      /*  display.animateCircle(display);*/
    
      

    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}