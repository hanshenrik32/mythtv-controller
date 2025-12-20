#include <iostream>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>


// used to download movie covers from TMDB

class TMDB {
public:
    TMDB(const std::string& apiKey): apiKey(apiKey) {}

// Søg film og returner poster_path (tom hvis ikke fundet)
std::string getPosterPath(const std::string& title) {
    std::string url =
        "https://api.themoviedb.org/3/search/movie?api_key=" +
        apiKey + "&query=" + urlEncode(title);

    std::string response;
    if (!httpGet(url, response))
        return "";

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    std::istringstream ss(response);
    if (!Json::parseFromStream(builder, ss, &root, &errs))
        return "";

    if (!root["results"].isArray() || root["results"].empty())
        return "";

    return root["results"][0]["poster_path"].asString();
}

    // Download cover i ønsket størrelse (w200, w500, original)
    bool downloadPoster(const std::string& posterPath,
                        const std::string& outFile,
                        const std::string& size = "w500")
    {
        if (posterPath.empty())
            return false;

        std::string url =
            "https://image.tmdb.org/t/p/" + size + posterPath;

        return downloadFile(url, outFile);
    }

private:
    std::string apiKey;

    static size_t writeString(void* ptr, size_t size, size_t nmemb, void* data)
    {
        std::string* str = (std::string*)data;
        str->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

    static size_t writeFile(void* ptr, size_t size, size_t nmemb, void* data)
    {
        std::ofstream* file = (std::ofstream*)data;
        file->write((char*)ptr, size * nmemb);
        return size * nmemb;
    }

    bool httpGet(const std::string& url, std::string& out)
    {
        CURL* curl = curl_easy_init();
        if (!curl)
            return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "tmdb-cpp");

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return res == CURLE_OK;
    }

    bool downloadFile(const std::string& url, const std::string& path)
    {
        CURL* curl = curl_easy_init();
        if (!curl)
            return false;

        std::ofstream file(path, std::ios::binary);
        if (!file)
            return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "tmdb-cpp");

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        file.close();

        return res == CURLE_OK;
    }

    std::string urlEncode(const std::string& s)
    {
        CURL* curl = curl_easy_init();
        char* out = curl_easy_escape(curl, s.c_str(), s.length());
        std::string encoded(out);
        curl_free(out);
        curl_easy_cleanup(curl);
        return encoded;
    }
};



/*

int main()
{
    TMDB tmdb("DIN_TMDB_API_KEY");

    std::string movie = "Inception";
    std::string poster = tmdb.getPosterPath(movie);

    if (poster.empty())
    {
        std::cerr << "Ingen cover fundet\n";
        return 1;
    }

    if (tmdb.downloadPoster(poster, "inception.jpg"))
        std::cout << "Cover gemt som inception.jpg\n";
    else
        std::cerr << "Download fejlede\n";

    return 0;

*/