#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

string scan_file_system(path& t){
string after_scan;
for(auto& p: recursive_directory_iterator(t))
    {
        cout << p.path().string() << endl;
	after_scan += p.path().string();
    }
    return after_scan;
}

string handle_info_request(int client_socket) {
    string response = "Server info:\n";
    response += "IP address: 127.0.0.1\n";
    response += "Port: 8080\n";
    return response;
}

int main(int argc, char *argv[]) {
    string response;
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating socket\n";
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        cout << "Error binding socket\n";
        return 1;
    }
    cout << "Создание сокета на основе клиентского успешно!" << endl;
    if (listen(listenfd, 10) < 0) {
        cout << "Error listening on socket\n";
        return 1;
    }
    cout << "Подключение произошло успешно!" << endl;

    while (true) {
        socklen_t clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

        if (connfd < 0) {
            cout << "Error accepting connection\n";
            continue;
        }

        char buffer[1024] = {0};
        int valread = read(connfd, buffer, 1024);
        string input(buffer);

        if (input == "QUIT") {
            close(connfd);
            break;
        }

        // Обработка запроса
	if(input == "INFO"){
	 	response = handle_info_request(connfd);
	 	send(connfd, response.c_str(), strlen(response.c_str()), 0);
	 	close(connfd);
	 	continue;
	}
	if(input == "LIST")
	{
		cout << "Отправлен запрос на возвращение файлов из текущего каталога" << endl;
		path current_place = current_path();
		cout << current_place << endl;
		string catalog_file_system = scan_file_system(current_place);
		send(connfd, catalog_file_system.c_str(), strlen(catalog_file_system.c_str()), 0);
		close(connfd);
		continue;	
	}
	if(input.substr(0, 2) == "CD"){
	 	string new_path_file = input.substr(3) + "/" + "server";
		cout << "Измнение каталога исполняемого файла: " <<  new_path_file << endl;
		string old_path_file = current_path().string() +"/" + "server";
		cout << "Старый путь исполняемого файла: " << old_path_file << endl;
		cout << "Новый путь исполняемого файла: " << new_path_file << endl;
		try
		{
			rename(old_path_file, new_path_file);
			cout << "Изменение пути исполняемого файла произошло успешно!" << endl;
		}
		catch(exception& ex)
		{
			cerr << "Произошла ошибка перемещения исполняемого файла: " << ex.what() << endl;
			exit(-1);
		}
		input = "Изменение пути исполняемого файла произошла успешно! ";
		
	}
        response = "Response to " + input;

        send(connfd, response.c_str(), strlen(response.c_str()), 0);

        close(connfd);
    }

    close(listenfd);

    return 0;
}

