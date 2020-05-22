#include "bitcoin.h"

int main() {
  using namespace std;
  using json = nlohmann::json;

  try {
    Bitcoin bitcoin;
    json bitcoinData = bitcoin.fetchBitcoinData();

    cout << "1 BTC = \n";
    for (json::iterator it = bitcoinData.begin(); it != bitcoinData.end(); ++it) {
      printf("\t(%3s)%10d %s\n", it.key().c_str(), it.value()["last"].get<int>(), 
          it.value()["symbol"].get<string>().c_str());
    }

  } catch (const std::exception& e) {
    cerr << "Failed to fetch bitcoin exchange rates\n";
    cerr << "Reason: " << e.what() << "\n";
  } catch (...) {
    cerr << "Caught unknown exception\n";
  }
}