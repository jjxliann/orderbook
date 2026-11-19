#pragma once
#include <vector>
#include <optional>
#include "ob/types.hpp"

namespace ob {

class OrderBook {
public:
    // Matches against the book; any unfilled limit remainder rests.
    // Unfilled market remainder is discarded.
    std::vector<Fill> submit(const Order& order);

    // Returns true if the order was resting and is now removed.
    bool cancel(OrderId id);

    std::optional<Price> best_bid() const;
    std::optional<Price> best_ask() const;
    Quantity volume_at(Side side, Price price) const;
};

}  