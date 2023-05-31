#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {

    //Созздание клиентаского сокета, связывание серверного сокета с клиентским сокетом
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating socket\n";
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported\n";
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        cout << "Connection Failed\n";
        return 1;
    }





    char buffer[1024] = {0};
    string input;
	
    while (true) {
        input.~string();
        getline(cin, input);
        if(input.substr(0, 4) == "ECHO"){
        input = input.substr(5);
        cout << "Отправляемый эхо-запрос: " << input  << endl;
        }

        if (input == "QUIT") {
            send(sockfd, input.c_str(), strlen(input.c_str()), 0);
            break;
        }

        if (input[0] == '@') {
            FILE *fp = fopen(input.substr(1).c_str(), "r");
            char c;

            while ((c = fgetc(fp)) != EOF) {
                send(sockfd, &c, 1, 0);
            }

            fclose(fp);
            continue;
        }

        send(sockfd, input.c_str(), strlen(input.c_str()), 0);

        int valread = read(sockfd, buffer, 1024);
        cout << buffer << endl;
    }

    close(sockfd);

    return 0;
} 

