// fetch stock quotes over http, json, and savit it
//libaries used, libcurl, json

#include <iostream> //print input and output
#include <string> // lets us store strings (type std::)
#include <algorithm> //tools such as std transform
#include <fstream> // allows us to save a text to a certain file
#include <curl/curl.h> // cURL feth data from the internet
#include <nlohmann/json.hpp> //Json

//simplify nlohman
using json = nlohmann::json;





//setting up a call back for lib curl
//curl downloads data in chunks in order to capture it in strings
// callback function
static size_t writeToString(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalBytes = size * nmemb; //total size of chunk
    auto* out = static_cast<std::string*>(userp); //cassts the data back to a string
    out->append(static_cast<char*>(contents), totalBytes); //appends the chunk
    return totalBytes; //telling curl we handled data
}

//creating API URL based on the stock symbol and API key
static std::string build_url(const std::string& symbol, const std::string& api_key) {
    std::string url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=";
    url += symbol; //stock symbols
    url+= "&apikey=";
    url += api_key; //api key
    return url;
}

//print a friendly summary from JSON
void print_summary(const json& j, const std::string& symbol) {
    if (!j.contains("Global Quote")) {
        std::cout << "Unexpected JSON format:\n" << j.dump(4) << "\n";
        return;
    }

    const auto& q = j.at("Global Quote");

    auto get = [&](const char* k) -> std::string {
        return q.contains(k) ? q.at(k).get<std::string>() : "";
    };

    std::string price   = get("05. price");
    std::string change  = get("09. change");
    std::string percent = get("10. change percent");
    std::string day     = get("07. latest trading day");

    std::cout << "=== " << symbol << " Quote ===\n";
    if (!price.empty())   std::cout << "Price: " << price << "\n";
    if (!change.empty())  std::cout << "Change: " << change << " (" << percent << ")\n";
    if (!day.empty())     std::cout << "As of: " << day << "\n";
}

int main() {
    // Step 1: Ask user for a stock symbol
    std::string symbol;
    std::cout << "Enter stock symbol (e.g., AAPL): ";
    std::cin >> symbol;
    std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);

    // Step 2: Directly assign API key (for testing)
    std::string api_key = "input your key here"; // <-- put your actual key here and leave the quotes (do not share API key with anyone)

    // Step 3: Build the full URL
    std::string url = build_url(symbol, api_key);

    // Step 4: Initialize cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    std::string readBuffer; // stores downloaded data

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());          // set URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString); // set callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);   // pointer to string
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);       // follow redirects
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);             // 20-second timeout

        CURLcode res = curl_easy_perform(curl);                   // perform request
        if (res != CURLE_OK) {
            std::cerr << "cURL failed: " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }

        curl_easy_cleanup(curl);   // cleanup handle
    }
    curl_global_cleanup();         // cleanup global resources

    // Step 5: Parse JSON
    json j = json::parse(readBuffer, nullptr, false);
    if (j.is_discarded()) {
        std::cerr << "Failed to parse JSON.\n";
        return 1;
    }

    // Step 6: Print summary
    print_summary(j, symbol);

    // Step 7: Save JSON to file
    std::ofstream outFile(symbol + "_quote.json");
    outFile << j.dump(4); // pretty print with 4-space indentation
    outFile.close();

    std::cout << "Saved full JSON to " << symbol + "_quote.json" << "\n";
    return 0;
}
