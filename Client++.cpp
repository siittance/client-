#define WIN32_LEAN_AND_MEAN //макрос

#define WIN32_LEAN_AND_MEAN

// Подключаем необходимые библиотеки
#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    // Инициализируем Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Устанавливаем параметры для поиска сетевых адресов
    ADDRINFO hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Семейство адресов IPv4
    hints.ai_socktype = SOCK_STREAM; // Тип сокета TCP
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Получаем список сетевых адресов для указанного хоста и порта
    ADDRINFO* addrResult;
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создаем сокет для подключения к серверу
    SOCKET ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключаемся к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Буфер для отправки данных
    const char* sendBuffer1 = "Hello from client 1";
    // Отправляем первое сообщение
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    // Выводим количество отправленных байт
    cout << "Sent: " << result << " bytes" << endl;

    // Буфер для отправки данных
    const char* sendBuffer2 = "Ioy from client 2";
    // Отправляем второе сообщение
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    // Выводим количество отправленных байт
    cout << "Sent: " << result << " bytes" << endl;

    // Завершаем отправку данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        // Выводим сообщение об ошибке и завершаем программу
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Буфер для приема данных
    char recvBuffer[512];

    // Цикл приема и вывода сообщений от сервера
    do {
        // Очищаем буфер приема
        ZeroMemory(recvBuffer, 512);

        // Принимаем данные от сервера
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            // Выводим количество принятых байт и принятые данные
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            // Соединение закрыто
            cout << "Connection closed" << endl;
        }
        else {
            // Ошибка при приеме данных
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Закрываем сокет
    closesocket(ConnectSocket);

    // Освобождаем память, выделенную для списка сетевых адресов
    freeaddrinfo(addrResult);

    // Очищаем ресурсы Winsock
    WSACleanup();

    // Возвращаем код завершения программы
    return 0;
}
