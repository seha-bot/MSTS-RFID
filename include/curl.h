#include<string>
#include<curl/curl.h>

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
