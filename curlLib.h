#include <iostream>
#include <memory>
#include <functional>
#include <exception>
#include <array>
#include <cstdio>
#include <string>

#include "curl/curl.h"
#include "json.hpp"

typedef std::unique_ptr<CURL, std::function<void(CURL*)>> CURL_ptr;

extern "C" std::size_t dataHandler(const char* buffer, 
    std::size_t size, std::size_t nmemb, std::string* userData) {

      if (userData == nullptr) {
        return 0;
      }

      userData->append(buffer, (size * nmemb));
      return size * nmemb;

}

class CurlException : public std::exception {
  private :
    int curlCode;
    std::string w;
  public :
    CurlException(int curlCode, const std::string& w) : curlCode(curlCode), w(w) {

    }

    int getCode() const noexcept {
      return curlCode;
    }

    const char* what() const noexcept {
      return w.c_str();
    }
};

class CurlHandle {
  private :
    CURL_ptr curlptr;
    std::string data;
    std::array<char, CURL_ERROR_SIZE> errorBuffer;
    constexpr static auto f = [](CURL* c) {
      curl_easy_cleanup(c);
      curl_global_cleanup();
    };

  public :
    CurlHandle() : curlptr(curl_easy_init(), f) {
      CURLcode code = CURLE_OK;
     
      code = curl_global_init(CURL_GLOBAL_ALL);
      if (code != CURLE_OK){
        throw CurlException(static_cast<int>(code), "Unable to global init");
      }

      curl_easy_setopt(curlptr.get(), CURLOPT_ERRORBUFFER, &errorBuffer[0]);
      if (code != CURLE_OK){
        throw CurlException(static_cast<int>(code), "Unable to set error buffer");
      }
      curl_easy_setopt(curlptr.get(), CURLOPT_WRITEFUNCTION, dataHandler);
      if (code != CURLE_OK){
        throw CurlException(static_cast<int>(code), std::string(&errorBuffer[0]));
      }

      curl_easy_setopt(curlptr.get(), CURLOPT_WRITEDATA, &data);
      if (code != CURLE_OK){
        throw CurlException(static_cast<int>(code), std::string(&errorBuffer[0]));
      }
    }
  
    void setUrl(const std::string& url) {
      CURLcode code = curl_easy_setopt(curlptr.get(), CURLOPT_URL, url.c_str());
      if (code != CURLE_OK){
        throw CurlException(static_cast<int>(code), std::string(&errorBuffer[0]));
      }

    }

    void fetch() {
      data.empty();
      CURLcode code = curl_easy_perform(curlptr.get());
      if (code != CURLE_OK){
        throw CurlException(static_cast<int>(code), std::string(&errorBuffer[0]));
      }
    }

    const std::string& getFetchedData() const {
      return data;
    }
};