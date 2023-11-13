#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>

#define N_MOVING_AVERAGE 26
#define TP_RATIO 0.02

enum Action {
    LONG,
    SHORT
};

class Order{
public:
    Action dir;
    double entryPrice;
    double volume;
    Order(Action _dir, double _price, double _vol): dir(_dir), entryPrice(_price), volume(_vol) {}
};

class OrderBook{
public:
    double balance;
    void deposit(double money) {balance = money;}
    void sendOrder(Action act, double price, double volume, double& total_lots);
    Order getLastOrder();
    bool hasOrder();
    void checkForClose(double pnl);
    double calculatePnL(double realtimeGap, std::ofstream& outputer, bool ifPrintOrders);
private:
    std::vector<Order> book;
};


class buffer{
public:
    void insert(double price);
    double getMA();
private:
    std::deque<double> buff;
    double sum{0};
};

void buffer::insert(double price) {
    buff.push_back(price);
    this->sum += price;
    while (buff.size() > N_MOVING_AVERAGE) {
        this->sum -= buff.front();
        buff.pop_front();
    }
}

double buffer::getMA() {
    if (buff.size() == 0) return 0;
    else return this->sum / buff.size();
}

void OrderBook::sendOrder(Action act, double price, double volume, double& total_lots) {
    volume = (int)(volume * 100) / 100.0;
    book.emplace_back(act, price, volume);
    total_lots += volume;
}

bool OrderBook::hasOrder() {
    return book.size() != 0;
}

void OrderBook::checkForClose(double pnl) {
    if (pnl >= this->balance * 0.02) {
        this->balance += pnl;
        this->book.clear();
    }
}

Order OrderBook::getLastOrder() {
    int n = book.size();
    return book[n - 1];
}

double OrderBook::calculatePnL(double realtimeGap, std::ofstream& outputer, bool ifPrintOrders = false) {
    double pnl = 0;
    if (ifPrintOrders) {
        outputer << "Current Gap is " << realtimeGap << std::endl;
    }
    double total_lots = 0;
    for (const auto& order: this->book) {
        if (order.dir == Action::LONG) {
            pnl += (realtimeGap - order.entryPrice) * 100000 * order.volume;
        } else {
            pnl += (order.entryPrice - realtimeGap) * 100000 * order.volume;
        }
        total_lots += order.volume;
        if (ifPrintOrders) {
            outputer << "Order Direction: " << (order.dir == Action::LONG ? "LONG" : "SHORT") << '\n' 
                    << "Entry Price: $" << order.entryPrice << '\n'
                    << "Volume: " << order.volume << '\n';
        }
    }
    if (ifPrintOrders) {
        outputer << "Current Lots: " << total_lots << std::endl;
        outputer << "Current Balance: " << this->balance  << std::endl;
        outputer << "Current Equity: " << this->balance + pnl << std::endl;
        outputer << "#############################################################\n";
    }
    return pnl;
}
