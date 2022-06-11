#include <stdio.h>
#define CURL_STATICLIB 
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

struct api_data_type {
    size_t size;
    char* data;
};

class FamarkCloudAPI {
    CURL* curl;
    CURLcode res;

private: 
    static size_t read_callback(char* dest, size_t size, size_t nmemb, struct api_data_type* request_ptr) {
        size_t buffer_size = (size * nmemb);

        if (request_ptr->size) {
            /* copy as much as possible from the source to the destination */
            size_t copy_this_much = request_ptr->size;
            if (copy_this_much > buffer_size)
                copy_this_much = buffer_size;
            memcpy(dest, request_ptr->data, copy_this_much);

            request_ptr->data += copy_this_much;
            request_ptr->size -= copy_this_much;
            return copy_this_much; /* we copied this many bytes */
        }

        return 0; /* no more data left to deliver */
    }

    static void copy_to_api_data(char* source, size_t source_index, struct api_data_type* data_ptr, size_t data_index, size_t buffer_size) {
        char* tmp;
        tmp = (char*)realloc(data_ptr->data, data_ptr->size + 1); /* +1 for '\0' */

        if (tmp) {
            data_ptr->data = tmp;
        }
        else {
            if (data_ptr->data) {
                free(data_ptr->data);
            }
            fprintf(stderr, "Failed to allocate memory.\n");
            return;
        }

        memcpy((data_ptr->data + data_index), &source[source_index], buffer_size);
        data_ptr->data[data_ptr->size] = '\0';
    }

    static size_t header_callback(char* head, size_t size, size_t nmemb, struct api_data_type* header_ptr) {
        size_t buffer_size = (size * nmemb);

#ifdef DEBUG
        printf("%.*s\n", buffer_size, head);
#endif

        const char* error_prefix = "ErrorMessage: ";
        size_t len_error_prefix = strlen(error_prefix);
        size_t len_head = strlen(head);
        if (len_head >= len_error_prefix && memcmp(error_prefix, head, len_error_prefix) == 0) {
            size_t index = header_ptr->size;
            header_ptr->size += buffer_size - len_error_prefix;
            copy_to_api_data(head, len_error_prefix, header_ptr, index, buffer_size - len_error_prefix);
        }

        return buffer_size;
    }

    static size_t write_callback(char* source, size_t size, size_t nmemb, struct api_data_type* response_ptr) {
        size_t buffer_size = (size * nmemb);

#ifdef DEBUG
        printf("%.*s\n", buffer_size, source);
#endif

        size_t index = response_ptr->size;
        response_ptr->size += buffer_size;
        copy_to_api_data(source, 0, response_ptr, index, buffer_size);
        return buffer_size;
    }

public:
    //Constructor
    FamarkCloudAPI() {
        /* In windows, this will init the winsock stuff */
        res = curl_global_init(CURL_GLOBAL_DEFAULT);
        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_global_init() failed: %s\n",
                curl_easy_strerror(res));
        }
        /* get a curl handle */
        curl = curl_easy_init();
    }

    char* post_data(const char* url_suffix, char* body, const char* session_id) {
        char url[128] = "https://www.famark.com/host/api.svc/api";
        strcat_s(url, sizeof(url), url_suffix);

        struct api_data_type request;
        request.data = body;
        request.size = strlen(body);

        struct api_data_type response;
        response.size = 0;
        response.data = (char*)malloc(4096); /* reasonable size initial buffer */
        if (response.data == NULL) {
            fprintf(stderr, "Failed to allocate memory.\n");
            return NULL;
        }
        response.data[0] = '\0';

        struct api_data_type error_header;
        error_header.size = 0;
        error_header.data = (char*)malloc(2048); /* reasonable size initial buffer */
        if (error_header.data == NULL) {
            fprintf(stderr, "Failed to allocate memory.\n");
            return NULL;
        }
        error_header.data[0] = '\0';

        //curl operation begin...
        curl_easy_setopt(curl, CURLOPT_URL, url);

        /* Now specify we want to POST data */
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)request.size);

        struct curl_slist* hs = NULL;
        hs = curl_slist_append(hs, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

        if (session_id) {
            struct curl_slist* sid = NULL;
            char sid_header[1024] = "SessionId: ";
            strcat_s(sid_header, sizeof(sid_header), session_id);
            sid = curl_slist_append(sid, sid_header);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, sid);
        }

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &request);

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &error_header);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        if (error_header.size) {
            fprintf(stderr, "ErrorMessage: %s\n", error_header.data);
        }

        free(error_header.data);
        return response.data;
    }

    //Destructor
    ~FamarkCloudAPI() {
        /* always cleanup */
        curl_easy_cleanup(curl);
        //global cleanup
        curl_global_cleanup();
    }
};
