#include<iostream>
#include<string>
#include<vector>
#include<curl/curl.h>
#include<dirent.h>
#include<string.h>

namespace io
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

    std::string getSiteData(std::string site)
    {
        std::string buffer;
        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, site.append(auth).c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_get);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 200L);

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
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 200L);

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

    int operateFile(std::string path, std::string buffer, std::string mode)
    {
        FILE * fp = fopen(path.c_str(), mode.c_str());
        if(!fp) return 1;
        fprintf(fp, "%s\n", buffer.c_str());
        fclose(fp);
        return 0;
    }

    int writeFile(std::string path, std::string buffer)
    {
        return operateFile(path, buffer, "w");
    }

    int appendFile(std::string path, std::string buffer)
    {
        return operateFile(path, buffer, "a");
    }

    std::string readFile(std::string path)
    {
        FILE * fp = fopen(path.c_str(), "r");
        if(!fp) return "";
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char* string = new char[fsize + 1];
        fread(string, fsize, 1, fp);
        std::string buffer = string;
        delete[] string;
        fclose(fp);
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

    void mkdir(std::string directory)
    {
        system(("mkdir -p " + directory).c_str());
    }

    void rmdir(std::string directory)
    {
        system(("rm -rf " + directory).c_str());
    }

    void rmfile(std::string file)
    {
        system(("rm -f " + file).c_str());
    }
}
