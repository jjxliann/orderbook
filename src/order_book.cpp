#include "ob/order_book.hpp"

namespace ob {

std::vector<Fill> OrderBook::submit(const Order&) { return {}; }
bool OrderBook::cancel(OrderId) { return false; }
std::optional<Price> OrderBook::best_bid() const { return std::nullopt; }
std::optional<Price> OrderBook::best_ask() const { return std::nullopt; }
Quantity OrderBook::volume_at(Side, Price) const { return 0; }

} 