#include "curlLib.h"
#include "json.hpp"

class Bitcoin {
  using json = nlohmann::json;

  private :
    CurlHandle curlHandle;
    static constexpr const char* API_URL = "https://blockchain.info/ticker";

  public :
    Bitcoin() : curlHandle({}) {
      curlHandle.setUrl(API_URL);
    }

    json fetchBitcoinData() {
      curlHandle.fetch();
      return json::parse(curlHandle.getFetchedData());
    }
};