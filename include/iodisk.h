#include<iostream>
#include<string>
#include<vector>
#include<curl/curl.h>
#include<dirent.h>
#include<string.h>

namespace io
{
    namespace
    {
        size_t write_get(void *contents, size_t size, size_t nmemb, void *userp)
        {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }
        size_t write_put(void *buffer, size_t size, size_t nmemb, void *userp)
        {
            return size * nmemb;
        }

        CURL *curl;
        const char* auth = ".json?auth=VH0KFbDUHbBlceeXYuxtktsloiaQnp69FKQjnbAO";
    }
    
    std::string getSiteData(std::string site)
    {
        std::string buffer;
        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, site.append(auth).c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_get);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            // curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            CURLcode curl_code = curl_easy_perform(curl);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_cleanup(curl);
            if(http_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK)
            {
                return buffer;
            }
            else return "";
        }
        return "";
    }

    int setSiteData(std::string site, std::string data)
    {
        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, site.append(auth).c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_put);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            CURLcode curl_code = curl_easy_perform(curl);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_cleanup(curl);
            if(http_code == 200 && curl_code != CURLE_ABORTED_BY_CALLBACK)
            {
                return 0;
            }
            else return 1;
        }
        return 1;
    }

    std::string readFile(std::string path)
    {
        FILE * db = fopen(path.c_str(), "r");
        if(!db) return "";
        fseek(db, 0, SEEK_END);
        long fsize = ftell(db);
        fseek(db, 0, SEEK_SET);
        char* string = new char[fsize + 1];
        fread(string, fsize, 1, db);
        std::string buffer = string;
        delete[] string;
        fclose(db);
        return buffer;
    }

    std::vector<std::string> readDir(std::string path)
    {
        DIR * dir = opendir(path.c_str());
        if(!dir) return std::vector<std::string>();
        struct dirent * f;
        std::vector<std::string> files;
        while(f = readdir(dir)) if(strcmp(f->d_name, ".") != 0 && strcmp(f->d_name, "..") != 0) files.push_back(f->d_name);
        closedir(dir);
        return files;
    }
}
