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
std::string getSiteData(std::string site)
{
    std::string buffer;
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, site.append(".json").c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_get);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return buffer;
}

void setSiteData(std::string site, std::string data)
{
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, site.append(".json").c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_put);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}
