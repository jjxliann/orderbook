#include <gtest/gtest.h>
#include "ob/order_book.hpp"

using namespace ob;

namespace {
Order limit(OrderId id, Side s, Price p, Quantity q) {
    return {id, s, OrderType::Limit, p, q};
}
Order market(OrderId id, Side s, Quantity q) {
    return {id, s, OrderType::Market, 0, q};
}
}  // namespace

TEST(OrderBook, LimitRestsOnEmptyBook) {
    OrderBook book;
    EXPECT_TRUE(book.submit(limit(1, Side::Buy, 10, 100)).empty());
    EXPECT_EQ(book.best_bid(), 10);
    EXPECT_EQ(book.volume_at(Side::Buy, 10), 100);
    EXPECT_FALSE(book.best_ask().has_value());
}

TEST(OrderBook, ExactMatchClearsBook) {
    OrderBook book;
    book.submit(limit(1, Side::Buy, 10, 100));
    auto fills = book.submit(limit(2, Side::Sell, 10, 100));
    ASSERT_EQ(fills.size(), 1u);
    EXPECT_EQ(fills[0], (Fill{2, 1, 10, 100}));
    EXPECT_FALSE(book.best_bid().has_value());
    EXPECT_FALSE(book.best_ask().has_value());
}

TEST(OrderBook, TimePriorityWithinLevelAndPartialFill) {
    // buy 100 @10, buy 50 @10, sell 120 @10
    OrderBook book;
    book.submit(limit(1, Side::Buy, 10, 100));
    book.submit(limit(2, Side::Buy, 10, 50));
    auto fills = book.submit(limit(3, Side::Sell, 10, 120));
    ASSERT_EQ(fills.size(), 2u);
    EXPECT_EQ(fills[0], (Fill{3, 1, 10, 100}));  // earliest order first
    EXPECT_EQ(fills[1], (Fill{3, 2, 10, 20}));   // partial fill
    EXPECT_EQ(book.volume_at(Side::Buy, 10), 30);
}

TEST(OrderBook, TradePrintsAtRestingPrice) {
    // then sell 100 @9 against remaining buy 30 @10
    OrderBook book;
    book.submit(limit(1, Side::Buy, 10, 30));
    auto fills = book.submit(limit(2, Side::Sell, 9, 100));
    ASSERT_EQ(fills.size(), 1u);
    EXPECT_EQ(fills[0], (Fill{2, 1, 10, 30}));   // price 10, not 9
    EXPECT_EQ(book.best_ask(), 9);               // remainder rests
    EXPECT_EQ(book.volume_at(Side::Sell, 9), 70);
}

TEST(OrderBook, SweepsMultipleLevelsBestPriceFirst) {
    OrderBook book;
    book.submit(limit(1, Side::Sell, 12, 10));
    book.submit(limit(2, Side::Sell, 11, 10));
    book.submit(limit(3, Side::Sell, 13, 10));
    auto fills = book.submit(limit(4, Side::Buy, 13, 25));
    ASSERT_EQ(fills.size(), 3u);
    EXPECT_EQ(fills[0], (Fill{4, 2, 11, 10}));
    EXPECT_EQ(fills[1], (Fill{4, 1, 12, 10}));
    EXPECT_EQ(fills[2], (Fill{4, 3, 13, 5}));
    EXPECT_EQ(book.volume_at(Side::Sell, 13), 5);
}

TEST(OrderBook, MarketOrderOnEmptyBookIsDiscarded) {
    OrderBook book;
    EXPECT_TRUE(book.submit(market(1, Side::Buy, 50)).empty());
    EXPECT_FALSE(book.best_bid().has_value());
}

TEST(OrderBook, MarketOrderPartialFillDiscardsRemainder) {
    OrderBook book;
    book.submit(limit(1, Side::Sell, 10, 30));
    auto fills = book.submit(market(2, Side::Buy, 50));
    ASSERT_EQ(fills.size(), 1u);
    EXPECT_EQ(fills[0], (Fill{2, 1, 10, 30}));
    EXPECT_FALSE(book.best_bid().has_value());   // remainder not rested
}

TEST(OrderBook, CancelRemovesRestingOrder) {
    OrderBook book;
    book.submit(limit(1, Side::Buy, 10, 100));
    EXPECT_TRUE(book.cancel(1));
    EXPECT_FALSE(book.best_bid().has_value());
}

TEST(OrderBook, CancelOfFilledOrUnknownOrderReturnsFalse) {
    OrderBook book;
    book.submit(limit(1, Side::Buy, 10, 100));
    book.submit(limit(2, Side::Sell, 10, 100));   // fills order 1
    EXPECT_FALSE(book.cancel(1));
    EXPECT_FALSE(book.cancel(999));
}

TEST(OrderBook, NonCrossingOrdersBothRest) {
    OrderBook book;
    book.submit(limit(1, Side::Buy, 9, 10));
    book.submit(limit(2, Side::Sell, 11, 10));
    EXPECT_EQ(book.best_bid(), 9);
    EXPECT_EQ(book.best_ask(), 11);
}