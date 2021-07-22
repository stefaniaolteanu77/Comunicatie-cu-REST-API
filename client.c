#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[]) {
    
    int sockfd;
    char buffer[BUFLEN];
    char username[100], password[100];

    char *message, *response, *cookie, *jwt_token;

    char title[100], author[100];
    char genre[100], publisher[100];
    int page_count;
    int id;

    
    while (1) {

        memset(buffer, 0, BUFLEN);
        fgets(buffer, BUFLEN, stdin);

        if (strcmp(buffer, "register\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            serialized_string = json_serialize_to_string_pretty(root_value);

            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/register", "application/json", 
                                            serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            print_error_message(response);
                
            
        }

        if (strcmp(buffer, "login\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            char *serialized_string = json_serialize_to_string_pretty(root_value);

            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/login", "application/json",
                                            serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char* copy_response = strdup(response);
            
            cookie = get_cookie(response);

            print_error_message(copy_response);
        }

        if (strcmp(buffer, "enter_library\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            memset(message, 0, BUFLEN);
            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access", NULL, cookie, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            JSON_Value *root_value = json_value_init_string(response);
            const char *json_response = json_value_get_string(root_value);

            char *copy_response;
            copy_response = strdup(json_response);
            jwt_token = get_jwt_token(copy_response);
            print_error_message(response);
            free(copy_response);
        }

        if (strcmp(buffer, "get_books\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books",NULL, cookie, jwt_token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            print_booklist(response);
        }

        if (strcmp(buffer, "get_book\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            printf("id=");
            scanf("%d", &id);
            char route[200] = "/api/v1/tema/library/books/";
            char id_numb[10];
            sprintf(id_numb, "%d", id);
            strcat(route, id_numb);
            message = compute_get_request("34.118.48.238", route, NULL, cookie, jwt_token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            print_error_message(response);
        }

        if (strcmp(buffer, "add_book\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            printf("title=");
            fgets(title, sizeof(title), stdin);
            title[strcspn(title,"\n")] = 0;
            printf("author=");
            fgets(author, sizeof(author), stdin);
            author[strcspn(author,"\n")] = 0;
            printf("genre=");
            fgets(genre, sizeof(genre), stdin);
            genre[strcspn(genre,"\n")] = 0;
            printf("page_count=");
            scanf("%d", &page_count);
            printf("publisher=");
            fgets(publisher, sizeof(publisher), stdin);
            publisher[strcspn(publisher,"\n")] = 0;
           

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            
            json_object_set_string(root_object, "title", title);  
            json_object_set_string(root_object, "author", author);
            json_object_set_string(root_object, "genre", genre);
            json_object_set_number(root_object, "page_count", page_count);
            json_object_set_string(root_object, "publisher", publisher);

            char* string = json_serialize_to_string_pretty(root_value);

            message = compute_post_request("34.118.48.238", "/api/v1/tema/library/books", "application/json",
                                            string, cookie, jwt_token);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            
            print_error_message(response);
        }

        if (strcmp(buffer, "delete_book\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            printf("id=");
            scanf("%d", &id);
            char route[200] = "/api/v1/tema/library/books/";
            char id_numb[10];
            sprintf(id_numb, "%d", id);
            strcat(route, id_numb);
            message = compute_delete_request("34.118.48.238", route, NULL, cookie, jwt_token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            print_error_message(response);
        }   

        if (strcmp(buffer, "logout\n") == 0) {
            sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout", NULL, cookie, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            print_error_message(response);
        }  

        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
    }

    if (cookie != NULL) {
        free(cookie);
    }
    if (jwt_token != NULL) {
        free(jwt_token);
    }
    
    close_connection(sockfd);
    return 0;
}
