#pragma once
#include <cstdint>

namespace ob {

using Price    = std::int64_t;   // integer ticks, never floating point
using Quantity = std::int64_t;
using OrderId  = std::uint64_t;

enum class Side : std::uint8_t { Buy, Sell };
enum class OrderType : std::uint8_t { Limit, Market };

struct Order {
    OrderId   id;
    Side      side;
    OrderType type;
    Price     price;     // ignored for Market orders
    Quantity  quantity;
};

struct Fill {
    OrderId  taker_id;   // incoming (aggressive) order
    OrderId  maker_id;   // resting (passive) order
    Price    price;      // always the resting order's price
    Quantity quantity;

    friend bool operator==(const Fill&, const Fill&) = default;
};

} 